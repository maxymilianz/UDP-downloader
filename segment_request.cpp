// Maksymilian Zawartko
// 289528

#include "segment_request.h"


SegmentRequest::SegmentRequest(int offset, int segment_size) {
    this->offset = offset;
    this->segment_size = segment_size;
    UpdateSendTime();
}

void SegmentRequest::UpdateSendTime() {
    send_time = duration_cast<microseconds>(system_clock::now().time_since_epoch());
}

std::string SegmentRequest::ToString() {
    return "GET " + std::to_string(offset) + ' ' + std::to_string(segment_size) + '\n';
}

microseconds SegmentRequest::GetSendTime() {
    return send_time;
}

int SegmentRequest::GetOffset() {
    return offset;
}

int SegmentRequest::GetSegmentSize() {
    return segment_size;
}