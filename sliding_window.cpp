// Maksymilian Zawartko
// 289528

#include "sliding_window.h"


SlidingWindow::SlidingWindow(int total_file_size, SocketDescriptor socket, struct sockaddr_in *server) {
    last_frame = ceil((double) total_file_size / (double) max_segment_size);
    this->total_file_size = total_file_size;
    this->socket = socket;
    this->server = server;
}

void SlidingWindow::Populate() {
    while (((window.empty() && last_frame_received < last_frame) ||
            (OffsetToFrameNumber(window.back().GetOffset()) < last_frame_received + max_window_size &&
             OffsetToFrameNumber(window.back().GetOffset()) < last_frame))
           && (buffer.empty() || OffsetToFrameNumber(buffer.back().GetOffset()) < last_frame)) {
        SegmentRequest *request = NextSegmentRequest();
        window.push_back(*request);
        SendSegmentRequest(request);
    }
}

struct timeval SlidingWindow::GetRemainingTime(SegmentRequest *request) {
    microseconds current_time = duration_cast<microseconds>(system_clock::now().time_since_epoch());
    long wait_microseconds = wait_milliseconds * 1000 - (current_time - request->GetSendTime()).count();

    struct timeval wait_time;
    wait_time.tv_sec = wait_microseconds / 1000000;
    wait_time.tv_usec = wait_microseconds % 1000000;
    return wait_time;
}

void SlidingWindow::SelectData() {
    auto oldest_request = window.begin();

    if (oldest_request != window.end()) {
        for (auto iterator = window.begin(); iterator != window.end(); iterator++)
            if (iterator->GetSendTime() < oldest_request->GetSendTime())
                oldest_request = iterator;

        struct timeval wait_time = GetRemainingTime(&*oldest_request);

        fd_set descriptors;
        FD_ZERO(&descriptors);
        FD_SET(socket, &descriptors);

        int ready = wait_time.tv_sec < 0 || wait_time.tv_usec < 0 ? 0 : select(socket + 1, &descriptors, NULL, NULL, &wait_time);

        if (ready < 0) {
            cout << strerror(errno) << " error on select\n";
            exit(1);
        }
        else if (ready == 0) {
            oldest_request->UpdateSendTime();
            SendSegmentRequest(&*oldest_request);
        }
        else {      // ready > 0
            struct sockaddr_in source;
            socklen_t source_size = sizeof(source);
            uint8_t *buffer = new uint8_t[IP_MAXPACKET];
            ssize_t packet_size = recvfrom(socket, buffer, IP_MAXPACKET, 0,
                                            (struct sockaddr *) &source, &source_size);

            if (packet_size <= 0) {
                cout << "Error on recvfrom\n";
            }
            else {      // packet_size > 0
                char expected_ip[max_ip_length];
                inet_ntop(AF_INET, &(server->sin_addr), expected_ip, max_ip_length);

                char source_ip[max_ip_length];
                inet_ntop(AF_INET, &(source.sin_addr), source_ip, max_ip_length);

                if (strcmp(expected_ip, source_ip) == 0)
                    ProcessData(buffer);
                // else ignore packet from unknown ip
            }

            delete buffer;
        }
    }
}

void SlidingWindow::ProcessData(uint8_t *data) {
    int i = 5;
    string offset_string;

    while ('0' <= data[i] && data[i] <= '9') {
        offset_string += data[i];
        i++;
    }

    int offset = stoi(offset_string);

    if (last_frame_received < OffsetToFrameNumber(offset)) {
        i++;
        string segment_size_string;

        while ('0' <= data[i] && data[i] <= '9') {
            segment_size_string += data[i];
            i++;
        }

        int segment_size = stoi(segment_size_string);

        RemoveSegmentRequest(offset, segment_size);

        i++;

        SegmentData segment_data(offset, segment_size, data + i);
        AddSegmentData(&segment_data);
    }
}

void SlidingWindow::AddSegmentData(SegmentData *data) {
    auto iterator = buffer.begin();

    while (iterator != buffer.end() && iterator->GetOffset() < data->GetOffset())
        iterator++;

    if (data->GetOffset() < iterator->GetOffset())
        buffer.insert(iterator, *data);
    else if (iterator == buffer.end() || data->GetOffset() != iterator->GetOffset())        // iterator should be inserted at end
        buffer.push_back(*data);
}

void SlidingWindow::RemoveSegmentRequest(int offset, int segment_size) {
    for (auto iterator = window.begin(); iterator != window.end(); iterator++)
        if (iterator->GetOffset() == offset && iterator->GetSegmentSize() == segment_size) {
            window.erase(iterator);
            return;
        }
}

void SlidingWindow::RemoveFirstSegmentData() {
    buffer.erase(buffer.begin());
}

SegmentData *SlidingWindow::GetData() {
    if (buffer.empty())
        return nullptr;
    else {
        SegmentData *first = &buffer.front();

        if (OffsetToFrameNumber(first->GetOffset()) == last_frame_received + 1) {
            last_frame_received++;
            return first;
        }
        else        // first data is not the oldest requested
            return nullptr;
    }
}

int SlidingWindow::NextFrameNumber() {
    if (window.empty()) {
        if (buffer.empty())
            return last_frame_received + 1;
        else if (buffer.size() == 1)
            return OffsetToFrameNumber(buffer.back().GetOffset()) + 1;
        else {
            int frame_number = last_frame_received;

            for (SegmentData segment_data : buffer) {
                if (frame_number + 1 != OffsetToFrameNumber(segment_data.GetOffset()))
                    return frame_number + 1;
                else
                    frame_number = OffsetToFrameNumber(segment_data.GetOffset());
            }
        
            return frame_number + 1;
        }
    }
    else {
        int frame_number = OffsetToFrameNumber(window.back().GetOffset()) + 1;
        
        if (buffer.empty())
            return frame_number;
        else {
            for (SegmentData segment_data : buffer) {
                if (frame_number == OffsetToFrameNumber(segment_data.GetOffset()))
                    frame_number++;
                else if (frame_number < OffsetToFrameNumber(segment_data.GetOffset()))
                    return frame_number;
            }

            return frame_number;
        }
    }
}

SegmentRequest *SlidingWindow::NextSegmentRequest() {
    int frame_number = NextFrameNumber();
    SegmentRequest *segment_request = new SegmentRequest(FrameNumberToOffset(frame_number),
                                                         frame_number == last_frame ? total_file_size % max_segment_size : max_segment_size);
    return segment_request;
}

void SlidingWindow::SendSegmentRequest(SegmentRequest *request) {
    string request_string = request->ToString();
    uint8_t *data = (uint8_t *) request_string.c_str();
    ssize_t bytes_sent = sendto(socket, data, request_string.length(), 0,
                                (struct sockaddr *) server, sizeof(*server));

    if (bytes_sent == -1) {
        cout << "Error on sendto\n";
        exit(1);
    }
}

int SlidingWindow::FrameNumberToOffset(int number) {
    return (number - 1) * max_segment_size;
}

int SlidingWindow::OffsetToFrameNumber(int offset) {
    return ceil((double) offset / (double) max_segment_size) + 1;
}

int SlidingWindow::GetLastFrame() {
    return last_frame;
}

int SlidingWindow::GetLastFrameReceived() {
    return last_frame_received;
}