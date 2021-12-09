#include "station.hpp"
#include "arguments.hpp"

void Station::addPlaces(std::vector<int> placesCount)
{
    for(int i = 0;i<4;i++)
    {
        this->placesCount[i] += placesCount[i];
    }
    isGettingRefueled = false;
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
int globalStationsCounter = 0;
Station::Station()
{
    this->number = globalStationsCounter++;
}

void Station::printPlacesInfo()
{
    simlib3::Print("Stat #%d has %d places [", number, places.size());
    for(int i = 0;i<4;i++)
    {
        simlib3::Print("%s:%d ", intToFuelString[i],this->placesCount[i]);
    }
    simlib3::Print("]\n");
}

void Station::printStatistics()
{
    Print("Stat #%d summary ༼ つ ◕_◕ ༽つ\n", number);
    for (auto p : places)
    {
        p.getTankQueue()->Output();
    }
}

int globalPlaceCounter = 0;
StationPlace::StationPlace(FuelType fuelType)
{
    this->fuelType = fuelType;
    std::string name = std::string(std::string(intToFuelString[Arguments::fuelTypeToInt[fuelType]]) +
                                   " place #" + std::to_string(globalPlaceCounter++));
    this->tankingQueue = new Facility(name.c_str());
}

simlib3::Facility *StationPlace::getTankQueue()
{
    return tankingQueue;
}

void RefuelStation::Create(Station station)
{
    stationToRefuel = station;
}

void RefuelStation::Behavior()
{
    stationToRefuel.isGettingRefueled = true;
    for (auto p : stationToRefuel.places)
    {
        if(p.fuelType != FuelType::Electric)
            p.getTankQueue()->Seize(this);
    }
    Print("Refilling station #%d\n", stationToRefuel.number);
    Wait(Arguments::refuelTime);
    Print("Station #%d refilled\n", stationToRefuel.number);
    for (auto p : stationToRefuel.places)
    {
        if(p.fuelType != FuelType::Electric)
            p.getTankQueue()->Release(this);
    }
    stationToRefuel.isGettingRefueled = false;
}

void RefuelTheStationGenerator::Create(Station station)
{
    stationToRefuel = station;
}

void RefuelTheStationGenerator::Behavior()
{
    auto nextRefill = Normal(Arguments::stationFill[0], Arguments::stationFill[1]);
    auto refuel = new RefuelStation();
    refuel->Create(stationToRefuel);
    refuel->Activate(Time + nextRefill);
    Activate(Time + nextRefill);
}