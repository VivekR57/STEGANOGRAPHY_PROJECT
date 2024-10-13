#include <stdio.h>
#include "encode.h"
#include "types.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    uint size;
    fseek(fptr, 0, SEEK_END);
    size = ftell(fptr);
    rewind(fptr);
    return size;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{

    // Open files
    if (open_files(encInfo) == e_failure)
    {
        return e_failure;
    }

    // Check capacity
    if (check_capacity(encInfo) == e_failure)
    {
        return e_failure;
    }

    // Copy BMP header
    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }

    // Encode magic string
    if (encode_magic_string(MAGIC_STRING, encInfo) == e_failure)
    {
        return e_failure;
    }

    return e_success;
}
Status check_capacity(EncodeInfo *encInfo)
{

    // Get the size of the source image
    uint image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    // Get the size of the secret file in bytes
    uint size_secret_file = get_file_size(encInfo->fptr_secret);

    // Get the length of the magic string in bytes
    uint magic_string_length = strlen(MAGIC_STRING);

    // Get the secret file extension
    char *file_extension = strchr(encInfo->secret_fname, '.');
    uint extension_size = 0;
    if (file_extension != NULL)
    {
        extension_size = strlen(file_extension);
        strcpy(encInfo->extn_secret_file, file_extension);
    }
    // Include BMP header size in calculations
    unsigned int header_size = 54;
    unsigned int total_size = header_size + ((magic_string_length + extension_size + sizeof(size_secret_file) + size_secret_file)*8);
    if (image_capacity >= total_size)
    {
        printf("Sufficient capacity to encode the secret data.\n");
        return e_success;
    }
    else
    {
        printf("ERROR: Insufficient capacity in the image to encode the secret data.\n");
        return e_failure;
    }
}
