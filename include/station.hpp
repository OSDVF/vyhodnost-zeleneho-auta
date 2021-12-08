#include "simlib.h"
#include "car.hpp"
#include <vector>
struct StationPlace
{
public:
    FuelType fuelType;

    StationPlace(FuelType fuelType) : fuelType(fuelType) {}

private:
    simlib3::Facility* place;
};

class Station
{
public:
    std::vector<StationPlace> places;

    Station(std::vector<int> placesCount);
    Station() {}
};