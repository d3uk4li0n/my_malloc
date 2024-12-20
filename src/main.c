#include "../include/my_malloc.h"

#include <string.h>

int main(){  //int argc, char **argv
    char* mem1;
	char* mem2;

	char* mem = my_malloc(100 * sizeof(char));
    
    strcpy(mem, "1"); 
    printf("address: %p, mem = %s\n", mem, mem);    

    my_free(mem);

	mem = my_malloc(2 * sizeof(char));
    strcpy(mem, "2");
    printf("address: %p, mem = %s\n", mem, mem);    

    my_free(mem);

    //test malloc again
    mem = my_malloc(100 * sizeof(char));
    strcpy(mem, "123");
    printf("address: %p, mem = %s\n", mem, mem);    

    my_free(mem);

	//test my_calloc
	mem = my_calloc(1, 100 * sizeof(char));
    printf("address: %p, mem[0] = %d\n", mem, (int)mem[0]);    

    my_free(mem);

	//test realloc
	mem1 = my_malloc(100 * sizeof(char));
    strcpy(mem1, "123");
    printf("address of mem1: %p, mem1 = %s\n", mem1, mem1);    

	mem2 = my_malloc(100 * sizeof(char));
    strcpy(mem2, "abc");
    printf("address of mem2: %p, mem2 = %s\n", mem2, mem2);   

    my_free(mem2);

	//realloc mem1
	mem1 = my_realloc(mem1, 200 * sizeof(char));
    strcpy(mem1 + 3, "xyz");
    printf("address of mem1: %p, mem1 = %s\n", mem1, mem1); 

    return 0;
}
