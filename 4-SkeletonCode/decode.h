#ifndef DECODE_H
#define DECODE_H

#include "types.h"

typedef struct _DecodeInfo
{
    FILE *fptr_stego_image;
    char *stego_image_fname;

    char extension[5];
    int length;

    FILE *fptr_output_file;
} DecodeInfo;

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Open files for decoding */
Status open_files_for_decode(DecodeInfo *decInfo);

/* Decode magic string to confirm stego file */
Status decode_magic_string(DecodeInfo *decInfo);

/* Decode  file extension Size */
Status decode_file_extn_size(DecodeInfo *decInfo);

/* Decode secret file extension */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode LSB to Byte */
Status decode_lsb_to_byte(char *data, char *image_buffer);

/* Decode LSB to Int */
Status decode_lsb_to_int(int *data, char *image_buffer);

#endif