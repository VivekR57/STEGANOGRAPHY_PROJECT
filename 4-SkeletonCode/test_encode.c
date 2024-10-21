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
    if (argc > 1)
    {
        if (check_operation_type(argv[1]) == e_encode)
        {
            if (read_and_validate_encode_args(argc, argv, &encInfo) == e_failure)
            {
                return e_failure;
            }
            do_encoding(&encInfo);
        }
        else if (check_operation_type(argv[1]) == e_decode)
        {
            if (read_and_validate_decode_args(argc, argv, &decInfo) == e_failure)
            {
                return e_failure;
            }
            do_decoding(&decInfo);
        }
        else
        {
            printf("Invalid input\n");
        }
    }
    else
    {
        printf("Encoding: ./lsb_steg -e <.bmp file> <.txt file> [output file]\n");
        printf("Decoding: ./lsb_steg -d <.bmp file> [output file]\n");
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
