#include "fat32.h"

extern struct FAT32BootBlock boot_sector;
extern FILE * img_fp;
extern struct File files [];

void mkdir(unsigned int current_cluster, char * dirname)
{

	
	int empty_dir_entry;
	struct FAT32DirBlock dblock, dblock2, dblock3, temp;
	int newDirCluster;

	temp = getDirectoryEntry(current_cluster,dirname,1);
	if(!strcmp(formatname((char *)temp.name,1),dirname))
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
	if(!strcmp(formatname((char *)temp.name,1),filename))
	{
		printf("\"%s\" ALREADY EXISTS\n", filename);
		return;
	}

	empty_dir_entry = findEmptyDirEntry(current_cluster);
	newDirCluster = findEmptyCluster();
	setFatIndex(newDirCluster,0x0FFFFFF8);
	writeDirectoryEntry(filename,0x20,newDirCluster/0x100,newDirCluster%0x100,&dblock);
	dblock.FileSize = 0;
	
	fseek(img_fp,empty_dir_entry,SEEK_SET);
	fwrite(&dblock,sizeof(struct FAT32DirBlock),1,img_fp);
}




void rmdir(unsigned int current_cluster, char * dirname)
{
	struct FAT32DirBlock dblock = getDirectoryEntry(current_cluster,dirname,1);
	
	unsigned int dir_addr;
	unsigned int x = 0;
	unsigned int rm_cluster_num;
	int dotDirs = (!strcmp(".",dirname) || !strcmp("..",dirname) );
	if(!strcmp(formatname((char *)dblock.name,1),dirname) && dblock.Attr == 0x10 && !dotDirs)
	{
		x = 0;
	    
	    rm_cluster_num = dblock.FstClusHI*0x100 + dblock.FstClusLO;
		dir_addr = getFirstCSector(rm_cluster_num);
		
		if(emptyDirectory(rm_cluster_num))
		{
			removeDirEntry(current_cluster,dirname,1);
			fseek(img_fp,dir_addr,SEEK_SET);
			fwrite(&x,sizeof(struct FAT32DirBlock),2,img_fp);
			setFatIndex(rm_cluster_num,x);
			return;
			
		}
		else
			printf("DIRECTORY NOT EMPTY\n");
	}
	else if(!strcmp(formatname((char *)dblock.name,1),dirname) && dblock.Attr != 0x10)
		printf("\"%s\" NOT A DIRECTORY\n", dirname);
	else if(strcmp(formatname((char *)dblock.name,1),dirname))
		printf("\"%s\" DOES NOT EXIST\n", dirname);
	
}

void rm(unsigned int current_cluster, char * filename)
{

	struct FAT32DirBlock dblock = getDirectoryEntry(current_cluster,filename,1);
	int i; 
	unsigned int rm_cluster_num;
	if(!strcmp(formatname((char *)dblock.name,1),filename) && dblock.Attr != 0x10)
	{
		rm_cluster_num = dblock.FstClusHI*0x100 + dblock.FstClusLO;
		removeAllDirEntries(rm_cluster_num);
		removeDirEntry(current_cluster,filename,0);
		setFatIndex(rm_cluster_num,0x00000000);

		for (i = 0 ; i < FILE_STRUCT_SIZE; i++)
		{
			if (files[i].name == NULL)
				continue;
				
			if (!strcmp(filename,files[i].name))
			{
				files[i].name = NULL;
				files[i].first_cluster_number = 0;
				files[i].mode = 0;
				return;
			}

		}

	}

}
