#include "car.hpp"

using namespace simlib3;

const int TRAVELINGTHROUGHCITY = 30;

void GoToStation(simlib3::Process* car, FuelType fuelTypes){}

void CitizenCar::Behavior()
{

}

void TravellerCar::Behavior()
{
	//Go to pump station, fuel up, go away
	Wait(TRAVELINGTHROUGHCITY);
	GoToStation(this, this->fuelTypes);
}

void TravellerCarGenerator::Behavior()
{
    (new TravellerCar(FuelType::Electric)) -> Activate();
    Activate(Time + Exponential(120));
}