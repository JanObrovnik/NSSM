#pragma once
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <filesystem>
#include <string>
#include <array>

#include "Logika.h"



enum GUMB {
	VSI = -1,
	LEVI,
	DESNI,
	BARVA,
	ZICA,
	BERI,
	BRISI,
	kolicina
};


class UporabniskiVmesnik {

private:
	cv::Mat kontrolnaPlosca;
	cv::Mat slika;
	const std::string& vhod, izhod;
	std::vector<std::string> seznamPoti;

public:
	UporabniskiVmesnik(std::string& vhod, std::string& izhod);

private:
	void urediKontrolo(const cv::Scalar& barva, const GUMB& gumb = VSI);

	void naloziSliko(const GUMB& gumb = VSI);

	static void onMouse(int event, int x, int y, int flags, void* userdata);
	void nadzorKontrol(int action, int x, int y, int flags);
	void pritisnjenGumb(const cv::Scalar& barva, const GUMB& gumb = VSI);
};


std::vector<std::string> preberiMapo(std::string& pot);