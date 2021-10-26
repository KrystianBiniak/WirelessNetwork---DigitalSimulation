#include "wireless_network.h"
#include "simulator.h"



int main(int argc, char* argv[])
{
	WirelessNetwork* wirelessnetwork = new WirelessNetwork();				//Create new wireless network where simulation happens
	Simulator simulator = Simulator(wirelessnetwork);

	simulator.Simulation(80000);

	return 0;
}