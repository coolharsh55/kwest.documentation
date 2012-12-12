/* FUSE FUNCTIONS */

/* LICENSE
** Copyright [2012] [Harshvardhan Pandit]
** Licensed under the Apache License, Version 2.0 (the "License"); You may 	not use this file except in compliance with the License. You may obtain a copy of the License at
       http://www.apache.org/licenses/LICENSE-2.0
   Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.   
*/
 
#ifndef FUSEFUNC_H_INCLUDED
#define FUSEFUNC_H_INCLUDED

#include <fuse.h>	/* fuse header file */


/* kwest_getattr: retrieve attributes 
 * param:	const char* path	-	path of entry
 * 				struct stat* buf		-	stat buffer to be filled
 * return:	0 on SUCCESS and -ENOENT for NO ENTRY
 * author: @HP
 * */
static int kwest_getattr(const char *path, struct stat *stbuf);


/* kwest_access: permissions
 * param:	const char* path	-	path of entry
 * 				int mask					-	
 * return:	0 on SUCCESS and -ENOENT for NO ENTRY
 * 				-ENOACcESS for invalid permissions
 * author: @HP
 * */
static int kwest_access(const char *path, int mask);

	
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
		       off_t offset, struct fuse_file_info *fi);


/* kwest_mkdir: create tag & associate with path
 * param:	const char* path	-	path of entry
 * 				mode_t mode		-
 * return:	0 on SUCCESS
 * author: @HP
 * */
static int kwest_mkdir(const char *path, mode_t mode);


/* kwest_unlink: remove file
 * param:	const char* path	-	path of entry
 * return:	0 on SUCCESS
 * author: @HP
 * */
static int kwest_unlink(const char *path);


/* kwest_rmdir: remove tag
 * param:	const char* path	-	path of entry
 * return:	0 on SUCCESS
 * author: @HP
 * */
static int kwest_rmdir(const char *path);


/* kwest_rename: rename a file
 * param:	const char* from	-	original file
 * 				const char* to		-	target file
 * return:	0 on SUCCESS
 * author: @HP
 * */
static int kwest_rename(const char *from, const char *to);


/* kwest_chmod: change permissions
 * param:	const char* path	-	path of entry
 * 				mode_t mode		-	new permissions
 * return:	0 on SUCCESS
 * author: @HP
 * */
static int kwest_chmod(const char *path, mode_t mode);


/* kwest_truncate: truncate a files length
 * param:	const char* path	-	path of entry
 * 				off_t size				-	new file size
 * return:	0 on SUCCESS
 * author: @HP
 * */
static int kwest_truncate(const char *path, off_t size);


/* kwest_ftruncate: truncate a files length
 * param:	const char* path	-	path of entry
 * 				off_t size				-	new file size
 * return:	0 on SUCCESS
 * author: @HP
 * */
static int kwest_ftruncate(const char *path, off_t size);


/* kwest_utimens: update access/modification time
 * param:	const char* path	-	path of entry
 * 				const struct timespec ts	-	time structs
 * 				ts[0] - access ; ts[1] modification
 * return:	0 on SUCCESS
 * author: @HP
 * */
static int kwest_utimens(const char *path, const struct timespec ts[2]);


/* kwest_open: check if file can be opened
 * param:	const char* path	-	path of entry
 * 				struct fuse_file_info* fi	-	fuse internal file handle
 * return:	0 on SUCCESS
 * author: @HP
 * */
static int kwest_open(const char *path, struct fuse_file_info *fi);


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
		    struct fuse_file_info *fi);


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
		     off_t offset, struct fuse_file_info *fi);


/* kwest_statfs: give filesystem info
 * param:	const char* path	-	path of entry
 * 				struct statvfs* stbuf	-	pass filesystem info
 * return:	0 on SUCCESS
 * author: @HP
 * */
static int kwest_statfs(const char *path, struct statvfs *stbuf);


/* kwest_statfs: give filesystem info
 * param:	const char* path	-	path of entry
 * 				struct fuse_file_info* fi	-	fuse internal file handle
 * return:	0 on SUCCESS
 * author: @HP
 * */
static int kwest_release(const char *path, struct fuse_file_info *fi);


/* kwest_fsync: flush data to disk
 * param:	const char* path	-	path of entry
 * 				int isdatasync		-	data is synced
 * 				struct fuse_file_info* fi	-	internal fuse file handle
 * return:	0 on SUCCESS
 * author: @HP
 * */
static int kwest_fsync(const char *path, int isdatasync,
		     struct fuse_file_info *fi);

#endif
