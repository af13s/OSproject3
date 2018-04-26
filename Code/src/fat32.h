#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define EXIT 0
#define INFO 1
#define LS 2
#define CD 3
#define SIZE 4
#define CREAT 5
#define MKDIR 6
#define RM 7
#define RMDIR 8
#define OPEN 9
#define CLOSE 10
#define READ 11
#define WRITE 12

#define NUMCMD 13
#define BOOTVARNUM 1

#define DIRECTORY 16 //HEX x10


//File Modes
#define F_READ 1
#define F_WRITE 2
#define F_READWRITE 3

//file struct size
#define FILE_STRUCT_SIZE 1000



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

unsigned int bpb_FATSz32;
unsigned short bpb_extflags;
unsigned short bpb_fsver; 
unsigned int bpb_rootcluster;
char volume_label[11];
char fs_type[8];
char boot_code[436];
unsigned short boot_sector_signature;
}__attribute__((packed)); 

 struct FAT32DirBlock{
 	unsigned char garb[32];
	unsigned char  name[11];
	unsigned char Attr;
	unsigned char NTRes;
	unsigned char CrtTimeTenth;
	unsigned short CrtTime;
	unsigned short CrtDate;
	unsigned short LstAccDate;
	unsigned short FstClusHI;
	unsigned short WrtTime;
	unsigned short WrtDate;
	unsigned short FstClusLO;
	unsigned int   FileSize;

}__attribute__((packed));


 struct File {
 char * name ;
 int first_cluster_number;
 int mode; //file 

};


// main
int parseTokens(char ** tokens);
int isValidArg(int cmd, int argNum);
void getInfo(int boolean);
int getCmd(char ** tokens);
void prompt();


//part1
void ls(int cluster_num);
void ls_wrapper(int num_toks, int current_cluster, int original_cluster, char ** tokens);
unsigned int cd (unsigned int current_cluster, char * dirname);
struct FAT32DirBlock getDirectoryEntry(unsigned int cluster_num, char * dirname,int directory);

void size_wrapper(int current_cluster, char * token);
unsigned int size (unsigned int current_cluster, char * filename);


// misc
void error_msg(char * string);
void cmd_error_msg(int cmd,int num_toks,int isValid);
unsigned int getFirstCSector(int clus_num);
unsigned int fatEntry(int clus_num);
char * formatname(char * name,int);
void updateDirectoryEntry(struct FAT32DirBlock block, unsigned int cluster_num, int directory);


void open (char * filename, char * mode , int current_cluster);
void close (char * filename);

void read_wrapper (char * filename,int current_cluster,unsigned int offset, unsigned int sz);
void read(int current_cluster,int offset, int sz, char * string);void read(int current_cluster,int offset, int sz, char * string);

void write_wrapper (char * filename,int current_cluster,unsigned int offset, unsigned int sz, char * string);
void write (unsigned int current_cluster,unsigned int offset, unsigned int sz, char * string);



void writeDirectoryEntry(char * name, unsigned char attr, unsigned short HI, unsigned short LO, struct FAT32DirBlock * dblock);
void linkClusters(unsigned int parent_cluster, unsigned int child_cluster);
int findEmptyDirEntry(unsigned int current_cluster);
int findEmptyCluster();

void setFatIndex(unsigned int clus_num, unsigned int val);
void removeDirEntry(unsigned int current_cluster, char * entry_name,int directory);
void deleteCluster(unsigned int parent_cluster, unsigned int child_cluster);
void removeAllDirEntries(unsigned int current_cluster);
int emptyDirectory(unsigned int cluster);
//int LinkedEmptyCluster(unsigned int cluster, int dir_flag);
//part2
void mkdir(unsigned int current_cluster, char * dirname);
void creat(unsigned int current_cluster, char * filename);
void rmdir(unsigned int current_cluster, char * dirname);
void rm(unsigned int current_cluster, char * filename);


