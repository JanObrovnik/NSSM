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

	std::vector<cv::Point> vertikalneTocke(vertikalnaPrepoznava(slika));


}

std::vector<cv::Point> vertikalnaPrepoznava(cv::Mat slika) {

	std::vector<cv::Point> resitev;

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
	cv::waitKey(200);

	for (int x = 0; x < razsirjena.cols; x += 10) { ///////////////////// dat v novo funkcijo

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
			
			std::vector<cv::Point> seznamSrednjihTock(sredinaVecihTock_V0(seznamNajdenihVertikalnihTock));
			for (const cv::Point& tocka : seznamSrednjihTock) {
				//cv::circle(slika, tocka, 3, cv::Scalar(0, 255, 0), -1);
				resitev.push_back(tocka);
			}
		}
	}

	for (const cv::Point& tocka : resitev)
		cv::circle(slika, tocka, 3, cv::Scalar(0, 255, 0), -1);

	cv::imshow("Slika", slika);
	cv::waitKey(200);
	
	//horizontalnaPoravnava_V0(razsirjena, resitev);
	horizontalnaPoravnava_V1(razsirjena, resitev);

	for (const cv::Point& tocka : resitev)
		cv::circle(slika, tocka, 3, cv::Scalar(255, 0, 0), -1);

	cv::imshow("Slika", slika);
	cv::waitKey(200);

	std::vector<cv::Point> urejenSeznamTock = narisiPovezave_V0(slika, resitev, resitev.back()); /////////////////////////////
	//std::vector<cv::Point> urejenSeznamTock = narisiPovezave_V0(slika, resitev, *(resitev.begin() + 10));
	std::vector<int> urejenSeznamRazdalj = narediSeznamRazdalj(urejenSeznamTock);

	for (int i = 1; i < urejenSeznamTock.size(); i++) {
		cv::line(slika, urejenSeznamTock[i - 1], urejenSeznamTock[i], cv::Scalar(255, 0, 102), 3);
	}

	std::vector<int>::iterator itMax = std::max_element(urejenSeznamRazdalj.begin(), urejenSeznamRazdalj.end());
	int index = itMax - urejenSeznamRazdalj.begin();
	int najvecjaRazdalja = *itMax;

	if (index == 0) {
		std::vector<cv::Point> novUrejenSeznamTock = narisiPovezave_V0(slika, urejenSeznamTock, urejenSeznamTock.front());
		std::vector<int> novUrejenSeznamRazdalj = narediSeznamRazdalj(novUrejenSeznamTock);

		std::vector<int>::iterator novItMax = std::max_element(novUrejenSeznamRazdalj.begin(), novUrejenSeznamRazdalj.end());
		int novIndex = novItMax - novUrejenSeznamRazdalj.begin();
		int novaNajvecjaRazdalja = *novItMax;

		if (novaNajvecjaRazdalja > najvecjaRazdalja) {
			urejenSeznamTock = novUrejenSeznamTock;
			urejenSeznamRazdalj = novUrejenSeznamRazdalj;
			najvecjaRazdalja = novaNajvecjaRazdalja;
		}

		novUrejenSeznamTock = narisiPovezave_V0(slika, urejenSeznamTock, urejenSeznamTock.back());
		novUrejenSeznamRazdalj = narediSeznamRazdalj(novUrejenSeznamTock);

		novItMax = std::max_element(novUrejenSeznamRazdalj.begin(), novUrejenSeznamRazdalj.end());
		novIndex = novItMax - novUrejenSeznamRazdalj.begin();
		novaNajvecjaRazdalja = *novItMax;

		if (novaNajvecjaRazdalja > najvecjaRazdalja) {
			urejenSeznamTock = novUrejenSeznamTock;
			urejenSeznamRazdalj = novUrejenSeznamRazdalj;
			najvecjaRazdalja = novaNajvecjaRazdalja;
		}


		std::cout << "dela.\n";
	}
	else {
		//int novaNajvecjaRazdalja = manhattanRazdalja(resitev[index], resitev[index - 1]);
		//std::cout << "najvecja razdalja: " << novaNajvecjaRazdalja << ':' << resitev[index] << '|' << resitev[index - 1] << '\n';
		//if (novaNajvecjaRazdalja > najvecjaRazdalja)
		//	najvecjaRazdalja = novaNajvecjaRazdalja;

		std::vector<cv::Point> novUrejenSeznamTock = narisiPovezave_V0(slika, urejenSeznamTock, urejenSeznamTock[index]);
		std::vector<int> novUrejenSeznamRazdalj = narediSeznamRazdalj(novUrejenSeznamTock);

		std::vector<int>::iterator novItMax = std::max_element(novUrejenSeznamRazdalj.begin(), novUrejenSeznamRazdalj.end());
		int novIndex = novItMax - novUrejenSeznamRazdalj.begin();
		int novaNajvecjaRazdalja = *novItMax;

		if (novaNajvecjaRazdalja > najvecjaRazdalja) {
			urejenSeznamTock = novUrejenSeznamTock;
			urejenSeznamRazdalj = novUrejenSeznamRazdalj;
			najvecjaRazdalja = novaNajvecjaRazdalja;
		}

		novUrejenSeznamTock = narisiPovezave_V0(slika, urejenSeznamTock, urejenSeznamTock[index - 1]);
		novUrejenSeznamRazdalj = narediSeznamRazdalj(novUrejenSeznamTock);

		novItMax = std::max_element(novUrejenSeznamRazdalj.begin(), novUrejenSeznamRazdalj.end());
		novIndex = novItMax - novUrejenSeznamRazdalj.begin();
		novaNajvecjaRazdalja = *novItMax;

		if (novaNajvecjaRazdalja > najvecjaRazdalja) {
			urejenSeznamTock = novUrejenSeznamTock;
			urejenSeznamRazdalj = novUrejenSeznamRazdalj;
			najvecjaRazdalja = novaNajvecjaRazdalja;
		}

		std::cout << "cakanje.\n";
	}




	for (int i = 1; i < urejenSeznamTock.size(); i++) {
		cv::line(slika, urejenSeznamTock[i - 1], urejenSeznamTock[i], cv::Scalar(255, 255, 0));
	}
	//cv::line(slika, urejenSeznamTock.front(), urejenSeznamTock.back(), cv::Scalar(255, 255, 0));

	cv::imshow("Slika", slika);
	cv::waitKey(200);


	return resitev;
}
cv::Point sredinaTock_V0(const std::vector<cv::Point>& seznamTock) {
	cv::Point resitev(0, 0);
	for (const cv::Point& tocka : seznamTock) {
		resitev += tocka;
	}
	resitev /= static_cast<int>(seznamTock.size());
	return resitev;
}
inline cv::Point sredinaTock_V1(const std::vector<cv::Point>::const_iterator& it1, const std::vector<cv::Point>::const_iterator& it2) {
	return (*it1 + *it2) / 2;
}
inline cv::Point sredinaTock_V1(const cv::Point& to1, const cv::Point& to2)
{
	return (to1 + to2) / 2;
}
std::vector<cv::Point> sredinaVecihTock_V0(const std::vector<cv::Point>& seznamTock) {
	
	std::vector<cv::Point> resitev;

	std::vector<cv::Point>::const_iterator zacetnaTocka = seznamTock.cbegin();
	std::vector<cv::Point>::const_iterator tekocaTocka = zacetnaTocka;
	

	for (std::vector<cv::Point>::const_iterator it = zacetnaTocka + 1; it != seznamTock.cend(); it++) {
	
		//std::cout << "razdalja: " << manhattanRazdalja(*tekocaTocka, *it) << '\n';
		if (manhattanRazdalja(*tekocaTocka, *it) > 1) {
			//resitev.push_back(sredinaTock_V0(std::vector<cv::Point>(zacetnaTocka, it)));
			resitev.push_back(sredinaTock_V1(zacetnaTocka, tekocaTocka));
			zacetnaTocka = it;
		}

		tekocaTocka = it;
	}
	resitev.push_back(sredinaTock_V1(zacetnaTocka, tekocaTocka));

	return resitev;
}
inline int manhattanRazdalja(const cv::Point& to1, const cv::Point& to2) {
	return abs(to1.x - to2.x) + abs(to1.y - to2.y);
}

void horizontalnaPoravnava_V0(cv::Mat slika, std::vector<cv::Point>& seznamTock) {

	for (cv::Point& tocka : seznamTock) {

		cv::Point levaTocka = tocka;

		while (slika.at<uchar>(levaTocka) > 0) {
			levaTocka += cv::Point(-1, 0); ///////////////////// mogoc tocka preide izven okvirja slike
		}

		cv::Point desnaTocka = tocka;

		while (slika.at<uchar>(desnaTocka) > 0) {
			desnaTocka += cv::Point(1, 0); ///////////////////// mogoc tocka preide izven okvirja slike
		}

		tocka = sredinaTock_V1(levaTocka, desnaTocka);
	}
}
void horizontalnaPoravnava_V1(cv::Mat slika, std::vector<cv::Point>& seznamTock) {

	for (cv::Point& tocka : seznamTock) {

		cv::Point zgornjaTocka = tocka;

		while (slika.at<uchar>(zgornjaTocka) > 0) {
			zgornjaTocka += cv::Point(0, -1); ///////////////////// mogoc tocka preide izven okvirja slike
		}

		cv::Point spodnjaTocka = tocka;

		while (slika.at<uchar>(spodnjaTocka) > 0) {
			spodnjaTocka += cv::Point(0, 1); ///////////////////// mogoc tocka preide izven okvirja slike
		}

		cv::Point levaTocka = tocka;

		while (slika.at<uchar>(levaTocka) > 0) {
			levaTocka += cv::Point(-1, 0); ///////////////////// mogoc tocka preide izven okvirja slike
		}

		cv::Point desnaTocka = tocka;

		while (slika.at<uchar>(desnaTocka) > 0) {
			desnaTocka += cv::Point(1, 0); ///////////////////// mogoc tocka preide izven okvirja slike
		}

		if (manhattanRazdalja(levaTocka, desnaTocka) < manhattanRazdalja(zgornjaTocka, spodnjaTocka))
			tocka = sredinaTock_V1(levaTocka, desnaTocka);
	}
}

std::vector<cv::Point> narisiPovezave_V0(cv::Mat slika, const std::vector<cv::Point>& seznamTock, const cv::Point& zacetnaTocka) {
	///////////////////// slike ne rabimo
	static bool prvic = true;
	static int najvecjaRazdalja = 0;

	std::vector<cv::Point> seznamTockKopija = seznamTock;

	cv::Point tekocaTocka = zacetnaTocka;
	seznamTockKopija.erase(std::find(seznamTockKopija.begin(), seznamTockKopija.end(), tekocaTocka));

	//cv::Point tekocaTocka = *(seznamTock.begin() + 5);
	//seznamTock.erase(seznamTock.begin() + 5);

	std::vector<cv::Point> resitev{tekocaTocka };

	while (!seznamTockKopija.empty()) {

		std::vector<cv::Point>::reverse_iterator itMin = seznamTockKopija.rbegin();
		int vrednostMin = manhattanRazdalja(tekocaTocka, *itMin);

		for (std::vector<cv::Point>::reverse_iterator rit = seznamTockKopija.rbegin() + 1; rit != seznamTockKopija.rend(); rit++) {

			const int vrednostNova = manhattanRazdalja(tekocaTocka, *rit);

			if (vrednostNova < vrednostMin) {

				vrednostMin = vrednostNova;
				itMin = rit;
			}
		}
		//std::cout << vrednostMin << ':' << tekocaTocka << '|' << *itMin << '\n';
		resitev.push_back(*itMin);

		tekocaTocka = *itMin;

		seznamTockKopija.erase(itMin.base() - 1);
	}

	//std::vector<int> urejenSeznamRazdalj(resitev.size());
	//
	//for (int i = 1; i < resitev.size(); i++) {
	//	urejenSeznamRazdalj[i] = manhattanRazdalja(resitev[i - 1], resitev[i]);
	//}
	//urejenSeznamRazdalj.front() = manhattanRazdalja(resitev.front(), resitev.back());
	//
	//int index = std::max_element(urejenSeznamRazdalj.begin(), urejenSeznamRazdalj.end()) - urejenSeznamRazdalj.begin();
	//
	//if (index == 0) {
	//	int novaNajvecjaRazdalja = manhattanRazdalja(resitev.front(), resitev.back());
	//	std::cout << "najvecja razdalja: " << novaNajvecjaRazdalja << ':' << resitev.front() << '|' << resitev.back() << '\n';
	//	if (novaNajvecjaRazdalja > najvecjaRazdalja)
	//		najvecjaRazdalja = novaNajvecjaRazdalja;
	//}
	//else {
	//	int novaNajvecjaRazdalja = manhattanRazdalja(resitev[index], resitev[index - 1]);
	//	std::cout << "najvecja razdalja: " << novaNajvecjaRazdalja << ':' << resitev[index] << '|' << resitev[index - 1] << '\n';
	//	if (novaNajvecjaRazdalja > najvecjaRazdalja)
	//		najvecjaRazdalja = novaNajvecjaRazdalja;
	//}
	//
	//if (prvic && index != 0) {
	//	prvic = false;
	//	resitev = narisiPovezave_V0(slika, seznamTock, resitev[index]);
	//	resitev = narisiPovezave_V0(slika, seznamTock, resitev[index - 1]);
	//}
	//
	//
	//std::cout << "Najnajvecja razdalja: " << najvecjaRazdalja << '\n';

	return resitev;
}
std::vector<int> narediSeznamRazdalj(const std::vector<cv::Point>& seznamTock) {
	
	std::vector<int> resitev(seznamTock.size());

	for (int i = 1; i < seznamTock.size(); i++) {
		resitev[i] = manhattanRazdalja(seznamTock[i - 1], seznamTock[i]);
	}
	resitev.front() = manhattanRazdalja(seznamTock.front(), seznamTock.back());

	return resitev;
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
