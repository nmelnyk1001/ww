#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "strbuf.h"

#ifndef DEBUG
#define DEBUG 0
#endif

int sb_init(strbuf_t *list, size_t length){
    list->data = malloc(sizeof(char) * length);
    if (!list->data)
        return EXIT_FAILURE;
    list->length = length;
    // Initialize end of list to be null terminated
    list->data[list->length] = '\0';
    list->used = 0;
    return EXIT_SUCCESS;
}

void sb_destroy(strbuf_t *list){
    free(list->data);
}

void sb_reset(strbuf_t *list, size_t length){
    list->length = 0;
    list->used = 0;
    list->data = realloc(list->data, sizeof(char) * length);
    memset(list->data, '\0', length);
}

// Resizes array size, return 0 if successful, 1 if error
int sb_inc_size(strbuf_t *list, size_t size){
    char *p = realloc(list->data, sizeof(char) * size);
    if (!p)
        return 1;
    list->data = p;
    list->length = size;
    list->data[list->length] = '\0';
    if (DEBUG) 
        printf("Last pos: %c\n", list->data[list->length]);
    return 0;
}

int sb_append(strbuf_t *list, char item){
    if (list->used == list->length){
        size_t size = list->length * 2;
        if (sb_inc_size(list, size))
            return 1;

        if (DEBUG) 
            printf("Increased size to %lu\n", size);
    }

    list->data[list->used] = item;
    ++list->used;
    // Null terminate the string, will be overwritten by next append
    list->data[list->used] = '\0';
    return 0;
}

int sb_remove(strbuf_t *list, char *item){
    if (list->used == 1)
        return 1;
    
    --list->used;

    if (item) 
        *item = list->data[list->used];

    return 1;
}

int sb_insert(strbuf_t *list, int index, char item){
    if (DEBUG) 
        printf("Length: %d\n", (int)list->length);

    if (index > list->length){
        if ( (2*list->length) > index) {
            int size = 2*list->length;
	    if (DEBUG) 
            printf("Increasing length to: %d\n", size);
            if (sb_inc_size(list, size))
		return 1;
        }
        else{
	    if (DEBUG) 
            printf("Increasing length to: %d\n", index+1);
            if (sb_inc_size(list, index+1))
                return 1;
        }
    }

    if (index > list->used){
        list->data[index] = item;
        if (DEBUG) 
            printf("List->data[%d]: %c\n", index, list->data[index]);
        list->used = index + 1;
        // Null terminate string
        list->data[list->used] = '\0';
        if (DEBUG) 
            printf("Used: %d -> %d\n", (int) list->used-1, (int) list->used);
    }
    else{
        if (DEBUG) 
            printf("Shifting data over\n");
        for (int pos = index; pos <= list->used; pos++){
            list->data[pos+1] = list->data[pos];
        }
        list->data[index] = item;
        if (DEBUG) 
            printf("List->data[%d]: %c\n", index, list->data[index]);
        list->used += 1;
        if (DEBUG) 
            printf("Used: %d -> %d\n", (int)list->used-1, (int) list->used);
    }
    return 0;
}

int sb_concat(strbuf_t *sb, char *str){
    int length = strlen(str);
    // check if length of str + length of used array is greater than the array size
    int size = (int)sb->used + length;
    if (DEBUG) 
        printf("String Length: %d\nList length: %d\nSize: %d\n", length, (int) sb->length, size);
    if (size > sb->length){
        // increase size accordingly
        if (sb_inc_size(sb, size))
            return 1;
    }

    // Add string to array
    while (*str != '\0'){
        sb->data[sb->used] = *str;
        sb->used++;
	    str++;
    }
    // Add a null terminatnor to the end 
    sb->data[sb->used] = '\0';
    return 0;
}
