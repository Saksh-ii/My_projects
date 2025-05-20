#ifndef DECODE_H
#define DECODE_H

#include "types.h"

typedef struct _DecodeInfo
{
    /*stego Image info*/
    char *stego_image_fname;
    FILE *fptr_stego_image;

    long secret_file_esize;
    long secret_file_size;
    char secret_file_extn[20];
    
    /*output file Info*/
    char output_fname[10];
    FILE *fptr_output;
    char output_file_name[20];

} DecodeInfo;

/* Decoding function prototype */

/*Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/*do decoding process*/
Status do_decoding(DecodeInfo *decInfo);

/*open files*/
Status open_file(DecodeInfo *decInfo);

/*Skip bmp header
Status skip_bmp_header(FILE *fptr_stego_image);*/

/*decoding of magic string*/
Status decode_magic_string(DecodeInfo *decInfo);

/*decode each data from img*/
Status decode_data_from_img(int size, FILE *fptr_stego_image, char *data);

/*decode secret file extension size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/*decoding size to lsb of a img data array*/
long decode_size_from_lsb(char *image_buffer);

/*Decode secret file extension data*/
Status decode_secret_file_extn(DecodeInfo *decInfo);

/*decode secret file data size*/
Status decode_secret_file_size(DecodeInfo *decInfo);

/*decode secret file data*/
Status decode_secret_data(DecodeInfo *decInfo);

#endif



