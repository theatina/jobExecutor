////////////////////////////////
//	
//	Christina - Theano Kylafi
//
///////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "funs.h"
#include "structs.h"
#include "trie.h"

struct Trie_t* trie;

//doc frequency function
int df_k( struct Trie_t *komvos)  //document frequency me dosmeno komvo grammatos 
{	
	int docfreq = komvos -> df;
	return docfreq;
}

//posting list function
void posting_list(struct Trie_t* komvos, int idnum, int grammi)   
{	
	struct Plist *plist, *temp1, *temp2 = NULL, *new;
	int docfreq = df_k(komvos);
	int temp;

	plist = (komvos -> postlist);
	if(plist == NULL)   //an den exw posting list, dimiourgw ton prwto komvo me katalliles arxikopoihseis
	{
		new = malloc(sizeof(struct Plist));
		new -> id = idnum; 
		new -> freq = 1;
		//new -> sumlines = 1;
		new -> grammes = NULL;
		new -> next = NULL;

		docfreq ++ ;
		komvos -> df = docfreq;

		//lines list -- prospathisa na ylopoihsw kai tis grammes alla eixe bug
		/*struct lista *newline = malloc(sizeof(struct lista));
		newline->gramminum = grammi;
		newline ->next  = NULL;
		new ->grammes = newline;*/

		(komvos -> postlist) = new;


	}
	else //an exw idi posting list, psaxnw an uparxei idi komvos me id keimenou idnum
	{
		temp2 = plist;

		while ( ((temp2 ->id) != idnum)  && (temp1 = temp2->next) != NULL   )  
		{	temp2 = temp1; }

		if ( idnum == (temp2->id) )  	//an uparxei idi posting list kai pair gia to keimeno me arithmo "idnum" tote auksanw emfaniseis
		{
			temp = temp2->freq;
			temp++;
			temp2 -> freq = temp;

			//lines list - an uparxei i leksi idi sto arxeio psaxnw na dw an uparxei i grammi
			//struct lista *tempor1 = NULL, *tempor2 = NULL;
			//tempor2 = temp2->grammes;
			//tempor1 = tempor2;
			//printf("Id - %d\n", idnum );
			//printf("Tempor grammi %d\n", tempor2->gramminum);
			//tsekare an uparxei i grammi , an oxi, kane komvo an uparxei tipota
			/*int eureka = 0;	
			while ( tempor1 != NULL  )  
			{	
				if ( (tempor1-> gramminum) == grammi)
				{
					eureka = 1;
				}
				//printf("\nGrammi listas %d\n", tempor1 ->gramminum);
				tempor2 = tempor1;
				tempor1 = tempor1->next; 

			}*/

			//printf("\nGrammi prin %d - exw %d\n", tempor2->gramminum, grammi);
			/*if ( !eureka )  
			{
				//printf("exw grammi = %d, vazw grammi %d\n", tempor2->gramminum, grammi );
				struct lista *newline = malloc(sizeof(struct lista));
				newline -> gramminum = grammi;
				newline -> next  = NULL;
				tempor2 -> next  = newline;
				//printf("tempor2 next %d\n", newline->gramminum );
				int plines = temp2 -> sumlines;
				plines++;
				temp2 -> sumlines = plines;
			}
			*/



		}
		else//an den uparxei emfanisi gia to sugekrimeno keimeno, prostithetai sto telos o komvos
		{
			new = malloc(sizeof(struct Plist));
			new -> id = idnum;
			new -> freq = 1;
			//new -> sumlines = 1;
			new -> grammes = NULL;
			new -> next = NULL;

			//pleon emfanizetai se ena epipleon keimeno
			docfreq ++ ;
			komvos -> df = docfreq;

			//lines list
			/*struct lista *newline = malloc(sizeof(struct lista));
			newline->gramminum = grammi;
			newline ->next  = NULL;
			new ->grammes = newline;*/

			temp2-> next = new;   //o teleutaios komvos pia einai o neos, to neo zeugari keimenou - emfanisis


		}	
	}

}



//search trie 
struct Trie_t* search_trie(char* word)
{
	int y;
	int length = strlen (word);
	char ch;
	struct Trie_t *temp2, *temp1;

	if((trie->child)!=NULL)  //elegxos an to trie einai keno
	{  	temp2 = trie->child; }
	else
	{ 	printf("\nERROR: empty trie!\n"); return NULL; }

	int eow = 0;  	//end of word leksis pou psaxnw
	int found = 0; 	//flag gia to an vrethike i leksi pou psaxnw

	for (y=0; y < length; y++)  //gia kathe gramma tis leksis
	{
		ch = *(word+y); //proxwraw to deikti tis leksis sto epomeno char mexri ton "\0"

		if (y==length-1) //an eimai sto teleutaio gramma , tote exw end of word pou psaxnw
		{	eow = 1; }

		while (( ch != (temp2->gramma) )  && ((temp1 = temp2->next) != NULL)  )  
		{	
			temp2 = temp1;
		}

		if ( ch == (temp2->gramma)  )  //an vrw gramma idio me tis leksis pou psaxnw
		{	
			if(eow  && ((temp2->postlist)!=NULL))  //an exw telos leksis pou psaxnw kai telos leksis sto trie, vrika ti leksi  
			{	found = 1; return temp2; }  //epistrefw ton pointer ston komvo tou teleutaiou grammatos tis leksis
			else
			{
				if((temp2 -> child) == NULL) // an den exw paidi i posting list ston komvo tote den uparxei i leksi
				{	return NULL; }  //epistrefw null afou i leksi de vrethike
				else
				{	temp2 = temp2->child;  } //sunexizw tin anazitisi sto paidi - (exw !eow)
			}
		}
		else //an de vrw gramma meta apo to while loop kai teleiwsan oi komvoi tou trie, de tha uparxei i leksi
		{	return NULL; }
	}
	
	if (found)  	//an vrethike i leksi epestrepse pointer ston komvo
	{	return temp2; }
			
	return NULL;  	//alliws null

}


//create domi trie - leksiko
void create_trie(char *docmap[], struct domimap *docinfo[], int totalfiles)//, char* map[], int lines, int* wordcount, int* maxlength, int* words)
{

	trie = malloc (sizeof(struct Trie_t));  //arxikos komvos trie domis
	
	trie->child = NULL;
	
	FILE *doc;
	int i,y, length;

	char *word, *templine = NULL, *ptemp1; 
	struct Trie_t  *temp1, *temp2;
	char delimit[] = " \t\n";
	int eow = 0, firstime = 0;  //eow - flag gia na kratame an einai telos leksis - firstime flag/metritis gia tous komvous tou trie - elegxos
	char ch;
	
	ssize_t linelen;	//line length - getline
	size_t len = 0 ;	//buffer length - initialisation se 0 wste na kanei moni tis realloc(leitourgia getline me buffersize = 0)
	
	struct lines *t1lines;
	for(int thesidoc = 0; thesidoc <totalfiles; thesidoc++)
	{	
		int totgrammes = 0;
		int totwords = 0;	
		int totchars = 0;
		if ((doc = fopen(docmap[thesidoc], "r")) == NULL)
		{  	perror("\nfopen source-file"); printf("\n");  }

		while(!feof(doc) ) //mexri na teleiwsei to arxeio
		{			

			linelen = getline(&templine, &len, doc);
			
			ptemp1 = templine;  //kratw pointer prin peiraksw ton temp1 gia free argotera

			int tempsize = strlen(templine);
			char *temp = (char*) malloc ((tempsize+1)*sizeof(char));	
			strcpy(temp , templine);  
			char *ptemp = temp; 	//krataw ton pointer gia free argotera
			struct lines *linesptr = malloc(sizeof(struct lines)+tempsize+1);
			
			//edw dimiourgw ti domi pou krataei oles tis grammes tou totgrammes- arxeiou alla den katafera na to teleiopoihsw
			if(totgrammes == 0)
			{
				linesptr -> numgrammis = totgrammes;
				strcpy(linesptr->grammi, temp);
				docinfo[thesidoc] -> grammesarxeiou = linesptr;
				t1lines = (docinfo[thesidoc] -> grammesarxeiou);
				
			}
			else
			{
				struct lines *newline =  malloc(sizeof(struct lines)+tempsize+1);
				newline -> numgrammis = totgrammes;
				newline -> next = NULL;
				//newline->grammi =temp;  //copy to document i stin temp
				strcpy(newline ->grammi , temp);

				t1lines -> next = newline;
				t1lines = newline;

			}


			while ((word = strtok_r(temp, delimit, &temp))) //gia kathe grammi kanei gia kathe leksi insert ta grammata kai posting list
			{	

				totwords++;

				length = strlen (word);
				totchars += length;

				for (y=0; y < length; y++)  				//gia kathe gramma tis kathe leksis
				{

					firstime++;      						//flag gia prwto komvo m
					ch = *(word+y);
					if (y==length-1)
					{ 	eow = 1;  }

					if(firstime ==1)   //an einai o prwtos komvos genika pairnw ksexwristi periptwsi epeidi exw ton Info ---???? mipws na to entaxw sto ypoloipo?!
					{
						struct Trie_t* new = malloc (sizeof(struct Trie_t));	
						new->gramma = ch; 
						new->next = NULL;
						new->child = NULL;
						new->wordend = eow;
						new->df = 0;
						new->postlist = NULL;

						if(eow)  //an exw telos leksis, elegxw posting list - leksi me ena gramma px "a" -> arthro
						{

							posting_list(new, thesidoc, totgrammes);
							eow = 0;
						}

						trie -> child = new; 
						temp2 = trie->child;
					}
					else
					{	
						if((temp2 -> child) == NULL) //den exw paidi kai exw gramma pou thelw na eisagw sti leksi san child - new child
						{
							//printf("Create - eisagwgi %c\n", ch );
							struct Trie_t* new = malloc (sizeof(struct Trie_t));	
							new->gramma = ch; 
							new->next = NULL;
							new->child = NULL;
							new->wordend = eow;
							new->df = 0;
							new->postlist = NULL;

							if(eow)  //an exw telos leksis, elegxw posting list - leksi me ena gramma px "a" -> arthro
							{ 	posting_list(new, thesidoc, totgrammes); eow = 0; }

							temp2 -> child = new;
							temp2 = new;  //temp2 = temp2 -> child
						}
						else
						{	
							temp2 = temp2->child;
							//psaksimo orizontia stous neighbours idiou epipedou
							while (((temp1 = temp2->next) != NULL) && ( ch != (temp2->gramma) ) )   //oso den eimaste oute ston teleutaio komvo kai den exoume vrei to xaraktira
							{ 	temp2 = temp1; } 

							if(ch == (temp2->gramma) )  	//an uparxei to gramma tote proxwraw sto epomeno child kai kanw to idio
							{	
								if(eow ==1)  //an exw telos leksis, elegxw posting list alliws apla sunexizw na prosthetw grammata sto leksiko
								{
									posting_list(temp2, thesidoc, totgrammes);
									eow = 0;
								}
							}
							else	//an den uparxei to gramma "ch" tote to eisagw sto telos se neo komvo
							{

								struct Trie_t* new = malloc (sizeof(struct Trie_t));	
								new->gramma = ch; 
								new->next = NULL;
								new->child = NULL;
								new->wordend = eow;
								new->df = 0;
								new->postlist = NULL;

								if(eow)  //an exw telos leksis, elegxw posting list
								{
									posting_list(new, thesidoc, totgrammes);  //epistrefei 
									eow = 0;
								}
								temp2 ->next = new;			//an exw ftasei sto telos tou epipedou kai den exw vrei to gramma, to prosthetw
								temp2 = new;				//temp2 = temp2 ->next
							}	
						}	
					}
				}

				temp2 = trie; 		//kathe fora tha arxizw na elegxw apo to prwto gramma poy eisigaga
				eow = 0;      		//arxizw nea leksi
			

			}
			free(ptemp);
		
			totgrammes++;
		}


		docinfo[thesidoc]->lines = totgrammes;
		docinfo[thesidoc]->words = totwords;
		docinfo[thesidoc]->characters = totchars;
		
		fclose(doc);

	}

	

}


//max_tf function
int max_tf(char *leksi, int *id)
{	
	struct Plist *temp1, *temp2;
	int docfreq = 0 ;
	int maxfreq;


	struct Trie_t *tfkomvos = search_trie(leksi);
	if (tfkomvos == NULL)  //an den exw vrei ti leksi genika sto trie- leksiko
	{ 	
		return 0;
	}
	else
	{	
		temp2 = tfkomvos->postlist;
		
		maxfreq = temp2->freq;
		(*id) = temp2->id;
		temp2  = temp2->next;

		while ( temp2 != NULL   )  
		{
			if((temp2->freq)>maxfreq)
			{
				maxfreq = temp2->freq;
				(*id) = temp2->id;
				
			}
			
			temp2  = temp2->next;

		}

		return maxfreq;

	}

}

int min_tf(char *leksi, int *id)
{	
	struct Plist *temp1, *temp2;
	int minfreq;


	struct Trie_t *tfkomvos = search_trie(leksi);
	if (tfkomvos == NULL)  //an den exw vrei ti leksi genika sto trie- leksiko
	{ 	
		return 0; 
	}
	else
	{	
		temp2 = tfkomvos->postlist;
		
		minfreq = temp2->freq;
		(*id) = temp2->id;
		temp2  = temp2->next;

		while ( temp2 != NULL   )  
		{

			if((temp2->freq)<minfreq)
			{
				minfreq = temp2->freq;
				(*id) = temp2->id;			
			}		

			temp2  = temp2->next;
		}

		return minfreq;

	}

}


//destroy - free trie function
void trie_destr(struct Trie_t* trie)
{
	if((trie->child) !=NULL)  
	{	trie_destr(trie->child); } 	//ftanw sto teleutaio child tou trie
		  
	if((trie->next)!=NULL)			//ftanw sto teleutaio geitona tou teleutaiou child
	{	trie_destr(trie->next); }

	if ((trie-> postlist)!=NULL)  //free to posting list tis kathe leksis 
	{
		struct Plist *p1, *p2;
		p1 = trie -> postlist;
		while ((p2 = p1->next)!=NULL)  
		{

			p1->next = NULL;
			free(p1);
			p1 = p2;

		}
		free(p1);
		trie -> postlist = NULL;
	}

	trie-> child = NULL;	
	trie-> next = NULL;	
	free(trie);				//free ton komvo kai epistrefei 
}

