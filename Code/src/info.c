#include "fat32.h"

void getInfo(struct FAT32BootBlock * bblk,FILE * img_fp, int boolean)
{	
	if (boolean)
		fread(bblk->jmp,sizeof(struct FAT32BootBlock),1,img_fp);

	if (!boolean)
	{
	 	printf("\noem:  %s\n",bblk->oem);
	 	printf("sector size: %d\n",bblk->sector_size);
	 	printf("Number of fats: %d\n",bblk->number_of_fats);
	 	printf("Root Clusters: %d\n\n",bblk->bpb_rootcluster);
	 	printf("sectors_per_cluster %d\n",bblk->sectors_per_cluster);
	}

 	/*
 	//printf("jmp %d\n",bblk->jmp);
	
 	printf("reserved_sectors %d\n",bblk->reserved_sectors);
 	printf("root_dir_entries %d\n",bblk->root_dir_entries);
 	printf("total_sectors_short %d\n",bblk->total_sectors_short);
 	printf("media_descriptor %d\n",bblk->media_descriptor);
 	printf("fat_size_sectors %d\n",bblk->fat_size_sectors);
 	printf("sectors_per_track %d\n",bblk->sectors_per_track);
 	printf("number_of_heads %d\n",bblk->number_of_heads);
 	printf("hidden_sectors %d\n",bblk->hidden_sectors);
 	printf("total_sectors_long %d\n",bblk->total_sectors_long);

	printf("bpb_FATz32 %d\n",bblk->bpb_FATz32);
	printf("bpb_extflags %d\n",bblk->bpb_extflags);
	printf("bpb_fsver %d\n",bblk->bpb_fsver); 
	
	printf("volume_label %s\n",bblk->volume_label);
	printf("fs_type %s\n",bblk->fs_type);
	printf("boot_code %s\n",bblk->boot_code);
	printf("boot_sector_signature %d\n",bblk->boot_sector_signature);
	*/
}

