#ifndef FUNS_H_  
#define FUNS_H_

struct domimap;

int countdocs(char *path);
int mapping(char *filename, char *map[], int countarxeia);
struct Trie_t* search_trie(char* word);
void tf( int idnum, char *leksi);
int create_docinfo(char *pathmap[], char  *docmap[], struct domimap *docinfo[], int pathnum, int countarxeia) ; //pairnei ena ena paths kai eisagei sto map


#endif // FUNS_H_