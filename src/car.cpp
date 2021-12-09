#include "car.hpp"
#include "arguments.hpp"
#include <unordered_map>
#include "statistics.hpp"
// Cars travel by 40km/h
// I am speed
#define SPEED 40

using namespace simlib3;
using simlib3::Print;

const int TRAVELINGTHROUGHCITY = 30;

int Car::carSerialNumberCounter = 0;
void CitizenCar::Travel(double time, double distance)
{
    Wait(time);
    _elapsedDayMinutes += time;
    this->fuel -= distance;
    Statistics::emit(fuelTypes, distance);
}

void GoToStation(simlib3::Process *car, FuelType fuelTypes)
{
    //Find a fueling place for ourselves
    long unsigned int stationsRefilling = 0;
    bool dumbWaysToDie = true;
    do
    {
        int randomStationIndex = simlib3::Uniform(0, Arguments::stations.size());
        auto currentStation = Arguments::stations[randomStationIndex];
        if(currentStation.isGettingRefueled)
        {
            if(!(stationsRefilling >= Arguments::stations.size()))
            {
                stationsRefilling++;
                continue;
            }
        }

        auto places = currentStation.places;
        // Find last index of the place with this fuel type
        std::size_t lastPlaceIndexWithThisFuel = 0;
        for(std::size_t placeIndex = 0; placeIndex < places.size(); placeIndex++)
        {
            if(places[placeIndex].fuelType & fuelTypes)
            {
                lastPlaceIndexWithThisFuel = placeIndex;
            }
        }
        for (std::size_t placeIndex = 0; placeIndex < places.size(); placeIndex++)
        {
            auto place = places[placeIndex];
            int minFullPlaceIndex = 0;
            std::size_t minFullness = 0;
            if (place.fuelType & fuelTypes)
            {
                CitizenCar *potentialCitizenCar = dynamic_cast<CitizenCar *>(car);
                if (place.getTankQueue()->Busy())
                {
                    auto queueLen = place.getTankQueue()->QueueLen();
                    if (queueLen < minFullness)
                    {
                        minFullness = queueLen;
                        minFullPlaceIndex = placeIndex;
                    }
                    if (placeIndex >= lastPlaceIndexWithThisFuel)
                    {
                        // Stands into the shortest queue
                        dumbWaysToDie = false;
                        if(stationsRefilling == Arguments::stations.size())
                        {
                            Car* carAsCar = dynamic_cast<Car*>(car);
                            Print("%s #%d is waiting because Stat #%d is refueling\n",potentialCitizenCar==nullptr?"Traveller":"Citizen", carAsCar->number, currentStation.number);
                        }
                        places[minFullPlaceIndex].getTankQueue()->Seize(car);
                        if (potentialCitizenCar != nullptr)
                        {
                            auto time = Arguments::tankingTimes[Arguments::fuelTypeToInt[potentialCitizenCar->fuelTypes]];
                            Print("Citizen #%d is fueling up at shortest queue! (after %d days) It will take him %f minutes\n", potentialCitizenCar->number, potentialCitizenCar->daysSinceLastRefuel, time);
                            potentialCitizenCar->Wait(time);
                            potentialCitizenCar->daysSinceLastRefuel = 0;
                            potentialCitizenCar->_elapsedDayMinutes += time;
                            potentialCitizenCar->fuel = potentialCitizenCar->tankSize;
                        }
                        else
                        {
                            auto travellerCar = dynamic_cast<TravellerCar *>(car);
                            auto someTime = Arguments::tankingTimes[Arguments::fuelTypeToInt[travellerCar->fuelTypes]];
                            Print("Traveller #%d is fueling up at shortest queue! It will take him %f minutes\n", travellerCar->number, someTime);
                            travellerCar->Wait(someTime);
                        }
                        places[minFullPlaceIndex].getTankQueue()->Release(car);
                        break;
                    }
                }
                else
                {
                    // Stand into free queue
                    dumbWaysToDie = false;
                    place.getTankQueue()->Seize(car);
                    if (potentialCitizenCar != nullptr)
                    {
                        auto time = Arguments::tankingTimes[Arguments::fuelTypeToInt[potentialCitizenCar->fuelTypes]];
                        Print("Citizen #%d is fueling up! (after %d days) It will take him %f minutes\n", potentialCitizenCar->number, potentialCitizenCar->daysSinceLastRefuel, time);
                        potentialCitizenCar->Wait(time);
                        potentialCitizenCar->daysSinceLastRefuel = 0;
                        potentialCitizenCar->_elapsedDayMinutes += time;
                        potentialCitizenCar->fuel = potentialCitizenCar->tankSize;
                    }
                    else
                    {
                        auto travellerCar = dynamic_cast<TravellerCar *>(car);
                        auto someTime = Arguments::tankingTimes[Arguments::fuelTypeToInt[travellerCar->fuelTypes]];
                        Print("Traveller #%d is fueling up! It will take him %f minutes\n", travellerCar->number, someTime);
                        travellerCar->Wait(someTime);
                    }
                    place.getTankQueue()->Release(car);
                    break;
                }
            }
        }
    }
    while(dumbWaysToDie);
}

void CitizenCar::MaybeGoToStation()
{
    if (this->fuel <= Arguments::controlLightLevel * this->tankSize)
    {
        GoToStation(this, this->fuelTypes);
    }
}

void TravellerCar::Create(FuelType fuelTypes)
{
    this->number = Car::carSerialNumberCounter++;
    this->fuelTypes = fuelTypes;
}

void CitizenCar::Create(double fuel, double tankSize, FuelType fuelTypes)
{
    this->daysSinceLastRefuel = 0;
    this->number = Car::carSerialNumberCounter++;
    this->fuel = fuel;
    this->tankSize = tankSize;
    this->fuelTypes = fuelTypes;
    Print("Citizen #%d powered by %s\n", number, intToFuelString[Arguments::fuelTypeToInt[fuelTypes]]);
}

void CitizenCar::Behavior()
{
    // It's the morning
    double timeToWork = Arguments::kilometersToWork / (SPEED / 60.0f /*convert to km/m*/);
    while (true)
    {
        Print("Citizen #%d is waking up with %f%% fuel!\n", number, (this->fuel / this->tankSize) * 100.0f);
        this->_elapsedDayMinutes = 0;
        double randomTimeToWork = simlib3::Exponential(timeToWork);
        Travel(randomTimeToWork / 2, Arguments::kilometersToWork / 2);
        // Now decide if we want to tank
        MaybeGoToStation();
        Print("Citizen #%d is going to work! It will take him: %f minutes\n", number, randomTimeToWork / 2);
        Travel(randomTimeToWork / 2, Arguments::kilometersToWork / 2);

        //Works
        double workTime = simlib3::Normal(Arguments::workMinutesMean, Arguments::workMinutesDispersion);
        Print("Citizen #%d is working! It will take him %f minutes\n", number, workTime);
        Wait(workTime);
        this->_elapsedDayMinutes += workTime;

        // Reevaluate time from work to home
        randomTimeToWork = simlib3::Exponential(timeToWork);
        Travel(randomTimeToWork / 2, Arguments::kilometersToWork / 2);
        // Now decide if we want to tank
        MaybeGoToStation();
        Print("Citizen #%d is going home! It will take him %f minutes\n", number, randomTimeToWork / 2);
        Travel(randomTimeToWork / 2, Arguments::kilometersToWork / 2);

        if (this->fuelTypes & FuelType::Electric && simlib3::Uniform(0, 1) < Arguments::nightChargeProbability)
        {
            this->fuel = this->tankSize;
        }
        auto sleepMinutes = Arguments::dayMinutesLength - _elapsedDayMinutes;
        Print("Citizen #%d is going to sleep for %f minutes!\n", number, sleepMinutes);
        Wait(sleepMinutes);
        daysSinceLastRefuel++;
    }
}

void TravellerCar::Behavior()
{
    //Go to pump station, fuel up, go away
    Wait(TRAVELINGTHROUGHCITY);
    Print("Traveller #%d reached pumping station!\n", this->number);
    GoToStation(this, this->fuelTypes);
}

void TravellerCarGenerator::Behavior()
{
    auto car = new TravellerCar();
    car->Create(fuelType);
    car->Activate();
    Print("Traveller #%d powered by %s has arrived!\n", car->number, intToFuelString[Arguments::fuelTypeToInt[fuelType]]);
    Activate(Time + Exponential(period));
}

void Day::Behavior()
{
    Print("^^^^^^^^^^^^^^^^^^^^^^^^^\n           Day %d        \n^^^^^^^^^^^^^^^^^^^^^^^^^\n", dayNumber);
    printf("Day %d\n", dayNumber);
    Statistics::print();
}

void Day::Create(int dayCounter)
{
    dayNumber = dayCounter;
}

void DayGenerator::Behavior()
{
    auto day = new Day();
    day->Create(dayCounter);
    day->Activate();
    dayCounter++;
    Activate(Time + 1440);
}

void DayGenerator::Create()
{
    dayCounter = 0;
}