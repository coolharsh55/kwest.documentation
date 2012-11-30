/* Importing Semantics */

/* 
   Executing:
	gcc import.c -o import
	./import .
	Pass path as argument
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>

/*     
int main(void)
{
 char *p=".";			//path to be added to Kwest - Current Directory
 char *dirname=strrchr(p,'/');	//Extract Directory name from path
 (void) dispdir(p,dirname+1); 	//success(1) or fail(0)
}
*/

//Command Line
int main(int argc, const char* argv[])
{
 if (argc < 2) return -1;
 char *dirname=strrchr(argv[1],'/');	//Extract Directory name from path
 (void) dispdir(argv[1],dirname+1);	//success(1) or fail(0)
}


int dispdir(char *path,char *dirname)
{
 DIR *directory;
 struct dirent *entry; 
 size_t path_len = strlen(path);
 
 directory = opendir (path);
 if (directory != NULL)
  {
   while (entry = readdir (directory))
    {
    struct stat fstat;
    char full_name[_POSIX_PATH_MAX + 1];
    size_t dir_len = strlen(entry->d_name);

    /*Calculate full name, check we are in file length limts*/
    if ((path_len + dir_len + 1) > _POSIX_PATH_MAX)
            continue;   
 
    strcpy(full_name, path);
    if (full_name[path_len - 1] != '/') strcat(full_name, "/");
    strcat(full_name, entry->d_name);
    
    /*Ignore special directories*/
    if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0))
            continue;

    /*Ignore files starting with .*/
    if((strchr(entry->d_name,'.')-entry->d_name)==0)
	    continue;
    
    /*Ignore files ending with ~*/
    if((strrchr(entry->d_name,'~')-entry->d_name)==dir_len-1)
	    continue;
    
    /*Get File Infromation : Returns 0 if successful*/
    if (stat(full_name, &fstat) < 0)
            continue;
    
    if (S_ISDIR(fstat.st_mode))		//Directory
	{
	 printf("Dir : %s\n",entry->d_name);
	 
	 //Hash Function : t_sub=entry->d_name
	 /*
	    QUERY : 
		Check if t_sub already exists in database [TAG]
		If not add t_sub as tag [path is in full_name]
	 */

	 (void) dispdir(full_name,entry->d_name); //Access Sub-Directories

	 //Tag-Tag Relation
	 //Hash Function : t_par=dirname
	 /*
	   QUERY : 
		Check if (t_sub,t_par,association) is already defined in database 
		If not add (t_sub,t_par,association) to database
	 */
	}
    else if(S_ISREG(fstat.st_mode))	//Regular File  
	{
	 printf("File: %s\n",entry->d_name);
	 
	 //Hash Function : f=entry->d_name
	 /*
	    QUERY : 
		Check if f already exists in database [FILE]		
		IF not 
			add f as File [path is in full_name]
			Extract Metadata
			[IF REQUIRED]Get inode information from fstat structure (fstat.st_ino)
	 */

	 //Tag-File Relation
	 //Hash Function : d=dirname
	 /*
	    QUERY : 
		Check if (f,d) is already defined in database
		If not add (f,d) to database
	 */ 
        }
   }
   (void) closedir (directory);
   return 1;
  }
 else
  perror ("Couldn't open the directory");     
 return 0;
}
