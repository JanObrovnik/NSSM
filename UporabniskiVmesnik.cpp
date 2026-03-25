#include "UporabniskiVmesnik.h"


UporabniskiVmesnik::UporabniskiVmesnik(std::string& vhod, std::string& izhod) : 
	kontrolnaPlosca(80, 80 * GUMB::kolicina, CV_8UC3, cv::Scalar(255, 255, 255)), vhod(vhod), izhod(izhod), seznamPoti(preberiMapo(vhod)) {

	seznamKontrolnihIkon = (preberiMapo("C:/Users/janob/Desktop/Faks/NapredniSenzorskiSistemi/CPP/ProjektNSSM/ProjektNSSM/Seminar/Resources"));
	std::sort(seznamKontrolnihIkon.begin(), seznamKontrolnihIkon.end());

	indexTab = -1;

	cv::namedWindow("Kontrolna plosca");
	cv::setMouseCallback("Kontrolna plosca", onMouse, this);

	urediKontrolo();
	naloziSliko();

	while (pritisnjenaTipka(cv::waitKey(0)));

	cv::destroyAllWindows();
}


void UporabniskiVmesnik::urediKontrolo(const GUMB& gumb) {

	const int& width = kontrolnaPlosca.cols;
	const int& height = kontrolnaPlosca.rows;

	if (gumb == VSI)
		for (int i = 0; i < GUMB::kolicina; i++) {
			cv::Mat ikona = cv::imread(seznamKontrolnihIkon[i], cv::IMREAD_UNCHANGED);
			cv::Rect roi(i * width / GUMB::kolicina, 0, ikona.cols, ikona.rows);
			ikona.copyTo(kontrolnaPlosca(roi));
		}
	else {
		cv::Mat roi = kontrolnaPlosca(cv::Rect(gumb * width / GUMB::kolicina, 0, 80, 80));
		cv::Mat color(roi.size(), CV_8UC3, cv::Scalar(255, 255, 255));
		double alpha = 0.3;
		cv::addWeighted(color, alpha, roi, 1.0 - alpha, 0.0, roi);
	}
	
	if (indexTab > VSI && indexTab < GUMB::kolicina) {
		cv::rectangle(kontrolnaPlosca, cv::Point(indexTab * width / GUMB::kolicina + 5, 5), cv::Point((indexTab + 1) * width / GUMB::kolicina - 5, height - 5), cv::Scalar(0, 0, 255));
	}

	imshow("Kontrolna plosca", kontrolnaPlosca);
}


void UporabniskiVmesnik::naloziSliko(const GUMB& gumb) {

	static char index = 0;

	if (gumb == LEVI) index--;
	else if (gumb == DESNI) index++;

	if (index == -1) index = seznamPoti.size() - 1;
	else if (index == seznamPoti.size()) index = 0;

	slika = cv::imread(seznamPoti[index]);
	cv::imshow("Slika", slika);
}

void UporabniskiVmesnik::shraniSliko() {

	static int index = 0;

	cv::imwrite((izhod + "/" + std::to_string(index++) + ".jpg"), slika);
}


void UporabniskiVmesnik::onMouse(int dogodek, int x, int y, int flags, void* userdata) {

	UporabniskiVmesnik* instance = static_cast<UporabniskiVmesnik*>(userdata);
	instance->nadzorKontrol(dogodek, x, y, flags);
}

void UporabniskiVmesnik::nadzorKontrol(int dogodek, int x, int y, int flags) {

	if (dogodek == cv::EVENT_LBUTTONDOWN || dogodek == cv::EVENT_LBUTTONDBLCLK) {

		pritisnjenGumb(static_cast<GUMB>(x / (kontrolnaPlosca.cols / GUMB::kolicina)));
	}
	else if (dogodek == cv::EVENT_LBUTTONUP) {

		pritisnjenGumb();
	}
}

void UporabniskiVmesnik::pritisnjenGumb(const GUMB& gumb) {

	urediKontrolo(gumb);
	cv::waitKey(1);

	switch (gumb) {

	case VSI:
		break;
	
	case LEVI:
		barve.clear(); ////////////////// mogoc brezveze
		naloziSliko(LEVI);
		break;
	
	case DESNI:
		barve.clear();
		naloziSliko(DESNI);
		break;
	
	case BARVA:
		barve.clear();
		barve = prepoznavaBarv_V2(slika);
		break;

	case ZICA:
		if (barve.empty()) break;
		prepoznavaZic_V3(slika, barve);
		break;

	case BERI:
		prepoznajTekst_V0(slika);
		break;

	case BRISI:
		naloziSliko(VSI);
		break;

	case SHRANI:
		shraniSliko();
		break;
	
	default:
		break;
	}
}


bool UporabniskiVmesnik::pritisnjenaTipka(const int& tipka) {

	//std::cout << "tipka: " << tipka << '\n';

	if (tipka == 113) // q
		return false;

	pritisnjenGumb();

	switch (tipka) {

	case 9: // tab
		indexTab++;
		if (indexTab == GUMB::kolicina) indexTab = -1;
		urediKontrolo();
		break;
	case 13: // enter
		pritisnjenGumb(static_cast<GUMB>(indexTab));
		break;

	case 97: // a
		pritisnjenGumb(LEVI);
		break;
	case 115: // s
		pritisnjenGumb(DESNI);
		break;
	case 100: // d
		pritisnjenGumb(BARVA);
		break;
	case 102: // f
		pritisnjenGumb(ZICA);
		break;
	case 103: // g
		pritisnjenGumb(BERI);
		break;
	case 104: // h
		pritisnjenGumb(BRISI);
		break;
	case 106: // j
		pritisnjenGumb(SHRANI);
		break;

	default:
		break;
	}

	return true;
}



std::vector<std::string> preberiMapo(std::string pot) {

	std::vector<std::string> resitev;

	while (!std::filesystem::exists(pot)) {
		std::cout << "Mape "<< pot <<" ni bilo mogoce najti.\n";
		std::cout << "Podajte novo mapo: ";
		std::cin >> pot;
	}

	for (const auto& entry : std::filesystem::directory_iterator(pot)) {
		if (entry.is_regular_file()) {
			//std::cout << pot + "/" + entry.path().filename().string() << '\n';
			resitev.push_back(pot + "/" + entry.path().filename().string());
		}
		else {
			std::cout << "Neprepoznana datoteka: " << entry.path().filename().string() << '\n';
		}
	}

	return resitev;
}
