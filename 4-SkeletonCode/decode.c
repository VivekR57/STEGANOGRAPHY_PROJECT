#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"

Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo)
{
    if (argc < 3 || argc > 4)
    {
        printf("  Encoding: ./lsb_steg -e <.bmp file> <.txt file> [output file]\n");
        printf("  Decoding: ./lsb_steg -d <.bmp file> [output file]\n");
        return e_failure;
    }

    if (strstr(argv[2], ".bmp") == NULL)
    {
        printf("ERROR: Source image file must have a .bmp extension.\n");
        return e_failure;
    }
    decInfo->stego_image_fname1 = argv[2];
    if (argc == 4)
    {
        decInfo->output_fname = argv[3];
    }
    else
    {
        decInfo->output_fname = "output.txt";
    }
}
Status do_decoding(DecodeInfo *decInfo)
{
    printf("INFO: ## Decoding Procedure Started ##\n");
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
    printf("INFO: ## Decoding Done Successfully ##\n");
    return e_success;
}
Status open_files_for_decode(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname1, "r");
    if (decInfo->fptr_stego_image == NULL)
    {
        printf("ERROR : unable to open the stego image\n");
        return e_failure;
    }
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);
    printf("INFO: Opening required files\n");
    printf("INFO: Opened %s\n", decInfo->stego_image_fname1);
    return e_success;
}
Status decode_magic_string(DecodeInfo *decInfo)
{
    char magic_string[3];
    char image_buffer[8] = {0};
    char decoded_magic_string[3] = {0};

    printf("Enter the magic string to decode:\n");
    scanf("%s", magic_string);

    for (int i = 0; i < strlen(magic_string); i++)
    {
        if (fread(image_buffer, sizeof(char), 8, decInfo->fptr_stego_image) != 8)
        {
            printf("ERROR : Unable to read 8 bytes from the stego image\n");
            return e_failure;
        }
        decode_lsb_to_byte(&decoded_magic_string[i], image_buffer);
    }

    decoded_magic_string[strlen(magic_string)] = '\0';

    if (strcmp(magic_string, decoded_magic_string) == 0)
    {
        printf("INFO: Decoding Magic String Signature\n");
        printf("INFO: Done\n");
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
        printf("ERROR: Unable to read 32 bytes from Stego image\n");
        return e_failure;
    }
    decode_lsb_to_int(&extn_size, image_buffer);

    decInfo->length = extn_size;
    return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    int size = decInfo->length;
    char file_exten[20] = {0};
    char image_buffer[8] = {0};
    char output_file_name[20] = "output";

    for (int i = 0; i < size; i++)
    {
        if (fread(image_buffer, sizeof(char), 8, decInfo->fptr_stego_image) != 8)
        {
            printf("ERROR: Unable to read 8 bytes from the stego image\n");
            return e_failure;
        }
        decode_lsb_to_byte(&file_exten[i], image_buffer);
    }

    file_exten[size] = '\0';
    strcpy(decInfo->extension, file_exten);
    strcat(output_file_name, file_exten);
    decInfo->fptr_output_file = fopen(output_file_name, "w");
    if (decInfo->fptr_output_file == NULL)
    {
        printf("ERROR: Unable to open the output file\n");
        return e_failure;
    }
    printf("INFO: Opened %s\n", decInfo->output_fname); // For the output file
    printf("INFO: Done. Opened all required files\n");
    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char image_buffer[32] = {0};
    int size = 0;
    if (fread(image_buffer, sizeof(char), 32, decInfo->fptr_stego_image) != 32)
    {
        printf("ERROR: Unable to read 32 bytes from Stego image\n");
        return e_failure;
    }

    decode_lsb_to_int(&size, image_buffer);
    // printf("----%d\n", size);
    decInfo->file_size = size;
    printf("INFO: Decoding %s File Size\n", decInfo->output_fname); // Reference to the output file
    printf("INFO: Done\n");
    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    rewind(decInfo->fptr_output_file);

    char buffer[8];
    char ch;
    //   printf("%d\n", decInfo->file_size);
    for (int i = 0; i < decInfo->file_size; i++)
    {
        if (fread(buffer, sizeof(char), 8, decInfo->fptr_stego_image) != 8)
        {
            printf("ERROR: Unable to read 8 bytes from stego image\n");
            return e_failure;
        }
        if (decode_lsb_to_byte(&ch, buffer) != e_success)
        {
            printf("ERROR: Decoding from LSB failed\n");
            return e_failure;
        }
        if (fwrite(&ch, sizeof(char), 1, decInfo->fptr_output_file) != 1)
        {
            printf("ERROR: Unable to write to output file\n");
            return e_failure;
        }
    }
    printf("INFO: Decoding %s File Data\n", decInfo->output_fname); // Reference to the output file
    printf("INFO: Done\n");
    return e_success;
}