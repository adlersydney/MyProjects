#ifndef LIST_H
#define LIST_H

#include<stdio.h>

struct Bucket {
  struct chunk* chunks;
  size_t numOfChunks;
  size_t chunkSize;
  size_t chunkCapacity; //how big the chunk array is
  size_t totalWords;
  char** list;
  size_t listCapacity;
  size_t listCounter; //keeps track of how many words your insert
};

struct chunk
{
  char* line;
  size_t spaceAvaliable;
  size_t numOfWords;
};

struct Bucket* createBucket (struct Bucket* bucket, size_t);
struct Bucket*  destroyBucket(struct Bucket*);
size_t getSize(struct Bucket*);
char* getChunk(struct Bucket* bucket, size_t);
char* getWord(struct Bucket*, size_t);
size_t getChunkSize(struct Bucket*, size_t);
void writeChunk(struct Bucket*, FILE*, size_t);
size_t insert(struct Bucket*, char*, size_t);
void expandList (struct Bucket* bucket);
#endif
