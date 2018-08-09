//---------------------------------------------------------------------------
// (c) Copyright 2000 Yaron Keren. All rights reserved.
//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#pragma hdrstop
//---------------------------------------------------------------------------
#include "ui.h"
//---------------------------------------------------------------------------
int ask_for_file(char* filename) {
  TOpenDialog* od=new TOpenDialog(NULL);

  od->Title=version "Select document to compress";
  od->Filter="Word files (*.doc)|*.doc|RTF files (*.rtf)|*.rtf";
  od->DefaultExt="doc";
  od->Options << ofNoChangeDir;
  od->Options << ofOverwritePrompt;

  int r;
  if (od->Execute()) {
    strcpy(filename,od->FileName.c_str());
    r=1;
  } else {
    strcpy(filename,"");
    r=0;
  }
  delete od;

  return r;
}
//---------------------------------------------------------------------------
void message(char* msg, char* title, int interactive) {
  if (interactive) {
    Application->MessageBox(msg,title,MB_OK);
  } else {
    printf(msg);
  }
} // message
//---------------------------------------------------------------------------

