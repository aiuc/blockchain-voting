#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <math.h>
#include <assert.h>
#include "quatrieme.h"
#include "premier.h"
#include <openssl/sha.h>

void save_block(Block* b, char* filename) {
    FILE* f = fopen(filename,"w");
    if (f == NULL) {
        printf("erreur : ouverture du fichier\n");
        return;
    }

    //writing the file's header :
    char* author_name = key_to_str(b->author);
    fprintf(f,"%s\n",author_name);
    fprintf(f,"%s\n",b->hash);
    fprintf(f,"%s\n",b->previous_hash);
    fprintf(f,"%d\n",b->nonce); 
    free(author_name);

    //writing all the votes :
    CellProtected* l_votes = b->votes;
    while(l_votes) {
        char* vote = protected_to_str(l_votes->data);
        fprintf(f,"%s\n",vote);
        free(vote);
        l_votes = l_votes->next;
    }
    fclose(f);
}

//malloc used ! ! ! ! ! ! ! ! ! ! ! ! !
Block* load_block(char* filename) {
    FILE* f = fopen(filename,"r");
    if (f == NULL) {
        printf("%s\n",filename);
        printf("load_block - erreur : ouverture du fichier\n");
        return NULL;
    }

    //loading Block's main attributes
    Block* b = (Block*) malloc(sizeof(Block));

    char buff[256];
    char cAut[256];

    unsigned char* hash = (unsigned char*)malloc(256*sizeof(char));
    unsigned char* pHash = (unsigned char*)malloc(256*sizeof(char));
    int nonce;

    //Lecture auteur
    fgets(buff,256,f);
    assert(sscanf(buff, "%s", cAut)==1);
    b->author= str_to_key(cAut);

    //Lecture hash
    fgets(buff,256,f);
    assert(sscanf(buff, "%s", hash)==1);
    b->hash = hash;

    //Lecture previous_hash
    fgets(buff,256,f);
    assert(sscanf(buff, "%s", pHash)==1);
    b->previous_hash = pHash;

    //Lecture nonce
    fgets(buff,256,f);
    assert(sscanf(buff, "%d", &nonce)==1);
    b->nonce = nonce;

    //loading all the votes in Block
    CellProtected** votes = (CellProtected**) malloc(sizeof(CellProtected*));
    *votes = NULL;
    char ligne[256];
    while( fgets(ligne, 256, f) ) {
        Protected* pr = str_to_protected(ligne);
        CellProtected* cp = create_cell_protected(pr);
        insert_cell_protected(votes,cp);
    }
    b->votes = *votes;
    free(votes);

    fclose(f);
    return b;
}

//malloc used ! ! ! ! ! ! ! ! ! ! ! ! !
char* block_to_str(Block* block) {
    if (block == NULL) return NULL;
    
    char* cp1 = (char*) malloc(100000*sizeof(char));

    //obtaining block->votes as a string
    char* str = (char*) malloc(100000*sizeof(char));
    CellProtected* cp = block->votes;
    int i = 0;
    while(cp) {
        char* vote = protected_to_str(cp->data);
        for(int j=0; j<strlen(vote); j++) {
            str[i] = vote[j];
            i++;
        }
        str[i] = '\n';
        i++;
        free(vote);
        cp = cp->next;
    }
    //assembling the cp1ult
    char* author = key_to_str(block->author); 
    sprintf(cp1,"%s\n%s\n%s\n%d",author,(char*) block->previous_hash,str,block->nonce);
    free(author); 
    free(str);
    return cp1;
}


unsigned char* hash__sha256(char* s) {
    return SHA256(s,strlen(s),0);
}

int enough_zeros(unsigned char* s, int d) {
    int i;
    for(i=0; i<d; i++) {
        if (s[i] != 0) {
            return 0 ;
        }
    }
    return 1;
}

void compute_proof_of_work(Block* b, int d) {
    b->nonce = 0;
    char* str = block_to_str(b);
    b->hash = hash__sha256(str);
    free(str);
    int i;
    while( !enough_zeros(b->hash,d) ) {
        b->nonce++;
        str = block_to_str(b);
        b->hash = hash__sha256(str);
        free(str);
    }
}

int verify_block(Block* b, int d) {
  char* str = block_to_str(b);
  unsigned char* hash = hash__sha256(b);
  free(str);
  return enough_zeros(hash,d);
}

void delete_block(Block* b) {
    if (b==NULL) return;
    free(b->previous_hash);
    free(b->hash);
    /*
    CellProtected* votes = b->votes;
    while(votes) {
        CellProtected* tmp = votes;
        votes = votes->next;
        free(tmp);
    }
    */
    liberer_key(b->author);
    delete_liste_chainee(b->votes);
    free(b);
}

//malloc used ! ! ! ! ! ! ! ! ! ! ! ! !
CellTree* create_node(Block* b) {
    CellTree* ct = (CellTree*) malloc(sizeof(CellTree));
    ct->block = b; 
    ct->father = NULL;
    ct->firstChild = NULL;
    ct->nextBro = NULL;
    ct->height = 0;
    return ct;
}

int update_height(CellTree* father, CellTree* child) {
    if(father->height >= (child->height +1)) {
        return 0;
    }
    father->height = child->height + 1;
    return 1;
}

void add_child(CellTree* father, CellTree* child) {
    CellTree* new_child = father->firstChild;

    //connecting father and child 
    if(new_child == NULL) {
        father->firstChild = child;
    } else {
        while(new_child->nextBro) {
            new_child = new_child->nextBro;
        }
        new_child->nextBro = child;
    }
    child->father = father;

    //updating all ascending heights 
    CellTree* tmp = child->father;
    new_child = child;
    int i = 0;
    while(tmp && i<50) {
        printf("%d \n",i);
        i++;
        update_height(tmp,new_child);
        new_child = tmp;
        tmp = tmp->father;
    }
}

void print_tree(CellTree* ct) {
    if (ct==NULL) {
        return;
    }
    //printing current node's informations
    if(!ct->father) {
        printf("- Node:[ %s , %d ]\n",ct->block->hash, ct->height);
    } else {
        printf("- Node:[ %s , %d ]",ct->block->hash, ct->height);
        printf(", father : [ %s , %d ]\n",ct->father->block->hash,ct->father->height);
    }
    //appel recursive
    print_tree(ct->nextBro);
    print_tree(ct->firstChild);
}

void delete_node(CellTree* node) {
    if(node == NULL) {
        return;
    }
    delete_block(node->block);
    free(node);
}

void delete_node_tree(CellTree* tree) {
    if(tree == NULL) {
        return;
    }
    delete_node_tree(tree->nextBro);
    delete_node_tree(tree->firstChild);
    delete_node(tree);
}

CellTree* highest_child(CellTree* cell) {
    CellTree* fav_child = cell->firstChild;
    CellTree* child = cell->firstChild;
    while(child) {
        
        if( child->height > fav_child->height ) {
            fav_child  = child;
        }
        child = child->nextBro;
    }
    return fav_child;
}

CellTree* last_node(CellTree* tree) {
    CellTree* last_node = tree;
    while(last_node->firstChild) {
        last_node = highest_child(last_node);
    }
    return last_node;
}

//malloc used ! ! ! ! ! ! ! ! ! ! ! ! !
Protected* copy_protected(Protected* p) {
    Protected* res = (Protected*) malloc(sizeof(Protected));
    res->pKey = (Key*) malloc(sizeof(Key));
    init_key(res->pKey,p->pKey->val,p->pKey->n);
    res->s = init_signature(p->s->tab,p->s->taille);
    res->mess = strdup(p->mess);
    return res;
}

//malloc used ! ! ! ! ! ! ! ! ! ! ! ! !
CellProtected* copy_list(CellProtected* list) {
    CellProtected* res = NULL;
    while(list) {
        Protected* pr = copy_protected(list->data);
        CellProtected* tmp = create_cell_protected(pr);
        insert_cell_protected(&res,tmp);

        list = list->next;
    }
    return res;
}

//malloc used ! ! ! ! ! ! ! ! ! ! ! ! !
CellProtected* fuse_cell_protected(CellProtected* cp1, CellProtected* cp2) {
    if(cp1 == NULL) {
        return copy_list(cp2);
    } else if (cp2 == NULL) {
        return copy_list(cp1);
    }
    CellProtected* res = copy_list(cp1);
    CellProtected* tmp = res;
    while(tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = copy_list(cp2);

    return res;
}

CellProtected* get_full_list(CellTree* blockchain) { 
    CellTree* tmp = blockchain;
    CellProtected* res = NULL;
    CellProtected* trashcan = NULL;

    while(tmp->height) {
        trashcan = res;
        res = fuse_cell_protected(res,tmp->block->votes);
        tmp = highest_child(tmp);
        delete_liste_chainee(trashcan);
    }
    return res;
} 

void submit_votes(Protected* p) {
    FILE* f = fopen("Pending_votes.txt","a");
    if (f == NULL) {
        printf("erreur : ouverture du fichier\n");
        return;
    }
    char* str = protected_to_str(p);
    fprintf(f,"%s\n",str);
    free(str);
    fclose(f);
}

void create_block(CellTree* tree, Key* author, int d) {
    CellTree* lastnode = last_node(tree);

    //creating a new block
    Block* b = (Block*) malloc(sizeof(Block));
    b->author = author;
    compute_proof_of_work(b,d);
    int size = strlen((char*)lastnode->block->hash);
    b->previous_hash = (unsigned char*) malloc(size*sizeof(unsigned char));
    for(int i=0; i<size; i++) {
        b->previous_hash[i] = (int) lastnode->block->hash[i];
    }
    b->votes = read_protected("Pending_votes.txt");

    //connecting the newest block to the blockchain
    CellTree* new_node = create_node(b);
    add_child(lastnode,new_node);
    
    remove("Pending_votes.txt");
    save_block(b,"Pending_block");
}

void add_block(int d, char* name) {
    Block* b = load_block("Pending_block");
    //printf("hash add_block : [ %s ] \n",b->hash);
    if( verify_block(b,d) ) {
        char rep[256] = "Blockchain/";
        strcat(rep,name);
        save_block(b,rep);
    } else {
        printf(" error : invalid block detected !\n");
    }
    delete_block(b);
    remove("Pending_block");
}

CellTree* read_tree() {
    DIR* rep = opendir("./Blockchain/");
    CellTree** tab = (CellTree**) malloc(100*sizeof(CellTree*));
    int ind = 0;
    int i,j;
    if(rep != NULL) {
        struct dirent* dir;
        while( (dir = readdir(rep)) ) {
            if( strcmp(dir->d_name,".")!=0 && strcmp(dir->d_name,"..")!=0 ) {
                char name[256] = "Blockchain/";
                strcat(name, dir->d_name);
                tab[ind] = create_node( load_block(name) );
                ind++;
            }
        }
        closedir(rep);
    }
    for(i = 0 ; i<ind ; i++) {
        unsigned char* str1 = tab[i]->block->hash;
        for(j = 0 ; j<ind ; j++) {
            unsigned char* str2 = tab[j]->block->previous_hash;
            if( strcmp((char*)str1,(char*)str2) == 0 ) {
                add_child(tab[i],tab[j]);
            }
        }
    }
    CellTree* racine = NULL;
    for(i = 0 ; i<ind ; i++) {
        if( tab[i]->father == NULL ) {
            racine = tab[i];
            break;
        }
    }
    return racine;
}

Key* compute_winner_BT(CellTree* tree, CellKey* candidates, CellKey* voters, int sizeC, int sizeV) {
    CellProtected* list_votes = get_full_list(tree);
    suppr_non_valide(list_votes);
    Key* winner = compute_winner(list_votes, candidates, voters, sizeC, sizeV);
    return winner;
}
