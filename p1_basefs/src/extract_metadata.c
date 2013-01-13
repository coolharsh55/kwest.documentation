/* audio.c
 * metadata and relations associated with audio metadata
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

#include "extract_metadata.h"
#include "dbbasic.h"
#include "logging.h"
#include "flags.h"

int audio_metadata()
{
	/* Create tags to browse Audio Files */
	add_tag("audio",SYSTEM_TAG);
	add_tag("artist",SYSTEM_TAG);
	add_tag("album",SYSTEM_TAG);
	add_tag("genre",SYSTEM_TAG);
	
	/* Metadata available for Audio Files */
	add_meta_info("audio","title");
	add_meta_info("audio","artist");
	add_meta_info("audio","album");
	add_meta_info("audio","genre");
	
	/* Establish tag-tag associations */
	add_association("audio","root",ASSOC_SUBGROUP);
	add_association("artist","audio",ASSOC_SUBGROUP);
	add_association("album","audio",ASSOC_SUBGROUP);
	add_association("genre","audio",ASSOC_SUBGROUP);
	
	return KW_SUCCESS;
}

int audio_metadata_associations()
{
	associate_file_metadata("audio","artist");
	associate_file_metadata("audio","album");
	associate_file_metadata("audio","genre");
	
	return KW_SUCCESS;
}
