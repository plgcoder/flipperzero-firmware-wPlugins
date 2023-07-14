#pragma once

#define EM4100_DECODED_DATA_SIZE (5)

typedef uint64_t EM4100DecodedData;
struct ProtocolDict {
    const ProtocolBase** base;
    size_t count;
    void** data;
};

typedef struct {
    uint8_t data[EM4100_DECODED_DATA_SIZE];
    
    EM4100DecodedData encoded_data;
    uint8_t encoded_data_index;
    bool encoded_polarity;
    ManchesterState decoder_manchester_state;
} ProtocolEM4100;
