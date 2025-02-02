#include "fat32.h"

char * commands [] ={"exit", "info", "ls", "cd", "size", "creat", "mkdir", "rm", "rmdir", "open" ,"close" , "read", "write"};
struct FAT32BootBlock boot_sector;
FILE * img_fp;
struct File files [FILE_STRUCT_SIZE] = {{0}};

int main(int argc, char * argv[])
{
	
	char * tokens[5];
	int num_toks = 0;
	int cmd = -1;
	unsigned int current_cluster;
	unsigned char original_cluster;


	/* open image file*/  
	img_fp = fopen(argv[1],"rb+");

	if(img_fp == NULL)
	{
		error_msg("Image could not be opened");
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
		if(num_toks == 0)
			continue;
		cmd = getCmd(tokens);

		if (cmd == 0) break;
		
		if (!isValidArg(cmd, num_toks))
		{
			cmd_error_msg(cmd,num_toks,isValidArg(cmd, num_toks));
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
		   		close (tokens[1]);
		    	break;

		    case READ:
		   		read_wrapper(tokens[1],current_cluster,atoi(tokens[2]), atoi(tokens[3]));
		    	break;

		    case WRITE:
		   		write_wrapper(tokens[1],current_cluster,atoi(tokens[2]), atoi(tokens[3]), tokens[4]);
		    	break;

		    case MKDIR:
		    	mkdir(current_cluster,strtolower(tokens[1]));
		    	break;
		    case CREAT:
		    	creat(current_cluster,strtolower(tokens[1]));
		    	break;
		    case RMDIR:
		    	rmdir(current_cluster,tokens[1]);
		    	break;
		    case RM:
		    	rm(current_cluster,tokens[1]);
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
    	if (!strcmp(tokens[1],"."))
    	{
    		ls(current_cluster);
    		return;
    	}
    	if ( cd(current_cluster, tokens[1]) != current_cluster)
    	{
	    	original_cluster = current_cluster;
	    	current_cluster = cd(current_cluster, tokens[1]);
	    	ls(current_cluster);
	    	current_cluster = original_cluster;
	    }
	    else
	    	error_msg("Not a directory");
    }
}

void size_wrapper(int current_cluster, char * token)
{
	int sz = size(current_cluster,token);
	if (sz >= 0)
		printf("file size of %s: %d bytes\n\n" , token, (sz-1));
	else
		error_msg("Not a file");
}

void read_wrapper (char * filename,int current_cluster,unsigned int offset, unsigned int sz)
{
	int i;
	int starting_cluster;
	int n;

	char * string = (char * )calloc(sz+1, sizeof(char));
	struct FAT32DirBlock dblock;
	dblock =  getDirectoryEntry(current_cluster, filename,!DIRECTORY);

	if (strcmp(formatname((char *)dblock.name,!DIRECTORY),filename))
	{
		 error_msg("file does not exist");
		return;
	}

	for (i = 0 ; i < FILE_STRUCT_SIZE; i++)
	{
		if (files[i].name == NULL)
			continue;

		if (!strcmp(filename,files[i].name) && (files[i].mode == F_READ || files[i].mode == F_READWRITE))
		{

			if (offset > dblock.FileSize)
			{
				error_msg("Offset larger than File Size");
				return;

			}

			if ( sz > dblock.FileSize || (sz+offset) > dblock.FileSize)
			{
				sz = (dblock.FileSize - offset);
				free(string);
				string = (char * )calloc(sz+1, sizeof(char));
			}

			starting_cluster = files[i].first_cluster_number;

			n = offset/boot_sector.sector_size;

			while (n != 0)
			{
				if (starting_cluster == 0x0000000 || starting_cluster == 0xFFFFFFF8)
				{
					error_msg("error accessing file data location");
					return; 
				}

				starting_cluster = fatEntry(starting_cluster);
				n--;
			}

			//starting cluster now represents the nth cluster that we need to start reading at

			read(starting_cluster, (offset % boot_sector.sector_size), sz, string);

			break; // file found performed operations and ending loop
		}
	}

	if(i == FILE_STRUCT_SIZE)
		error_msg("file not opened for reading");
	else
		printf("read->%s\n\n", string);
}

void write_wrapper (char * filename,int current_cluster,unsigned int offset, unsigned int sz, char * string)
{
	int i;
	int starting_cluster;
	int n;
	struct FAT32DirBlock dblock;
	
	char * newstring = strdup(string);

	
	if (strlen(string) <= sz)
	{
		newstring = (char *)calloc(sz, sizeof(char));
		strcpy(newstring,string);
	}
	
	dblock =  getDirectoryEntry(current_cluster, filename,!DIRECTORY);

	if (strcmp(formatname((char *)dblock.name,!DIRECTORY),filename))
	{
		error_msg("file does not exist");
		return;
	}

	for ( i = 0 ; i < FILE_STRUCT_SIZE; i++)
	{
		if (files[i].name == NULL)
			continue;

		if (!strcmp(filename,files[i].name) && (files[i].mode == F_WRITE || files[i].mode == F_READWRITE))
		{
			if (offset > dblock.FileSize)
			{
				error_msg("Offset larger than File Size");
				return;
			}

			if(offset + sz > dblock.FileSize)
			{
				dblock.FileSize = offset + sz;
				dblock.Attr = 0x20;
				updateDirectoryEntry(dblock,current_cluster,!DIRECTORY);
			}

			starting_cluster = files[i].first_cluster_number;

			n = offset/boot_sector.sector_size;

			while (n != 0)
			{
				if (starting_cluster == 0x0000000 || starting_cluster == 0xFFFFFFF8)
				{
					error_msg("error writing file data");
					return;
				}

				starting_cluster = fatEntry(starting_cluster);
				n--;
			}

			//starting cluster now represents the nth cluster that we need to start reading at

			write(starting_cluster, (offset % boot_sector.sector_size), sz, newstring);

			break; // file found performed operations and ending loop
		}
	}
	if(i == FILE_STRUCT_SIZE)
		error_msg("file not opened for writing");
	free(newstring);
}

char * strtolower(char * str)
{
	for(int i = 0; i < strlen(str); i++)
	{
  		str[i] = tolower(str[i]);
	}

	return str;
}







