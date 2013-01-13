/* magicstrings.h
 * magic strings (!!!)
 */

/* LICENSE
 * Copyright 2013 Harshvardhan Pandit
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

#ifndef KWEST_MAGICSTRINGS_H
#define KWEST_MAGICSTRINGS_H




#define TE_MSG "Tag Exists : "
#define TNF_MSG "Tag Not Found : "
#define FE_MSG "File Exists : "
#define FNF_MSG "File Not Found : "
#define RNF_MSG "Relation Not Defined : "
#define IE_MSG "Information Exists : "
#define ADDF_MSG "Error adding file : "
#define ADDM_MSG "Error adding Metadata for file : "
#define ADDT_MSG "Error adding tag : "
#define REMF_MSG "Error removing file : "
#define REMT_MSG "Error removing tag : "
#define PREP_MSG "Error Preparing query"

#define DBCFAIL_MSG "Database Connection Failed"
#define DBCLOSE_MSG "Error Closing Database"

#define DATABASE_STORAGE "/.config/kwest"
#define DATABASE_NAME "/kwest.db"

#define ASSOCN_SYSTEM_STR "system"
#define ASSOCN_PR_STR "probably_related"
#define ASSOCN_SUBG_STR "subgroup"
#define ASSOCN_REL_STR "related"
#define ASSOCN_NOTREL_STR "not_related"

#define TAG_ROOT "root"
#define TAG_FILES "files"

#define TAG_AUDIO "audio"
#define TAG_TITLE "title"
#define TAG_ALBUM "album"
#define TAG_ARTIST "artist"
#define TAG_GENRE "genre"

#define TAG_UNKNOWN "unknown"

#endif
