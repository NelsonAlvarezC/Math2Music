#include "filewave.h"

void write_wav(char* filename, double* data, wave_header_t* header, unsigned int notes_len, unsigned samples_by_note){
    FILE* file;
    if((file = fopen(filename, "wb"))){
        fwrite(header, sizeof(wave_header_t), 1, file);
        fwrite(data, sizeof(double), samples_by_note*notes_len, file);
        fclose(file);
    }
    else
        fprintf(stderr,"[Write_wav] Couldn't create file\n");
}

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
    header->fmt_body_size = 18;         // size of fmt body for format 3
    header->n_channels = channels;
    header->fmt_samples_rate = samplerate;
    header->fmt_bytes_rate = bytes_per_sample * samplerate;
    header->fmt_block_align = bytes_per_sample;
    header->fmt_bits_per_sample = 64;   // double 64-bit
    header->extension_size = 0;         // format 3 does not require any extension

    header->fact_chunk_id[0] = 'f';
    header->fact_chunk_id[1] = 'a';
    header->fact_chunk_id[2] = 'c';
    header->fact_chunk_id[3] = 't';
    header->fact_body_size = 4;
    header->n_sample_frames = samples_by_channel * channels;    // total of samples

    header->data_chunk_id[0] = 'd';
    header->data_chunk_id[1] = 'a';
    header->data_chunk_id[2] = 't';
    header->data_chunk_id[3] = 'a';

    header->data_body_size = samples_by_channel * channels * bytes_per_sample;

    // body size excluding riff header:
    header->riff_body_size = 50 + header->data_body_size;
    return header;
}
