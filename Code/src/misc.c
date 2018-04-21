#include "fat32.h"

extern struct FAT32BootBlock boot_sector;
extern FILE * img_fp;


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

