#pragma once
#include "simlib.h"
#include "car.hpp"
#include <vector>
struct StationPlace
{
public:
    FuelType fuelType;

    StationPlace(FuelType fuelType);
    simlib3::Facility* getTankQueue();
private:
    simlib3::Facility* tankingQueue = nullptr;
};

class Station
{
public:
    std::vector<StationPlace> places;

    Station(std::vector<int> placesCount);
    Station() {}
};