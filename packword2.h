//---------------------------------------------------------------------------
#ifndef packword2H
#define packword2H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <NMsmtp.hpp>
#include <Psock.hpp>
//---------------------------------------------------------------------------
class TProgressForm : public TForm
{
__published:	// IDE-managed Components
        TProgressBar *ProgressBar;
        void __fastcall FormActivate(TObject *Sender);
        void __fastcall FormPaint(TObject *Sender);
private:	// User declarations
  FILE* in;
  FILE* out;

  char inname[512];
  char outname[512];
  char msg[512];

  int insize;
  int interactive;

  bool doClose;
  AnsiString e_mail;

  bool CheckSubscribe();
  void Subscribe();
public:		// User declarations
        __fastcall TProgressForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TProgressForm *ProgressForm;
//---------------------------------------------------------------------------
#endif
