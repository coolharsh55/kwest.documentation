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
static char *get_entry_name(const char *path)
{
	return (strrchr(path, '/') + 1);
}
	


/* check_path_validity
 * checks whether current path is valid in database
 */
int check_path_validity(const char *path)
{
	/* @TODO
	 * write check_path_validity for FILES
	 * check associations between consecutive dir entries
	 */
	if(istag(get_entry_name(path)) == TRUE)
	{
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
	if(istag(get_entry_name(path)) == FALSE)
		return FALSE;
	return TRUE;
}


/* path_is_file
 * checks whether given path has a file entry
 */
BOOL path_is_file(const char *path)
{
	if(isfile(get_entry_name(path)) == FALSE) {
		return FALSE;
	}
	
	return TRUE;
}


/* get_absolute_path
 * returns absolute path of said file
 */
const char *get_absolute_path(const char *path)
{
	return get_abspath_by_fname(path);
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
char *readdir_files(const char *path)
{
	return KW_RETURN_ERROR;
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
	return KW_SUCCESS;

	return KW_FAIL;
}


/* make_directory
 * create a new directory
 */
int make_directory(const char *path, mode_t mode)
{
	return KW_SUCCESS;

	return KW_FAIL;
}


/* remove_directory
 * remove directory
 */
int remove_directory(const char *path)
{
	return KW_SUCCESS;

	return KW_FAIL;
}
