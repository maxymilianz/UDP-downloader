#ifndef SEGMENT_REQUEST
#define SEGMENT_REQUEST

#include <chrono>
#include <string>

using namespace std::chrono;


class SegmentRequest {
    private: microseconds send_time;
    private: int offset, segment_size;

    public: SegmentRequest(int offset, int segment_size);

    public: void UpdateSendTime();

    public: std::string ToString();

    public: microseconds GetSendTime();
    public: int GetOffset();
    public: int GetSegmentSize();
};

#endif