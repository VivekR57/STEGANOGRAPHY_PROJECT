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
    if (check_operation_type(argv[1]) == e_encode)
    {
       // printf("User selected Encoding\n");
        if (read_and_validate_encode_args(argc, argv, &encInfo) == e_failure)
        {
            //printf("Argument validation failed. Exiting...\n");
            return e_failure;
        }
        do_encoding(&encInfo);
    }
    else if (check_operation_type(argv[1]) == e_decode)
    {
      //  printf("User selected Decoding\n");
        if (read_and_validate_decode_args(argc, argv, &decInfo) == e_failure)
        {
           // printf("Argument validation failed. Exiting...\n");
            return e_failure;
        } 
        do_decoding(&decInfo);
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
