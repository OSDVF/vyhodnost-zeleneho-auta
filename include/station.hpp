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
    int number;
    int placesCount[4] = {0,0,0,0};
public:
    std::vector<StationPlace> places;

    void addPlaces(std::vector<int> placesCount);
    void printPlacesInfo();
    Station();
};