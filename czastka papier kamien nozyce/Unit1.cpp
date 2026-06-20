//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"


#define Sx 400 //definiowanie szerokosci obszaru ruchu
#define Sy 300 //definiowanie wysokosci obszaru ruchu
#define N 100  //maksymalna pojemnosc tablicy wskaznikow
int start =24; //poczatkowa liczba obiektow na poczatku symulacji

TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{

}
//---------------------------------------------------------------------------



class czastka     //klasa bazowa
  {
  protected:              //dane zhermetyzowane - dostep do dych danych maja tylko klasy dziedziczace i ta klasa
        int x0,y0,vx,vy;
        TShape *ksz;     //wskaznik na wizualne kolko
  public:
        czastka(void);                  //konstruktor
        virtual ~czastka(void);          //destruktor
        void ruch(void);                 //mechanika ruchu
        void odbij(void);                //mechanika odbijania kulek
        virtual char typ(void) = 0;      //f wirtualna wymusza na dzieciach posiadanie swojego zwracanego typu
        int getX(){                    //gettery bezpieczny odczyt chronionych danych
                return x0;
        }
        int getY(){
                return y0;
        }
        TShape* getksz(){
                return ksz;
        }
  };

class kamien : public czastka {       //kamien i kazda podklasa to samo dziedziczy mechanike ruhu odbijanie i zmienne z klasy czastka
public:
        kamien(): czastka() {                      //wywolanie konstruktora
                ksz->Brush->Color = clGray;       //nadanie koloru
        }
        char typ(){
                return 'K';          // z funkcji wirtualnej nadanie typu obiektowi
        }
};
class papier : public czastka {
public:
        papier(): czastka() {
                ksz->Brush->Color = clWhite;
        }
        char typ(){
                return 'P';
        }
};
class nozyce : public czastka {
public:
        nozyce(): czastka() {
                ksz->Brush->Color = clRed;
        }
        char typ(){
                return 'N';
        }
};

czastka *tw[N];   //   globalna tablica czastek, tablica wskaznikow przechowuje adresy pamieci pod ktorymi leza obiekty

czastka::czastka()
      {
      x0=random(Sx);            //losowanie wspolrzednych poczatkowych w oknie
      y0=random(Sy);
      while( (vx=random(7)-3)==0);      //losowanie poczatkowej predkosci   zakres -3 do 3 poza zerem bo obiet staby w miejscu
      while( (vy=random(7)-3)==0);
      ksz=new TShape(Form1);        //tworzenie wizualnego obiektu
      ksz->Parent=Form1;
      ksz->Left=x0;
      ksz->Top=y0;
      ksz->Width=5;
      ksz->Height=5;
      ksz->Shape=stCircle;  // tu dodatkowo ustawiamy na kó³ko
      }

czastka::~czastka()
      {    // usuwanie obiektu wizualnego po jego smierci
      if(ksz != NULL){
        ksz->Visible = false;
        delete ksz;
        ksz = NULL;
      }
      }



void czastka::ruch()
      {
      x0+=vx;      //aktualizacja pozycji obiektu
      y0+=vy;
      if(x0>=Sx) { vx=-vx; x0=Sx-1; }           // odbijanie od koncow okienka
      if(x0<0)   { vx=-vx; x0=0;    }
      if(y0>=Sy) { vy=-vy; y0=Sy-1; }
      if(y0<0)   { vy=-vy; y0=0;    }
      ksz->Left=x0;     // zmiana pozycji kolka jako ksztaltu
      ksz->Top=y0;
      }
void czastka::odbij(){
        vx = -vx;     //odbicie sie zmiana kierunku obiektu
        vy= -vy;
        x0 += vx *5;    //adresywne nadanie pedu zwiekszenie predkosci przy odbijaniu
        y0 += vy *5;
        ksz -> Left = x0;
        ksz -> Top = y0;
}

//void czastka::smierc()
  //{

  //}



void __fastcall TForm1::Button1Click(TObject *Sender)
  {       //public:
  }
//---------------------------------------------------------------------------




void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
    for(int i = 0; i < N; i++)     //wywolanie ruchu
    {
        if (tw[i] != NULL)      //jezeli miejsce w tabeli jest puste nie trzeba wykonowac operacji
        {
            tw[i]->ruch();
            // tw[i]->smierc();
        }
    }
    for(int i = 0; i<N;i++){
        if (tw[i] != NULL){   //jezeli miejsce w tabeli jest puste nie trzeba wykonowac operacji
             //continue;

        for (int j = i+1;j<N;j++){   //druga petla aby sprawdzac dwie unikalne pary i nie sprawdzac tego samego obiektu
                if(tw[j] != NULL){   //sprawdzamy czy w miejscu j jest jakos obiekt
                        //continue;
                        int dx = tw[i] -> getX() - tw[j]->getX();    //wyliczanie odleglosci obiektow od siebie
                        int dy = tw[i] -> getY() - tw[j]->getY();
                        if(dx*dx + dy*dy <= 25){      // jezeli odleglosc jest mniejsza niz 25 znaczy ze czastki na siebie naszly
                                char typ1 = tw[i]->typ();      //sprawdzamy typy czastki
                                char typ2 = tw[j]->typ();
                        if (typ1 != typ2){      //zderzenie wrogich sobie obiektow
                                if ((typ1 == 'K' && typ2 == 'N') || (typ1 == 'N' && typ2 == 'P') || (typ1 == 'P' && typ2 == 'K')){  //logika papier kamien nozyce
                                        delete tw[j];  //likwidacja przegranego obiektu
                                        tw[j]=NULL;    //wyczyszczenie wskaznika pamieci po przegranym
                                }else {
                                        delete tw[i];   //to tak samo tylko dla drugiego przegranego
                                        tw[i]=NULL;
                                        break;
                                }
                        }else{        //zderzaja sie te same obiekty
                                tw[i]->odbij();    //odbijaja sie od siebie
                                tw[j]->odbij();
                                for (int k = 0; k<N;k++){    //szukamy wolnego wskaznika w tablicy
                                        if(tw[k]==NULL){
                                                if(typ1=='K'){      //produkcja nowego obeiktu tego samego typu
                                                        tw[k]=new kamien();
                                                }else if(typ1=='P'){
                                                        tw[k]=new papier();
                                                }else {
                                                        tw[k]=new nozyce();
                                                }
                                                break;      //wchodzi tylko jeden nowy
                                        }
                                }
                        }
                }
                }
        }
        }
    }        //mechanizm konca gry sprawdzenie kto wygrywa
    int ilosckamieni = 0;
    int iloscpapieru = 0;
    int iloscnozyc = 0;
    for (int i = 0; i<N; i++){
        if (tw[i] != NULL){
                if(tw[i]->typ()=='K'){
                        ilosckamieni++;
                }else if(tw[i]->typ()=='P'){
                        iloscpapieru++;
                }else if(tw[i]->typ()=='N'){
                        iloscnozyc++;
                }
        }
    }
    if(ilosckamieni>0 && iloscpapieru == 0 && iloscnozyc == 0){    // sprawdzenie kto konkretnie wygrywa gdy wszystkie inne frakcje wyginely
        Timer1->Enabled = false;    //czas stop
        ShowMessage("Koniec wygraly kamienie");
    }else if(ilosckamieni==0 && iloscpapieru>0 && iloscnozyc == 0){
        Timer1->Enabled = false;
        ShowMessage("Koniec wygraly papiery");
    } else if(ilosckamieni==0 && iloscpapieru==0 && iloscnozyc > 0){
        Timer1->Enabled = false;
        ShowMessage("Koniec wygraly nozyce");
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button2Click(TObject *Sender)
{
        for(int i = 0;i<N;i++){     //czyszczenie pamieci aby usunac dane z poprzedniej gry w przypadku drugiego klikniecia start
                if(tw[i]!=NULL){
                        delete tw[i];
                        tw[i] = NULL;
                }
        }
        for(int i=0;i<start;i++){       //losowe wybieranie kogo jest ile
           int losowo = random(3);
           if(losowo == 0){
                tw[i]=new kamien();
           }else if(losowo == 1){
                tw[i]=new papier();
           }else {
                tw[i]=new nozyce();
           }
        }
Timer1->Enabled = 1;     //timer start, logika symulacji sie uruchamia
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button3Click(TObject *Sender)
{
Timer1->Enabled = false;
int ilosckamieni = 0;
int iloscpapieru = 0;
int iloscnozyc = 0;
for (int i = 0; i<N; i++){        //sprawdzenie stanu tablicy
        if (tw[i] != NULL){
                if(tw[i]->typ()=='K'){
                        ilosckamieni++;
                }else if(tw[i]->typ()=='P'){
                        iloscpapieru++;
                }else if(tw[i]->typ()=='N'){
                        iloscnozyc++;
                }
        }
}
String raport = "Zatrzymano symulacje wyniki: \n";   //konstrukcja komunikatu aby dac dany status gry i pokazac zwyciezce
raport += "Kamieni : " + IntToStr(ilosckamieni) + "\n";
raport += "Papieru : " + IntToStr(iloscpapieru) + "\n";
raport += "Nozyc : " + IntToStr(iloscnozyc) + "\n";
if (ilosckamieni > iloscpapieru && ilosckamieni > iloscnozyc){
        raport += "Wygral kamien";
}else if(ilosckamieni < iloscpapieru && iloscpapieru > iloscnozyc){
        raport += "Wygral papier";
}else if(ilosckamieni < iloscnozyc && iloscpapieru < iloscnozyc){
        raport+="Wygraly nozyce";
}else{
        raport += "Remis, brak zwyciezcy";
}
ShowMessage(raport);     //wyswietlenie komunikatu ze zwyciezca
int i;
for(i=0;i<N;i++){      //reset planszy
        delete tw[i];
        tw[i]=NULL;

}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormCreate(TObject *Sender)
{
    // czyscimy tablice wskaznikow na samym starcie aby uniknac smieci
    for(int i = 0; i < N; i++) {
        tw[i] = NULL;
    }
}
//---------------------------------------------------------------------------

