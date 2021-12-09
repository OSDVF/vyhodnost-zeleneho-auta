#include "arguments.hpp"

double Arguments::controlLightLevel;
double Arguments::dayMinutesLength;
double Arguments::workMinutesMean;
double Arguments::workMinutesDispersion;
double Arguments::stationFill;
double Arguments::kilometersToWork;
double Arguments::nightChargeProbability;
double Arguments::kilometersToStation;
std::vector<Station> Arguments::stations;
std::vector<double> Arguments::tankingTimes;

cxxopts::Options Arguments::setupArguments()
{
    cxxopts::Options options("Výhodnost zeleného auta", "Pokud chce EU splnit green deal v roce 2050, kolik bude třeba čerpacích a nabíjecích stanic pro jednotlivá paliva a jak moc se různé paliva ekonomicky vyplatí pro konkrétního jednotlivce, aby byl splněn limit emisí o 55% menších, než v roce 1990");
    options.add_options("Klasika")
        ("P,Pumpy", "Počet čerpacích stanic", cxxopts::value<int>()->default_value("40"))
        ("V,Vzdalenost", "Vzdálenost do práce (km)", cxxopts::value<double>()->default_value("5"))
        ("B,VzdalenostStanice", "Vzdálenost na stanici", cxxopts::value<double>()->default_value("5"))
        ("Z,Zastavky","Počet zastávek na jedné čerpací stanici B,N,E,V", cxxopts::value<std::vector<int>>()->default_value("4,4,5,5"))
        ("T,Tankovani", "Doba tankování (min) plné nádrže jednotlivých paliv B,N,E,V", cxxopts::value<std::vector<double>>()->default_value("2,2,20,1"))
        ("A,Auta","Počet aut různého typu paliva B,N,E,V", cxxopts::value<std::vector<int>>()->default_value("87500,17500,6125,1225"))
        ("N,Nadrz","Dojezd na jednu nádrž pro různé auta (střední hodnota, rozptyl) Bs,Br,Ns,Nr,Es,Er,Vs,Vr",cxxopts::value<std::vector<double>>()->default_value("482,50,1000,100,300,50,650,80"))
        ("D,Doplneni","Perioda doplňování paliva na pumpách (střední hodnota)",cxxopts::value<double>()->default_value("4320"))
        ("Y,Vylet","Perioda generování výletníků pro auta různých typů B,N,E,V",cxxopts::value<std::vector<double>>()->default_value("120,120,120,120"))
        ("M,Minuty","Doba běhu simulace", cxxopts::value<double>()->default_value("10080"/* week */));
    options.add_options("Pokročilé")
        ("C,Pravdepodobnost","Pravděpodobnost, že člověk dá svůj elektromobil nabít přes noc", cxxopts::value<double>()->default_value("0.9"))
        ("K,Kontrolka","Kdy se rozsvítí kontrolka, že by auto mělo jet tankovat (% zbylého paliva v nádrži)", cxxopts::value<double>()->default_value("10"))
        ("H,Den","Délka dne v minutách", cxxopts::value<double>()->default_value("1440"))
        ("R,Prace","pRacovní doba v minutách (střední hodnota, rozptyl)", cxxopts::value<std::vector<double>>()->default_value("480,60"));
    return options;
}