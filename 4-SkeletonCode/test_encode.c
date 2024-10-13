#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;
    encInfo.src_image_fname = argv[2];
    encInfo.secret_fname = argv[3];
    encInfo.stego_image_fname = "stego_img.bmp";
    if (check_operation_type(argv[1]) == e_encode)
    {
        printf("User selected Encoding\n");
    }
    else if (check_operation_type(argv[1]) == e_decode)
    {
        printf("User selected Decoding\n");
    }
    else
    {
        printf("Invalid input\n");
    }
    return 0;
}

// Check operation type
OperationType check_operation_type(char *argv)
{
    if (strcmp(argv, "-e") == 0)
    {
        return e_encode;
    }
    else if (strcmp(argv, "-d") == 0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}
