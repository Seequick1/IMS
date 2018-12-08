#include <iostream>

#include "simlib.h"

/** GLOBALNE PREMENNE **/
#define POCET_LINIEK_PRE_CISTENIE 3
#define POCET_LINIEK_PRE_PRAZENICIE 3
#define POCET_DRVICIEK 1
#define POCET_MLETICEK 2
#define POCET_HYDRAULICKYCH_LISOV 5
#define POCET_MIESACOK 6

#define MINUTA 60                                   // minuta
#define HODINA MINUTA * 60                          // hodina
#define POLHODINA 60 * 30                           // pol hodina

#define KAPACITA_PODRVENYCH_SKLAD 1000              // velkost kapacity podrvenych
#define KAPACITA_PODRVENYCH_PODELENYCH_SKLAD 1000   // velkost kapacity rozdelenych podrvenych
#define KAPACITA_PRAZENYCH_SKLAD 1000               // velkost kapacity prazenych
#define KAPACITA_KAKAOVEHO_LIKERU_SKLAD 1000         // velkost kapacity likeru
#define VYSTUP_PRAZENIA 250                        // za jednotlivy pas 1000kg bobov

#define ROK 32140800
#define DEN HODINA * 24

Store Cisticky("Cisticky", POCET_LINIEK_PRE_CISTENIE);              // 3 linky pre cistenie
Store Prazenicky("Prazenie", POCET_LINIEK_PRE_PRAZENICIE);          // 3 linky pre prazenie
Store Drvicky("Drvicky", POCET_DRVICIEK);                           // 1 linka pre drtenie
Store Mleticky("Mleticky", POCET_MLETICEK);                         // 2 mlynceky
Store HydraulickyLis("HydraulickyLis", POCET_HYDRAULICKYCH_LISOV);  // 5 hydraulickych lisov
Store Miesacky("Miesacky", POCET_MIESACOK);                         // 6 miesacok

Store SkladPodrvenychBobov("Sklad podrvených bôbov", KAPACITA_PODRVENYCH_SKLAD);
Store SkladPrazenichBobov("Sklad prazenich bôbov", KAPACITA_PRAZENYCH_SKLAD);
Store SkladRozdelenychPodrvenychBobov("Sklad rozdrvenych podelenych bobov", KAPACITA_PODRVENYCH_PODELENYCH_SKLAD);
Store SkladKakaovehoLikeru("Sklad kakaoveho likeru", KAPACITA_KAKAOVEHO_LIKERU_SKLAD);

unsigned int pocetOdpaduSkrupinKg = 0;  // celkovy odpadu skrupin v kg
unsigned int pocetKakovejDrtiKg = 0;    // celkovy kakaovej drti v kg
unsigned int pocetKakaovehoLikeruKg = 0;    // celkovy kakaovej kakaoveho likeru v kg
unsigned int pocetLikeruNaLisovanie = 0;    // celkovy liker na lisovanie
unsigned int pocetLikeruAkoSurovina = 0;    // celkovy liker ako surovina
unsigned int pocetKakaovehoPrasku = 0;      // celkovy pocet kakaoveho prasku
unsigned int pocetKakaovehoMasla = 0;       // celkovy pocet kakaoveho masla
unsigned int pocetCokoladyKg = 0;           // celkovy pocet cokolady v kg
unsigned int aktualnyPocetKakaovejDrtiKg = 0; // aktualny kakaovej drti v kg
unsigned int aktualnyPocetKakaovehoLikeru = 0; // aktualny pocet kakaoveho likeru v kg
unsigned int aktualnyPocetLikeruNaLisovanie = 0; // aktulany pocet likeru na lisovanie v kg
unsigned int aktualnyPocetLikeruAkoSurovina = 0; // aktulany pocet likeru ako surovina v kg
unsigned int aktualnyPocetKakaovehoMasla = 0; // aktulany pocet kakaoveho masla v kg

double zaciatokSimulacie = 0;              // zaciatok simulacie
double konieSimulacie = 0;              // koniec simulacie

bool jeRozbehnuty = false;

/* PROCES CISTENIE */
class Cistenie : public Process {
    void Behavior(){
        // proces cistenia na kazdu linku
        if (Cisticky.Free()) {
            Enter(Cisticky, 1);
            Wait(POLHODINA);
            std::cout << "         -----> CISTENIE:Som v case   : " << (Time /3600)   << "h <-----" <<  std::endl;
        }
    }
};

/* PROCES PRAZENIE */
class Prazenie : public Process {
    void Behavior(){
        if(jeRozbehnuty){
            // proces prazenia na kazdu linku
            if (Prazenicky.Free() && Cisticky.Used() > 0){
                // odobereme gulicku od stavu vycistene
                Leave(Cisticky, 1);
                Enter(Prazenicky, 1);
                Wait(POLHODINA);
                Leave(Prazenicky, 1);
                std::cout << "<-        -----> PRAZENIE:Som v case   : " << (Time /3600)   << "h <-----" <<  std::endl;
                Enter(SkladPrazenichBobov, VYSTUP_PRAZENIA);
                std::cout << "Prazenie:SkladPrazenichBobov.Used() ->" << SkladPrazenichBobov.Used() << std::endl;
            }
        }
        else{
            // je to kvoli rozbehnutiu celeho systemu na zaciatku a posunutiu
            Wait(POLHODINA);
        }
        jeRozbehnuty = true;
    }
};

/* PROCES DRVENIE */
class Drvenie : public Process {
    void Behavior(){
        std::cout << "SkladPrazenichBobov.Used() ->" << SkladPrazenichBobov.Used() << std::endl;
        // proces drtenia
        // ak je drvicka volna a zaroven  v state pred drvenim je 375kg bobôv
        if ((Drvicky.Free()) && (SkladPrazenichBobov.Used() >= 375)) {
            // zobereme si jednu drticku
            Enter(Drvicky, 1);
            // odobereme 375kg
            Leave(SkladPrazenichBobov, 375);
            std::cout << "Drvenie:SkladPrazenichBobov.Used() ->" << SkladPrazenichBobov.Used() << std::endl;
            // pridame 375kg do skladu podrvenych bobov
            Enter(SkladPodrvenychBobov, 375);
            std::cout <<  "<------> DRVENIE:Som v case   : " << (Time /3600)   << "h <-----" <<  std::endl;
            while(SkladPodrvenychBobov.Used() > 0){
                // toto je odpad
                if(Random() <= 0.15){
                    SkladPodrvenychBobov.Leave(1);
                    pocetOdpaduSkrupinKg++;
                }
                // kakaova drt
                else if(Random()> 0.15){
                    SkladPodrvenychBobov.Leave(1);
                    pocetKakovejDrtiKg++;
                    aktualnyPocetKakaovejDrtiKg++;
                }
            }
            Leave(Drvicky, 1);
        }
    }
};

/////* PROCES MLETIE */
class Mletie : public Process {
    void Behavior(){
        // ak je stroj k dispozicii a zaroven je v bode viac ako 1349kg kakaovej drte
        // ak je pocetKakaovej drti viac ako 320kg
        if(Mleticky.Free() && aktualnyPocetKakaovejDrtiKg >= 350){
            // zobereme si 350 kakaovej drti
            aktualnyPocetKakaovejDrtiKg -= 350;
            // pridanie 350 likeru v kg
            pocetKakaovehoLikeruKg += 350;
            aktualnyPocetKakaovejDrtiKg += 350;
            std::cout <<  "<------> MLETIE:Som v case   : " << (Time /3600)   << "h <-----" <<  std::endl;
            Enter(Mleticky, 1);
            // vyrobenie 350g kakaoveho likeru
            Enter(SkladKakaovehoLikeru, 350);
            Leave(Mleticky, 1);
            if(aktualnyPocetKakaovejDrtiKg >= 350){
                // odoberie sa 350 kg kakaoveho likeru
                aktualnyPocetKakaovehoLikeru -= 350;
                // vznikne 192kg likeru na lisovanie
                pocetLikeruNaLisovanie += 192;
                aktualnyPocetLikeruNaLisovanie += 192;
                pocetLikeruAkoSurovina += 158;
                aktualnyPocetLikeruAkoSurovina += 158;
            }
        }
    }
};

/* PROCES STLACANIE */
class Stlacanie : public Process {
    void Behavior(){
        if(HydraulickyLis.Free() && aktualnyPocetLikeruNaLisovanie >= 192 ){
            Enter(HydraulickyLis, 1);
            // zobereme si likeru
            aktualnyPocetLikeruNaLisovanie -= 192;
            // pridanie kakaoveho prasku a masla
            pocetKakaovehoPrasku += 90;
            pocetKakaovehoMasla += 102;
            Leave(HydraulickyLis, 1);
        }
    }
};

/* PROCES MIXOVANIE */
class Mixovanie : public Process {
    void Behavior(){
        // mixovat sa bude jedine ak bude nejaka miesacka dostupna, zaroven likeru bude 450kg a zaroven 100kg masla
        if(Miesacky.Free() && aktualnyPocetLikeruAkoSurovina >= 450 && aktualnyPocetKakaovehoMasla > 100){
            Enter(Miesacky, 1);
            aktualnyPocetKakaovehoMasla -= 100;
            aktualnyPocetLikeruAkoSurovina -= 450;
            pocetCokoladyKg += 1000;
            Leave(Miesacky, 1);
        }
    }
};

class GeneratorCistenia : public Event {
    void Behavior (){
        double zaciatokSimulacie = Time;;        // premenna na ziskanie casu pri zaciatku simulacie
        // trikrat spustame procesu  imitovanie 3 Liniek
        (new Cistenie)->Activate();
        (new Cistenie)->Activate();
        (new Cistenie)->Activate();
        Activate(Time + POLHODINA);
    }
};

class GeneratorPrazenia : public Event {
    void Behavior(){
        // trikrat spustame proces prazenie imitovanie 3 Liniek
        (new Prazenie)->Activate();
        (new Prazenie)->Activate();
        (new Prazenie)->Activate();
        Activate(Time + POLHODINA);
    }
};

class GeneratorDrvenia : public Event {
    void Behavior(){
        (new Drvenie)->Activate();
        // kazdych 10 az 15 minut
        Activate(Time + Uniform(60*10, 60*15));
    }
};

class GeneratorMletia : public Event {
    void Behavior(){
        (new Mletie)->Activate();
        // kazdych 12 minut
        Activate(Time + 60*6);
    }
};

class GeneratorStlacania : public Event {
    void Behavior(){
        // vytvorenie naraz 5 paralenych mixerov
        (new Stlacanie)->Activate();
        (new Stlacanie)->Activate();
        (new Stlacanie)->Activate();
        (new Stlacanie)->Activate();
        (new Stlacanie)->Activate();
        // kazdych od 18 po 22 min
        Activate(Time + Uniform(60 * 18, 60 * 22));
    }
};

class GeneratorMixovania : public Event {
    void Behavior(){
        (new Mixovanie)->Activate();
        (new Mixovanie)->Activate();
        (new Mixovanie)->Activate();
        (new Mixovanie)->Activate();
        (new Mixovanie)->Activate();
        (new Mixovanie)->Activate();
        // 120 az 140 min
        Activate(Time + Uniform(120*60, 140*60));
    }
};

int main(void) {
    Init(0, 2 * HODINA);

    (new GeneratorCistenia)->Activate();
    (new GeneratorPrazenia)->Activate();
    (new GeneratorDrvenia)->Activate();
    (new GeneratorMletia)->Activate();
    (new GeneratorStlacania)->Activate();
    (new GeneratorMixovania)->Activate();
    Run();

    Cisticky.Output();
    Prazenicky.Output();
    Drvicky.Output();
    Mleticky.Output();
    HydraulickyLis.Output();
    Miesacky.Output();
    konieSimulacie = Time - zaciatokSimulacie;

    std::cout << "         -----> Odpadu skrupin    : " << pocetOdpaduSkrupinKg  << "kg/h <-----" << std::endl;
    std::cout << "         -----> Kakaovej drti     : " << pocetKakovejDrtiKg  << "kg/h <-----" << std::endl;
    std::cout << "         -----> Kakaoveho likeru  : " << pocetKakaovehoLikeruKg  << "kg/h <-----" << std::endl;
    std::cout << "         -----> Liker na lisovanie: " << pocetLikeruNaLisovanie  << "kg/h <-----" << std::endl;
    std::cout << "         -----> Liker ako surovina: " << pocetLikeruAkoSurovina  << "kg/h <-----" << std::endl;
    std::cout << "         -----> Kakaovy prasok    : " << pocetKakaovehoPrasku  << "kg/h <-----" << std::endl;
    std::cout << "         -----> Kakaove maslo     : " << pocetKakaovehoMasla  << "kg/h <-----" << std::endl;
    std::cout << "         -----> Cokolady          : " << pocetCokoladyKg  << "kg/h <-----" << std::endl;
    std::cout << "         -----> Sucet             : " << pocetKakovejDrtiKg + pocetOdpaduSkrupinKg  << "kg/h <-----" << std::endl;
    std::cout << "         -----> Celkovy cas       : " << (konieSimulacie /3600)   << "h <-----" <<  std::endl;

    return 0;
}