#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include <iostream>
#include <filesystem>
#include <string>
#include <array>

#include "UporabniskiVmesnik.h"



int main(int argc, char* argv[]) {

	std::string potVhod = "C:/Users/janob/Desktop/Faks/NapredniSenzorskiSistemi/CPP/ProjektNSSM/ProjektNSSM/Seminar/Vhod";
	std::string potIzhod = "C:/Users/janob/Desktop/Faks/NapredniSenzorskiSistemi/CPP/ProjektNSSM/ProjektNSSM/Seminar/Izhod";

	if (argc > 1) potVhod = argv[1];
	if (argc > 2) potIzhod = argv[2];

	UporabniskiVmesnik uporabniskiVmesnik(potVhod, potIzhod);


	return 0;
}
