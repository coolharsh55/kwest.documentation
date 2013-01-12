/* extract_audio_taglib.c
 * extract audio metadata using taglib
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

#include "extract_audio_taglib.h"


/* extract_metadata_file: extract from physical location
 * param: const char* path - path of file
 * return: 0 SUCCESS, 1 NA, -1 ERROR
 * author: @HP
 * */
TagLib_File *extract_metadata_file(const char* path, struct METADATA_AUDIO *M)
{
	TagLib_File* file = taglib_file_new(path); 
	if(file == NULL) {
		/* log_error("_is_audio","ERROR opening file"); */
		return NULL;
	}
	if(taglib_file_is_valid(file) == 0) {
		extract_clear_strings(file);
		return NULL;
	}
		
	TagLib_Tag* tag = taglib_file_tag(file);
	/* struct METADATA_AUDIO to store audio keywords */
	M->title = taglib_tag_title(tag);
	M->artist = taglib_tag_artist(tag);
	M->album = taglib_tag_album(tag);
	M->genre = taglib_tag_genre(tag);
	
	/* extract_clear_strings(file); */
	return file;
}

/* _is_audio: check if filetype = audio
 * param: const char* path - path of file
 * return: 0 AUDIO, 1 NO, -1 ERROR
 * author: @HP
 * */
int is_audio(const char* path)
{
	TagLib_File* file = NULL;
	file = taglib_file_new(path);
	if(file == NULL) {
		/* log_error("_is_audio","ERROR opening file"); */
		return -1;
	}
	if(taglib_file_is_valid(file) != 0) {
		taglib_file_free(file);
		return 0;
	} else {
		taglib_file_free(file);
		return 1;
	}
}

void extract_clear_strings(TagLib_File* file)
{
	taglib_tag_free_strings();
	taglib_file_free(file);
	return;
}
