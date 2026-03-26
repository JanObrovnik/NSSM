#include "Logika.h"


bool operator==(const Tocka& to1, const Tocka& to2) {
	return to1.x == to2.x && to1.y == to2.y;
}
bool operator< (const Tocka& to1, const Tocka& to2) {
	return (to1.x < to2.x) || (to1.x == to2.x && to1.y < to2.y);
}
std::ostream& operator<<(std::ostream& os, const Tocka& to) {
	return os << '[' << to.x << ',' << to.y << ']';
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

			if (k == 13) resitev.push_back({ spodnja,zgornja }); // enter
			else if (k == 8); // backspace
			else if (k == 113) break; // q
			else goto NEVELJAVEN_ZNAK;
		}
	}

	cv::imshow("Slika", slika);

	return resitev;
}


std::vector<cv::Point> prepoznavaZic_V3(cv::Mat slika, const std::vector<std::pair<cv::Scalar, cv::Scalar>>& barve) {

	std::vector<cv::Point> resitev;

	cv::Mat slikaKopija;
	slika.copyTo(slikaKopija);

	for (const std::pair<cv::Scalar, cv::Scalar>& barva : barve) {

		cv::Mat maska;
		izdelavaMaske_V2(slikaKopija, maska, barva);

		std::vector<std::vector<cv::Point>> konture;
		std::vector<cv::Vec4i> hierarchy;
		cv::findContours(maska, konture, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		std::vector<cv::Point> seznamKoncev;

		for (const auto& kontura : konture) {

			cv::Mat konturnaMaska = cv::Mat::zeros(maska.size(), CV_8UC1);
			cv::drawContours(konturnaMaska, std::vector<std::vector<cv::Point>>{kontura}, -1, cv::Scalar(255), -1);

			cv::Mat delnaMaska;
			maska.copyTo(delnaMaska, konturnaMaska);

			std::vector<cv::Point> seznamTock = prepoznavaVsehTock(delnaMaska);

			std::vector<cv::Point> robneTocke = narisiPovezave_V3(delnaMaska, seznamTock);

			seznamKoncev.push_back(robneTocke.front()); /////////////////// mogoce drugacen kontejner
			seznamKoncev.push_back(robneTocke.back());
			cv::line(slika, robneTocke.front(), robneTocke.back(), cv::Scalar(0, 255, 0));
		}

		//std::cout << "seznamKoncev.size(): " << seznamKoncev.size() << '\n';

		std::vector<cv::Point> prostaKonca;

		if (seznamKoncev.size() > 2) {

			std::vector<cv::Point> seznamPovezavKoncev = narisiPovezaveKoncev_V0(seznamKoncev);

			for (int i = 1; i < seznamPovezavKoncev.size(); i += 2) {
				//std::cout << seznamPovezavKoncev[i - 1] << ' ' << seznamPovezavKoncev[i] << '\n';
				cv::arrowedLine(slika, seznamPovezavKoncev[i - 1], seznamPovezavKoncev[i], cv::Scalar(153, 0, 102)); ///////////////
			}

			prostaKonca = narisiProstaKonca0_V0(seznamPovezavKoncev);
		}
		else {
			prostaKonca = seznamKoncev;
		}

		cv::circle(slika, prostaKonca.front(), 5, cv::Scalar(0, 0, 0), -1);
		cv::circle(slika, prostaKonca.back(), 5, cv::Scalar(0, 0, 0), -1);

		resitev.push_back(prostaKonca.front());
		resitev.push_back(prostaKonca.back());
	}

	cv::imshow("Slika", slika);

	return resitev;
}

void izdelavaMaske_V2(cv::Mat slika, cv::Mat& maska, const std::pair<cv::Scalar, cv::Scalar>& barva) {

	const cv::Scalar& spodnjaMeja = barva.first;
	const cv::Scalar& zgornjaMeja = barva.second;

	cv::Mat gauss;

	cv::GaussianBlur(slika, gauss, cv::Size(3, 3), 0);
	cv::inRange(gauss, spodnjaMeja, zgornjaMeja, maska);

	cv::dilate(maska, maska, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));

	cv::ximgproc::thinning(maska, maska, cv::ximgproc::ThinningTypes::THINNING_ZHANGSUEN);
	//cv::ximgproc::thinning(maska, maska2, cv::ximgproc::ThinningTypes::THINNING_GUOHALL);
}

std::vector<cv::Point> prepoznavaVsehTock(cv::Mat& maska) {

	std::vector<cv::Point> resitev;

	cv::findNonZero(maska, resitev);

	auto it = std::remove_if(resitev.begin(), resitev.end(), [&](const cv::Point& tocka) {
		const int& x = tocka.x;
		const int& y = tocka.y;

		if (x == 0 || x == (maska.cols - 1) || y == 0 || y == (maska.rows - 1)) {
			maska.at<uchar>(tocka) = 0;
			return true;
		}
		else return false;
		});

	resitev.erase(it, resitev.end());

	return resitev;
}

inline int manhattanRazdalja(const cv::Point& to1, const cv::Point& to2) {
	return abs(to1.x - to2.x) + abs(to1.y - to2.y);
}

std::vector<cv::Point> narisiPovezave_V3(cv::Mat maska, const std::vector<cv::Point>& seznamTock) {

	std::vector<cv::Point> resitev;

	for (const cv::Point& tocka : seznamTock) {

		const int& x = tocka.x;
		const int& y = tocka.y;

		//if (x == 0 || x == maska.cols - 1 || y == 0 || y == maska.rows - 1)
		//	continue;

		int stSosed = -1;

		for (int xx : { -1,0,1 }) {
			for (int yy : { -1,0,1 }) {
				stSosed += (maska.at<uchar>(cv::Point(x + xx, y + yy)) != 0);
			}
		}

		if (stSosed == 1)
			resitev.push_back(tocka);
	}

	return resitev;
}

std::vector<cv::Point> narisiPovezaveKoncev_V0(const std::vector<cv::Point>& seznamTock) {

	if (seznamTock.size() < 4)
		return seznamTock;

	std::vector<cv::Point> resitev;

	for (int i = 0; i < seznamTock.size(); i++) {

		int minRazdaljaIndex = i + 2;
		if (minRazdaljaIndex >= seznamTock.size()) minRazdaljaIndex -= 4;
		int minRazdalja = manhattanRazdalja(seznamTock[i], seznamTock[minRazdaljaIndex]);

		for (int j = 0; j < seznamTock.size(); j++) {
			
			if (i / 2 == j / 2)
				continue;

			const int novaRazdalja = manhattanRazdalja(seznamTock[i], seznamTock[j]);

			if (novaRazdalja < minRazdalja) {
				minRazdalja = novaRazdalja;
				minRazdaljaIndex = j;
			}
		}

		resitev.push_back(seznamTock[i]);
		resitev.push_back(seznamTock[minRazdaljaIndex]);
	}

	return resitev;
}
std::vector<cv::Point> narisiProstaKonca0_V0(const std::vector<cv::Point>& seznamTock) {

	std::vector<cv::Point> resitev;

	std::map<const Tocka, int> mapaTock;

	for (const cv::Point& tocka : seznamTock) {
		mapaTock[tocka]++;
	}

	for (const std::pair<const Tocka, int>& par : mapaTock) {
		//std::cout << par.first << ": " << par.second << '\n';
		if (par.second == 1)
			resitev.push_back(cv::Point(par.first.x, par.first.y));
	}

	if (resitev.empty()) {
		resitev = narisiProstaKonca1_V0(seznamTock);
	}

	return resitev;
}
std::vector<cv::Point> narisiProstaKonca1_V0(const std::vector<cv::Point>& seznamTock) {

	std::vector<cv::Point> resitev;

	int maxRazdalja = 0;
	int maxIndex = -1;

	for (int i = 1; i < seznamTock.size(); i += 2) {
		//std::cout << seznamTock[i - 1] << ' ' << seznamTock[i] << '\n';

		const int novaRazdalja = manhattanRazdalja(seznamTock[i - 1], seznamTock[i]);

		if (novaRazdalja > maxRazdalja) {
			maxRazdalja = novaRazdalja;
			maxIndex = i;
		}
	}

	resitev.push_back(seznamTock[maxIndex]);
	resitev.push_back(seznamTock[maxIndex - 1]);

	return resitev;
}


std::vector<TextZica> prepoznajTekst_V1(cv::Mat slika, const std::vector<cv::Point>& seznamTock, const int razsiritev) {

	std::vector<TextZica> resitev;

	cv::Mat obdelanaSlika;

	cv::cvtColor(slika, obdelanaSlika, cv::COLOR_BGR2GRAY);

	tesseract::TessBaseAPI ocr = tesseract::TessBaseAPI();
	ocr.Init("C:/Dev/vcpkg/buildtrees/tesseract/tessdata", "eng", tesseract::OEM_DEFAULT);
	ocr.SetPageSegMode(tesseract::PSM_AUTO);
	ocr.SetImage(obdelanaSlika.data, obdelanaSlika.cols, obdelanaSlika.rows, 1, obdelanaSlika.step);

	Boxa* boxes = ocr.GetWords(NULL);
	if (boxes) {

		std::vector<cv::Point> seznamTockTexta;
		std::vector<std::string> seznamStringTexta;
		std::vector<int> seznamIndexovTexta;

		const int count = boxaGetCount(boxes);
		for (int i = 0; i < count; ++i) {

			int x, y, w, h;
			boxaGetBoxGeometry(boxes, i, &x, &y, &w, &h);

			seznamTockTexta.push_back(cv::Point(x + w / 2, y + h / 2));

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
			seznamStringTexta.push_back(outText);
			//std::cout << "zaznan text: " << outText << '\n' << '\n';

			cv::rectangle(slika, cv::Rect(novX, novY, novW, novH), cv::Scalar(0, 255, 0));
		}
		boxaDestroy(&boxes);

		seznamIndexovTexta = poveziBliznjeTocke_V0(slika, seznamTock, seznamTockTexta);

		TextZica textZica;

		for (int i = 0; i < seznamTock.size(); i++) {

			textZica.tockaZica = seznamTock[i];
			textZica.tockaText = seznamTockTexta[seznamIndexovTexta[i]];
			textZica.text = seznamStringTexta[seznamIndexovTexta[i]];

			resitev.push_back(textZica);
		}
	}

	for (const TextZica& tz : resitev) cv::arrowedLine(slika, tz.tockaZica, tz.tockaText, cv::Scalar(255, 0, 255), 2);

	cv::imshow("Slika", slika);

	ocr.End();

	return resitev;
}

std::vector<int> poveziBliznjeTocke_V0(cv::Mat slika, const std::vector<cv::Point>& seznamTock, const std::vector<cv::Point>& seznamTockTexta) {

	std::vector<int> resitev;
	resitev.reserve(seznamTock.size());

	//std::cout << "vel: " << seznamTock.size() << '\n';
	//std::cout << "vel: " << seznamTockTexta.size() << '\n';

	for (const cv::Point& tocka : seznamTock) {

		int minIndex = 0;
		int minRazdaja = manhattanRazdalja(tocka, seznamTockTexta[minIndex]);

		for (int i = 1; i < seznamTockTexta.size(); i++) {

			const int novaRazdalja = manhattanRazdalja(tocka, seznamTockTexta[i]);

			if (novaRazdalja < minRazdaja) {
				minRazdaja = novaRazdalja;
				minIndex = i;
			}
		}

		resitev.push_back(minIndex);
	}

	return resitev;
}
