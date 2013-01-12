/* logging.c
 * functions for logging filesystem operations
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
 
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "logging.h"
#include "flags.h"

/* log_init
 * initialize logging
 * 
 * @param none
 * @return KW_SUCCESS on success, KW_FAIL on fail
 * @author @HP
 */
int log_init(void)
{
	logfile = fopen("kwest.log","w");
	if(logfile == NULL) {
		return KW_FAIL;
	}
	return KW_SUCCESS;
}

/* log_msg
 * print log message to logfile
 * 
 * @param none
 * @return none
 * @author @HP 
 */
void log_msg(const char *msg, ...)
{
    va_list argptr;
    va_start(argptr, msg);
    vfprintf(logfile, msg, argptr);
    va_end(argptr);
}

/* log_close
 * close the logfile
 * 
 * @param none
 * @return none
 * @author @HP
 */
int log_close(void)
{
	fclose(logfile);
	return KW_SUCCESS;
}
