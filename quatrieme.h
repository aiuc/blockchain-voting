#include "troisieme.h"

typedef struct block {
    Key* author;
    CellProtected* votes;
    unsigned char* hash;
    unsigned char* previous_hash;
    int nonce;
} Block;

typedef struct block_tree_cell {
    Block* block;
    struct block_tree_cell* father;
    struct block_tree_cell* firstChild;
    struct block_tree_cell* nextBro;
    int height;
} CellTree;

void save_block(Block* b, char* filename);
Block* load_block(char* filename);
char* block_to_str(Block* block);
void compute_proof_of_work(Block* b, int d);
int verify_block(Block* b, int d);
void delete_block(Block* b);

CellTree* create_node(Block* b);
int update_height(CellTree* father, CellTree* child);
void add_child(CellTree* father, CellTree* child);
void print_tree(CellTree* ct);
void delete_node(CellTree* node);
void delete_node_tree(CellTree* tree);

CellTree* highest_child(CellTree* cell);
CellTree* last_node(CellTree* tree);

Protected* copy_protected(Protected* p);
CellProtected* copy_list(CellProtected* list);
CellProtected* fuse_cell_protected(CellProtected* cp1, CellProtected* cp2);
CellProtected* get_full_list(CellTree* blockchain);

void submit_votes(Protected* p);
void create_block(CellTree* tree, Key* author, int d);
void add_block(int d, char* name);

CellTree* read_tree();







