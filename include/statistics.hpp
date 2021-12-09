#include <simlib.h>
#include "car.hpp"
class Statistics
{
    // To find if we fulfill the requirement of 55% reduction from year 1990 (672 MtCO2e)
    // https://www.eea.europa.eu/data-and-maps/daviz/greenhouse-gas-emissions-from-transport-2#tab-chart_1
    // It was 383.8 Mt of CO2 globally
    // For city with 35000 cars it was
public:
    static double directEmissions[2];
    static double indirectEmissions[4];

    // Emissions per km
    static double directEmissionForFuel[4];
    static double indirectEmissionForFuel[4];

    static void emit(FuelType fuel, double km);
    static void print();
};