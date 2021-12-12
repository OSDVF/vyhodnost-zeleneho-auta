#pragma once
#include "cxxopts.hpp"
#include "station.hpp"
constexpr const char *intToFuelString[] = {
    "Petrol",
    "Diesel",
    "Electric",
    "Hydrogen"};

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
    static std::vector<double> stationFill;
    static double refuelTime;
    static double kilometersToWork;
    static double kilometersToWorkDeviation;
    //static double kilometersToStation;
    static double differenceBetweenWorkBegin;
    static double nightChargeProbability;
    static double stupidity;
    static int totalCars;
    static std::vector<int> carsCount;
    static std::vector<Station> stations;
    static std::vector<double> tankingTimes;
    static std::unordered_map<FuelType, int> fuelTypeToInt;

    static cxxopts::Options setupArguments();
};
