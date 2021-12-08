#include "station.hpp"

Station::Station(std::vector<int> placesCount)
{
    int remainingPetrol = placesCount[0];
    int remainingDiesel = placesCount[1];

    int remainingElectric = placesCount[2];
    int remainingHydrogen = placesCount[3];
    do
    {
        int generatedType = simlib3::Uniform(0, 3);
        switch (generatedType)
        {
        case 0:
            if (remainingHydrogen != 0)
            {
                places.push_back(StationPlace(FuelType::Hydrogen));
                remainingHydrogen--;
                break;
            }
            [[fallthrough]];
        case 1:
            if (remainingElectric != 0)
            {
                places.push_back(StationPlace(FuelType::Electric));
                remainingElectric--;
                break;
            }
            [[fallthrough]];
        case 2:
            if (remainingPetrol != 0)
            {
                places.push_back(StationPlace(FuelType::Petrol));
                remainingPetrol--;
                break;
            }
            [[fallthrough]];
        case 3:
            if (remainingDiesel != 0)
            {
                places.push_back(StationPlace(FuelType::Diesel));
                remainingDiesel--;
                break;
            }
        }
    } while (remainingPetrol |
             remainingHydrogen |
             remainingDiesel |
             remainingElectric);
}

simlib3::Facility* StationPlace::getTankQueue()
{
    return tankingQueue;
}