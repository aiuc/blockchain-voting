#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include "deuxieme.h"


void init_key(Key* key, long val, long n) 
{
    key->val = val;
    key->n=n;
}

void init_pair_keys(Key* pKey, Key* sKey, long low_size, long up_size) 
{
    //finding 2 different primes p & q
    long p = random_prime_number(low_size,up_size,5000);
    long q = random_prime_number(low_size,up_size,5000);
    while (p==q) q = random_prime_number(low_size,up_size,5000);
    long n,s,u;

    generate_key_values(p,q,&n,&s,&u);

    //ensuring s*u mod t = 1 
    if (u<0){
        long t = (p-1)*(q-1);
        u = u+t; 
    } 
    init_key(pKey,s,n);
    init_key(sKey,u,n);
}

//malloc used ! ! ! ! ! ! ! ! ! ! ! ! !
char* key_to_str(Key* key) 
{
    char* hex = (char*) malloc(20*sizeof(char));
    sprintf(hex,"(%lx,%lx)",key->val,key->n);
    return hex;
}

//malloc used ! ! ! ! ! ! ! ! x! ! ! ! !
Key* str_to_key(char* str) 
{
    Key* key = (Key*) malloc(sizeof(Key));
    long val, n;
    sscanf(str,"(%lx,%lx)",&val,&n);
    init_key(key,val,n);
    return key;
}

void liberer_key(Key* k) 
{
    if(k != NULL) {
        free(k);
    }
}

//malloc used ! ! ! ! ! ! ! ! ! ! ! ! !
Signature* init_signature(long* content, int size) 
{
    Signature* s = (Signature*) malloc(sizeof(Signature));
    s->tab = (long*) malloc(size*sizeof(long));
    int i;
    for(i=0; i<size; i++) {
        s->tab[i] = content[i];
    }
    s->taille = size;
    return s;
}

//malloc used ! ! ! ! ! ! ! ! ! ! ! ! !
Signature* sign(char* mess, Key* sKey) 
{
    long* encrypted = encrypt(mess,sKey->val,sKey->n);
    Signature* s = init_signature(encrypted,strlen(mess));
    free(encrypted);
    return s;
}

//malloc used ! ! ! ! ! ! ! ! ! ! ! ! !
char* signature_to_str(Signature* sgn)
{
    char* result = malloc(10*sgn->taille*sizeof(char)); 
    result[0]= '#';
    int pos = 1; 
    char buffer [156];
    for (int i=0; i<sgn->taille; i++){ 
        sprintf(buffer, "%lx", sgn->tab[i]); 
        for (int j=0; j< strlen(buffer); j++){
            result[pos] = buffer[j];
            pos = pos +1; 
        }
        result[pos] = '#';
        pos = pos +1; 
    }
    result[pos] = '\0';
    result = realloc(result, (pos+1)*sizeof(char)); 
    return result;
}

//malloc used ! ! ! ! ! ! ! ! ! ! ! ! !
Signature* str_to_signature(char* str)
{
    int len = strlen(str);
    long* content = (long*)malloc(sizeof(long)*len); 
    int num = 0;
    char buffer [256];
    int pos = 0;
    for (int i=0; i<len; i++){
        if (str[i] != '#'){ 
            buffer[pos] = str[i]; 
            pos=pos+1;
        }else{
            if (pos != 0){
                buffer[pos] = '\0';
                sscanf(buffer, "%lx", &(content[num])); 
                num = num + 1;
                pos = 0;
            } 
        }
    }
    content = realloc(content ,num*sizeof(long)); 
    Signature* res = init_signature(content , num);
    free(content);
    return res;
}

void liberer_signature(Signature* s) 
{
    if(s->tab != NULL) free(s->tab);
    if(s != NULL) free(s);
}

//malloc used ! ! ! ! ! ! ! ! ! ! ! ! !
Protected* init_protected(Key* pKey, char* mess, Signature* sgn) 
{
    Protected* pr = (Protected*) malloc(sizeof(Protected));
    pr->mess = strdup(mess);
    pr->s = sgn;
    pr->pKey = pKey;
    return pr;
}

int verify(Protected* pr) 
{
    char* decrypted = decrypt(pr->s->tab,pr->s->taille,pr->pKey->val,pr->pKey->n);
    int is_verified = ! strcmp(decrypted,pr->mess);
    printf("verify : %d \n",is_verified);
    free(decrypted);
    return is_verified;
}

//malloc used ! ! ! ! ! ! ! ! ! ! ! ! !
char* protected_to_str(Protected* pr) 
{
    char* str = (char*) malloc(1000*sizeof(char));
    char* pKey = key_to_str(pr->pKey);
    char* sgn = signature_to_str(pr->s);
    sprintf(str,"%s %s %s",pKey,pr->mess,sgn);
    free(pKey); 
    free(sgn);
    return str;    
}

//malloc used ! ! ! ! ! ! ! ! ! ! ! ! !
Protected* str_to_protected(char* str) 
{
    char k[256]; char m[256]; char s[256];
    sscanf(str,"%s %s %s",k,m,s); // nice
    Protected* pr = init_protected(str_to_key(k),m,str_to_signature(s));
    return pr;
}

void liberer_protected(Protected* pr) 
{
    if (pr->mess != NULL) free(pr->pKey);
    if (pr->mess != NULL) free(pr->mess);
    if (pr->s != NULL) liberer_signature(pr->s);
    if (pr != NULL) free(pr);
}

int in_storage(Key** storage, int size, long val, long n) 
{
    //check if a certain key belongs in a Key Array
    int i;
    for(i=0; i<size; i++) {
        if(storage[i]->val == val) {
            if(storage[i]->n == n) {
                    return 1;
                }
            }
        }
    return 0;
}

void generate_random_data(int nv, int nc) 
{
    FILE* fkeys = fopen("keys.txt","w");
    FILE* fcand = fopen("candidates.txt","w");
    FILE* fdec = fopen("declarations.txt","w");
    
    assert(fkeys);
    assert(fcand);
    assert(fdec);

    //allocation pour public/secret keys created
    Key** pk_storage = (Key**) malloc(nv*sizeof(Key*));
    Key** sk_storage = (Key**) malloc(nv*sizeof(Key*));

    int i,r;

    //creation de nv keys
    for(i=0; i<nv; i++) {
        Key* pk = (Key*) malloc(sizeof(Key));
        Key* sk = (Key*) malloc(sizeof(Key));
        init_pair_keys(pk,sk,3,7);
        if(in_storage(sk_storage,i,sk->val,sk->n)) init_pair_keys(pk,sk,3,7);
        pk_storage[i] = pk;
        sk_storage[i] = sk;
        fprintf(fkeys,"(%lx,%lx) (%lx,%lx)\n",pk->val,pk->n,sk->val,sk->n);
    }
    fclose(fkeys);

    //allocation pour candidate keys 
    Key** cand_storage = (Key**) malloc(nc*sizeof(Key*));
    
    //creation nc candidates
    for(i=0; i<nc; i++) {
        r = rand()%nv;
        while( in_storage(cand_storage,i,pk_storage[r]->val,pk_storage[r]->n) ) r = rand()%nv;
        cand_storage[i] = pk_storage[r];
        Key* pc = pk_storage[r];
        Key* sc = sk_storage[r];
        fprintf(fcand,"(%lx,%lx) (%lx,%lx)\n",pc->val,pc->n,sc->val,sc->n);
    }
    fclose(fcand);

    Protected** pr_storage = (Protected**) malloc(nv*sizeof(Protected*));

    //creation nv declarations 
    for(i=0; i<nv; i++) {
        r = rand()%nc;
        char* mess = key_to_str(cand_storage[r]);
        Signature* s = sign(mess,sk_storage[i]);
        pr_storage[i] = init_protected(pk_storage[i],mess,s);

        char* pr_str = protected_to_str(pr_storage[i]);
        fprintf(fdec,"%s \n",pr_str);
        free(pr_str);
        free(mess);
    }
    fclose(fdec);

    //free pour memoire
    for(i=0; i<nv; i++) {
        liberer_key(sk_storage[i]);
        liberer_protected(pr_storage[i]);
    }
    free(pk_storage); free(sk_storage); free(cand_storage); free(pr_storage);

}
