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

    std::vector<int> stationCounts;
    stationCounts = result["P"].as<decltype(stationCounts)>();

    std::size_t totalStationCount = 0;
    for (auto stationCount : stationCounts)
    {
        totalStationCount += stationCount;
    }
    /*int remPetrolStat = stationCounts[0];
    int remDieselStat = stationCounts[1];
    int remElStat = stationCounts[2];
    int remHyStat = stationCounts[3];*/
    auto individialPlacesCount = result["Z"].as<std::vector<int>>();
    std::vector<Station> stations;
    for(std::size_t i = 0; i < totalStationCount;i++)
    {
        stations.push_back(Station(individialPlacesCount));
    }

    simlib3::DebugON();
    simlib3::SetOutput("output.txt");

    simlib3::Init(0, result["D"].as<double>());
}

unsigned int stdStr2intHash(std::string str, int h)
{
    return !str.c_str()[h] ? 5381 : (str2intHash(str.c_str(), h + 1) * 33) ^ str.c_str()[h];
}