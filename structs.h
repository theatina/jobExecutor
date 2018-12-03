#ifndef STRUCTS_H_  
#define STRUCTS_H_


struct lista  //lista me tis grammes stis opoies emfanizetai i leksi
{
	int gramminum;
	struct lista *next;
	//struct lines *grammesarxeiou;
};

struct Plist 
{
	int id;					//id arxeiou(path) sto opoio periexetai h leksi
	int freq; 				//emfaniseis leksis sto doc me id
	//int sumlines;			//plithos grammwn stis opoies emfanizetai
	struct lista *grammes;  //lista me grammes stis opoies emfanizetai h leksi
	struct Plist* next;  	//next arxeio sto opoio emfanizetai h leksi
};

struct Trie_t 
{
	char gramma;					//the letter
	struct Trie_t* next;			// next letter horizontally - same level (under the same letter);	
	struct Trie_t* child;			// child letter
	int wordend;      				//0 - if not end of word, 1 - if end of word - flag gia end of word
	int df ;						// ean wordend = 1, tote krataw to document frequency, diladi se posa docs emfanizetai
	struct Plist* postlist; 		//the posting list - if the wordend variable is 1 !! - deixnei ston prwto komvo tou posting list			
};

struct lines  //lista me pointer se oles tis grammes tou keimenou
{	
	int numgrammis;  	//arithmos grammis gia kalutero elegxo (den einai aparaitito)
	struct lines *next;	//epomeni grammi
	char grammi[];		//grammi numgrammis tou keimenou
	
};

struct domimap
{	
	int id; 				//id tou path tou doc - thesi ston pinaka - peritti info
	int characters;  		//xaraktires arxeiou
	int words;				//plithos leksewn keimenou
	int lines;				//grammes keimenou
	struct lines *grammesarxeiou; //pointers stis grammes tou keimenou
};

#endif /* STRUCTS_H_ */