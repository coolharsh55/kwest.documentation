/* IMPORTING SEMANTICS */

/* EXECUTING:
	gcc import.c dbfunc.c -o import -l sqlite3
	./import directory_path
   EXAMPLE: 
	./import /home/admin/Folder1
*/

/* LICENSE
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

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#include "dbfunc.h"	/* Datadase Functions */

/* Database Functions Required:
 * int initdb();
 * int closedb();
 * int addtag(char *tagname)
 * int addfile(char *abspath)
 * int tagfile(char *t,char *f)
 * int addassociation(char *t1,char *t2,char *relationtype)
 * */

int main(int argc, const char *argv[])
{
	 if (argc < 2) return -1;

	 initdb();

	 /* Extract Directory name from path */
	 char *dirname=strrchr(argv[1],'/');

	 if(addtag(dirname+1)==1)
		printf("Creating Tag : %s\n",dirname+1);

	 if (import_semantics(argv[1],dirname+1))
		printf("Tag Imported\n");
	 else
		printf("Operation Failed\n");

	 closedb();
	 return 0;
}

/* import_semantics:
 * param:	char *path - Absolute path of Diectory to be imported
 *			char *dirname - Name of Directory 
 * return:	1 on SUCCESS
 * author: 	@SG
 * */
int import_semantics(char *path, char *dirname)
{
	 DIR *directory;
	 struct dirent *entry; 
	 size_t path_len = strlen(path);
	 
	 directory = opendir(path);
	 if (directory != NULL) {
		while (entry = readdir (directory)) {
			struct stat fstat;
			char full_name[_POSIX_PATH_MAX + 1];
			size_t dir_len = strlen(entry->d_name);

			/* Calculate full name, check we are in file length limts */
			if ((path_len + dir_len + 1) > _POSIX_PATH_MAX)
				continue;

			strcpy(full_name, path);
			if (full_name[path_len - 1] != '/') 
				strcat(full_name, "/");
			strcat(full_name, entry->d_name);

			/* Ignore files starting with . */
			if((strchr(entry->d_name,'.')-entry->d_name)==0)
				continue;

			/* Ignore files ending with ~ */
			if((strrchr(entry->d_name,'~')-entry->d_name)==dir_len-1)
				continue;
		
			/* Get File Infromation : Returns 0 if successful */
			if (stat(full_name, &fstat) < 0)
				continue;
		
			if (S_ISDIR(fstat.st_mode)) {			/* Directory */
				if(addtag(entry->d_name)==1)
					printf("Creating Tag : %s\n",entry->d_name);
									
				/* Access Sub-Directories */
				(void) import_semantics(full_name,entry->d_name);
				/* Tag-Tag Relation */
				addassociation(entry->d_name,dirname,"subgroup");

			} else if(S_ISREG(fstat.st_mode)) {		/* Regular File */
				if(addfile(full_name)==1)
					printf("Adding File  : %s\n",entry->d_name);	 
				/* Tag-File Relation */
				tagfile(dirname,full_name);	
			}
		}
		(void) closedir (directory);
		return 1;
	 } 
	 else
		perror ("Couldn't open the directory");     
	 return 0;
}
