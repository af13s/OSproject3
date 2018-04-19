#include <stdio.h>
#include <string.h>

#define exit 0
#define info 1
#define ls 2
#define cd 3
#define size 4
#define creat 5
#define mkdir 6
#define rm 7
#define rmdir 8
#define open 9
#define close 10
#define read 11
#define write 12

#define numcmd 13
#define BootVarNum 1




 struct FAT32BootBlock{
 unsigned char jmp[3];
 char oem[8];
 unsigned short sector_size;
 unsigned char sectors_per_cluster;
 unsigned short reserved_sectors;
 unsigned char number_of_fats; 
 unsigned short root_dir_entries;
 unsigned short total_sectors_short;
 unsigned char media_descriptor;
 unsigned short fat_size_sectors;
 unsigned short sectors_per_track;
 unsigned short number_of_heads;
 unsigned int hidden_sectors;
 unsigned int total_sectors_long;

unsigned int bpb_FATz32;
unsigned short bpb_extflags;
unsigned short bpb_fsver; 
unsigned int bpb_rootcluster;
char volume_label[11];
char fs_type[8];
char boot_code[436];
unsigned short boot_sector_signature;
}__attribute__((packed)); 

 struct FAT32DirBlock{

	unsigned char  name[12];
	unsigned char Attr;
	unsigned char NTRes;
	unsigned char CrtTimeTenth;
	unsigned char CrtTime;
	unsigned char CrtDate;
	unsigned char LstAccDate;
	unsigned char FstClusHI;
	unsigned char WrtTime;
	unsigned char WrtDate;
	unsigned char FstClusLO;
	unsigned int   FileSize;

}__attribute__((packed));

void getInfo(struct FAT32BootBlock * bblk,FILE * img_fp);