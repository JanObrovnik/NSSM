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



void prepoznavaBarv_V0(cv::Mat slika) {

	cv::Mat slikaVmesna, slikaCanny, slikaHSV, maska;

	cv::GaussianBlur(slika, slikaVmesna, cv::Size(3, 3), 0);

	cv::cvtColor(slikaVmesna, slikaHSV, cv::COLOR_BGR2HSV);


	cv::Canny(slikaVmesna, slikaCanny, 100, 200);
	cv::dilate(slikaCanny, slikaCanny, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
	cv::erode(slikaCanny, slikaCanny, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));


	///
	//cv::imshow("canny", slikaCanny);
	//cv::imshow("HSV", slikaHSV);
	//cv::waitKey(0);
	//cv::destroyAllWindows();
	///




	cv::imshow("importImage()", slika);
	cv::waitKey(0);


	//const int locljivost = 18;
	const int sirinaObmocje = 30;
	const int korak = 6;

	int zacetek, konec;

	for (int loc = sirinaObmocje; loc <= 180; loc += korak) {

		zacetek = loc - sirinaObmocje;
		konec = loc;

		/////////////////////////////////

		int hmin = zacetek, smin = 40, vmin = 40;
		int hmax = konec, smax = 255, vmax = 255;

		cv::Scalar spodnja(hmin, smin, vmin);
		cv::Scalar zgornja(hmax, smax, vmax);

		cv::inRange(slikaHSV, spodnja, zgornja, maska);

		cv::imshow("maska", maska);
		cv::waitKey(100);


		std::cout << "ob: " << zacetek << " - " << konec << '\n';
	}
	cv::destroyAllWindows();



}

void prepoznavaZic_V0(cv::Mat slika) {





}


void prepoznajTekst(const std::string& pot, const int razsiritev = 10) {

	cv::Mat im = cv::imread(pot, cv::IMREAD_COLOR);
	if (im.empty()) {
		std::cout << "Slike \"" << pot << "\" ni bilo mogoce odpreti.\n";
		return;
	}

	tesseract::TessBaseAPI ocr = tesseract::TessBaseAPI();
	ocr.Init("C:/Dev/vcpkg/buildtrees/tesseract/tessdata", "eng", tesseract::OEM_DEFAULT);
	ocr.SetPageSegMode(tesseract::PSM_AUTO);
	ocr.SetImage(im.data, im.cols, im.rows, 3, im.step);

	//std::string outText = std::string(ocr.GetUTF8Text());
	//std::cout << outText;

	Boxa* boxes = ocr.GetWords(NULL);
	if (boxes) {

		const int count = boxaGetCount(boxes);
		for (int i = 0; i < count; ++i) {

			int x, y, w, h;
			boxaGetBoxGeometry(boxes, i, &x, &y, &w, &h);

			int width = im.cols;
			int height = im.rows;

			int novX = x - razsiritev;
			int novY = y - razsiritev;
			int novW = w + 2 * razsiritev;
			int novH = h + 2 * razsiritev;

			if (novX <= 0) novX = 0;
			if (novY <= 0) novY = 0;
			if ((novW + novX) >= width) novW = width - novX;
			if ((novH + novY) >= height) novH = height - novY;
			std::cout << "dimenzije : " << novX << ' ' << novY << ' ' << novW << ' ' << novH << '\n';

			ocr.SetRectangle(novX, novY, novW, novH);
			std::string outText = std::string(ocr.GetUTF8Text());
			std::cout << "zaznan text: " << outText << '\n' << '\n';

			cv::rectangle(im, cv::Rect(novX, novY, novW, novH), cv::Scalar(0, 255, 0));
		}
		boxaDestroy(&boxes);
	}

	cv::imshow("Obrisane besede", im);
	cv::waitKey(0);
	cv::destroyAllWindows();

	ocr.End();
}





int main() {


	const std::string& potVhod = "Seminar/Vhod";
	const std::string& potIzhod = "Seminar/Izhod";

	UporabniskiVmesnik uporabniskiVmesnik(potVhod, potIzhod);




	//std::string pot = "Seminar/Vhod/Text2.jpg";
	//prepoznajTekst(pot,5);

	
	//cv::Mat slika = cv::imread(pot);
	//
	//if (slika.empty()) {
	//	std::cout << "Slike \"" << pot << "\" ni bilo mogoce odpreti.\n";
	//	return 1;
	//}
	//
	//
	////prepoznavaBarv_V0(slika);
	//
	//prepoznajTekst(slika);
	//
	//
	//cv::imshow("importImage()", slika);
	//cv::waitKey(0);
	////while (cv::waitKey(1) != 32) cv::imshow("importImage()", slika);
	//
	//cv::destroyAllWindows();
	//
	//
	//return 0;


	//cv::Mat img(600, 800, CV_8UC3, cv::Scalar(255, 255, 255)); // Blue image
	//
	//cv::namedWindow("Blank Image", cv::WINDOW_AUTOSIZE);
	//cv::imshow("Blank Image", img);
	//cv::waitKey(0);
	//
	//cv::destroyAllWindows();



	return 0;
}
