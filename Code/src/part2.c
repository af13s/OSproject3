#include "fat32.h"

extern struct FAT32BootBlock boot_sector;
extern FILE * img_fp;

void mkdir(unsigned int current_cluster, char * dirname)
{
	
	int empty_dir_entry;
	struct FAT32DirBlock dblock, dblock2, dblock3;
	int newDirCluster;
	empty_dir_entry = findEmptyDirEntry(current_cluster);
	newDirCluster = findEmptyCluster();
	setFatIndex(newDirCluster,0x0FFFFFF8);
	writeDirectoryEntry(dirname,0x10,newDirCluster/0x100,newDirCluster%0x100,&dblock);
	fseek(img_fp,empty_dir_entry,SEEK_SET);
	fwrite(&dblock,sizeof(struct FAT32DirBlock),1,img_fp);

	/*Now write . and .. entries for new cluster*/

	writeDirectoryEntry(".",0x10,newDirCluster/0x100,newDirCluster%0x100,&dblock2);
	empty_dir_entry = findEmptyDirEntry(newDirCluster);
	fseek(img_fp,empty_dir_entry,SEEK_SET);
	fwrite(&dblock2,sizeof(struct FAT32DirBlock),1,img_fp);

	writeDirectoryEntry("..",0x10,current_cluster/0x100,current_cluster%0x100,&dblock3);
	empty_dir_entry = findEmptyDirEntry(newDirCluster);
	fseek(img_fp,empty_dir_entry,SEEK_SET);
	fwrite(&dblock3,sizeof(struct FAT32DirBlock),1,img_fp);
	

}

