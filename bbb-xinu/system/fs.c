#include <xinu.h>
#include <kernel.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>


#if FS
#include <fs.h>

static struct fsystem fsd;
int dev0_numblocks;
int dev0_blocksize;
char *dev0_blocks;

extern int dev0;

char block_cache[512];

#define SB_BLK 0
#define BM_BLK 1
#define RT_BLK 2

#define NUM_FD 16
struct filetable oft[NUM_FD];
int next_open_fd = 0;


#define INODES_PER_BLOCK (fsd.blocksz / sizeof(struct inode))
#define NUM_INODE_BLOCKS (( (fsd.ninodes % INODES_PER_BLOCK) == 0) ? fsd.ninodes / INODES_PER_BLOCK : (fsd.ninodes / INODES_PER_BLOCK) + 1)
#define FIRST_INODE_BLOCK 2

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock);

/* YOUR CODE GOES HERE */
/* file and directory functions */
int fs_create(char *filename, int mode){
	if (mode == O_CREAT){
		int i;
		int opendirent = -1;
		for(i=0;i<DIRECTORY_SIZE;i++){

			if(strcmp(filename,fsd.root_dir.entry[i].name)== 0) {
				printf("A file already exists with this name. Please choose a different name");
				return SYSERR;
			}

			if(fsd.root_dir.entry[i].inode_num == -1)
				opendirent = i;
		}
		if(opendirent == -1) {
			//printf("No more files available\n");
			return SYSERR;
		}	



		int m, inod = FIRST_INODE_BLOCK; 
		for(m = 0;m<fsd.ninodes;m++) {
			struct inode free;
			fs_get_inode_by_num(0,m,&free);
			if(free.type == -1) {
				inod = free.id;
				break;
			}
		}

		struct inode fNode;
		fs_get_inode_by_num(0,inod,&fNode);
		fNode.nlink=0;
		fNode.size=0;
		fNode.type=1;

		fsd.root_dir.numentries++;

		memcpy(fsd.root_dir.entry[opendirent].name,filename,strlen(filename));
		fsd.root_dir.entry[opendirent].inode_num = fNode.id;


		fs_put_inode_by_num(0,fNode.id,&fNode);

		return OK;
	}
	else{
		printf("The mode is not valid.\n");
		return -1;
	}

}

int fs_open(char *filename, int flags){
	if ((filename != NULL) && (filename[0] == '\0')) {
		printf("File name is empty\n");
		return -1;
	}
	if (16 == next_open_fd){
		printf("No file can be open right now.\n");
		return -1;	
	}

	int i;
	int curnum =-1;
	for(i=0;i<DIRECTORY_SIZE;i++){
		if(strcmp(filename,fsd.root_dir.entry[i].name)) {
			curnum = fsd.root_dir.entry[i].inode_num;
			break; 
		}
	}

	if(curnum == -1) {
		printf("File %s does not exist\n",filename);
		return -1;
	}

	struct inode newnode;

	fs_get_inode_by_num(0, curnum, &newnode);


	// Allocates file descriptor
	int j;
	int fd;
	for(j = 0;j<DIRECTORY_SIZE*2;j++) {
		if(oft[j].state == -1) {
			fd = j;
			break;
		}
	}

	if(fd == -1)
		return SYSERR;

	oft[fd].in = newnode;
	oft[fd].state=1;
	oft[fd].fileptr=0;

	oft[fd].in.nlink++;
	if(flags==1) {
		oft[fd].fileptr=oft[fd].in.size;
	}

	// Update inode
	fs_put_inode_by_num(0,curnum,&oft[fd].in);
	return fd;
}



int fs_close(int fd){
	if(oft[fd].state == -1) {
		printf("The file descriptor is not valid.\n");
		return -1;
	}
	//Update inode and save to memory
	struct inode updatenode;
	fs_get_inode_by_num(0,oft[fd].in.id,&updatenode);
	updatenode.nlink--;
	fs_put_inode_by_num(0,oft[fd].in.id,&updatenode);


	oft[fd].state = -1;
	oft[fd].fileptr = 0;


	return OK;
}


int fs_seek(int fd, int offset){
	int fileptr = oft[fd].fileptr + offset;
	if(fileptr > oft[fd].in.size) {
		printf("Offset exceeds the file size.\n");
		return -1;
	}
	if((fileptr/fsd.blocksz) > INODEBLOCKS) {
		printf("The offset is out of boundary.\n");
		return -1;
	}
	else
		oft[fd].fileptr = fileptr;

	return OK;
}


int fs_read(int fd, void *buf, int nbytes){
	fs_get_inode_by_num(0,oft[fd].in.id,&oft[fd].in);
	if(oft[fd].in.size < nbytes + oft[fd].fileptr) {
		//if the last piece is not enough, then read whatever is left there.
		nbytes = oft[fd].in.size;
	}
	char* buff = buf;
	buff[0]='\0';
	int bytesread = 0;
	while(bytesread < nbytes) {
		int curBlock = oft[fd].fileptr/fsd.blocksz;
		// Check here to see if out of bounds
		int bytesOnCurBlock = fsd.blocksz - (oft[fd].fileptr%fsd.blocksz);

		if(bytesOnCurBlock < nbytes - bytesread) {
			char r[fsd.blocksz];
			int btor = fsd.blocksz - oft[fd].fileptr%fsd.blocksz;
			bs_bread(0,oft[fd].in.blocks[curBlock], oft[fd].fileptr%fsd.blocksz,&r,btor);
			oft[fd].fileptr += btor;
			strncat(buf,r,btor);
			bytesread += btor;
		}
		// Only need to read on current block
		else {
			char r[fsd.blocksz];
			bs_bread(0,oft[fd].in.blocks[curBlock],oft[fd].fileptr%fsd.blocksz,&r,nbytes - bytesread);
			oft[fd].fileptr += nbytes - bytesread;
			strncat(buf,r,nbytes-bytesread);
			bytesread += nbytes - bytesread;

		}	
	}
	fs_put_inode_by_num(0,oft[fd].in.id,&oft[fd].in);


	return OK;
}

int fs_write(int fd, void *buf, int nbytes){
	fs_get_inode_by_num(0,oft[fd].in.id,&oft[fd].in);
	int block_num=(oft[fd].fileptr+nbytes)/fsd.blocksz+1;
	int cur_block_num = oft[fd].in.size/fsd.blocksz;
	if(oft[fd].in.size%fsd.blocksz != 0)
		cur_block_num++;

	int alloc_num;
	int i, j, flag = -1;
	if((alloc_num = block_num - cur_block_num) > 0){
		for(i = 0; i < alloc_num; i++){
			for(j = 0; j < fsd.nblocks; j++){
				if(fs_getmaskbit(j) == 0){
					oft[fd].in.blocks[cur_block_num] = j;
					fs_setmaskbit(j);
					flag = 0;
				} 
			}  
			if(flag != -1)
				cur_block_num++;
			else {
				fs_put_inode_by_num(0,oft[fd].in.id,&oft[fd].in);
				return -1;
			}
		}
	}


	int byteswrite = 0;
	while(byteswrite < nbytes) {
		int curBlock = oft[fd].fileptr/fsd.blocksz;
		int offset = oft[fd].fileptr % fsd.blocksz;
		// Check here to see if out of bounds
		int bytesOnCurBlock = fsd.blocksz - (oft[fd].fileptr%fsd.blocksz);

		if(bytesOnCurBlock < nbytes - byteswrite) {
			char* buff = ((char*) buf);
			//printf("%d %d %d", )
			bs_bwrite(0,oft[fd].in.blocks[curBlock],offset ,&buff[byteswrite],bytesOnCurBlock);
			oft[fd].fileptr += bytesOnCurBlock;
			byteswrite += bytesOnCurBlock;
			if(oft[fd].fileptr > oft[fd].in.size)
				oft[fd].in.size = oft[fd].fileptr;
		}
		// Only need to read on current block
		else {
			char* buff = ((char*) buf);
			bs_bwrite(0,oft[fd].in.blocks[curBlock],offset, &buff[byteswrite],nbytes - byteswrite);
			oft[fd].fileptr += nbytes - byteswrite;
			byteswrite += nbytes - byteswrite;
			if(oft[fd].fileptr > oft[fd].in.size) {
				oft[fd].in.size = oft[fd].fileptr;
			}

		}	
	}
	fs_put_inode_by_num(0,oft[fd].in.id,&oft[fd].in);


	return OK;
}


int fs_fileblock_to_diskblock(int dev, int fd, int fileblock) {
	int diskblock;

	if (fileblock >= INODEBLOCKS - 2) {
		printf("No indirect block support\n");
		return SYSERR;
	}

	diskblock = oft[fd].in.blocks[fileblock]; //get the logical block address

	return diskblock;
}

/* read in an inode and fill in the pointer */
int
fs_get_inode_by_num(int dev, int inode_number, struct inode *in) {
	int bl, inn;
	int inode_off;

	if (dev != 0) {
		printf("Unsupported device\n");
		return SYSERR;
	}
	if (inode_number > fsd.ninodes) {
		printf("fs_get_inode_by_num: inode %d out of range\n", inode_number);
		return SYSERR;
	}

	bl = inode_number / INODES_PER_BLOCK;
	inn = inode_number % INODES_PER_BLOCK;
	bl += FIRST_INODE_BLOCK;

	inode_off = inn * sizeof(struct inode);

	/*
	   printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
	   printf("inn*sizeof(struct inode): %d\n", inode_off);
	 */

	bs_bread(dev0, bl, 0, &block_cache[0], fsd.blocksz);
	memcpy(in, &block_cache[inode_off], sizeof(struct inode));

	return OK;

}

int
fs_put_inode_by_num(int dev, int inode_number, struct inode *in) {
	int bl, inn;

	if (dev != 0) {
		printf("Unsupported device\n");
		return SYSERR;
	}
	if (inode_number > fsd.ninodes) {
		printf("fs_put_inode_by_num: inode %d out of range\n", inode_number);
		return SYSERR;
	}

	bl = inode_number / INODES_PER_BLOCK;
	inn = inode_number % INODES_PER_BLOCK;
	bl += FIRST_INODE_BLOCK;

	/*
	   printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
	 */

	bs_bread(dev0, bl, 0, block_cache, fsd.blocksz);
	memcpy(&block_cache[(inn*sizeof(struct inode))], in, sizeof(struct inode));
	bs_bwrite(dev0, bl, 0, block_cache, fsd.blocksz);

	return OK;
}

int fs_mkfs(int dev, int num_inodes) {
	int i;

	if (dev == 0) {
		fsd.nblocks = dev0_numblocks;
		fsd.blocksz = dev0_blocksize;
	}
	else {
		printf("Unsupported device\n");
		return SYSERR;
	}

	if (num_inodes < 1) {
		fsd.ninodes = DEFAULT_NUM_INODES;
	}
	else {
		fsd.ninodes = num_inodes;
	}

	i = fsd.nblocks;
	while ( (i % 8) != 0) {i++;}
	fsd.freemaskbytes = i / 8; 

	if ((fsd.freemask = getmem(fsd.freemaskbytes)) == (void *)SYSERR) {
		printf("fs_mkfs memget failed.\n");
		return SYSERR;
	}

	/* zero the free mask */
	for(i=0;i<fsd.freemaskbytes;i++) {
		fsd.freemask[i] = '\0';
	}

	fsd.inodes_used = 0;

	/* write the fsystem block to SB_BLK, mark block used */
	fs_setmaskbit(SB_BLK);
	bs_bwrite(dev0, SB_BLK, 0, &fsd, sizeof(struct fsystem));

	/* write the free block bitmask in BM_BLK, mark block used */
	fs_setmaskbit(BM_BLK);
	bs_bwrite(dev0, BM_BLK, 0, fsd.freemask, fsd.freemaskbytes);

	return 1;
}

void
fs_print_fsd(void) {

	printf("fsd.ninodes: %d\n", fsd.ninodes);
	printf("sizeof(struct inode): %d\n", sizeof(struct inode));
	printf("INODES_PER_BLOCK: %d\n", INODES_PER_BLOCK);
	printf("NUM_INODE_BLOCKS: %d\n", NUM_INODE_BLOCKS);
}

/* specify the block number to be set in the mask */
int fs_setmaskbit(int b) {
	int mbyte, mbit;
	mbyte = b / 8;
	mbit = b % 8;

	fsd.freemask[mbyte] |= (0x80 >> mbit);
	return OK;
}

/* specify the block number to be read in the mask */
int fs_getmaskbit(int b) {
	int mbyte, mbit;
	mbyte = b / 8;
	mbit = b % 8;

	return( ( (fsd.freemask[mbyte] << mbit) & 0x80 ) >> 7);
	return OK;

}

/* specify the block number to be unset in the mask */
int fs_clearmaskbit(int b) {
	int mbyte, mbit, invb;
	mbyte = b / 8;
	mbit = b % 8;

	invb = ~(0x80 >> mbit);
	invb &= 0xFF;

	fsd.freemask[mbyte] &= invb;
	return OK;
}

/* This is maybe a little overcomplicated since the lowest-numbered
   block is indicated in the high-order bit.  Shift the byte by j
   positions to make the match in bit7 (the 8th bit) and then shift
   that value 7 times to the low-order bit to print.  Yes, it could be
   the other way...  */
void fs_printfreemask(void) {
	int i,j;

	for (i=0; i < fsd.freemaskbytes; i++) {
		for (j=0; j < 8; j++) {
			printf("%d", ((fsd.freemask[i] << j) & 0x80) >> 7);
		}
		if ( (i % 8) == 7) {
			printf("\n");
		}
	}
	printf("\n");
}

#endif /* FS */
