/*
  CSE 109
  Sydney Adler
  sea220
  This program will take in a text as an input and a command
  line arguement, and store each word in a bucket memory. Then the
  program will print out each word.
  Program 2
*/

#include<stdio.h>
#include <string.h>
#include<stdlib.h>
#include"Bucket.h"

struct Bucket* createBucket(struct Bucket* bucket, size_t lineSize)
{
  bucket -> chunks = NULL;
  bucket -> numOfChunks = 0;
  bucket -> chunkSize = lineSize;
  bucket -> chunkCapacity = 0;
  bucket-> totalWords = 0;
  bucket-> listCapacity = 0;
  bucket-> list = NULL;
  return bucket;
}

struct Bucket*  destroyBucket(struct Bucket* bucket)
{
  for (int i = 0; i < bucket->numOfChunks; i++)
    {
      free(bucket->chunks[i].line);
      bucket->chunks[i].line = NULL;
    }
  free(bucket->list);
  bucket->list = NULL;
  
  free(bucket->chunks);
  bucket->chunks = NULL;
  free(bucket);
  bucket = NULL;

  return NULL;
}

size_t getSize(struct Bucket* bucket)
{
  return bucket -> numOfChunks;
}

char* getChunk(struct Bucket* bucket, size_t index)
{
  if(bucket->numOfChunks <= index)
    {
      return NULL;
    }
  return bucket->chunks[index].line;
}

char* getWord(struct Bucket* bucket, size_t index)
{
  return bucket->list[index];
}

size_t getChunkSize(struct Bucket* bucket, size_t index)
{
  if (index > bucket->numOfChunks) //checks for in bounds
    {
      return 0;
    }
  return bucket->chunkSize - bucket->chunks[index].spaceAvaliable; 
}

void writeChunk(struct Bucket* bucket, FILE* file, size_t index)
{
  if (bucket->numOfChunks < index)
    {
      return;
    }
  
  size_t chunkSize = getChunkSize(bucket, index);
  fprintf(file, "%6ld: ", chunkSize);
  for (int i=0; i<bucket->chunkSize; i++)
    {
      if (i >= chunkSize)
	{
	  fprintf(stdout, " ");
	  continue;
	}
      if (bucket->chunks[index].line[i] == '\0')
	{
	  fprintf(stdout, " ");
	}
      else
	{
	  fprintf(file, "%c", bucket->chunks[index].line[i]);
	}
    }
}

size_t insert(struct Bucket* bucket, char* word, size_t length)
{
  /*
    1.check if its already in a chunk, if its already in the chunk
    then remove it
    2. if its not then insert
    3. search for an exact fit to insert in the removed spaces
    4. search through chunks in order and if there is no empty space
    in any chunk then create new chunk
    5. check capacity and numOfChunks before adding another chunk
  */
  char* duplicate;
  size_t wordCount = 0;
  
  if (length > bucket->chunkSize)
    {
      return 2;
    }
  
  for (int i=0; i< bucket->totalWords; i++)
    {
      if (strcmp(getWord(bucket, i), word) == 0)
	{
	  duplicate = getWord(bucket,wordCount);
	  for (int k=0; k<length; k++)
	    {
	      duplicate[k] = '_';
	    }
	  memmove(bucket->list + i, bucket->list + i + 1, (bucket->totalWords - i - 1) * sizeof(char*));
	  bucket->totalWords--;
	  return 1;
	}
    }
  
  size_t underscoreCounter = 0;
  for (int l=0; l< bucket->numOfChunks; l++)
    {
      for(int m = 0; m< bucket->chunkSize; m++)
	{
	  if(bucket->chunks[l].line[m] == '_')
	    {
	      size_t n=m;
	      while (bucket->chunks[l].line[n] == '_')
		{
		  underscoreCounter++;
		  n++;
		}
	      n=m;
	      if(underscoreCounter == length)
		{
		  for(int p = 0; p< length; p++)
		    {
		      bucket->chunks[l].line[n] = word[p];
		      n++;
		    }
		  bucket->chunks[l].line[n+1] = '\0';
		  expandList(bucket);
		  bucket->list[bucket->totalWords] = word;
		  bucket->totalWords++;
		  return 0;
		}
	      m += underscoreCounter;
	    }
	  underscoreCounter = 0;
	}
    }
  
  for (int l=0; l< bucket->numOfChunks; l++)
    {
      if(bucket -> chunks[l].spaceAvaliable > length)
	{
	  strcpy(bucket->chunks[l].line + getChunkSize(bucket, l), word);
	  bucket->chunks[l].spaceAvaliable -= (length + 1);
	  expandList(bucket);
	  bucket->list[bucket->totalWords] = word;
	  bucket->totalWords++;
	  
	  return 0;
	  //insert into chunk l
	}
    }
  
  //expansion of chunks
  if (bucket->numOfChunks == bucket-> chunkCapacity)
    {
      size_t newCapacity = (bucket->chunkCapacity*2)+1;
      struct chunk* newChunk = (struct chunk*)malloc(newCapacity * sizeof(struct chunk));
      
      for(int i=0; i< bucket->numOfChunks; i++)
	{
	  newChunk[i] = bucket->chunks[i];
	}
      free(bucket->chunks);
      bucket->chunks = newChunk;
      newChunk = NULL;
      bucket->chunkCapacity = newCapacity;
    }
  
  bucket->chunks[bucket->numOfChunks].line = (char*)malloc(bucket->chunkSize);
  bucket->chunks[bucket->numOfChunks].spaceAvaliable = bucket->chunkSize;
  bucket->chunks[bucket->numOfChunks].numOfWords = 0;
  bucket->numOfChunks++;
  
  //insert into new chunk
  bucket->chunks[bucket->numOfChunks] = bucket->chunks[bucket->numOfChunks + 1];
  
  return 0;
}

void expandList (struct Bucket* bucket)
{
  if (bucket->totalWords == bucket->listCapacity)
    {
      size_t newCapacity = (bucket->listCapacity * 2) + 1;
      char** newList = (char**)malloc(newCapacity * sizeof(char*));
      
      for (int i=0; i< bucket->totalWords; i++)
	{
	  newList[i] = bucket->list[i];
	}
      free(bucket->list);
      bucket->list = newList;
      newList = NULL;
      bucket->listCapacity = newCapacity;
    }
}
