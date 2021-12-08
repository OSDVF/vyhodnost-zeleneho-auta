#include <simlib.h>

enum class FuelType
{
    Hydrogen = 1,
    Electric = 2,
    Gasoline = 4,
    Oil = 8
};

inline FuelType operator|(FuelType a, FuelType b)
{
    return static_cast<FuelType>(static_cast<int>(a) | static_cast<int>(b));
}

class Car : simlib3::Process
{
public:
    double fuel;
    double tankSize;
    FuelType fuelTypes;
    double consumption;


};