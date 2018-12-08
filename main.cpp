#include <iostream>

#include "simlib.h"

/** GLOBALNE PREMENNE **/
#define POCET_LINIEK_PRE_CISTENIE 3
#define POCET_LINIEK_PRE_PRAZENICIE 3
#define POCET_DRVICIEK 1
#define POCET_MLETICEK 2
#define POCET_HYDRAULICKYCH_LISOV 5
#define POCET_MIESACOK 6
#define POCET_KONSOVACICH_STROJOV 16
#define POCET_FORMOVACICH_LINIEK 2

#define MINUTA 60                                   // minuta
#define HODINA MINUTA * 60                          // hodina
#define POLHODINA 60 * 30                           // pol hodina

#define KAPACITA_PODRVENYCH_SKLAD 1000              // velkost kapacity podrvenych
#define KAPACITA_PRAZENYCH_SKLAD 1000               // velkost kapacity prazenych
#define KAPACITA_KAKAOVEHO_LIKERU_SKLAD 1000         // velkost kapacity likeru
#define KAPACITA_VSEOBECNEHO_SKLADU 40000         // velkost vseobecneho skladu
#define VYSTUP_PRAZENIA 250                        // za jednotlivy pas 1000kg bobov

#define ROK 32140800
#define DEN HODINA * 24

Store Cisticky("Cisticky", POCET_LINIEK_PRE_CISTENIE);              // 3 linky pre cistenie
Store Prazenicky("Prazenie", POCET_LINIEK_PRE_PRAZENICIE);          // 3 linky pre prazenie
Store Drvicky("Drvicky", POCET_DRVICIEK);                           // 1 linka pre drtenie
Store Mleticky("Mleticky", POCET_MLETICEK);                         // 2 mlynceky
Store HydraulickyLis("HydraulickyLis", POCET_HYDRAULICKYCH_LISOV);  // 5 hydraulickych lisov
Store Miesacky("Miesacky", POCET_MIESACOK);                         // 6 miesacok
Store KonsovaciStroj("KonsovaciStroj", POCET_KONSOVACICH_STROJOV);  // 16 konsovacich strojov
Store Formovacky("Formovacky", POCET_FORMOVACICH_LINIEK);           // 2 linky pre formovanie cokolady

Store SkladPrazenichBobov("Sklad prazenich bôbov", KAPACITA_PRAZENYCH_SKLAD);
Store SkladPodrvenychBobov("Sklad podrvených bôbov", KAPACITA_PODRVENYCH_SKLAD);
Store SkladKakaovehoLikeru("Sklad kakaoveho likeru", KAPACITA_VSEOBECNEHO_SKLADU);
Store SkladKakaovejDrti("Sklad kakaovej drti", KAPACITA_VSEOBECNEHO_SKLADU);
Store SkladLikeruNaLisovanie("Sklad likeru na lisovanie", KAPACITA_VSEOBECNEHO_SKLADU);
Store SkladLikeruNaAkoSurovina("Sklad likeru ako surovina", KAPACITA_VSEOBECNEHO_SKLADU);
Store SkladKakaovehoMasla("Sklad kakaoveho masla", KAPACITA_VSEOBECNEHO_SKLADU);
Store SkladCokolady("Sklad cokolady", KAPACITA_VSEOBECNEHO_SKLADU);
Store SkladKonsovanejCokolady("Sklad konsovanej cokolady", KAPACITA_VSEOBECNEHO_SKLADU);
Store SkladFormovanejCokolady("Sklad formovanej cokolady", KAPACITA_VSEOBECNEHO_SKLADU);

unsigned int pocetOdpaduSkrupinKg = 0;  // celkovy odpadu skrupin v kg
unsigned int pocetKakovejDrtiKg = 0;    // celkovy kakaovej drti v kg
unsigned int pocetKakaovehoLikeruKg = 0;    // celkovy kakaovej kakaoveho likeru v kg
unsigned int pocetLikeruNaLisovanie = 0;    // celkovy liker na lisovanie
unsigned int pocetLikeruAkoSurovina = 0;    // celkovy liker ako surovina
unsigned int pocetKakaovehoPrasku = 0;      // celkovy pocet kakaoveho prasku
unsigned int pocetKakaovehoMasla = 0;       // celkovy pocet kakaoveho masla
unsigned int pocetCokoladyKg = 0;           // celkovy pocet cokolady v kg
unsigned int pocetKonsovanejCokoladyKg = 0; // celkovy pocet konsovanej cokolady v kg
unsigned int pocetFormovanychCokolad = 0;   // celkovy pocet formovanych cokolad v kg
unsigned int pocetTemperovanychCokolad = 0;   // celkovy pocet temperovanych cokolad v kg

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
            std::cout << "|CISTENIE|:Som v case   : " << (Time /3600)   << "h <-----" <<  std::endl;
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
                std::cout << "|PRAZENIE|:Som v case   : " << (Time /3600)   << "h <-----" <<  std::endl;
                Enter(SkladPrazenichBobov, VYSTUP_PRAZENIA);
                std::cout << "|PRAZENIE|:SkladPrazenichBobov.Used() ->" << SkladPrazenichBobov.Used() << std::endl;
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
        // proces drtenia
        // ak je drvicka volna a zaroven  v state pred drvenim je 375kg bobôv
        if ((Drvicky.Free()) && (SkladPrazenichBobov.Used() >= 375)) {
            std::cout <<  "|DRVENIE|:Som v case   : " << (Time /3600)   << "h <-----" <<  std::endl;
            std::cout << "|DRVENIE|:SkladPrazenichBobov.Used() ->" << SkladPrazenichBobov.Used() << std::endl;
            // zobereme si jednu drticku
            Enter(Drvicky, 1);
            // odobereme 375kg
            Leave(SkladPrazenichBobov, 375);
            // pridame 375kg do skladu podrvenych bobov
            Enter(SkladPodrvenychBobov, 375);
            while(SkladPodrvenychBobov.Used() > 0){
                // toto je odpad
                if(Random() <= 0.15){
                    Leave(SkladPodrvenychBobov, 1);
                    pocetOdpaduSkrupinKg++;
                }
                // kakaova drt
                else if(Random()> 0.15){
                    Leave(SkladPodrvenychBobov, 1);
                    pocetKakovejDrtiKg++;
                    Enter(SkladKakaovejDrti, 1);
                }
            }
            Leave(Drvicky, 1);
        }
    }
};

/////* PROCES MLETIE */
class Mletie : public Process {
    void Behavior(){
        // ak je stroj k dispozicii a zaroven je v bode viac ako 350 kakaovej drte
        // ak je pocetKakaovej drti viac ako 320kg
        if(Mleticky.Free() && SkladKakaovejDrti.Used() >= 350){
            Enter(Mleticky, 1);
            std::cout <<  "|MLETIE|:Som v case   : " << (Time /3600)   << "h <-----" <<  std::endl;
            std::cout <<  "|MLETIE|:Pridany drt   : " << SkladKakaovejDrti.Used()   << "kg <-----" <<  std::endl;
            Leave(SkladKakaovejDrti, 350);

            // pridanie 350 likeru v kg
            pocetKakaovehoLikeruKg += 350;
            Enter(SkladKakaovehoLikeru, 350);
            Leave(Mleticky, 1);
        }
        if(SkladKakaovehoLikeru.Used() > 350){
            // odpocitanie od skladky 350kg likeru
            Leave(SkladKakaovehoLikeru, 350);
            pocetLikeruNaLisovanie += 192;
            pocetLikeruAkoSurovina += 158;
//            vznikne 192kg likeru na lisovanie
            Enter(SkladLikeruNaLisovanie, 192);
            Enter(SkladLikeruNaAkoSurovina, 158);
        }
    }
};

///* PROCES STLACANIE */
class Stlacanie : public Process {
    void Behavior(){
        if(HydraulickyLis.Free() && SkladLikeruNaLisovanie.Used() >= 192 ){
            std::cout <<  "|STLACANIE|:Som v case|:" << (Time /3600)   << "h <-----" <<  std::endl;
            Enter(HydraulickyLis, 1);
            // zobereme si likeru
            Leave(SkladLikeruNaLisovanie, 192);
            // pridanie kakaoveho prasku a masla
            pocetKakaovehoPrasku += 90;
            pocetKakaovehoMasla += 102;
            Enter(SkladKakaovehoMasla, 102);
            Leave(HydraulickyLis, 1);
            std::cout <<  "|STLACANIE|:pocet kakoveho prasku|:" << pocetKakaovehoPrasku <<  std::endl;

        }
    }
};

///* PROCES MIXOVANIE */
class Mixovanie : public Process {
    void Behavior(){
        // mixovat sa bude jedine ak bude nejaka miesacka dostupna, zaroven likeru bude 450kg a zaroven 100kg masla
        if(Miesacky.Free() && SkladLikeruNaAkoSurovina.Used() >= 450 && SkladKakaovehoMasla.Used() >= 100){
            Enter(Miesacky, 1);
            Leave(SkladLikeruNaAkoSurovina, 450);
            Leave(SkladKakaovehoMasla, 100);
            pocetCokoladyKg += 1000;
            Enter(SkladCokolady, 1000);
            Leave(Miesacky, 1);
        }
    }
};

/* PROCES KONSOVANIE */
class Konsovanie : public Process{
    void Behavior(){
        // konsovat sa bude jedine ak bude nejaky konsovaci stoj volny a zaroven bude pripravenych 2000kg cokolody
        if(KonsovaciStroj.Free() && SkladCokolady.Used() >= 2000){
            Enter(KonsovaciStroj, 1);
            Leave(SkladCokolady, 2000);
            pocetKonsovanejCokoladyKg += 2000;
            Enter(SkladKonsovanejCokolady, 2000);
            Leave(KonsovaciStroj, 1);
        }
    }
};

/* PROCES FORMOVANIE */
class Formovanie : public Process {
    void Behavior(){
        // formovanie sa bude jedine ak bude nejake formovaciu linka k dispozicii a zaroven bude pripravencych 2000kg
        // konsovanej cokolady
        if(Formovacky.Free() && SkladKonsovanejCokolady.Used() >= 1000){
            Enter(Formovacky, 1);
            Leave(SkladKonsovanejCokolady, 1000);
            pocetFormovanychCokolad += 1000;
            Enter(SkladFormovanejCokolady);
            Leave(Formovacky, 1);
        }
    }
};

/* PROCES TEMPEROVANIE */
class Temperovanie : public Process {
    void Behavior(){
        // temperovanie sa bude vykonavat ak budu k dispozicii 1000kg formovanej cokolady
        if(SkladFormovanejCokolady.Used() >= 1000){
            pocetTemperovanychCokolad += 1000;
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
        (new Mletie)->Activate();
        // kazdych 6 minut
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

class GenerovanieKonsovania : public Event {
    void Behavior(){
        (new Konsovanie)->Activate();
        (new Konsovanie)->Activate();
        (new Konsovanie)->Activate();
        (new Konsovanie)->Activate();
        (new Konsovanie)->Activate();
        (new Konsovanie)->Activate();
        (new Konsovanie)->Activate();
        (new Konsovanie)->Activate();
        (new Konsovanie)->Activate();
        (new Konsovanie)->Activate();
        (new Konsovanie)->Activate();
        (new Konsovanie)->Activate();
        (new Konsovanie)->Activate();
        (new Konsovanie)->Activate();
        (new Konsovanie)->Activate();
        (new Konsovanie)->Activate();
        (new Konsovanie)->Activate();
        // 16 - 24h
        Activate(Time + Uniform(16 * HODINA, 24 * HODINA));
    }
};

class GeneratorFormovania : public Event{
    void Behavior(){
        (new Formovanie)->Activate();
        (new Formovanie)->Activate();
        // 25 min
        Activate(Time + 25*60);
    }
};

class GeneratorTemperovania : public Event {
    void Behavior(){
        (new Temperovanie)->Activate();
        // 35 - 40 min
        Activate(Time + Uniform(60 *35, 60*40));
    }
};

int main(void) {
    Init(0, 100 * HODINA);
    (new GeneratorCistenia)->Activate();
    (new GeneratorPrazenia)->Activate();
    (new GeneratorDrvenia)->Activate();
    (new GeneratorMletia)->Activate();
    (new GeneratorStlacania)->Activate();
    (new GeneratorMixovania)->Activate();
    (new GenerovanieKonsovania)->Activate();
    (new GeneratorFormovania)->Activate();
    (new GeneratorTemperovania)->Activate();
    Run();

//    Cisticky.Output();
//    Prazenicky.Output();
//    Drvicky.Output();
//    Mleticky.Output();
//    HydraulickyLis.Output();
//    Miesacky.Output();
    konieSimulacie = Time - zaciatokSimulacie;

    std::cout << "         ----->    Drt + Odpad      : " << pocetKakovejDrtiKg + pocetOdpaduSkrupinKg  << "kg/h <-----" << std::endl;
    std::cout << "         -----> Kakaovej drti       : " << pocetKakovejDrtiKg  << "kg/h <-----" << std::endl;
    std::cout << "         -----> Odpadu skrupin      : " << pocetOdpaduSkrupinKg  << "kg/h <-----" << std::endl;
    std::cout << "         -----> Kakaoveho likeru    : " << pocetKakaovehoLikeruKg  << "kg/h <-----" << std::endl;
    std::cout << "         -----> Liker na lisovanie  : " << pocetLikeruNaLisovanie  << "kg/h <-----" << std::endl;
    std::cout << "         -----> Liker ako surovina  : " << pocetLikeruAkoSurovina  << "kg/h <-----" << std::endl;
    std::cout << "         -----> Kakaovy prasok      : " << pocetKakaovehoPrasku  << "kg/h <-----" << std::endl;
    std::cout << "         -----> Kakaove maslo       : " << pocetKakaovehoMasla  << "kg/h <-----" << std::endl;
    std::cout << "         -----> Cokolady            : " << pocetCokoladyKg  << "kg/h <-----" << std::endl;
    std::cout << "         -----> Konšovaná Cokolada  : " << pocetKonsovanejCokoladyKg  << "kg/h <-----" << std::endl;
    std::cout << "         -----> Formovaná Cokolada  : " << pocetFormovanychCokolad  << "kg/h <-----" << std::endl;
    std::cout << "         -----> Temperovana Cokolada: " << pocetFormovanychCokolad  << "kg/h <-----" << std::endl;
    std::cout << "         -----> Celkovy cas         : " << (konieSimulacie /3600)   << "h <-----" <<  std::endl;

    return 0;
}