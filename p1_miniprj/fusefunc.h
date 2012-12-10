/* FUSE functions
 * list of functions:
 * getattr
 * access		
 * readlink - NOT_REQUIRED	
 * readdir	
 * mknod	- NOT_REQUIRED	
 * mkdir		
 * symlink	- NOT_REQUIRED	
 * unlink		
 * rmdir		
 * rename		
 * link		- NOT_REQUIRED	
 * chmod		
 * chown	- NOT_REQUIRED	
 * truncate	
 * utimens	
 * open		
 * read		
 * write		
 * statfs		
 * release	
 * fsync		
 */
 
 /*

   Copyright [2012] [harshvardhan pandit]

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
   
 */
 
#ifndef FUSEFUNC_H_INCLUDED
#define FUSEFUNC_H_INCLUDED

/* fuse header file */
#include <fuse.h>

/* kwest_getattr: 
 * input: path as string
 * input: stat buffer pointer
 * return: integer for operation status
 * operation: return attributes in stat buffer
 * */
static int kwest_getattr(const char *path, struct stat *stbuf);
	
/* kwest_access: 
 * input: path as string
 * input: mask integer
 * return: integer for permission
 * operation: checks if calling process can access the file path-name
 * */
static int kwest_access(const char *path, int mask);	
	
/* kwest_readdir: 
 * input: path as string
 * input: filler buffer pointer
 * input: fuse defined filler function to populate directories
 * input: offset of last read location
 * input: fuse file handle pointer
 * return: integer for operation status
 * operation: populate list of directories and files under current
 * path by using the filler function
 * */
static int kwest_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi);

/* kwest_mkdir: 
 * input: path as string
 * input: mode containing directory permissions
 * return: integer for operation status
 * operation: create a new tag
 * */
static int kwest_mkdir(const char *path, mode_t mode);

/* kwest_unlink: 
 * input: path as string
 * return: integer for operation status
 * operation: delete the file
 * */
static int kwest_unlink(const char *path);

/* kwest_rmdir: 
 * input: path as string
 * return: integer for operation status
 * operation: delete the tag
 * */
static int kwest_rmdir(const char *path);

/* kwest_rmdir: 
 * input: source filename path - from
 * input: target filename path - to
 * return: integer for operation status
 * operation: rename the file and re-assign tags
 * */
static int kwest_rename(const char *from, const char *to);

/* kwest_chmod: 
 * input: path of the file
 * input: mode structure containing permissions
 * return: integer for operation status
 * operation: assign permissions to file
 * */
static int kwest_chmod(const char *path, mode_t mode);

/* kwest_truncate: 
 * input: path of the file
 * input: size of file
 * return: integer for operation status
 * operation: truncate or extend file to given size
 * */
static int kwest_truncate(const char *path, off_t size);

/* kwest_ftruncate: 
 * input: path of the file
 * input: size of file
 * return: integer for operation status
 * operation: truncate or extend file to given size
 * */
static int kwest_ftruncate(const char *path, off_t size);

/* kwest_utimens: 
 * input: path of the file
 * input: timespec structure containing last access/modification time
 * return: integer for operation status
 * operation: assign last access/modification time
 * */
static int kwest_utimens(const char *path, const struct timespec ts[2]);

/* kwest_open: 
 * input: path of the file
 * input: fuse file handle
 * return: integer for operation status
 * operation: read permission for file and return if valid for open
 * */
static int kwest_open(const char *path, struct fuse_file_info *fi);

/* kwest_read: 
 * input: path of the file
 * input: character buffer for file data
 * input: size in bytes to read
 * input: offset of file to start reading
 * input: fuse file handle
 * return: integer for operation status
 * operation: read specified bytes of file
 * */
static int kwest_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi);

/* kwest_write: 
 * input: path of the file
 * input: character buffer for file data
 * input: size in bytes to write
 * input: offset of file to start writing
 * input: fuse file handle
 * return: integer for operation status
 * operation: write specified bytes of file
 * */
static int kwest_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi);

/* kwest_statfs: 
 * input: path of the file
 * input: statvfs buffer 
 * return: integer for operation status
 * operation: fill statvfs with filesystem info
 * */
static int kwest_statfs(const char *path, struct statvfs *stbuf);

/* kwest_release: 
 * input: path of the file
 * input: fuse file handle
 * return: integer for operation status
 * operation: release file, do nothing
 * */
static int kwest_release(const char *path, struct fuse_file_info *fi);

/* kwest_fsync: 
 * input: path of the file
 * input: isdatasync denotes data/metadata sync
 * input: fuse file handle
 * return: integer for operation status
 * operation: flush data to disk, do nothing
 * */
static int kwest_fsync(const char *path, int isdatasync,
		     struct fuse_file_info *fi);

#endif
