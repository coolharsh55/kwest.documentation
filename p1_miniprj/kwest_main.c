/* KWEST EXECUTABLE */

/* gcc -c kwest_main.c -D_FILE_OFFSET_BITS=64 -lfuse
 * Mounting   : ./kwest mount_point
 * Unmounting : fusermount -u mount_point
 */ 

/* LICENSE
   Copyright [2013] [harshvardhan pandit]
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

#define FUSE_USE_VERSION 28

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <fuse.h>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include "dbfunc.h"
#include "import.h"

static int kwest_getattr(const char *path, struct stat *stbuf)
{
	char *abspath;
	int tmp=0;
	
	if(*(path+1) == '\0') { /* root */
		stbuf->st_mode= S_IFDIR | 0755;
		stbuf->st_nlink=1;
		return 0;
	}
	
	const char *t=strrchr(path,'/');
	if((tmp=istag(t+1))>0) { /* tag */
		stbuf->st_mode= S_IFDIR | 0755;
		stbuf->st_nlink=1;
		return 0;
	} 
	
	tmp = 0;
	const char *f=strrchr(path,'/');
	if((tmp=isfile(f+1))>0) { /* file */
		abspath=get_abspath(path);
		stat(abspath,stbuf);
		free((char *)abspath);
		return 0;
	}
	return -ENOENT;
}

static int kwest_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi)
{
	(void) offset;
	(void) fi;
	void *ptr;
	int tmp = 0;
	
	if(*(path+1) == '\0') {	/* root */
		const char *tname;
		
		/* Display all tags under audio */
		ptr=get_association("audio",RELATION_SYSTEM);
		while((tname=get_data(ptr))!=NULL) { 
			struct stat st; //struct stat
			memset(&st, 0, sizeof(st));
			st.st_mode = S_IFDIR | 0755;
			if (filler(buf, tname, NULL, 0)) {
				break;
			}
		}
		
		/* Display all files under audio */
		ptr=get_file("audio");
		while((tname=get_data(ptr))!=NULL) { 
			struct stat st; //struct stat
			memset(&st, 0, sizeof(st));
			st.st_mode = S_IFREG | 0444;
			if (filler(buf, tname, NULL, 0)) {
				break;
			}
		}
		
	} else {
		tmp = 0;
		const char *t=strrchr(path,'/');
		const char *tag;
		
		if((tmp=istag(t+1)) == 0){
			 return -ENOENT;
		}
		
		/* Display System Tags */
		ptr=get_association(t+1,RELATION_SYSTEM); 
		while(tag=get_data(ptr)) { /* Get Tags */
			struct stat st; //struct stat
			memset(&st, 0, sizeof(st));
			st.st_mode = S_IFDIR | 0755;
			if (filler(buf, tag, NULL, 0)) {
				break;
			}
		}
		
		/* Display User Tags */
		ptr=get_association(t+1,RELATION_SUBGROUP); 
		while(tag=get_data(ptr)) { /* Get Tags */
			struct stat st; //struct stat
			memset(&st, 0, sizeof(st));
			st.st_mode = S_IFDIR | 0755;
			if (filler(buf, tag, NULL, 0)) {
				break;
			}
		}
		
		/* Display Files */
		ptr=get_file(t+1);
		while(tag=get_data(ptr)) { /* Get Files */ 
			struct stat st; //struct stat
			memset(&st, 0, sizeof(st));
			st.st_mode = S_IFREG | 0444;
			if (filler(buf, tag, NULL, 0)) {
				break;
			}
		}
	}
	return 0;
}

static int kwest_access(const char *path, int mask)
{
	return 0;
}

static int kwest_unlink(const char *path)
{
	char *filename = strrchr(path,'/')+1;
	const char *abs_path = get_abspath(filename);
	if(abs_path!=NULL){
		if(remove_file(abs_path)==KWEST_SUCCESS){
			return 0;
		}
	}
	return -errno;
}

struct fuse_operations ft_oper = {
	.getattr = kwest_getattr,
	.readdir = kwest_readdir,
	.access  = kwest_access,
	.unlink  = kwest_unlink,

};

int main(int argc,char *argv[])
{

	init_db();
	create_db();
	
	/* Create tags to browse Audio Files */
	add_tag("audio",SYSTEM_TAG);
	add_tag("artist",SYSTEM_TAG);
	add_tag("album",SYSTEM_TAG);
	add_tag("genre",SYSTEM_TAG);
	
	/* Metadata available for Audio Files */
	add_meta_info("audio","title");
	add_meta_info("audio","artist");
	add_meta_info("audio","album");
	add_meta_info("audio","genre");
	
	/* Establish tag-tag associations */
	add_association("artist","audio",RELATION_SYSTEM);
	add_association("album","audio",RELATION_SYSTEM);
	add_association("genre","audio",RELATION_SYSTEM);
	
	/* Include all files under Music Directory in Home */
	struct passwd *pw = getpwuid(getuid());
	const char *homedir = pw->pw_dir;
	const char *musicdir = strdup(strcat((char *)homedir,"/Music"));
	if(import(musicdir) == IMP_FAIL) return -1;
	free((char *)musicdir);
	
	/* Establish file-tag associations 
	 * to tag all audio files under appropriate metadata tag */
	system_associations("audio","artist");
	system_associations("audio","album");
	system_associations("audio","genre");
	
	close_db();
	
	init_db();
	return fuse_main(argc, argv, &ft_oper,NULL);
}
