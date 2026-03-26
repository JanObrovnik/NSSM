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


//bool operator==(cv::Point const& to1, cv::Point const& to2);
//bool operator<(cv::Point const& to1, cv::Point const& to2);
//bool operator==(cv::Point& to1, cv::Point& to2);
//bool operator<(cv::Point& to1, cv::Point& to2);

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


std::vector<std::pair<cv::Scalar, cv::Scalar>> prepoznavaBarv_V0(cv::Mat slika);
std::vector<std::pair<cv::Scalar, cv::Scalar>> prepoznavaBarv_V1(cv::Mat slika);
std::vector<std::pair<cv::Scalar, cv::Scalar>> prepoznavaBarv_V2(cv::Mat slika);


void prepoznavaZic_V0(cv::Mat slika, const std::vector<std::pair<cv::Scalar, cv::Scalar>>& barve);
void prepoznavaZic_V1(cv::Mat slika, const std::vector<std::pair<cv::Scalar, cv::Scalar>>& barve);
void prepoznavaZic_V2(cv::Mat slika, const std::vector<std::pair<cv::Scalar, cv::Scalar>>& barve);
std::vector<cv::Point> prepoznavaZic_V3(cv::Mat slika, const std::vector<std::pair<cv::Scalar, cv::Scalar>>& barve);

void izdelavaMaske_V0(cv::Mat slika, cv::Mat& maska, const std::vector<std::pair<cv::Scalar, cv::Scalar>>& barve);
void izdelavaMaske_V1(cv::Mat slika, cv::Mat& maska, const std::vector<std::pair<cv::Scalar, cv::Scalar>>& barve);
void izdelavaMaske_V2(cv::Mat slika, cv::Mat& maska, const std::pair<cv::Scalar, cv::Scalar>& barva);

std::vector<cv::Point> vertikalnaPrepoznava(cv::Mat maska, int korak);
void horizontalnaPoravnava_V0(cv::Mat maska, std::vector<cv::Point>& seznamTock); //- stara verzija
void horizontalnaPoravnava_V1(cv::Mat maska, std::vector<cv::Point>& seznamTock);

std::vector<cv::Point> horizontalnaPrepoznava(cv::Mat maska, int korak);
void vertikalnaPoravnava_V1(cv::Mat maska, std::vector<cv::Point>& seznamTock);

std::vector<cv::Point> prepoznavaVsehTock(cv::Mat& maska);

std::vector<cv::Point> zdruzitevSeznamovTock_V0(const std::vector<cv::Point>& sez1, const std::vector<cv::Point>& sez2, const int& filter);
std::vector<cv::Point> zdruzitevSeznamovTock_V1(const std::vector<cv::Point>& sez1, const std::vector<cv::Point>& sez2, const int& filter);
std::vector<cv::Point> zdruzitevSeznamovTock_V2(const std::vector<cv::Point>& sez1, const std::vector<cv::Point>& sez2, const int& filter);
std::vector<cv::Point> zdruzitevSeznamovTock_V3(const std::vector<cv::Point>& sez1, const std::vector<cv::Point>& sez2, const int& filter);

cv::Point sredinaTock_V0(const std::vector<cv::Point>& seznamTock); //- stara verzija
inline cv::Point sredinaTock_V1(const std::vector<cv::Point>::const_iterator& it1, const std::vector<cv::Point>::const_iterator& it2);
inline cv::Point sredinaTock_V1(const cv::Point& to1, const cv::Point& to2);
std::vector<cv::Point> sredinaVecihTock_V0(const std::vector<cv::Point>& seznamTock);
inline int manhattanRazdalja(const cv::Point& to1, const cv::Point& to2);
void skrajneTocke(cv::Mat maska, const cv::Point& tocka, cv::Point& desnaTocka, cv::Point& zgornjaTocka, cv::Point& levaTocka, cv::Point& spodnjaTocka);

std::vector<cv::Point> narisiPovezave_V0(cv::Mat slika, const std::vector<cv::Point>& seznamTock, const cv::Point& zacetnaTocka);
std::vector<cv::Point> narisiPovezave_V1(cv::Mat slika, const std::vector<cv::Point>& seznamTock, const cv::Point& zacetnaTocka);
std::vector<cv::Point> narisiPovezave_V2(cv::Mat slika, const std::vector<cv::Point>& seznamTock, const cv::Point& zacetnaTocka);
std::vector<cv::Point> narisiPovezave_V3(cv::Mat maska, const std::vector<cv::Point>& seznamTock);
std::vector<int> narediSeznamRazdalj(const std::vector<cv::Point>& seznamTock);

std::vector<cv::Point> narisiPovezaveKoncev_V0(const std::vector<cv::Point>& seznamTock);
std::vector<cv::Point> narisiProstaKonca0_V0(const std::vector<cv::Point>& seznamTock);
std::vector<cv::Point> narisiProstaKonca1_V0(const std::vector<cv::Point>& seznamTock);


void prepoznajTekst_V0(cv::Mat slika, const int razsiritev = 10);
std::vector<TextZica> prepoznajTekst_V1(cv::Mat slika, const std::vector<cv::Point>& seznamTock, const int razsiritev = 10);

std::vector<int> poveziBliznjeTocke_V0(cv::Mat slika, const std::vector<cv::Point>& seznamTock, const std::vector<cv::Point>& seznamTockTexta);
