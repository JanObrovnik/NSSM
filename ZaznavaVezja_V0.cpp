#include <string>

#include "UporabniskiVmesnik.h"



int main(int argc, char* argv[]) {

	std::string potVhod = "C:/Users/janob/Desktop/Faks/NapredniSenzorskiSistemi/CPP/ProjektNSSM/ProjektNSSM/Seminar/Vhod";
	std::string potIzhod = "C:/Users/janob/Desktop/Faks/NapredniSenzorskiSistemi/CPP/ProjektNSSM/ProjektNSSM/Seminar/Izhod";

	if (argc > 1) potVhod = argv[1];
	if (argc > 2) potIzhod = argv[2];

	UporabniskiVmesnik uporabniskiVmesnik(potVhod, potIzhod);


	return 0;
}
