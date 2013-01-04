/* EXTRACTION AUDIO METADATA USING TAGLIB */

/* LICENSE
   Copyright [2013] [Harshvardhan Pandit]
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

#ifndef EXTRACT_AUDIO_TAGLIB_H_INCLUDED
#define EXTRACT_AUDIO_TAGLIB_H_INCLUDED

#include <taglib/tag_c.h>
#include <stdio.h>

struct METADATA_AUDIO {
	const char *title;
	const char *artist;
	const char *album;
	const char *genre;
};


/* extract_metadata_file: extract from physical location
 * param: const char* path - path of file
 * return: 0 SUCCESS, 1 NA, -1 ERROR
 * author: @HP
 * */
TagLib_File *extract_metadata_file(const char* path, struct METADATA_AUDIO *M);


/* extract_metadata_fileid: extract from fileid
 * param: int file_id - file_id from database
 * return: 0 SUCCESS, 1 NA, -1 ERROR
 * author: @HP
 * */
int extract_metadata_fileid(int file_id);

/* _is_audio: check if filetype = audio
 * param: const char* path - path of file
 * return: 0 AUDIO, 1 NO, -1 ERROR
 * author: @HP
 * */
inline int _is_audio(const char* path);

inline void _extract_clear_strings(TagLib_File* file);

#endif
