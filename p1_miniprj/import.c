/* IMPORTING SEMANTICS */
/* gcc -Wall -c import.c -o import.o -lsqlite3 */

/* LICENSE
   Copyright [2013] [sahil gupta]
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

#include "import.h"

/* import_semantics:
 * param: char *path - Absolute path of Diectory to be imported
 * param: char *dirname - Name of Directory 
 * return: 1 on SUCCESS
 * author: @SG
 */
static int import_semantics(const char *path,const char *dirname)
{
	char full_name[_POSIX_PATH_MAX + 1];
	DIR *directory;
	struct dirent *entry; 
	struct stat fstat;
	size_t path_len;
	size_t dir_len;
	 
	 path_len = strlen(path);
	 directory = opendir(path);
	 if (directory == NULL) {
		perror ("Couldn't open the directory");
		return IMP_FAIL;
	}

	while ((entry = readdir(directory))) {
		dir_len = strlen(entry->d_name);
		
		/* Calculate full name, check we are in file length limts */
		if ((path_len + dir_len + 1) > _POSIX_PATH_MAX){
			continue;
		}
		strcpy(full_name, path);
		if (full_name[path_len - 1] != '/'){
			strcat(full_name, "/");
		}
		strcat(full_name, entry->d_name);

		/* Ignore files starting with . */
		if((strchr(entry->d_name,'.')-entry->d_name)==0){
			continue;
		}

		/* Ignore files ending with ~ */
		if((strrchr(entry->d_name,'~')-entry->d_name)==dir_len-1){
			continue;
		}

		/* Get File Infromation : Returns 0 if successful */
		if (stat(full_name, &fstat) < 0){
			continue;
		}

		if (S_ISDIR(fstat.st_mode)) { /* Directory */

			if(add_tag(entry->d_name,USER_TAG)==KWEST_SUCCESS){
				printf("Creating Tag : %s\n",entry->d_name);
			}

			/* Access Sub-Directories */
			import_semantics(full_name,entry->d_name);

			/* Tag-Tag Relation */
			add_association(entry->d_name,dirname,
			                RELATION_SUBGROUP);

		} else if(S_ISREG(fstat.st_mode)) { /* Regular File */

			if(add_file(full_name)==KWEST_SUCCESS){
				printf("Adding File  : %s\n",entry->d_name);

				/* Tag-File Relation */
				tag_file(dirname,full_name);
			}
		}
	}

	(void) closedir (directory);
	return IMP_SUCCESS;
}

/* import: Import Directory-File structure from File System to Kwest
 * param: char *path - Absolute path of Diectory to be imported
 * return: 1 on SUCCESS
 * author: @SG
 */
int import(const char *path)
{
	/* Extract Directory name from path */
	const char *dirname=strrchr(path,'/');

	/* Create Tag for directory to be imported */
	if(add_tag(dirname+1,USER_TAG)==KWEST_SUCCESS){
		printf("Creating Tag : %s\n",dirname+1);
	}

	if (import_semantics(path,dirname+1)==IMP_SUCCESS) {
		printf("Tag Imported\n");
		return IMP_SUCCESS;
	}

	printf("Operation Failed\n");
	return IMP_FAIL;
}
