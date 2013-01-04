/* IMPORTING SEMANTICS */

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

#ifndef IMPORT_H_INCLUDED
#define IMPORT_H_INCLUDED

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h> /* stat structure */
#include <dirent.h>   /* DIR */
#include <sqlite3.h>  /* SQLITE Functions */

#include "dbfunc.h" /* Kwest Datadase Functions */
#include "extract_audio_taglib.h" /* Extract Audio Metadata */ 

#define IMP_SUCCESS 0
#define IMP_FAIL -1

/* Database Functions Required:
 * int add_tag(char *tagname,int tagtype)
 * int add_file(char *abspath)
 * int tag_file(char *t,char *f)
 * int add_association(char *t1,char *t2,int relationid)
 */

/* import: Import Directory-File structure from File System to Kwest
 * param: char *path - Absolute path of Diectory to be imported
 * return: 1 on SUCCESS
 * author: @SG
 */
int import(const char *path);

#endif
