/* dbbasic.c
 * basic interation with database
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>    /* SQLITE Functions */

#include "dbinit.h"
#include "dbkey.h"
#include "extract_audio_taglib.h"

#include "dbbasic.h"
#include "logging.h"
#include "flags.h"
#include "magicstrings.h"


/* ---------------- ADD/REMOVE -------------------- */

/* add_tag: Create a new user tag in kwest 
 * param: char *tagname
 * return: 0 on SUCCESS
 * author: @SG
 */
int add_tag(const char *tagname,int tagtype)
{
	sqlite3_stmt *stmt;
	char query[QUERY_SIZE];
	int status;
	int tno; /* Tag ID */
	
	/* Call Function to set tno for Tag */
	if(tagtype == USER_TAG){ 
		tno = set_tag_id(tagname,USER_TAG); /* Add User Tag */ 
	} else if(tagtype == SYSTEM_TAG) { 
		tno = set_tag_id(tagname,SYSTEM_TAG); /* Add System Tag */ 
	}
	
	/* Return if Tag Exists */
	if(tno == KWEST_TE){
		return KWEST_TE;
	} 
	
	/* Insert (tno, tagname) in TagDetails Table */
	strcpy(query,"INSERT INTO TagDetails VALUES(:tno,:tagname);");
	sqlite3_prepare_v2(get_kwdb(),query,-1,&stmt,0);
	
	sqlite3_bind_int(stmt,1,tno);
	sqlite3_bind_text(stmt,2,tagname,-1,SQLITE_STATIC);
	
	status = sqlite3_step(stmt);
	if(status == SQLITE_DONE){
		sqlite3_finalize(stmt);
		return KW_SUCCESS;
	}
	
	sqlite3_finalize(stmt);
	return status;
}

/* remove_tag: Remove an existing tag from kwest 
 * param: char *tagname
 * return: 0 on SUCCESS
 * author: @SG
 */
int remove_tag(const char *tagname)
{
	char query[QUERY_SIZE];
	int status;
	int tno;
	
	tno = get_tag_id(tagname); /* Get Tag ID */
	
	/* Return if Tag does not Exists */
	if(tno == KWEST_TNF){ 
		printf("%s%s\n",TNF_MSG,tagname);
		return KWEST_TNF;
	} 
	
	/* Remove all Tag-Tag Associations */
	sprintf(query,"delete from TagAssociation where t1 = %d or t2 = %d;",
	        tno,tno);
	status = sqlite3_exec(get_kwdb(),query,0,0,0);
	
	/* Remove all File-Tag Associations */
	sprintf(query,"delete from FileAssociation where tno = %d;",tno);
	status = sqlite3_exec(get_kwdb(),query,0,0,0);
	
	/* Remove all Tag from database */
	sprintf(query,"delete from TagDetails where tno = %d;",tno);
	status = sqlite3_exec(get_kwdb(),query,0,0,0);
	
	return status;
}

/* add_file: Add file to kwest
 * param: char *abspath
 * return: 0 on SUCCESS
 * author: @SG
 */
int add_file(const char *abspath)
{
	sqlite3_stmt *stmt;
	char query[QUERY_SIZE];
	int status,status_meta;
	int fno;
	char *fname;
	struct METADATA_AUDIO M; /* Structure for holding metadata of a file */
	void *meta;
	
	fno = set_file_id(abspath); /* Call Function to set fno for File */
	fname = strrchr(abspath,'/')+1;
	
	if(fno == KWEST_FE){ /* Return if File already Exists */
		return KWEST_FE;
	} 
	
	/* Query : Insert (fno, fname, abspath) in FileDetails Table */
	strcpy(query,"INSERT INTO FileDetails VALUES(:fno,:fname,:abspath);");
	sqlite3_prepare_v2(get_kwdb(),query,-1,&stmt,0);
	
	sqlite3_bind_int(stmt,1,fno);
	sqlite3_bind_text(stmt,2,fname,-1,SQLITE_STATIC);
	sqlite3_bind_text(stmt,3,abspath,-1,SQLITE_STATIC);
	
	status = sqlite3_step(stmt);
	if(status != SQLITE_DONE){
		printf("%s%s\n",ADDF_MSG,fname);
		sqlite3_finalize(stmt);
		return status;
	}
	sqlite3_finalize(stmt);
	
	/* Get Metadata for file */
	meta = extract_metadata_file(abspath, &M);
	if(meta != NULL) {
		strcpy(query,"INSERT INTO Audio VALUES"
		             "(:fno,:title,:artist,:album,:genre);");
		sqlite3_prepare_v2(get_kwdb(),query,-1,&stmt,0); 
		
		sqlite3_bind_int(stmt,1,fno);
		sqlite3_bind_text(stmt,2,M.title,-1,SQLITE_STATIC);
		sqlite3_bind_text(stmt,3,M.artist,-1,SQLITE_STATIC);
		sqlite3_bind_text(stmt,4,M.album,-1,SQLITE_STATIC);
		sqlite3_bind_text(stmt,5,M.genre,-1,SQLITE_STATIC);
		
		status_meta = sqlite3_step(stmt);
		/* Handle if Error while Adding Metadata */
		if(status_meta != SQLITE_DONE){
			printf("%s%s\n",ADDM_MSG,fname);
		}
		extract_clear_strings(meta);
		sqlite3_finalize(stmt);
	}
	
	if(status == SQLITE_DONE && status_meta == SQLITE_DONE){
		return KW_SUCCESS;
	}
	
	return status;
}

/* remove_file: Remove file form kwest 
 * param: char *fname - File name
 * return: 0 on SUCCESS
 * author: @SG 
 */
int remove_file(const char *fname)
{
	char query[QUERY_SIZE];
	int status;
	int fno;
	
	fno = get_file_id(fname); /* Get File ID */
	
	if(fno == KWEST_FNF){ /* Return if File does not Exists */
		printf("%s%s\n",FNF_MSG,fname);
		return KWEST_FNF;
	} 
	
	/* Remove File-Tag Associations */
	sprintf(query,"delete from FileAssociation where fno = %d;",fno);
	status = sqlite3_exec(get_kwdb(),query,0,0,0); 
	
	/* Remove File-metadata from Database */
	sprintf(query,"delete from Audio where fno = %d;",fno);
	sqlite3_exec(get_kwdb(),query,0,0,0);
	
	/* Remove File from Database */
	sprintf(query,"delete from FileDetails where fno = %d;",fno);
	status = sqlite3_exec(get_kwdb(),query,0,0,0); 
	
	return status;
}

/* add_meta_info: Add new category to identify metadata 
 * param: filetype - Type of File
 * param: tag - Category of Metadata
 * return: 0 on SUCCESS
 * author: @SG  
 */
int add_meta_info(const char *filetype,const char *tag)
{
	sqlite3_stmt* stmt; 
	char query[QUERY_SIZE];
	int status;
	
	/* Check if info already exists */
	sprintf(query,
	      "select count(*) from MetaInfo where filetype = '%s' and tag = '%s';",
	      filetype,tag);
	sqlite3_prepare_v2(get_kwdb(),query,-1,&stmt,0);
	
	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW) {
		if(atoi((const char*)sqlite3_column_text(stmt,0))>0) {
			sqlite3_finalize(stmt);
			return KWEST_IE;
		}
	}
	sqlite3_finalize(stmt);
	
	/* Query to add metainfo */
	sprintf(query,"INSERT INTO MetaInfo VALUES('%s','%s');",filetype,tag);
	status = sqlite3_exec(get_kwdb(),query,0,0,0);
	
	return status;
}

/* --------------- Tag-File Relation ------------- */

/* tag_file: Associate a tag with a file 
 * param: char *t - tagname
 * param: char *f - filename
 * return: 1 on SUCCESS
 * author: @SG
 */
int tag_file(const char *t,const char *f)
{
	sqlite3_stmt* stmt; 
	char query[QUERY_SIZE];
	int status;
	int fno,tno;
	
	fno = get_file_id(f); /* Get File ID */
	if(fno == KWEST_FNF){ /* Return if File not found */
		printf("%s%s\n",FNF_MSG,f);
		return KWEST_FNF;
	}
	
	tno = get_tag_id(t); /* Get Tag ID */
	if(tno == KWEST_TNF){ /* Return if Tag not found */
		printf("%s%s\n",TNF_MSG,t);
		return KWEST_TNF;
	}
	
	/* Query : check if entry exists in File Association Table */
	sprintf(query,"select fno from FileAssociation where tno = %d;",tno);
	status = sqlite3_prepare_v2(get_kwdb(),query,-1,&stmt,0);
	
	while(1) { /* Query return all files under tag t */
		status = sqlite3_step(stmt);
		
		if(status == SQLITE_ROW) { /* check if f is in tag t */
			if(fno == atoi((const char*)sqlite3_column_text(stmt,0))){
				sqlite3_finalize(stmt);
				return KWEST_IE; /* File is already tagged */
			}
		} else if(status == SQLITE_DONE) {
			break; /* File not yet tagged */
		}
	}
	sqlite3_finalize(stmt);
	
	/* Query : add tno,fno to File Association Table */
	sprintf(query,"insert into FileAssociation values(%d,%d);",tno,fno);
	status = sqlite3_exec(get_kwdb(),query,0,0,0);
	
	return status;
}

/* untag_file: Remove the existing association between the tag and file
 * param: char *t - tagname
 * param: char *f - filename
 * return: 0 on SUCCESS
 * author: @SG
 */
int untag_file(const char *t,const char *f)
{
	char query[QUERY_SIZE];
	int status;
	int fno,tno;
	
	fno = get_file_id(f); /* Get File ID */
	if(fno == KWEST_FNF){ /* Return if File not found */
		printf("%s%s\n",FNF_MSG,f);
		return KWEST_FNF;
	}
	
	tno = get_tag_id(t); /* Get Tag ID */
	if(tno == KWEST_TNF){ /* Return if Tag not found */
		printf("%s%s\n",TNF_MSG,t);
		return KWEST_TNF;
	}
	
	/* Query to remove File-Tag Association */
	sprintf(query,"delete from FileAssociation where fno = %d and tno = %d;",
	        fno,tno);
	status = sqlite3_exec(get_kwdb(),query,0,0,0);
	
	return status;
}

/* get_fname_under_tag: Return list of files associated to given tag 
 * param: char *t - tagname
 * return: sqlite3_stmt pointer
 * author: @SG
 */
sqlite3_stmt *get_fname_under_tag(const char *t)
{
	sqlite3_stmt *stmt;
	char query[QUERY_SIZE];
	int status;
	int tno;
	
	tno = get_tag_id(t); /* Get Tag ID */
	if(tno == KWEST_TNF){ /* Return if Tag not found */
		printf("%s%s\n",TNF_MSG,t);
		return NULL;
	}
	
	/* Query to get all files associated with tag t */
	sprintf(query,
	        "select fname from FileDetails where fno in"
	        "(select fno from FileAssociation where tno = %d);",tno);
	status = sqlite3_prepare_v2(get_kwdb(),query,-1,&stmt,0);
	
	if(status != SQLITE_OK){ /* Error Preparing query */
		printf("Error : get_fname_under_tag\n");
		return NULL;
	}
	
	return stmt;
}

/* get_tags_for_file: Return list of tags associated with a given file
 * param: char *f - filename
 * return: sqlite3_stmt pointer
 * author: @SG 
 */
sqlite3_stmt *get_tags_for_file(const char *f)
{
	sqlite3_stmt *stmt;
	char query[QUERY_SIZE];
	int status;
	int fno;
	
	fno = get_file_id(f); /* Get File ID */
	if(fno == KWEST_FNF){ /* Return if File not found */
		printf("%s%s\n",FNF_MSG,f);
		return NULL;
	}
	
	/* Query to get all tags associated with file f */
	sprintf(query,"select tagname from TagDetails where tno in"
	        "(select tno from FileAssociation where fno = %d);",fno);
	status = sqlite3_prepare_v2(get_kwdb(),query,-1,&stmt,0);
	
	if(status != SQLITE_OK){ /* Error Preparing query */
		printf("Error : get_tags_for_file\n");
		return NULL;
	}
	
	return stmt;
}


/*----------------- Tag-Tag Relation ------------------*/

/* add_association: Associate a tag with another tag 
 * param: char *t1,*t2 - tagname of both tags to be associated
 * param: int associationid - relation between tags to be formed
 * return: 0 on SUCCESS
 * author: @SG
 */
int add_association(const char *t1,const char *t2,int associationid)
{
	char query[QUERY_SIZE];
	int status;
	int t1_id,t2_id;
	
	/* Return if relation Undefined */
	if(is_association_type(associationid) == 0){ 
		printf("%s%d\n",RNF_MSG,associationid);
		return KWEST_RNF;
	}
	
	t1_id = get_tag_id(t1); /* Get Tag ID for tag t1*/
	if(t1_id == KWEST_TNF){ /* Return if Tag not found */
		printf("%s%s\n",TNF_MSG,t1);
		return KWEST_TNF;
	}
	
	t2_id = get_tag_id(t2); /* Get Tag ID for tag t2*/
	if(t2_id == KWEST_TNF){ /* Return if Tag not found */
		printf("%s%s\n",TNF_MSG,t2);
		return KWEST_TNF;
	}
	
	/* Query : check if entry already exists in TagAssociation Table */
	if(get_association(t1,t2)!= KWEST_FAIL){
		return KWEST_IE;
	}
	
	/* Query : add (t1, t2, associationtype) to TagAssociation Table */
	sprintf(query,"insert into TagAssociation values(%d,%d,%d);",
	        t1_id,t2_id,associationid);
	status = sqlite3_exec(get_kwdb(),query,0,0,0);
	
	return status;
}

/* remove_association: Remove the existing association between the two tags 
 * param: char *t1,*t2 - tagname of both tags whose associated is to be removed
 * param: int associationid - relation between tags to be removed
 * return: 0 on SUCCESS
 * author: @SG
 */
int remove_association(const char *t1,const char *t2,int associationid)
{
	char query[QUERY_SIZE];
	int status;
	int t1_id,t2_id;
	
	/* Return if relation Undefined */
	if(is_association_type(associationid) == 0){ 
		printf("%s%d\n",RNF_MSG,associationid);
		return KWEST_RNF;
	}
	
	t1_id = get_tag_id(t1); /* Get Tag ID for tag t1*/
	if(t1_id == KWEST_TNF){ /* Return if Tag not found */
		printf("%s%s\n",TNF_MSG,t1);
		return KWEST_TNF;
	}
	
	t2_id = get_tag_id(t2); /* Get Tag ID for tag t2*/
	if(t2_id == KWEST_TNF){ /* Return if Tag not found */
		printf("%s%s\n",TNF_MSG,t2);
		return KWEST_TNF;
	}
	
	/* Query to remove association between t1 and t2 */
	sprintf(query,"delete from TagAssociation where "
	              "t1 = %d and t2 = %d and associationid = %d;",
	              t1_id,t2_id,associationid);
	status = sqlite3_exec(get_kwdb(),query,0,0,0);
	
	return status;
}

/* get_association: Return type of association between the two tags
 * param: char *t1,*t2 - tagname of both tags in association
 * return: int associationid
 * author: @SG
 */
int get_association(const char *t1,const char *t2)
{
	sqlite3_stmt* stmt; 
	char query[QUERY_SIZE];
	int status;
	int t1_id,t2_id,associationid;
	
	t1_id = get_tag_id(t1); /* Get Tag ID for tag t1*/
	if(t1_id == KWEST_TNF){ /* Return if Tag not found */
		printf("%s%s\n",TNF_MSG,t1);
		return KWEST_TNF;
	}
	
	t2_id = get_tag_id(t2); /* Get Tag ID for tag t2*/
	if(t2_id == KWEST_TNF){ /* Return if Tag not found */
		printf("%s%s\n",TNF_MSG,t2);
		return KWEST_TNF;
	}
	
	/* Query to get association between t1 and t2 */
	sprintf(query,
	      "select associationid from TagAssociation where t1 = %d and t2 = %d;",
	        t1_id,t2_id);
	status = sqlite3_prepare_v2(get_kwdb(),query,-1,&stmt,0);
	
	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW) {
		associationid = atoi((const char*)sqlite3_column_text(stmt,0));
		sqlite3_finalize(stmt);
		return associationid; /* return associationid */
	}
	
	sqlite3_finalize(stmt);
	return KWEST_FAIL; /*No association between tags*/
}

/* get_tags_by_association: Get tags having association with another tag 
 * param: char *t - tagname
 * param: int associationid - relation between tags
 * return: sqlite3_stmt pointer
 * author: @SG 
 */
sqlite3_stmt *get_tags_by_association(const char *t,int associationid)
{
	sqlite3_stmt* stmt; 
	char query[QUERY_SIZE];
	int status;
	int t_id;
	
	/* Return if relation Undefined */
	if(is_association_type(associationid) == 0){ 
		printf("%s%d\n",RNF_MSG,associationid);
		return NULL;
	}
	
	t_id = get_tag_id(t); /* Get Tag ID */
	if(t_id == KWEST_TNF){ /* Return if Tag not found */
		printf("%s%s\n",TNF_MSG,t);
		return NULL;
	}
	
	/* Query to get tags associated with tag t */
	sprintf(query,"select tagname from TagDetails where tno in"
	              "(select t1 from TagAssociation where "
	              "t2 = %d and associationid = %d);",
	              t_id,associationid);
	status = sqlite3_prepare_v2(get_kwdb(),query,-1,&stmt,0);
	
	if(status != SQLITE_OK){ /* Error Preparing query */
		return NULL;
	}
	
	return stmt;
}

/* ----------------- Associations --------------------- */

/* add_association_type: Create a new association type 
 * param: char *associationtype
 * return: int associationid
 * author: @SG 
 */
int add_association_type(const char *associationtype)
{
	sqlite3_stmt *stmt;
	char query[QUERY_SIZE];
	int status;
	int associationid = 0;
	
	/* Query : check if entry exists in Associations Table */  
	sprintf(query,"select count(associationid) from Associations where "
	              "associationtype = '%s';",associationtype);
	status = sqlite3_prepare_v2(get_kwdb(),query,-1,&stmt,0); 
	
	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW) {
		if(atoi((const char*)sqlite3_column_text(stmt,0))!= 0) {
			sqlite3_finalize(stmt);
			return KWEST_IE; /* Relation Exists */
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	
	/* Query to get maximum relation id existing in database */
	strcpy(query,"select max(associationid) from Associations;");
	status = sqlite3_prepare_v2(get_kwdb(),query,-1,&stmt,0); 
	
	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW) {
		if((const char*)sqlite3_column_text(stmt,0) == NULL){ 
			associationid = 0; /* First Entry */
		} else {
			associationid = 
			    atoi((const char*)sqlite3_column_text(stmt,0))+1;
		}
		sqlite3_finalize(stmt); 
		
		/* Query:add (assocnid,assocntype) to Association Table */
		sprintf(query,"INSERT INTO Associations VALUES(%d,'%s');",
		        associationid,associationtype);
		status = sqlite3_exec(get_kwdb(),query,0,0,0);
		return status;
	}
	
	sqlite3_finalize(stmt); 
	return KWEST_FAIL;
}

/* is_association_type: Check if relation exists
 * param: int associationid
 * return: 1 if relation present
 * author: @SG
 */
int is_association_type(int associationid)
{
	sqlite3_stmt *stmt;
	char query[QUERY_SIZE];
	int status;
	
	sprintf(query,"select count(associationid) from Association where "
	              "associationid = %d;",associationid);
	status = sqlite3_prepare_v2(get_kwdb(),query,-1,&stmt,0); 
	
	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW) {
		associationid = atoi((const char*)sqlite3_column_text(stmt,0));
		sqlite3_finalize(stmt);
		return associationid;
	}
	
	sqlite3_finalize(stmt);
	return KWEST_FAIL;

}

/* --------------------- Others --------------------- */

/* list_user_tags: List all tags in the system 
 * param: void
 * return: sqlite3_stmt *
 * author: @SG
 */
sqlite3_stmt *list_user_tags(void)
{
	sqlite3_stmt * stmt;
	char query[QUERY_SIZE];
	int status;
	
	/* Query to get user tags from Database */
	strcpy(query,"select tagname from TagDetails where tno>99;");
	status = sqlite3_prepare_v2(get_kwdb(),query,-1,&stmt,0);
	
	if(status != SQLITE_OK){ /* Error Preparing query */
		printf("Error : list_user_tags\n");
		return NULL;
	}
	
	return stmt;
}

/* associate_file_metadata: Form association for metadata in file
 * param: filetype - Type of File
 * param: tag - Metadata Category
 * return: 0 on SUCCESS
 * author: @SG
 */
int associate_file_metadata(const char *filetype,const char *tag)
{
	sqlite3_stmt* stmt;
	char query[QUERY_SIZE];
	int status;
	const char *tmp; /* Hold result of query */
	char *newtag=NULL;
	
	/* Query : get all metadata under metadata-category */
	sprintf(query,"select %s from %s;",tag,filetype);
	sqlite3_prepare_v2(get_kwdb(),query,-1,&stmt,0);
	
	while(1){
		status = sqlite3_step(stmt);
		if(status == SQLITE_ROW) {
			tmp = (const char*)sqlite3_column_text(stmt,0);
			if(strrchr(tmp,'/')!= NULL){ /* Special Character */
				continue;
			} 
			if(strcmp(tmp,"") == 0){ /* No meta information */
				newtag=strdup(TAG_UNKNOWN);
				newtag=strcat(newtag,tag);
				/* Create Tag Unknown */
				add_tag(newtag,SYSTEM_TAG); 
				/* Associate Tag Unknown with File Type*/
				add_association(newtag,tag,ASSOC_SUBGROUP);
				/* Tag File to Unknown Tag */
				tag_file_with_metadata(filetype,tag,tmp);
				free((char *)newtag);
			} else { /* Metadata Exist */
				/* Create Tag for Metadata */
				add_tag(tmp,USER_TAG);
				/* Associate Metadata tag with File Type */
				add_association(tmp,tag,ASSOC_SUBGROUP);
				/* Tag File to Metadata Tag */
				tag_file_with_metadata(filetype,tag,tmp);
			}
		} else if(status == SQLITE_DONE){
			sqlite3_finalize(stmt);
			return status;
		} else {
			sqlite3_finalize(stmt);
			return KWEST_FAIL;
		}
	}
	
	return KWEST_FAIL;
}

/* tag_file_with_metadata: Form association between file and associated metadata
 * param: tag - Metadata Category
 * param: name - Metadata
 * return: 1 on SUCCESS
 * author: @SG
 */
int tag_file_with_metadata(const char *filetype,const char *tag,
                           const char *name)
{
	sqlite3_stmt* stmt; 
	char query[QUERY_SIZE];
	int status;
	const char *tmp; /* hold absolute path */
	char *newtag=NULL;
	
	/* Query : get file to be tagged under Metadata Category */
	sprintf(query,"select fno from %s where %s = :name;",filetype,tag);
	sqlite3_prepare_v2(get_kwdb(),query,-1,&stmt,0);
	sqlite3_bind_text(stmt,1,name,-1,SQLITE_STATIC);
	
	while(1){ /* If multile files present under same Metadata Category*/
		status = sqlite3_step(stmt);
		
		if(status == SQLITE_ROW) {
			tmp = get_file_name( /* Get absolute path */
			    atoi((const char*)sqlite3_column_text(stmt,0)));
			if((strcmp(name,"") == 0) || (strcmp(name,tag) == 0)){ 
				/* No Metadata exist */
				newtag=strdup(TAG_UNKNOWN);
				newtag=strcat(newtag,tag);
				tag_file(newtag,tmp); /* Tag under Unknown */
				free((char *)newtag);
			} else {
				/* Metadata exist */
				tag_file(name,tmp); /* Tag File to Metadata */
			}
			free((char *)tmp);
		} else if(status == SQLITE_DONE){
			sqlite3_finalize(stmt);
			return status;
		} else {
			sqlite3_finalize(stmt);
			return KWEST_FAIL;
		}
	}
	
	return KWEST_FAIL;
}

/* string_from_stmt: Returns data for multiple rows is query
 * param: sqlite3_stmt *stmt - statement holding query
 * return: const char* data returned by query
 * author: @SG 
 */
const char* string_from_stmt(sqlite3_stmt *stmt)
{
	int status;
	if(stmt == NULL) {
		log_msg("string_from_stmt: stmt is NULL");
	}
	
	status = sqlite3_step(stmt); /* Execute Query */
	if(status == SQLITE_ROW){ /* Return data if present */
		return (const char*)sqlite3_column_text(stmt,0);
	} else { /* Return NULL to mark end of Data */
		sqlite3_finalize(stmt);
		return NULL;
	}
}

/* -------------------- Fuse Functions --------------------- */

/* istag: Check if given tag is present in system
 * param: char *t - tagname
 * return: 1 if tag present
 * author: @SG 
 */
int istag(const char *t)
{
	sqlite3_stmt *stmt;
	char query[QUERY_SIZE];
	int status;
	int tmp;  /* To Hold result of Query */
	
	/* check if tag with name t exist */
	sprintf(query,"select count(*) from TagDetails where tagname = :t;");
	sqlite3_prepare_v2(get_kwdb(),query,-1,&stmt,0);
	sqlite3_bind_text(stmt,1,t,-1,SQLITE_STATIC);
	
	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW) {
		tmp = atoi((const char*)sqlite3_column_text(stmt,0));
		sqlite3_finalize(stmt);
		return tmp;
	}
	
	sqlite3_finalize(stmt);
	return KWEST_FAIL;
}

/* isfile: Check if given file is present in system
 * param: char *f - filename
 * return: 1 if file present
 * author: @SG
 */
int isfile(const char *f)
{
	sqlite3_stmt *stmt;
	char query[QUERY_SIZE];
	int status;
	int tmp; /* To Hold result of Query */
	
	/* check if file with name f exist */
	strcpy(query,"select count(*) from FileDetails where fname = :f;");
	sqlite3_prepare_v2(get_kwdb(),query,-1,&stmt,0);
	sqlite3_bind_text(stmt,1,f,-1,SQLITE_STATIC);
	
	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW) {
		tmp = atoi((const char*)sqlite3_column_text(stmt,0));
		sqlite3_finalize(stmt);
		return tmp;
	}
	
	sqlite3_finalize(stmt);
	return KWEST_FAIL;
}

/* get_abspath_by_fname: return absolute path of file
 * param: char *path - kwest path
 * return: char * absolute path
 * author: @SG 
 */
char *get_abspath_by_fname(const char *path)
{
	sqlite3_stmt *stmt;
	char query[QUERY_SIZE];
	int status;
	const char *tmp; /* To hold filename */
	char *abspath;
	
	/*TODO: Ckeck if path is valid*/
	
	/* Get filename from path */
	if((tmp = strrchr(path,'/')) == NULL){
		tmp = path;
	} else {
		tmp = tmp+1;
	}
	
	/* Query to get absolute path from file name */
	strcpy(query,"select abspath from FileDetails where fname = :tmp;");
	sqlite3_prepare_v2(get_kwdb(),query,-1,&stmt,0);
	sqlite3_bind_text(stmt,1,tmp,-1,SQLITE_STATIC);
	
	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW) {
		abspath = strdup((const char*)sqlite3_column_text(stmt,0));
		sqlite3_finalize(stmt);
		return abspath;
	}
	
	sqlite3_finalize(stmt);
	return NULL;
}

int check_path(const char *path)
{
	/*
	char *tmp = path+1;
	char *tag1,*tag2;
	int assocn;
	
	while((tag1 = strchr(tmp,'/')) != NULL)
	{
		tmp = strchr(tmp,'/')+1;
		tag2 = strchr(tmp,'/');
		if(tag2 == NULL){
			if(istag(tag1)==1) {
				return 1;
			}
		} 
		assocn = get_association(tag1,tag2);
		if(assocn < 0) return 0; 
	}
	*/
	return 1;
}
