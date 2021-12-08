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
    options.add_options("Povinné")
        ("P,Pumpy", "Počet čerpacích stanic", cxxopts::value<int>())
        ("V,Vzdalenost", "Vzdálenost do práce (km)", cxxopts::value<double>()->default_value("5"))
        ("B,VzdalenostStanice", "Vzdálenost na stanici", cxxopts::value<double>()->default_value("5"))
        ("Z,Zastavky","Počet zastávek na jedné čerpací stanici (střední hodnota, rozptyl) Bs,Br,Ns,Nr,Es,Er,Vs,Vr", cxxopts::value<std::vector<int>>()->default_value("4,2,4,2,5,2,5,2"))
        ("T,Tankovani", "Doba tankování (min) plné nádrže jednotlivých paliv B,N,E,V", cxxopts::value<std::vector<double>>())
        ("A,Auta","Počet aut různého typu paliva B,N,E,V", cxxopts::value<std::vector<int>>())
        ("N,Nadrz","Dojezd na jednu nádrž pro různé auta (střední hodnota, rozptyl) Bs,Br,Ns,Nr,Es,Er,Vs,Vr",cxxopts::value<std::vector<double>>())
        ("D,Doplneni","Perioda doplňování paliva na pumpách (střední hodnota)",cxxopts::value<double>()->default_value("480"))
        ("Y,Vylet","Perioda generování výletníků pro auta různých typů B,N,E,V",cxxopts::value<std::vector<double>>())
        ("M,Minuty","Doba běhu simulace", cxxopts::value<double>());
    options.add_options("Volitelné")
        ("C,Pravdepodobnost","Pravděpodobnost, že člověk dá svůj elektromobil nabít přes noc", cxxopts::value<double>()->default_value("0.9"))
        ("K,Kontrolka","Kdy se rozsvítí kontrolka, že by auto mělo jet tankovat (% zbylého paliva v nádrži)", cxxopts::value<double>()->default_value("10"))
        ("H,Den","Délka dne v minutách", cxxopts::value<double>()->default_value("1440"))
        ("R,Prace","pRacovní doba v minutách (střední hodnota, rozptyl)", cxxopts::value<std::vector<double>>()->default_value("480,60"));
    return options;
}