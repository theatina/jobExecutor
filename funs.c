////////////////////////////////
//	
//	Christina - Theano Kylafi
//
///////////////////////////////


#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <dirent.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h> 
#include <errno.h>
#include <sys/wait.h>  
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

#include "structs.h"
#include "funs.h"
#include "trie.h"

#define MSGSIZE 2048

struct Trie_t* trie;

int countdocs(char *path) //count docs for given path
{
	int i= 0;
	DIR *dirname ;
	struct dirent *ent;
	if ((dirname = opendir (path)) != NULL) 
	{	
		while ((ent = readdir (dirname)) != NULL)
		{

			if ( (ent -> d_type) != DT_DIR )  //an den einai directory einai arxeio metraw 
			{
				i++;
			}			
		}
	}
	closedir(dirname);
	return i;
}


int mapping(char *filename, char *map[], int countarxeia) //pairnei path kai epistrefei ti thesi tou ston map
{
	int thesi, i = 0;

	while ((strcmp(filename, map[i])) && (i < countarxeia))
	{

		i++;
	}

	thesi = i;
	
	return thesi;

}


int create_docinfo(char *pathmap[], char  *docmap[], struct domimap *docinfo[], int pathnum, int countarxeia) //pairnei ena ena paths kai eisagei sto map
{

	DIR *dir;
	struct dirent *doc;
	FILE *tempfp;
	int i = 0;
	for (int h = 0; h < pathnum; h++) //for all paths, i save all the docs info
	{
		int pathlen = strlen(pathmap[h]);
		char *tempdirpath = (char* ) malloc ((pathlen+1)*sizeof(char));
		strcpy(tempdirpath, pathmap[h]);
		char *ftempdirpath = tempdirpath; //free later

		

		if ((dir = opendir (tempdirpath)) != NULL) 
		{
			if (dir)  //an uparxei to directory - path
			{
			    //save all the info of every doc sto docinfo pinaka
		  		while ((doc = readdir (dir)) != NULL) 
		  		{
		  			if (( (doc -> d_type) != DT_DIR ) )
		  			{
			 
			  			int docnamelen = strlen(doc->d_name);

						char *docname = (char*) malloc ((docnamelen+pathlen+2)*sizeof(char));
						strcpy(docname, tempdirpath);

						strcat(docname, "/");
						strcat(docname, doc->d_name);
						int sizedocmap = (strlen(docname))+1;
			    		docmap[i]= (char*) malloc ((sizedocmap)*sizeof(char)); //dimiourgw ton docmap - pinaka files
				  		strcpy(docmap[i], docname); //antigrafi tou document name ston pinaka 
			  			
			  			//anoigma arxeiou kai save info - create trie
				  		docinfo[i]->id = i;

		  				i++;	 //epomeno arxeio 

		  				free(docname);
		  			}
		  		}

	  			closedir (dir);
			    
			}
	  		
		}
		else 
		{

	  		return -13;
		}


		free(ftempdirpath);

	}

	create_trie(docmap, docinfo, countarxeia);

	
	return 8;

}

