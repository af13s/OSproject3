typedef struct {
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
}__attribute((packed)) FAT32BootBlock; 

typedef struct {

	unsigned char  name[12];
	unsigned short Attr;
	unsigned short NTRes;
	unsigned short CrtTimeTenth;
	unsigned short CrtTime;
	unsigned short CrtDate;
	unsigned short LstAccDate;
	unsigned short FstClusHI;
	unsigned short WrtTime;
	unsigned short WrtDate;
	unsigned short FstClusLO;
	unsigned int   FileSize;

}__attribute((packed)) FAT32DirBlock;