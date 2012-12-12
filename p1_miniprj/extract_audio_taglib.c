/* EXTRACT AUDIO METADATA USING TAGLIB */
/* gcc -Wall -c extract_audio_taglib.c -o extract_audio_taglib.o -ltag_c */

/* LICENSE
** Copyright [2012] [Harshvardhan Pandit]
** Licensed under the Apache License, Version 2.0 (the "License"); You may not use this file except in compliance with the License. You may obtain a copy of the License at
       http://www.apache.org/licenses/LICENSE-2.0
   Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.   
*/

#include "extract_audio_taglib.h"


/* extract_metadata_file: extract from physical location
 * param:	const char* path	-	path of file
 * return:	0 SUCCESS, 1 NA, -1 ERROR
 * author: @HP
 * */
int extract_metadata_file(const char* path)
{
	TagLib_File* file = taglib_file_new(path); 
	if(file == NULL) {
		/* log_error("_is_audio","ERROR opening file"); */
		return -1;
	}
	if(taglib_file_is_valid(file) == 0) {
		_extract_clear_strings(file);
		return 1;
	}
		
	TagLib_Tag* tag = taglib_file_tag(file);
	/* struct METADATA_AUDIO to store audio keywords */
	/* METADATA_AUDIO.title = */ taglib_tag_title(tag);
	/* METADATA_AUDIO.artist = */ taglib_tag_artist(tag);
	/* METADATA_AUDIO.album = */ taglib_tag_album(tag);
	/* METADATA_AUDIO.genre = */ taglib_tag_genre(tag);
	
	_extract_clear_strings(file);	
	return 0;
}


/* extract_metadata_fileid: extract from fileid
 * param:	int file_id			-	file_id from database
 * return:	0 SUCCESS, 1 NA, -1 ERROR
 * author: @HP
 * */
int extract_metadata_fileid(int file_id)
{
	TagLib_File* file = NULL ; 
	/* file = taglib_file_new(db_file_path_fileid(file_id)); */
	if(file == NULL) {
		/* log_error("_is_audio","ERROR opening file"); */
		return -1;
	}
	if(taglib_file_is_valid(file) == 0) {
		_extract_clear_strings(file);
		return 1;
	}
		
	TagLib_Tag* tag = taglib_file_tag(file);
	/* struct METADATA_AUDIO to store audio keywords */
	/* METADATA_AUDIO.title = */ taglib_tag_title(tag);
	/* METADATA_AUDIO.artist = */ taglib_tag_artist(tag);
	/* METADATA_AUDIO.album = */ taglib_tag_album(tag);
	/* METADATA_AUDIO.genre = */ taglib_tag_genre(tag);
	
	_extract_clear_strings(file);
	return 0;
}


/* _is_audio: check if filetype = audio
 * param:	const char* path	-	path of file
 * return:	0 AUDIO, 1 NO, -1 ERROR
 * author: @HP
 * */
inline int _is_audio(const char* path)
{
	TagLib_File* file;
	file = taglib_file_new(path);
	if(file == NULL) {
		/* log_error("_is_audio","ERROR opening file"); */
		return -1;
	}
	if(taglib_file_is_valid(file) != 0) {
		_extract_clear_strings(file);
		return 0;
	} else {
		_extract_clear_strings(file);
		return 1;
	}
}

inline void _extract_clear_strings(TagLib_File* file)
{
	taglib_tag_free_strings();
	taglib_file_free(file);
	return;
}

