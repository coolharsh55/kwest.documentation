/* DATABASE FUNCTIONS */

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

#ifndef DBFUNC_H_INCLUDED
#define DBFUNC_H_INCLUDED

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>    /* SQLITE Functions */
#include <pwd.h>        /* Function : getpwuid */
#include <unistd.h>     /* Function : getuid */

#include "extract_audio_taglib.h"

/* Size of array holding query */
#define QUERY_SIZE 400

/* Type of tags */
#define USER_TAG   1      /* Tag Accessible to user */
#define SYSTEM_TAG 2

/* Starting id for tags */
#define USER_TAG_START   100
#define SYSTEM_TAG_START 1

/* Error Codes */
#define KWEST_SUCCESS  0  /* Operation Successfully Completed */
#define KWEST_FAIL    -1  /* Operation Failed */
#define KWEST_TE      -2  /* Tag Exists */
#define KWEST_TNF     -3  /* Tag Not Found */
#define KWEST_FE      -4  /* File Exists */
#define KWEST_FNF     -5  /* File Not Found */
#define KWEST_RNF     -6  /* Relation Not Found */
#define KWEST_IE      -7  /* Information Exists */

/* Tag-Tag Relations */
#define RELATION_SYSTEM           0
#define RELATION_PROBABLY_RELATED 1
#define RELATION_SUBGROUP         2
#define RELATION_RELATED          3
#define RELATION_NOT_RELATED      4

/* ------------ Global Variable ------------------- */
static sqlite3 *db; /* SQLite database handle */

/* ------------ Database Connections -------------- */

/* create_db: Create Kwest database for first use
 * return: 0 on SUCCESS
 * author: @SG
 */
int create_db(void);

/* init_db: Establish Kwest Database Connection
 * return: 0 on SUCCESS
 * author: @SG
 */
int init_db(void);

/* close_db: Close Kwest Database Connection
 * return: 0 on SUCCESS
 * author: @SG
 */
int close_db(void);

/* ---------------- ADD/REMOVE -------------------- */

/* add_tag: Create a new tag in kwest
 * param: char *tagname
 * return: 0 on SUCCESS
 * author: @SG
 */
int add_tag(const char *tagname,int tagtype);

/* remove_tag: Remove an existing tag from kwest 
 * param: char *tagname
 * return: 0 on SUCCESS
 * author: @SG
 */
int remove_tag(const char *tagname);

/* add_file: Add file to kwest
 * param: char *abspath
 * return: 0 on SUCCESS
 * author: @SG 
 */
int add_file(const char *abspath);

/* remove_file: Remove file form kwest 
 * param: char *path - Path in Kwest
 * return: 0 on SUCCESS
 * author: @SG 
 */
int remove_file(const char *path);

/* add_meta_info: Add new category to identify metadata 
 * param: filetype - Type of File
 * param: tag - Category of Metadata
 * return: 0 on SUCCESS
 * author: @SG  
 */
int add_meta_info(const char *filetype,const char *tag);

/* remove_meta_info: remove category identifying metadata 
 * param: filetype - Type of File
 * param: tag - Category of Metadata
 * return: 0 on SUCCESS
 * author: 
 */
int remove_meta_info(const char *filetype,const char *tag);

/* --------------- Tag-File Relation ------------- */

/* tag_file: Associate a tag with a file 
 * param: char *t - tagname
 * param: char *f - filename
 * return: 0 on SUCCESS
 * author: @SG
 */
int tag_file(const char *t,const char *f);

/* untag_file: Remove the existing association between the tag and file
 * param: char *t - tagname
 * param: char *f - filename
 * return: 0 on SUCCESS
 * author: @SG
 */
int untag_file(const char *t,const char *f);

/* get_file: Return list of files associated to given tag 
 * param: char *t - tagname
 * return: sqlite3_stmt *
 * author: @SG
 */
sqlite3_stmt *get_file(const char *t);

/* get_file_abspath: Return list of files associated to given tag 
 * param: char *t - tagname
 * return: sqlite3_stmt pointer
 * author: @SG
 */
sqlite3_stmt *get_file_abspath(const char *t);

/* get_tag: Return list of tags associated with a given file
 * param: char *f - filename
 * return: sqlite3_stmt *
 * author: @SG
 */
sqlite3_stmt *get_tag(const char *f);


/*----------------- Tag-Tag Relation ------------------*/

/* add_association: Associate a tag with another tag 
 * param: char *t1,*t2 - tagname of both tags to be associated
 * param: int relationid - relation between tags to be formed
 * return: 0 on SUCCESS
 * author: @SG
 */
int add_association(const char *t1,const char *t2,int relationid);

/* remove_association: Remove the existing association between tags 
 * param: char *t1,*t2 - tagnames whose association is to be removed
 * param: int relationid - relation between tags to be removed
 * return: 0 on SUCCESS
 * author: @SG
 */
int remove_association(const char *t1,const char *t2,int relationid);

/* get_association_relation: Return type of association between tags
 * param: char *t1,*t2 - tagname of both tags in association
 * return: int relationid
 * author: @SG
 */
int get_association_relation(const char *t1,const char *t2);

/* get_association: Get tag having particular association with other tag 
 * param: char *t - tagname
 * param: int relationid - relation between tags
 * return: sqlite3_stmt *
 * author: @SG 
 */
sqlite3_stmt* get_association(const char *t,int relationid);

/* ----------------- Associations --------------------- */

/* add_relation: Create a new association type 
 * param: char *relationtype
 * return: int relationid
 * author: @SG 
 */
int add_relation(const char *relationtype);

/* remove_relation: Remove an existing association type
 * param: int relationid
 * return: 0 on SUCCESS
 * author:
 */
int remove_relation(int relationid);

/* get_relation_id: Returns relationid for given relationtype 
 * param: char *relationtype
 * return: int relationid
 * author: @SG
 */
int get_relation_id(const char *relationtype);

/* get_relation_type: Returns relationtype for given relationid
 * param: int relationid
 * return: char *relationtype
 * author:
 */
const char *get_relation_type(int relationid);

/* isrelation: Check if relation exists
 * param: int relationid
 * return: 1 if relation present
 * author: @SG
 */
int isrelation(int relationid);

/* --------------------- Others --------------------- */

/* list_tags: List all tags in the system 
 * param: void
 * return: sqlite3_stmt *
 * author: @SG
 */
sqlite3_stmt *list_tags(void);

/* system_associations: Form association for metadata in file
 * param: filetype - Type of File
 * param: tag - Metadata Category
 * return: 0 on SUCCESS
 * author: @SG
 */
int system_associations(const char *filetype,const char *tag);

/* tag_meta_file: Form association between file and associated metadata
 * param: tag - Metadata Category
 * param: name - Metadata
 * return: 0 on SUCCESS
 * author: @SG
 */
int tag_meta_file(const char *tag,const char *name);

/* get_data: Returns data for multiple rows is query
 * param: sqlite3_stmt *stmt - statement holding query
 * return: const char* data returned by query
 * author: @SG 
 */
const char* get_data(sqlite3_stmt *stmt);

/* -------------------- Hashing --------------------- */

/* set_file_id: Generate id for new file to be added in kwest
 * param: char *abspath - Absolute Path of File
 * return: int fno
 * author: @SG 
 */
static int set_file_id(const char *abspath);

/* set_tag_id: Generate id for new tag to be created in kwest
 * param: char *tagname
 * return: int tno 
 * author: @SG 
 */
static int set_tag_id(const char *tagname,int tagtype);

/* get_file_id: return id for file in kwest
 * param: char *abspath - Absolute Path of File
 * return: int fno 
 * author: @SG 
 */
static int get_file_id(const char *abspath);

/* get_tag_id: return id for a tag in kwest
 * param: char *tagname
 * return: int tno 
 * author: @SG 
 */
static int get_tag_id(const char *tagname);

/* get_file_path: Retrieve filepath by its id
 * param: int fno - file number
 * return: char *filename
 * author: @SG 
 */
static const char *get_file_path(int fno);

/* get_tag_name: Retrieve tag name by its id
 * param: int tno - tag number
 * return: char *tagname
 * author: @SG 
 */
static const char *get_tag_name(int tno);

/* -------------------- Fuse Functions --------------------- */

/* istag: Check if given tag is present in system
 * param: char *t - tagname
 * return: 1 if tag present
 * author: @SG 
 */
int istag(const char *t);

/* isfile: Check if given file is present in system
 * param: char *f - filename
 * return: 1 if file present
 * author: @SG 
 */
int isfile(const char *f);

/* get_abspath: return absolute path of file
 * param: char *path - kwest path
 * author: @SG 
 * return: char * absolute path
 */
char *get_abspath(const char *path);

#endif
