/*
 * ftest.c
 * run01: combine database into fuse example
 * run02: try generating directory listing based on dB    
 * run03: table2 - files and tags
 */
 
 /*
  * defines the standard fuse version
  * common examples include 
  * 22 ; 23 ; 24 ; 26
  * version installed on this machine 2.8.6
  */
 #define FUSE_USE_VERSION 28
 
 // common headers
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <dirent.h>
 #include <errno.h>
 
 //fuse header
 #include <fuse.h>
 
 //sqlite3 header
 #include <sqlite3.h>
 
 //database to be used -> "test.db" in this case
 static sqlite3 *db;
 //log file
 FILE *ft_log;
 
 /*
  * required:
  * common buffer for storing dB queries
  * 	OR
  * seperate file containing all the dB queries
  */
 
 
 /*
  * getattr
  * returns attributes of entries
  * in this case all are files
  */
 static int ft_getattr(const char *path, struct stat *stbuf)
{
	int res = 0; //result of operation	
	
	if(*(path+1) == '\0') { //path is root
	//set root as directory, return
		memset(stbuf, 0, sizeof(struct stat));
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
		return 0;
	}
	
	char filename[20]; //%%shift to common storage
	char *tmp = NULL;
	const char *p = path;
	tmp = strrchr(p,'/'); //get last name
	if(tmp == NULL) return 0; //error in syntax
	strcpy(filename, tmp+1); //copy filename
		
	sqlite3_stmt *stmt1 = NULL;
	int qres = 0;
	
	char query1[80]; //%%shift to common storage
	//%%query: can shift to seperate file
	//query to check if path==file
	strcpy(query1, "select filename from files where filename=\""); 
	strcat(query1,filename);
	tmp = strcat(query1, "\";");
	sqlite3_prepare(db, query1, -1, &stmt1, NULL);
	qres = sqlite3_step(stmt1);
	if(qres == SQLITE_ROW) { //success
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = strlen("Harsh");
	} else { //no rows returned
	//%%query: can shift to seperate file
	//query to check if path==directory
		strcpy(query1, "select tag from tags where tag=\""); 
		strcat(query1,filename);
		tmp = strcat(query1, "\";");
		sqlite3_prepare(db, query1, -1, &stmt1, NULL);
		qres = sqlite3_step(stmt1);
		if(qres == SQLITE_ROW) { //success
			stbuf->st_mode = S_IFDIR | 0755;
			stbuf->st_nlink = 1;
		} else { //error
			sqlite3_finalize(stmt1);
			return -ENOENT;
			//return error in path
		}
	}
	sqlite3_finalize(stmt1);	
	return res;
}

/*
 * readdir
 * should list the contents of the directory
 * right now we have only one path
 * so we display the dB contents
 */
 static int ft_readdir(const char *path, void *buf, 
				fuse_fill_dir_t filler, 
				off_t offset, struct fuse_file_info *fi)
{
	(void) offset;
	(void) fi;
	
	sqlite3_stmt *stmt1 = NULL;
	int qres = 0;
	
	//%%query: can shift to seperate file
	if (strcmp(path,"/")==0) { //path is ROOT
		char *query1 = "select filename from files where fno in (select fno from tags where tag=\"untagged\");";
		//query to get all files under ROOT
		char *query2 = "select tag from tags where tag!=\"untagged\" group by tag;";
		//query to get all tags
		
		sqlite3_prepare(db, query1, -1, &stmt1, NULL);
		qres = sqlite3_step(stmt1);
		
		const char *result = NULL;
				
		// list files
		while(qres == SQLITE_ROW) { //success
			struct stat st; //struct stat
			//fuse filler allows stat to be declared here itself
			//%%BUG: does not work
			memset(&st, 0, sizeof(st));
			st.st_mode = S_IFREG | 0444; //set as file
			result = sqlite3_column_text(stmt1,0); //get result from dB
			//%%warning: pointer targets in assignment differ in signedness [-Wpointer-sign]
			if (filler(buf, result, NULL, 0)) //fill to buffer
				break;
			qres = sqlite3_step(stmt1);
		}
		
		sqlite3_prepare(db, query2, -1, &stmt1, NULL);
		qres = sqlite3_step(stmt1);
		
		// list directories
		while(qres == SQLITE_ROW) { //success
			struct stat st; //struct stat
			//fuse filler allows stat to be declared here itself
			//%%BUG: does not work
			memset(&st, 0, sizeof(st));
			st.st_mode = S_IFDIR | 0755;
			result = sqlite3_column_text(stmt1,0);
			//%%warning: pointer targets in assignment differ in signedness [-Wpointer-sign]
			if (filler(buf, result, NULL, 0)) //fill to buffer
				break;
			qres = sqlite3_step(stmt1);
		}				
	} else { //path is not root
		fprintf(ft_log, "path=%s\n",path);
		
		/* old code to extract last tag
		char filename[20]; //%%shift to common area
		char *tmp = NULL;
		const char *p = path;
		tmp = strrchr(p,'/');
		if(tmp == NULL) return -ENOENT;
		strcpy(filename, tmp+1);
		*/
		
		//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
		
		char *t1 = path+1; //starting of tag
		char *t2 = strchr(t1,'/'); //next tag
		char tstore[25]; //%%shift to common area
		char query[200]; //%%shift to common area
		int i_ts = 0;
		if(t2 == NULL) {
			/*
			 * single tag
			 */
			i_ts = 0;
			while(*t1 != '\0') { //extract tag
				tstore[i_ts] = *t1;
				i_ts++; t1++;
			}
			tstore[i_ts] = '\0';
			fprintf(ft_log,"tag=%s\n",tstore);
			
			//%%query: can be shifted to seperate file
			//query to get all files under tag = TAG
			strcpy(query,"select filename from files as F,tags as T where F.fno=T.fno and tag=\"");
			strcat(query,tstore);
			strcat(query,"\";");
			fprintf(ft_log,"query=%s\n",query);
			sqlite3_prepare(db, query, -1, &stmt1, NULL);
			qres = sqlite3_step(stmt1);
			
			const char *result = NULL;
					
			//all files under tag=TAG
			while(qres == SQLITE_ROW) {
				struct stat st; //struct stat
				//BUG: filler does not populate from here
				memset(&st, 0, sizeof(st));
				st.st_mode = S_IFREG | 0444;
				result = sqlite3_column_text(stmt1,0);
				//%%warning: pointer targets in assignment differ in signedness [-Wpointer-sign]
				if (filler(buf, result, NULL, 0))
					break;
				qres = sqlite3_step(stmt1);
			}
			
			//%%query: can be shifted to seperate file
			//query to get all tags associated with tag=TAG wherein they have atleast one file common
			strcpy(query,"select distinct tag from tags where fno in (select fno from tags where tag=\"");
			strcat(query,tstore);
			strcat(query,"\") and tag!=\"");
			strcat(query,tstore);
			strcat(query,"\";");
			
			sqlite3_prepare(db, query, -1, &stmt1, NULL);
			qres = sqlite3_step(stmt1);
					
			//tags as directories
			while(qres == SQLITE_ROW) {
				struct stat st; //struct stat
				//BUG: filler wont populate with stat
				memset(&st, 0, sizeof(st));
				st.st_mode = S_IFDIR | 0755;
				result = sqlite3_column_text(stmt1,0);
				//%%warning: pointer targets in assignment differ in signedness [-Wpointer-sign]
				if (filler(buf, result, NULL, 0))
					break;
				qres = sqlite3_step(stmt1);
			}
			
			
		} else {
			/*
			 * multiple tags
			 * select filename from files where 
			 * fno=(select fno from tags where 
			 * fno=(select fno from tags where tag="doc") 
			 * and tag="proj");
			 */
			strcpy(query,"select filename from files where fno in (select fno from tags where fno in (select fno from tags where tag=\"");
			while(t2 != NULL) {
				
				i_ts = 0;
				while(t1 != t2) { //extract tag
					tstore[i_ts] = *t1;
					i_ts++; t1++;
				}
				tstore[i_ts] = '\0';
				fprintf(ft_log,"tag=%s\n",tstore);
				strcat(query,tstore);
				strcat(query,"\")"); //append tag to query
				t1 = t2;
				t2 = strchr(t1 + 1, '/');
			}
			if (*t1 != '\0') { //last tag
				t1++;
				t2 = strchr(path,'\0');
				i_ts = 0;
				while(t1 != t2) { //extract tag
					tstore[i_ts] = *t1;
					i_ts++; t1++;
				}
				tstore[i_ts] = '\0';
				strcat(query," and tag=\"");
				strcat(query,tstore);
				strcat(query,"\")"); //append tag to query
				fprintf(ft_log,"tag=%s\n",tstore);
			}
			
			
			//%%query: can be shifted to seperate file
			//query to get all files under tag = TAG
			
			strcat(query,";");
			fprintf(ft_log,"query=%s\n",query);
			sqlite3_prepare(db, query, -1, &stmt1, NULL);
			qres = sqlite3_step(stmt1);
			
			const char *result = NULL;
					
			//all files under tag=TAG
			while(qres == SQLITE_ROW) {
				struct stat st; //struct stat
				//BUG: filler does not populate from here
				memset(&st, 0, sizeof(st));
				st.st_mode = S_IFREG | 0444;
				result = sqlite3_column_text(stmt1,0);
				//%%warning: pointer targets in assignment differ in signedness [-Wpointer-sign]
				if (filler(buf, result, NULL, 0))
					break;
				qres = sqlite3_step(stmt1);
			}
			
			
			//%%query: can be shifted to seperate file
			//query to get all tags associated with tag=TAG wherein they have atleast one file common
			strcpy(query,"select filename from files where fno=(select fno from tags where tag=\"");
			strcat(query,tstore);
			strcat(query,"\") and tag!=\"");
			strcat(query,tstore);
			strcat(query,"\";");
			
			sqlite3_prepare(db, query, -1, &stmt1, NULL);
			qres = sqlite3_step(stmt1);
					
			//tags as directories
			while(qres == SQLITE_ROW) {
				struct stat st; //struct stat
				//BUG: filler wont populate with stat
				memset(&st, 0, sizeof(st));
				st.st_mode = S_IFDIR | 0755;
				result = sqlite3_column_text(stmt1,0);
				//%%warning: pointer targets in assignment differ in signedness [-Wpointer-sign]
				if (filler(buf, result, NULL, 0))
					break;
				qres = sqlite3_step(stmt1);
			}			
		}
		
		
		
		//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
		
		
	}
	
	sqlite3_finalize(stmt1);
	//standard fillers
	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
		
	return 0;
}

/*
 * init
 * called while initialising the filesystem
 */
static void* ft_init(struct fuse_conn_info *conn)
{	
	return conn;
}

/*
 * destroy
 * called while exiting/unmounting the filesystem
 */
static void ft_destroy(void *private_data)
{	
	sqlite3_close(db);
	printf("dB close...\n");
	fprintf(ft_log, "exitXXX");
}

/*
 * fuse operations structure
 * total operations for this program = 4
 */
static struct fuse_operations ft_oper = {
	.init		=	ft_init,
	.destroy	=	ft_destroy,
	.getattr	=	ft_getattr,	
	.readdir	=	ft_readdir,
};


/*
 * main program
 * open database
 * return (Call) to fuse_main
 */
int main(int argc, char *argv[])
{
	/*
	 * LOG FILE OPERATIONS
	 */ ft_log = fopen("log.txt","w+");
	  fprintf(ft_log,"program initiated\n");
	  fprintf(ft_log,"sqlite version %d\n",SQLITE_VERSION_NUMBER);
	 /**/
	//open database for operation
	sqlite3_open("./test.db",&db);
	
	if(db == NULL) {
		//fprintf(ft_log,"dB opening failed\n");
		return -1;
	}
		//fprintf(ft_log,"dB opening success\n");	
	return fuse_main(argc, argv, &ft_oper,NULL);
}


