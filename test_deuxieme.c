#include "deuxieme.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>

int main()
{
    srand(time(NULL));
    //Tester Init Keys
    printf("\n{ Testing init keys }\n");
    Key* pKey = malloc(sizeof(Key));
    Key* sKey = malloc(sizeof(Key)); 
    init_pair_keys(pKey, sKey,3,7);
    printf("    . Electeur - pKey: %lx , %lx \n", pKey->val, pKey->n); 
    printf("    . Electeur - sKey: %lx , %lx \n", sKey->val, sKey->n);

    //Tester Key Serialization
    printf("\n{ Testing Key Serialization }\n");
    
    char* k2s = key_to_str(pKey);
    printf("    . key_to_str (pKey) : %s \n", k2s);
    free(k2s);

    Key* s2k = str_to_key(k2s);
    printf("    . str_to_key (pKey) : (%lx,%lx) \n", s2k->val, s2k->n);
    free(s2k);

    //Tester signature
    printf("\n{ Testing signature }\n");
    
    //  Candidate keys:
    Key* pKeyC = malloc(sizeof(Key));
    Key* sKeyC = malloc(sizeof(Key)); 
    init_pair_keys(pKeyC, sKeyC,3,7);
    
    //  Declaration:
    char* mess = key_to_str(pKeyC);
    
    char* elec = key_to_str(pKey);
    printf("    . %s vote pour %s\n\n",elec, mess); 
    free(elec); free(mess); 
   
    Signature* sgn = sign(mess, sKey); 
    printf("    . signature :"); print_long_vector(sgn->tab,sgn->taille);

    char* sgn2s = signature_to_str(sgn);
    printf("    . signature to str : %s \n\n", sgn2s);
    free(sgn2s);

    Signature* s2sgn = str_to_signature(sgn2s);
    printf("    . str to signature :"); print_long_vector(sgn->tab,sgn->taille);
    liberer_signature(s2sgn);

    //Tester protected
     printf("\n{ Testing protected }\n");

    Protected* pr = init_protected(pKey,mess, sgn);
        
    //Verification:
    if (verify(pr)) { 
        printf("    . Signature valide\n");
    } else {
        printf("    . Signature non valide\n");
    }
    char* p2s = protected_to_str(pr);
    printf("    . protected to str : %s\n", p2s);
    free(p2s);

    Protected* s2p = str_to_protected(p2s);

    char* str_key = key_to_str(s2p->pKey);
    char* str_sgn = signature_to_str(s2p->s);
    printf("    . str to protected : %s %s %s\n",str_key,s2p->mess,str_sgn);
    free(str_key); free(str_sgn);
    liberer_protected(s2p);
    free(sKey);
    free(pKeyC); free(sKeyC);
    liberer_protected(pr);

    generate_random_data(50,5);

    return 0;
}