//---------------------------------------------------------------------------
// (c) Copyright 2000 Yaron Keren. All rights reserved.
//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
//---------------------------------------------------------------------------
#include "szdd.h"
//---------------------------------------------------------------------------

#define N 4096
#define F 16
#define THRESHOLD 3

#define dad  (node+1)
#define lson (node+1+N)
#define rson (node+1+N+N)
#define root (node+1+N+N+N)
#define NIL -1

#pragma warn -8071
//---------------------------------------------------------------------------
char *buffer=NULL;
int *node=NULL;
int pos=0;
//---------------------------------------------------------------------------
int insert_node(int i, int run)
{
  int c, j, k, l, n, match;
  int *p;

  k = l = 1;
  match = THRESHOLD - 1;
  p = &root[(unsigned char) buffer[i]];
  lson[i] = rson[i] = NIL;

  while ((j = *p) != NIL)
    {
//      n = min (k, l);
      if (k<l) {
	n = k;
      } else{
	n = l;
      }
      while (n < run && (c = (buffer[j + n] - buffer[i + n])) == 0)
	n++;

      if (n > match)
	{
	  match = n;
	  pos = j;
	}
      if (c < 0)
	{
	  p = &lson[j];
	  k = n;
	}
      else if (c > 0)
	{
	  p = &rson[j];
	  l = n;
	}
      else
	{
	  dad[j] = NIL;
	  dad[lson[j]] = lson + i - node;
	  dad[rson[j]] = rson + i - node;
	  lson[i] = lson[j];
	  rson[i] = rson[j];
	  break;
	}
    }
  dad[i] = p - node;
  *p = i;
  return match;
} // insert_node
//---------------------------------------------------------------------------
void delete_node(int z) {
  int j;

  if (dad[z] != NIL)
    {
      if (rson[z] == NIL)
	{
	  j = lson[z];
	}
      else if (lson[z] == NIL)
	{
	  j = rson[z];
	}
      else
	{
	  j = lson[z];
	  if (rson[j] != NIL)
	    {
	      do
		{
		  j = rson[j];
		}
	      while (rson[j] != NIL);
	      node[dad[j]] = lson[j];
	      dad[lson[j]] = dad[j];
	      lson[j] = lson[z];
	      dad[lson[z]] = lson + j - node;
	    }
	  rson[j] = rson[z];
	  dad[rson[z]] = rson + j - node;
	}
      dad[j] = dad[z];
      node[dad[z]] = j;
      dad[z] = NIL;
    }
} // delete_node
//---------------------------------------------------------------------------
int compress(FILE* in, char *inname, int insize, FILE* out, char *outname, step_type step) {
  int ch, i, run, len, match, size, mask;
  char buf[F+1]; // was 17
  unsigned long magic1;
  unsigned long magic2;
  unsigned short magic3;
  unsigned long filesize;
  int chars_read=0;

  buffer = (char*)malloc (N + F + (N + 1 + N + N + 256) * sizeof (int));

  // Fill in header
  magic1 = 0x44445A53L;		/* SZDD */
  magic2 = 0x3327F088L;
  magic3 = 0x0041;
  filesize = insize;

  // Write header to the output file
  fwrite (&magic1, 1, sizeof (magic1), out);
  fwrite (&magic2, 1, sizeof (magic2), out);
  fwrite (&magic3, 1, sizeof (magic3), out);
  fwrite (&filesize, 1, sizeof (filesize), out);

  node = (int *) (buffer + N + F);
  for (i = 0; i < 256; i++)
    root[i] = NIL;
  for (i = NIL; i < N; i++)
    dad[i] = NIL;
  size = mask = 1;
  buf[0] = 0;
  i = N - F - F;
  for (len = 0; len < F && (ch = fgetc (in)) != EOF; len++)
    {
      buffer[i + F] = ch;
      i = (i + 1) % N;
    }
  run = len;
  do
    {
      ch = fgetc (in);
      chars_read++;

      if (chars_read%step_bytes==0) {
        step();
      }

      if (i >= N - F)
	{
	  delete_node (i + F - N);
	  buffer[i + F] = buffer[i + F - N] = ch;
	}
      else
	{
	  delete_node (i + F);
	  buffer[i + F] = ch;
	}
      match = insert_node (i, run);
      if (ch == -1)
	{
	  run--;
	  len--;
	}
      if (len++ >= run)
	{
	  if (match >= THRESHOLD)
	    {
	      buf[size++] = pos;
	      buf[size++] = ((pos >> 4) & 0xF0) + (match - 3);
	      len -= match;
	    }
	  else
	    {
	      buf[0] |= mask;
	      buf[size++] = buffer[i];
	      len--;
	    }
	  if (!((mask += mask) & 0xFF))
	    {
	      if (fwrite (buf, 1, size, out) != (size_t)size)
		{
		  perror(outname);
		  free(buffer);
		  return -1;
		}
	      size = mask = 1;
	      buf[0] = 0;
	    }
	}
      i = (i + 1) & (N - 1);
    }
  while (len > 0);

  if (size > 1)
    if (fwrite (buf, 1, size, out) != (size_t)size)
      {
	perror(outname);
	free(buffer);
	return -1;
      }

  free (buffer);
  return 0;
} // compress
//---------------------------------------------------------------------------
/*
void copy(FILE* in, FILE* out) {
  const int buf_sz=8192;
  char buf[buf_sz];

  while (1) {
    int bytes_read=fread(buf,sizeof(char),buf_sz,in);
    if (bytes_read==0) {
      break;
    }
    fwrite(buf,sizeof(char),bytes_read,out);
  }
}
*/
//---------------------------------------------------------------------------

