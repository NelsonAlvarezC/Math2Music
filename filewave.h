// Author: Nelson Alvarez

#ifndef FILEWAVE
#define FILEWAVE
#include <stdlib.h>

typedef struct __attribute__((__packed__)) wave_header{

    // *********** RIFF CHUNCK ***********
    char                riff_chunk_id[4];
    unsigned int        riff_body_size;
    char                riff_format_code[4];
    

    // *********** FMT  CHUNCK ***********
    char                fmt_chunk_id[4];
    unsigned int        fmt_body_size;
    unsigned short int  fmt_format_code;
    unsigned short int  n_channels;
    unsigned int        fmt_samples_rate;
    unsigned int        fmt_bytes_rate;
    unsigned short int  fmt_block_align;        // bytes per sample frame
    unsigned short int  fmt_bits_per_sample;
    unsigned short int  extension_size;         // 0 for float format

    // *********** FACT CHUNCK ***********
    char                fact_chunk_id[4];
    unsigned int        fact_body_size;
    unsigned int        n_sample_frames;        // n_channels * n_sample_by_channel

    // *********** DATA CHUNCK ***********
    char                data_chunk_id[4];
    unsigned int        data_body_size;
} wave_header_t;


wave_header_t* make_wave_header(unsigned int samples_by_channel,unsigned short channels, unsigned int samplerate, unsigned short bytes_per_sample){
    wave_header_t* header = (wave_header_t*)malloc(sizeof(wave_header_t));

    header->riff_chunk_id[0] = 'R';
    header->riff_chunk_id[1] = 'I';
    header->riff_chunk_id[2] = 'F';
    header->riff_chunk_id[3] = 'F';
    
    header->riff_format_code[0] = 'W';
    header->riff_format_code[1] = 'A';
    header->riff_format_code[2] = 'V';
    header->riff_format_code[3] = 'E';

    header->fmt_chunk_id[0] = 'f';
    header->fmt_chunk_id[1] = 'm';
    header->fmt_chunk_id[2] = 't';
    header->fmt_chunk_id[3] = ' ';

    header->fmt_format_code = 3;
    header->fmt_body_size = 18;
    header->n_channels = channels;
    header->fmt_samples_rate = samplerate;
    header->fmt_bytes_rate = bytes_per_sample * samplerate;
    header->fmt_block_align = bytes_per_sample;
    header->fmt_bits_per_sample = 64;   // double
    header->extension_size = 0;

    header->fact_chunk_id[0] = 'f';
    header->fact_chunk_id[1] = 'a';
    header->fact_chunk_id[2] = 'c';
    header->fact_chunk_id[3] = 't';
    header->fact_body_size = 4;
    header->n_sample_frames = samples_by_channel * channels;

    header->data_chunk_id[0] = 'd';
    header->data_chunk_id[1] = 'a';
    header->data_chunk_id[2] = 't';
    header->data_chunk_id[3] = 'a';

    header->data_body_size = samples_by_channel * channels * bytes_per_sample;

    // body size excluding riff header:
    header->riff_body_size = 50 + header->data_body_size;
    return header;
}


#endif
