#include "UporabniskiVmesnik.h"


UporabniskiVmesnik::UporabniskiVmesnik(std::string& vhod, std::string& izhod) : 
	kontrolnaPlosca(80, 80 * GUMB::kolicina, CV_8UC3, cv::Scalar(255, 255, 255)), vhod(vhod), izhod(izhod), seznamPoti(preberiMapo(vhod)) {

	seznamKontrolnihIkon = (preberiMapo("C:/Users/janob/Desktop/Faks/NapredniSenzorskiSistemi/CPP/ProjektNSSM/ProjektNSSM/Seminar/Resources"));
	std::sort(seznamKontrolnihIkon.begin(), seznamKontrolnihIkon.end());

	cv::namedWindow("Kontrolna plosca");
	cv::setMouseCallback("Kontrolna plosca", onMouse, this);

	urediKontrolo(cv::Scalar(255, 102, 102));
	naloziSliko();

	while (int k = cv::waitKey(0) != 113) { // q
		
		//std::cout << "znak: " << k << '\n';
		
		//if (k == 99) { // c
		//}
	}

	cv::destroyAllWindows();
}


void UporabniskiVmesnik::urediKontrolo(const cv::Scalar& barva, const GUMB& gumb) {

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


void UporabniskiVmesnik::onMouse(int event, int x, int y, int flags, void* userdata) {

	UporabniskiVmesnik* instance = static_cast<UporabniskiVmesnik*>(userdata);
	instance->nadzorKontrol(event, x, y, flags);
}

void UporabniskiVmesnik::nadzorKontrol(int action, int x, int y, int flags) {

	if (action == cv::EVENT_LBUTTONDOWN || action == cv::EVENT_LBUTTONDBLCLK) {

		for (int i = 0; i < GUMB::kolicina; i++) {
			if (x > i * kontrolnaPlosca.cols / GUMB::kolicina && x < (i + 1) * kontrolnaPlosca.cols / GUMB::kolicina) {
				pritisnjenGumb(cv::Scalar(255, 204, 204), (GUMB)i);
			}
		}
	}
	else if (action == cv::EVENT_LBUTTONUP) {

		pritisnjenGumb(cv::Scalar(255, 102, 102));
	}
}

void UporabniskiVmesnik::pritisnjenGumb(const cv::Scalar& barva, const GUMB& gumb) {

	urediKontrolo(barva, gumb);

	switch (gumb) {

	case VSI:
		break;
	
	case LEVI:
		naloziSliko(LEVI);
		break;
	
	case DESNI:
		naloziSliko(DESNI);
		break;
	
	case BARVA:
		prepoznavaBarv_V0(slika);
		break;

	case ZICA:
		prepoznavaZic_V0(slika);
		break;

	case BERI:
		cv::waitKey(1);
		prepoznajTekst_V0(slika);
		break;

	case BRISI:
		naloziSliko(VSI);
		break;
	
	default:
		break;
	}
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
