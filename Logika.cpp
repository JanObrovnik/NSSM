#include "Logika.h"


std::vector<std::pair<cv::Scalar, cv::Scalar>> prepoznavaBarv_V0(cv::Mat slika) {

	std::vector<std::pair<cv::Scalar, cv::Scalar>> resitev;


	cv::Mat slikaVmesna, slikaCanny, slikaHSV, maska;

	cv::GaussianBlur(slika, slikaVmesna, cv::Size(3, 3), 0);

	cv::cvtColor(slikaVmesna, slikaHSV, cv::COLOR_BGR2HSV); //////////////// brisi ko das v BGR


	const int sirinaObmocje = 6;
	const int korak = 6;

	int zacetek, konec;

	for (int loc = sirinaObmocje; loc <= 180; loc += korak) {

		zacetek = loc - sirinaObmocje;
		konec = loc;

		int hmin = zacetek, smin = 40, vmin = 40; /////////////////// pretvori v BGR
		int hmax = konec, smax = 255, vmax = 255;

		cv::Scalar spodnja(hmin, smin, vmin);
		cv::Scalar zgornja(hmax, smax, vmax);

		cv::inRange(slikaHSV, spodnja, zgornja, maska);

		if (cv::countNonZero(maska) != 0) {

			cv::imshow("Slika", maska);

			int k;

			NEVELJAVEN_ZNAK:

			k = cv::waitKey(0);
			//std::cout << k << '\n';

			if (k == 13) resitev.push_back({ spodnja,zgornja }); // enter
			else if (k == 8); // backspace
			else if (k == 113) break; // q
			else goto NEVELJAVEN_ZNAK;
		}
	}

	std::cout << "resitev:" << resitev.size() << '\n';

	cv::imshow("Slika", slika);
	cv::waitKey(200);


	return resitev;
}
std::vector<std::pair<cv::Scalar, cv::Scalar>> prepoznavaBarv_V1(cv::Mat slika) {

	std::vector<std::pair<cv::Scalar, cv::Scalar>> resitev;


	cv::Mat slikaVmesna, maska;

	cv::GaussianBlur(slika, slikaVmesna, cv::Size(3, 3), 0);


	int sirinaOkna = 32;

	bool izhod = false;
	for (int b = 0; b < 256; b += sirinaOkna) {
		for (int g = 0; g < 256; g += sirinaOkna) {
			for (int r = 0; r < 256; r += sirinaOkna) {

				cv::Scalar spodnja(b, g, r);
				cv::Scalar zgornja(b + sirinaOkna, g + sirinaOkna, r + sirinaOkna);

				cv::inRange(slikaVmesna, spodnja, zgornja, maska);

				if (cv::countNonZero(maska) > 50 && cv::countNonZero(maska) < maska.cols * maska.rows / 2) {

					cv::imshow("Slika", maska);

					int k;

					NEVELJAVEN_ZNAK:

					k = cv::waitKey(0);
					//std::cout << k << '\n';

					if (k == 13) resitev.push_back({ spodnja,zgornja }); // enter
					else if (k == 8); // backspace
					else if (k == 113) izhod = true; // q
					else goto NEVELJAVEN_ZNAK;
				}

				if (izhod) break;
			}
			if (izhod) break;
		}
		if (izhod) break;
	}

	//std::cout << "resitev:" << resitev.size() << '\n';

	cv::imshow("Slika", slika);

	return resitev;
}
std::vector<std::pair<cv::Scalar, cv::Scalar>> prepoznavaBarv_V2(cv::Mat slika) {

	std::vector<std::pair<cv::Scalar, cv::Scalar>> resitev;

	std::vector<cv::Scalar> dataBaza{
		cv::Scalar(32,29,234),
		cv::Scalar(199,72,65),
		cv::Scalar(5,240,252),
		cv::Scalar(22,0,136),
		cv::Scalar(73,177,35),
		cv::Scalar(162,72,163),
	};

	cv::Mat slikaObdelana, maska;

	cv::GaussianBlur(slika, slikaObdelana, cv::Size(3, 3), 0);

	int polmerOknaBarve = 10; /////////////// mogoc vecji

	for (const cv::Scalar& meja : dataBaza) {

		cv::Scalar spodnja(meja[0] - polmerOknaBarve, meja[1] - polmerOknaBarve, meja[2] - polmerOknaBarve);
		cv::Scalar zgornja(meja[0] + polmerOknaBarve, meja[1] + polmerOknaBarve, meja[2] + polmerOknaBarve);

		cv::inRange(slikaObdelana, spodnja, zgornja, maska);

		if (cv::countNonZero(maska) != 0) {

			cv::imshow("Slika", maska);

			int k;

		NEVELJAVEN_ZNAK:

			k = cv::waitKey(0);
			//std::cout << k << '\n';

			if (k == 13) resitev.push_back({ spodnja,zgornja }); // enter
			else if (k == 8); // backspace
			else if (k == 113) break; // q
			else goto NEVELJAVEN_ZNAK;
		}
	}

	cv::imshow("Slika", slika);

	return resitev;
}


void prepoznavaZic_V0(cv::Mat slika, const std::vector<std::pair<cv::Scalar, cv::Scalar>>& barve) {

	cv::Mat maska;
	izdelavaMaske(slika, maska, barve);

	int korak = 10;
	std::vector<cv::Point> vertikalneTocke(vertikalnaPrepoznava(maska, korak));
	std::vector<cv::Point> horizontalneTocke(horizontalnaPrepoznava(maska, korak));

	std::vector<cv::Point> seznamTock = zdruzitevSeznamovTock_V0(vertikalneTocke, horizontalneTocke, korak);

	for (const cv::Point& tocka : seznamTock)
		cv::circle(slika, tocka, 3, cv::Scalar(0, 255, 0), -1);
	cv::imshow("Slika", slika);
	cv::waitKey(200);


	///////////////////////funkcija za povazave 5
	//narisiPovezave_V1();
}

void izdelavaMaske(cv::Mat slika, cv::Mat& maska, const std::vector<std::pair<cv::Scalar, cv::Scalar>>& barve) {

	const cv::Scalar& spodnjaMeja = barve.front().first; /////////////////// deluje samo za prvo
	const cv::Scalar& zgornjaMeja = barve.front().second;

	cv::Mat gauss;

	//////////////
	//////////////////// sam za test, pol bo BGR ///////////// brisi
	//cv::GaussianBlur(slika, gauss, cv::Size(3, 3), 0);		   ///////////// brisi
	//cv::cvtColor(gauss, gauss, cv::COLOR_BGR2HSV);			   ///////////// brisi
	////////////////
	
	cv::GaussianBlur(slika, gauss, cv::Size(3, 3), 0); ///////////////// dodaj nazaj
	cv::inRange(gauss, spodnjaMeja, zgornjaMeja, maska);
	//cv::imshow("Slika", maska);
	//cv::waitKey(0);
	
	cv::dilate(maska, maska, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
	//cv::imshow("Slika", maska);
	//cv::waitKey(0);
}

std::vector<cv::Point> vertikalnaPrepoznava(cv::Mat maska, int korak) {

	std::vector<cv::Point> resitev;

	for (int x = 0; x < maska.cols; x += korak) {

		std::vector<cv::Point> seznamNajdenihVertikalnihTock{};

		for (int y = 0; y < maska.rows; y++) {

			if (maska.at<uchar>(y, x) > 0) {

				seznamNajdenihVertikalnihTock.push_back(cv::Point(x, y));
			}
		}

		if (seznamNajdenihVertikalnihTock.size() > 0) {
			
			std::vector<cv::Point> seznamSrednjihTock(sredinaVecihTock_V0(seznamNajdenihVertikalnihTock));

			for (const cv::Point& tocka : seznamSrednjihTock) {
				resitev.push_back(tocka);
			}
		}
	}

	horizontalnaPoravnava_V1(maska, resitev);

	return resitev;
}
void horizontalnaPoravnava_V0(cv::Mat maska, std::vector<cv::Point>& seznamTock) {

	for (cv::Point& tocka : seznamTock) {

		cv::Point levaTocka = tocka;

		while (maska.at<uchar>(levaTocka) > 0) {
			levaTocka += cv::Point(-1, 0); ///////////////////// mogoc tocka preide izven okvirja slike
		}

		cv::Point desnaTocka = tocka;

		while (maska.at<uchar>(desnaTocka) > 0) {
			desnaTocka += cv::Point(1, 0); ///////////////////// mogoc tocka preide izven okvirja slike
		}

		tocka = sredinaTock_V1(levaTocka, desnaTocka);
	}
}
void horizontalnaPoravnava_V1(cv::Mat maska, std::vector<cv::Point>& seznamTock) {

	for (cv::Point& tocka : seznamTock) {

		cv::Point zgornjaTocka, spodnjaTocka, levaTocka, desnaTocka;

		skrajneTocke(maska, tocka, desnaTocka, zgornjaTocka, levaTocka, spodnjaTocka);

		if (manhattanRazdalja(levaTocka, desnaTocka) < manhattanRazdalja(zgornjaTocka, spodnjaTocka))
			tocka = sredinaTock_V1(levaTocka, desnaTocka);
	}
}

std::vector<cv::Point> horizontalnaPrepoznava(cv::Mat maska, int korak) {

	std::vector<cv::Point> resitev;

	for (int y = 0; y < maska.rows; y += korak) {

		std::vector<cv::Point> seznamNajdenihVertikalnihTock{};

		for (int x = 0; x < maska.cols; x++) {

			if (maska.at<uchar>(y, x) > 0) {

				seznamNajdenihVertikalnihTock.push_back(cv::Point(x, y));
			}
		}

		if (seznamNajdenihVertikalnihTock.size() > 0) {

			std::vector<cv::Point> seznamSrednjihTock(sredinaVecihTock_V0(seznamNajdenihVertikalnihTock));
			for (const cv::Point& tocka : seznamSrednjihTock) {
				resitev.push_back(tocka);
			}
		}
	}

	vertikalnaPoravnava_V1(maska, resitev);

	return resitev;
}
void vertikalnaPoravnava_V1(cv::Mat maska, std::vector<cv::Point>& seznamTock) {

	for (cv::Point& tocka : seznamTock) {

		cv::Point zgornjaTocka, spodnjaTocka, levaTocka, desnaTocka;

		skrajneTocke(maska, tocka, desnaTocka, zgornjaTocka, levaTocka, spodnjaTocka);

		if (manhattanRazdalja(zgornjaTocka, spodnjaTocka) < manhattanRazdalja(levaTocka, desnaTocka))
			tocka = sredinaTock_V1(zgornjaTocka, spodnjaTocka);
	}
}

std::vector<cv::Point> zdruzitevSeznamovTock_V0(const std::vector<cv::Point>& sez1, const std::vector<cv::Point>& sez2, const int& filter) {

	std::vector<cv::Point> resitev(sez1);

	for (const cv::Point& tocka : sez2)
		if (std::none_of(sez1.begin(), sez1.end(), [&](const cv::Point& to) {return manhattanRazdalja(tocka, to) < filter; }))
			resitev.push_back(tocka);
	
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
	
		if (manhattanRazdalja(*tekocaTocka, *it) > 1) {

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
void skrajneTocke(cv::Mat maska, const cv::Point& tocka, cv::Point& desnaTocka, cv::Point& zgornjaTocka, cv::Point& levaTocka, cv::Point& spodnjaTocka) {

	zgornjaTocka = tocka;
	while (maska.at<uchar>(zgornjaTocka) > 0) {
		zgornjaTocka += cv::Point(0, -1); ///////////////////// mogoc tocka preide izven okvirja slike
	}

	spodnjaTocka = tocka;
	while (maska.at<uchar>(spodnjaTocka) > 0) {
		spodnjaTocka += cv::Point(0, 1); ///////////////////// mogoc tocka preide izven okvirja slike
	}

	levaTocka = tocka;
	while (maska.at<uchar>(levaTocka) > 0) {
		levaTocka += cv::Point(-1, 0); ///////////////////// mogoc tocka preide izven okvirja slike
	}

	desnaTocka = tocka;
	while (maska.at<uchar>(desnaTocka) > 0) {
		desnaTocka += cv::Point(1, 0); ///////////////////// mogoc tocka preide izven okvirja slike
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
std::vector<cv::Point> narisiPovezave_V1(cv::Mat slika, const std::vector<cv::Point>& seznamTock, const cv::Point& zacetnaTocka) {
	std::vector<cv::Point> resitev;

	//std::vector<cv::Point> urejenSeznamTock = narisiPovezave_V0(slika, resitev, resitev.back()); /////////////////////////////
	////std::vector<cv::Point> urejenSeznamTock = narisiPovezave_V0(slika, resitev, *(resitev.begin() + 10));
	//std::vector<int> urejenSeznamRazdalj = narediSeznamRazdalj(urejenSeznamTock);
	//
	//for (int i = 1; i < urejenSeznamTock.size(); i++) {
	//	cv::line(slika, urejenSeznamTock[i - 1], urejenSeznamTock[i], cv::Scalar(255, 0, 102), 3);
	//}
	//
	//std::vector<int>::iterator itMax = std::max_element(urejenSeznamRazdalj.begin(), urejenSeznamRazdalj.end());
	//int index = itMax - urejenSeznamRazdalj.begin();
	//int najvecjaRazdalja = *itMax;
	//
	//if (index == 0) {
	//	std::vector<cv::Point> novUrejenSeznamTock = narisiPovezave_V0(slika, urejenSeznamTock, urejenSeznamTock.front());
	//	std::vector<int> novUrejenSeznamRazdalj = narediSeznamRazdalj(novUrejenSeznamTock);
	//
	//	std::vector<int>::iterator novItMax = std::max_element(novUrejenSeznamRazdalj.begin(), novUrejenSeznamRazdalj.end());
	//	int novIndex = novItMax - novUrejenSeznamRazdalj.begin();
	//	int novaNajvecjaRazdalja = *novItMax;
	//
	//	if (novaNajvecjaRazdalja > najvecjaRazdalja) {
	//		urejenSeznamTock = novUrejenSeznamTock;
	//		urejenSeznamRazdalj = novUrejenSeznamRazdalj;
	//		najvecjaRazdalja = novaNajvecjaRazdalja;
	//	}
	//
	//	novUrejenSeznamTock = narisiPovezave_V0(slika, urejenSeznamTock, urejenSeznamTock.back());
	//	novUrejenSeznamRazdalj = narediSeznamRazdalj(novUrejenSeznamTock);
	//
	//	novItMax = std::max_element(novUrejenSeznamRazdalj.begin(), novUrejenSeznamRazdalj.end());
	//	novIndex = novItMax - novUrejenSeznamRazdalj.begin();
	//	novaNajvecjaRazdalja = *novItMax;
	//
	//	if (novaNajvecjaRazdalja > najvecjaRazdalja) {
	//		urejenSeznamTock = novUrejenSeznamTock;
	//		urejenSeznamRazdalj = novUrejenSeznamRazdalj;
	//		najvecjaRazdalja = novaNajvecjaRazdalja;
	//	}
	//
	//
	//	std::cout << "dela.\n";
	//}
	//else {
	//	//int novaNajvecjaRazdalja = manhattanRazdalja(resitev[index], resitev[index - 1]);
	//	//std::cout << "najvecja razdalja: " << novaNajvecjaRazdalja << ':' << resitev[index] << '|' << resitev[index - 1] << '\n';
	//	//if (novaNajvecjaRazdalja > najvecjaRazdalja)
	//	//	najvecjaRazdalja = novaNajvecjaRazdalja;
	//
	//	std::vector<cv::Point> novUrejenSeznamTock = narisiPovezave_V0(slika, urejenSeznamTock, urejenSeznamTock[index]);
	//	std::vector<int> novUrejenSeznamRazdalj = narediSeznamRazdalj(novUrejenSeznamTock);
	//
	//	std::vector<int>::iterator novItMax = std::max_element(novUrejenSeznamRazdalj.begin(), novUrejenSeznamRazdalj.end());
	//	int novIndex = novItMax - novUrejenSeznamRazdalj.begin();
	//	int novaNajvecjaRazdalja = *novItMax;
	//
	//	if (novaNajvecjaRazdalja > najvecjaRazdalja) {
	//		urejenSeznamTock = novUrejenSeznamTock;
	//		urejenSeznamRazdalj = novUrejenSeznamRazdalj;
	//		najvecjaRazdalja = novaNajvecjaRazdalja;
	//	}
	//
	//	novUrejenSeznamTock = narisiPovezave_V0(slika, urejenSeznamTock, urejenSeznamTock[index - 1]);
	//	novUrejenSeznamRazdalj = narediSeznamRazdalj(novUrejenSeznamTock);
	//
	//	novItMax = std::max_element(novUrejenSeznamRazdalj.begin(), novUrejenSeznamRazdalj.end());
	//	novIndex = novItMax - novUrejenSeznamRazdalj.begin();
	//	novaNajvecjaRazdalja = *novItMax;
	//
	//	if (novaNajvecjaRazdalja > najvecjaRazdalja) {
	//		urejenSeznamTock = novUrejenSeznamTock;
	//		urejenSeznamRazdalj = novUrejenSeznamRazdalj;
	//		najvecjaRazdalja = novaNajvecjaRazdalja;
	//	}
	//
	//	std::cout << "cakanje.\n";
	//}
	//
	//
	//
	//
	//for (int i = 1; i < urejenSeznamTock.size(); i++) {
	//	cv::line(slika, urejenSeznamTock[i - 1], urejenSeznamTock[i], cv::Scalar(255, 255, 0));
	//}
	////cv::line(slika, urejenSeznamTock.front(), urejenSeznamTock.back(), cv::Scalar(255, 255, 0));
	//
	//cv::imshow("Slika", slika);
	//cv::waitKey(200);

	return resitev;
}
std::vector<cv::Point> narisiPovezave_V2(cv::Mat slika, const std::vector<cv::Point>& seznamTock, const cv::Point& zacetnaTocka) {

	return std::vector<cv::Point>();
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
