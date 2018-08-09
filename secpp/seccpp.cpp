#include <stdio.h>
#include <stdlib.h>

int main() {
  FILE* se;
  se=fopen("se.doc","rb");
  if (se==NULL) {
    exit(1);
  }
  
  const int maxbuf=200000;
  unsigned char buf[maxbuf];

  int l=fread(buf,sizeof(char),maxbuf,se);
  fclose(se);

  FILE* secpp;
  secpp=fopen("se.cpp","wt");

  int i;
  for (i=0;i<l;i++) {
    fprintf(secpp,"0x%02x ",int(buf[i]));
    if (i<l-1) {
      fprintf(secpp,", ");
    }
    if ((i+1)%16==0) {
      fprintf(secpp,"\n");
    }
  }

  fclose(secpp);

  return 0;
}

