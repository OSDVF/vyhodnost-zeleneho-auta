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
        int randomIndex = simlib3::Uniform(0, Arguments::stations.size());
        auto places = Arguments::stations[randomIndex].places;
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
                        places[minFullPlaceIndex].getTankQueue()->Seize(car);
                        if (potentialCitizenCar != nullptr)
                        {
                            auto time = Arguments::tankingTimes[fuelTypeToInt[potentialCitizenCar->fuelTypes]];
                            potentialCitizenCar->Wait(time);
                            potentialCitizenCar->_elapsedDayMinutes += time;
                            potentialCitizenCar->fuel = potentialCitizenCar->tankSize;
                        }
                        else
                        {
                            auto travellerCar = dynamic_cast<TravellerCar *>(car);
                            travellerCar->Wait(Arguments::tankingTimes[fuelTypeToInt[travellerCar->fuelTypes]]);
                        }
                        places[minFullPlaceIndex].getTankQueue()->Release(car);
                    }
                }
                else
                {
                    // Stand into free queue
                    place.getTankQueue()->Seize(car);
                    if (potentialCitizenCar != nullptr)
                    {
                        auto time = Arguments::tankingTimes[fuelTypeToInt[potentialCitizenCar->fuelTypes]];
                        potentialCitizenCar->Wait(time);
                        potentialCitizenCar->_elapsedDayMinutes += time;
                        potentialCitizenCar->fuel = potentialCitizenCar->tankSize;
                    }
                    else
                    {
                        auto travellerCar = dynamic_cast<TravellerCar *>(car);
                        travellerCar->Wait(Arguments::tankingTimes[fuelTypeToInt[travellerCar->fuelTypes]]);
                    }
                    place.getTankQueue()->Release(car);
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

void CitizenCar::Behavior()
{
    // It's the morning
    double timeToWork = Arguments::kilometersToWork / SPEED;
    while (true)
    {
        this->_elapsedDayMinutes = 0;
        double randomTimeToWork = simlib3::Exponential(timeToWork) * 60;
        Travel(randomTimeToWork / 2, Arguments::kilometersToWork / 2);
        // Now decide if we want to tank
        MaybeGoToStation();
        Travel(randomTimeToWork / 2, Arguments::kilometersToWork / 2);

        //Works
        double workTime = simlib3::Normal(Arguments::workMinutesMean, Arguments::workMinutesDispersion);
        Wait(workTime);
        this->_elapsedDayMinutes += workTime;

        // Reevaluate time from work to home
        randomTimeToWork = simlib3::Exponential(timeToWork) * 60;
        Travel(randomTimeToWork / 2, Arguments::kilometersToWork / 2);
        // Now decide if we want to tank
        MaybeGoToStation();
        Travel(randomTimeToWork / 2, Arguments::kilometersToWork / 2);

        if (this->fuelTypes & FuelType::Electric && simlib3::Uniform(0, 1) < Arguments::nightChargeProbability)
        {
            this->fuel = this->tankSize;
        }
        Wait(Arguments::dayMinutesLength - _elapsedDayMinutes);
    }
}

void TravellerCar::Behavior()
{
    //Go to pump station, fuel up, go away
    Wait(TRAVELINGTHROUGHCITY);
    GoToStation(this, this->fuelTypes);
}

void TravellerCarGenerator::Behavior()
{
    (new TravellerCar(intToFuelType[(int)simlib3::Uniform(0, 3)]))->Activate();
    Activate(Time + Exponential(120));
}