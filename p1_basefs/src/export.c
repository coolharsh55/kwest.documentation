/* export.h
 * ./export tag_name absolute_path
 */

/* LICENSE
 * Copyright 2013 Sahil Gupta
 * Licensed under the Apache License, Version 2.0 (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * 	http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, 
 * software distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "export.h"

/* send_file_fun : copies file from source to destination
 * param: char *source - source of file
 * param: char *destination - path where file is to be copied
 * return: 1 on SUCCESS
 * author: @SG
 */
static int send_file(const char *source,const char *destination)
{
	int read_src;
	int write_dst;
	struct stat stat_buf;
	off_t offset = 0;
	char dst[_POSIX_PATH_MAX];
	 
	/* Open the input file */
	read_src = open(source, O_RDONLY);
	if(read_src == -1) {
		printf("Cannot open source file : %s\n", source);
		return EXPORT_COPY_FAIL;
	}
	 
	/* Call Stat on input file to obtain its size */
	if(fstat(read_src, &stat_buf) == -1) {
		printf("Error accessing source file : %s\n", source);
		return EXPORT_COPY_FAIL;
	}
	
	/* Open the dstput file for writing, 
	 * with the same permissions as the source file 
	 */
	strcpy(dst,destination);
	strcat(dst,strrchr(source,'/'));
	write_dst = open(dst , O_WRONLY | O_CREAT, stat_buf.st_mode);
	if( write_dst == -1 ) {
		printf("Cannot open destination file : %s\n", dst);
		return EXPORT_COPY_FAIL;
	}

	/* Blast the bytes from one file to the other */
	if(sendfile(write_dst, read_src, &offset, stat_buf.st_size) == -1){
		return EXPORT_COPY_FAIL; 
	}
	
	/* Close Files */
	close (read_src);
	close (write_dst);
	 
	return EXPORT_COPY_SUCCESS;
}

/* export: Exports a tag in kwest as Directory-file structure on FS
 * param: char *tag - Tag name in Kwest
 * param: char *path - Absolute Path of Location where tag is to be exported
 * return: EXPORT_SUCCESS on SUCCESS
 * author: @SG
 */
int export(const char *tag,const char *path)
{
	DIR *directory;
	const char *filename,*tagname,*filepath;
	char tagpath[_POSIX_PATH_MAX];
	void *ptr;

	/* Check if path is valid */
	if((directory = opendir(path)) == NULL) {
		printf("Error opening path\n");
		return EXPORT_FAIL;
	}
	if(closedir(directory) != 0){
		printf("Error closing path\n");
	}

	/* Form path for creating Directory */
	strcpy(tagpath,path);
	strcat(strcat(tagpath,"/"),tag);

	/* Create Directory for tag */ 
	printf("Creating Dir : %s\n",tag);
	if(mkdir(tagpath,0777) == -1 && errno != EEXIST) {
		printf("Error Creating Directory\n");
		return EXPORT_FAIL;
	} 

	/* Copy Files in Directory */
	ptr = get_fname_under_tag(tag); /* Get Path */
	while((filename = string_from_stmt(ptr))!= NULL) {
		filepath = get_abspath_by_fname(filename);
		if(send_file(filepath,tagpath) == EXPORT_COPY_FAIL){ 
			printf("Error copying file %s\n",filename);
		} else {
			printf("Copy File : %s\n",filename);
		}
	}

	/* Copy Sub-Directories */
	ptr = get_tags_by_association(tag,ASSOC_SUBGROUP); /* Get Tag */
	while((tagname = string_from_stmt(ptr))!= NULL) {
		/* Copy Sub-Directory contents */
		export(tagname,tagpath); 
	}

	return EXPORT_SUCCESS;
} 

int main(int argc,const char *argv[])
{
	if(argc<3) return -1;

	if(export(argv[1],argv[2]) == EXPORT_SUCCESS){
		printf("Tag Exported\n");
		close_db();
		return EXPORT_SUCCESS;
	}

	printf("Operation Failed\n");
	close_db(); 
	return EXPORT_FAIL;
}
