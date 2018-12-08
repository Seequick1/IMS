#include <iostream>

#include "simlib.h"

Queue frontaPreCistenie;                   // fronta pre cistenie
Queue frontaPreDrvenie;                   // fronta pre cistenie

#define MINUTA 60               // minuta
#define HODINA 60 * 60          // hodina
#define POLHODINA 60 * 30       // pol hodina

Facility LinkyPreCistenie[3];   // 3 linky pre cistenie
Facility LinkyPreDrteniee[3];   // 3 linky pre drtenie

class VyrobaCokolady : public Process {
    void Behavior(){

        // proces cistenia
        int indexLinkyPreCistenie = 0;
        for (int i = 0; i < 3; i++) { //pokus o nalezeni volneho zarizeni
            if (!LinkyPreCistenie[i].Busy()) {
                indexLinkyPreCistenie = i;
            }
        }
        Seize(LinkyPreCistenie[indexLinkyPreCistenie]);
        Wait(POLHODINA);
        Release(LinkyPreCistenie[indexLinkyPreCistenie]);

        // proces drtenia
        int indexLinkyPreDrvenie = 0;
        for (int i = 0; i < 3; i++) { //pokus o nalezeni volneho zarizeni
            if (!LinkyPreDrteniee[i].Busy()) {
                indexLinkyPreDrvenie = i;
            }
        }
        Seize(LinkyPreDrteniee[indexLinkyPreDrvenie]);
        Wait(Uniform(POLHODINA, 2*HODINA));
        Release(LinkyPreDrteniee[indexLinkyPreDrvenie]);
    }
};


// generovanie vyroby cokolady...
class Generator : public Event {
    void Behavior() {
        (new VyrobaCokolady)->Activate();
    }
};

int main(void) {
    for (int i = 0; i < 3; i++) {
        LinkyPreCistenie[i].SetQueue(frontaPreCistenie);
        LinkyPreDrteniee[i].SetQueue(frontaPreDrvenie);
    }
    Init(0, 100000);
    (new Generator)->Activate();
    Run();

    frontaPreCistenie.Output();
    frontaPreDrvenie.Output();
    for (int i = 0; i < 3; i++) {
        LinkyPreCistenie[i].Output();
        LinkyPreDrteniee[i].Output();
    }

    return 0;


//    std::cout << "Toto je kapacita linky pre cistenie ->" <<  LinkyPreCistenie.Capacity() << std::endl;
//    std::cout << "Toto je kapacita linky pre drtenie ->" << LinkyPreDrteniee.Capacity() << std::endl;
}