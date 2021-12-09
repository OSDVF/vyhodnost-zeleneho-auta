#pragma once
#include <simlib.h>
#include "car.hpp"
class Statistics
{
    // To find if we fulfill the requirement of 55% reduction from year 1990 (672 MtCO2e)
    // https://www.eea.europa.eu/data-and-maps/daviz/greenhouse-gas-emissions-from-transport-2#tab-chart_1
    // It was 383.8 Mt of CO2 globally
    // There was 163628 cars in EU in 1990 (https://www.statista.com/statistics/452447/europe-eu-28-number-of-registered-passenger-cars/)
    // 
    // For city with 35000 cars it was (383.8/163628) = 0,00234556432884347422201579191825 Mt
    // = 2 345.5643t CO2 per year
public:
    static double directEmissions[2];
    static double indirectEmissions[4];

    // Emissions per km
    static double directEmissionForFuel[4];
    static double indirectEmissionForFuel[4];

    static void emit(FuelType fuel, double km);
    static void print();
};