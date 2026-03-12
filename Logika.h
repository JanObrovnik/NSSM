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



void prepoznavaBarv_V0(cv::Mat slika);

void prepoznavaZic_V0(cv::Mat slika);

void prepoznajTekst_V0(cv::Mat slika, const int razsiritev = 10);