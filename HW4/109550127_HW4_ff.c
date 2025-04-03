#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

struct block {
    size_t size;
    int free;
    struct block *prev;
    struct block *next;
};
void* mem_pool;
size_t pool_len = 20000;
int init = 1;

void *malloc(size_t size){
    if(init){
        mem_pool = mmap(NULL, pool_len, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
        struct block *init_data = mem_pool;
        init_data->size = pool_len;
        init_data->next = NULL;
        init_data->prev = NULL;
        init_data->free = 1;
        init_data->size-=32;
        init=0;
    }
    if(size==0){
        size_t max_able_size = 0;
        struct block *now = mem_pool;
        int cnt = 0;
        while(now!=NULL){
            if((max_able_size < now->size) && (now->free == 1)){
                max_able_size = now->size;
            }
            now = now->next;
        }

        int num = max_able_size;
        if(num==0){
            cnt=1;
        }
        else{
            while(num!=0){
                cnt++;
                num/=10;
            }
        }
        int num1 = max_able_size;
        char max_chunk[cnt+1];
        max_chunk[cnt]='\n';
        if(num1==0){
            max_chunk[0]='0';
        }
        else{
            while(num1!=0){
                cnt--;
                max_chunk[cnt]=(char)((num1 % 10) + '0');
                num1/=10;
            }
        }

        char text[50]="Max Free Chunk Size = ";
        strcat(text, max_chunk );
        write(1,text,strlen(text));
        munmap(mem_pool, pool_len);
        return NULL;
    }

    if(size!=0){
        size_t need_size = (((size-1)/32)+1)*32;
        size_t min_able_size = 20000;
        struct block *min_able_block = NULL;
        struct block *now =  mem_pool;
        while(now != NULL){
            size_t nowsize=now->size;
            if(nowsize >= need_size && now->free == 1){
                min_able_block = now;
                min_able_size = nowsize;
                break;
            }
            else{
                now = now->next;
            }
        }
        if(min_able_size == need_size){
            min_able_block->free = 0;
        }
        size_t tmp=need_size+32;
        if(min_able_size >= tmp){
            struct block *after_next = (void*)min_able_block + 32 + need_size;
            struct block *before_next = min_able_block->next;
            after_next->size = min_able_block->size - need_size;
            after_next->free = 1;
            after_next->prev = min_able_block;
            after_next->next = before_next;
            after_next->size -=32;     

            min_able_block->free = 0;
            min_able_block->size = need_size;
            min_able_block->next = after_next;

            if(before_next!=NULL){
                before_next->prev = after_next;
            }
        }
        return (void*)(min_able_block)+32;
    }
}

void free(void *p){
    struct block* now = p-32;
    struct block *before_prev = now->prev;
    struct block* before_next = now->next;
    now->free = 1;
    if(before_next != NULL && before_next->free == 1){
        if(before_next->next != NULL){
            before_next->next->prev = now;
        }
        now->size = now->size + before_next->size ;
        now->size += 32;
        now->next = before_next->next;

        before_next->next = NULL;
        before_next->prev = NULL;
        before_next->size = 0;
        before_next->free = 0;
    }
    if(before_prev != NULL && before_prev->free == 1){
        if(now->next != NULL){
            now->next->prev = before_prev;
        }
        before_prev->next = now->next;
        before_prev->size = before_prev->size + now->size;
        before_prev->size += 32;
        now->next = NULL;
        now->prev = NULL;
        now->free = 0;
        now->size = 0;
    }
}
