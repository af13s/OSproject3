#include "fat32.h"

char * commands [] ={"exit", "info", "ls", "cd", "size", "creat", "mkdir", "rm", "rmdir", "open" ,"close" , "read", "write"};
struct FAT32BootBlock boot_sector;
FILE * img_fp;
struct File files [FILE_STRUCT_SIZE] = {0};

int main(int argc, char * argv[])
{
	
	char * tokens[5];
	int num_toks = 0;
	int cmd = -1;
	unsigned int current_cluster;
	unsigned char original_cluster;



	//  
	img_fp = fopen(argv[1],"rb+");
	if(img_fp == NULL)
	{
		printf("Image could not be opened\n");
		return 1;
	}

	//get boot sector information
	getInfo(1);
	current_cluster = boot_sector.bpb_rootcluster;
	original_cluster = current_cluster;

	while(1) 
	{

		prompt();
		num_toks = parseTokens(tokens);
		cmd = getCmd(tokens);

		if (cmd == 0) break;
		
		if (!isValidArg(cmd, num_toks))
		{
			error_msg(cmd,num_toks);
			continue;
		}

		// EXECUTE COMMAND //
		switch(cmd)
		{   
		   	case INFO:
		    getInfo(0);
		    break;

		    case LS:
		    ls_wrapper(num_toks,current_cluster,original_cluster,tokens);
		    break;

		    case CD:
		    current_cluster = cd(current_cluster, tokens[1]);
		    break;

		    case SIZE:
		    size_wrapper(current_cluster,tokens[1]);
		    break;

		    case OPEN:
		    open (tokens[1], tokens[2], current_cluster);
		    break;

		    case CLOSE:
		    close (tokens[1],current_cluster);
		    break;

		    case READ:
		    read(tokens[1],current_cluster, atoi(tokens[2]), atoi(tokens[3]));
		    break;

		}
	}






	return 0;
}





void ls_wrapper(int num_toks, int current_cluster, int original_cluster, char ** tokens)
{
	if (num_toks == 1)
		    	ls(current_cluster);
		    else
		    {
		    	if ( cd(current_cluster, tokens[1]) != current_cluster)
		    	{
			    	original_cluster = current_cluster;
			    	current_cluster = cd(current_cluster, tokens[1]);
			    	ls(current_cluster);
			    	current_cluster = original_cluster;
			    }
			    //else error_msg();
		    }
}

void size_wrapper(int current_cluster, char * token)
{
	unsigned int sz = size(current_cluster,token);
	if (sz > 0)
		printf("file size of %s: %d bytes\n\n" , token, sz);
	//else error: not found
}


