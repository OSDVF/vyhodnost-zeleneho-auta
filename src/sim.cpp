#include <simlib.h>
#include <iostream>
#include <vector>
#include "arguments.hpp"
#include "car.hpp"
#include "station.hpp"
#include "statistics.hpp"

unsigned int stdStr2intHash(std::string str, int h = 0);
constexpr unsigned int str2intHash(const char *str, int h = 0)
{
    return !str[h] ? 5381 : (str2intHash(str, h + 1) * 33) ^ str[h];
}

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        switch (str2intHash(argv[1]))
        {

        case str2intHash("h"):
            std::cout << Arguments::setupArguments().help();
        }
    }

    auto result = Arguments::setupArguments().parse(argc, argv);
    Arguments::controlLightLevel = result["K"].as<double>() / 100.0f;
    Arguments::dayMinutesLength = result["H"].as<double>();
    auto kToW = result["V"].as<std::vector<double>>();
    Arguments::kilometersToWork = kToW[0];
    Arguments::kilometersToWorkDeviation = kToW[1];
    //Arguments::kilometersToStation = result["B"].as<double>();
    auto workMinutes = result["W"].as<std::vector<double>>();
    Arguments::workMinutesMean = workMinutes[0];
    Arguments::workMinutesDispersion = workMinutes[1];
    Arguments::stationFill = result["D"].as<std::vector<double>>();
    Arguments::refuelTime = result["R"].as<double>();
    Arguments::tankingTimes = result["T"].as<std::vector<double>>();
    Arguments::nightChargeProbability = result["C"].as<double>();
    Arguments::stupidity = result["L"].as<decltype(Arguments::stupidity)>();
    Arguments::fuelTypeToInt = std::unordered_map<FuelType, int>({{FuelType::Petrol, 0},
                                                                  {FuelType::Diesel, 1},
                                                                  {FuelType::Electric, 2},
                                                                  {FuelType::Hydrogen, 3}});
    auto emissionRates = result["E"].as<std::vector<double>>();
    Statistics::directEmissionForFuel[0] = emissionRates[0];
    Statistics::directEmissionForFuel[1] = emissionRates[2];
    Statistics::indirectEmissionForFuel[0] = emissionRates[1];
    Statistics::indirectEmissionForFuel[1] = emissionRates[3];
    Statistics::indirectEmissionForFuel[2] = emissionRates[4];
    Statistics::indirectEmissionForFuel[3] = emissionRates[5];

    int totalStationCount = result["P"].as<int>();
    auto individialPlacesCount = result["Z"].as<std::vector<int>>();
    int totalPlacesCount = 0;
    Arguments::stations = std::vector<Station>(totalStationCount);
    for (auto p : individialPlacesCount)
    {
        totalPlacesCount += p;
    }

    // Like card giving process (each station gets max one on each station type)
    int currentStation = 0;
    int notYetGivenPlaces = totalPlacesCount;
    while (notYetGivenPlaces > 0)
    {
        std::vector<int> currentCount = {0, 0, 0, 0};
        for (int i = 0; i < 4; i++)
        {
            if (individialPlacesCount[i] > 0)
            {
                currentCount[i]++;
                individialPlacesCount[i]--;
                notYetGivenPlaces--;
            }
        }
        Arguments::stations[currentStation++ % totalStationCount].addPlaces(currentCount);
    }
    for (int i = 0; i < totalStationCount; i++)
    {
        Arguments::stations[i].printPlacesInfo();
    }

    auto carsCount = Arguments::carsCount = result["A"].as<std::vector<int>>();
    auto tankSizes = result["N"].as<std::vector<double>>();

    //simlib3::DebugON();
    simlib3::Init(0, result["M"].as<double>());

    std::vector<CitizenCar *> cars;
    for (int c = 0; c < 4; c++)
    {
        for (int i = 0; i < carsCount[c]; i++)
        {
            double tankSize = simlib3::Normal(tankSizes[c * 2], tankSizes[c * 2 + 1]);
            double generatedFuel = simlib3::Uniform(Arguments::controlLightLevel * tankSize, tankSize);
            auto newCar = new CitizenCar();
            newCar->Create(generatedFuel, tankSize, intToFuelType[c]);
            cars.push_back(newCar);
            newCar->Activate();
            Arguments::totalCars++;
        }
    }
    simlib3::Print("Generated %d cars.\n", cars.size());

    auto travellersPerMinute = result["Y"].as<double>();
    auto gen = new TravellerCarGenerator;
    gen->Create(travellersPerMinute);
    gen->Activate();

    auto dayCounterGen = new DayGenerator;
    dayCounterGen->Create();
    dayCounterGen->Activate();

    for (auto s : Arguments::stations)
    {
        auto refillGen = new RefuelTheStationGenerator;
        refillGen->Create(s);
        refillGen->Activate();
    }

    simlib3::Run();
    for (auto s : Arguments::stations)
    {
        s.printStatistics();
    }
    simlib3::SIMLIB_statistics.Output();
}

unsigned int stdStr2intHash(std::string str, int h)
{
    return !str.c_str()[h] ? 5381 : (str2intHash(str.c_str(), h + 1) * 33) ^ str.c_str()[h];
}