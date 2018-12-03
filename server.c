////////////////////////////////
//	
//	Christina - Theano Kylafi
//
///////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <sys/wait.h>  
#include <errno.h>
#include <dirent.h>
#include <ctype.h>
#include <time.h>

#include "structs.h"
#include "funs.h"
#include "trie.h"

#define MSGSIZE 2048

struct Trie_t* trie;

int main(int argc, char **argv)
{

	FILE* fp;
	int workers;


	//elegxoi arguments
	if (argc!=5)
	{
		printf("\nToo Few/Many Arguments!\n");
  		fprintf(stderr,"Usage: %s -d <docfile> -w <# of workers>\n", argv[0]);
    	return -51;
	}
	else
	{
  		if(!(strcmp(argv[1], "-d")))
  		{
			if ((fp = fopen(argv[2], "r")) == NULL)
  			{  	perror("\nfopen source-file"); printf("\n"); return 1; }

  			if(!(strcmp(argv[3], "-w")))
  			{	  			
	  			char* temp = argv[4] ;
	  			char xarakt = *temp;
	  			int isdig = 1;
	  			while((xarakt != '\0') && isdig)
	  			{
	  				if (isdigit(xarakt)==0)
	  					isdig = 0;
	  				temp++;
	  				xarakt = *temp;
	  			}

	  			if (!isdig)
	  			{
	  				printf("\n<# of workers> should not contain characters\n");
	  				fprintf(stderr,"\nUsage: %s -d <docfile> -w <# of workers>\n", argv[0]);
	  				return -7;
	  			}
	  			else
	  			{ 	workers = atoi(argv[4]); }	
	  		}
	  		else 
	  		{ 	fprintf(stderr,"\nUsage: %s -d <docfile> -w <# of workers>\n", argv[0]); return -51; }

  		}
  		else if(!(strcmp(argv[3], "-d")))
  		{
  			if ((fp = fopen(argv[4], "r")) == NULL)
  			{  	perror("\nfopen source-file"); printf("\n"); return 1; }

  			if(!(strcmp(argv[1], "-w")))
  			{
	  			char* temp = argv[2] ;
	  			char xarakt = *temp;
	  			int isdig = 1;
	  			while((xarakt != '\0') && isdig)
	  			{
	  				if (isdigit(xarakt)==0)
	  					isdig = 0;
	  				temp++;
	  				xarakt = *temp;
	  			}

	  			if (!isdig)
	  			{
	  				printf("\n<# of workers> should not contain characters\n");
	  				fprintf(stderr,"\nUsage: %s -d <docfile> -w <# of workers>\n", argv[0]);
	  				return -7;
	  			}
	  			else
	  			{ 	workers = atoi(argv[2]); }	
	  		}
	  		else 
	  		{ 	fprintf(stderr,"\nUsage: %s -d <docfile> -w <# of workers>\n", argv[0]); return -51; }
  		}
	}

	//metrisi path num
	int lines = 1;
	char ch;
  	while(!feof(fp))   //count file lines
	{	
  		ch = fgetc(fp);
  		if(ch == '\n')
  		{	
    		lines++;   		
  		}
	}
	printf("\n");
	int pathnum = lines;  //sunolikes grammes text (plithos directories)

 	if (pathnum < workers) //an exw ligotera paths ap osous workers, tote na ftiaxnei mono tosous workers osa ta paths
 	{
 		workers = pathnum;

 	}

 	rewind(fp);  //ksanathetw ton pointer stin arxi tou arxeiou

 	//dimiourgia pinaka me paths
 	char *paths[pathnum];

 	//extract paths
 	size_t len = 0 ;	//buffer length - initialisation se 0 wste na kanei moni tis realloc(leitourgia getline me buffersize = 0)
	char *temp=NULL;

	int counter = 0;
	int linelen;
 	while(!feof(fp))    //apothikeusi ston pinaka "paths"
	{	

  		linelen = getline(&temp, &len, fp);
  		if( ( temp!=NULL && (strcmp(temp, "\n")!=0)))
  		{
 			int llen = strlen(temp)+1;
	  		char temp2[llen];
	  		strcpy(temp2, temp);
	  		
		  	if (temp2[llen-2] == '\n') 
	  		{
	     		temp2[llen-2] = '\0';
	     		llen --;
	  
	  		}
	  	
	  		paths[counter] = (char* ) malloc(llen*sizeof(char));
		  	strcpy(paths[counter], temp2); //antigrafi tou document ston pinaka path

		  	counter++;
		}
  	}

  	free(temp);

  	fclose(fp);

  	//creation of pipes

  	char *fifojw[workers];
	char *fifowj[workers];

	//making children
	int k;
	int fdwj[workers];
	for (k = 0; k<workers; k++)
	{

		char jw[sizeof "jwpipe000"];
		char wj[sizeof "wjpipe000"];

    	sprintf(jw, "jwpipe%03d", k);
    	sprintf(wj, "wjpipe%03d", k);

    	fifojw[k] = (char*)malloc (sizeof (jw));
    	strcpy(fifojw[k], jw);

    	//se periptwsi pou uparxei idi 
    	struct stat stat1;
	    if ( stat( fifojw[k], &stat1 ) < 0 )
	    {
	        if ( errno != ENOENT )          // ENOENT is ok, since we intend to delete the file anyways
	        {
	            perror( "stat failed" );    // any other error is a problem
	            return( -1 );
	        }
	    }
	    else                                // stat succeeded, so the file exists
	    {
	        if ( unlink( fifojw[k] ) < 0 )   // attempt to delete the file
	        {
	            perror( "unlink failed" );  // the most likely error is EBUSY, indicating that some other process is using the file
	            return( -1 );
	        }
	    }
  		if ( mkfifo(fifojw[k], 0666) == -1 )
		{ 
			if ( errno!=EEXIST ) 
			{
				perror("receiver: mkfifo1"); exit(6);
			}
		}
    	
    	
    	int tempss = sizeof(wj);
    	fifowj[k] = (char*)malloc (sizeof(wj));
    	strcpy(fifowj[k], wj);

    	struct stat stat2;
	    if ( stat( fifowj[k], &stat2 ) < 0 )
	    {
	        if ( errno != ENOENT )          // ENOENT is ok, since we intend to delete the file anyways
	        {
	            perror( "stat failed" );    // any other error is a problem
	            return( -1 );
	        }
	    }
	    else                                // stat succeeded, so the file exists
	    {
	        if ( unlink( fifowj[k] ) < 0 )   // attempt to delete the file
	        {
	            perror( "unlink failed" );  // the most likely error is EBUSY, indicating that some other process is using the file
	            return( -1 );
	        }
	    }
    	if ( mkfifo(fifowj[k], 0666) == -1 )
		{ 
			if ( errno!=EEXIST ) 
			{
				perror("receiver: mkfifo2"); exit(6);
			}
		}

		pid_t pid = fork();

		if (pid ==0)  //------WORKER----------
		{
			
			//create log dir if it does not exist
			struct stat st = {0};

			if (stat("./log", &st) == -1) {
			    mkdir("./log", 0755);
			}

			//create - open logfile to write
			pid_t pid = getpid();
			int namesize = strlen("Worker_.txt") + sizeof(pid_t) + 1;
			char lfile[namesize];
			sprintf(lfile, "Worker_%d.txt", pid);
			int pathsize =strlen("./log/Worker_.txt") + sizeof(pid_t) + 1;
			char lpath[pathsize];
			sprintf(lpath, "./log/Worker_%d.txt", pid);

			FILE *logfile = fopen(lpath, "w");  

			int fdjw;
			int fdwj;
			char msgbuf[MSGSIZE];

			printf("Worker %d (pid: %d) started\n" , k, getpid());

			//open pipes
			if ( (fdjw=open(fifojw[k],  O_RDONLY)) < 0)
			{ 
				if ( errno!=EEXIST ) 
				{	
					perror("fifo open error"); exit(1);
				}
			}

			if ( (fdwj=open(fifowj[k],  O_WRONLY)) < 0)
			{ 
				if ( errno!=EEXIST ) 
				{	
					perror("fifo open error"); exit(1);
				}
			}

			//read num of paths from jobex
			int numofpaths;
			if ( read(fdjw, &numofpaths , sizeof(int)) < 0) 
			{ 	perror("problem in reading");
				exit(5);
			}

			//pinakas me paths sto k-osto paidi
			char *pathmap[numofpaths];
			int sizetoreceive;

			//save paths to array pathmap
			for(int o = 0; o<numofpaths; o++)
			{	
				//read size of path
				if ( read(fdjw, &sizetoreceive, sizeof(int)) < 0) 
				{ 	perror("problem in reading");
					exit(5);
				}			

				//read path from parent
				if ( read(fdjw, msgbuf, sizetoreceive) < 0) 
				{ 	perror("problem in reading");
					exit(5);
				}	
	
				pathmap[o] = (char*) malloc(sizetoreceive);
				strncpy(pathmap[o], msgbuf, sizetoreceive);
				
			}

			//metrisi documents
			int docnum = 0;
			int counterarxeia = 0;
			for(int m = 0; m < numofpaths; m++)
			{
				docnum = countdocs(pathmap[m]);
				counterarxeia += docnum ;
			}

			//dimiourgia domimap pinaka me info gia kathe arxeio
			struct domimap *docinfo[counterarxeia]; //se kathe thesi tha exw to antistoixo arxeio tis thesis tou pathmap
			for (int b = 0; b < counterarxeia; b++)
				docinfo[b] = (struct domimap*) malloc (sizeof(struct domimap));

			char *docmap[counterarxeia];


			//dimiourgia domis info - domimap 
			create_docinfo(pathmap, docmap , docinfo, numofpaths, counterarxeia);
			
			//wait for commands from jobexec
			//read command
			int commandnum; //1 - mincount, 2 - maxcount, 3 - wc, 4 - search, 5 - exit
			if ( read(fdjw, &commandnum, sizeof(int)) < 0) 
			{ 	perror("problem in reading");
				exit(5);
			}
			
			// - logfile timestamp
			time_t      t;
		    struct tm   ttm;
		    t = time(NULL);
		    ttm = * localtime(&t);		    
		    time_t ttime = time(NULL);
			struct tm *tm = localtime(&ttime);
			char date[20];
			strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", tm);	

			int test;
			int nwrite;
			while( commandnum != 5 )
			{
				if (commandnum == 1) //mincount
				{ 
					char *key;
					int keysize;
					char *pathsend;
					int sizepath;
					int emfan = 0, idkeimenou;

					//read size of keyword
					if ( read(fdjw, &keysize, sizeof(int)) < 0) 
					{ 	perror("problem in reading");
						exit(5);
					}	

					key = (char*) malloc (sizeof(char)*keysize);
					//read keyword
					if ( read(fdjw, key, keysize) < 0) 
					{ 	perror("problem in reading");
						exit(5);
					}	

					emfan = min_tf(key, &idkeimenou);
					//send emfaniseis keyword
					if ((nwrite=write(fdwj, &emfan, sizeof(int))) == -1)
						{ perror("Error in Writing"); exit(2); }

					
					if (emfan>0)
					{

						int ssizeofpath = strlen(docmap[idkeimenou])+1;
						pathsend = (char*) malloc (sizeof(char)*ssizeofpath);
						strcpy(pathsend, docmap[idkeimenou]);

						fprintf(logfile, "Time_of_query_arrival: %s ", date);
						fprintf(logfile, "| maxcount | %s | %s\n", key, pathsend );

						//send size of path
						if ((nwrite=write(fdwj, &ssizeofpath, sizeof(int))) == -1)
						{ perror("Error in Writing"); exit(2); }

						//send path 
						if ((nwrite=write(fdwj, pathsend, ssizeofpath)) == -1)
						{ perror("Error in Writing"); exit(2); }

						free(pathsend);
					}

					free(key);

				}
				else if (commandnum == 2) //maxcount -antistoixa me mincount
				{
					char *key;
					int keysize;
					char *pathsend;
					int sizepath;
					int emfan = 0, idkeimenou;

					if ( read(fdjw, &keysize, sizeof(int)) < 0) 
					{ 	perror("problem in reading");
						exit(5);
					}	

					key = (char*) malloc (sizeof(char)*keysize);
					if ( read(fdjw, key, keysize) < 0) 
					{ 	perror("problem in reading");
						exit(5);
					}	
			
					emfan = max_tf(key, &idkeimenou);

					if ((nwrite=write(fdwj, &emfan, sizeof(int))) == -1)
						{ perror("Error in Writing"); exit(2); }
					
					if (emfan>0)
					{
						int ssizeofpath = strlen(docmap[idkeimenou])+1;
						pathsend = (char*) malloc (sizeof(char)*ssizeofpath);
						strcpy(pathsend, docmap[idkeimenou]);

						fprintf(logfile, "Time_of_query_arrival: %s ", date);
						fprintf(logfile, "| maxcount | %s | %s\n", key, pathsend );


						if ((nwrite=write(fdwj, &ssizeofpath, sizeof(int))) == -1)
						{ perror("Error in Writing"); exit(2); }

						if ((nwrite=write(fdwj, pathsend, ssizeofpath)) == -1)
						{ perror("Error in Writing"); exit(2); }

						free(pathsend);
					}

					free(key);

					

				}
				else if (commandnum == 3) //wc 
				{
					
					int schars = 0, swords = 0, slines = 0;
					for(int u = 0; u < counterarxeia; u++)
					{
						schars += docinfo[u]-> characters;
						swords += docinfo[u]-> words;
						slines += docinfo[u]-> lines;
						
					}

					int data[3];
					data[0] = schars; 	//counter xaraktirwn
					data[1] = swords;	//counter leksewn
					data[2] = slines;	//counter grammwn

					for(int u = 0; u < 3; u++)
					{
						//send plithos twn parapanw
						if ((nwrite=write(fdwj, &(data[u]), sizeof(int))) == -1)
						{ perror("Error in Writing"); exit(2); }
					}

				}
				else if (commandnum == 4) //search
				{	
				 	//read size of query
					if ( read(fdjw, &sizetoreceive, sizeof(int)) < 0) 
					{ 	perror("problem in reading");
						exit(5);
					}			

					char* temp2 = (char*)malloc(sizetoreceive*sizeof(char));
					char *ypoloipo;
					char* temp22 = (char*)malloc(sizetoreceive*sizeof(char));
					char *ptemp2, *ptemp22;

					//read query
					if ( read(fdjw, temp2, sizetoreceive) < 0) 
					{ 	perror("problem in reading");
						exit(5);
					}

					strncpy(temp22, temp2, sizetoreceive);
					ptemp2 = temp2; //pointer for free later
					ptemp22 = temp22; //pointer for free later

					char delimiters[] = " \t\n";
					char* quer;
					quer = strtok_r(temp2,delimiters,&ypoloipo);

					//printf("Quer = %s\n", quer );
					if((quer == NULL) ) //an exw null query - error - double check kanw check kai ston parent
					{	
						printf("ERROR\nUsage: /search <query1> <query2> ... <query10>\n");
					}
					else
					{

						struct Trie_t *sres = NULL;
						int atleast1found= 0;
						int q = 0;  //plithos queries 

						int answer = 0;  // 0 -- telos query, 9 -- vrike ti leksi -- -9 de vrike ti leksi
						while ((quer !=NULL) && (q<10) && ((strcmp(quer, "-d"))!=0) )  //psaxnw gia to polu 10 queries an uparxoun sto arxeio 
						{		
							
							sres = search_trie(quer);  //psaxnw ti leksi kai epistrefw ton komvo tou teleutaiou grammatos h null an den uparxei
							
							if(sres == NULL)
							{												
								//steile -9 ston parent gia na dilwsei oti de vrike ti leksi
								answer = -9;
								if ((nwrite=write(fdwj, &answer, sizeof(int))) == -1)
								{ perror("Error in Writing"); exit(2); }
							}
							else
							{	

								if (!atleast1found) //vrika estw ena query
									atleast1found= 1;


								//write to logfile
								fprintf(logfile, "Time_of_query_arrival: %s ", date);

								fprintf(logfile, "| search | %s |", quer );

								struct Plist *templist = sres -> postlist;
								struct lista *lines;
								struct lines *doclines;
								int temppathsize = 0;
								do 
								{
									lines = (sres->postlist) -> grammes;
									int thesi = templist -> id;
									temppathsize = strlen(docmap[thesi])+1;

									//send path size
									if ((nwrite=write(fdwj, &temppathsize, sizeof(int))) == -1)
									{ perror("Error in Writing"); exit(2); }

									strncpy(msgbuf, docmap[thesi], temppathsize );

									//send path
									if ((nwrite=write(fdwj, msgbuf, temppathsize)) == -1)
									{ perror("Error in Writing"); exit(2); }

									//write path to logfile
									fprintf(logfile, " %s", docmap[thesi] );

									//---edw egine prospatheia apostolis grammwn kai periexomenou alla upirxe bug pou de vrethike

									/*int sizetosend = templist -> sumlines;
									printf("\nsending %d\n", sizetosend );
									if ((nwrite=write(fdwj, &sizetosend, sizeof(int))) == -1)
									{ perror("Error in Writing"); exit(2); }

									int tplines = sizetosend;
									for(int y = 0; y < tplines ; y++)
									{
										printf("Y = %d\n", y );
										sizetosend = (lines -> gramminum);
										printf("\nsending %d\n", sizetosend );
										if ((nwrite=write(fdwj, &sizetosend, sizeof(int))) == -1)
										{ perror("Error in Writing"); exit(2); }

										doclines = docinfo[thesi] -> grammesarxeiou;
										int l ;
										while((l= doclines -> numgrammis) != sizetosend)
										{
											printf("\nl = %d - thelw = %d\n", l, sizetosend );
											doclines = doclines -> next;
										}

										char *grammisend = doclines-> grammi;

										sizetosend = strlen(grammisend )+1;
										printf("\nsending %d\n", sizetosend );

										if ((nwrite=write(fdwj, &sizetosend, sizeof(int))) == -1)
										{ perror("Error in Writing"); exit(2); }

										printf("\nSending %s worker %d\n", grammisend, k );
										if ((nwrite=write(fdwj, grammisend, sizetosend)) == -1)
										{ perror("Error in Writing"); exit(2); }

										printf("Gramminum = %d\n", lines -> gramminum );
										printf("worker %d\n", k );
										
										printf("Lines next %s\n", lines->next );
										lines = lines -> next;
										//printf("Gramminum = %d\n", lines -> gramminum );
				

									}-----*/


									templist = templist ->next;

								}while (templist !=NULL);

							}
							printf("\n");

							fprintf(logfile, "\n" );
							
							q++;
							temp2= ypoloipo;
							quer = strtok_r(temp2,delimiters,&ypoloipo); //epomeno query 
						}

						answer = 0;

						//telos apostolis path - send to parent
						if ((nwrite=write(fdwj, &answer, sizeof(int))) == -1)
						{ perror("Error in Writing"); exit(2); }


						if (atleast1found )  		//an exw vrei estw ena query oti uparxei
						{
							
							//make the argument array			
							char* token ;
							char d[] = " \t\n";
							char *argarray[q];
							char *temp1, *temp12;
							int l = 0;
							//printf("Array %s\n", temp22 );
							while ((l < q) && ((token = strtok_r(temp22,d,&temp12))!=NULL)  )//krataw ta queries se pinaka
							{	
								argarray[l] = (char*)malloc((strlen(token)+1)*sizeof(char));
								strcpy(argarray[l], token);						
								temp22 = temp12;
								l++;
							}
							
							printf("\n");

							for (int w = 0; w<q; w++)
								free(argarray[w]);
						}

					}

					free(ptemp2);
					free(ptemp22);
				}

				//read next command
				if ( read(fdjw, &commandnum, sizeof(int)) < 0) 
				{ 	perror("problem in reading");
					exit(5);
				}	

				//new timestamp
			    struct tm   ttm;
			    time_t ttime = time(NULL);
			    ttm = *localtime(&ttime);		
				struct tm *tm = localtime(&ttime);
				strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", tm);
				
			}

			close (fdjw);
			unlink(fifojw[k]);
			free(fifojw[k]);

			close (fdwj);
			unlink(fifowj[k]);
			free(fifowj[k]);

			//free memory of child
			for (int r = 0; r < numofpaths; r++)
			{
				free(pathmap[r]);
			}

			for (int r = 0; r < counterarxeia; r++)
			{
				struct lines *temp1, *temp2;
				temp1 = docinfo[r]->grammesarxeiou;
				
				if (temp1!=NULL)
				{
					do
					{
						temp2 = temp1 -> next;
						free(temp1);
						temp1 = temp2;

					}while (temp2!=NULL);
				}

				free(docinfo[r]);  //edw isws yparxei thema me tis grammes
				free(docmap[r]);
			}
			trie_destr(trie->child);
			trie->child = NULL;
			free(trie);
			trie = NULL;  

			printf("\nChild with pid: %d freed memory & exited (worker %d)\n", getpid(), k );

			fclose(logfile);

			return 1;

		}
		else if(pid ==-1)
		{
			perror("fork call");
			return -4;
		}

	}


	//---------JOBEXECUTOR--------
	int nwrite;

	//fd's of pipes ston parent
	int fdjw[workers];
	char msgbuf[MSGSIZE];

	//diamoirasmos path
	int pathseach = 1; //an exw iso arithmo workers kai paths, pairnei ena o kathe worker
	int ypol = 0;

	if (pathnum!=workers)
	{
		pathseach = (int)(pathnum / workers);
		ypol = pathnum % workers; //an einai >0 tote oi ypol prwtoi tha paroun +1 path
	}

	int sizetosend;
	int thesipathmap = 0;
  	//apostoli plithous paths stous workers wste na kanoun statiko path map kai ekeinoi
	for (int l = 0; l < workers ; l++)  //se kathe worker stelnw me ti seira ta paths
	{
		int tempathseach;
		if ( (fdjw[l]=open(fifojw[l], O_WRONLY)) < 0)
		{ perror("fifo open problem"); exit(3); }
		
		if ( (fdwj[l]=open(fifowj[l], O_RDONLY)) < 0)
		{ perror("fifo open problem"); exit(3); }

		if(ypol>0)
		{
			tempathseach = pathseach+1;
			ypol--;
		}
		else
		{
			tempathseach = pathseach;
		}

		//apostoli plithous paths
		if ((nwrite=write(fdjw[l], &tempathseach, sizeof(tempathseach))) == -1)
			{ perror("Error in Writing"); exit(2); }

		int n;
		//apostoli antistoixwn path ston kathe worker
		for (n = thesipathmap; n < (thesipathmap + tempathseach); n++) //pernw kathe path ston antistoixo worker
		{
			//send path size
			sizetosend = strlen(paths[n])+1;
			if ((nwrite=write(fdjw[l], &sizetosend, sizeof(int))) == -1)
				{ perror("Error in Writing"); exit(2); }

			strcpy(msgbuf, paths[n]);
			//send path
			if ((nwrite=write(fdjw[l], msgbuf, sizetosend)) == -1)
				{ perror("Error in Writing"); exit(2); }
	
		}

		thesipathmap = n;

	}

  	//entoli apo xristi se loop
 	len = 0 ; 	//me len (buffersize) 0, i getline kanei realloc oso xreiazetai gia to input
 	char *input = NULL, *quer, *temp2; 
 	char delimiters[] = " \t\n";

 	printf("\n");
 	printf("\nSelect mode:\n1. \"/search <query1> <query2> ... <query10> -d <deadline>\"\n");
 	printf("2. \"/maxcount <keyword>\"\n3. \"/mincount <keyword>\"\n4. \"/exit\"\n");
 	printf("Enter command: ");

	if ((linelen = getline(&input, &len, stdin)) == -1)
	{	perror("\nGetline\n");  }
	char* command = strtok_r(input,delimiters,&temp2);

	while ((command == NULL)) //pairnw input apo stdin mexri na dwthei swsta mia entoli
	{
		printf("\nCommand must be one of the following!\n");
		printf("\nSelect mode:\n1. \"/search <query1> <query2> ... <query10> -d <deadline>\"\n");
 		printf("2. \"/maxcount <keyword>\"\n3. \"/mincount <keyword>\"\n4. \"/exit\"\n");
 		printf("Enter command: ");
		if ((linelen = getline(&input, &len, stdin)) == -1)
 		{	perror("\nGetline\n");  }
		command = strtok_r(input,delimiters,&temp2);
	}

	int test;
	while ((test=(strcmp(command, "/exit"))!=0) )
	{
		if ((strcmp(command, "/mincount"))==0)
		{
			
			char* leksi, *temp1;

			if((leksi = strtok_r(temp2,delimiters,&temp1))!=NULL ) //an exw 1o orisma meta tin entoli "/tf"
			{
				printf("\n-Mincount mode selected-\n");
				
				sizetosend = 1;
				//send command code
				for (int l = 0; l < workers; l++)
				{
					if ((nwrite=write(fdjw[l], &sizetosend, sizeof(int))) == -1)
						{ perror("Error in Writing"); exit(2); }
				}

				int keysize = strlen(leksi) + 1;
				char* keywordtosend = (char*) malloc(sizeof(char)*keysize);
				
				strcpy(keywordtosend, leksi);

				for (int l = 0; l < workers; l++)
				{
					//send keyword size
					if ((nwrite=write(fdjw[l], &keysize, sizeof(int))) == -1)
						{ perror("Error in Writing"); exit(2); }

					//send keyword
					if ((nwrite=write(fdjw[l], keywordtosend, keysize)) == -1)
						{ perror("Error in Writing"); exit(2); }
				
				}

				char *pathmin = NULL;
				int min = 0;
				for (int l = 0; l < workers; l++)
				{	

					int emfan;
					//read emfaniseis
					if ( read(fdwj[l], &emfan, sizeof(int)) < 0) 
					{ 	perror("problem in reading");
						exit(5);
					}	
					
					if (emfan>0) //an uparxei toylaxiston 1 fora i leksi
					{
						int psize;
						//read path size
						if ( read(fdwj[l], &psize, sizeof(int)) < 0) 
						{ 	perror("problem in reading");
							exit(5);
						}	

						char *keypath = (char*) malloc(sizeof(char)*psize);
						//read path
						if ( read(fdwj[l], keypath, psize) < 0) 
						{ 	perror("problem in reading");
							exit(5);
						}	
					
						if(l == 0)
						{
							pathmin = (char*) malloc(sizeof(char)*psize);
							strcpy (pathmin, keypath);
							min = emfan;
						} 
						else if ( emfan < min)
						{
							pathmin = (char*) realloc(pathmin ,sizeof(char)*psize);
							strcpy (pathmin, keypath);
							min = emfan;
						}

						free(keypath);

					}

				}
				if (min > 0)
				{
					printf("\nKeyword \"%s\" least frequent (%d times) in file:\n%s\n", keywordtosend, min, pathmin);
					free(pathmin);
				}
				else
				{
					printf("\nKeyword \"%s\" not found!\n", keywordtosend );
				}

				
				free(keywordtosend);
					
			}
			else
			{ 	printf("\nERROR\nUsage: /maxcount <query>\n"); }



		}
		else if ((strcmp(command, "/maxcount"))==0)
		{			
			

			char* leksi, *temp1;

			if((leksi = strtok_r(temp2,delimiters,&temp1))!=NULL ) //an exw 1o orisma meta tin entoli "/tf"
			{
				printf("\n-Maxcount mode selected-\n");

				sizetosend = 2;
				for (int l = 0; l < workers; l++)
				{
					if ((nwrite=write(fdjw[l], &sizetosend, sizeof(int))) == -1)
						{ perror("Error in Writing"); exit(2); }
				}

			
				int keysize = strlen(leksi) + 1;
				char* keywordtosend = (char*) malloc(sizeof(char)*keysize);
				
				strcpy(keywordtosend, leksi);

				for (int l = 0; l < workers; l++)
				{
					if ((nwrite=write(fdjw[l], &keysize, sizeof(int))) == -1)
						{ perror("Error in Writing"); exit(2); }

					if ((nwrite=write(fdjw[l], keywordtosend, keysize)) == -1)
						{ perror("Error in Writing"); exit(2); }
				
				}


				char *pathmax = NULL;
				int max = 0;
				for (int l = 0; l < workers; l++)
				{
					
					int emfan;
					if ( read(fdwj[l], &emfan, sizeof(int)) < 0) 
					{ 	perror("problem in reading");
						exit(5);
					}	

					if (emfan>0)
					{

						int psize;
						if ( read(fdwj[l], &psize, sizeof(int)) < 0) 
						{ 	perror("problem in reading");
							exit(5);
						}	

						char *keypath = (char*) malloc(sizeof(char)*psize);
						if ( read(fdwj[l], keypath, psize) < 0) 
						{ 	perror("problem in reading");
							exit(5);
						}	
						

						if(l == 0)
						{
							pathmax = (char*) malloc(sizeof(char)*psize);
							strcpy (pathmax, keypath);
							max = emfan;
						} 
						else if ( emfan > max)
						{
							pathmax = (char*) realloc(pathmax ,sizeof(char)*psize);
							strcpy (pathmax, keypath);
							max = emfan;
						}

						free(keypath);
					}

				}

				if (max > 0)
				{
					printf("\nKeyword: %s most frequent (%d times) in file:\n%s\n", keywordtosend, max, pathmax);
					free(pathmax);
				}
				else
				{
					printf("\nKeyword %s not found!\n", keywordtosend );
				}

				
				free(keywordtosend);
							
			}
			else
			{ 	printf("\nERROR\nUsage: /maxcount <query>\n"); }


		}
		else if ((strcmp(command, "/wc"))==0)
		{
			printf("\n-Word Count mode selected-\n");
			sizetosend = 3;

			for (int l = 0; l < workers; l++)
			{
				if ((nwrite=write(fdjw[l], &sizetosend, sizeof(int))) == -1)
				{ perror("Error in Writing"); exit(2); }

			}

			int times[3]; //chars 0 - words 1 - lines 2
			times[0] = 0;  	//characters
			times[1] = 0;	//words
			times[2] = 0;	//lines

			for (int j = 0; j< workers; j++)
			{
				//diavazei ta apotelesmata apo kathe worker kai ta athroizei
				
				for (int b = 0; b < 3; b++)
				{
					int timi;

					//diavazei timi
					if ( read(fdwj[j], &timi, sizeof(int)) < 0) 
					{ 	perror("problem in reading");
						exit(5);
					}	
					times[b] += timi;

				}

			}

			printf("\nIn Files:\nTotal Characters: %d\nTotal words: %d\nTotal Lines: %d\n", times[0], times[1], times[2]);

		}
		else if ((strcmp(command, "/search"))==0)
		{

			char *argstring ;
			int argnum = 0;
			
			printf("\n-Search mode selected-\n");
		
			if (temp2!=NULL)  
			{
				
				argstring =  (char*) malloc((strlen(temp2)+1)*sizeof(char));					
				strcpy(argstring, temp2); //antigrafw ta queries gia epeksergasia
				char *pargstring = argstring; //gia free

				long deadline;
				char *ypol;
				char *temptoken = strtok_r(temp2,delimiters,&ypol);
				if (temptoken==NULL) 
				{
					printf("ERROR\nUsage: /search <query1> <query2> ... <query10> -d <deadline>\n");
				}
				else
				{

					while((temptoken!=NULL ) && ((strcmp(temptoken, "-d"))!=0)  )
					{
						temp2 = ypol;
						temptoken = strtok_r(temp2,delimiters,&ypol);
					}
					if (ypol!=NULL )
					{
						sizetosend = 4;
						for (int l = 0; l < workers; l++)
						{
							if ((nwrite=write(fdjw[l], &sizetosend, sizeof(int))) == -1)
								{ perror("Error in Writing"); exit(2); }

						}
						
						//ypologiza na pairnei kai to deadline an upirxe alla den to prolava
						deadline = atoi(ypol);  
						//ypothetw oti tha vazei panta mono noumero kai oxi gramma sto deadline 

						//deadline
						struct timespec ts;  //eidiki struct pou periexei tin wra se nsec
		            	long  timetemp1,timetemp2; /* timestamp in microsecond. */
						timespec_get(&ts, TIME_UTC);
		                timetemp2 =(long)ts.tv_sec * 1000000000L + ts.tv_nsec;

						sizetosend = strlen(argstring)+1;
						//send query to workers
						for (int l = 0; l < workers; l++)
						{
							if ((nwrite=write(fdjw[l], &sizetosend, sizeof(int))) == -1)
							{ perror("Error in Writing"); exit(2); }

							strcpy(msgbuf, argstring);
							if ((nwrite=write(fdjw[l], msgbuf, sizetosend)) == -1)
							{ perror("Error in Writing"); exit(2); }
						}

						int sizetoreceive;
						int myvar = 0;
						for (int b = 0; b < workers; b++)
						{					
							//mode - 0 , 9  or -9 analoga me to an vrike ti leksi 
							if ( read(fdwj[b], &sizetoreceive, sizeof(int)) < 0) 
							{ 	perror("problem in reading");
								exit(5);
							}	
							
							while ((sizetoreceive!=0))
							{

								
								if (sizetoreceive == -9)  //de vrike ti leksi - no paths available to read
								{
									if ( read(fdwj[b], &sizetoreceive, sizeof(int)) < 0) 
									{ 	perror("problem in reading");
										exit(5);
									}	
								}
								else
								{
									
									myvar++; //an vrika estw mia leksi
									
									while(sizetoreceive!=0) 
									{
										
										if (sizetoreceive > 0)
										{

											char* ppath = (char*)malloc(sizetoreceive*sizeof(char));
											
											if ( read(fdwj[b], ppath, sizetoreceive) < 0) 
											{ 	perror("problem in reading");
												exit(5);
											}
											printf("\nKeyword found in file: %s\n", ppath );


											//edw egine prospatheia gia lista me grammes kai periexomena alla eixe bug 
											/*int grammesnum;
											if ( read(fdwj[b], &grammesnum, sizeof(int)) < 0) 
											{ 	perror("problem in reading");
												exit(5);

											}	
											printf("\nreceiving %d grammes from worker %d\n", grammesnum, b );


											for (int grammi = 0; grammi < grammesnum; grammi++)
											{
												int grammifile ;
												int sizegramm;
												char *gram;
												if ( read(fdwj[b], &grammifile, sizeof(int)) < 0) 
												{ 	perror("problem in reading");
													exit(5);

												}	
												printf("\nreceiving %d grammi from worker %d\n", grammifile, b );

												if ( read(fdwj[b], &sizegramm, sizeof(int)) < 0) 
												{ 	perror("problem in reading");
													exit(5);

												}	

												gram = (char*)malloc(sizegramm*sizeof(char));
												printf("\nreceiving grammi me size %d from worker %d\n", sizegramm, b );

												if ( read(fdwj[b], gram, sizegramm) < 0) 
												{ 	perror("problem in reading");
													exit(5);

												}	
												printf("\nreceiving grammi : %s from worker %d\n", gram, b );

												

											}*/

											//strncpy(temp22, temp2, sizetoreceive);

											free(ppath);
										}

										//neo mode

										if ( read(fdwj[b], &sizetoreceive, sizeof(int)) < 0) 
										{ 	perror("problem in reading");
											exit(5);

										}	

									}

								}

							}

						}

						if (!myvar) //an de vrika kamia leksi
						{
							printf("\nKeywords not found in any file!\n\n");
						}

					}
					else
					{
						printf("ERROR\nUsage: /search <query1> <query2> ... <query10> -d <deadline>\n");

					}
				}

				free (pargstring);

			}
			else
			{ 	
				printf("\nCommand must be one of the following !\n");

			}


		}
		else
		{

			printf("\nCommand must be one of the following !\n");
		}


		printf("\nSelect mode:\n1. \"/search <query1> <query2> ... <query10> -d <deadline>\"\n");
 		printf("2. \"/maxcount <keyword>\"\n3. \"/mincount <keyword>\"\n4. \"/exit\"\n");
 		printf("Enter command: ");

		if ((linelen = getline(&input, &len, stdin)) == -1)
 		{	perror("\nGetline\n");  }
		command = strtok_r(input,delimiters,&temp2);
		while ((command == NULL))
		{
			printf("\nCommand must be one of the following !\n");
			printf("\n1. \"/search <query1> <query2> ... <query10> -d <deadline>\"\n");
 			printf("2. \"/maxcount <keyword>\"\n3. \"/mincount <keyword>\"\n4. \"/exit\"\n");
 			printf("Enter command: ");
			if ((linelen = getline(&input, &len, stdin)) == -1)
 			{	perror("\nGetline\n");  }
			command = strtok_r(input,delimiters,&temp2);
		}
	}
	free(input);


	//jobexec tells children to terminate by sending 5 value
	
	sizetosend = 5;
	for (int l = 0; l < workers; l++)
	{
		if ((nwrite=write(fdjw[l], &sizetosend, sizeof(int))) == -1)
			{ perror("Error in Writing"); exit(2); }

	}

	for (int k = 0; k<workers; k++)
	{
		//printf("Worker %d termatise\n", k);
		wait(NULL);
	}

	//free domes
	for (int p = 0; p < workers; p++)
	{
		close(fdjw[p]);
		close(fdwj[p]);
	}
	
	for (int r = 0; r < workers; r++)
	{
		unlink(fifojw[r]);
		free(fifojw[r]);

		unlink(fifowj[r]);
		free(fifowj[r]);
	}

	for (int r = 0; r < pathnum; r++)
	{
		free(paths[r]);
	}


	*paths = NULL;


	printf("\n");

	return 1;
}



