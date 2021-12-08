#include <simlib.h>
#include <iostream>
#include <vector>
#include "arguments.hpp"
#include "car.hpp"
#include "station.hpp"

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
    Arguments::kilometersToWork = result["V"].as<double>();
    Arguments::kilometersToStation = result["B"].as<double>();
    auto workMinutes = result["R"].as<std::vector<double>>();
    Arguments::workMinutesMean = workMinutes[0];
    Arguments::workMinutesDispersion = workMinutes[1];
    Arguments::stationFill = result["D"].as<double>();
    Arguments::tankingTimes = result["T"].as<std::vector<double>>();
    Arguments::nightChargeProbability = result["C"].as<double>();

    std::size_t totalStationCount = result["P"].as<int>();
    auto individialPlacesCount = result["Z"].as<std::vector<int>>();
    for (std::size_t i = 0; i < totalStationCount; i++)
    {
        Arguments::stations.push_back(Station(individialPlacesCount));
    }

    auto carsCount = result["A"].as<std::vector<int>>();
    auto tankSizes = result["N"].as<std::vector<double>>();

    //simlib3::DebugON();
    simlib3::SetOutput("output.txt");

    std::vector<CitizenCar *> cars;
    for (int c = 0; c < 4; c++)
    {
        for (int i = 0; i < carsCount[c]; i++)
        {
            double tankSize = simlib3::Normal(tankSizes[c * 2], tankSizes[c * 2 + 1]);
            double generatedFuel = simlib3::Uniform(Arguments::controlLightLevel * tankSize, tankSize);
            auto newCar = new CitizenCar(generatedFuel, tankSize, intToFuelType[c]);
            cars.push_back(newCar);
            newCar->Activate();
        }
    }

    auto tripPeriods = result["Y"].as<std::vector<double>>();
    for (int i = 0; i < 4; i++)
    {
        (new TravellerCarGenerator(intToFuelType[i], tripPeriods[i]))->Activate();
    }
    simlib3::Print("Generated %d cars.\n", cars.size());

    simlib3::Init(0, result["M"].as<double>());
    simlib3::Run();
    simlib3::SIMLIB_statistics.Output();
}

unsigned int stdStr2intHash(std::string str, int h)
{
    return !str.c_str()[h] ? 5381 : (str2intHash(str.c_str(), h + 1) * 33) ^ str.c_str()[h];
}