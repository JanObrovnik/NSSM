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

	static std::vector<std::pair<cv::Scalar, cv::Scalar>> barvnaBaza{
		{cv::Scalar(130,32,32),cv::Scalar(150,255,255)},	// vijocna
		{cv::Scalar(107,160,80),cv::Scalar(118,255,255)},	// modra
		{cv::Scalar(91,110,68),cv::Scalar(104,255,255)},	// zelena
		{cv::Scalar(23,159,114),cv::Scalar(43,234,233)},	// rumena
		{cv::Scalar(11,138,160),cv::Scalar(17,255,255)},	// oranzna
		{cv::Scalar(0,138,40),cv::Scalar(10,255,255)},		// rdeca
		{cv::Scalar(7,50,15),cv::Scalar(24,175,255)},		// rjava
		////{cv::Scalar(0,0,0),cv::Scalar(120,153,100)},		   // crna
		////{cv::Scalar(72,173,14),cv::Scalar(180,255,40)},	   // bela
		////{cv::Scalar(90,68,36),cv::Scalar(139,255,106)},	   // riva
		{cv::Scalar(0,32,32),cv::Scalar(8,200,232)},
		{cv::Scalar(10,89,142),cv::Scalar(18,205,241)},
		{cv::Scalar(20,72,151),cv::Scalar(29,205,255)},
		{cv::Scalar(81,30,56),cv::Scalar(91,150,204)},
		{cv::Scalar(110,60,20),cv::Scalar(124,255,80)},
		{cv::Scalar(134,16,51),cv::Scalar(166,66,137)},
	};

	cv::Mat slikaObdelana, maska;

	cv::GaussianBlur(slika, slikaObdelana, cv::Size(9, 9), 0);

	cv::cvtColor(slikaObdelana, slikaObdelana, cv::COLOR_BGR2HSV);

	for (const std::pair<cv::Scalar, cv::Scalar>& meja : barvnaBaza) {

		const cv::Scalar& spodnja = meja.first;
		const cv::Scalar& zgornja = meja.second;

		cv::inRange(slikaObdelana, spodnja, zgornja, maska);

		if (cv::countNonZero(maska) > 100) {
			//std::cout << cv::countNonZero(maska) << '|' << (dimenzijaSlike / 600) << '\n';
			
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
			
			//std::cout << kontura.size() << '\n';
			if (kontura.size() < 24)
				continue;
			
			cv::Mat konturnaMaska = cv::Mat::zeros(maska.size(), CV_8UC1);
			cv::drawContours(konturnaMaska, std::vector<std::vector<cv::Point>>{kontura}, -1, cv::Scalar(255), -1);

			cv::Mat delnaMaska;
			maska.copyTo(delnaMaska, konturnaMaska);

			std::vector<cv::Point> seznamTock = prepoznavaVsehTock(delnaMaska);

			std::vector<cv::Point> robneTocke = narisiPovezave_V3(delnaMaska, seznamTock);

			seznamKoncev.push_back(robneTocke.front());
			seznamKoncev.push_back(robneTocke.back());
		}

		std::vector<cv::Point> prostaKonca;

		if (seznamKoncev.size() > 2) {
			prostaKonca = narisiProstaKonca0_V0(narisiPovezaveKoncev_V0(seznamKoncev));
		}
		else {
			prostaKonca = seznamKoncev;
		}

		cv::circle(slika, prostaKonca.front(), 5, cv::Scalar(0, 0, 0), -1);
		cv::circle(slika, prostaKonca.back(), 5, cv::Scalar(0, 0, 0), -1);
		cv::line(slika, prostaKonca.front(), prostaKonca.back(), cv::Scalar(255, 0, 255), 2);

		resitev.push_back(prostaKonca.front());
		resitev.push_back(prostaKonca.back());
	}

	cv::imshow("Slika", slika);

	return resitev;
}

void izdelavaMaske_V2(cv::Mat slika, cv::Mat& maska, const std::pair<cv::Scalar, cv::Scalar>& barva) {

	const cv::Scalar& spodnjaMeja = barva.first;
	const cv::Scalar& zgornjaMeja = barva.second;

	cv::Mat slikaObdelana;

	cv::GaussianBlur(slika, slikaObdelana, cv::Size(5, 5), 0);
	cv::cvtColor(slikaObdelana, slikaObdelana, cv::COLOR_BGR2HSV);
	cv::inRange(slikaObdelana, spodnjaMeja, zgornjaMeja, maska);

	cv::dilate(maska, maska, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
	cv::erode(maska, maska, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
	//cv::erode(maska, maska, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
	//cv::dilate(maska, maska, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));

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
		if (par.second == 1)
			resitev.push_back(cv::Point(par.first.x, par.first.y));
	}

	if (resitev.empty())
		resitev = narisiProstaKonca1_V0(seznamTock);

	return resitev;
}
std::vector<cv::Point> narisiProstaKonca1_V0(const std::vector<cv::Point>& seznamTock) {

	std::vector<cv::Point> resitev;

	int maxRazdalja = 0;
	int maxIndex = -1;

	for (int i = 1; i < seznamTock.size(); i += 2) {

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

			ocr.SetRectangle(novX, novY, novW, novH);

			std::string outText = std::string(ocr.GetUTF8Text());
			seznamStringTexta.push_back(outText);

			if (!outText.empty())
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
