#ifndef TRIE_H_  
#define TRIE_H_

void create_trie(char *docmap[], struct domimap *docinfo[], int totalfiles);
void posting_list(struct Trie_t* komvos, int idnum, int grammi);
void trie_destr(struct Trie_t *trie);
int df_k( struct Trie_t *komvos);
int max_tf(char *leksi, int *id);
int min_tf(char *leksi, int *id);

#endif // TRIE_H_