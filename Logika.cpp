#include "Logika.h"


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

	//cv::imshow("Slika", slika);
	//cv::waitKey(0);


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

		cv::imshow("Slika", maska);
		cv::waitKey(100);


		//std::cout << "ob: " << zacetek << " - " << konec << '\n';
	}
}


void prepoznavaZic_V0(cv::Mat slika) {

	int obmocje = 10;

	int b = 36;  // b /////////////////////////////
	int g = 28 ; // g
	int r = 237; // r

	cv::Scalar spodnjaMeja(b - obmocje, g - obmocje, r - obmocje);
	cv::Scalar zgornjaMeja(b + obmocje, g + obmocje, r + obmocje);
	
	cv::Mat gauss, maska, razsirjena;

	cv::GaussianBlur(slika, gauss, cv::Size(3, 3), 0);

	cv::inRange(gauss, spodnjaMeja, zgornjaMeja, maska);

	cv::dilate(maska, razsirjena, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
	//cv::erode(razsirjena, razsirjena, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));

	cv::imshow("Slika", razsirjena);
	cv::waitKey(1000);

	for (int x = 0; x < razsirjena.cols; x += 10) {

		std::vector<cv::Point> seznamNajdenihVertikalnihTock{};

		for (int y = 0; y < razsirjena.rows; y++) {

			if (razsirjena.at<uchar>(y, x) > 0) {
				//std::cout << "pix (y = " << i << "): " << (int)razsirjena.at<uchar>(i, j) << '\n';
				//std::cout << "Tocka (y,x) : (" << y << ',' << x << ")\n";

				seznamNajdenihVertikalnihTock.push_back(cv::Point(x, y));
				//cv::circle(slika, cv::Point(x, y), 3, cv::Scalar(0, 255, 0), -1);
			}
		}
		if (seznamNajdenihVertikalnihTock.size() > 0) {
			//cv::circle(slika, sredinaTock_V0(seznamNajdenihVertikalnihTock), 3, cv::Scalar(0, 255, 0), -1);
			//std::cout << "velikost: " << seznamNajdenihVertikalnihTock.size() << '\n';
			
			std::vector<cv::Point> seznamSrednjihTock(sredinaTock_V1(seznamNajdenihVertikalnihTock));
			for (const cv::Point& tocka : seznamSrednjihTock) {
				cv::circle(slika, tocka, 3, cv::Scalar(0, 255, 0), -1); //////////////////// vse tocke v en seznam
			}
		}
	}

	cv::imshow("Slika", slika);
}

cv::Point sredinaTock_V0(const std::vector<cv::Point>& seznamTock) {
	cv::Point resitev(0, 0);
	for (const cv::Point& tocka : seznamTock) {
		resitev += tocka;
	}
	resitev /= static_cast<int>(seznamTock.size());
	return resitev;
}

std::vector<cv::Point> sredinaTock_V1(const std::vector<cv::Point>& seznamTock) {
	
	std::vector<cv::Point> resitev;

	std::vector<cv::Point>::const_iterator zacetnaTocka = seznamTock.cbegin();
	std::vector<cv::Point>::const_iterator tekocaTocka = zacetnaTocka;
	

	for (std::vector<cv::Point>::const_iterator it = zacetnaTocka + 1; it != seznamTock.cend(); it++) {
	
		//std::cout << "razdalja: " << manhattanRazdalja(*tekocaTocka, *it) << '\n';
		if (manhattanRazdalja(*tekocaTocka, *it) > 1) {
			resitev.push_back(sredinaTock_V0(std::vector<cv::Point>(zacetnaTocka, it)));
			zacetnaTocka = it;
		}

		tekocaTocka = it;
	}
	resitev.push_back(sredinaTock_V0(std::vector<cv::Point>(zacetnaTocka, seznamTock.cend())));

	return resitev;
}

inline int manhattanRazdalja(const cv::Point& to1, const cv::Point& to2) {
	return abs(to1.x - to2.x) + abs(to1.y - to2.y);
}


void prepoznajTekst_V0(cv::Mat slika, const int razsiritev) {

	cv::Mat obdelanaSlika;

	cv::cvtColor(slika, obdelanaSlika, cv::COLOR_BGR2GRAY);

	tesseract::TessBaseAPI ocr = tesseract::TessBaseAPI();
	ocr.Init("C:/Dev/vcpkg/buildtrees/tesseract/tessdata", "eng", tesseract::OEM_DEFAULT);
	ocr.SetPageSegMode(tesseract::PSM_AUTO);
	ocr.SetImage(obdelanaSlika.data, obdelanaSlika.cols, obdelanaSlika.rows, 1, obdelanaSlika.step);

	Boxa* boxes = ocr.GetWords(NULL);
	if (boxes) {

		const int count = boxaGetCount(boxes);
		for (int i = 0; i < count; ++i) {

			int x, y, w, h;
			boxaGetBoxGeometry(boxes, i, &x, &y, &w, &h);

			int width = obdelanaSlika.cols;
			int height = obdelanaSlika.rows;

			int novX = x - razsiritev;
			int novY = y - razsiritev;
			int novW = w + 2 * razsiritev;
			int novH = h + 2 * razsiritev;

			if (novX <= 0) novX = 0;
			if (novY <= 0) novY = 0;
			if ((novW + novX) >= width) novW = width - novX;
			if ((novH + novY) >= height) novH = height - novY;
			//std::cout << "dimenzije : " << novX << ' ' << novY << ' ' << novW << ' ' << novH << '\n';

			ocr.SetRectangle(novX, novY, novW, novH);
			std::string outText = std::string(ocr.GetUTF8Text());
			//std::cout << "zaznan text: " << outText << '\n' << '\n';

			cv::rectangle(slika, cv::Rect(novX, novY, novW, novH), cv::Scalar(0, 255, 0));
		}
		boxaDestroy(&boxes);
	}

	cv::imshow("Slika", slika);

	ocr.End();
}
