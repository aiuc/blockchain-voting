#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "troisieme.h"
#include "premier.h"

void mem_reset(char *buffer, size_t n) 
{ 
    for(size_t i = 0; i < n; i++)
        buffer[i] = '\0';
}
//reset memoire avec buffer

CellKey * create_cell_key(Key* key) 
{
    CellKey * ck = (CellKey*) malloc(sizeof(CellKey));
    ck->data = key;
    ck->next = NULL;
    return ck;
}

void insert_cell_key (CellKey** ck, Key* c) 
{
    CellKey* a = create_cell_key(c);
    if (*ck == NULL)
    {
        *ck = a;
        return;
    }
    a->next = *ck;
    *ck = a;
}

CellKey* read_public_key(char* nomfichier) 
{
    FILE* fd = fopen(nomfichier, "r");
    if (!fd){
        puts("Erreur d'ouverture du fichier");
        return NULL;
    }

    char ligne[256] = {0};
    char* buff = ligne;
    char public[256];
    char private[256];
    CellKey* ck = NULL;
    
    while (fgets(buff, 256, fd))
    {
        sscanf(buff, "%s %s", public, private);
        Key* pk = str_to_key(public);
        insert_cell_key(&ck, pk);
        mem_reset(buff, (size_t) 256);
    }

    fclose(fd);
    return ck;
}

void print_list_keys (CellKey* c) {
    CellKey* temp = c;
    char* data;
    while (temp)
    {
        data = key_to_str(temp->data);
        printf("%s\n", data);
        free(data);
        temp = temp->next;
    }
}

void delete_cell_key (CellKey* c) 
{
    free(c->data);
    c->next = NULL;
    free(c);
}

void delete_list_ck (CellKey* c) 
{
    CellKey* temp = NULL;
    while (c)
    {
        temp = c->next;
        delete_cell_key(c);
        c = temp;
    }
    free(c);
}

CellProtected* create_cell_protected (Protected* pr) 
{
    CellProtected* Cpr = (CellProtected*) malloc(sizeof(CellProtected));
    if (!Cpr)
    {
        puts("Erreur d'allocation");
        return NULL;
    }
    Cpr->data = pr;
    Cpr->next = NULL;
    return Cpr;
}

void insert_cell_protected (CellProtected** cp, CellProtected* pr) 
{
    if (*cp == NULL)
    {
        *cp = pr;
        return;
    }
    pr->next = *cp;
    *cp = pr;
}

CellProtected* read_protected(char* filename) 
{
    FILE* fd = fopen(filename, "r");
    if (!fd) {
        puts("Erreur d'ouverture du fichier.");
        return NULL;
    }

    char ligne[256] = {0};
    char* buff = ligne;
    size_t n = 256;
    CellProtected* cp = NULL;
    Protected* pr = NULL;
    CellProtected* temp = NULL;

    while (getline(&buff, &n, fd) > 0) {
        if(strcmp(buff, "\n") != 0) {
            pr = str_to_protected(buff);
            temp = create_cell_protected(pr);
            insert_cell_protected(&cp, temp);
        }
    }

    fclose(fd);
    return cp;
}

void print_list_protected (CellProtected* cp) 
{
    int nb = 0;
    CellProtected* temp = cp;
    char* data;
    while (temp)
    {
        data = protected_to_str(temp->data);
        printf("    - %s \n", data);
        free(data);
        temp = temp->next;
        nb++;
    }
    printf("Nbr de CellProtected : %d\n", nb);
}

void delete_cell_protected (CellProtected* cp) 
{
    if (cp == NULL) return;
    if(cp->data) liberer_protected(cp->data);
    cp->next = NULL;
    free(cp);
}

void delete_liste_chainee (CellProtected* cp) 
{
    if(cp == NULL) return;
    CellProtected* temp = NULL;
    while (cp) {
        temp = cp->next;
        delete_cell_protected(cp);
        cp = temp;
    }
    free(cp);
}

void suppr_non_valide(CellProtected** cp) 
{
    CellProtected* prec = *cp;
    
    if (!prec->data || !verify(prec->data))
    {
        *cp = (*cp)->next;
        delete_cell_protected(prec);
    }
    
    prec = *cp;
    CellProtected* temp = (*cp)->next;

    while (temp)
    {
        if (!temp->data || !verify(temp->data))
        {
            if (!temp->next)
            {
                delete_cell_protected(temp);
                prec->next = NULL;
            }
            else
            {
                prec->next = temp->next;
                delete_cell_protected(temp);
            }
        }
        prec = prec->next;
        temp = temp->next;
    }
}

HashCell* create_hashcell(Key* key)
{
    HashCell* hc = (HashCell*) malloc(sizeof(HashCell));
    hc->key = key;
    hc->val = 0;
    return hc;
}

int equal_key(Key * key1, Key * key2)
{
    return (key1->val==key2->val) && (key1->n==key2->n);
}

int hash_function(Key* key, int size) 
{
    //hachage par multiplication
    double gold_ratio = ( sqrt(5) - 1 ) / 2;
    int x = key->val;
    return (int) (size*( x*gold_ratio - (int) (x*gold_ratio) ));
}

int find_position(HashTable * t,Key * key)
{
    int pos=hash_function(key, t->size);
    for(int i=0; i<(t->size-pos);i++){
        if(t->tab[i+1]){
            if( equal_key(t->tab[pos+i]->key,key) ){
                return pos+i;
            }
        }
    }
    for(int i=0; i<pos;i++){
        if(t->tab[pos+i]){
            if(equal_key(t->tab[i]->key,key)){
                return i;
            }
        }
    }
    return pos;
}

HashTable* create_hashtable(CellKey* keys, int size)
{
    HashTable* hashtable = (HashTable*) malloc(sizeof(HashTable));
    hashtable->size = size;
    hashtable->tab= (HashCell**) malloc(size*sizeof(HashCell*));
    for(int i=0; i<size; i++){
        hashtable->tab[i] = NULL;
    }
    while(keys) {
        HashCell* hashcell = create_hashcell(keys->data);
        int pos = find_position(hashtable,keys->data);
        hashtable->tab[pos] = hashcell;
        keys=keys->next;
    }
    return hashtable;
}

void delete_hashtable(HashTable* t)
{
    if(!t)
    {
        printf("erreur pas de Hashtable : delete_hashtable");
        return;
    }
    for (int i = 0; i < t->size; i++)
    {
        if (t->tab[i])
        {
            free(t->tab[i]->key); // on free les cle cases par cases
            free(t->tab[i]);
        }
    }
    free(t->tab);
    free(t);
}

Key *compute_winner(CellProtected *decl, CellKey *candidates, CellKey *voters, int sizeC, int sizeV)
{
    HashTable *hc = create_hashtable(candidates, sizeC);
    HashTable *hv = create_hashtable(voters, sizeV);
    int majorite;
    if (sizeV % 2 == 0){
        majorite = sizeV / 2;
    }else{
        majorite = ceil(sizeV / 2);
    }
    CellProtected* temp = decl;
    Key *ktemp;
    int val;
    while (temp) {
        ktemp = temp->data->pKey;
        val = hv->tab[find_position(hv, ktemp)]->val;   
        val++;
        if (val > 1){
            continue; // joueur qui a deja voté
        }
        ktemp = str_to_key(temp->data->mess); 
        hc->tab[find_position(hc, ktemp)]->val++; 
        free(ktemp);
        temp = temp->next;
    }
    int index;
    int maxval = 0;
    Key* winner = calloc(1, sizeof(Key));//nouvelle cle qui va stocker la clé du gagnant
    for (int i = 0; i < sizeC; i++){
        if (hc->tab[i]->val > majorite){ // finir quand joueur obtient marjorite
            printf("Victoire de majorite pour %d : %d total de voteurs pour %d votes\n", i, sizeV,  hc->tab[i]->val);
            init_key(winner, hc->tab[i]->key->val, hc->tab[i]->key->n);
            delete_hashtable(hc);
            delete_hashtable(hv);
            return winner;
        }
        if (hc->tab[i]->val > maxval){
            maxval = hc->tab[i]->val;
            index = i;
        }
    }
    printf("%d a eu %d votes pour un total de %d \n", index, hc->tab[index]->val, sizeV);
    init_key(winner, hc->tab[index]->key->val, hc->tab[index]->key->n);
    delete_hashtable(hc);
    delete_hashtable(hv);
    return winner;
}




















