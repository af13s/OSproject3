#include "fat32.h"

extern struct File files [];
extern FILE * img_fp;
/*Part 10: open FILENAME MODE
Opens a file named FILENAME in the current working directory. A file can only be read from or
written to if it is opened first.

You will need to maintain a table of opened files
add FILENAME to it when open is called

MODE is a string and is only valid if it is one of the following:
• r – read-only
• w – write-only
• rw – read and write
• wr – write and read

Print an error if the file is already opened, if the file does not exist, or an invalid mode is used.
*/

void open (char * filename, char * mode , int current_cluster)
{
	struct File file;
	struct FAT32DirBlock dblock;
	int md;

	if (!strcmp(mode ,"read-only"))
		md = F_READ;
	else if (!strcmp(mode ,"write-only"))
		md = F_WRITE;
	else if (!strcmp(mode ,"read and write") || !strcmp(mode ,"write and read"))
		md = F_READWRITE;
	else
		//mode not valid
		return;


	dblock =  getDirectoryEntry(current_cluster, filename,!DIRECTORY);

	if (!strcmp(formatname((char *)dblock.name,!DIRECTORY),filename) && !(dblock.Attr == DIRECTORY))
	{
		file.name = strdup(filename);
		file.first_cluster_num = getFirstCSector(dblock.FstClusHI*0x100 + dblock.FstClusLO);
		file.mode = md;
	}
	else
		return;
	 // return error file does not exist or not a file
		

	for (int i = 0 ; i < FILE_STRUCT_SIZE; i++ )
	{
		if ( files[i].name != NULL && !strcmp(filename,files[i].name))
		{
			 // error already in the open file table
			return;
		}
	}

	for (int i = 0 ; i < FILE_STRUCT_SIZE; i++)
	{
		if (files[i].name == NULL)
		{
			files[i] = file;
			printf("added %s to open file table in read mode %d\n\n" , files[i].name , files[i].mode);
			break;
		}
	}

	// no space for allocation
}



/*Part 11: close FILENAME
Closes a file named FILENAME. Needs to remove the file entry from the open file table.

Print an error if the file is not opened, or if the file does not exist.*/

void close (char * filename, int current_cluster)
{
	struct FAT32DirBlock dblock;

	dblock =  getDirectoryEntry(current_cluster, filename,!DIRECTORY);

	if (strcmp(formatname((char *)dblock.name,!DIRECTORY),filename))
		//error msg file does not exist
		return;
	else if (dblock.Attr == DIRECTORY)
		//not a file
		return;

	for (int i = 0 ; i < FILE_STRUCT_SIZE; i++)
	{
		if (files[i].name == NULL)
			continue;

		if (!strcmp(filename,files[i].name))
		{
			printf("closing file: %s\n\n" , files[i].name);
			files[i].name = NULL;
			files[i].first_cluster_num = 0;
			files[i].mode = 0;
			break;
		}
	}

	 //error_msgs file not open
}





/*Part 12: read FILENAME OFFSET SIZE
Read the data from a file in the current working directory with the name FILENAME. Start reading
from the file at OFFSET bytes and stop after reading SIZE bytes.

If the OFFSET+SIZE is larger than the size of the file, just read size-OFFSET bytes starting at OFFSET.

Print an error if FILENAME does not exist, if FILENAME is a directory, if the file is not opened for
reading, or if OFFSET is larger than the size of the file.*/


void read (char * filename,int current_cluster,int offset, int sz)
{
	char string [sz];
	struct FAT32DirBlock dblock;
	dblock =  getDirectoryEntry(current_cluster, filename,!DIRECTORY);

	if (strcmp(formatname((char *)dblock.name,!DIRECTORY),filename))
		//file does not exist
		return;

	for (int i = 0 ; i < FILE_STRUCT_SIZE; i++)
	{
		if (files[i].name == NULL)
			continue;

		if (!strcmp(filename,files[i].name) && (files[i].mode == F_READ || files[i].mode == F_READWRITE))
		{
			if (size(current_cluster, files[i].name) >= (offset+sz))
			{
				fseek(img_fp,files[i].first_cluster_num,SEEK_SET);
				fread(string,sizeof(string),1,img_fp);
				break;
			}

		}
	}

	printf("read: %s\n\n",string);
	// else //error on content didnt perform read
}


/*Part 13: write FILENAME OFFSET SIZE STRING
Writes to a file in the current working directory with the name FILENAME. Start writing at OFFSET
bytes and stop after writing SIZE bytes.

If OFFSET+SIZE is larger than the size of the file, you will
need to extend the length of the file to at least hold the data being written.

You will write STRING at this position. 

If STRING is larger than SIZE, write only the first SIZE
characters of STRING. 

If STRING is smaller than SIZE, write the remaining characters as ASCII 0
(null characters).

Print an error if FILENAME does not exist, if FILENAME is a directory, or if the file is not opened for
writing.*/

int write (char * filename, int offset, int size, char * string)
{
	return 0;
}