#pragma once
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>
#include <array>


enum GUMB {
	VSI = -1,
	LEVI,
	DESNI,
	BERI,
	BRISI
};
const int stGumbov = 4;


class UporabniskiVmesnik {

public:
	cv::Mat kontrolnaPlosca;
	cv::Mat slika;

	UporabniskiVmesnik();


	void urediKontrolo(const cv::Scalar& barva, const GUMB& gumb = VSI);

	void naloziSliko(const GUMB& gumb = VSI);

	static void onMouse(int event, int x, int y, int flags, void* userdata);
	void nadzorKontrol(int action, int x, int y, int flags);
	void pritisnjenGumb(const cv::Scalar& barva, const GUMB& gumb = VSI);
};
