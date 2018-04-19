#include "fat32.h"



int main(int argc, char * argv[])
{
	char * commands [] ={"exit", "info", "ls", "cd", "size", "creat", "mkdir", "rm", "rmdir", "open" ,"close" , "read", "write"};
	char input[80];
	char * cmdline; 
	char * tokens[5];
	int num_toks = 0;
	char * temp;
	int i;
	int cmd = -1;
	struct FAT32BootBlock boot_sector;
	FILE * img_fp;



	img_fp = fopen(argv[1],"rw");
	if(img_fp == NULL)
	{
		printf("Image could not be opened\n");
		return 1;
	}

	printf("FAT32 cmdline->");
	fgets(input,79,stdin);
	cmdline = strtok(input,"\n");

	while(1) {

		if(!strcmp("exit",input))
			 break;
	
		/*Parse the input*/
		temp = strtok(cmdline, " ");
		while(temp != NULL )
		{
			tokens[num_toks++] = temp;
			temp = strtok(NULL," ");
		}
		/************************/

		//printf("%s ",tokens[0]);

		for (i =0; i < numcmd; i++)
			if(!strcmp(tokens[0],commands[i]))
				cmd = i;

		

		switch(cmd) {
			
		   case info:
		    getInfo(&boot_sector,img_fp);
		    break;

		   default:
		   break;
		}
		

		/* Get new tokens*/
		printf("FAT32 cmdline->");
		fgets(input,79,stdin);
		cmdline = strtok(input,"\n");	
	}


}