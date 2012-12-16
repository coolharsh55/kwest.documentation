/* EXPORTING SEMANTICS */

/* EXECUTING:
	gcc export.c dbfunc.c -o export -l sqlite3
	./export Tag_Name Path
   EXAMPLE:
	./export Folder1 /home/admin/Folder2
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

#include<stdio.h>
#include<string.h>
#include<dirent.h>
#include<sys/stat.h>
#include<errno.h>
#include<fcntl.h>
#include<sys/sendfile.h>

#include "dbfunc.h"	/* Database Functions */

/* Database Functions Required:
 * node *getassociation(char *tag);
 * node *getfile(char *tag);
 * */

int send_file_fun(char *source, char *destination);

int main(int argc,const char *argv[])
{
	 if(argc<3) return -1;

	 initdb();

	 if(export_semantics(argv[1],argv[2]))
		printf("Tag Exported\n");
	 else
		printf("Operation Failed\n");

	 /* (void)closedb(); */
	 return 0;
}

/* export_semantics:
 * param:	char *tag - Tag name in Kwest
 *			char *path - Absolute Path of Location where tag is to be exported
 * return:	1 on SUCCESS
 * author:  @SG
 * */
int export_semantics(char *tag, char *path)
{
	 char *tagpath;
	 char temppath[_POSIX_PATH_MAX];
	 DIR *directory;
	 node *filenode,*tagnode,*temp;
	 
	 /* Check if path is valid */
	 if((directory = opendir(path)) == NULL) {
		printf("Error opening path\n");
		return 0;
	 }
	 if(closedir(directory) != 0)
		printf("Error closing path\n");
	 
	 /* Create Directory for tag */ 
	 printf("Creating Dir : %s\n",tag);
	 tagpath = strcat(strcat(strdup(path),"/"),tag);
	 if(mkdir(tagpath,0777) == -1 && errno != EEXIST) {
		printf("Error Creating Directory\n");
		return 0;
	 } 

	 /* Copy Files in Directory */
	 if((filenode=getfile(tag))!=NULL) {
		temp=filenode;
		strcpy(temppath,tagpath);
		strcat(temppath,"/");
		while(filenode) {
			printf("Copying File : %s\n",filenode->data);
			if(!send_file_fun(filenode->data,temppath)) 
				printf("There was an error copying file %s\n",filenode->data);
			filenode=filenode->next;
		}
		while(temp) { /* Deallocate Memory */
			filenode=temp->next;
			free(temp);
			temp=filenode;
		} 
	 }

	 /* Create Sub-Directory */ 
	 if((tagnode=getassociation(tag,"subgroup"))!=NULL) {
		temp=tagnode;
		while(tagnode) {	
			export_semantics(tagnode->data,tagpath);
			tagnode=tagnode->next;
		}
		while(temp)	{ /* Deallocate Memory */
			tagnode=temp->next;
			free(temp);
			temp=tagnode;
		} 
	 }
	 
	 return 1;
} 

/* send_file_fun : copies file from source to destination
 * param:	char *source - source of file
 *	 		char *destination - path where file is to be copied
 * return: 	1 on SUCCESS
 * author: 	@SG
 * */
int send_file_fun(char *source, char *destination)
{
	 int read_fd;
	 int write_fd;
	 struct stat stat_buf;
	 off_t offset = 0;
	 char out[_POSIX_PATH_MAX];

	 /* Open the input file */
	 if((read_fd=open(source, O_RDONLY)) == -1) {
		printf("Cannot open %s\n", source);
		return 0;
	 }
	 
	 /* Stat the input file to obtain its size */
	 if( fstat (read_fd, &stat_buf) == -1 ) {
		printf("Error fstat open %s\n", source);
		return 0;
	 }

	 /* Open the output file for writing, with the same permissions as the source file */
	 strcpy(out,destination); 
	 if( (write_fd = open(strcat(out,strrchr(source,'/')+1), O_WRONLY | O_CREAT, stat_buf.st_mode)) == -1 ) {
		printf("Cannot open dest %s\n", destination);
		return 0;
	 }

	 /* Blast the bytes from one file to the other */
	 if(sendfile(write_fd, read_fd, &offset, stat_buf.st_size) == -1)
		return 0; 

	 /* Close Files */
	 close (read_fd);
	 close (write_fd);
	 
	 return 1;
}
