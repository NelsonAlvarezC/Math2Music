// Author: Nelson Alvarez

#pragma once
#include <stdlib.h>
#include <stdio.h>

typedef struct __attribute__((__packed__)) wave_header{

    // *********** RIFF CHUNCK ***********
    char                riff_chunk_id[4];       // "RIFF"
    unsigned int        riff_body_size;
    char                riff_format_code[4];    // "WAVE"
    

    // *********** FMT  CHUNCK ***********
    char                fmt_chunk_id[4];        // "fmt "
    unsigned int        fmt_body_size;
    unsigned short int  fmt_format_code;        // 3 for floating point
    unsigned short int  n_channels;             
    unsigned int        fmt_samples_rate;
    unsigned int        fmt_bytes_rate;
    unsigned short int  fmt_block_align;        // bytes per sample frame
    unsigned short int  fmt_bits_per_sample;
    unsigned short int  extension_size;         // 0 for float format

    // *********** FACT CHUNCK ***********
    char                fact_chunk_id[4];       // "fact"
    unsigned int        fact_body_size;
    unsigned int        n_sample_frames;        // n_channels * n_sample_by_channel

    // *********** DATA CHUNCK ***********
    char                data_chunk_id[4];       // "data"
    unsigned int        data_body_size;
} wave_header_t;


void write_wav(char* filename, double* data, wave_header_t*, unsigned int, unsigned );
wave_header_t* make_wave_header(unsigned int, unsigned short, unsigned int, unsigned short);
