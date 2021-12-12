#pragma once
#include "simlib.h"
#include "car.hpp"
#include <vector>
#include <array>

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
    int placesCount[4] = {0,0,0,0};
public:
    int thisDayCustomers = 0;
    int number;
    bool isGettingRefueled;
    std::vector<StationPlace> places;

    void addPlaces(std::vector<int> placesCount);
    void printPlacesInfo();
    void printStatistics();
    std::array<int, 4> queueLengthsByFuel();
    Station();
};

class RefuelStation : public simlib3::Process
{
public:
    Station stationToRefuel;
    void Create(Station station);
    void Behavior();
};

class RefuelTheStationGenerator : public simlib3::Event
{
public:
    Station stationToRefuel;
    void Create(Station station);
    void Behavior();
};