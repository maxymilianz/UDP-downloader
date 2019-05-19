#ifndef SEGMENT_DATA
#define SEGMENT_DATA

#include <cstdint>

using namespace std;


class SegmentData {
    private: int offset, size;
    private: uint8_t *data;

    public: SegmentData(int offset, int size, uint8_t *data);
    public: ~SegmentData() = default;
    
    private: void SaveData(uint8_t *data);

    public: int GetOffset();
    public: int GetSize();
    public: uint8_t *GetData();
};

#endif