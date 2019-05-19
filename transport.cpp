// Maksymilian Zawartko
// 289528

#include "transport.h"


bool IsIp(string argument) {
    int dots_count = 0;

    for (char character : argument) {
        if (character == '.') {
            dots_count++;

            if (dots_count > 3)
                return false;
        }
        else if (character < '0' || '9' < character)
            return false;
        // else character is number and I assume there is good number of numbers in each IP segment
    }

    return true;
}

bool IsNumber(string argument) {
    for (char character : argument)
        if (character < '0' || '9' < character)
            return false;

    return true;
}

void AssureCorrectArguments(int argc, char *argv[]) {
    if (argc != 5 ||
            !IsIp(argv[1]) ||
            !IsNumber(argv[2]) ||       // is port
            // I'm not checking if the filename is correct
            !IsNumber(argv[4])) {       // is size
        cout << "Wrong arguments\n";
        exit(1);
    }
}

FileDescriptor CreateFileDescriptor(string filename) {
    FileDescriptor file = open(filename.c_str(), O_APPEND | O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);

    if (file == -1) {
        cout << "Error opening file\n";
        exit(1);
    }
    else
        return file;
}

void SaveDataToFile(FileDescriptor file, SegmentData *data) {
    int bytes_written = write(file, data->GetData(), data->GetSize());

    if (bytes_written == -1) {
        cout << "Error writing to file\n";
        exit(1);
    }
}

struct sockaddr_in *CreateRecipient(string ip, int port) {
    struct sockaddr_in *result = new sockaddr_in;
    bzero(result, sizeof(*result));
    result->sin_family = AF_INET;
    result->sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &result->sin_addr);
    return result;
}

void AcquireFile(string filename, int size, SocketDescriptor socket, struct sockaddr_in *server) {
    FileDescriptor file = CreateFileDescriptor(filename);
    SlidingWindow sliding_window(size, socket, server);
    int segments_acquired = 0, last_frame_received = sliding_window.GetLastFrameReceived(), last_frame = sliding_window.GetLastFrame();

    while (last_frame_received < last_frame) {
        sliding_window.Populate();
        sliding_window.SelectData();

        SegmentData *data = sliding_window.GetData();
        while (data != nullptr) {
            SaveDataToFile(file, data);
            sliding_window.RemoveFirstSegmentData();
            data = sliding_window.GetData();
        }
        
        last_frame_received = sliding_window.GetLastFrameReceived();
        if (segments_acquired != last_frame_received) {
            segments_acquired = last_frame_received;
            cout << "Acquired " << segments_acquired << '/' << last_frame << " segments.\n";
        }
    }
}

int main(int argc, char *argv[]) {
    AssureCorrectArguments(argc, argv);

    string ip = argv[1];
    int port = stoi(argv[2]);
    string filename = argv[3];
    int size = stoi(argv[4]);

    SocketDescriptor socket = CreateSocket();
    BindSocket(socket, ip, port);


    AcquireFile(filename, size, socket, CreateRecipient(ip, port));

    return 0;
}