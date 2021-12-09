#include "car.hpp"
#include "arguments.hpp"
#include <unordered_map>
// Cars travel by 40km/h
// I am speed
#define SPEED 40

using namespace simlib3;

const int TRAVELINGTHROUGHCITY = 30;

std::unordered_map<FuelType, int> fuelTypeToInt = std::unordered_map<FuelType, int>({{FuelType::Petrol, 0},
                                                                                     {FuelType::Diesel, 1},
                                                                                     {FuelType::Electric, 2},
                                                                                     {FuelType::Hydrogen, 3}});
int CitizenCar::carSerialNumberCounter = 0;
void CitizenCar::Travel(double time, double distance)
{
    Wait(time);
    _elapsedDayMinutes += time;
    this->fuel -= distance;
}

void GoToStation(simlib3::Process *car, FuelType fuelTypes)
{
    //Find a station for ourselves
    int foundStation = -1;
    do
    {
        int randomStationIndex = simlib3::Uniform(0, Arguments::stations.size());
        auto places = Arguments::stations[randomStationIndex].places;
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
                    if (placeIndex == places.size() - 1)
                    {
                        // Stands into the shortest queue
                        foundStation = randomStationIndex;
                        places[minFullPlaceIndex].getTankQueue()->Seize(car);
                        if (potentialCitizenCar != nullptr)
                        {
                            auto time = Arguments::tankingTimes[fuelTypeToInt[potentialCitizenCar->fuelTypes]];
                            Print("Citizen #%d is fueling up at shortest queue! (after %d days) It will take him %f minutes\n", potentialCitizenCar->carSerialNumber, potentialCitizenCar->daysSinceLastRefuel, time);
                            potentialCitizenCar->Wait(time);
                            potentialCitizenCar->daysSinceLastRefuel = 0;
                            potentialCitizenCar->_elapsedDayMinutes += time;
                            potentialCitizenCar->fuel = potentialCitizenCar->tankSize;
                        }
                        else
                        {
                            auto travellerCar = dynamic_cast<TravellerCar *>(car);
                            auto someTime = Arguments::tankingTimes[fuelTypeToInt[travellerCar->fuelTypes]];
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
                    foundStation = randomStationIndex;
                    place.getTankQueue()->Seize(car);
                    if (potentialCitizenCar != nullptr)
                    {
                        auto time = Arguments::tankingTimes[fuelTypeToInt[potentialCitizenCar->fuelTypes]];
                        Print("Citizen #%d is fueling up! (after %d days) It will take him %f minutes\n", potentialCitizenCar->carSerialNumber, potentialCitizenCar->daysSinceLastRefuel, time);
                        potentialCitizenCar->Wait(time);
                        potentialCitizenCar->daysSinceLastRefuel = 0;
                        potentialCitizenCar->_elapsedDayMinutes += time;
                        potentialCitizenCar->fuel = potentialCitizenCar->tankSize;
                    }
                    else
                    {
                        auto travellerCar = dynamic_cast<TravellerCar *>(car);
                        auto someTime = Arguments::tankingTimes[fuelTypeToInt[travellerCar->fuelTypes]];
                        Print("Traveller #%d is fueling up! It will take him %f minutes\n", travellerCar->number, someTime);
                        travellerCar->Wait(someTime);
                    }
                    place.getTankQueue()->Release(car);
                    break;
                }
            }
        }
    } while (foundStation == -1);
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
    this->number = CitizenCar::carSerialNumberCounter++;
    this->fuelTypes = fuelTypes;
}

void CitizenCar::Create(double fuel, double tankSize, FuelType fuelTypes)
{
    this->daysSinceLastRefuel = 0;
    this->carSerialNumber = carSerialNumberCounter++;
    this->fuel = fuel;
    this->tankSize = tankSize;
    this->fuelTypes = fuelTypes;
    Print("Citizen #%d powered by %s\n", carSerialNumber, intToFuelString[fuelTypeToInt[fuelTypes]]);
}

void CitizenCar::Behavior()
{
    // It's the morning
    double timeToWork = Arguments::kilometersToWork / (SPEED/60.0f /*convert to km/m*/ );
    while (true)
    {
        Print("Citizen #%d is waking up with %f%% fuel!\n", carSerialNumber, (this->fuel / this->tankSize) * 100.0f);
        this->_elapsedDayMinutes = 0;
        double randomTimeToWork = simlib3::Exponential(timeToWork);
        Travel(randomTimeToWork / 2, Arguments::kilometersToWork / 2);
        // Now decide if we want to tank
        MaybeGoToStation();
        Print("Citizen #%d is going to work! It will take him: %f minutes\n", carSerialNumber, randomTimeToWork / 2);
        Travel(randomTimeToWork / 2, Arguments::kilometersToWork / 2);

        //Works
        double workTime = simlib3::Normal(Arguments::workMinutesMean, Arguments::workMinutesDispersion);
        Print("Citizen #%d is working! It will take him %f minutes\n", carSerialNumber, workTime);
        Wait(workTime);
        this->_elapsedDayMinutes += workTime;

        // Reevaluate time from work to home
        randomTimeToWork = simlib3::Exponential(timeToWork);
        Travel(randomTimeToWork / 2, Arguments::kilometersToWork / 2);
        // Now decide if we want to tank
        MaybeGoToStation();
        Print("Citizen #%d is going home! It will take him %f minutes\n", carSerialNumber, randomTimeToWork / 2);
        Travel(randomTimeToWork / 2, Arguments::kilometersToWork / 2);

        if (this->fuelTypes & FuelType::Electric && simlib3::Uniform(0, 1) < Arguments::nightChargeProbability)
        {
            this->fuel = this->tankSize;
        }
        auto sleepMinutes = Arguments::dayMinutesLength - _elapsedDayMinutes;
        Print("Citizen #%d is going to sleep for %f minutes!\n", carSerialNumber, sleepMinutes);
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
    Print("Traveller #%d powered by %s has arrived!\n", car->number, intToFuelString[fuelTypeToInt[fuelType]]);
    Activate(Time + Exponential(period));
}

void Day::Behavior()
{
    Print("^^^^^^^^^^^^^^^^^^^^^^^^^\n           Day %d        \n^^^^^^^^^^^^^^^^^^^^^^^^^\n", dayNumber);
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