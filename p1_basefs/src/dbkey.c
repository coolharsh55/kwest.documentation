/* dbkey.c
 * handle requests for files and tags
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

#include "dbkey.h"
#include <stdlib.h>

/* set_file_id: Generate id for new file to be added in kwest
 * param: char *abspath - Absolute Path of File
 * return: int fno
 * author: @SG 
 */
int set_file_id(const char *abspath)
{
	sqlite3_stmt *stmt = NULL;
	char query[QUERY_SIZE];
	int status;
	int fno;

	/* Check if file exists */
	fno = get_file_id((strrchr(abspath,'/')+1)); /* Get File ID */
	if(fno != KWEST_FNF){ 
		return KWEST_FE; /* Return if file already exists */
	}

	/* Query to get maximum no of files existing in database */
	strcpy(query,"select max(fno) from FileDetails;");
	status = sqlite3_prepare_v2(get_kwdb(),query,-1,&stmt,0); 
	
	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW) {
		if((const char*)sqlite3_column_text(stmt,0) == NULL) { 
			sqlite3_finalize(stmt);
			return 1; /* NO ENTRY */
		} else { /* Increment file count */
			status = atoi((const char*)sqlite3_column_text(stmt,0))+1;
			sqlite3_finalize(stmt); 
			return status;
		} 
	}

	sqlite3_finalize(stmt);
	return KWEST_FAIL;
}

/* set_tag_id: Generate id for a new tag to be created in kwest
 * param: char *tagname
 * return: int tno 
 * author: @SG 
 */
int set_tag_id(const char *tagname,int tagtype)
{
	sqlite3_stmt *stmt = NULL;  
	char query[QUERY_SIZE];
	int status;
	const char *tmp; /* Hold result of query */
	int tno;

	/* Check if tag exists */
	tno = get_tag_id(tagname); /* Get Tag ID */
	if(tno != KWEST_TNF){
		return KWEST_TE; /* Return if tag already exists */
	}

	/* Check type of tag : USER / SYSTEM */
	if(tagtype == USER_TAG){
		/* Start adding user tag at tno = 100 */
		strcpy(query,
		       "select count(tno) from TagDetails where tno>99;");
		status = sqlite3_prepare_v2(get_kwdb(),query,-1,&stmt,0);
		
		status = sqlite3_step(stmt);
		if(status == SQLITE_ROW) {
			tmp = (const char*)sqlite3_column_text(stmt,0);
			if(atoi(tmp) == 0) { 
				sqlite3_finalize(stmt);
				return USER_TAG_START; /* NO ENTRY */
			} else { /* increment Tag ID */
				tno = atoi(tmp) + USER_TAG_START;
				sqlite3_finalize(stmt);
				return tno;
			}
		}
	} else if(tagtype == SYSTEM_TAG){
		/* Start adding system tag at tno = 0 */
		strcpy(query,
		       "select count(tno) from TagDetails where tno<100;");
		status = sqlite3_prepare_v2(get_kwdb(),query,-1,&stmt,0);
		
		status = sqlite3_step(stmt);
		if(status == SQLITE_ROW) {
			tmp = (const char*)sqlite3_column_text(stmt,0);
			if(tmp == NULL) { 
				sqlite3_finalize(stmt);
				return SYSTEM_TAG_START; /* NO ENTRY */
			} else { /* Increment Tag ID */
				tno = atoi(tmp) + SYSTEM_TAG_START;
				sqlite3_finalize(stmt);
				return tno;
			}
		}
	}

	sqlite3_finalize(stmt); 
	return KWEST_FAIL;
}

/* get_file_id: Return id for file in kwest
 * param: char *fname - Name of File
 * return: int fno 
 * author: @SG 
 */
int get_file_id(const char *fname)
{
	sqlite3_stmt *stmt = NULL;
	char query[QUERY_SIZE];
	int status;

	/* Query to get fno */
	strcpy(query,"select fno from FileDetails where fname = :fname;");
	sqlite3_prepare_v2(get_kwdb(),query,-1,&stmt,0);
	sqlite3_bind_text(stmt,1,fname,-1,SQLITE_STATIC);
	
	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW) { /* Return fno if file exists */
		status = atoi((const char*)sqlite3_column_text(stmt,0));
		sqlite3_finalize(stmt);
		return status;
	}

	sqlite3_finalize(stmt);
	return KWEST_FNF; /* File not found Error*/
}

/* get_tag_id: Return id for a tag in kwest
 * param: char *tagname
 * return: int tno 
 * author: @SG 
 */
int get_tag_id(const char *tagname)
{
	sqlite3_stmt *stmt = NULL;  
	char query[QUERY_SIZE];
	int status;

	/* Query to get tno */
	sprintf(query,"select tno from TagDetails where tagname = :tagname;");
	sqlite3_prepare_v2(get_kwdb(),query,-1,&stmt,0);
	sqlite3_bind_text(stmt,1,tagname,-1,SQLITE_STATIC);
	
	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW) { /* Return tno if tag exists */
		status = atoi((const char*)sqlite3_column_text(stmt,0));
		sqlite3_finalize(stmt);
		return status; 
	}

	sqlite3_finalize(stmt);
	return KWEST_TNF; /* Tag not found Error*/
}

/* get_file_name: Retrieve filename by its id
 * param: int fno - file number 
 * return: char *filename
 * author: @SG 
 */
const char *get_file_name(int fno)
{
	sqlite3_stmt *stmt;
	char query[QUERY_SIZE];
	int status;
	const char *filename = NULL;

	/* Query to get file name from fno */
	sprintf(query,"select fname from FileDetails where fno = %d;",fno);
	status = sqlite3_prepare_v2(get_kwdb(),query,-1,&stmt,0);

	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW){ /* return filename if file exists */
		filename = strdup((const char*)sqlite3_column_text(stmt,0));
	}

	sqlite3_finalize(stmt);
	return filename; /* Return Null */
}

/* get_tag_name: Retrieve tag name by its id
 * param: int tno - tag number
 * return: char *tagname
 * author: @SG 
 */
const char *get_tag_name(int tno)
{
	sqlite3_stmt *stmt;
	char query[QUERY_SIZE];
	int status;
	const char *tagname = NULL;

	/* Query to get tagname */
	sprintf(query,"select tagname from TagDetails where tno = %d",tno);
	status = sqlite3_prepare_v2(get_kwdb(),query,-1,&stmt,0);
	
	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW){ /* return tagname if tag exists */
		tagname = strdup((const char*)sqlite3_column_text(stmt,0));
	}

	sqlite3_finalize(stmt);
	return tagname; /* Return Null */
}

