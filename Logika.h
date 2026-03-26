#pragma once
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv4/opencv2/ximgproc.hpp>

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include <iostream>
#include <filesystem>
#include <string>
#include <array>
#include <vector>
#include <map>
#include <chrono>


struct Tocka {
	int x, y;
	Tocka() : x(0), y(0) {}
	Tocka(int x, int y) : x(x), y(y) {}
	Tocka(cv::Point tocka) : x(tocka.x), y(tocka.y) {}
};

struct TextZica {
	cv::Point tockaText;
	cv::Point tockaZica;
	std::string text;
};


std::vector<std::pair<cv::Scalar, cv::Scalar>> prepoznavaBarv_V2(cv::Mat slika);


std::vector<cv::Point> prepoznavaZic_V3(cv::Mat slika, const std::vector<std::pair<cv::Scalar, cv::Scalar>>& barve);

void izdelavaMaske_V2(cv::Mat slika, cv::Mat& maska, const std::pair<cv::Scalar, cv::Scalar>& barva);

std::vector<cv::Point> prepoznavaVsehTock(cv::Mat& maska);

std::vector<cv::Point> narisiPovezave_V3(cv::Mat maska, const std::vector<cv::Point>& seznamTock);
std::vector<cv::Point> narisiPovezaveKoncev_V0(const std::vector<cv::Point>& seznamTock);
std::vector<cv::Point> narisiProstaKonca0_V0(const std::vector<cv::Point>& seznamTock);
std::vector<cv::Point> narisiProstaKonca1_V0(const std::vector<cv::Point>& seznamTock);

inline int manhattanRazdalja(const cv::Point& to1, const cv::Point& to2);


std::vector<TextZica> prepoznajTekst_V1(cv::Mat slika, const std::vector<cv::Point>& seznamTock, const int razsiritev = 10);

std::vector<int> poveziBliznjeTocke_V0(cv::Mat slika, const std::vector<cv::Point>& seznamTock, const std::vector<cv::Point>& seznamTockTexta);
