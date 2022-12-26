#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "quatrieme.h"
#include "premier.h"


int main() {
    srand(time(NULL));
    
    //creation de block


    Block* test = (Block*) malloc(sizeof(Block));
    test->author = (Key*) malloc(sizeof(Key));
    init_key(test->author,127,643);
    
    test->hash = (unsigned char*) malloc(3*sizeof(unsigned char));
    test->hash[0] = 64; 
    test->hash[1] = 118; 
    test->hash[2] = '\0';

    test->previous_hash = (unsigned char*) malloc(3*sizeof(unsigned char));
    test->previous_hash[0] = 56; 
    test->previous_hash[1] = 74; 
    test->previous_hash[2] = '\0';

    test->votes = read_protected("declarations.txt");
    test->nonce = 212;

    //TEST 1 : save_block()  load_block()
    char* filename = key_to_str(test->author);
    strcat(filename,".txt");

    save_block(test,filename);
    
    Block* b = load_block(filename);
 
    printf("\n[ TEST 1 : save_block() and load_block() ]\n");
    printf("\ntest valide si [val] 1 et [val] 2 sont les memes : \n");

    printf("- author->val 1 : %ld ",test->author->val);
    printf("| author->val 2 : %ld \n",b->author->val);
    printf("- hash 1 : %s ",test->hash);
    printf("| hash 2 : %s \n",b->hash);
    printf("- previous_hash 1 : %s ",test->previous_hash);
    printf("| previous_hash 2 : %s \n",b->previous_hash);
    printf("- votes 1 : \n"); print_list_protected(test->votes);
    printf("- votes 2 : \n"); print_list_protected(b->votes);

    //TEST 2 : block_to_str() 
    printf("\n[ TEST 2 : block_to_str() ]\n");

    char* test_str = block_to_str(test);
    printf("%s \n",test_str);
    assert(block_to_str(NULL) == NULL);

    //TEST 4 : create_node(), update_height(), add_child(), print_tree()
    printf("\n[ TEST 4 : create_node(), update_height(), add_child(), print_tree() ]\n");

    CellTree* big_segment = create_node(load_block(filename));
    CellTree* tmp = big_segment;
    for(int i=0; i<10; i++) {
        Block* testblock = load_block(filename);
        CellTree* new_child = create_node(testblock);
        add_child(tmp,new_child);
        tmp = new_child;
    }

    print_tree(big_segment);

    CellTree* lastnode = last_node(big_segment);
    printf("\nthe last node of this blockchain is :\n");
    print_tree(lastnode);

    //TEST 5 : fuse_cell_protected() , get_full_list()
    printf("\n[ TEST 5 : fuse_cell_protected() , get_full_list() ]\n");

    CellProtected* l1 = read_protected("declarations.txt");
    CellProtected* l2 = read_protected("declarations.txt");
    CellProtected* fusion = fuse_cell_protected(l1,l2);
    print_list_protected(fusion);

    CellProtected* blockchain_list = get_full_list(big_segment);
    printf("\nthis list should have 120 lines : \n");
    print_list_protected(blockchain_list);

    //TEST 6 : submit_vote()
    CellProtected* tmp_list = blockchain_list;
    while(tmp_list) {
        submit_votes(tmp_list->data);
        tmp_list = tmp_list->next;
    }

    //TEST 7 : create_block()
    printf("\n[ TEST 7 : create_block() ]\n");
    Key* aut = (Key*) malloc(sizeof(Key));
    init_key(aut,404,303);
    create_block(big_segment,aut,1);
    print_tree(big_segment);

    //TEST 8 : add_block() , read_tree()
    printf("\n[ TEST 8 : add_block() , read_tree() ]\n");

    CellProtected* p = l1;
    int i = 0;
    while(p) {
        submit_votes(p->data);
    
        Key* tmpkey = (Key*) malloc(sizeof(Key));
        init_key(tmpkey,404,303);
        create_block(big_segment,tmpkey,1);
        
        char blockname[256];
        sprintf(blockname,"Block_%d",i);
        add_block(1,blockname);
        
        p = p->next;
        i++;
    }
    print_tree(big_segment);
    CellTree* blockchain = read_tree();
    print_tree(blockchain);

    //TEST 1 : frees 
    free(filename);
    free(test->author);
    free(test->hash);
    free(test->previous_hash);
    delete_liste_chainee(test->votes);
    free(test);

    free(b->author);
    free(b->hash);
    free(b->previous_hash);
    delete_liste_chainee(b->votes);
    free(b);
    
    //TEST 2 : frees
    free(test_str);

    //TEST 4 : frees
    delete_node_tree(big_segment);

    //TEST 5 : frees
    delete_liste_chainee(l1);
    delete_liste_chainee(l2);
    delete_liste_chainee(fusion);
    delete_liste_chainee(blockchain_list);

    //TEST 8 : frees
    delete_node_tree(blockchain);

    return 0;
} 
