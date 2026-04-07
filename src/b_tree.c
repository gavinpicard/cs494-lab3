#include "b_tree.h"
#include "jdisk.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

typedef struct tnode {
  unsigned char bytes[JDISK_SECTOR_SIZE+256]; /* This holds the sector for reading and writing.  
                                                 It has extra room because your internal representation  
                                                 will hold an extra key. */
  unsigned char nkeys;                      /* Number of keys in the node */
  unsigned char flush;                      /* Should I flush this to disk at the end of b_tree_insert()? */
  unsigned char internal;                   /* Internal or external node */
  unsigned int lba;                         /* LBA when the node is flushed */
  unsigned char **keys;                     /* Pointers to the keys.  Size = MAXKEY+1 */
  unsigned int *lbas;                       /* Pointer to the array of LBA's.  Size = MAXKEY+2 */
  struct tnode *parent;                     /* Pointer to my parent -- useful for splitting */
  int parent_index;                         /* My index in my parent */
  struct tnode *ptr;                        /* Free list link */
} Tree_Node;

typedef struct {
  int key_size;                 /* These are the first 16/12 bytes in sector 0 */
  unsigned int root_lba;
  unsigned long first_free_block;

  void *disk;                   /* The jdisk */
  unsigned long size;           /* The jdisk's size */
  unsigned long num_lbas;       /* size/JDISK_SECTOR_SIZE */
  int keys_per_block;           /* MAXKEY */
  int lbas_per_block;           /* MAXKEY+1 */
  Tree_Node *free_list;         /* Free list of nodes */
  
  Tree_Node *tmp_e;             /* When find() fails, this is a pointer to the external node */
  int tmp_e_index;              /* and the index where the key should have gone */
 
  int flush;                    /* Should I flush sector[0] to disk after b_tree_insert() */
} B_Tree;

// This creates an empty btree with the given file size, key_size and filename. 
// The empty btree will have a root node which is external and has zero keys. 
// It returns a handle to the btree in a void *.
void *b_tree_create(char *filename, long size, int key_size) 
{
  int MAXKEY = (JDISK_SECTOR_SIZE - 6) / (key_size + 4);
  B_Tree *bt = (B_Tree *) malloc(sizeof(B_Tree));

  bt->key_size = key_size;
  bt->root_lba = 1;
  bt->first_free_block = 1;

  bt->disk = jdisk_create(filename, size);
  bt->size = size
  bt->num_lbas = size / JDISK_SECTOR_SIZE;
  bt->keys_per_block = MAXKEY;
  bt->lbas_per_block = MAXKEY + 1;
  bt->free_list = NULL;

  Tree_Node *root = tree_node_create(tree, 0);
  // something to free & write to disk

  // something to write header to sector 0
  bt->flush = 0;

  return (void *) bt;
}

// This opens the given btree file, which should have been created previously with b_tree_create(). 
// Again, it returns a handle to the btree.
void *b_tree_attach(char *filename) 
{
  return NULL;
}

// In this procedure, key is a pointer to key_size bytes, and record is a pointer to JDISK_SECTOR_SIZE bytes. 
// If the key is in the btree, then the procedure replaces the val with record, and returns the LBA of the val. 
// If the key is not in the btree, then it is inserted, and the val for that key is set to record. 
// In either case, the LBA of the val is returned. It will return 0 if our file is out of room.
// When this returns, the btree file is in the proper shape (in other words, jdisk_write() 
// calls need to be made for all of the sectors that have been added or changed).
//
// I want to stress here that even though our examples above used null-terminated strings as keys, 
// our btrees can take any keys that are key_size bytes. Use memcmp() for key comparison. (And use memcpy() to copy keys and vals to their respective homes if need be).
unsigned int b_tree_insert(void *b_tree, void *key, void *record) 
{
  return 0;
}

// This finds the given key, and returns the LBA of the val. 
// If the key is not in the tree, this returns 0.
unsigned int b_tree_find(void *b_tree, void *key)
{
  return 0;
}

// This returns the jdisk pointer for the btree.
void *b_tree_disk(void *b_tree) 
{
  return ((B_Tree *) b_tree)->disk;
}

// This returns the key size.
int b_tree_key_size(void *b_tree) 
{
  return ((B_Tree *) b_tree)->key_size;
}

// This prints the tree -- see my examples for format. 
// I'm not going to grade you on this. This can be a very useful procedure for debugging.
void b_tree_print_tree(void *b_tree) 
{
  B_Tree *tree = (B_Tree *) b_tree;

  return;
}

static Tree_Node *tree_node_create(B_Tree *tree, char internal) {
  Tree_Node *node;

  if (tree->free_list != NULL) {
    node = tree->free_list;
    tree->free_list = node->ptr; // move free list forward
  } else {
    node = malloc(sizeof(Tree_Node));
    node->keys = malloc(sizeof(unsigned char *) * (tree->keys_per_block + 1));
    node->lbas = malloc(sizeof(unsigned int) * (tree->lbas_per_block + 1));

    for (int i = 0; i < tree->keys_per_block + 1; i++) {
      node->keys[i] = node->bytes + 2 + i * tree->key_size;
    }
  }

  node->lba = tree->first_free_block;
  tree->first_free_block++;
  tree->flush = 1;

  node->nkeys = 0;
  node->internal = internal;
  node->flush = 1;
  node->parent = NULL;
  node->parent_index = -1;

  return node;
}