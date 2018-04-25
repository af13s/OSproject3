#include "fat32.h"

extern struct FAT32BootBlock boot_sector;
extern FILE * img_fp;
extern char * commands [];

// returns FAT[clus_num]
unsigned int fatEntry(int clus_num)
{
	unsigned int val;
	unsigned int offset = boot_sector.reserved_sectors*boot_sector.sector_size + clus_num*sizeof(int);
	fseek(img_fp,offset,SEEK_SET);
	fread(&val,sizeof(int),1,img_fp);
	return val;
}


//Returns the First sector of the cluster passed in
unsigned int getFirstCSector(int clus_num)
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
			return(argNum == 3);
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
	unsigned int begining = boot_sector.reserved_sectors*boot_sector.sector_size;
	unsigned end = begining + boot_sector.bpb_FATSz32;
	int i;
	for(i=0; begining + i*(sizeof(int)) < end;i++)
	{
		if(!fatEntry(i))
			return i;
	}
	return -1;
}



//return the address of the emptry directory entry if found; or -1 
int findEmptyDirEntry(unsigned int current_cluster)
{
	
	int i = 0;
	struct FAT32DirBlock dblock;
	unsigned int fat_val = fatEntry(current_cluster);
	unsigned int firstsector = getFirstCSector(current_cluster);
	
	unsigned int dentry_addr = firstsector;
	fseek(img_fp,dentry_addr,SEEK_SET);
	int new_cluster;

	while(i*sizeof(struct FAT32DirBlock) < boot_sector.sector_size)
	{
		fread(&dblock,sizeof(struct FAT32DirBlock),1,img_fp);
		if (dblock.name[0] == 0x00)
			return firstsector + i*sizeof(struct FAT32DirBlock);
		
		i++;
	}



	if(fat_val != 0x0FFFFFF8 && fat_val != 0x0FFFFFFF)
		return findEmptyDirEntry(fat_val);
	// IF Full create a new cluster
	//and link to new cluster and return new cluster addr

	if(fat_val == 0x0FFFFFF8 || fat_val == 0x0FFFFFFF)
	{
		new_cluster = findEmptyCluster();
		linkClusters(current_cluster,new_cluster);
		return getFirstCSector(new_cluster);
	}
	
	return -1;
}



/* Links the parent cluster to the child cluster and sets the child
as the last cluster */
void linkClusters(unsigned int parent_cluster, unsigned int child_cluster)
{
	unsigned int last = 0x0FFFFFF8;
	setFatIndex(parent_cluster,child_cluster);
	setFatIndex(child_cluster,last);
}

/* Sets FAT[clus_num] = val */
void setFatIndex(unsigned int clus_num, unsigned int val)
{
	int offset = boot_sector.reserved_sectors*boot_sector.sector_size + clus_num*sizeof(unsigned int);
	fseek(img_fp,offset,SEEK_SET);
	fwrite(&val,sizeof(unsigned int),1,img_fp);
}

/*Writes to a FAT32DirBlock struct*/
void writeDirectoryEntry(char * name, unsigned char attr, unsigned short HI, unsigned short LO, struct FAT32DirBlock * dblock)
{
	memset(dblock,0,sizeof(struct FAT32DirBlock));
	strcpy((char *)dblock->name,name);
	dblock->Attr = attr;
	dblock->FstClusHI = HI;
	dblock->FstClusLO = LO;

}

/* Removes directory entry with name entry_name from the cluster   */
void removeDirEntry(unsigned int current_cluster, char * entry_name,int directory)
{
	int i = 0;
	unsigned int x = 0;
	struct FAT32DirBlock dblock;
	

	unsigned int fat_val = fatEntry(current_cluster);
	unsigned int firstsector = getFirstCSector(current_cluster);

	unsigned int dentry_addr = firstsector;
	fseek(img_fp,dentry_addr,SEEK_SET);

	while(i*sizeof(struct FAT32DirBlock) < boot_sector.sector_size)
	{
		fread(&dblock,sizeof(struct FAT32DirBlock),1,img_fp);

		if(strcmp(formatname((char *)dblock.name,directory),entry_name) == 0)
		{
			fseek(img_fp,dentry_addr+i*sizeof(struct FAT32DirBlock), SEEK_SET);
			fwrite(&x,sizeof(struct FAT32DirBlock),1,img_fp);
			return;
		}

		i++;
	}


	if(fat_val != 0x0FFFFFF8 && fat_val != 0x0FFFFFFF && fat_val != 0x00000000 )
		removeDirEntry(fat_val, entry_name,directory);

	
}

/* Deletes child cluster and sets the parent cluster as the last cluster*/
void deleteCluster(unsigned int parent_cluster, unsigned int child_cluster)
{
	unsigned int last = 0x0FFFFFF8;
	unsigned int zero = 0x00000000;
	setFatIndex(parent_cluster,last);
	setFatIndex(child_cluster,zero);
}

/*Removes all directory entries from the chain of clusters 
and deletes all clusters that are empty as a result*/
void removeAllDirEntries(unsigned int current_cluster)
{
	int i = 0;
	unsigned int x = 0;
	struct FAT32DirBlock dblock;


	unsigned int next_val = fatEntry(current_cluster);
	unsigned int firstsector = getFirstCSector(current_cluster);

	unsigned int dentry_addr = firstsector;
	fseek(img_fp,dentry_addr,SEEK_SET);

	if(next_val != 0x0FFFFFF8 && next_val != 0x0FFFFFFF)
		removeAllDirEntries(next_val);



	while(i*sizeof(struct FAT32DirBlock) < boot_sector.sector_size)
	{
		fread(&dblock,sizeof(struct FAT32DirBlock),1,img_fp);
		fseek(img_fp,dentry_addr+i*sizeof(struct FAT32DirBlock), SEEK_SET);
		fwrite(&x,sizeof(struct FAT32DirBlock),1,img_fp);
		i++;
	}

	deleteCluster(current_cluster,next_val);
	
}

/*
int LinkedEmptyCluster(unsigned int cluster, int dir_flag)
{
	unsigned int dir_addr = getFirstCSector(cluster);
	struct FAT32DirBlock dblock;
	int i = 0;
	if(dir_flag == 0)
		fseek(img_fp,dir_addr,SEEK_SET);
	else
	{
		i = 2;
		fseek(img_fp,dir_addr + 2*sizeof(struct FAT32DirBlock),SEEK_SET);
	}
	
	while(i*sizeof(struct FAT32DirBlock) < boot_sector.sector_size)
	{
		fread(&dblock,sizeof(struct FAT32DirBlock),1,img_fp);

		if(dblock.name[0] != 0x00)
		{
			return 0;
		}

		i++;
	}
	return 1;

}*/


int emptyDirectory(unsigned int cluster)
{
	int fat_val = fatEntry(cluster);
	int i = 0;
	struct FAT32DirBlock dblock;
	int result = 1;

	unsigned int dir_addr = getFirstCSector(cluster);
	
	fseek(img_fp,dir_addr,SEEK_SET);
	
	while(i*sizeof(struct FAT32DirBlock) < boot_sector.sector_size)
	{
		fread(&dblock,sizeof(struct FAT32DirBlock),1,img_fp);

		if (dblock.name[0] != 0x00 && dblock.name[0] != '.')
		{
			result = 0;
			return result;
		}
			
		i++;
	}

	if(fat_val == 0x0FFFFFF8 || fat_val == 0x0FFFFFFF )
		return result;

	
	return emptyDirectory(fat_val);
	
	
}


void updateDirectoryEntry(struct FAT32DirBlock block, unsigned int cluster_num, int directory)
{
	int i = 0;
	struct FAT32DirBlock temp_block;
	
	unsigned int fat_val = fatEntry(cluster_num);
	unsigned int firstsector = getFirstCSector(cluster_num);

	unsigned int dentry_addr = firstsector;
	fseek(img_fp,dentry_addr,SEEK_SET);

	while(i*sizeof(struct FAT32DirBlock) < boot_sector.sector_size)
	{
		fread(&temp_block,sizeof(struct FAT32DirBlock),1,img_fp);

		if(strcmp(formatname((char *)temp_block.name,directory),formatname((char *)block.name,directory)) == 0)
		{
			fseek(img_fp,dentry_addr + i*sizeof(struct FAT32DirBlock),SEEK_SET);
			fwrite(&block,sizeof(struct FAT32DirBlock),1,img_fp);
			return;		
		}

		i++;
	}


	if(fat_val != 0x0FFFFFF8 && fat_val != 0x0FFFFFFF && fat_val != 0x00000000 )
		updateDirectoryEntry(block, fat_val, directory);

	
}
