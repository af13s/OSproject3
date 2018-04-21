#include "fat32.h"

char * commands [] ={"exit", "info", "ls", "cd", "size", "creat", "mkdir", "rm", "rmdir", "open" ,"close" , "read", "write"};
struct FAT32BootBlock boot_sector;
FILE * img_fp;

void prompt()
{
	printf("FAT32 cmdline->");
}

int parseTokens(char ** tokens)
{
	char * cmdline;
	char input[80];
	char * temp;
	int num_toks = 0;

	fgets(input,79,stdin);
	cmdline = strtok(input,"\n");

	/*Parse the input*/
		temp = strtok(cmdline, " ");
		while(temp != NULL )
		{
			tokens[num_toks++] = strdup(temp);
			temp = strtok(NULL," ");
		}

	return num_toks;
}

void error_msg(int cmd,int num_toks)
{

}

int isValidArg(int cmd, int argNum)
{
	switch(cmd)
	{

		case INFO:
			return (argNum == 1);
		case LS:
			return(argNum == 2 || argNum == 1);
		case CD:
			return(argNum == 2);
		case SIZE:
			return(argNum == 2);
		case CREAT:
			return(argNum == 2);
		case MKDIR:
			return(argNum == 2);
		case RM:
			return(argNum == 2);
		case RMDIR:
			return(argNum == 2);
		case OPEN:
			return(argNum == 2);
		case CLOSE:
			return(argNum == 2);
		case READ:
			return(argNum == 4);
		case WRITE:
			return(argNum == 5);
	}

	return 0;
}

int getCmd(char ** tokens)
{
	int i;
	int cmd = -1;

	for (i =0; i < NUMCMD; i++)
		if(!strcmp(tokens[0],commands[i]))
		{
			cmd = i;
			break;
		}
	return cmd;
}

int main(int argc, char * argv[])
{
	
	char * tokens[5];
	int num_toks = 0;
	int cmd = -1;

	img_fp = fopen(argv[1],"rw");
	if(img_fp == NULL)
	{
		printf("Image could not be opened\n");
		return 1;
	}

	getInfo(1);

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
		    ls(boot_sector.bpb_rootcluster);
		    break;
		}
	}

	return 0;
}



