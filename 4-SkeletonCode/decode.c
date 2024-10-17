#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"

Status do_decoding(DecodeInfo *decInfo)
{
    if (open_files_for_decode(decInfo) == e_failure)
    {
        return e_failure;
    }

    if (decode_magic_string(decInfo) == e_failure)
    {
        return e_failure;
    }

    if (decode_file_extn_size(decInfo) == e_failure)
    {
        return e_failure;
    }
    if (decode_secret_file_extn(decInfo) == e_failure)
    {
        return e_failure;
    }
    if (decode_secret_file_size(decInfo) == e_failure)
    {
        return e_failure;
    }
    if (decode_secret_file_data(decInfo) == e_failure)
    {
        return e_failure;
    }

    return e_success;
}
Status open_files_for_decode(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    if (decInfo->fptr_stego_image == NULL)
    {
        printf("ERROR : unable to open the stego image\n");
        return e_failure;
    }
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET); // Skip the header for BMP images

    return e_success;
}
Status decode_magic_string(DecodeInfo *decInfo)
{
    char magic_string[3];
    char image_buffer[8] = {0};
    char decoded_magic_string[3] = {0};
    printf("Enter the magic string to decode :\n");
    scanf("%s", magic_string);

    for (int i = 0; i < strlen(magic_string); i++)
    {
        if (fread(image_buffer, sizeof(char), 8, decInfo->fptr_stego_image) != 8)
        {
            printf("ERROR : Unable to read the 8 bytes from the stego image\n");
            return e_failure;
        }
        decode_lsb_to_byte(&decoded_magic_string[i], image_buffer);
    }

    if (strcmp(magic_string, decoded_magic_string) == 0)
    {
        printf("The decoded magic string matches the input magic string!\n");
        return e_success;
    }
    else
    {
        printf("The decoded magic string does not match the input magic string.\n");
        return e_failure;
    }
}
Status decode_lsb_to_byte(char *data, char *image_buffer)
{
    *data = 0;
    for (int i = 0; i < 8; i++)
    {
        int lsb = image_buffer[i] & 1;
        *data = (*data << 1) | lsb;
    }
    return e_success;
}
Status decode_lsb_to_int(int *data, char *image_buffer)
{
    *data = 0;
    for (int i = 0; i < 32; i++)
    {
        int lsb = image_buffer[i] & 1;
        *data = (*data << 1) | lsb;
    }
    return e_success;
}
Status decode_file_extn_size(DecodeInfo *decInfo)
{
    char image_buffer[32] = {0};
    int extn_size = 0;
    if (fread(image_buffer, sizeof(char), 32, decInfo->fptr_stego_image) != 32)
    {
        printf("ERROR: Unable to read 32 bytes from Stego image image\n");
        return e_failure;
    }
    decode_lsb_to_int(&extn_size, image_buffer);

    return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char file_exten[5];
    char image_buffer[8] = {0};

    for (int i = 0; i < 4; i++)
    {
        if (fread(image_buffer, sizeof(char), 8, decInfo->fptr_stego_image) != 8)
        {
            printf("ERROR : Unable to read 8 bytes from the stego image\n");
            return e_failure;
        }
        decode_lsb_to_byte(&file_exten[i], image_buffer);
    }
    printf("The file extension is: %s\n", file_exten);
    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    
    return e_success;
}


Status decode_secret_file_data(DecodeInfo *decInfo)
{
    
    return e_success;
}


