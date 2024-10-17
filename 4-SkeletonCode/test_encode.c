#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    encInfo.src_image_fname = argv[2];
    encInfo.secret_fname = argv[3];
    encInfo.stego_image_fname = "stego_img.bmp";
    if (check_operation_type(argv[1]) == e_encode)
    {
        printf("User selected Encoding\n");
        do_encoding(&encInfo);
    }
    else if (check_operation_type(argv[1]) == e_decode)
    {
        printf("User selected Decoding\n");
        do_encoding(&encInfo);
    }
    else
    {
        printf("Invalid input\n");
    }
    return 0;
}

/*check operation type
DESCRIPTION:check_operation_type based on the command line argument
INPUT:command line argument as input for encode '-e' and for decode '-d'
output:it will return based on input for '-e' return e_encode and '-d' return e_decode and for invalid input return e_unspported
*/
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
