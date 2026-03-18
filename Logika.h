#pragma once
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
#include <vector>
//#include <set>



void prepoznavaBarv_V0(cv::Mat slika);


void prepoznavaZic_V0(cv::Mat slika);

std::vector<cv::Point> vertikalnaPrepoznava(cv::Mat slika, int korak);
void horizontalnaPoravnava_V0(cv::Mat slika, std::vector<cv::Point>& seznamTock); //- stara verzija
void horizontalnaPoravnava_V1(cv::Mat slika, std::vector<cv::Point>& seznamTock);

std::vector<cv::Point> horizontalnaPrepoznava(cv::Mat slika, int korak);
void vertikalnaPoravnava_V1(cv::Mat slika, std::vector<cv::Point>& seznamTock);

std::vector<cv::Point> zdruzitevSeznamovTock(const std::vector<cv::Point>& sez1, const std::vector<cv::Point>& sez2);

cv::Point sredinaTock_V0(const std::vector<cv::Point>& seznamTock); //- stara verzija
inline cv::Point sredinaTock_V1(const std::vector<cv::Point>::const_iterator& it1, const std::vector<cv::Point>::const_iterator& it2);
inline cv::Point sredinaTock_V1(const cv::Point& to1, const cv::Point& to2);
std::vector<cv::Point> sredinaVecihTock_V0(const std::vector<cv::Point>& seznamTock);
inline int manhattanRazdalja(const cv::Point& to1, const cv::Point& to2);

std::vector<cv::Point> narisiPovezave_V0(cv::Mat slika, const std::vector<cv::Point>& seznamTock, const cv::Point& zacetnaTocka);
std::vector<cv::Point> narisiPovezave_V1(cv::Mat slika, const std::vector<cv::Point>& seznamTock, const cv::Point& zacetnaTocka);
std::vector<int> narediSeznamRazdalj(const std::vector<cv::Point>& seznamTock);


void prepoznajTekst_V0(cv::Mat slika, const int razsiritev = 10);
