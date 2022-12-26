#include "deuxieme.h"

#ifndef EX5
#define EX5

typedef struct cellKey {
	Key* data ;
	struct cellKey * next ;
} CellKey;

typedef struct cellProtected{
    Protected* data;
    struct cellProtected* next;
}CellProtected;

typedef struct hashcell {
    Key* key;
    int val;
} HashCell;

typedef struct hashtable {
    HashCell** tab;
    int size;
} HashTable;

// liste chainee : cles

void mem_reset(char *buff, size_t n);
CellKey* create_cell_key(Key* key);
void insert_cell_key(CellKey** c, Key* key);
CellKey* read_public_key(char* nomfic);
void print_list_keys(CellKey* c);
void delete_cell_key(CellKey* c);
void delete_list_ck(CellKey* c);

// liste chainee : declarations sigees

CellProtected* create_cell_protected(Protected* pr);
void insert_cell_protected(CellProtected** cp, CellProtected* pr);
CellProtected* read_protected(char* filename);
void print_list_protected(CellProtected* cp);
void delete_cell_protected(CellProtected* cp);
void delete_liste_chainee(CellProtected* cp);
 
//trouver gagnant de l election

void suppr_non_valide(CellProtected** cp);

//fonctions hashtable
HashCell* create_hashcell(Key* key);
int equal_key(Key * key1, Key * key2);
int hash_function(Key* key, int size);
int find_position(HashTable * t,Key * key);
HashTable* create_hashtable(CellKey* keys, int size);
void delete_hashtable(HashTable* t);
Key *compute_winner(CellProtected *decl, CellKey *candidates, CellKey *voters, int sizeC, int sizeV);
#endif