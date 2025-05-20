#include<stdio.h>
#include<string.h>
#include "Decode.h"
#include "common.h"
#include "types.h"
#include<stdlib.h>

/*opening files*/
Status open_file(DecodeInfo *decInfo)                                                            //function definition for open file
{
	printf("---opening files---\n");
	decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname,"r");                       //open the stego image file in read mode store in stego image pointer
	if(decInfo -> fptr_stego_image == NULL)                                                  //validate the pointer
	{
		perror("fopen");
		fprintf(stderr, "Error : unable to open file %s\n", decInfo->stego_image_fname);
		return e_failure;
	}

	return e_success;                                                                         //return success if it is not equal to null
}

/*read and validation*/
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)                          //read and validate the function by passing 
{
	unsigned char arr[2];
	if(strcmp(strchr(argv[2],'.'),".bmp") != 0)                                              //check whether input file is bmp file
	{
		printf("The source file is not a bmp file\n");                                   
	       return e_failure;
	}
	
	decInfo->fptr_stego_image = fopen(argv[2],"r");
	fread(arr, 1, 2, (decInfo->fptr_stego_image));
	if((arr[0] == 'B') && (arr[1] == 'M'))	                                                   //check for first 2 bytes to find bmp file or not
	{
		printf("Source file is bmp file\n");                                            
		decInfo->stego_image_fname = argv[2];
	}
	fclose(decInfo->fptr_stego_image);
	if(argv[3] != NULL)                                                                       //check whether output file has passed or not
	{
		printf("The output file is passed\n");
		strcpy(decInfo->output_fname, argv[3]);
		printf("The file name given by user is %s\n",decInfo->output_fname);
	}
	else
	{ 
		strcpy(decInfo->output_fname, "decoded_file");                                  //if not passed default name will be provided
		printf("The default filename is %s\n",decInfo->output_fname);
	}
	return e_success;
}

Status decode_magic_string(DecodeInfo *decInfo)                                                  //decode magic string here 
{	
	int len = strlen(MAGIC_STRING);
	char str[len+1];
	fseek(decInfo->fptr_stego_image, 54, SEEK_SET);
	decode_data_from_img(len, decInfo->fptr_stego_image, str);
	if(strcmp(MAGIC_STRING, str) == 0)                                                   //after decoding check whether original and decoded magic strings are same
		return e_success;
	else
		return e_failure;
	return e_success;
}
Status decode_data_from_img(int size, FILE *fptr_stego_image, char *str)                     //decode data from image 
{
	char arr[8];                                                                          
	for(int i = 0; i < size; i++)
	{
		fread(arr, 8, 1, fptr_stego_image);
		unsigned char ch = 0;
		for(int j = 0; j < 8; j++)
		{
			ch |= ((arr[j] & 1) << (7-j));                                        //logic for decoding data from image
		}
		str[i] = ch;
	}
	str[size] = '\0';                                                                     //after that add null char to str
        return e_success;	
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)                                 //decode secret file extension size
{
	printf("--Decoding o/p file extn size--\n");
	char buffer[32];
	fread(buffer, 32, 1,decInfo->fptr_stego_image);
	decInfo->secret_file_esize = decode_size_from_lsb(buffer);                        //by calling decode size from lsb function decode the secret file extension
	printf("%ld\n",decInfo->secret_file_esize);
	return e_success;                                                                 //return success if itis true
}

long decode_size_from_lsb(char *buffer)                                                  //function definition decode size from lsb 
{
	long num = 0;
	for(int i = 0; i < 32; i++)
	{
		num = num | ((buffer[i] & 1) <<(31-i));                                       //logic for decoding size from lsb
	}
	return num;                                                                         //return number
}

Status decode_secret_file_extn(DecodeInfo *decInfo)                                          //decode secret file extension
{
	char str[decInfo->secret_file_esize + 1];                                            
	int len = strlen(str);
	decode_data_from_img(decInfo->secret_file_esize, decInfo->fptr_stego_image, str);  //call decode data from image
	strcpy(decInfo->secret_file_extn, str);
	return e_success;                                                                   //return success if it is true 
}

Status decode_secret_file_size(DecodeInfo *decInfo)                                            //decode secret file size 
{
	printf("--Decoding o/p file extn size--\n");
	char buffer[32];
	fread(buffer, 1, 32,decInfo->fptr_stego_image);                                        //read 32 bytes from stego image pointer to buffer                                   
	decInfo->secret_file_size = decode_size_from_lsb(buffer);                               //and call decode_size_from_lsb and store it in secret file size
	return e_success;
}

Status decode_secret_data(DecodeInfo *decInfo)                                              //fun definition for decode secret data
{       printf("---Decoding secret file data---\n");                    
	strcpy(decInfo->output_file_name, decInfo->output_fname);                           //copy data to output file 
	strcat(decInfo->output_file_name, decInfo->secret_file_extn);                        //concatenate extension to output file name
	decInfo->fptr_output = fopen(decInfo->output_file_name, "w");                        //and write it into output file
	if(decInfo->fptr_output == NULL)
	{
		perror("fopen");
		fprintf(stderr, "Error: unable to open file %s\n",decInfo->output_fname);
		return e_failure;
	}
char str[decInfo->secret_file_size + 1];                                                    //create 1 str and store secret file size + 1
int len = strlen(str);
decode_data_from_img(decInfo->secret_file_size, decInfo->fptr_stego_image, str);             //call decode data from img
fwrite(str, decInfo->secret_file_size, 1, decInfo->fptr_output);                              //fwrite from output into str byte by byte
printf("%s\n",str);                                                                          //print the string
return e_success;                                                                               //and return if it is success
}

Status do_decoding(DecodeInfo *decInfo)                                                       //do decoding
{

	printf("---Decoding process started---\n");                                           
	if(open_file(decInfo) != e_success)                                                  //fun call for open files and validate it
	{
		printf("error in opening file\n");
		return e_failure;
	}
	printf("File opened successfully\n");

	if(decode_magic_string(decInfo) != e_success)                                         //fun call for decode_magic_string and validate it
	{
		printf("magic string not decoded\n");
		
	}
	else
	{
		printf("Magic string successfully decoded\n");
	}

	if(decode_secret_file_extn_size(decInfo) != e_success)                              //fun call for secret file extn size and validate it
	{
		printf("failed to decode secret file extn size\n");
		return e_failure;
	}
	printf("secret file extn size decoded successfully\n");

	if(decode_secret_file_extn(decInfo) != e_success)                                    //fun call for secret file extn and validate it
	{
		printf("Failed to decode secret file extension data\n");
		return e_failure;
	}
	printf("secret file extension file data decoded successfully\n");

	if(decode_secret_file_size(decInfo) != e_success)                                     //fun call for decode secret file size and validate it
	{
		printf("Failed to decode secret file data size\n");
		return e_failure;
	}
	printf("secret file data size decoded successfully\n");

	if(decode_secret_data(decInfo) != e_success)                                       //fun call for decode secret data and validate it
	{
		printf("Data not decoded\n");
		return e_failure;
	}
	printf("Secret data decoded successfully\n");
	return e_success;

}

