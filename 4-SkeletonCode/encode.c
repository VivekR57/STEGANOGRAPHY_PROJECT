#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"

#define MAGIC_STRING "#*"
#define MAX_SECRET_BUF_SIZE 1024
/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

// Get image size
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

// Get image size
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

/*It manges the encoding process
Description:It manages the encoding process,calling other function like open file,check capacity and all other functions
INPUT:Get the structure pointer as input
output: If all operation success means return e_succes or if not means return e_failure
*/
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

    if(encode_secret_extn_size(encInfo->extn_size,encInfo)==e_failure)
    {
        return e_failure;
    }

    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_failure)
    {
        return e_failure;
    }

    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure)
    {
        return e_failure;
    }

    if(encode_secret_file_data(encInfo)==e_failure)
    {
        return e_failure;
    }

    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }

    return e_success;
}
/*
check capacity
Description:check if the BMP image has sufficient capacity to encode the secret data or not
input:source image,secret file,magic string,file extension,header size
Output:if the image can hold the required data means return e_succes otherwise return e_failure
*/
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
        encInfo->size_secret_file = strlen(file_extension);
        extension_size = strlen(file_extension);
        encInfo->extn_size=extension_size;
        strcpy(encInfo->extn_secret_file, file_extension);
    }
    // Include BMP header size in calculations
    unsigned int header_size = 54;
    // Calculate total size correctly
    unsigned int total_size = header_size + ((magic_string_length + extension_size + size_secret_file + sizeof(size_secret_file) + sizeof(extension_size)) * 8);
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

/*
copy BMP header
Description:copying BMP Header from source image to destination image
Input:file pointer of source image and file pointer of destination image
Output:If the function succesfully copies BMP Header from source image to destination means return e_succes ,if fails means return e_failure 
*/
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char buffer[54];
    rewind(fptr_src_image);
    rewind(fptr_dest_image);

    if (fread(buffer, 54, 1, fptr_src_image) != 1)
    {
        printf("ERROR: Unable to read the BMP header from the source image.\n");
        return e_failure;
    }

    if (fwrite(buffer, 54, 1, fptr_dest_image) != 1)
    {
        printf("ERROR: Unable to write the BMP header from the destination image.\n");
        return e_failure;
    }

    return e_success;
}

/*Encode magic string
Description:Encode a magic string into the LSB of BMP image
Input:magic string
output:If the magic string is encoded to the corresponding bits in the image means return e_success ,otherwise return e_failure
*/
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{

    FILE *src_file = encInfo->fptr_src_image;
    FILE *stego_file = encInfo->fptr_stego_image;
    char image_buffer[8] = {0};

    int length = strlen(MAGIC_STRING);
    for (int i = 0; i < length; i++)
    {
        if (fread(image_buffer, sizeof(char), 8, src_file) != 8)
        {
            return e_failure;
        }

        char ch = magic_string[i];

        encode_byte_to_lsb(ch, image_buffer);

        if (fwrite(image_buffer, sizeof(char), 8, stego_file) != 8)
        {
            return e_failure;
        }
    }
    return e_success;
}
Status encode_secret_extn_size(long file_size, EncodeInfo *encInfo)
{
    FILE *src_file = encInfo->fptr_src_image;
    FILE *stego_file = encInfo->fptr_stego_image;

    char image_buffer[32] = {0};

    // Read 32 bytes from the source image file
    if (fread(image_buffer, sizeof(char), 32, src_file) != 32)
    {
        printf("ERROR: Unable to read 32 bytes from source image\n");
        return e_failure;
    }

    // Encode the file size into LSBs of the 32-byte buffer
    encode_int_to_lsb(file_size, image_buffer);

    // Write the modified buffer to the stego image file
    if (fwrite(image_buffer, sizeof(char), 32, stego_file) != 32)
    {
        printf("ERROR: Unable to write 32 bytes to stego image\n");
        return e_failure;
    }

    return e_success;
}

/*
Encode byte to LSB
Description : Encode aa byte into LSB of a provided image buffer
Input: data,image buffer
Output: if  the byte is  Encoded into LSB of image means return e_success
*/

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 0; i < 8; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE);    // Clear the least significant bit
        char bit = (data & (1 << (7 - i))) >> (7 - i); // Extract the bit from the data
        image_buffer[i] |= bit;                        // Set the least significant bit with the bit from data
    }
    return e_success;
}


/*
Encode Int to LSB
Description:Encode a inter value into the LSB of the provided image buffer
Input: data,image buffer
output: if the bytes is encode into LSB of image means return e_succes
*/
Status encode_int_to_lsb(int data, char *image_buffer)
{
    for (int i = 0; i < 32; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE);     // Clear the LSB
        int bit = (data & (1 << (31 - i))) >> (31 - i); // Extract the bit from data
        image_buffer[i] |= bit;                         // Set the LSB with the extracted bit
    }
    return e_success;
}

/* 
Encode secret file extenstion 
Description:Encode the file extension of secret file into BMP image's LSB
Input: file extension
Output: if the file extension is modifies into the corresponding bits in the image means return e_success otherwise return e_failure
*/
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    FILE *src_file = encInfo->fptr_src_image;
    FILE *stego_file = encInfo->fptr_stego_image;
    char image_buffer[8] = {0};

    int length = strlen(file_extn);
   // printf("Encoding file extension: %s\n", file_extn);

    for (int i = 0; i < length; i++)
    {
        if (fread(image_buffer, sizeof(char), 8, src_file) != 8)
        {
            printf("Failed to read 8 bytes from source file\n");
            return e_failure;
        }

        char ch = file_extn[i];
        // printf("Encoding character: %c\n", ch);
        encode_byte_to_lsb(ch, image_buffer);

        if (fwrite(image_buffer, sizeof(char), 8, stego_file) != 8)
        {
            printf("Failed to write 8 bytes to stego file\n");
            return e_failure;
        }
    }
    return e_success;
}


/*
Encode secret file size
Description:Encode the  size of the secret file into the BMP image LSB
Input: file size
Output: if the file size modified into the corresponding bits in the image means return e_success otherwise return e_failure
*/
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    FILE *src_file = encInfo->fptr_src_image;
    FILE *stego_file = encInfo->fptr_stego_image;

    char image_buffer[32] = {0};

    // Read 32 bytes from the source image file
    if (fread(image_buffer, sizeof(char), 32, src_file) != 32)
    {
        printf("ERROR: Unable to read 32 bytes from source image\n");
        return e_failure;
    }

    // Encode the file size into LSBs of the 32-byte buffer
    encode_int_to_lsb(file_size, image_buffer);

    // Write the modified buffer to the stego image file
    if (fwrite(image_buffer, sizeof(char), 32, stego_file) != 32)
    {
        printf("ERROR: Unable to write 32 bytes to stego image\n");
        return e_failure;
    }

    return e_success;
}

/* Encode secret file data
Description:Encode the actual data of the secret file into the LSB of the BMP image
Input: file pointer of secret file
Output: if the secret file data is encoded into the LSB of the BMP image means return e_success otherwise return e_failure
*/

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    FILE *src_file = encInfo->fptr_src_image;
    FILE *stego_file = encInfo->fptr_stego_image;
    FILE*secret_file=encInfo->fptr_secret;
    if(secret_file==NULL)
    {
        printf("ERROR: Unable to open the file\n");
        return e_failure;
    }
    long size=encInfo->size_secret_file;
    char secret_data[MAX_SECRET_BUF_SIZE];

    if(fread(secret_data,sizeof(char),size,secret_file)!=size)
    {
        printf("ERROR: Failed to read secret file data.\n");
        return e_failure;
    }

    char image_buffer[8]={0};
    for(int i=0;i<size;i++)
    {
        if(fread(image_buffer,sizeof(char),8,src_file)!=8)
        {
            printf("ERROR: Failed to read 8 bytes from source image\n");
            return e_failure;
        }

        encode_byte_to_lsb(secret_data[i],image_buffer);

        if(fwrite(image_buffer,sizeof(char),8,stego_file)!=8)
        {
            printf("ERROR: Failed to write encoded data to stego image\n");
            return e_failure;
        }
    }
    return e_success;
}

/*Copy remaining image data
Description: copies the remaining data of the source BMP image to the destination BMP image after encoding all the things 
Input: file pointer of source image and destination image
Output: if the source image copies those bytes to the stego image means return e_success otherwise return e_failure
*/

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    int ch;

    // Loop until the end of the source file
    while((ch=fgetc(fptr_src))!=EOF)
    {
        // Write the character to the destination file
        if(fputc(ch,fptr_dest)==EOF)
        {
            printf("ERROR : unable to write the remaining data to stego image\n");
            return e_failure;
        }
    }
    return e_success;
}

