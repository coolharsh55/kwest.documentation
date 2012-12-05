/* 
 * gcc -Wall -c fusefunc.c -o fusefunc -D_FILE_OFFSET_BITS=64 -lfuse
 * FUSE function implementations
 * */

#include "fusefunc.h"

/* kwest_getattr: 
 * input: path as string
 * input: stat buffer pointer
 * return: integer for operation status
 * operation: return attributes in stat buffer
 * */
static int kwest_getattr(const char *path, struct stat *stbuf)
{
	return 0;
	/* OPERATION:
	 * decode path
	 * last entry is file OR directory
	 * rest entries are tags associated/related
	 * get info from DB
	 * copy in stbuf:
	 * stbuf->st_mode = FILE|DIRECTORY
	 * stbuf->st_nlink = ???
	 * stbuf->st_size = size of file
	 * return 0 for success
	 * return -ENOENT for NO ENTRY
	 * */
}
	
/* kwest_access: 
 * input: path as string
 * input: mask integer
 * return: integer for permission
 * operation: checks if calling process can access the file path-name
 * */
static int kwest_access(const char *path, int mask)
{
	return 0;
	/* OPERATION
	 * decode path
	 * last entry if file/directory
	 * mask contains requested permissions
	 * check if permission is available
	 * return -ENOENT for invalid path
	 * return -EACCESS for permission not valid
	 * return 0 for success
	 * */
}	
	
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
		       off_t offset, struct fuse_file_info *fi)
{
	return 0;
	/* OPERATION
	 * populate the directory with entries
	 * results are a series of `struct dirents`
	 * struct stat for each file in directory (only requires st_ino & st_mode)
	 * call filler function with buffer, filename, struct stat, offset(NULL)
	 * filler returns non-zero value for correct operation
	 * do same for all files and directories under current path
	 * */
}		     

/* kwest_mkdir: 
 * input: path as string
 * input: mode containing directory permissions
 * return: integer for operation status
 * operation: create a new tag
 * */
static int kwest_mkdir(const char *path, mode_t mode)
{
	return 0;
	/* OPERATION
	 * we have to create a tag here with the specified name under path
	 * decode the path
	 * last name is tag name
	 * create that tag
	 * associate it with all other tags in path
	 * */
}

/* kwest_unlink: 
 * input: path as string
 * return: integer for operation status
 * operation: delete the file
 * */
static int kwest_unlink(const char *path)
{
	return 0;
	/* OPERATION
	 * delete the file
	 * remove the file from the associated tags
	 * if file has no more tags, delete the file
	 * */
}

/* kwest_rmdir: 
 * input: path as string
 * return: integer for operation status
 * operation: delete the tag
 * */
static int kwest_rmdir(const char *path)
{
	return 0;
	/* OPERATION
	 * decode the path
	 * remove the tag from system
	 * remove all its associations
	 * */
}

/* kwest_rename: 
 * input: source filename path - from
 * input: target filename path - to
 * return: integer for operation status
 * operation: rename the file and re-assign tags
 * */
static int kwest_rename(const char *from, const char *to)
{
	return 0;
	/* OPERATION
	 * decode from path
	 * identify file
	 * decode to path
	 * identify new filename
	 * change base filename
	 * change tag relationships with file
	 * */
}

/* kwest_chmod: 
 * input: path of the file
 * input: mode structure containing permissions
 * return: integer for operation status
 * operation: assign permissions to file
 * */
static int kwest_chmod(const char *path, mode_t mode)
{
	return 0;
	/* OPERATION
	 * decode path
	 * get filename or directory name
	 * check for userpermissions from mode
	 * if permissions are available return 0
	 * else return -EACCESS
	 * */
}

/* kwest_truncate: 
 * input: path of the file
 * input: size of file
 * return: integer for operation status
 * operation: truncate or extend file to given size
 * */
static int kwest_truncate(const char *path, off_t size)
{
	return 0;
	/* OPERATION
	 * decode path, get filename
	 * get physical location
	 * pass size to system call to truncate
	 * */
}

/* kwest_ftruncate: 
 * input: path of the file
 * input: size of file
 * return: integer for operation status
 * operation: truncate or extend file to given size
 * */
static int kwest_ftruncate(const char *path, off_t size)
{
	return 0;
	/* OPERATION
	 * decode path, get filename
	 * get physical location
	 * pass size to system call to truncate
	 * */
}

/* kwest_utimens: 
 * input: path of the file
 * input: timespec structure containing last access/modification time
 * return: integer for operation status
 * operation: assign last access/modification time
 * */
static int kwest_utimens(const char *path, const struct timespec ts[2])
{
	return 0;
	/* OPERATION
	 * decode path
	 * get filename
	 * update the last access time to current time
	 * whenever file is changed via open change modification time
	 * update with values in DB
	 * */
}

/* kwest_open: 
 * input: path of the file
 * input: fuse file handle
 * return: integer for operation status
 * operation: read permission for file and return if valid for open
 * */
static int kwest_open(const char *path, struct fuse_file_info *fi)
{
	return 0;
	/* OPERATION
	 * decode path
	 * get filename
	 * check permissions
	 * if all available return 0
	 * else return -EACCESS
	 * */
}

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
		    struct fuse_file_info *fi)
{
	return 0;
	/* OPERATION
	 * decode path
	 * get filename
	 * get physical location
	 * use pread() to read `size` bytes from `offset` into `buf`
	 * */
}

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
		     off_t offset, struct fuse_file_info *fi)
{
	return 0;
	/* OPERATION
	 * DOES NOT RETURN O
	 * decode path
	 * get filename
	 * get physical location
	 * write bytes specified to file
	 * */
}

/* kwest_statfs: 
 * input: path of the file
 * input: statvfs buffer 
 * return: integer for operation status
 * operation: fill statvfs with filesystem info
 * */
static int kwest_statfs(const char *path, struct statvfs *stbuf)
{
	return 0;
	/* OPERATION
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
}

/* kwest_release: 
 * input: path of the file
 * input: fuse file handle
 * return: integer for operation status
 * operation: release file, do nothing
 * */
static int kwest_release(const char *path, struct fuse_file_info *fi)
{
	return 0;
	/* OPERATION
	 * decode path
	 * get filename
	 * update access time
	 * */
}

/* kwest_fsync: 
 * input: path of the file
 * input: isdatasync denotes data/metadata sync
 * input: fuse file handle
 * return: integer for operation status
 * operation: flush data to disk, do nothing
 * */
static int kwest_fsync(const char *path, int isdatasync,
		     struct fuse_file_info *fi)
{
	return 0;
	/* OPERATION
	 * DO NOTHING
	 * */
}