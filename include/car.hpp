#pragma once
#include <simlib.h>

enum class FuelType
{
    Hydrogen = 1,
    Electric = 2,
    Petrol = 4,
    Diesel = 8
};

inline FuelType operator|(FuelType a, FuelType b)
{
    return static_cast<FuelType>(static_cast<int>(a) | static_cast<int>(b));
}

class CitizenCar : public simlib3::Process
{
public:
    //should be set at start
    double fuel;
    double tankSize;
    FuelType fuelTypes;

    //varying
    double _elapsedDayMinutes = 0;
    CitizenCar(double fuel, double tankSize, FuelType fuelTypes)
        : fuel(fuel), tankSize(tankSize), fuelTypes(fuelTypes) {}

    void Behavior();
};

class TravellerCar : public simlib3::Process
{
public:
    FuelType fuelTypes;
    double tankSize;
    TravellerCar(FuelType fuelTypes, double tankSize) : fuelTypes(fuelTypes),tankSize(tankSize) {}
    void Behavior();
};

class TravellerCarGenerator : public simlib3::Event
{
public:
    FuelType fuelType;

    TravellerCarGenerator(FuelType fuelType) : fuelType(fuelType) {}
    void Behavior();
};