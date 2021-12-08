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

class Car : simlib3::Process
{
public:
    //should be set at start
    double fuel;
    double tankSize;
    FuelType fuelTypes;
    double consumption;

    //varying
    double _elapsedDayMinutes = 0;

};