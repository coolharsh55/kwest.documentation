/* dbfuse.c
 * functions connecting fuse and database modules
 */

/* LICENSE
 * Copyright 2013 Harshvardhan Pandit
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

#include "dbfuse.h"

#include "flags.h"
#include "dbbasic.h"
#include "logging.h"

#include <string.h>
#include <sys/stat.h>


/* log_init
 * initialize logging
 */
BOOL _is_path_root(const char *path) 
{
	if(*(path+1) == '\0') {
		return TRUE;
	}
	else {
		return FALSE;
	}
}


/* get_entry_name
 * get last filename/tagname from path
 */
static const char *get_entry_name(const char *path)
{
	return (strrchr(path, '/') + 1);
}


/* check_path_validity
 * checks whether current path is valid in database
 */
int check_path_validity(const char *path)
{
	/* @TODO
	 * check associations between consecutive dir entries
	 */
	if(istag(get_entry_name(path)) == TRUE) {
		return KW_SUCCESS;
	} else if(isfile(get_entry_name(path)) == TRUE) {
		return KW_SUCCESS;
	} else {
		return -ENOENT;	
	}
	return KW_FAIL;
}


/* path_is_dir
 * checks whether given path has a directory entry
 */
BOOL path_is_dir(const char *path)
{
	if(istag(get_entry_name(path)) != TRUE)
		return FALSE;
	return TRUE;
}


/* path_is_file
 * checks whether given path has a file entry
 */
BOOL path_is_file(const char *path)
{
	if(isfile(get_entry_name(path)) != TRUE) {
		return FALSE;
	}
	
	return TRUE;
}


/* get_absolute_path
 * returns absolute path of said file
 */
const char *get_absolute_path(const char *path)
{
	return get_abspath_by_fname(get_entry_name(path));
}


/* readdir_dirs
 * get directory entries for said path
 */
char *readdir_dirs(const char *path, void **ptr)
{
	log_msg ("readdir_dirs: %s\n",path);
	if(*ptr == NULL) {
		log_msg("ptr is NULL\n");
		if(*(path + 1) == '\0') {
			log_msg("path is ROOT\n");
			*ptr = get_tags_by_association("root", ASSOC_SUBGROUP);
		} else {
			const char *t = strrchr(path,'/');
			*ptr = get_tags_by_association(t + 1, ASSOC_SUBGROUP); 
		}
		if(*ptr == NULL) {
			log_msg("pointer p is NULL\n");
			return NULL;
		}
	}
	
	return (char *)string_from_stmt(*ptr);
}


/* readdir_files
 * get file entries for said path
 */
char *readdir_files(const char *path, void **ptr)
{
	log_msg ("readdir_files: %s\n",path);
	if(*ptr == NULL) {
		log_msg("ptr is NULL\n");
		if(*(path + 1) == '\0') {
			log_msg("path is ROOT\n");
			*ptr = get_fname_under_tag("root");
		} else {
			const char *t = strrchr(path,'/');
			*ptr = get_fname_under_tag(t + 1); 
		}
		if(*ptr == NULL) {
			log_msg("pointer p is NULL\n");
			return NULL;
		}
	}
	
	return (char *)string_from_stmt(*ptr);
}


/* get_newfile_path
 * create a new file and return is absolute path
 */
const char *get_newfile_path(const char *path)
{
	return KW_RETURN_ERROR;
}


/* rename_file
 * rename the said file from -> to
 */
int rename_file(const char *from, const char *to)
{
	return KW_SUCCESS;

	return KW_FAIL;
}


/* remove_file
 * remove the said file
 */
int remove_this_file(const char *path)
{
	log_msg ("remove_this_file: %s\n",path);
	
	char *f = (char *)get_entry_name(path);
	log_msg ("filename: %s\n",f);
	
	strncpy((char *)path,path,(strlen(path)-strlen(f)+1));
	char *t = (char *)get_entry_name(path);
	log_msg ("tagname: %s\n",t);
	
	if(untag_file(t,f) == KW_SUCCESS) {
	return KW_SUCCESS;
	}
	
	return KW_FAIL;
}


/* make_directory
 * create a new directory
 */
int make_directory(const char *path, mode_t mode)
{
	char *newtag;
	char *parenttag;
	
	log_msg ("make_directory: %s\n",path);
	
	newtag = (char *)get_entry_name(path);
	log_msg ("new tagname: %s\n",newtag);
	
	if(add_tag(newtag,USER_TAG) != 1) {
		return KW_FAIL;
	}

	strncpy((char *)path, path, (strlen(path)-strlen(newtag) + 1));
	parenttag = (char *)get_entry_name(path);
	log_msg ("parent tagname: %s\n",parenttag);
	
	if(add_association(newtag, parenttag, ASSOC_SUBGROUP) != 1) {
		return KW_FAIL;
	}
	
	return KW_SUCCESS;
}


/* remove_directory
 * remove directory
 */
int remove_directory(const char *path)
{
	log_msg ("remove_directory: %s\n",path);
	
	if(remove_tag(get_entry_name(path)) == KW_SUCCESS) {
		return KW_SUCCESS;
	}
	
	return KW_FAIL;
}
