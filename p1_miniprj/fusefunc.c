/* FUSE FUNCTIONS */
/* gcc -Wall -c fusefunc.c -o fusefunc.o -D_FILE_OFFSET_BITS=64 -lfuse */

/* LICENSE
** Copyright [2012] [Harshvardhan Pandit]
** Licensed under the Apache License, Version 2.0 (the "License"); You may not use this file except in compliance with the License. You may obtain a copy of the License at
       http://www.apache.org/licenses/LICENSE-2.0
   Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.   
*/

#include "fusefunc.h"	/* fuse functions */
#include <string.h>
#include <errno.h>

/* kwest_getattr: retrieve attributes 
 * param:	const char* path	-	path of entry
 * 				struct stat* buf		-	stat buffer to be filled
 * return:	0 on SUCCESS and -ENOENT for NO ENTRY
 * author: @HP
 * */
static int kwest_getattr(const char *path, struct stat *stbuf)
{
	/* if path is root, return with DIRECTORY */
	if(*(path+1) == '\0') { 
		memset(stbuf, 0, sizeof(struct stat));
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
		return 0;
	}
	
	char name[20]; 
	char *tmp = NULL;
	const char *p = path;
	tmp = strrchr(p,'/'); 	/* get last name */
	if(tmp == NULL) return 0; 	/* error in syntax */
	strcpy(name, tmp+1);
		
	/*
	 * return (db_get_attr(name, stbuf));
	 * query to check if path==file
	if(query_result == SQLITE_ROW) { //success
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = no_links;
		stbuf->st_size = size;
	} else { //no rows returned
	 * query to check if path==directory
	   if(query_result == SQLITE_ROW) { //success
			stbuf->st_mode = S_IFDIR | 0755;
			stbuf->st_nlink = 1;
		} else { //error
			return -ENOENT;
		}
	}
	* */
	
	return 0;
}
	
/* kwest_access: permissions
 * param:	const char* path	-	path of entry
 * 				int mask					-	
 * return:	0 on SUCCESS and -ENOENT for NO ENTRY
 * 				-ENOACcESS for invalid permissions
 * author: @HP
 * */
static int kwest_access(const char *path, int mask)
{
	/* if path is root - return */
	if(*(path+1) == '\0')
		return 0;		
		
	char name[20];
	char *tmp = NULL;
	const char *p = path;
	tmp = strrchr(p,'/');
	if(tmp == NULL) return 0;
	strcpy(name, tmp+1);
	
	/*
	 * return (db_access(path));
	 * query to get physical path of file
	if(query_result == SQLITE_ROW) { //success
		return access(path, mask);
	} else { //no rows returned
		return -ENOENT;
		}
	}
	* */
	
	return 0;
}	
	
/* kwest_readdir: list directory contents
 * param:	const char* path	-	path of entry
 * 				void *buf				-	buffer to be filled
 * 				fuse_fill_dir_t filler	-	filler function
 * 				off_t offset				-	offset of current read
 * 				struct fuse_file_info *fi	- fuse internal file handle
 * return:	0 on SUCCESS and -ENOENT for NO ENTRY
 * author: @HP
 * */	
static int kwest_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
	(void) offset;
	(void) fi;
	
	/* return (db_list(path, buf, filler)); */	
	
	return 0;
}		     

/* kwest_mkdir: create tag & associate with path
 * param:	const char* path	-	path of entry
 * 				mode_t mode		-
 * return:	0 on SUCCESS
 * author: @HP
 * */
static int kwest_mkdir(const char *path, mode_t mode)
{
	char tagname[20]; 
	char *tmp = NULL;
	const char *p = path;
	tmp = strrchr(p,'/');
	if(tmp == NULL) return 0;
	strcpy(tagname, tmp+1); 	/* get new tag name - is at last */
	
	int tag_id = 0; /* db_tag_new(tagname); */
	if(tag_id == 0) {
		return 0; /* return directory already exists */
	}
	
	const char *p2 = strchr(p+1,'/');
	int count = 0;
	while(p != tmp && p2 != NULL) { /* associate with other tags */
	    ++p;
	    count = 0;
		while(p != p2) { /* seperate tags to associate */
			tagname[count++] = *p++;
		}
		tagname[count] = '\0';		
		/* if(db_tag_associate(tag_id, tagname) == 0) { 
			return 0; /* error in creating directory * /
		} */
		p2 = strchr(p+1,'/');
	}	
	
	return 0;
}

/* kwest_unlink: remove file
 * param:	const char* path	-	path of entry
 * return:	0 on SUCCESS
 * author: @HP
 * */
static int kwest_unlink(const char *path)
{
	char name[20]; 
	char *tmp = NULL;
	const char *p = path;
	tmp = strrchr(p,'/');  
	if(tmp == NULL) return 0;
	strcpy(name, tmp+1);  /* get filename */
	int file_id = 0;  /* db_file_getid(name) */
	if(file_id == 0) {
		return 0; /* error: file does not exist */
	}
	
	const char *p2 = strchr(p+1,'/');
	int count = 0;
	while(p != tmp && p2 != NULL) {  /* remove from tags */
	    ++p;
	    count = 0;
		while(p != p2) { 
			name[count++] = *p++;
		}
		name[count] = '\0';		
		
		/* if(db_file_tag_remove(file_id, name) == 0) { 
			return 0; /* error in removing file * /
		} */
		p2 = strchr(p+1,'/');
	}	
	return 0;
}

/* kwest_rmdir: remove tag
 * param:	const char* path	-	path of entry
 * return:	0 on SUCCESS
 * author: @HP
 * */
static int kwest_rmdir(const char *path)
{
	char tagname[20]; 
	char *tmp = NULL;
	const char *p = path;
	tmp = strrchr(p+1,'/'); 
	if(tmp == NULL) 
		return 0;  /* error in reading directory */
		
	/* if(db_tag_remove(tmp) == 0) {
		return 0; /* error in removing directory * /
	} */
	
	return 0;
}

/* kwest_rename: rename a file
 * param:	const char* from	-	original file
 * 				const char* to		-	target file
 * return:	0 on SUCCESS
 * author: @HP
 * */
static int kwest_rename(const char *from, const char *to)
{
	char fromname[20]; 
	char toname[20]; 
	char *tmp = NULL;
	const char *p = from;
	tmp = strrchr(p,'/'); 
	if(tmp == NULL) return 0;
	strcpy(fromname, tmp+1); 
	p = to;
	tmp = strrchr(p,'/'); 
	if(tmp == NULL) return 0;
	strcpy(toname, tmp+1); 
	
	/* if(db_file_rename(fromname, toname) == 0) {
		return 0; /* error renaming file * /
	} */
	
	return 0;
	
}

/* kwest_chmod: change permissions
 * param:	const char* path	-	path of entry
 * 				mode_t mode		-	new permissions
 * return:	0 on SUCCESS
 * author: @HP
 * */
static int kwest_chmod(const char *path, mode_t mode)
{
	char filename[20]; 
	char *tmp = NULL;
	const char *p = path;
	tmp = strrchr(p,'/'); 
	if(tmp == NULL) return 0;
	strcpy(filename, tmp+1); 
	
	/* if(db_file_chmod(filename) == 0) {
		return 0; /* error setting permissions * /
	} */
	
	return 0;
}

/* kwest_truncate: truncate a files length
 * param:	const char* path	-	path of entry
 * 				off_t size				-	new file size
 * return:	0 on SUCCESS
 * author: @HP
 * */
static int kwest_truncate(const char *path, off_t size)
{
	char filename[20]; 
	char *tmp = NULL;
	const char *p = path;
	tmp = strrchr(p,'/'); 
	if(tmp == NULL) return 0;
	strcpy(filename, tmp+1); 
	
	/* if(db_file_truncate(filename, size) == 0) {
		return 0; /* cannot truncate file * /
	} */
	
	return 0;
}

/* kwest_ftruncate: truncate a files length
 * param:	const char* path	-	path of entry
 * 				off_t size				-	new file size
 * return:	0 on SUCCESS
 * author: @HP
 * */
static int kwest_ftruncate(const char *path, off_t size)
{
	char filename[20]; 
	char *tmp = NULL;
	const char *p = path;
	tmp = strrchr(p,'/'); 
	if(tmp == NULL) return 0;
	strcpy(filename, tmp+1); 
	
	/* if(db_file_truncate(filename, size) == 0) {
		return 0; /* cannot truncate file * /
	} */
	
	return 0;
}

/* kwest_utimens: update access/modification time
 * param:	const char* path	-	path of entry
 * 				const struct timespec ts	-	time structs
 * 				ts[0] - access ; ts[1] modification
 * return:	0 on SUCCESS
 * author: @HP
 * */
static int kwest_utimens(const char *path, const struct timespec ts[2])
{
	char filename[20]; 
	char *tmp = NULL;
	const char *p = path;
	tmp = strrchr(p,'/'); 
	if(tmp == NULL) return 0; 
	strcpy(filename, tmp+1); 
	
	/* if(db_file_utimens(filename, ts) == 0) {
		return 0; /* error updating time * /
	} */
	
	return 0;
}

/* kwest_open: check if file can be opened
 * param:	const char* path	-	path of entry
 * 				struct fuse_file_info* fi	-	fuse internal file handle
 * return:	0 on SUCCESS
 * author: @HP
 * */
static int kwest_open(const char *path, struct fuse_file_info *fi)
{
	char filename[20]; 
	char *tmp = NULL;
	const char *p = path;
	tmp = strrchr(p,'/');
	if(tmp == NULL) return 0;
	strcpy(filename, tmp+1); 
	return 0; /* db_file_access(path); */
}

/* kwest_read: read blocks from file
 * param:	const char* path	-	path of entry
 * 				char* buf				-	buffer to pass data
 * 				size_t size				-	size of data to be read
 * 				off_t offset				-	position to start reading
 * 				struct fuse_file_info* fi	-	internal fuse file handle
 * return:	0 on SUCCESS
 * author: @HP
 * */
static int kwest_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
	char filename[20]; 
	char *tmp = NULL;
	const char *p = path;
	tmp = strrchr(p,'/'); 
	if(tmp == NULL) return 0;
	strcpy(filename, tmp+1); 
	
	/* pread(db_file_path_name(filename) */
	
	return 0;
}

/* kwest_write: write blocks from file
 * param:	const char* path	-	path of entry
 * 				char* buf				-	buffer to pass data
 * 				size_t size				-	size of data to write
 * 				off_t offset				-	position to start writing
 * 				struct fuse_file_info* fi	-	internal fuse file handle
 * return:	0 on SUCCESS
 * author: @HP
 * */
static int kwest_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	char filename[20]; 
	char *tmp = NULL;
	const char *p = path;
	tmp = strrchr(p,'/'); 
	if(tmp == NULL) return 0;
	strcpy(filename, tmp+1); 
	
	/* write(db_file_path(filename) */
	
	return 0;
}

/* kwest_statfs: give filesystem info
 * param:	const char* path	-	path of entry
 * 				struct statvfs* stbuf	-	pass filesystem info
 * return:	0 on SUCCESS
 * author: @HP
 * */
static int kwest_statfs(const char *path, struct statvfs *stbuf)
{
	/*
	 * supposed to return FS information
	 * but ours is virtual file system where this does not apply
	 * we can combine total monitor total disk data and represent that
	 * struct statvfs {
       unsigned long  f_bsize;    file system block size
       unsigned long  f_frsize;   fragment size
       fsblkcnt_t     f_blocks;   size of fs in f_frsize units
       fsblkcnt_t     f_bfree;    # free blocks
       fsblkcnt_t     f_bavail;   # free blocks for unprivileged users
       fsfilcnt_t     f_files;    # inodes
       fsfilcnt_t     f_ffree;    # free inodes
       fsfilcnt_t     f_favail;   # free inodes for unprivileged users
	   unsigned long  f_fsid;     file system ID
       unsigned long  f_flag;     mount flags
       unsigned long  f_namemax;  maximum filename length
       };
	 * */
	 return 0;
}

/* kwest_statfs: give filesystem info
 * param:	const char* path	-	path of entry
 * 				struct fuse_file_info* fi	-	fuse internal file handle
 * return:	0 on SUCCESS
 * author: @HP
 * */
static int kwest_release(const char *path, struct fuse_file_info *fi)
{
	return 0;
}

/* kwest_fsync: flush data to disk
 * param:	const char* path	-	path of entry
 * 				int isdatasync		-	data is synced
 * 				struct fuse_file_info* fi	-	internal fuse file handle
 * return:	0 on SUCCESS
 * author: @HP
 * */
static int kwest_fsync(const char *path, int isdatasync,
		     struct fuse_file_info *fi)
{
	return 0;
}
