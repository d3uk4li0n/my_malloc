#include "../include/my_malloc.h"

#include <stdio.h>
#include <stdlib.h>
#define HASHTABLE_SIZE 97

 /*
 create a header node (singly linked list)
 */
typedef struct HeaderNode {

    int header; //size and flag
    struct HeaderNode* next; //pointer to next node
} HeaderNode;

#define HEADER_SIZE sizeof(HeaderNode)

/*
hash table element (singly linked list)
 */
typedef struct HashTableElement {
    int address;
    HeaderNode* data;
    struct HashTableElement* next; //pointer to next node
} HashTableElement;

 /*
hash table
 */
typedef struct HashTable {
    HashTableElement* table[HASHTABLE_SIZE];
} HashTable;

//the memory with header
static HeaderNode* dataList = NULL;

//store the used node
static HashTable hashTable;

//chunk align
size_t chunk_align(size_t size){
    return (((size)+(CHUNK_SIZE-1)) & ~(CHUNK_SIZE-1));
}

/*
  8 aligned
  return number of bytes requested
*/
static size_t align(size_t size) {
    if (size % 8 == 0)
    {
        return size;
    }
    //add bytes to 8 bytes aligned
    return size + (8 - size % 8);
}

/*
get size of node
*/
static int get_size(int header) {
    return (header & ~0x7); //clear 3 bits (right most)
}

/*
  check if the node is free
*/
static int is_allocated(int header) {
    return (header & 0x1); //last bit
}

//hash function
int hashFunction(int i){
    if (i < 0){
        i *= -1;
    }
    return i % HASHTABLE_SIZE;
}

//initialize the structure to manage to map
static void initialize(){

	int i;

    //if free table has not yet initialized
    if (dataList == NULL)
    {
        //printf("chunk_align(CHUNK_SIZE) = %ld\n", chunk_align(CHUNK_SIZE));
        dataList = (HeaderNode*)mmap(0, chunk_align(CHUNK_SIZE), PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
		//dataList = (HeaderNode*)malloc(chunk_align(CHUNK_SIZE) * sizeof(char));

        if (dataList == NULL)
        {
            fprintf(stderr, "mmap error\n");
            exit(-1);
        }        
        
        dataList->header = (CHUNK_SIZE - HEADER_SIZE) | 0x0; //mark as free
        dataList->next = NULL;

        //printf("dataList->header: %x ",  dataList->header);
        //printf(", is_allocated: %d\n", is_allocated(dataList->header));

        //initiale hash table
        for (i = 0; i < HASHTABLE_SIZE; i++)
        {
            hashTable.table[i] = NULL;
        }        

        //printf("size: %d ",  get_size(dataList->header));
        //printf(", is_allocated: %d\n", is_allocated(dataList->header));
    }
}

//add a node to hash
static void add_2_hash_table(HeaderNode* node){

    int address = *(int *)((char*)(node) + HEADER_SIZE);
    int idx = hashFunction(address);

    HashTableElement *newElement = (HashTableElement *)malloc(sizeof(HashTableElement));
    newElement->address = address;
    newElement->data = node;
    newElement->next = NULL;

    if (hashTable.table[idx] == NULL)
    {
        hashTable.table[idx] = newElement;
    }else{
        newElement->next = hashTable.table[idx];
        hashTable.table[idx] = newElement;
    }
}

/*
find free block from free list table
*/
static HeaderNode* find_free_block(int size) {

    HeaderNode* node = dataList; //from head node

    //printf("size: %d ",  get_size(node->header));
    //printf(", is_allocated: %d\n", is_allocated(node->header));

    //iterate and find the free block
    while (node != NULL && (is_allocated(node->header) == 1 || get_size(node->header) < size))
    {

        node = node->next;
        
        //printf("debug: (3) find_free_block, node = %p\n", node);
        //printf("debug: (3) find_free_block, node is null? %d\n", node == NULL);
        //printf("debug: (3) find_free_block, get_size(node->header) = %d ",  get_size(node->header));
    }

    //printf("node = %p\n", node);

    //free node not found
    if (node == NULL)
    {
        //return the block
        return NULL;
    }
    else {

        //split node ?
        if (get_size(node->header) - (int)size > (int)HEADER_SIZE)
        {
            //printf("debug: (1) find_free_block, node = %p\n", node);
            //printf("debug: (1) find_free_block, node + (HEADER_SIZE) = %p\n", (char*)node + HEADER_SIZE);
            //printf("debug: (1) find_free_block, node + (HEADER_SIZE + size) = %p\n", (char*)node + (HEADER_SIZE + size));

            HeaderNode* afterNode = (HeaderNode*)((char*)node + (HEADER_SIZE + size));
            afterNode->header = (get_size(node->header) - size - HEADER_SIZE) | 0x0; //free node
            afterNode->next = node->next;
            node->next = afterNode;

            //printf("debug: (1) find_free_block, afterNode = %p\n", afterNode);
            //printf("debug: (1) find_free_block, get_size(node->header) = %d\n", get_size(afterNode->header));
            //printf("debug: (1) find_free_block, is_allocated ? %d\n", is_allocated(afterNode->header));
            //printf("debug: (1) find_free_block, afterNode->next is null? %d\n", afterNode->next == NULL);
            
            //reduce the size of current node
            node->header = size;

            //printf("debug: (1) find_free_block, size = %d\n", size);
            //printf("debug: (1) find_free_block, node->next = %p\n", node->next);
            //printf("debug: (1) find_free_block, size: %d ",  get_size(node->header));
            //printf(", is_allocated: %d\n", is_allocated(node->header));


        }        

        return node;
    }
}

//malloc
void* my_malloc(size_t size) {

    //header node
    HeaderNode* node;

    //init allocator
    initialize();

    //size (aligned HEADER_SIZE bytes)
    size = align(size);

    //printf("debug: (1) my_malloc, size = %ld\n", size);
    
    //find free block
    node = find_free_block(size);

    //printf("debug: (2) my_malloc, size = %ld\n", size);

    if (node == NULL)
    {
        return NULL;
    }    

    node->header = size | 0x1; //mark as allocated
    add_2_hash_table(node); //add to hash table

    return ((char*)(node) + HEADER_SIZE);  //+ HEADER_SIZE (data part)
}

void* my_calloc(size_t nmemb, size_t size){

    //(aligned HEADER_SIZE bytes)
    int requested_size = align(nmemb * size);

    //call malloc
    void* ptr = malloc(requested_size);

    //zeros
    memset(ptr, 0, requested_size);

    return ptr;

}

void* my_realloc(void* ptr, size_t size){

	HeaderNode* curNode; //current node of ptr
    int curSize;         //current size
	HeaderNode* nextNode; //next node
	void *new_ptr;        //new ptr

    if (ptr == NULL)
    {
        return my_malloc(size);
    }    

    curNode = (HeaderNode*)((char*)ptr - HEADER_SIZE);
    curSize = get_size(curNode->header);

    //enough size
    if (curSize >= (int)size)
    {
        return ptr;
    }

    //try to merge with next node
    nextNode = curNode->next;
    if (nextNode != NULL && is_allocated(nextNode->header) == 0 && get_size(nextNode->header) + curSize >= (int)size)
    {
        //merge node
        curNode->next = nextNode->next;
        curNode->header = (get_size(nextNode->header) + curSize);
        curNode->header = curNode->header | 0x1; //mark as allocated

        return ptr;
    }
    
    //not enough
    new_ptr = my_malloc(size);
    if (new_ptr == NULL)
    {
        return NULL;
    }

    //copy current data
    memcpy(new_ptr, ptr, curSize);
    
    //free current
    my_free(ptr);

    return new_ptr;
}

//free
void my_free(void* ptr){

    int address = *(int *)((char*)(ptr));
    int idx = hashFunction(address);

    HashTableElement *prev = NULL;
    HashTableElement *curr = hashTable.table[idx];

    while (curr != NULL)
    {
        if (curr->address == address)
        {
            //set as free node
            curr->data->header = curr->data->header & ~0x1;

            if (prev == NULL)
            {
                hashTable.table[idx] = curr->next;
            }else{
                prev->next = curr->next;
            }
            
            free(curr);

            return;
        }

        prev = curr;
        curr = curr->next;
    }    
}

