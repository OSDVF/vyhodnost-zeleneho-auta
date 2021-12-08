#include "cxxopts.hpp"

cxxopts::Options setupArguments()
{
    cxxopts::Options options("Výhodnost zeleného auta", "Pokud chce EU splnit green deal v roce 2050, kolik bude třeba čerpacích a nabíjecích stanic pro jednotlivá paliva a jak moc se různé paliva ekonomicky vyplatí pro konkrétního jednotlivce, aby byl splněn limit emisí o 55% menších, než v roce 1990");
        options.add_options("Povinné")
            ("P,Pumpy", "Počet čerpacích stanic Benzín,Nafta,Elektro,Vodík", cxxopts::value<std::vector<int>>())
            ("T,Tankovani", "Doba tankování (min) plné nádrže jednotlivých paliv B,N,E,V", cxxopts::value<std::vector<float>>())
            ("K,Kontrolka","Kdy se rozsvítí kontrolka, že by auto mělo jet tankovat (% zbylého paliva v nádrži)", cxxopts::value<float>())
            ("N,Nadrz","Velikost nádrže od,do", cxxopts::value<std::vector<float>>()->default_value("45,60"))//https://www.rockethq.com/learn/personal-finances/how-much-does-it-cost-you-to-fill-up
            ("A,Auta","Počet aut různého typu paliva B,N,E,V", cxxopts::value<std::vector<int>>())
            ("S,Spotreba","Spotřeba paliva pro různé auta (střední hodnota, rozptyl) Bs,Br,Ns,Nr,Es,Er,Vs,Vr",cxxopts::value<std::vector<float>>())
            ("D,Doplneni","Perioda doplňování paliva na pumpách (střední hodnota)",cxxopts::value<float>())
            ("M,Minuty","Doba běhu simulace", cxxopts::value<double>());
        options.add_options("Volitelné")
            ("H,Den","Délka dne v minutách", cxxopts::value<double>()->default_value("1440"))
            ("R,Prace","pRacovní doba v minutách (střední hodnota, rozptyl)", cxxopts::value<std::vector<double>>()->default_value("480,60"));
        return options;
}