#include <iostream>

#include "simlib.h"

Facility narocne ("Narocne");
Facility nornamlna ("Normalna");

class Zakaznik : public Process {
    void Behavior(){
        if (Uniform(0, 100) <= 33){
            Seize(narocne);
            Wait(Exponential(45));
            Release(narocne);
        }
        else{
            Seize(nornamlna);
            Wait(Exponential(15));
            Release(nornamlna);
        }
    }
};

class Generator : public Event {
    void Behavior(){
        (new Zakaznik) -> Activate();
        Activate(Time + Exponential(20));
    }
};

int main(void) {
    Init(0, 1000);
    (new Generator)->Activate();
    Run();
    SetOutput("prva.out");
}