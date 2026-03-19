#pragma once
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <filesystem>
#include <string>
#include <array>
#include <algorithm>

#include "Logika.h"



enum GUMB {
	VSI = -1,
	LEVI,
	DESNI,
	BARVA,
	ZICA,
	BERI,
	BRISI,
	SHRANI,
	kolicina
};


class UporabniskiVmesnik {

private:
	cv::Mat kontrolnaPlosca;
	cv::Mat slika;
	const std::string& vhod, izhod;
	std::vector<std::string> seznamPoti;
	std::vector<std::string> seznamKontrolnihIkon;
	int indexTab;
	std::vector<std::pair<cv::Scalar, cv::Scalar>> barve;

public:
	UporabniskiVmesnik(std::string& vhod, std::string& izhod);

private:
	void urediKontrolo(const GUMB& gumb = VSI);

	void naloziSliko(const GUMB& gumb = VSI);
	void shraniSliko();

	static void onMouse(int dogodek, int x, int y, int flags, void* userdata);
	void nadzorKontrol(int dogodek, int x, int y, int flags);
	void pritisnjenGumb(const GUMB& gumb = VSI);

	bool pritisnjenaTipka(const int& tipka);
};


std::vector<std::string> preberiMapo(std::string pot);
