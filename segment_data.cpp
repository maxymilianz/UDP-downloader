// Maksymilian Zawartko
// 289528

#include "segment_data.h"


SegmentData::SegmentData(int offset, int size, uint8_t *data) {
    this->offset = offset;
    this->size = size;
    this->data = new uint8_t[size];
    SaveData(data);
}

void SegmentData::SaveData(uint8_t *data) {
    for (int i = 0; i < size; i++)
        this->data[i] = data[i];
}

int SegmentData::GetOffset() {
    return offset;
}

int SegmentData::GetSize() {
    return size;
}

uint8_t *SegmentData::GetData() {
    return data;
}