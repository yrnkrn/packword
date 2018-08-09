//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("packword.res");
USEFORM("packword2.cpp", ProgressForm);
USEUNIT("ui.cpp");
USEUNIT("szdd.cpp");
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        try
        {
                 Application->Initialize();
                 Application->CreateForm(__classid(TProgressForm), &ProgressForm);
                 Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        return 0;
}
//---------------------------------------------------------------------------
