/* DATABASE FUNCTIONS */
/* gcc -Wall -c dbfunc.c -o dbfunc.o -lsqlite3 */

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

#include "dbfunc.h"


/* ------------ Database Connections ---------------- */

/* create_db: Create Kwest database for first use
 * return: 0 on SUCCESS
 * author: @SG
 */
int create_db(void)
{
	char query[QUERY_SIZE];
	int status;

	strcpy(query,"CREATE TABLE IF NOT EXISTS FileDetails "
	"(fno INTEGER PRIMARY KEY,f_name VARCHAR2(50),abspath VARCHAR2(100));");
	status = sqlite3_exec(db,query,0,0,0);

	strcpy(query,"CREATE TABLE IF NOT EXISTS TagDetails "
	"(tno INTEGER PRIMARY KEY,tagname VARCHAR2(50));");
	status = sqlite3_exec(db,query,0,0,0);

	strcpy(query,"CREATE TABLE IF NOT EXISTS FileAssociation "
	"(tno INTEGER,fno INTEGER);");
	status = sqlite3_exec(db,query,0,0,0);

	strcpy(query,"CREATE TABLE IF NOT EXISTS TagAssociation "
	"(t1 INTEGER,t2 INTEGER,relationid INTEGER);");
	status = sqlite3_exec(db,query,0,0,0);

	strcpy(query,"CREATE TABLE IF NOT EXISTS MetaInfo "
	"(filetype VARCHAR2(20),tag VARCHAR2(20));");
	status = sqlite3_exec(db,query,0,0,0);

	strcpy(query,"CREATE TABLE IF NOT EXISTS Audio "
	"(fno INTEGER,title VARCHAR2(75),artist VARCHAR2(75),"
	"album VARCHAR2(75),genre VARCHAR2(75));");
	status = sqlite3_exec(db,query,0,0,0);

	strcpy(query,"CREATE TABLE IF NOT EXISTS Associations "
	"(relationid INTEGER PRIMARY KEY,relationtype VARCHAR2(20));");
	status = sqlite3_exec(db,query,0,0,0);

	/* Possible Tag-Tag Relations */
	add_relation("system");
	add_relation("probably_related");
	add_relation("subgroup");
	add_relation("related");
	add_relation("not_related");

	return status;
}

/* init_db: Establish Kwest Database Connection
 * return: 0 on SUCCESS
 * author: @SG
 */
int init_db(void)
{
	int status;
	struct passwd *pw;
	const char *homedir;
	const char *kwestdir;

	/* Set path for database file to /home/user/.config */
	pw = getpwuid(getuid());
	homedir = pw->pw_dir; /* initial working directory */
	kwestdir = strdup(strcat((char *)homedir,"/.config/kwestdb"));

	status = sqlite3_open(kwestdir,&db);
	sqlite3_exec(db,"BEGIN",0,0,0); /* Begin Transaction */

	if(status != SQLITE_OK) {
		printf("Database Connection Failed. Error Code : %d\n",status);
	}
	
	return status;
}

/* close_db: Close Kwest Database Connection
 * return: 0 on SUCCESS
 * author: @SG
 */
int close_db(void)
{
	int status;

	sqlite3_exec(db,"COMMIT",0,0,0); /* End Transaction */
	status = sqlite3_close(db);

	if (status != SQLITE_OK) {  
		printf("Error Closing Database\nError Code : %d\n",status);
	}

	return status;
}

/* ---------------- ADD/REMOVE -------------------- */

/* add_tag: Create a new user tag in kwest 
 * param: char *tagname
 * return: 0 on SUCCESS
 * author: @SG
 */
int add_tag(const char *tagname,int tagtype)
{
	char query[QUERY_SIZE];
	int status;
	int tno; /* Tag ID */

	/* Call Function to set tno for Tag */
	if(tagtype==USER_TAG){ 
		tno=set_tag_id(tagname,USER_TAG); /* Add User Tag */ 
	} else if(tagtype==SYSTEM_TAG) { 
		tno=set_tag_id(tagname,SYSTEM_TAG); /* Add System Tag */ 
	}

	/* Return if Tag Exists */
	if(tno == KWEST_TE){
		return KWEST_TE;
	} 

	/* Insert (tno, tagname) in TagDetails Table */
	sprintf(query,"INSERT INTO TagDetails VALUES(%d,'%s');",tno,tagname);
	status = sqlite3_exec(db,query,0,0,0);

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
		printf("Tag Not Found : %s\n",tagname);
		return KWEST_TNF;
	} 

	/* Remove all Tag-Tag Associations */
	sprintf(query,"delete from TagAssociation where t1=%d or t2=%d;",
	        tno,tno);
	status = sqlite3_exec(db,query,0,0,0);

	/* Remove all File-Tag Associations */
	sprintf(query,"delete from FileAssociation where tno=%d;",tno);
	status = sqlite3_exec(db,query,0,0,0);

	/* Remove all Tag from database */
	sprintf(query,"delete from TagDetails where tno=%d;",tno);
	status = sqlite3_exec(db,query,0,0,0);

	return status;
}

/* add_file: Add file to kwest
 * param: char *abspath
 * return: 0 on SUCCESS
 * author: @SG
 */
int add_file(const char *abspath)
{
	char query[QUERY_SIZE];
	int status,status_meta;
	int fno;
	struct METADATA_AUDIO M; /* Structure for holding metadata of a file */
	void *meta;

	fno=set_file_id(abspath); /* Call Function to set fno for File */

	if(fno == KWEST_FE){ /* Return if File already Exists */
		return KWEST_FE;
	} 

	/* TODO : Deal with special characters */
	
	/* Query : Insert (fno, f_name, abspath) in FileDetails Table */
	sprintf(query,"INSERT INTO FileDetails VALUES(%d,'%s','%s');",
	        fno,strrchr(abspath,'/')+1,abspath);
	status = sqlite3_exec(db,query,0,0,0);

	if(status!=SQLITE_OK){
		printf("Error adding file : %s\n",strrchr(abspath,'/')+1);
		return status;
	}

	/* Get Metadata for file */
	meta = extract_metadata_file(abspath, &M);
	if(meta != NULL) {
		sprintf(query,
		        "INSERT INTO Audio VALUES(%d,'%s','%s','%s','%s');"
		        ,fno,M.title,M.artist,M.album,M.genre);
		status_meta = sqlite3_exec(db,query,0,0,0); 

		/* Handle if Error while Adding Metadata */
		if(status_meta != SQLITE_OK){
			printf("Error adding metadata for file %s\n",
			strrchr(abspath,'/')+1);
		}
		_extract_clear_strings(meta);
	}

	return status;
}

/* remove_file: Remove file form kwest 
 * param: char *path - Path in Kwest
 * return: 0 on SUCCESS
 * author: @SG 
 */
int remove_file(const char *path)
{
	char query[QUERY_SIZE];
	int status;
	int fno;

	fno=get_file_id(path); /* Get File ID */

	if(fno==KWEST_FNF){ /* Return if File does not Exists */
		printf("File Not Found : %s\n",path);
		return KWEST_FNF;
	} 

	/* Remove File-Tag Associations */
	sprintf(query,"delete from FileAssociation where fno=%d;",fno);
	status = sqlite3_exec(db,query,0,0,0); 

	/* Remove File-metadata from Database */
	sprintf(query,"delete from Audio where fno=%d;",fno);
	sqlite3_exec(db,query,0,0,0);

	/* Remove File from Database */
	sprintf(query,"delete from FileDetails where fno=%d;",fno);
	status = sqlite3_exec(db,query,0,0,0); 

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
	      "select count(*) from MetaInfo where filetype='%s' and tag='%s';",
	      filetype,tag);
	sqlite3_prepare_v2(db,query,-1,&stmt,0);

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
	status = sqlite3_exec(db,query,0,0,0);

	return status;
}

/* remove_meta_info: remove category identifying metadata 
 * param: filetype - Type of File
 * param: tag - Category of Metadata
 * return: 0 on SUCCESS
 * author:  
 */
int remove_meta_info(const char *filetype,const char *tag)
{
	return KWEST_FAIL;
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

	fno=get_file_id(f); /* Get File ID */
	if(fno==KWEST_FNF){ /* Return if File not found */
		printf("Tag : %s File Not Found : %s\n",t,f);
		return KWEST_FNF;
	}

	tno=get_tag_id(t); /* Get Tag ID */
	if(tno==KWEST_TNF){ /* Return if Tag not found */
		printf("Tag Not Found : %s\n",t);
		return KWEST_TNF;
	}

	/* Query : check if entry exists in File Association Table */
	sprintf(query,"select fno from FileAssociation where tno=%d;",tno);
	status = sqlite3_prepare_v2(db,query,-1,&stmt,0);
	
	while(1) { /* Query return all files under tag t */
		status = sqlite3_step(stmt);
  
		if(status == SQLITE_ROW) { /* check if f is in tag t */
			if(fno==atoi((const char*)sqlite3_column_text(stmt,0))){
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
	status = sqlite3_exec(db,query,0,0,0);

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
		printf("File Not Found : %s\n",f);
		return KWEST_FNF;
	}

	tno = get_tag_id(t); /* Get Tag ID */
	if(tno == KWEST_TNF){ /* Return if Tag not found */
		printf("Tag Not Found : %s\n",t);
		return KWEST_TNF;
	}

	/* Query to remove File-Tag Association */
	sprintf(query,"delete from FileAssociation where fno=%d and tno=%d;",
	        fno,tno);
	status = sqlite3_exec(db,query,0,0,0);
	
	return status;
}

/* get_file: Return list of files associated to given tag 
 * param: char *t - tagname
 * return: sqlite3_stmt pointer
 * author: @SG
 */
sqlite3_stmt *get_file(const char *t)
{
	sqlite3_stmt *stmt;
	char query[QUERY_SIZE];
	int status;
	int tno;

	tno = get_tag_id(t); /* Get Tag ID */
	if(tno == KWEST_TNF){ /* Return if Tag not found */
		printf("Tag Not Found : %s\n",t);
		return NULL;
	}

	/* Query to get all files associated with tag t */
	sprintf(query,
	        "select f_name from FileDetails where fno in"
	        "(select fno from FileAssociation where tno=%d);",tno);
	status = sqlite3_prepare_v2(db,query,-1,&stmt,0);

	if(status != SQLITE_OK){ /* Error Preparing query */
		printf("Error : get_file\n");
		return NULL;
	}
	
	return stmt;
}

/* get_file_abspath: Return list of files associated to given tag 
 * param: char *t - tagname
 * return: sqlite3_stmt pointer
 * author: @SG
 */
sqlite3_stmt *get_file_abspath(const char *t)
{
	sqlite3_stmt *stmt;
	char query[QUERY_SIZE];
	int status;
	int tno;

	tno = get_tag_id(t); /* Get Tag ID */
	if(tno == KWEST_TNF){ /* Return if Tag not found */
		printf("Tag Not Found : %s\n",t);
		return NULL;
	}

	/* Query to get all files associated with tag t */
	sprintf(query,
	        "select abspath from FileDetails where fno in"
	        "(select fno from FileAssociation where tno=%d);",tno);
	status = sqlite3_prepare_v2(db,query,-1,&stmt,0);

	if(status != SQLITE_OK){ /* Error Preparing query */
		printf("Error : get_file\n");
		return NULL;
	}
	
	return stmt;
}

/* get_tag: Return list of tags associated with a given file
 * param: char *f - filename
 * return: sqlite3_stmt pointer
 * author: @SG 
 */
sqlite3_stmt *get_tag(const char *f)
{
	sqlite3_stmt *stmt;
	char query[QUERY_SIZE];
	int status;
	int fno;

	fno=get_file_id(f); /* Get File ID */
	if(fno==KWEST_FNF){ /* Return if File not found */
		printf("File Not Found : %s\n",f);
		return NULL;
	}

	/* Query to get all tags associated with file f */
	sprintf(query,"select tagname from TagDetails where tno in"
	        "(select tno from FileAssociation where fno=%d);",fno);
	status = sqlite3_prepare_v2(db,query,-1,&stmt,0);
	
	if(status != SQLITE_OK){ /* Error Preparing query */
		printf("Error : get_tag\n");
		return NULL;
	}
	
	return stmt;
}


/*----------------- Tag-Tag Relation ------------------*/

/* add_association: Associate a tag with another tag 
 * param: char *t1,*t2 - tagname of both tags to be associated
 * param: int relationid - relation between tags to be formed
 * return: 0 on SUCCESS
 * author: @SG
 */
int add_association(const char *t1,const char *t2,int relationid)
{
	char query[QUERY_SIZE];
	int status;
	int t1_id,t2_id;
	
	if(isrelation(relationid)==0){ /* Return if relation Undefined */
		printf("Unknown Relation Type : %d\n",relationid);
		return KWEST_RNF;
	}

	t1_id=get_tag_id(t1); /* Get Tag ID for tag t1*/
	if(t1_id==KWEST_TNF){ /* Return if Tag not found */
		printf("Tag Not Found : %s\n",t1);
		return KWEST_TNF;
	}

	t2_id=get_tag_id(t2); /* Get Tag ID for tag t2*/
	if(t2_id==KWEST_TNF){ /* Return if Tag not found */
		printf("Tag Not Found : %s\n",t2);
		return KWEST_TNF;
	}

	/* Query : check if entry already exists in TagAssociation Table */
	if(get_association_relation(t1,t2)!=KWEST_FAIL){
		return KWEST_IE;
	}

	/* Query : add (t1, t2, relationtype) to TagAssociation Table */
	sprintf(query,"insert into TagAssociation values(%d,%d,%d);",
	        t1_id,t2_id,relationid);
	status = sqlite3_exec(db,query,0,0,0);

	return status;
}

/* remove_association: Remove the existing association between the two tags 
 * param: char *t1,*t2 - tagname of both tags whose associated is to be removed
 * param: int relationid - relation between tags to be removed
 * return: 0 on SUCCESS
 * author: @SG
 */
int remove_association(const char *t1,const char *t2,int relationid)
{
	char query[QUERY_SIZE];
	int status;
	int t1_id,t2_id;

	if(isrelation(relationid)==0){ /* Return if relation Undefined */
		printf("Unknown Relation Type : %d\n",relationid);
		return KWEST_RNF;
	}

	t1_id=get_tag_id(t1); /* Get Tag ID for tag t1*/
	if(t1_id==KWEST_TNF){ /* Return if Tag not found */
		printf("Tag Not Found : %s\n",t1);
		return KWEST_TNF;
	}

	t2_id=get_tag_id(t2); /* Get Tag ID for tag t2*/
	if(t2_id==KWEST_TNF){ /* Return if Tag not found */
		printf("Tag Not Found : %s\n",t2);
		return KWEST_TNF;
	}

	/* Query to remove association between t1 and t2 */
	sprintf(query,
	  "delete from TagAssociation where t1=%d and t2=%d and relationid=%d;",
	  t1_id,t2_id,relationid);
	status = sqlite3_exec(db,query,0,0,0);

	return status;
}

/* get_association_relation: Return type of association between the two tags
 * param: char *t1,*t2 - tagname of both tags in association
 * return: int relationid
 * author: @SG
 */
int get_association_relation(const char *t1,const char *t2)
{
	sqlite3_stmt* stmt; 
	char query[QUERY_SIZE];
	int status;
	int t1_id,t2_id,relationid;

	t1_id=get_tag_id(t1); /* Get Tag ID for tag t1*/
	if(t1_id==KWEST_TNF){ /* Return if Tag not found */
		printf("Tag Not Found : %s\n",t1);
		return KWEST_TNF;
	}

	t2_id=get_tag_id(t2); /* Get Tag ID for tag t2*/
	if(t2_id==KWEST_TNF){ /* Return if Tag not found */
		printf("Tag Not Found : %s\n",t2);
		return KWEST_TNF;
	}

	/* Query to get association between t1 and t2 */
	sprintf(query,
	        "select relationid from TagAssociation where t1=%d and t2=%d;",
	        t1_id,t2_id);
	status = sqlite3_prepare_v2(db,query,-1,&stmt,0);

	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW) {
		relationid=atoi((const char*)sqlite3_column_text(stmt,0));
		sqlite3_finalize(stmt);
		return relationid; /* return relationid */
	}

	sqlite3_finalize(stmt);
	return KWEST_FAIL; /*No association between tags*/
}

/* get_association: Get tag having a particular association with another tag 
 * param: char *t - tagname
 * param: int relationid - relation between tags
 * return: sqlite3_stmt pointer
 * author: @SG 
 */
sqlite3_stmt *get_association(const char *t,int relationid)
{
	sqlite3_stmt* stmt; 
	char query[QUERY_SIZE];
	int status;
	int t_id;

	if(isrelation(relationid)==0){ /* Return if relation Undefined */
		printf("Unknown Relation Type : %d\n",relationid);
		return NULL;
	}

	t_id=get_tag_id(t); /* Get Tag ID */
	if(t_id==KWEST_TNF){ /* Return if Tag not found */
		printf("Tag Not Found : %s\n",t);
		return NULL;
	}

	/* Query to get tags associated with tag t */
	sprintf(query,
	       "select tagname from TagDetails where tno in"
	       "(select t1 from TagAssociation where t2=%d and relationid=%d);",
	       t_id,relationid);
	status = sqlite3_prepare_v2(db,query,-1,&stmt,0);

	if(status != SQLITE_OK){ /* Error Preparing query */
		return NULL;
	}

	return stmt;
}

/* ----------------- Associations --------------------- */

/* add_relation: Create a new association type 
 * param: char *relationtype
 * return: int relationid
 * author: @SG 
 */
int add_relation(const char *relationtype)
{
	sqlite3_stmt *stmt;
	char query[QUERY_SIZE];
	int status;
	int relationid=0;

	/* Query : check if entry exists in Associations Table */  
	sprintf(query,
	   "select count(relationid) from Associations where relationtype='%s';"
	   ,relationtype);
	status = sqlite3_prepare_v2(db,query,-1,&stmt,0); 

	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW) {
		if(atoi((const char*)sqlite3_column_text(stmt,0))!=0) {
			sqlite3_finalize(stmt);
			return KWEST_IE; /* Relation Exists */
		}
	}
	sqlite3_finalize(stmt);
	stmt=NULL;

	/* Query to get maximum relation id existing in database */
	strcpy(query,"select max(relationid) from Associations;");
	status = sqlite3_prepare_v2(db,query,-1,&stmt,0); 
	
	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW) {
		if((const char*)sqlite3_column_text(stmt,0)==NULL){ 
			relationid = 0; /* First Entry */
		} else {
			relationid = 
			    atoi((const char*)sqlite3_column_text(stmt,0))+1;
		}
		sqlite3_finalize(stmt); 

		/* Query : add (relationid,relationtype) to Association Table */
		sprintf(query,"INSERT INTO Associations VALUES(%d,'%s');",
		        relationid,relationtype);
		status = sqlite3_exec(db,query,0,0,0);
		return status;
	}

	sqlite3_finalize(stmt); 
	return KWEST_FAIL;
}

/* remove_relation: Remove an existing association type
 * param: int relationid
 * return: 1 on SUCCESS
 * author:
 */
int remove_relation(int relationid)
{
	/* OPERATION
	 * Query : Check if any entries with relationid in TagAssociation exist
	 * deal with existing entries if any
	 * Query : remove entry for relationid in Associations Table
	 */
	return KWEST_FAIL;
}

/* get_relation_id: Returns relationid for given relationtype 
 * param: char *relationtype
 * return: int relationid
 * author: @SG
 */
int get_relation_id(const char *relationtype)
{
	sqlite3_stmt *stmt;
	char query[QUERY_SIZE];
	int status;
	int relationid;

	/* Query : get relationid from Associations Table */
	sprintf(query,
	        "select relationid from Associations where relationtype='%s';",
	        relationtype);
	status = sqlite3_prepare_v2(db,query,-1,&stmt,0); 

	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW) {
		relationid=atoi((const char*)sqlite3_column_text(stmt,0));
		sqlite3_finalize(stmt);
		return relationid;
	}

	sqlite3_finalize(stmt);
	return KWEST_FAIL;
}

/* get_relation_type: Returns relationtype for given relationid
 * param: int relationid
 * return: char *relationtype
 * author:
 */
const char *get_relation_type(int relationid)
{
	/* OPERATION
	 * Query : return relationtype from AssociationRelation Table
	 */
	return NULL;
}

/* isrelation: Check if relation exists
 * param: int relationid
 * return: 1 if relation present
 * author: @SG
 */
int isrelation(int relationid)
{
	sqlite3_stmt *stmt;
	char query[QUERY_SIZE];
	int status;

	sprintf(query,
	       "select count(relationid) from Association where relationid=%d;",
	       relationid);
	status = sqlite3_prepare_v2(db,query,-1,&stmt,0); 

	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW) {
		relationid=atoi((const char*)sqlite3_column_text(stmt,0));
		sqlite3_finalize(stmt);
		return relationid;
	}

	sqlite3_finalize(stmt);
	return KWEST_FAIL;

}
/* --------------------- Others --------------------- */

/* list_tags: List all tags in the system 
 * param: void
 * return: sqlite3_stmt *
 * author: @SG
 */
sqlite3_stmt *list_tags(void)
{
	sqlite3_stmt * stmt;
	char query[QUERY_SIZE];
	int status;

	/* Query to get user tags from Database */
	strcpy(query,"select tagname from TagDetails where tno>99;");
	status = sqlite3_prepare_v2(db,query,-1,&stmt,0);

	if(status != SQLITE_OK){ /* Error Preparing query */
		printf("Error : list_tags\n");
		return NULL;
	}

	return stmt;
}

/* system_associations: Form association for metadata in file
 * param: filetype - Type of File
 * param: tag - Metadata Category
 * return: 0 on SUCCESS
 * author: @SG
 */
int system_associations(const char *filetype,const char *tag)
{
	sqlite3_stmt* stmt;
	char query[QUERY_SIZE];
	int status;
	const char *tmp; /* Hold result of query */

	/* Query : get all metadata under metadata-category */
	sprintf(query,"select %s from %s;",tag,filetype);
	sqlite3_prepare_v2(db,query,-1,&stmt,0);

	while(1){
		status = sqlite3_step(stmt);
		if(status == SQLITE_ROW) {
			tmp = (const char*)sqlite3_column_text(stmt,0);

			if(strcmp(tmp,"")==0){ /* No meta information */
				/* Create Tag Unknown */
				add_tag("Unknown",SYSTEM_TAG); 
				/* Associate Tag Unknown with File Type*/
				add_association("Unknown",tag,RELATION_SYSTEM);
				/* Tag File to Unknown Tag */
				tag_meta_file(tag,tmp);

			} else { /* Metadata Exist */
				/* Create Tag for Metadata */
				add_tag(tmp,USER_TAG);
				/* Associate Metadata tag with File Type */
				add_association(tmp,tag,RELATION_SYSTEM);
				/* Tag File to Metadata Tag */
				tag_meta_file(tag,tmp);
			}
		} else if(status==SQLITE_DONE){
			sqlite3_finalize(stmt);
			return status;
		} else {
			sqlite3_finalize(stmt);
			return KWEST_FAIL;
		}
	}

	return KWEST_FAIL;
}

/* tag_meta_file: Form association between file and associated metadata
 * param: tag - Metadata Category
 * param: name - Metadata
 * return: 1 on SUCCESS
 * author: @SG
 */
int tag_meta_file(const char *tag,const char *name)
{
	sqlite3_stmt* stmt; 
	char query[QUERY_SIZE];
	int status;
	const char *tmp; /* hold absolute path */

	/* Query : get file to be tagged under Metadata Category */
	sprintf(query,"select fno from Audio where %s='%s';",tag,name);
	sqlite3_prepare_v2(db,query,-1,&stmt,0);
	
	while(1){ /* If multile files present under same Metadata Category*/
		status = sqlite3_step(stmt);

		if(status == SQLITE_ROW) {
			tmp = get_file_path( /* Get absolute path */
			    atoi((const char*)sqlite3_column_text(stmt,0)));
			if(strcmp(name,"")==0){ /* No Metadata exist */
				tag_file("Unknown",tmp); /* Tag under Unknown */
			} else {
				tag_file(name,tmp); /* Tag File to Metadata */
			}
			free((char *)tmp);
		} else if(status==SQLITE_DONE){
			sqlite3_finalize(stmt);
			return status;
		} else {
			sqlite3_finalize(stmt);
			return KWEST_FAIL;
		}
	}

	return KWEST_FAIL;
}

/* get_data: Returns data for multiple rows is query
 * param: sqlite3_stmt *stmt - statement holding query
 * return: const char* data returned by query
 * author: @SG 
 */
const char* get_data(sqlite3_stmt *stmt)
{
	int status;

	status = sqlite3_step(stmt); /* Execute Query */
	if(status == SQLITE_ROW){ /* Return data if present */
		return (const char*)sqlite3_column_text(stmt,0);
	} else { /* Return NULL to mark end of Data */
		sqlite3_finalize(stmt);
		return NULL;
	}
}

/* -------------------- Hashing --------------------- */

/* set_file_id: Generate id for new file to be added in kwest
 * param: char *abspath - Absolute Path of File
 * return: int fno
 * author: @SG 
 */
static int set_file_id(const char *abspath)
{
	sqlite3_stmt *stmt=NULL;
	char query[QUERY_SIZE];
	int status;
	int fno;

	/* Check if file exists */
	fno=get_file_id(abspath); /* Get File ID */
	if(fno != KWEST_FNF){ 
		return KWEST_FE; /* Return if file already exists */
	}

	/* Query to get maximum no of files existing in database */
	strcpy(query,"select max(fno) from FileDetails;");
	status = sqlite3_prepare_v2(db,query,-1,&stmt,0); 
	
	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW) {
		if((const char*)sqlite3_column_text(stmt,0)==NULL) { 
			sqlite3_finalize(stmt);
			return 1; /* NO ENTRY */
		} else { /* Increment file count */
			status=atoi((const char*)sqlite3_column_text(stmt,0))+1;
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
static int set_tag_id(const char *tagname,int tagtype)
{
	sqlite3_stmt *stmt=NULL;  
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
		/* Start adding user tag at tno=100 */
		strcpy(query,
		       "select count(tno) from TagDetails where tno>99;");
		status = sqlite3_prepare_v2(db,query,-1,&stmt,0);
		
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
		/* Start adding system tag at tno=0 */
		strcpy(query,
		       "select count(tno) from TagDetails where tno<100;");
		status = sqlite3_prepare_v2(db,query,-1,&stmt,0);
		
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
 * param: char *abspath - Absolute Path of File
 * return: int fno 
 * author: @SG 
 */
static int get_file_id(const char *abspath)
{
	sqlite3_stmt *stmt=NULL;
	char query[QUERY_SIZE];
	int status;

	/* Query to get fno */
	sprintf(query,"select fno from FileDetails where abspath='%s';",
	        abspath);
	status = sqlite3_prepare_v2(db,query,-1,&stmt,0);
	
	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW) { /* Return fno if file exists */
		status=atoi((const char*)sqlite3_column_text(stmt,0));
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
static int get_tag_id(const char *tagname)
{
	sqlite3_stmt *stmt=NULL;  
	char query[QUERY_SIZE];
	int status;

	/* Query to get tno */
	sprintf(query,"select tno from TagDetails where tagname='%s';",tagname);
	status = sqlite3_prepare_v2(db,query,-1,&stmt,0);
	
	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW) { /* Return tno if tag exists */
		status=atoi((const char*)sqlite3_column_text(stmt,0));
		sqlite3_finalize(stmt);
		return status; 
	}

	sqlite3_finalize(stmt);
	return KWEST_TNF; /* Tag not found Error*/
}

/* get_file_path: Retrieve filepath by its id
 * param: int fno - file number 
 * return: char *filename
 * author: @SG 
 */
static const char *get_file_path(int fno)
{
	sqlite3_stmt *stmt;
	char query[QUERY_SIZE];
	int status;
	const char *filename=NULL;

	/* Query to get absolute path from fno */
	sprintf(query,"select abspath from FileDetails where fno=%d;",fno);
	status = sqlite3_prepare_v2(db,query,-1,&stmt,0);

	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW){ /* return abspath if file exists */
		filename=strdup((const char*)sqlite3_column_text(stmt,0));
	}

	sqlite3_finalize(stmt);
	return filename; /* Return Null */
}

/* get_tag_name: Retrieve tag name by its id
 * param: int tno - tag number
 * return: char *tagname
 * author: @SG 
 */
static const char *get_tag_name(int tno)
{
	sqlite3_stmt *stmt;
	char query[QUERY_SIZE];
	int status;
	const char *tagname=NULL;

	/* Query to get tagname */
	sprintf(query,"select tagname from TagDetails where tno=%d",tno);
	status = sqlite3_prepare_v2(db,query,-1,&stmt,0);
	
	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW){ /* return tagname if tag exists */
		tagname=strdup((const char*)sqlite3_column_text(stmt,0));
	}

	sqlite3_finalize(stmt);
	return tagname; /* Return Null */
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
	sprintf(query,"select count(*) from TagDetails where tagname='%s';",t);
	status = sqlite3_prepare_v2(db,query,-1,&stmt,0);
	
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
	sprintf(query,"select count(*) from FileDetails where f_name='%s';",f);
	status = sqlite3_prepare_v2(db,query,-1,&stmt,0);

	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW) {
		tmp=atoi((const char*)sqlite3_column_text(stmt,0));
		sqlite3_finalize(stmt);
		return tmp;
	}

	sqlite3_finalize(stmt);
	return KWEST_FAIL;
}

/* get_abspath: return absolute path of file
 * param: char *path - kwest path
 * return: char * absolute path
 * author: @SG 
 */
char *get_abspath(const char *path)
{
	sqlite3_stmt *stmt;
	char query[QUERY_SIZE];
	int status;
	const char *tmp; /* To hold filename */
	char *abspath;

	/*TODO: Ckeck if path is valid*/

	/* Get filename from path */
	if((tmp=strrchr(path,'/'))==NULL){
		tmp=path;
	} else {
		tmp=tmp+1;
	}

	/* Query to get absolute path from file name */
	sprintf(query,"select abspath from FileDetails where f_name='%s';",tmp);
	status = sqlite3_prepare_v2(db,query,-1,&stmt,0);

	status = sqlite3_step(stmt);
	if(status == SQLITE_ROW) {
		abspath=strdup((const char*)sqlite3_column_text(stmt,0));
		sqlite3_finalize(stmt);
		return abspath;
	}

	sqlite3_finalize(stmt);
	return NULL;
}
