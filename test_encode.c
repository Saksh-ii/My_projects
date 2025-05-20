#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "Decode.h"
#include <string.h>

int main(int argc, char *argv[])                                                                      //main function by passing argc and argv array
{
     EncodeInfo encInfo;                                                                             //take 1 structure member
     int ret = check_operation_type(argv);                                                           //call check operation type and store it in ret 
     if(ret == e_encode)                                                                             //if ret is equals e_encode
    {
	 printf("--Encoding operation is selected--\n");                                             //encode operation is selected
	 if(argc >= 4)                                                                               //if argc exceeds or equals to 4
	{
	    int res = read_and_validate_encode_args(argv, &encInfo);                                //call read and validate encode args and store it in res
	    {
		 if(res == e_failure)                                                                //if res == failure
		 {
		    printf("read and validate failed\n");                                          //read and validate failed
		    return 0;
		 }
		 else
		    printf("success\n");                                                         //otherwise success
	    }
	    if(do_encoding(&encInfo) == e_failure)                                                 //validate the do_encoding , if it's failure
	    {
		 printf("encoding failed\n");                                                       //encoding failed
		 return 0;
	    }
	   else
	    {
		 printf("Encoding successfull\n");                                                 //otherwise encoding successfull
	    }
	}
	else
	   printf("insufficient arguments\n");                                                    //otherwise insufficient arguments
	}
	else if(ret == e_decode)                                                                  //if it is e_decode
        {
		DecodeInfo decInfo;
		printf("--Decoding operation is selected--\n");                                    
		if(argc >= 3)                                                                     //if argc is equals to 
		{ 
			int res = read_and_validate_decode_args(argv, &decInfo);                  //call and validate read and validate decode args
			if(res == e_failure)
			{
				printf("Read and validate failed and decoding cannot be done\n");
				return 0;
		        }
			else if(res == e_success)
			{
				int rett = do_decoding(&decInfo);                                   //also call do_encoding and validate
			        if(rett == e_failure)
			        {
					printf("Decoding failed\n");
					return 0;
			        }
				else
					printf("Decoding successfully\n");
		         }
	        }
		else
		{
		  printf("insufficient arguments\n");
		  return 0;
		}
	}
     else
     {
	     printf("Unsupported format\n");
     
     }
     return 0;
}
    
/*To check operation type*/
OperationType check_operation_type(char *argv[])             //fun definition for check_operation_type passing char*argv[]
{
	    if(strcmp(argv[1], "-e") == 0)                //if this comparision is true
		    return e_encode;                       //then return as encode operation
	    
	    else if(strcmp(argv[1], "-d") == 0)             //if this is true
		    return e_decode;                        //return as decode operation
	    else
		    return e_unsupported;                    //if both fails return as unsupported
}






