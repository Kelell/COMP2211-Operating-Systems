#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include "memory_management.h"

typedef enum {false, true} bool; //bool values implemented for free values.

//implementing the block headers

typedef struct block {
  size_t blockSize;
  bool free;
  struct block *next;
} blockHeader;

blockHeader* head=NULL; //initalise the start of the memory location

//This function spilts a block that has excess space into one block
//with the requested space and another block that holds the excess
void splitBlock(blockHeader *freeBlock,size_t freeSize){
    blockHeader *new=(void*)((void*)freeBlock+freeSize+sizeof(blockHeader));
    new->blockSize=(freeBlock->blockSize)-freeSize-sizeof(blockHeader);
    new->free=true;
    new->next=freeBlock->next;
    freeBlock->blockSize=freeSize;
    freeBlock->free=false;
    freeBlock->next=new;
}

//malloc takes a request for a certain amount of space a returns a pointer
//to the allocated space, or NULL if the value is 0 or invalid
void *_malloc(size_t size){
 blockHeader *cur,*prev;
 void *ptr;

 //If the user requests an invalid value with malloc, return NULL
 if(size <1){
    ptr=NULL;
    printf("Requested size is 0, NULL returned\n"); //testing line
    return ptr;
  }


 //if there is no memory, allocate the start of the fisrt block.
 if(head==NULL){
   head= (blockHeader*)sbrk(4096);
   head->blockSize=size-sizeof(blockHeader);
   head->free=false;
   //printf("Head has been created! with size: "); testing line
   // printf("%d %d\n", head->blockSize, head); testing line

   //after the head has been created, create the first block.
   blockHeader *newBlock=(blockHeader*)((char*)head+size+sizeof(head));
   head->next=newBlock;
   newBlock->blockSize=4096-sizeof(blockHeader)-size;
   newBlock->free=true;
   newBlock->next=NULL;
   //printf("Curret avaiable memory: "); testing line
   //printf("%d %d\n", newBlock->blockSize, newBlock); testing line
 }
 cur=head;

 //Check the existing blocks for the memory requested, loops while there
 //are no free blocks
 while(cur!=NULL){
  prev=cur;

  //If there exists a block with the exact amount of memory as he number
  //request, return the whole block
  if((cur->blockSize)==size){
      cur->free=false;
       printf("%d %d\n", cur->blockSize, cur); //testing line
      ptr=(void*)(++cur);
      cur=cur->next;
      return ptr;
    }

  //If there exists a block with excess memory, execute the splitBlock function
    if((cur->blockSize)>(size+sizeof(blockHeader))){

      splitBlock(cur,size);
       printf("%d %d\n", cur->blockSize, cur); //testing line
       printf("next:%d %d\n", cur->next->blockSize, cur->next); //testing line
      ptr=(void*)(++cur);
      cur=cur->next;
      return ptr;
    }

    //If no block exists with enough memory, make one
    if(cur->next==NULL){
      blockHeader *new;
      new = (blockHeader*)sbrk(4096);
      if(prev->free){ //If the previous block is free, merge the new memory with it
        prev->blockSize=prev->blockSize+4096;
        }
        else{ //if the prevoius block isn't free, make a new block
          new->blockSize=4096;
          new->free=true;
          new->next=NULL;
          cur->next=new;
        }
        cur=head; //restart the loopas you're at the end by this point
        printf("Create Land!\n"); //testing line
      }
      cur=cur->next;
    }
}

/*
void _free(void * ptr){
}
*/

int main(){
  //Testing the addBlock function
  _malloc(100);
  _malloc(5000);
  _malloc(10000);
  return 0;
}
