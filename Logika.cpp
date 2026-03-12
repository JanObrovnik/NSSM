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

			cv::rectangle(obdelanaSlika, cv::Rect(novX, novY, novW, novH), cv::Scalar(0, 255, 0));
		}
		boxaDestroy(&boxes);
	}

	cv::imshow("Slika", obdelanaSlika);

	ocr.End();
}
