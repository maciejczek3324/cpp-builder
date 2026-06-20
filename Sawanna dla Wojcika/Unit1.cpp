//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include <cstdlib> 
#include <ctime>   
#include <cmath> 
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

// --- UKRYTA WARSTWA TRAWY (ZIELONE TLO) ---
// Przechowuje informacje, czy na danej kratce rosnie trawa (true/false)
bool mapaTrawy[ROZMIAR][ROZMIAR];
//---------------------------------------------------------------------------

// ==================================================
// 1. KLASA BAZOWA DLA ZWIERZAT
// ==================================================
// Klasa Organizm to fundament - po niej dziedzicza lwy i antylopy.
// Zawiera wspolne cechy: wspolrzedne, wiek, plec i liczniki rozrodu.

class Organizm {
protected:
    int x, y;
    char znak;
    int wiek;
    int maxWiek;
    char plec; 
    int czasDoRozmnozenia; 
    int czasCiazy; 
public:
    Organizm(int x, int y, char z, char p, int mw) 
        : x(x), y(y), znak(z), plec(p), wiek(0), maxWiek(mw), czasDoRozmnozenia(0), czasCiazy(0) {}
    virtual ~Organizm() {}
    
    // Czysto wirtualna funkcja akcji - kazde zwierze musi miec wlasna logike ruchu
    virtual void akcja(Organizm* plansza[ROZMIAR][ROZMIAR]) = 0;
    
    char getZnak() { return znak; }
    char getPlec() { return plec; }
    int getX() { return x; }
    int getY() { return y; }
    int getCzasCiazy() { return czasCiazy; }
    
    void setCzasCiazy(int c) { czasCiazy = c; }
    void setCzasDoRozmnozenia(int c) { czasDoRozmnozenia = c; }
    
    virtual char getWyswietlanyZnak() { return znak; }
    
    bool czyDorosly() { return wiek >= 4; } // Zwierzeta dorastaja w 4 turze
    virtual bool czyUmarw() { return wiek >= maxWiek; }
    
    // Funkcja dodajaca lat i redukujaca czas odpoczynku po porodzie
    void postarz() { 
        wiek++; 
        if (czasDoRozmnozenia > 0) czasDoRozmnozenia--;
    }
};

// Deklaracje wyprzedzajace, aby kompilator wiedzial o ich istnieniu
class Lew;
class Antylopa;

// ==================================================
// 2. KLASA ZWIERZE (Z OBSLUGA UKRYTEJ TRAWY)
// ==================================================
// Rozszerza Organizm o logike poruszania sie, glodu i prokreacji.

class Zwierze : public Organizm {
protected:
    int glod; 
public:
    Zwierze(int x, int y, char z, char p, int mw) : Organizm(x, y, z, p, mw), glod(0) {}

    // Zwraca mala litere dla dzieci (np. 'l' dla malego lwa)
    virtual char getWyswietlanyZnak() {
        if (!czyDorosly()) {
            if (znak == 'L') return 'l';
            if (znak == 'A') return 'a';
        }
        return znak;
    }

    // Standardowy, losowy ruch na sasiadujace, wolne pole
    void ruchLosowy(Organizm* plansza[ROZMIAR][ROZMIAR]) {
        int dx = (rand() % 3) - 1;
        int dy = (rand() % 3) - 1;
        int nx = x + dx;
        int ny = y + dy;
        
        if (nx >= 0 && nx < ROZMIAR && ny >= 0 && ny < ROZMIAR) {
            if (plansza[nx][ny] == NULL) {
                plansza[nx][ny] = plansza[x][y];
                plansza[x][y] = NULL;
                x = nx; y = ny;
            }
        }
    }

    // Precyzyjny ruch w kierunku podanego celu (wykorzystywany przez sztuczna inteligencje)
    bool zrobKrokWStrone(Organizm* plansza[ROZMIAR][ROZMIAR], int cx, int cy) {
        int nx = x; int ny = y;
        if (cx > x) nx++; else if (cx < x) nx--;
        if (cy > y) ny++; else if (cy < y) ny--;
        
        if (nx >= 0 && nx < ROZMIAR && ny >= 0 && ny < ROZMIAR) {
            if (plansza[nx][ny] == NULL) {
                plansza[nx][ny] = plansza[x][y];
                plansza[x][y] = NULL;
                x = nx; y = ny;
                return true;
            }
        }
        return false;
    }

    // Deklaracje zlozonych metod zachowania (rozwiniete nizej)
    bool zjedzJesliObok(Organizm* plansza[ROZMIAR][ROZMIAR], char celZnak);
    bool idzDoNajblizszego(Organizm* plansza[ROZMIAR][ROZMIAR], char szukanyZnak, char szukanaPlec, bool musiBycDorosly, int zasieg);
    bool sprobujRozmnozyc(Organizm* plansza[ROZMIAR][ROZMIAR]);
    bool urodzDziecko(Organizm* plansza[ROZMIAR][ROZMIAR], char gatunek);
};

// ==================================================
// 3. DEFINICJA GATUNKU: LEW
// ==================================================

class Lew : public Zwierze {
public:
    Lew(int x, int y, char p) : Zwierze(x, y, 'L', p, 40) {} 
    
    // Lew ginie, gdy osiagnie maksymalny wiek lub nie jadl przez 12 tur
    virtual bool czyUmarw() {
        return (wiek >= maxWiek || glod >= 12); 
    }

    void akcja(Organizm* plansza[ROZMIAR][ROZMIAR]) {
        postarz();
        glod++; 

        // Sprawdzenie stanu ciazy i ewentualny porod
        if (plec == 'K' && czasCiazy > 0) {
            czasCiazy--;
            if (czasCiazy == 0) urodzDziecko(plansza, 'L');
        }

        // Logika mlodych lwow - podazaja za dorosla matka
        if (!czyDorosly()) {
            if (idzDoNajblizszego(plansza, 'L', 'K', true, 5)) return; 
            ruchLosowy(plansza);
            return;
        }

        // Logika glodu - poszukiwanie antylopy w promieniu 4 kratek
        if (glod > 4) {
            if (zjedzJesliObok(plansza, 'A')) return;
            if (idzDoNajblizszego(plansza, 'A', 'X', false, 4)) return; 
        }

        // Logika rozrodu - poszukiwanie partnera w promieniu 6 kratek
        if (glod <= 6 && czasDoRozmnozenia == 0 && czasCiazy == 0) {
            if (sprobujRozmnozyc(plansza)) return; 
            char poszukiwanaPlec = (plec == 'M') ? 'K' : 'M';
            if (idzDoNajblizszego(plansza, 'L', poszukiwanaPlec, true, 6)) return; 
        }

        // Domyslne akcje (jedzenie przypadkowej ofiary lub ruch)
        if (zjedzJesliObok(plansza, 'A')) return;
        ruchLosowy(plansza);
    }
};

// ==================================================
// 4. DEFINICJA GATUNKU: ANTYLOPA
// ==================================================

class Antylopa : public Zwierze {
public:
    Antylopa(int x, int y, char p) : Zwierze(x, y, 'A', p, 35) {} 
    
    virtual bool czyUmarw() {
        return (wiek >= maxWiek || glod >= 12); 
    }

    void akcja(Organizm* plansza[ROZMIAR][ROZMIAR]) {
        postarz();
        glod++;

        if (plec == 'K' && czasCiazy > 0) {
            czasCiazy--;
            if (czasCiazy == 0) urodzDziecko(plansza, 'A');
        }

        // Logika mlodych antylop
        if (!czyDorosly()) {
            if (idzDoNajblizszego(plansza, 'A', 'K', true, 4)) return;
            ruchLosowy(plansza);
            return;
        }

        // Szukanie i jedzenie zielonego podloza (trawy)
        if (glod > 2) {
            if (zjedzJesliObok(plansza, 'T')) return;
            if (idzDoNajblizszego(plansza, 'T', 'X', false, 4)) return;
        }

        // Rozmnazanie
        if (glod < 6 && czasDoRozmnozenia == 0 && czasCiazy == 0) {
            if (sprobujRozmnozyc(plansza)) return;
            char poszukiwanaPlec = (plec == 'M') ? 'K' : 'M';
            if (idzDoNajblizszego(plansza, 'A', poszukiwanaPlec, true, 5)) return;
        }

        if (zjedzJesliObok(plansza, 'T')) return;
        ruchLosowy(plansza);
    }
};

// ==================================================
// 5. LOGIKA POLOWANIA I DETEKCJI WARSTW
// ==================================================

// Sprawdza pola obok pod katem wystepowania jedzenia
bool Zwierze::zjedzJesliObok(Organizm* plansza[ROZMIAR][ROZMIAR], char celZnak) {
    if (celZnak == 'T') {
        // Antylopa zjada ukryta warstwe trawy
        if (mapaTrawy[x][y]) {
            mapaTrawy[x][y] = false; 
            glod = 0;
            return true;
        }
    } else {
        // Lew atakuje antylope na sasiednich kratkach
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                int nx = x + dx; int ny = y + dy;
                if (nx >= 0 && nx < ROZMIAR && ny >= 0 && ny < ROZMIAR) {
                    if (plansza[nx][ny] != NULL && plansza[nx][ny]->getZnak() == celZnak) {
                        delete plansza[nx][ny]; 
                        plansza[nx][ny] = plansza[x][y];
                        plansza[x][y] = NULL;
                        x = nx; y = ny;
                        glod = 0; 
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

// Najwazniejsza funkcja AI - "wzrok" zwierzat i obliczanie dystansu
bool Zwierze::idzDoNajblizszego(Organizm* plansza[ROZMIAR][ROZMIAR], char szukanyZnak, char szukanaPlec, bool musiBycDorosly, int zasieg) {
    int najX = -1, najY = -1;
    int minDystans = 9999;

    // Skanowanie otoczenia w promieniu okreslonym zmienna 'zasieg'
    for (int r = 1; r <= zasieg; r++) {
        for (int dx = -r; dx <= r; dx++) {
            for (int dy = -r; dy <= r; dy++) {
                int nx = x + dx; int ny = y + dy;
                if (nx >= 0 && nx < ROZMIAR && ny >= 0 && ny < ROZMIAR) {
                    
                    if (szukanyZnak == 'T') {
                        // Skanowanie tablicy z trawa
                        if (mapaTrawy[nx][ny]) {
                            int dystans = std::abs(nx - x) + std::abs(ny - y);
                            if (dystans < minDystans) {
                                minDystans = dystans; najX = nx; najY = ny;
                            }
                        }
                    } else {
                        // Skanowanie tablicy ze zwierzetami
                        Organizm* cel = plansza[nx][ny];
                        if (cel != NULL && cel->getZnak() == szukanyZnak) {
                            if (szukanaPlec != 'X' && cel->getPlec() != szukanaPlec) continue;
                            if (musiBycDorosly && !cel->czyDorosly()) continue;
                            if (cel->getCzasCiazy() > 0) continue; 
                            
                            int dystans = std::abs(nx - x) + std::abs(ny - y);
                            if (dystans < minDystans) {
                                minDystans = dystans; najX = nx; najY = ny;
                            }
                        }
                    }
                }
            }
        }
        if (minDystans != 9999) break; // Przerwanie skanowania jesli cel znaleziono blizej
    }

    if (najX != -1) return zrobKrokWStrone(plansza, najX, najY);
    return false;
}

// Inicjacja procesu rozmnozania (ciazy)
bool Zwierze::sprobujRozmnozyc(Organizm* plansza[ROZMIAR][ROZMIAR]) {
    Organizm* partner = NULL;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            int nx = x + dx; int ny = y + dy;
            if (nx >= 0 && nx < ROZMIAR && ny >= 0 && ny < ROZMIAR) {
                Organizm* sasiad = plansza[nx][ny];
                if (sasiad != NULL && sasiad->getZnak() == this->znak && 
                    sasiad->czyDorosly() && sasiad->getPlec() != this->plec && sasiad->getCzasCiazy() == 0) {
                    partner = sasiad;
                    break;
                }
            }
        }
        if (partner != NULL) break;
    }

    if (partner != NULL) {
        int dlugoscCiazy = (this->znak == 'L') ? 7 : 2; 
        if (this->plec == 'K') this->czasCiazy = dlugoscCiazy;
        else partner->setCzasCiazy(dlugoscCiazy);
        
        this->czasDoRozmnozenia = (this->znak == 'L') ? 12 : 2; 
        partner->setCzasDoRozmnozenia((this->znak == 'L') ? 12 : 2);
        return true; 
    }
    return false;
}

// Logika narodzin - tworzy nowy obiekt obok matki
bool Zwierze::urodzDziecko(Organizm* plansza[ROZMIAR][ROZMIAR], char gatunek) {
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            int nx = x + dx; int ny = y + dy;
            if (nx >= 0 && nx < ROZMIAR && ny >= 0 && ny < ROZMIAR) {
                if (plansza[nx][ny] == NULL) {
                    char nowaPlec = (rand() % 2 == 0) ? 'M' : 'K';
                    if (gatunek == 'L') plansza[nx][ny] = new Lew(nx, ny, nowaPlec);
                    else if (gatunek == 'A') plansza[nx][ny] = new Antylopa(nx, ny, nowaPlec);
                    
                    this->czasDoRozmnozenia = (gatunek == 'L') ? 10 : 2; 
                    return true; 
                }
            }
        }
    }
    return false;
}

// ==================================================
// 6. OBSLUGA OKIENKA VCL
// ==================================================

__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner) {}

// Przygotowanie symulacji podczas wlaczania programu
void __fastcall TForm1::FormCreate(TObject *Sender)
{
    srand(time(NULL));
    StringGrid1->DoubleBuffered = true; 

    for(int i = 0; i < ROZMIAR; i++) {
        for(int j = 0; j < ROZMIAR; j++) {
            plansza[i][j] = NULL;
            mapaTrawy[i][j] = false;
            int los = rand() % 1000;
            
            // Losowe wpuszczanie doroslych osobnikow na plansze startowa
            if (los < 20) {
                plansza[i][j] = new Lew(i, j, (rand() % 2 == 0) ? 'M' : 'K');
                plansza[i][j]->postarz(); plansza[i][j]->postarz(); plansza[i][j]->postarz(); plansza[i][j]->postarz();
            }
            else if (los < 200) {
                plansza[i][j] = new Antylopa(i, j, (rand() % 2 == 0) ? 'M' : 'K');
                plansza[i][j]->postarz(); plansza[i][j]->postarz(); plansza[i][j]->postarz(); plansza[i][j]->postarz();
            }
            else if (los < 600) {
                mapaTrawy[i][j] = true; // Generator zielonego tla
            }
        }
    }
    AktualizujEkran();
}

// Logika calej tury - wywolywana recznie i przez Timer
void __fastcall TForm1::ButtonTuraClick(TObject *Sender)
{
    bool wykonanyRuch[ROZMIAR][ROZMIAR];
    int aktualneLwy = 0;
    int aktualneAntylopy = 0;

    for(int i = 0; i < ROZMIAR; i++) {
        for(int j = 0; j < ROZMIAR; j++) {
            wykonanyRuch[i][j] = false;
            if(plansza[i][j] != NULL) {
                if(plansza[i][j]->getZnak() == 'L') aktualneLwy++;
                if(plansza[i][j]->getZnak() == 'A') aktualneAntylopy++;
            }
            
            // Samoczynne rozrastanie sie trawy po kazdej turze
            if(mapaTrawy[i][j] && rand() % 100 < 3) {
                int dx = (rand() % 3) - 1; int dy = (rand() % 3) - 1;
                int nx = i + dx; int ny = j + dy;
                if(nx >= 0 && nx < ROZMIAR && ny >= 0 && ny < ROZMIAR) {
                    mapaTrawy[nx][ny] = true;
                }
            }
        }
    }

    // Automatyczna migracja awaryjna jesli lwy wyginely
    if (aktualneLwy == 0) {
        int posadzone = 0;
        while (posadzone < 2) {
            int rx = rand() % ROZMIAR; int ry = rand() % ROZMIAR;
            if (plansza[rx][ry] == NULL) {
                plansza[rx][ry] = new Lew(rx, ry, (posadzone == 0) ? 'M' : 'K');
                plansza[rx][ry]->postarz(); plansza[rx][ry]->postarz(); plansza[rx][ry]->postarz(); plansza[rx][ry]->postarz();
                posadzone++;
            }
        }
    }

    // Automatyczna migracja awaryjna jesli antylopy wyginely
    if (aktualneAntylopy == 0) {
        int posadzone = 0;
        while (posadzone < 15) {
            int rx = rand() % ROZMIAR; int ry = rand() % ROZMIAR;
            if (plansza[rx][ry] == NULL) {
                plansza[rx][ry] = new Antylopa(rx, ry, (rand() % 2 == 0) ? 'M' : 'K');
                plansza[rx][ry]->postarz(); plansza[rx][ry]->postarz(); plansza[rx][ry]->postarz(); plansza[rx][ry]->postarz();
                posadzone++;
            }
        }
    }

    // Glowna petla uruchamiajaca zachowanie wszystkich organizmow
    for(int j = 0; j < ROZMIAR; j++) {
        for(int i = 0; i < ROZMIAR; i++) {
            if(plansza[i][j] != NULL) {
                if (plansza[i][j]->czyUmarw()) {
                    delete plansza[i][j];
                    plansza[i][j] = NULL;
                }
                else if (!wykonanyRuch[i][j]) {
                    Organizm* org = plansza[i][j];
                    org->akcja(plansza);
                    int nx = org->getX(); int ny = org->getY();
                    wykonanyRuch[nx][ny] = true;
                }
            }
        }
    }
    AktualizujEkran();
}

// Funkcja odpowiedzialna za generowanie panelu bocznego
void TForm1::AktualizujEkran()
{
    int lwM = 0, lwK = 0, lwDzieci = 0, lwCiaze = 0;
    int antM = 0, antK = 0, antDzieci = 0, antCiaze = 0;
    int iloscTrawy = 0;

    // Przeliczanie statystyk przed narysowaniem
    for(int j = 0; j < ROZMIAR; j++) {
        for(int i = 0; i < ROZMIAR; i++) {
            if (mapaTrawy[i][j]) iloscTrawy++;
            
            if(plansza[i][j] != NULL) {
                Organizm* org = plansza[i][j];
                char znak = org->getZnak();
                char plec = org->getPlec();
                bool dorosly = org->czyDorosly();

                if (znak == 'L') {
                    if (!dorosly) lwDzieci++;
                    else if (plec == 'M') lwM++;
                    else if (plec == 'K') {
                        lwK++; if (org->getCzasCiazy() > 0) lwCiaze++;
                    }
                }
                else if (znak == 'A') {
                    if (!dorosly) antDzieci++;
                    else if (plec == 'M') antM++;
                    else if (plec == 'K') {
                        antK++; if (org->getCzasCiazy() > 0) antCiaze++;
                    }
                }
            }
        }
    }

    // Wypisanie statystyk do Memo
    MemoStats->Clear(); 
    MemoStats->Lines->Add("    STATYSTYKI SAWANNY    ");
    MemoStats->Lines->Add(""); 
    MemoStats->Lines->Add("LWY (RAZEM: " + IntToStr(lwM + lwK + lwDzieci) + ")");
    MemoStats->Lines->Add(" - Samce: " + IntToStr(lwM));
    MemoStats->Lines->Add(" - Samice: " + IntToStr(lwK));
    MemoStats->Lines->Add(" - W ciazy: " + IntToStr(lwCiaze));
    MemoStats->Lines->Add(" - Mlode: " + IntToStr(lwDzieci));
    MemoStats->Lines->Add("");
    MemoStats->Lines->Add("ANTYLOPY (RAZEM: " + IntToStr(antM + antK + antDzieci) + ")");
    MemoStats->Lines->Add(" - Samce: " + IntToStr(antM));
    MemoStats->Lines->Add(" - Samice: " + IntToStr(antK));
    MemoStats->Lines->Add(" - W ciazy: " + IntToStr(antCiaze));
    MemoStats->Lines->Add(" - Mlode: " + IntToStr(antDzieci));
    MemoStats->Lines->Add("");
    MemoStats->Lines->Add("Zielona trawa: " + IntToStr(iloscTrawy));

    // Odswieza komponent i zmusza do ponownego pomalowania komorek
    StringGrid1->Invalidate();
}

// ==================================================
// SYSTEM RYSOWANIA GRAFIKI VCL
// ==================================================
void __fastcall TForm1::StringGrid1DrawCell(TObject *Sender, int ACol, int ARow, TRect &Rect, TGridDrawState State)
{
    // 1. Tlo kratki - jasna zielen, jesli wystepuje ukryta trawa
    if (mapaTrawy[ACol][ARow]) {
        StringGrid1->Canvas->Brush->Color = (TColor)0xD2FFD2; 
    } else {
        StringGrid1->Canvas->Brush->Color = clWhite;
    }
    StringGrid1->Canvas->FillRect(Rect);

    // 2. Rysowanie tekstu nad tlem (znaki zwierzat, plec i znaki ciazy)
    if (plansza[ACol][ARow] != NULL) {
        AnsiString info = AnsiString(plansza[ACol][ARow]->getWyswietlanyZnak());
        if (plansza[ACol][ARow]->getCzasCiazy() > 0) {
            info += "+";
        } else if (plansza[ACol][ARow]->getPlec() != 'X') {
            info += AnsiString(plansza[ACol][ARow]->getPlec());
        }

        StringGrid1->Canvas->Font->Color = clBlack;
        StringGrid1->Canvas->Font->Style = TFontStyles(); 
        
        // Obliczenia do wysrodkowania literki
        int xText = Rect.Left + (Rect.Width() - StringGrid1->Canvas->TextWidth(info)) / 2;
        int yText = Rect.Top + (Rect.Height() - StringGrid1->Canvas->TextHeight(info)) / 2;
        StringGrid1->Canvas->TextOut(xText, yText, info);
    }
}

// Czyszczenie pamieci RAM przy zamykaniu programu
void __fastcall TForm1::FormDestroy(TObject *Sender)
{
    for(int i = 0; i < ROZMIAR; i++) {
        for(int j = 0; j < ROZMIAR; j++) {
            if(plansza[i][j] != NULL) delete plansza[i][j];
        }
    }
}

// Zegar napedzajacy symulacje
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
    ButtonTuraClick(this); 
}

// ==================================================
// PRZYCISKI SPECJALNE (BOSKIE MOCE)
// ==================================================

// Tworzy nagly wysyp trawy na 30% pustych pol
void __fastcall TForm1::DeszczClick(TObject *Sender)
{
    for(int i = 0; i < ROZMIAR; i++) {
        for(int j = 0; j < ROZMIAR; j++) {
            if (!mapaTrawy[i][j]) { 
                if (rand() % 100 < 30) { 
                    mapaTrawy[i][j] = true;
                }
            }
        }
    }
    AktualizujEkran();
}

// Rzuca na plansze oddzial doroslych lwow
void __fastcall TForm1::LewplusClick(TObject *Sender)
{
    int lwy = 6;
    while(lwy > 0) {
        int rx = rand() % ROZMIAR;
        int ry = rand() % ROZMIAR;
        if(plansza[rx][ry] == NULL) {
            char p = (lwy % 3 == 0) ? 'M' : 'K'; 
            plansza[rx][ry] = new Lew(rx, ry, p);
            for(int k = 0; k < 4; k++) plansza[rx][ry]->postarz(); 
            lwy--;
        }
    }
    AktualizujEkran();
}

// Rzuca na plansze spora kolonie doroslych antylop
void __fastcall TForm1::AntylopaplusClick(TObject *Sender)
{
    int antylopy = 15;
    while(antylopy > 0) {
        int rx = rand() % ROZMIAR;
        int ry = rand() % ROZMIAR;
        if(plansza[rx][ry] == NULL) {
            char p = (antylopy % 3 == 0) ? 'M' : 'K'; 
            plansza[rx][ry] = new Antylopa(rx, ry, p);
            for(int k = 0; k < 4; k++) plansza[rx][ry]->postarz(); 
            antylopy--;
        }
    }
    AktualizujEkran();
}
//---------------------------------------------------------------------------
