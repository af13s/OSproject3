#include "fat32.h"

extern struct FAT32BootBlock boot_sector;
extern FILE * img_fp;
extern char * commands [];


void printDir(struct FAT32BootBlock * boot_sector, FILE * img_fp)
{

	//int fds = boot_sector->reserved_sectors + (boot_sector->number_of_fats* boot_sector->fat_size_sectors);
	//int fsc = ((boot_sector->bpb_rootcluster -2)*boot_sector->sectors_per_cluster) + fds;
	//int x  = fseek(img_fp, fsc,SEEK_SET);
	//fread()
}

// returns FAT[clus_num]
unsigned int fatEntry(int clus_num)
{
	int val;
	int offset = boot_sector.reserved_sectors*boot_sector.sector_size + clus_num*sizeof(int);
	fseek(img_fp,offset,SEEK_SET);
	fread(&val,sizeof(int),1,img_fp);
	return val;
}


//Returns the First sector of the cluster passed in
int getFirstCSector(int clus_num)
{
	
	unsigned int FirstDataSector = boot_sector.reserved_sectors + (boot_sector.number_of_fats* boot_sector.bpb_FATSz32);
	unsigned int FirstSectorofCluster = ((clus_num - 2)*boot_sector.sectors_per_cluster) + FirstDataSector;
	FirstSectorofCluster = FirstSectorofCluster * boot_sector.sector_size;
	
	
	return FirstSectorofCluster;
}

char * formatname(char * name, int directory)
{	
	
	 char * formatted = strdup(name);

  	for(int i = 0; i < strlen(name); i++)
  	{
  		if (isspace(name[i]) && directory)
  			formatted[i] = '\0';
  		else if (isspace(name[i]) && !directory)
  		{
  			if (!isspace(name[i+1]) && (i < strlen(name) -1))
  				formatted[i] = '.';
  			else
  				formatted[i] = '\0';

  		}
  		else if (isalpha(name[i]))
  			formatted[i] = tolower(name[i]);
  		else
  			formatted[i] = name[i];
  	}

  	return formatted;
}




/*  main misc */

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


struct FAT32DirBlock getDirectoryEntry(unsigned int cluster_num, char * entry_name,int directory)
{
	int i = 0;
	struct FAT32DirBlock dblock;
	struct FAT32DirBlock empty = {0};

	unsigned int fat_val = fatEntry(cluster_num);
	unsigned int firstsector = getFirstCSector(cluster_num);

	unsigned int dentry_addr = firstsector;
	fseek(img_fp,dentry_addr,SEEK_SET);

	while(i*sizeof(struct FAT32DirBlock) < boot_sector.sector_size)
	{
		fread(&dblock,sizeof(struct FAT32DirBlock),1,img_fp);

		if(strcmp(formatname((char *)dblock.name,directory),entry_name) == 0)
			return dblock;

		i++;
	}


	if(fat_val != 0x0FFFFFF8 && fat_val != 0x0FFFFFFF && fat_val != 0x00000000 )
		return getDirectoryEntry(fat_val, entry_name, directory);

	return empty;
}


//return first empty cluster number
int findEmptyCluster()
{
	/*0,1are reserved*/
	unsigned int i = 2;
	for(i = 2; i < boot_sector.fat_size_sectors;i++)
	{
		if(!fatEntry(i))
			return i;
	}
	return -1;
}

