#include "fat32.h"

extern struct FAT32BootBlock boot_sector;
extern FILE * img_fp;

void mkdir(unsigned int current_cluster, char * dirname)
{

	
	int empty_dir_entry;
	struct FAT32DirBlock dblock, dblock2, dblock3, temp;
	int newDirCluster;

	temp = getDirectoryEntry(current_cluster,dirname,1);
	if(!strcmp(formatname(temp.name,1),dirname))
	{
		printf("\"%s\" ALREADY EXISTS\n", dirname);
		return;
	}


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


void creat(unsigned int current_cluster, char * filename)
{
	int empty_dir_entry;
	struct FAT32DirBlock dblock,temp;
	int newDirCluster;

	temp = getDirectoryEntry(current_cluster,filename,1);
	if(!strcmp(formatname(temp.name,1),filename))
	{
		printf("\"%s\" ALREADY EXISTS\n", filename);
		return;
	}

	empty_dir_entry = findEmptyDirEntry(current_cluster);
	newDirCluster = findEmptyCluster();
	setFatIndex(newDirCluster,0x0FFFFFF8);
	writeDirectoryEntry(filename,0x03,newDirCluster/0x100,newDirCluster%0x100,&dblock);
	dblock.FileSize = 0;
	fseek(img_fp,empty_dir_entry,SEEK_SET);
	fwrite(&dblock,sizeof(struct FAT32DirBlock),1,img_fp);
}


int rmdir(unsigned int current_cluster, char * dirname)
{
	struct FAT32DirBlock dblock = getDirectoryEntry(current_cluster,dirname,1);
	
	unsigned int dir_addr,dir_cluster;
	struct FAT32DirBlock temp;
	unsigned int x = 0;
	unsigned rm_cluster_num;
	int dotDirs = (!strcmp(".",dirname) || !strcmp("..",dirname) );
	if(!strcmp(formatname(dblock.name,1),dirname) && dblock.Attr == 0x10 && !dotDirs)
	{
		x = 0;
	    rm_cluster_num = dblock.FstClusHI*0x100 + dblock.FstClusLO;
		dir_addr = getFirstCSector(rm_cluster_num);
		// To the begining of third entry
		fseek(img_fp,dir_addr + 2*sizeof(struct FAT32DirBlock),SEEK_SET);
		fread(&temp,sizeof(struct FAT32DirBlock),1,img_fp);
		if(temp.name[0] == 0x00)
		{
			removeDirEntry(current_cluster,dirname,1);
			fseek(img_fp,dir_addr,SEEK_SET);
			fwrite(&x,sizeof(struct FAT32DirBlock),2,img_fp);
			setFatIndex(rm_cluster_num,x);
			return 1;
			
		}
		else
			printf("DIRECTORY NOT EMPTY\n");
	}
	else if(!strcmp(formatname(dblock.name,1),dirname) && dblock.Attr != 0x10)
		printf("\"%s\" NOT A DIRECTORY\n", dirname);
	else if(strcmp(formatname(dblock.name,1),dirname))
		printf("\"%s\" DOES NOT EXIST\n", dirname);
	
	return -1;
}

