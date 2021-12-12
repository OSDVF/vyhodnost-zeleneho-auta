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
int givenUps[4] = {0,0,0,0};
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
    bool stupidlyLongQue = false;
    int guard = 0;
    do
    {
        stupidlyLongQue = false;
        int randomStationIndex = simlib3::Uniform(0, Arguments::stations.size());
        auto &currentStation = Arguments::stations[randomStationIndex];
        if (currentStation.isGettingRefueled)
        {
            if (!(stationsRefilling >= Arguments::stations.size()))
            {
                stationsRefilling++;
                continue;
            }
        }

        auto places = currentStation.places;
        // Find last index of the place with this fuel type
        std::size_t lastPlaceIndexWithThisFuel = 0;
        for (std::size_t placeIndex = 0; placeIndex < places.size(); placeIndex++)
        {
            if (places[placeIndex].fuelType & fuelTypes)
            {
                lastPlaceIndexWithThisFuel = placeIndex;
            }
        }
        std::size_t minFullness = places[0].getTankQueue()->QueueLen();
        int minFullPlaceIndex = 0;
        Car *carAsCar = dynamic_cast<Car *>(car);
        for (std::size_t placeIndex = 0; placeIndex < places.size(); placeIndex++)
        {
            auto place = places[placeIndex];
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
                        if (minFullness > 10)
                        {
                            stupidlyLongQue = true;
                            printf("Stupidly long queue\n");
                            break;
                        }
                        // Stands into the shortest queue
                        Statistics::queueLengthForFuel[Arguments::fuelTypeToInt[fuelTypes]](minFullness);
                        dumbWaysToDie = false;
                        currentStation.thisDayCustomers++;
                        const char *carName = potentialCitizenCar == nullptr ? "Traveller" : "Citizen";
                        if (stationsRefilling == Arguments::stations.size())
                        {
                            Print("%s #%d is waiting because Stat #%d is refueling\n", carName, carAsCar->number, currentStation.number);
                        }
                        Print("%s #%d is waiting at %s Place at Stat #%d with queue long %d\n", carName, carAsCar->number, intToFuelString[Arguments::fuelTypeToInt[place.fuelType]], randomStationIndex, queueLen);
                        places[minFullPlaceIndex].getTankQueue()->Seize(car);
                        if (potentialCitizenCar != nullptr)
                        {
                            auto time = Arguments::tankingTimes[Arguments::fuelTypeToInt[potentialCitizenCar->fuelTypes]];
                            Print("[%f] Citizen #%d is fueling up at shortest queue! (after %d days) It will take him %f minutes\n", Time, potentialCitizenCar->number, potentialCitizenCar->daysSinceLastRefuel, time);
                            potentialCitizenCar->Wait(time);
                            potentialCitizenCar->daysSinceLastRefuel = 0;
                            potentialCitizenCar->_elapsedDayMinutes += time;
                            potentialCitizenCar->fuel = potentialCitizenCar->tankSize;
                        }
                        else
                        {
                            auto travellerCar = dynamic_cast<TravellerCar *>(car);
                            auto someTime = Arguments::tankingTimes[Arguments::fuelTypeToInt[travellerCar->fuelTypes]];
                            Print("[%f] Traveller #%d is fueling up at shortest queue! It will take him %f minutes\n", Time, travellerCar->number, someTime);
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
                    currentStation.thisDayCustomers++;
                    Statistics::queueLengthForFuel[Arguments::fuelTypeToInt[fuelTypes]](0);
                    place.getTankQueue()->Seize(car);
                    if (potentialCitizenCar != nullptr)
                    {
                        auto time = Arguments::tankingTimes[Arguments::fuelTypeToInt[potentialCitizenCar->fuelTypes]];
                        Print("[%f] Citizen #%d is fueling up! (after %d days) It will take him %f minutes\n", Time, potentialCitizenCar->number, potentialCitizenCar->daysSinceLastRefuel, time);
                        potentialCitizenCar->Wait(time);
                        potentialCitizenCar->daysSinceLastRefuel = 0;
                        potentialCitizenCar->_elapsedDayMinutes += time;
                        potentialCitizenCar->fuel = potentialCitizenCar->tankSize;
                    }
                    else
                    {
                        auto travellerCar = dynamic_cast<TravellerCar *>(car);
                        auto someTime = Arguments::tankingTimes[Arguments::fuelTypeToInt[travellerCar->fuelTypes]];
                        Print("[%f] Traveller #%d is fueling up! It will take him %f minutes\n", Time, travellerCar->number, someTime);
                        travellerCar->Wait(someTime);
                    }
                    place.getTankQueue()->Release(car);
                    break;
                }
            }
        }
        if (stupidlyLongQue)
        {
            guard++;
        }
        if(guard >= (int)Arguments::stations.size() * 2)
        {
            Print("[%f] Citizen #%d given up!\n",Time, carAsCar->number);
            givenUps[Arguments::fuelTypeToInt[fuelTypes]]++;
            break;
        }
    } while (dumbWaysToDie);
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
    double currentKilometers = simlib3::Uniform(Arguments::kilometersToWork - Arguments::kilometersToWorkDeviation, Arguments::kilometersToWork + Arguments::kilometersToWorkDeviation);
    double timeToWork = currentKilometers / (SPEED / 60.0f /*convert to km/m*/);
    double workHourStart = simlib3::Uniform(0, Arguments::differenceBetweenWorkBegin);
    while (true)
    {
        Print("Citizen #%d woke up with %f%% fuel!\n", number, (this->fuel / this->tankSize) * 100.0f);
        Wait(workHourStart);
        this->_elapsedDayMinutes = workHourStart;
        Travel(timeToWork / 2, currentKilometers / 2);
        // Now decide if we want to tank
        MaybeGoToStation();
        //Print("Citizen #%d is going to work! It will take him: %f minutes\n", number, timeToWork / 2);
        Travel(timeToWork / 2, currentKilometers / 2);

        //Works
        double workTime;
        do
        {
            workTime = simlib3::Normal(Arguments::workMinutesMean, Arguments::workMinutesDispersion);
        } while (workTime < 0); // Normal distribution can be even -inf

        //Print("Citizen #%d is working! It will take him %f minutes\n", number, workTime);
        Wait(workTime);
        this->_elapsedDayMinutes += workTime;

        // Reevaluate time from work to home
        Travel(timeToWork / 2, currentKilometers / 2);
        // Now decide if we want to tank
        MaybeGoToStation();
        //Print("Citizen #%d is going home! It will take him %f minutes\n", number, timeToWork / 2);
        Travel(timeToWork / 2, currentKilometers / 2);

        if (this->fuelTypes & FuelType::Electric && simlib3::Uniform(0, 1) < Arguments::nightChargeProbability)
        {
            this->fuel = this->tankSize;
        }
        auto sleepMinutes = Arguments::dayMinutesLength - _elapsedDayMinutes;
        //Print("Citizen #%d is going to sleep for %f minutes!\n", number, sleepMinutes);
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
double lastGeneratedTime = 0;

void TravellerCarGenerator::Behavior()
{
    auto car = new TravellerCar();

    double fuelTypeProbability = Uniform(0, Arguments::totalCars);

    FuelType choosedFuelType;
    if (fuelTypeProbability < Arguments::carsCount[0])
    {
        choosedFuelType = intToFuelType[0];
    }
    else if (fuelTypeProbability < Arguments::carsCount[0] + Arguments::carsCount[1])
    {
        choosedFuelType = intToFuelType[1];
    }
    else if (fuelTypeProbability < Arguments::carsCount[0] + Arguments::carsCount[1] + Arguments::carsCount[2])
    {
        choosedFuelType = intToFuelType[2];
    }
    else
    {
        choosedFuelType = intToFuelType[3];
    }

    car->Create(choosedFuelType);
    double nextTime = Time + Exponential(period);
    auto timeSpanBetweenTravellers = nextTime - lastGeneratedTime;
    lastGeneratedTime = simlib3::Time;
    car->Activate(nextTime);
    Print("Traveller #%d powered by %s has arrived after %f minutes!\n", car->number, intToFuelString[Arguments::fuelTypeToInt[choosedFuelType]], timeSpanBetweenTravellers);
    Activate(nextTime);
}

void Day::Behavior()
{
    for (auto &station : Arguments::stations)
    {
        Statistics::numberOfStationCustomersPerDay(station.thisDayCustomers);
        station.thisDayCustomers = 0;
    }
    for (int i = 0; i < 4; i++)
    {
        Statistics::givenUpCars[i](givenUps[i]);
        givenUps[i] = 0;
    }

    Statistics::print();
    Print("^^^^^^^^^^^^^^^^^^^^^^^^^\n           Day %d        \n^^^^^^^^^^^^^^^^^^^^^^^^^\n", dayNumber);
    printf("Day %d\n", dayNumber);
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