#include <stdio.h> 
#include "encode.h"
#include "types.h"
#include<string.h>
#include "common.h"

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

Status get_file_size(FILE *fptr)
{
	fseek(fptr, -1, SEEK_END);
	int pos = ftell(fptr);
	rewind(fptr);
	return pos;
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
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");                             //open the source_file in read mode
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)                                                         //do the validation
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo -> src_image_fname);          

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");                                    //open the secret file in read mode
    // Do Error handling
    if (encInfo->fptr_secret == NULL)                                                            //fo the validation
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");                             //open the stego image in write mode
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)                                                          //do error handling
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}


/*read and validate fun definition*/
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)                            //read and validate
{
	if(strcmp(strchr(argv[2], '.'),".bmp") == 0)
	{
	unsigned char arr1[2], arr2[2];                                   				  //to hold first 2 bytes read from source_image file
	encInfo -> fptr_src_image = fopen(argv[2], "r");           				//source image file passed as a cla, in read mode, fopen returns ptr 
	fread(arr1, 1, 2, (encInfo -> fptr_src_image));             				//reads first 2 bytes from src_image into arr1
	
		if((arr1[0] == 0x42) && (arr1[1] == 0x4d))          				 //0x42=B, 0x4d=M
		{
			printf("It is bmp file\n");
			encInfo -> src_image_fname = argv[2];  					  //set src_image_fname in encinfo to argv[2], which is source file name
		}
		else
		{
			printf("It is not a bmp file\n");
			fclose(encInfo -> fptr_src_image);
			return e_failure;
		}
		fclose(encInfo -> fptr_src_image);
		
		encInfo -> secret_fname = argv[3];             					//set secret file name to argv[3]
		strcpy(encInfo -> extn_secret_file, strchr(argv[3],'.'));
		encInfo->sec_file_extn_size = strlen(encInfo->extn_secret_file);
	        printf("secret file extension is %s\n",encInfo -> extn_secret_file); 					//if argv[3] contains .txt file copy it to extern_secret file
				
	}
	else
	{
		printf("Output file is not a bmp file\n");
		return e_failure;
	}
		if(argv[4]!=NULL)
		{
		   if(strcmp(strchr(argv[4],'.'),".bmp") == 0)
		   {
			printf("Output file is .bmp file\n");
			encInfo->stego_image_fname = argv[4];
			
		   }
		   else
		   {
			printf("output file is not .bmp file\n");
			return e_failure;
		   }
		}
		else
		{
		   encInfo->stego_image_fname = "stego.bmp";
		   printf("%s\n",encInfo->stego_image_fname);
		}
		return e_success;                                                               //return e_success for all the validation passed successfully

}


/*check capacity fun definition*/
Status check_capacity(EncodeInfo *encInfo)
{
	printf("checking capacity of %s to hold %s", encInfo -> src_image_fname, encInfo -> secret_fname);
	encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);                      //finding image capacity to hold the data 
       // printf("The capacity of Image file is %d bytes\n",encInfo -> image_capacity);

	encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);
	printf("The size of the secret file is %ld bytes\n",(strlen(MAGIC_STRING)* 8) + 32 + (strlen(encInfo -> extn_secret_file) * 8) +32+ ((encInfo -> size_secret_file) * 8) + 54);

	if(((strlen(MAGIC_STRING) * 8) + 32 + (strlen(encInfo -> extn_secret_file) * 8) + 32 + ((encInfo -> size_secret_file) * 8) + 54) >= encInfo -> image_capacity) 
        {
		printf("More than image file\n");
		return e_failure;
	}
	else
	{
		printf("less than image size\n");
	}

}

/*copy bmp header file*/
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)           //copy the header part
{
	rewind(fptr_src_image);
	rewind(fptr_dest_image);
	char buffer[54];						     //create 1 buffer of char type
//	fseek(fptr_src_image, 0, SEEK_SET);                                  //set the curser at 1st position of src_image_bmp_file
	fread(buffer, sizeof(char), 54, fptr_src_image);                    //read  one one bytes 54 times from src_image bmp file
	fwrite(buffer,sizeof(char), 54,  fptr_dest_image);                  //write 54 bytes red from src_image bmp file into stego.bmp file(buffer)
	return e_success; 
}

/*encoding magic string*/
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)                                        //encode the magic string
{
	printf("Encoding magic string\n");
	int size = strlen(magic_string);                                                                   //get the length of magic string
	encode_data_to_image(magic_string, size, encInfo -> fptr_src_image, encInfo -> fptr_stego_image);  //call encode data to image function
	return e_success;
}

Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)             //encode the extn zise of secret file
{
	char str[32];                                                          //create 1 bufffer
	fread(str, 32, 1, encInfo -> fptr_src_image);                          //read 32 bytes ffrom source image
	encode_size_to_lsb(size, str);                                          //call the encode_size_to lsb function
	fwrite(str, 32, 1, encInfo -> fptr_stego_image);                        //write it into stego image
	return e_success;
}

/*encode data to image*/
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)             //encode data to image
{
	char buffer1[8];                                                                                //create 1 array
	for(int i = 0; i < size; i++)                                                                   //ru a loop upto size
	{
		fread(buffer1, 8, sizeof(char), fptr_src_image);                                        //read 8 bytes from source file store it in stego file
		encode_byte_to_lsb(data[i], buffer1);                                                    //by calling encode byte to lsb function
		fwrite(buffer1, 8, sizeof(char), fptr_stego_image);
	}
	return e_success;

}

/*encode byte to lsb*/
Status encode_byte_to_lsb(char data, char *image_buffer)                                       //encode byte to lsb side of image
{
	for(int i = 0; i < 8; i++)                                                             //run a loop from 0 to 8
	{
		image_buffer[i] = (image_buffer[i] & (0xFE)) | ((data >> (7 - i)) & 1);         //do the bitwise operation to encode byte to lsb and store in im buffer
	}
	return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)                       //encode secret file extension by calling encode_data_to image function
{
	encode_data_to_image(encInfo->extn_secret_file, strlen(encInfo->extn_secret_file), encInfo->fptr_src_image, encInfo->fptr_stego_image);
	return e_success;
}

Status encode_size_to_lsb(char data, char *image_buffer)                                            //encode the size to lsb
{
	for(int i = 0; i < 32; i++)                                                                 //run a loopfrom 0 to 32
	{
		image_buffer[i] = (image_buffer[i] & (0xFE)) | ((data >> (31 - i)) & 1);            //in image buffer's every index we have to store the encoded size
	}
	
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)                                 //encode secret file size
{
	char str1[32];                                                                              //create 1 buffer to store size
	fread(str1, 32, 1, encInfo -> fptr_src_image);                                              //read 32 bytes from source file
	encode_size_to_lsb(file_size, str1);                                                        //call encode_size_to_lsb function
	fwrite(str1, 32, 1, encInfo -> fptr_stego_image);                                           //write it into stego image file
	printf("loaded successfully\n"); 
}

Status encode_secret_file_data(EncodeInfo *encInfo)                                                 //encode data of secret file now
{
	long size = encInfo->size_secret_file;                                                      //declrae size and initialized with size secret file pointer
	char secret_data[encInfo->size_secret_file];                                                //create 1 array 
	char ch;
	int i = 0; 
	while((ch = fgetc(encInfo->fptr_secret)) != EOF)                                              //get one one byte from secret file upto EOF store in c
	{
		secret_data[i++] = ch;                                                               //everytime store it in array by post incrementing the index
	}
	secret_data[i] = '\0';                                                                        //at last index store null character to indicate end of the string
	encode_data_to_image(secret_data, strlen(secret_data),encInfo->fptr_src_image, encInfo -> fptr_stego_image);   //call the data_to_image function
	return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)                         //copy remaining data from source to destination file
{
	char ch;
	while(fread(&ch, 1, 1, fptr_src) > 0)                                          //read from source file upto equals to 0
	{
		fwrite(&ch, 1, 1, fptr_dest);                                          //write it into destination one byte by byte we are writing here
	}
	return e_success;
}

/*do_encoding fun*/
Status do_encoding(EncodeInfo *encInfo)                                                 //this is do encoding
{
{

	printf("---Encoding process started---\n");
}

	if(open_files(encInfo) == e_failure)                                           //fun call for open_files
	{
		printf("Error in opening file\n");
		return e_failure;
	}
	printf("checking %s file size\n",encInfo->secret_fname);
{

	if(check_capacity(encInfo) == 0)                                             //fun call  capacity 
	{
		printf("Error: Not having capacity");
		return e_failure;
	}
	else
	{
		printf("capacity is ther to store\n");
	}
}
	if(copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_failure)             //fun call for copy bmp header
	{
		printf("data from source file to stego file not copied\n");
       		return e_failure;
        }
	printf("Header copied successfully into stego.bmp file\n");

        if(encode_magic_string(MAGIC_STRING, encInfo) == e_failure)                       //fun call for encode magicstring 
	{
		printf("Error: not copied\n");
		return e_failure;
	}	
	printf("Encoded successfully\n");

        if(encode_secret_file_extn_size(encInfo -> sec_file_extn_size, encInfo) == e_failure)            //fun call for secret file extn size
	{
		printf("failed to encode secret file size\n");
		return e_failure;
	}
	
	if(encode_secret_file_extn((encInfo->extn_secret_file), encInfo) == e_failure)                 //fun call for secret file extn
	{
		
		printf("failed in encode secret file\n");
		return e_failure;
	}
	printf("Encoded successfully\n");

	if(encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure)                   //fun call for secret file size
	{
		return e_failure;
	}
	printf("Encoded successfully\n");

	if(encode_secret_file_data(encInfo) == e_failure)                                              //fun call for secret file data
	{
		return e_failure;
	}
	printf("Encoded Successfully\n");

	if(copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_failure)         //fun call for copy remaining image data
	{
		return e_failure;
	}
	printf("Remaining data encoded successfully\n");
	return e_success;                                                                            //if all validations done then return success
}



































