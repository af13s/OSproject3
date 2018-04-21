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
	}

 	/*
 	//printf("jmp %d\n",boot_sector->jmp);
	 	printf("reserved_sectors %d\n",boot_sector->reserved_sectors);
 	printf("root_dir_entries %d\n",boot_sector->root_dir_entries);
 	printf("total_sectors_short %d\n",boot_sector->total_sectors_short);
 	printf("media_descriptor %d\n",boot_sector->media_descriptor);
 	printf("fat_size_sectors %d\n",boot_sector->fat_size_sectors);
 	printf("sectors_per_track %d\n",boot_sector->sectors_per_track);
 	printf("number_of_heads %d\n",boot_sector->number_of_heads);
 	printf("hidden_sectors %d\n",boot_sector->hidden_sectors);
 	printf("total_sectors_long %d\n",boot_sector->total_sectors_long);

	printf("bpb_FATz32 %d\n",boot_sector->bpb_FATz32);
	printf("bpb_extflags %d\n",boot_sector->bpb_extflags);
	printf("bpb_fsver %d\n",boot_sector->bpb_fsver); 
	
	printf("volume_label %s\n",boot_sector->volume_label);
	printf("fs_type %s\n",boot_sector->fs_type);
	printf("boot_code %s\n",boot_sector->boot_code);
	printf("boot_sector_signature %d\n",boot_sector->boot_sector_signature);
	*/
}

int ls(int cluster_num)
{
	int i = 0;
	struct FAT32DirBlock dblock;
	unsigned int fatVal = fatEntry(cluster_num);
	unsigned int firstsector = getFirstCSector(cluster_num);
	
	unsigned int dentry_addr = firstsector;
	fseek(img_fp,dentry_addr,SEEK_SET);

	while(i*sizeof(struct FAT32DirBlock) < boot_sector.sector_size)
	{
		fread(&dblock,sizeof(struct FAT32DirBlock),1,img_fp);
		printf("%s\n",dblock.name);
		i++;
	}

	if(fatVal != 0x0FFFFFF8 && fatVal != 0x0FFFFFFF && fatVal != 0x00000000 )
	{
		ls(fatVal);
	}
	else
		return 1;

	return 1;
}
