#include <iostream>

#include "simlib.h"

/** GLOBALNE PREMENNE **/
#define POCET_CISTICIEK 3
#define POCET_DRVICIEK 3


#define MINUTA 60               // minuta
#define HODINA 60 * 60          // hodina
#define POLHODINA 60 * 30       // pol hodina

#define KAPACITA_PODRVENYCH_SKLAD 1000

#define ROK 32140800
#define DEN HODINA * 24

Facility LinkyPreCistenie;   // 3 linky pre cistenie
Facility LinkyPreDrteniee;   // 3 linky pre drtenie


Store Cisticky("Cisticky", POCET_CISTICIEK);
Store* SkladPodrvenychBobo = new Store("Sklad podrvených bôbov", KAPACITA_PODRVENYCH_SKLAD);      // sklad kde si vytvoríme na boby s kapacitou 1000

unsigned int pocetOdpaduSkrupinKg = 0;  // odpadu skrupin v kg
unsigned int pocetKakovejDrtiKg = 0;    // kakaovej drti v kg

double zaciatokSimulacie = 0;              // zaciatok simulacie
double konieSimulacie = 0;              // koniec simulacie

// TODO: spravit to cez dve samotne processy..

bool prvyKrat = false;


class Cistenie : public Process {
    void Behavior(){
        // proces cistenia
        if (!LinkyPreCistenie.Busy()) {
            Seize(LinkyPreCistenie);
            Wait(POLHODINA);
            Release(LinkyPreCistenie);
        }
    }
};

class Drvenie : public Process {
    void Behavior(){
        // proces drtenia
        if (!LinkyPreDrteniee.Busy()) {
            Seize(LinkyPreDrteniee);
            Wait(HODINA);
            Release(LinkyPreDrteniee);
            SkladPodrvenychBobo->Enter(this, KAPACITA_PODRVENYCH_SKLAD);     // vlozenie 1000 zaznamov...
            while(SkladPodrvenychBobo->Used() > 0){
                // toto je odpad
                if(Random() <= 0.15){
                    SkladPodrvenychBobo->Leave(1);
                    pocetOdpaduSkrupinKg++;
                }
                    // kakaova drt
                else if(Random() > 0.15){
                    SkladPodrvenychBobo->Leave(1);
                    pocetKakovejDrtiKg++;
                }
            }
        }
    }
};

class GeneratorCistenia : public Event {
    void Behavior (){
        double zaciatokSimulacie = Time;;        // premenna na ziskanie casu pri zaciatku simulacie
        (new Cistenie)->Activate();
        Activate(Time + POLHODINA);
    }
};

class GeneratorDrvenia : public Event {
    void Behavior(){
        (new Drvenie)->Activate();
        Activate(Time + 1);
    }
};

int main(void) {
    Init(0, DEN);

    (new GeneratorCistenia)->Activate();
    (new GeneratorDrvenia)->Activate();
    Run();

    LinkyPreCistenie.Output();
    konieSimulacie = Time - zaciatokSimulacie;
//    for (int i = 0; i < 3; i++) {
//        LinkyPreCistenie[i].Output();
//        LinkyPreDrteniee[i].Output();
//    }

//    SkladPodrvenychBobo->Output();

    std::cout << "         -----> Odpadu skrupin: " << pocetOdpaduSkrupinKg  << "kg <-----" << std::endl;
    std::cout << "         -----> Kakaovej drti : " << pocetKakovejDrtiKg  << "kg <-----" << std::endl;
    std::cout << "         -----> Celkovy cas   : " << konieSimulacie  << "s <-----" <<  std::endl;
    std::cout << "         -----> Celkovy cas   : " << konieSimulacie /60  << "m <-----" <<  std::endl;
    std::cout << "         -----> Celkovy cas   : " << (konieSimulacie /3600)   << "h <-----" <<  std::endl;

    return 0;
}