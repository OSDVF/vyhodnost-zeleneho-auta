#include "statistics.hpp"
#include "arguments.hpp"
#include "simlib.h"

double Statistics::directEmissions[2];
double Statistics::indirectEmissions[4];

double Statistics::directEmissionForFuel[4];
double Statistics::indirectEmissionForFuel[4];
simlib3::Stat Statistics::numberOfStationCustomersPerDay;
simlib3::Stat Statistics::givenUpCars[4];

simlib3::Histogram Statistics::queueLengthForFuel[4] {
    simlib3::Histogram("Petrol Queues Length",0,1,100),
    simlib3::Histogram("Diesel Queues Length",0,1,100),
    simlib3::Histogram("Electric Queues Length",0,1,100),
    simlib3::Histogram("Hydrogen Queues Length",0,1,100),
};

void Statistics::emit(FuelType fuel, double km)
{
    int fuelIndex = Arguments::fuelTypeToInt[fuel];
    directEmissions[fuelIndex] += directEmissionForFuel[fuelIndex] * km;
    indirectEmissions[fuelIndex] += indirectEmissionForFuel[fuelIndex] * km;
}

void Statistics::print()
{
    simlib3::Print("==Emission statistics==\n");
    double totalDirect = 0;
    double totalIndirect = 0;
    for (int i = 0; i < 4; i++)
    {
        simlib3::Print("%s direct: %f\n", intToFuelString[i], totalDirect += directEmissions[i]);
        simlib3::Print("%s indirect: %f\n", intToFuelString[i], totalIndirect += indirectEmissions[i]);
    }
    simlib3::Print("Total direct: %f\nTotal indirect: %f\n", totalDirect, totalIndirect);
    simlib3::Print("TOTAL: %f\n", totalDirect + totalIndirect);

    simlib3::Print("Customers per day:");
    numberOfStationCustomersPerDay.Output();
    for(int i = 0;i<4;i++)
    {
        simlib3::Print("Given up cars %s:\n",intToFuelString[i]);
        givenUpCars[i].Output();
    }

    for(int i = 0;i<4;i++)
    {
        queueLengthForFuel[i].Output();
    }
}