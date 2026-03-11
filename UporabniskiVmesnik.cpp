#include "UporabniskiVmesnik.h"


UporabniskiVmesnik::UporabniskiVmesnik(const std::string& vhod, const std::string& izhod) : 
	kontrolnaPlosca(80, 80 * stGumbov, CV_8UC3, cv::Scalar(255, 255, 255)), vhod(vhod), izhod(izhod), seznamPoti(preberiMapo(vhod)) {

	cv::namedWindow("Kontrolna plosca");
	cv::setMouseCallback("Kontrolna plosca", onMouse, this);

	urediKontrolo(cv::Scalar(255, 102, 102));
	naloziSliko();

	std::cout << "dela\n";

	while (int k = cv::waitKey(0) != 113) { // q
		
		//std::cout << "znak: " << k << '\n';
		
		//if (k == 99) { // c
		//}
	}

	cv::destroyAllWindows();
}


void UporabniskiVmesnik::urediKontrolo(const cv::Scalar& barva, const GUMB& gumb) {

	static std::array<cv::Scalar, stGumbov> spominBarv;
	
	if (gumb == VSI) 
		for (cv::Scalar& s : spominBarv)
			s = barva;
	else
		spominBarv[gumb] = barva;

	const int& width = kontrolnaPlosca.cols;
	const int& height = kontrolnaPlosca.rows;

	if (gumb == VSI)
		for (int i = 0; i < stGumbov; i++)
			cv::rectangle(kontrolnaPlosca, cv::Point(i * width / stGumbov + 5, 5), cv::Point((i + 1) * width / stGumbov - 5, height - 5), spominBarv[i], -1);
	else
		cv::rectangle(kontrolnaPlosca, cv::Point(gumb * width / stGumbov + 5, 5), cv::Point((gumb + 1) * width / stGumbov - 5, height - 5), barva, -1);

	imshow("Kontrolna plosca", kontrolnaPlosca);
}

void UporabniskiVmesnik::naloziSliko(const GUMB& gumb) {

	static char index = 0;

	if (gumb == LEVI) index--;
	else if (gumb == DESNI) index++;

	if (index == -1) index = seznamPoti.size() - 1;
	else if (index == seznamPoti.size()) index = 0;

	slika = cv::imread(seznamPoti[index]);
	cv::imshow("Liki", slika);
}


void UporabniskiVmesnik::onMouse(int event, int x, int y, int flags, void* userdata) {

	UporabniskiVmesnik* instance = static_cast<UporabniskiVmesnik*>(userdata);
	instance->nadzorKontrol(event, x, y, flags);
}

void UporabniskiVmesnik::nadzorKontrol(int action, int x, int y, int flags) {

	if (action == cv::EVENT_LBUTTONDOWN || action == cv::EVENT_LBUTTONDBLCLK) {

		for (int i = 0; i < stGumbov; i++) {
			if (x > i * kontrolnaPlosca.cols / stGumbov && x < (i + 1) * kontrolnaPlosca.cols / stGumbov) {
				pritisnjenGumb(cv::Scalar(255, 204, 204), (GUMB)i);
			}
		}
	}
	else if (action == cv::EVENT_LBUTTONUP) {

		pritisnjenGumb(cv::Scalar(255, 102, 102));
	}
}

void UporabniskiVmesnik::pritisnjenGumb(const cv::Scalar& barva, const GUMB& gumb) {

	switch (gumb) {

	case VSI:
		urediKontrolo(barva);
		break;
	
	case LEVI:
		urediKontrolo(barva, LEVI);
		naloziSliko(LEVI);
		break;
	
	case DESNI:
		urediKontrolo(barva, DESNI);
		naloziSliko(DESNI);
		break;
	
	case BERI:
		urediKontrolo(barva, BERI);
		///////////////////////////////
		break;
	
	case BRISI:
		urediKontrolo(barva, BRISI);
		///////////////////////////////
		break;
	
	default:
		break;
	}
}



std::vector<std::string> preberiMapo(const std::string& pot) {

	std::vector<std::string> resitev;

	if (!std::filesystem::exists(pot)) {
		std::cout << "Mape ni bilo mogoce najti.\n";
		return resitev;
	}

	for (const auto& entry : std::filesystem::directory_iterator(pot)) {
		if (entry.is_regular_file()) {
			resitev.push_back(pot + "/" + entry.path().filename().string());
		}
		else {
			std::cout << "Neprepoznana datoteka: " << entry.path().filename().string() << '\n';
		}
	}

	return resitev;
}
