//---------------------------------------------------------------------------
// (c) Copyright 2000 Yaron Keren. All rights reserved.
//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <vcl.h>
#include <vcl/registry.hpp>
#pragma hdrstop
//---------------------------------------------------------------------------
#include "packword2.h"
#include "szdd.h"
#include "ui.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TProgressForm *ProgressForm;
//---------------------------------------------------------------------------
unsigned char se_doc[] = {
#include "se.cpp"
};

const char s_compressed[]="(compressed)";
const char s_doc[]=".doc";
//const char s_Run[]="Run";

//---------------------------------------------------------------------------
void install_rmb() {
  AnsiString CurVer=NULL;

  TRegistry* Registry;
  Registry=new TRegistry;
  Registry->RootKey=HKEY_CLASSES_ROOT;
  if (Registry->OpenKey(AnsiString("Word.Document\\CurVer"),true)) {
    CurVer=Registry->ReadString("");
  }

  delete Registry;

  if (CurVer==NULL) {
    return;
  }

  Registry=new TRegistry;
  Registry->RootKey=HKEY_CLASSES_ROOT;

  try {
    Registry->OpenKey(CurVer+"\\shell\\packword",true);
    Registry->WriteString("","&PackWord");

    Registry->OpenKey("command",true);
//    Registry->WriteString("","\""+AnsiString(_argv[0])+" %1\"");
    Registry->WriteString("",AnsiString(_argv[0])+" \"%1\"");
  }
  catch (...) {
  }

  delete Registry;
}
//---------------------------------------------------------------------------
/*
int run_no() {
  int run;

  TRegistry* Registry;
  Registry=new TRegistry;

  Registry->OpenKey("Software\\PackWord",true);
  try {
    run=Registry->ReadInteger(s_Run);
  }
  catch (...) {
    run=0;
  }
  run++;
  Registry->WriteInteger(s_Run,run);

  delete Registry;

  return run;
}
//---------------------------------------------------------------------------
void TProgressForm::Subscribe() {
  try {
    SMTP->Host="mail.packword.com";
    SMTP->Port=25;
    SMTP->UserID=NULL;
    SMTP->Connect();

    SMTP->PostMessage->FromAddress=e_mail;
    SMTP->PostMessage->ToBlindCarbonCopy->Clear();
    SMTP->PostMessage->ToCarbonCopy->Clear();
    SMTP->PostMessage->Attachments->Clear();
    SMTP->PostMessage->FromName="PackWord.exe ";
    SMTP->PostMessage->Subject="subscribe packword-announce";
    SMTP->PostMessage->ToAddress->Clear();
    SMTP->PostMessage->ToAddress->Add("info@packword.com");
    SMTP->PostMessage->Body->Clear();
    SMTP->PostMessage->Body->Add("");
    SMTP->SendMail();
    SMTP->Disconnect();
  }
  catch (...) {
    return;
  }
}
//---------------------------------------------------------------------------
bool TProgressForm::CheckSubscribe() {
  int run=run_no();
  if (run==30) {
    int answer=Application->MessageBox(
"You have just run PackWord thirty times. We hope you find PackWord useful and will continue to use it.\n\
\n\
Would you like to hear about the next version of PackWord, when available?",
    version "Congratulations!",MB_YESNO);

    if (answer==IDYES) {
      if (InputQuery(version "Subscribe","Enter your e-mail address (to abort press Cancel)",e_mail)) {
        return true;
      } // if InputQuery
    }
  }

  return false;
}
*/
//---------------------------------------------------------------------------
__fastcall TProgressForm::TProgressForm(TComponent* Owner)
        : TForm(Owner)
{
  doClose=true;
  Caption=version "Compressing...";
}
//---------------------------------------------------------------------------
void step() {
  ProgressForm->ProgressBar->StepBy(step_bytes);
}
//---------------------------------------------------------------------------
void __fastcall TProgressForm::FormActivate(TObject *Sender)
{
  interactive=(_argc<2);

  install_rmb();
/*
  if (CheckSubscribe()) {
    Subscribe();
  }
*/
  if (interactive) {
    if (!ask_for_file(inname)) {
      return;
    }
  } else {
    strcpy(inname,_argv[1]);
  }

// if no extra arguments do show all messages even if invoked from command line (also as rmb)
  interactive=(_argc<3);

  if (strstr(inname,s_compressed)!=NULL) {
    sprintf(msg, "'%s' is already compressed.\n",inname);
    message(msg,"Problem",interactive);
    return;
  }

  strcpy(outname, inname);

  struct stat st;
  if (stat (inname, &st) < 0) {
    sprintf(msg, "Could not get size of file '%s'.\n",inname);
    message(msg,"Problem",interactive);
    return;
  }

  insize=st.st_size;

  ProgressBar->Min=0;
  ProgressBar->Max=insize;

  // chop .doc
  char* p=strchr(outname,'.');
  if (p==NULL) {
    sprintf(msg, "The file '%s' has no .doc extension.\n",inname);
    message(msg,"Problem",interactive);
    return;
  }
  *p='\0';
  strcat(outname,s_compressed);
  strcat(outname,s_doc);

  out=fopen (outname, "wb");
  if (out == NULL) {
    sprintf(msg, "The file '%s' could not be written.\n",outname);
    message(msg,"Problem",interactive);
    return;
  }

// write se.doc
  fwrite(se_doc,sizeof(char),sizeof(se_doc)/sizeof(char),out);

  in = fopen (inname, "rb");
  if (in==NULL) {
    sprintf(msg, "The file '%s' could not be read.\n",inname);
    message(msg,"Problem",interactive);
    return;
  }

  doClose=false;
}
//---------------------------------------------------------------------------
void __fastcall TProgressForm::FormPaint(TObject *Sender)
{
  if (doClose) {
    Close();
    return;
  }

  static bool once=false;
  if (once) {
    once=true;
    return;
  }

  int se_size=ftell(out);
// then append the compressed data
  compress (in, inname, insize, out, outname, step);
// finally append se.doc size
  fwrite (&se_size, 1, sizeof (se_size), out);

  int outsize=ftell(out);
  fclose (out);
  fclose (in);

  if (outsize<insize) {
    double perc=100.0*outsize/insize;
    sprintf(msg,"'%s' was compressed from %d bytes to %d bytes, %.1f%% of original size.\n\nThe compressed file is named '%s'.",inname,insize,outsize,perc,outname);
  } else {
    sprintf(msg,"'%s' could not be compressed since it is too small or its contents are not compressible - probably images.\n\nPlease use the original file instead of a compressed version.",inname);
    remove(outname);
  }

  Visible=false;

  message(msg,version "Results",interactive);

  Close();
}
//---------------------------------------------------------------------------

