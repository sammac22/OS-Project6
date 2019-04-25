
#include "fs.h"
#include "disk.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <math.h>

#define FS_MAGIC           0xf0f03410
#define INODES_PER_BLOCK   128
#define POINTERS_PER_INODE 5
#define POINTERS_PER_BLOCK 1024

struct fs_superblock {
	int magic;
	int nblocks;
	int ninodeblocks;
	int ninodes;
};

struct fs_inode {
	int isvalid;
	int size;
	int direct[POINTERS_PER_INODE];
	int indirect;
};

union fs_block {
	struct fs_superblock super;
	struct fs_inode inode[INODES_PER_BLOCK];
	int pointers[POINTERS_PER_BLOCK];
	char data[DISK_BLOCK_SIZE];
};

int fs_format()
{
  union fs_block block;
  int size = disk_size();
	disk_read(0,block.data);
  block.super.magic = FS_MAGIC;
  block.super.nblocks = size;
  block.super.ninodeblocks = (int) ceil(size * 0.1);
  block.super.ninodes = INODES_PER_BLOCK * block.super.ninodeblocks;
  disk_write(0,block.data);

  for(int i = 1; i < size; i++){
    disk_read(i,block.data);
    for(int j = 0; j < INODES_PER_BLOCK; j++){
      if(block.inode[j].isvalid == 1){
        block.inode[j].isvalid = 0;
      }
    }
    disk_write(i,block.data);
  }

	return 1;
}

void fs_debug()
{
	union fs_block block;

	disk_read(0,block.data);

	printf("superblock:\n");
  if(block.super.magic == FS_MAGIC){
    printf("    magic number is valid\n");
  } else {
    printf("    magic number is not valid\n");
  }
	printf("    %d blocks\n",block.super.nblocks);
	printf("    %d inode blocks\n",block.super.ninodeblocks);
	printf("    %d inodes\n",block.super.ninodes);

  int max = block.super.nblocks;

  for(int i = 1; i < max; i++){
    disk_read(i,block.data);
    for(int j = 0; j < INODES_PER_BLOCK; j++){
      if(block.inode[j].isvalid == 1){
        printf("inode: %d\n",j);
        printf("    size: %d bytes\n", block.inode[j].size);
        printf("    direct blocks: ");
        for(int k = 0; k < POINTERS_PER_INODE; k++){
          if(block.inode[j].direct[k] != 0){
            printf("%d ",block.inode[j].direct[k]);
          }
        }
        printf("\n");
        if(block.inode[j].indirect != 0){
          printf("    indirect: %d\n",block.inode[j].indirect);
        }
      }
    }
  }
}

int fs_mount()
{
	return 0;
}

int fs_create()
{
	return -1;
}

int fs_delete( int inumber )
{
	return 0;
}

int fs_getsize( int inumber )
{
	return -1;
}

int fs_read( int inumber, char *data, int length, int offset )
{
	return 0;
}

int fs_write( int inumber, const char *data, int length, int offset )
{
	return 0;
}
