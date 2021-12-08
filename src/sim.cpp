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
            std::cout << setupArguments().help();
        }
    }

    auto result = setupArguments().parse(argc, argv);
    controlLight = result["K"].as<double>() / 100.0f;
    dayMinutesLength = result["H"].as<double>();
    auto workMinutes = result["R"].as<std::vector<double>>();
    workMinutesMean = workMinutes[0];
    workMinutesDispersion = workMinutes[1];
    stationFill = result["D"].as<double>();

    std::size_t totalStationCount = result["P"].as<int>();
    auto individialPlacesCount = result["Z"].as<std::vector<int>>();
    std::vector<Station> stations;
    for (std::size_t i = 0; i < totalStationCount; i++)
    {
        stations.push_back(Station(individialPlacesCount));
    }

    auto carsCount = result["A"].as<std::vector<int>>();
    auto tankSizes = result["N"].as<std::vector<double>>();
    const FuelType intToType[] = {
        FuelType::Petrol,
        FuelType::Diesel,
        FuelType::Electric,
        FuelType::Hydrogen};

    //simlib3::DebugON();
    simlib3::SetOutput("output.txt");
    
    std::vector<CitizenCar *> cars;
    for (int c = 0; c < 4; c++)
    {
        for (int i = 0; i < carsCount[c]; i++)
        {
            double tankSize = simlib3::Normal(tankSizes[c * 2], tankSizes[c * 2 + 1]);
            double generatedFuel = simlib3::Uniform(controlLight * tankSize, tankSize);
            cars.push_back(new CitizenCar(generatedFuel, tankSize, intToType[c]));
        }
    }

    simlib3::Print("Generated %d cars\n.", cars.size());

    simlib3::Init(0, result["D"].as<double>());

    simlib3::Run();
    simlib3::SIMLIB_statistics.Output();
}

unsigned int stdStr2intHash(std::string str, int h)
{
    return !str.c_str()[h] ? 5381 : (str2intHash(str.c_str(), h + 1) * 33) ^ str.c_str()[h];
}