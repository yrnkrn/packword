//---------------------------------------------------------------------------
// (c) Copyright 2000 Yaron Keren. All rights reserved.
//---------------------------------------------------------------------------

#define step_bytes 20000

typedef void step_type();

int insert_node(int i, int run);
void delete_node(int z);
int compress(FILE* in, char *inname, int insize, FILE* out, char *outname, step_type step);
