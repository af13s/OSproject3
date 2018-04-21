#include "fat32.h"

void printDir(struct FAT32BootBlock * bblk, FILE * img_fp)
{

	//int fds = bblk->reserved_sectors + (bblk->number_of_fats* bblk->fat_size_sectors);
	//int fsc = ((bblk->bpb_rootcluster -2)*bblk->sectors_per_cluster) + fds;
	//int x  = fseek(img_fp, fsc,SEEK_SET);
	//fread()
}
