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

inline int operator&(FuelType a, FuelType b)
{
    return (static_cast<int>(a) & static_cast<int>(b));
}

void GoToStation(simlib3::Process *car, FuelType fuelTypes);

class Car
{
public:
    static int carSerialNumberCounter;
    //should be set at start
    int number;
};

class CitizenCar : public simlib3::Process, public Car
{
public:
    double fuel;
    double tankSize;
    FuelType fuelTypes;
    int daysSinceLastRefuel;

    //varying
    double _elapsedDayMinutes = 0;
    void Create(double fuel, double tankSize, FuelType fuelTypes);

    void Behavior();
    void MaybeGoToStation();
    void Travel(double time, double distance);
};

class TravellerCar : public simlib3::Process, public Car
{
public:
    FuelType fuelTypes;
    void Create(FuelType fuelTypes);
    void Behavior();
};

class TravellerCarGenerator : public simlib3::Event
{
public:
    double period;

    void Create(double period)
    {
        this->period = period;
    }
    void Behavior();
};

class Day : public simlib3::Process
{
public:
    int dayNumber;
    void Create(int dayCounter);
    void Behavior();
};

class DayGenerator : public simlib3::Event
{
public:
    int dayCounter;
    void Create();
    void Behavior();
};