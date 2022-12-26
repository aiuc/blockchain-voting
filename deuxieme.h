#include "premier.h"


typedef struct _key{
    long val;
    long n;
} Key;

typedef struct _signature {
    int taille;
    long* tab;
} Signature;

typedef struct _protected {
    Key* pKey;
    char* mess;
    Signature* s;
} Protected;

void init_key(Key* key, long val, long n);
void init_pair_keys(Key* pKey, Key* sKey, long low_size, long up_size);
char* key_to_str(Key* key);
Key* str_to_key(char* str);
void liberer_key(Key* k);

Signature* init_signature(long* content, int size);
Signature* sign(char* mess, Key* sKey);
char* signature_to_str(Signature* sgn);
Signature* str_to_signature(char* str);
void liberer_signature(Signature* s);

Protected* init_protected(Key* pKey, char* mess, Signature* sgn);
int verify(Protected* pr);
char* protected_to_str(Protected* pr) ;
Protected* str_to_protected(char* str);
void liberer_protected(Protected* pr) ;
int in_storage(Key** storage, int size, long val, long n);

void generate_random_data(int nv, int nc) ;
