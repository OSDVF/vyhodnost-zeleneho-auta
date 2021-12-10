#include "arguments.hpp"

double Arguments::controlLightLevel;
double Arguments::dayMinutesLength;
double Arguments::workMinutesMean;
double Arguments::workMinutesDispersion;
std::vector<double> Arguments::stationFill;
double Arguments::refuelTime;
double Arguments::kilometersToWork;
double Arguments::kilometersToWorkDeviation;
double Arguments::nightChargeProbability;
//double Arguments::kilometersToStation;
double Arguments::stupidity;
std::vector<Station> Arguments::stations;
std::vector<double> Arguments::tankingTimes;
std::vector<int> Arguments::carsCount;
int Arguments::totalCars = 0;
std::unordered_map<FuelType, int> Arguments::fuelTypeToInt;

cxxopts::Options Arguments::setupArguments()
{
    cxxopts::Options options("Výhodnost zeleného auta", "Pokud chce EU splnit green deal v roce 2050, kolik bude třeba čerpacích a nabíjecích stanic pro jednotlivá paliva a jak moc se různé paliva ekonomicky vyplatí pro konkrétního jednotlivce, aby byl splněn limit emisí o 55% menších, než v roce 1990");
    options.add_options("Klasika")
        ("P,Pumpy", "Počet čerpacích stanic", cxxopts::value<int>()->default_value("7"))
        ("V,Vzdalenost", "Vzdálenost do práce (km) (střední hodnota, směrodatná odchylka)", cxxopts::value<std::vector<double>>()->default_value("15,5"))
        // https://www.eea.europa.eu/data-and-maps/daviz/co2-emission-intensity-9/#tab-googlechartid_googlechartid_googlechartid_googlechartid_chart_11111
        // 118 g of CO2e per kWh by electricity plants
        // https://www.virta.global/blog/ev-charging-101-how-much-electricity-does-an-electric-car-use
        // electric car consumes 0.20 kWh per km
        // 188 * 0.2 = 23,6
        
        // for fossil fuel cars: https://www.eea.europa.eu/ims/co2-performance-of-new-passenger
        // fuel consumtion: https://www.iea.org/reports/fuel-consumption-of-cars-and-vans
        // (for computing indirect emisions) and https://innovationorigins.com/en/producing-gasoline-and-diesel-emits-more-co2-than-we-thought/
        // says 720 g/l of petrol and 640 g/l of diesel
        // consumption target by IEA Net zero Scenarion: 3.1l/100km
        // indirect emissions = 720*0.031 = 22,32g/km for petrol
        // and                  640*0.031 = 19.84g/km for diesel

        // direct fossil fuel emission targets are from https://www.eea.europa.eu/ims/co2-performance-of-new-passenger

        // For Hydrogen:
        // https://www.forbes.com/sites/rrapier/2020/06/06/estimating-the-carbon-footprint-of-hydrogen-production/
        // 9.3g CO2 per 1g of H
        // 8.6g car consumption per 1km average
        // 8.6*9.3 = 79.98 g per 1 km
        // https://h2.live/en/fahren/
        ("E,Emise","Emise jednotlivých typů paliv v gramech CO2 kilomtr B přímé,B nepřímé,N přímé,N nepřímé,E nepřímé,V nepřímé", cxxopts::value<std::vector<double>>()->default_value("59.4,22.32,59.4,19.84,23.6,79.98"))
        //("B,VzdalenostStanice", "Vzdálenost na stanici", cxxopts::value<double>()->default_value("20"))
        ("Z,Zastavky","Celkový počet na všech stanicích B,N,E,V", cxxopts::value<std::vector<int>>()->default_value("14,14,23,1"))//https://www.blesk.cz/clanek/regiony-brno-brno-zpravy/682491/stovky-dobijecek-pro-elektromobily-v-brne-nasavat-tu-mohou-i-prespolni.html
        ("T,Tankovani", "Doba tankování (min) plné nádrže jednotlivých paliv B,N,E,V", cxxopts::value<std::vector<double>>()->default_value("15,15,60,4"))
        ("A,Auta","Počet aut různého typu paliva B,N,E,V", cxxopts::value<std::vector<int>>()->default_value("17858,5250,10936,955"))
        ("N,Nadrz","Dojezd na jednu nádrž pro různé auta (střední hodnota, rozptyl) Bs,Br,Ns,Nr,Es,Er,Vs,Vr",cxxopts::value<std::vector<double>>()->default_value("482,50,1000,100,300,50,650,80"))
        ("D,Doplneni","Perioda doplňování paliva na pumpách (střední hodnota, rozptyl)",cxxopts::value<std::vector<double>>()->default_value("4320,1440"/*once a 3 days*/))
        ("Y,Vylet","Počet generovaných výletníků za minutu",cxxopts::value<double>()->default_value("0.47916"))
        ("M,Minuty","Doba běhu simulace", cxxopts::value<double>()->default_value("10080"/* week */));
    options.add_options("Pokročilé")
        ("R,RefuelTime","Přesná doba poteřbná na doplnění paliva z příchozí cisterny (min)", cxxopts::value<double>()->default_value("30"))
        ("L,Blbost", "Pravděpodobnost, že je člověk vlastnící elektromobil jej nikdy nebude nabíjet doma", cxxopts::value<double>()->default_value("0.0"))
        ("C,Pravdepodobnost","Pravděpodobnost, že člověk dá svůj elektromobil nabít přes noc", cxxopts::value<double>()->default_value("0.9"))
        ("K,Kontrolka","Kdy se rozsvítí kontrolka, že by auto mělo jet tankovat (% zbylého paliva v nádrži)", cxxopts::value<double>()->default_value("10"))
        ("H,Den","Délka dne v minutách", cxxopts::value<double>()->default_value("1440"))
        ("W,Work","pRacovní doba v minutách (střední hodnota, směrodatná odchylka)", cxxopts::value<std::vector<double>>()->default_value("480,60"));
    return options;
}