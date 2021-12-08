#pragma once
#include "cxxopts.hpp"
#include "station.hpp"

constexpr FuelType intToFuelType[] = {
    FuelType::Petrol,
    FuelType::Diesel,
    FuelType::Electric,
    FuelType::Hydrogen};

class Arguments
{
public:
    static double controlLightLevel;
    static double dayMinutesLength;
    static double workMinutesMean;
    static double workMinutesDispersion;
    static double stationFill;
    static double kilometersToWork;
    static double kilometersToStation;
    static std::vector<Station> stations;
    static std::vector<double> tankingTimes;

    static cxxopts::Options setupArguments();
};
