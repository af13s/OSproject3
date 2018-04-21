#include "fat32.h"

char * commands [] ={"exit", "info", "ls", "cd", "size", "creat", "mkdir", "rm", "rmdir", "open" ,"close" , "read", "write"};
struct FAT32BootBlock boot_sector;
FILE * img_fp;

int main(int argc, char * argv[])
{
	
	char * tokens[5];
	int num_toks = 0;
	int cmd = -1;
	unsigned int current_cluster;
	

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
		    ls(current_cluster);
		    break;

		    case CD:
		    current_cluster = cd(current_cluster, tokens[1]);
		}
	}






	return 0;
}



