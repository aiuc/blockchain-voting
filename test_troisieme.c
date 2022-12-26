#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "troisieme.h"
#include "premier.h"

int main() {
    
    //testing first

    //creating a list of votes
    CellProtected* cp = read_protected("declarations.txt");

    printf("read_protected : tested\n");
    print_list_protected(cp); 
    
    printf("\n");

    //creating one (non-valid) vote and inserting it into the list
    Key* ktest = (Key*) malloc(sizeof(Key));
    ktest->val = 69;
    ktest->n = 42;
    Signature* ss = sign("first", ktest);
    Protected* prott = init_protected(ktest, "second", ss);
    CellProtected* cpp = create_cell_protected(prott);
    insert_cell_protected(&cp, cpp);

    puts("insertion de non valide \n");
    print_list_protected(cp);

    puts("\nsuppression effectuee \n");
    suppr_non_valide(&cp);
    print_list_protected(cp);
    
    //hashtable_functions

    Key* ktest2 = (Key*) malloc(sizeof(Key));
    ktest2->val = 69;
    ktest2->n = 42;
 
    Key* ktest4 = (Key*) malloc(sizeof(Key));
    ktest4->val = 69;
    ktest4->n = 42;

    printf("test d egalite: %d \n",equal_key(ktest2, ktest4));
    puts("si 1 donc valide. \n");

    HashCell * hash2 = create_hashcell(ktest2);
    HashCell * hash4 = create_hashcell(ktest4);

    printf("Hash function : %d \n",hash_function(ktest2, 8));

    
    CellKey* keys = create_cell_key(ktest2);
    free(keys);
    free(ktest2);
    
    keys = read_public_key("keys.txt");
    print_list_keys(keys);

    CellKey* tmp = keys;
    printf("testing hash function\n");
    while(tmp) {
        printf("%d \n",hash_function(tmp->data,5000));
        tmp = tmp->next;
    } 

    HashTable* Htable = create_hashtable(keys, 5000);
    CellKey* candidates_ = read_public_key("candidates.txt");

    //Key* winner = compute_winner(cp, candidates_, keys, 5, 50);

    //free
    free(ktest4);
    free(hash2);
    free(hash4);

    tmp = keys;
    while(tmp) {
        tmp = keys->next;
        free(keys);
        keys = tmp;
    }
    
    delete_hashtable(Htable);
    delete_liste_chainee(cp);
  
    return 0;
}