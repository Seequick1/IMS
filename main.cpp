#include <iostream>
#include <cstring>                                  // parsovanie argumentov
#include "simlib.h"

/** GLOBALNE PREMENNE **/
#define POCET_LINIEK_PRE_CISTENIE 3                 // pocet liniek na cistenie
#define POCET_LINIEK_PRE_PRAZENICIE 3               // pocet liniek na prazenie
#define POCET_DRVICIEK 1                            // pocet drviciek
#define POCET_MLETICEK 2                            // pocet mlynov
#define POCET_HYDRAULICKYCH_LISOV 5                 // pocet hydraulickych lisov
#define POCET_MIESACOK 6                            // pocet miesacok
#define POCET_KONSOVACICH_STROJOV 16                // pocet konsovacich strojov
#define POCET_FORMOVACICH_LINIEK 2                  // pocet liniek na formovanie

#define KAPACITA_PODRVENYCH_SKLAD 2000              // velkost kapacity podrvenych
#define KAPACITA_PRAZENYCH_SKLAD 2000               // velkost kapacity prazenych
#define KAPACITA_VSEOBECNEHO_SKLADU 400000           // velkost vseobecneho skladu
#define VYSTUP_PRAZENIA 250                         // za jednotlivy pas 1000kg bobov

#define MINUTA 60                                   // minuta
#define HODINA MINUTA * 60                          // hodina
#define POLHODINA 60 * 30                           // pol hodina
#define DEN HODINA * 24                             // den
#define TYZDEN DEN * 7                              // tyzden
#define MESIAC DEN * 30                             // mesiac
#define ROK MESIAC * 12                             // rok

Store Cisticky("Cisticky", POCET_LINIEK_PRE_CISTENIE);              // 3 linky pre cistenie
Store Prazenicky("Prazenie", POCET_LINIEK_PRE_PRAZENICIE);          // 3 linky pre prazenie
Store Drvicky("Drvicky", POCET_DRVICIEK);                           // 1 linka pre drtenie
Store Mleticky("Mleticky", POCET_MLETICEK);                         // 2 mlynceky
Store HydraulickyLis("HydraulickyLis", POCET_HYDRAULICKYCH_LISOV);  // 5 hydraulickych lisov
Store Miesacky("Miesacky", POCET_MIESACOK);                         // 6 miesacok
Store KonsovaciStroj("KonsovaciStroj", POCET_KONSOVACICH_STROJOV);    // 16 konsovacich strojov (primarne)
Store Formovacky("Formovacky", POCET_FORMOVACICH_LINIEK);           // 2 linky pre formovanie cokolady

Store SkladPrazenichBobov("Sklad prazenich bôbov", KAPACITA_PRAZENYCH_SKLAD);               // sklad prazenych bobov
Store SkladPodrvenychBobov("Sklad podrvených bôbov", KAPACITA_PODRVENYCH_SKLAD);            // sklad podrvenych bobov
Store SkladKakaovehoLikeru("Sklad kakaoveho likeru", KAPACITA_VSEOBECNEHO_SKLADU);          // sklad kakaovych lirkov
Store SkladKakaovejDrti("Sklad kakaovej drti", KAPACITA_VSEOBECNEHO_SKLADU);                // sklad kakaovej drti
Store SkladLikeruNaLisovanie("Sklad likeru na lisovanie", KAPACITA_VSEOBECNEHO_SKLADU);     // sklad likeru
Store SkladLikeruNaAkoSurovina("Sklad likeru ako surovina", KAPACITA_VSEOBECNEHO_SKLADU);   // sklad suroviny
Store SkladKakaovehoMasla("Sklad kakaoveho masla", KAPACITA_VSEOBECNEHO_SKLADU);            // sklad masla
Store SkladCokolady("Sklad cokolady", KAPACITA_VSEOBECNEHO_SKLADU);                         // sklad cokolady
Store SkladKonsovanejCokolady("Sklad konsovanej cokolady", KAPACITA_VSEOBECNEHO_SKLADU);    // sklad konsovanej cokolady
Store SkladFormovanejCokolady("Sklad formovanej cokolady", KAPACITA_VSEOBECNEHO_SKLADU);    // sklad formovanej cokolady

unsigned int pocetOdpaduSkrupinKg = 0;          // celkovy odpadu skrupin v kg
unsigned int pocetKakovejDrtiKg = 0;            // celkovy kakaovej drti v kg
unsigned int pocetKakaovehoLikeruKg = 0;        // celkovy kakaovej kakaoveho likeru v kg
unsigned int pocetLikeruNaLisovanie = 0;        // celkovy liker na lisovanie
unsigned int pocetLikeruAkoSurovina = 0;        // celkovy liker ako surovina
unsigned int pocetKakaovehoPrasku = 0;          // celkovy pocet kakaoveho prasku
unsigned int pocetKakaovehoMasla = 0;           // celkovy pocet kakaoveho masla
unsigned int pocetCokoladyKg = 0;               // celkovy pocet cokolady v kg
unsigned int pocetKonsovanejCokoladyKg = 0;     // celkovy pocet konsovanej cokolady v kg
unsigned int pocetFormovanychCokolad = 0;       // celkovy pocet formovanych cokolad v kg
unsigned int pocetTemperovanychCokolad = 0;     // celkovy pocet temperovanych cokolad v kg

double zaciatokSimulacie = 0;                   // zaciatok simulacie
double konieSimulacie = 0;                      // koniec simulacie

bool jeRozbehnuty = false;

/* PROCES CISTENIE */
class Cistenie : public Process {
    void Behavior(){
        // proces cistenia na kazdu linku
        if (Cisticky.Free()) {
            Enter(Cisticky, 1);
            Wait(POLHODINA);
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
                Enter(SkladPrazenichBobov, VYSTUP_PRAZENIA);
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
            Leave(SkladKakaovejDrti, 350);

            // pridanie 350 likeru v kg
            pocetKakaovehoLikeruKg += 350;
            Enter(SkladKakaovehoLikeru, 350);
            Leave(Mleticky, 1);
        }
        if(SkladKakaovehoLikeru.Used() > 350){
            // odpocitanie od skladky 350kg likeru
            Leave(SkladKakaovehoLikeru, 350);
            pocetLikeruNaLisovanie += 108;
            pocetLikeruAkoSurovina += 242;
//            vznikne 108kg likeru na lisovanie
            Enter(SkladLikeruNaLisovanie, 108);
            Enter(SkladLikeruNaAkoSurovina, 242);
        }
    }
};

///* PROCES STLACANIE */
class Stlacanie : public Process {
    void Behavior(){
        if(HydraulickyLis.Free() && SkladLikeruNaLisovanie.Used() >= 108 ){
            Enter(HydraulickyLis, 1);
            // zobereme si likeru
            Leave(SkladLikeruNaLisovanie, 108);
            // pridanie kakaoveho prasku a masla
            pocetKakaovehoPrasku += 53;
            pocetKakaovehoMasla += 55;
            Enter(SkladKakaovehoMasla, 55);
            Leave(HydraulickyLis, 1);
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

/* GENERATOR CISTENIA */
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

/* GENERATOR PRAZENIA */
class GeneratorPrazenia : public Event {
    void Behavior(){
        // trikrat spustame proces prazenie imitovanie 3 Liniek
        (new Prazenie)->Activate();
        (new Prazenie)->Activate();
        (new Prazenie)->Activate();
	// kazdych 30 az 35 min
        Activate(Time + Uniform(POLHODINA, 60*35));
    }
};

/* GENERATOR DRVANIA */
class GeneratorDrvenia : public Event {
    void Behavior(){
        (new Drvenie)->Activate();
        // kazdych 10 az 15 minut
        Activate(Time + Uniform(60*10, 60*15));
    }
};

/* GENERATOR MLETIA */
class GeneratorMletia : public Event {
    void Behavior(){
        (new Mletie)->Activate();
        (new Mletie)->Activate();
        // kazdych 6 minut
        Activate(Time + 60*6);
    }
};

/* GENERATOR STLACANIA */
class GeneratorStlacania : public Event {
    void Behavior(){
        // vytvorenie naraz 5 paralenych lisov
        (new Stlacanie)->Activate();
        (new Stlacanie)->Activate();
        (new Stlacanie)->Activate();       
        (new Stlacanie)->Activate();
        (new Stlacanie)->Activate();
        // kazdych od 18 po 22 min
        Activate(Time + Uniform(60 * 15, 60 * 20));
    }
};

/* GENERATOR MIXOVANIA */
class GeneratorMixovania : public Event {
    void Behavior(){
    	// vytvorenie naraz 6 paralenych mixerov
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

/* GENERATOR KONSOVANIA */
class GenerovanieKonsovania : public Event {
    void Behavior(){
    	// 16 strojov (je mozne ich pocet menit)
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

/* GENERATOR FORMOVANIA */
class GeneratorFormovania : public Event{
    void Behavior(){
        (new Formovanie)->Activate();
        (new Formovanie)->Activate();
        // 25 min
        Activate(Time + 25*60);
    }
};

/* GENERATOR TEMPEROVANIA */
class GeneratorTemperovania : public Event {
    void Behavior(){
        (new Temperovanie)->Activate();
        // 35 - 40 min
        Activate(Time + Uniform(60 *35, 60*40));
    }
};

int main(int argc, char* argv[]) {
    // TYZDEN, MESIAC, ROK
    Init(0, TYZDEN);
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
//    KonsovaciStroj.Output();
//    Formovacky.Output();
    konieSimulacie = Time - zaciatokSimulacie;

    std::cout << "|------------------SPECIFIC DATA-----------------|"<< std::endl;
    std::cout << "|------------------------------------------------|"<< std::endl;
    std::cout << "|-----> Kakovej Drti + Odpad: |" << pocetKakovejDrtiKg + pocetOdpaduSkrupinKg  << "kg|" << std::endl;
    std::cout << "|-----> Kakaovej drti       : |" << pocetKakovejDrtiKg  << "kg|" << std::endl;
    std::cout << "|-----> Odpadu skrupin      : |" << pocetOdpaduSkrupinKg  << "kg|" << std::endl;
    std::cout << "|-----> Kakaoveho likeru    : |" << pocetKakaovehoLikeruKg  << "kg|" << std::endl;
    std::cout << "|-----> Liker na lisovanie  : |" << pocetLikeruNaLisovanie  << "kg|" << std::endl;
    std::cout << "|-----> Liker ako surovina  : |" << pocetLikeruAkoSurovina  << "kg|" << std::endl;
    std::cout << "|-----> Kakaovy prasok      : |" << pocetKakaovehoPrasku  << "kg|" << std::endl;
    std::cout << "|-----> Kakaove maslo       : |" << pocetKakaovehoMasla  << "kg|" << std::endl;
    std::cout << "|-----> Cokolady            : |" << pocetCokoladyKg  << "kg|" << std::endl;
    std::cout << "|-----> Konšovaná Cokolada  : |" << pocetKonsovanejCokoladyKg  << "kg|" << std::endl;
    std::cout << "|-----> Formovaná Cokolada  : |" << pocetFormovanychCokolad  << "kg|" << std::endl;
    std::cout << "|------------------------------------------------|"<< std::endl;
    std::cout << "|-----> Temperovana Cokolada: |" << pocetFormovanychCokolad  << "kg|" << std::endl;
    std::cout << "|------------------------------------------------|"<< std::endl;
    std::cout << "|-----> Celkovy cas         : |" << (konieSimulacie /3600)   << "h|" <<  std::endl;
    std::cout << "|------------------------------------------------|"<< std::endl;

    return 0;
}
