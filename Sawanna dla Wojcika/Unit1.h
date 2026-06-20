//---------------------------------------------------------------------------
#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <ExtCtrls.hpp> // Potrzebne do obs³ugi TTimer
//---------------------------------------------------------------------------

#define ROZMIAR 50

class Organizm;

class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TStringGrid *StringGrid1;
	TButton *ButtonTura;
	TTimer *Timer1;
        TMemo *MemoStats;
        TButton *Deszcz;
        TButton *Antylopaplus;
        TButton *Lewplus; // Nasz automatyczny zegar
	void __fastcall ButtonTuraClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall StringGrid1DrawCell(TObject *Sender, int ACol,
          int ARow, TRect &Rect, TGridDrawState State);
        void __fastcall DeszczClick(TObject *Sender);
        void __fastcall LewplusClick(TObject *Sender);
        void __fastcall AntylopaplusClick(TObject *Sender); // Funkcja wywo³ywana przez zegar
private:	// User declarations
    Organizm* plansza[ROZMIAR][ROZMIAR];
    void AktualizujEkran();
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
