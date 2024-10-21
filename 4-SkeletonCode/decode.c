#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"

/* Read and validate decoding arguments
 * Description: Validates the command line arguments for decoding the stego image.
 * Input: argc - argument count, argv - argument vector, decInfo - structure to hold decoding information.
 * Output: Returns e_success if arguments are valid, else returns e_failure.
 */
Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo)
{
    if (argc < 3 || argc > 4) // Check if the argument count is correct (either 3 or 4)
    {
        printf("Decoding: ./lsb_steg -d <.bmp file> [output file]\n");
        return e_failure;
    }

    // Validate source image file (should end with .bmp)
    char *str = strstr(argv[2], ".bmp");
    if (str != NULL || strcmp(str, ".bmp") == 0)
    {
        decInfo->stego_image_fname1 = argv[2]; // Store the name of the stego image
    }
    else
    {
        printf("Decoding: ./lsb_steg -d <.bmp file> [output file]\n");
        return e_failure;
    }

    // Handle optional output file name
    if (argc == 4)
    {
        strcpy(decInfo->output_fname, argv[3]); // Store the name as provided, without extension manipulation
    }
    else
    {
        strcpy(decInfo->output_fname, "output"); // Default name without extension
        printf("No output file provided. Using default: %s\n", decInfo->output_fname);
    }

    printf("Output file name: %s\n", decInfo->output_fname);
    return e_success;
}

/* Do decoding
 * Description: Performs the decoding procedure for the stego image.
 * Input: decInfo - structure containing decoding information.
 * Output: Returns e_success if decoding is completed successfully, else returns e_failure.
 */
Status do_decoding(DecodeInfo *decInfo)
{
    printf("INFO: ## Decoding Procedure Started ##\n");
    if (open_files_for_decode(decInfo) == e_failure) // Open files needed for decoding
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

/* Open files for decoding
 * Description: Opens the stego image file and prepares it for reading.
 * Input: decInfo - structure containing decoding information.
 * Output: Returns e_success if files are opened successfully, else returns e_failure.
 */
Status open_files_for_decode(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname1, "r"); // Open the stego image file in read mode
    if (decInfo->fptr_stego_image == NULL)
    {
        printf("ERROR : unable to open the stego image\n");
        return e_failure;
    }
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET); // Skip BMP header
    printf("INFO: Opening required files\n");
    printf("INFO: Opened %s\n", decInfo->stego_image_fname1);
    return e_success;
}

/* Decode magic string
 * Description: Decodes a magic string from the stego image and validates it against user input.
 * Input: decInfo - structure containing decoding information.
 * Output: Returns e_success if the decoded magic string matches the input, else returns e_failure.
 */
Status decode_magic_string(DecodeInfo *decInfo)
{
    char magic_string[3];         // Buffer for the magic string to be decoded
    char image_buffer[8] = {0};   // Buffer to hold image data
    char decoded_magic_string[3]; // Buffer for the decoded magic string

    printf("Enter the magic string to decode:\n");
    scanf("%s", magic_string);

    // Decode the magic string from the stego image
    for (int i = 0; i < strlen(magic_string); i++)
    {
        if (fread(image_buffer, sizeof(char), 8, decInfo->fptr_stego_image) != 8)
        {
            printf("ERROR : Unable to read 8 bytes from the stego image\n");
            return e_failure;
        }
        decode_lsb_to_byte(&decoded_magic_string[i], image_buffer); // Decode each byte
    }

    decoded_magic_string[strlen(magic_string)] = '\0'; // Null-terminate the decoded string

    if (strcmp(magic_string, decoded_magic_string) == 0) // Check if the decoded magic string matches the input
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

/* Decode LSB to byte
 * Description: Decodes a single byte from the least significant bits of an image buffer.
 * Input: data - pointer to the variable to store the decoded byte, image_buffer - buffer containing image data.
 * Output: Returns e_success after decoding.
 */
Status decode_lsb_to_byte(char *data, char *image_buffer)
{
    *data = 0; // Initialize the decoded byte to zero
    for (int i = 0; i < 8; i++)
    {
        int lsb = image_buffer[i] & 1; // Extract the least significant bit
        *data = (*data << 1) | lsb;    // Shift and store the LSB
    }
    return e_success;
}

/* Decode LSB to integer
 * Description: Decodes an integer from the least significant bits of an image buffer.
 * Input: data - pointer to the variable to store the decoded integer, image_buffer - buffer containing image data.
 * Output: Returns e_success after decoding.
 */
Status decode_lsb_to_int(int *data, char *image_buffer)
{
    *data = 0; // Initialize the decoded integer to zero
    for (int i = 0; i < 32; i++)
    {
        int lsb = image_buffer[i] & 1; // Extract the least significant bit
        *data = (*data << 1) | lsb;    // Shift and store the LSB
    }
    return e_success;
}

/* Decode file extension size
 * Description: Reads the size of the file extension from the stego image.
 * Input: decInfo - structure containing decoding information.
 * Output: Returns e_success if the size is read successfully, else returns e_failure.
 */
Status decode_file_extn_size(DecodeInfo *decInfo)
{
    char image_buffer[32] = {0}; // Buffer to hold 32 bytes of image data
    int extn_size = 0;           // Variable to store the decoded extension size

    // Read 32 bytes from the stego image
    if (fread(image_buffer, sizeof(char), 32, decInfo->fptr_stego_image) != 32)
    {
        printf("ERROR: Unable to read 32 bytes from Stego image\n");
        return e_failure;
    }
    decode_lsb_to_int(&extn_size, image_buffer); // Decode the extension size

    decInfo->length = extn_size; // Store the decoded size in DecodeInfo structure
    return e_success;
}

/* Decode secret file extension
 * Description: Decodes the file extension of the secret file from the stego image.
 * Input: decInfo - structure containing decoding information.
 * Output: Returns e_success if the extension is decoded successfully, else returns e_failure.
 */
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    int size = decInfo->length; // Get the size of the extension to decode
    char file_exten[20] = {0};  // Buffer for the decoded file extension
    char image_buffer[8] = {0}; // Buffer to hold image data

    // Read the extension from the stego image
    for (int i = 0; i < size; i++)
    {
        if (fread(image_buffer, sizeof(char), 8, decInfo->fptr_stego_image) != 8)
        {
            printf("ERROR: Unable to read 8 bytes from the stego image\n");
            return e_failure;
        }
        decode_lsb_to_byte(&file_exten[i], image_buffer);// Decode each byte of the extension
    }
    file_exten[size] = '\0';// Null-terminate the decoded extension
    strcpy(decInfo->extension, file_exten); // Store the extension in the DecodeInfo structure

    // Update the output file name with the decoded extension
    char *dot = strchr(decInfo->output_fname, '.'); // Find the last dot in the output file name
    if (dot != NULL)
    {
        *dot = '\0'; // Remove the existing extension
    }
    strcat(decInfo->output_fname, file_exten); // Append the decoded extension
    printf("Output file with decoded extension: %s\n", decInfo->output_fname);

    decInfo->fptr_output_file = fopen(decInfo->output_fname, "w"); // Open the output file for writing
    if (decInfo->fptr_output_file == NULL)
    {
        printf("ERROR: Unable to open the output file\n");
        return e_failure;
    }

    printf("INFO: Opened %s\n", decInfo->output_fname); // For the output file
    printf("INFO: Done. Opened all required files\n");
    return e_success;
}


/* Decode secret file size
 * Description: Reads the size of the secret file from the stego image.
 * Input: decInfo - structure containing decoding information.
 * Output: Returns e_success if the size is read successfully, else returns e_failure.
 */
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char image_buffer[32] = {0}; // Buffer to hold 32 bytes of image data
    int size = 0; // Variable to store the decoded file size

    // Read 32 bytes from the stego image
    if (fread(image_buffer, sizeof(char), 32, decInfo->fptr_stego_image) != 32)
    {
        printf("ERROR: Unable to read 32 bytes from Stego image\n");
        return e_failure;
    }

    decode_lsb_to_int(&size, image_buffer);// Decode the file size
    decInfo->file_size = size; // Store the decoded size in the DecodeInfo structure
    printf("INFO: Decoding %s File Size\n", decInfo->output_fname); // Reference to the output file
    printf("INFO: Done\n");
    return e_success;
}


/* Decode secret file data
 * Description: Reads the actual data of the secret file from the stego image and writes it to the output file.
 * Input: decInfo - structure containing decoding information.
 * Output: Returns e_success if the data is decoded and written successfully, else returns e_failure.
 */
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    rewind(decInfo->fptr_output_file);// Reset the output file pointer to the beginning

    char buffer[8]; // Buffer to hold 8 bytes of image data
    char ch;// Variable to hold each decoded character
    for (int i = 0; i < decInfo->file_size; i++)// Read the secret file data from the stego image
    {
        if (fread(buffer, sizeof(char), 8, decInfo->fptr_stego_image) != 8)
        {
            printf("ERROR: Unable to read 8 bytes from stego image\n");
            return e_failure;
        }
        if (decode_lsb_to_byte(&ch, buffer) != e_success)// Decode each character
        {
            printf("ERROR: Decoding from LSB failed\n");
            return e_failure;
        }
        if (fwrite(&ch, sizeof(char), 1, decInfo->fptr_output_file) != 1) // Write the decoded character to the output file
        {
            printf("ERROR: Unable to write to output file\n");
            return e_failure;
        }
    }
    printf("INFO: Decoding %s File Data\n", decInfo->output_fname);
    printf("INFO: Done\n");
    return e_success;
}