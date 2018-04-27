#include "fat32.h"

extern struct FAT32BootBlock boot_sector;
extern FILE * img_fp;

void getInfo(int boolean)
{	
	if (boolean)
		fread(&boot_sector,sizeof(struct FAT32BootBlock),1,img_fp);
	
	if (!boolean)
	{
	 	printf("\noem:  %s\n",boot_sector.oem);
	 	printf("sector size: %d\n",boot_sector.sector_size);
	 	printf("Number of fats: %d\n",boot_sector.number_of_fats);
	 	printf("Root Cluster Num: %d\n\n",boot_sector.bpb_rootcluster);
	 	printf("sectors_per_cluster %d\n",boot_sector.sectors_per_cluster);
	 	printf("root_dir_entries %d\n",boot_sector.root_dir_entries);
	 	printf("fat_size_sectors %d\n",boot_sector.fat_size_sectors);
	}
}

void ls(int cluster_num)
{
	int i = 0;
	struct FAT32DirBlock dblock;
	unsigned int fat_val = fatEntry(cluster_num);
	unsigned int firstsector = getFirstCSector(cluster_num);
	
	unsigned int dentry_addr = firstsector;
	fseek(img_fp,dentry_addr,SEEK_SET);

	
	while(i*sizeof(struct FAT32DirBlock) < boot_sector.sector_size)
	{
		fread(&dblock,sizeof(struct FAT32DirBlock),1,img_fp);

		if (dblock.name[0] == 0x00)
		{
			i++;
			continue;
		}
			

		if (dblock.Attr == DIRECTORY) 
			printf("%s\n",formatname((char *)dblock.name,DIRECTORY));
		else
			printf("%s\n",formatname((char *)dblock.name,!DIRECTORY));

		i++;
	}


	if(fat_val != 0x0FFFFFF8 && fat_val != 0x0FFFFFFF && fat_val != 0x00000000 )
	{
		ls(fat_val);
	}
}

unsigned int cd (unsigned int current_cluster, char * dirname)
{
	struct FAT32DirBlock dblock = getDirectoryEntry(current_cluster,dirname,DIRECTORY);
	if (!strcmp(formatname((char *)dblock.name,DIRECTORY),dirname) && (dblock.Attr == DIRECTORY))	
	{
		if (dblock.FstClusHI*0x100 + dblock.FstClusLO == 0)
			return boot_sector.bpb_rootcluster;

		return dblock.FstClusHI*0x100 + dblock.FstClusLO;
	}
	else
	{
		error_msg("Directory not found");
		return current_cluster;
	}

}

unsigned int size (unsigned int current_cluster, char * filename)
{
	struct FAT32DirBlock dblock = getDirectoryEntry(current_cluster,filename,!DIRECTORY);
	if (!strcmp(formatname((char *)dblock.name,!DIRECTORY),filename) && !(dblock.Attr == DIRECTORY))	
	{
		return dblock.FileSize + 1;
	}
	

	return 0;
}
