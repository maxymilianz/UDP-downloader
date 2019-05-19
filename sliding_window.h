#ifndef SLIDING_WINDOW
#define SLIDING_WINDOW

#include "constants.h"
#include "segment_data.h"
#include "segment_request.h"
#include "socket_tools.h"

#include <cmath>
#include <errno.h>
#include <list>
#include <netinet/ip.h>
#include <cstring>

using namespace std;


class SlidingWindow {
    private: static const int max_window_size = 2500;       // 2500 - ~40 s on ~9MB

    private: int last_frame, last_frame_received = 0;
    private: int total_file_size;

    private: SocketDescriptor socket;
    private: struct sockaddr_in *server;

    private: list<SegmentData> buffer;
    private: list<SegmentRequest> window;

    public: SlidingWindow(int total_file_size, SocketDescriptor socket, struct sockaddr_in *server);

    public: void Populate();

    private: struct timeval GetRemainingTime(SegmentRequest *request);
    
    public: void SelectData();

    private: void ProcessData(uint8_t *data);

    private: void AddSegmentData(SegmentData *data);

    private: void RemoveSegmentRequest(int offset, int segment_size);

    public: void RemoveFirstSegmentData();

    public: SegmentData *GetData();

    private: int NextFrameNumber();

    private: SegmentRequest *NextSegmentRequest();

    private: void SendSegmentRequest(SegmentRequest *request);

    private: int FrameNumberToOffset(int number);
    private: int OffsetToFrameNumber(int offset);

    public: int GetLastFrame();
    public: int GetLastFrameReceived();
};

#endif