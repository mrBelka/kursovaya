#include <stdio.h> 
#include <opencv2/opencv.hpp> 

using namespace cv;

int main(int argc, char* argv[])
{
	const char *cannyWinName = "Canny";
	Mat img, grayImg, edgesImg;
	double lowThreshold = 70, uppThreshold = 260;

	// загрузка изображения 
	img = imread("lena2.jpg", 1);
	// удаление шумов 
	blur(img, img, Size(3, 3));
	// преобразование в оттенки серого 
	cvtColor(img, grayImg, CV_RGB2GRAY);
	// применение детектора Канни 
	Canny(grayImg, edgesImg, lowThreshold, uppThreshold);

	// отображение результата 
	namedWindow(cannyWinName, CV_WINDOW_AUTOSIZE);
	imshow(cannyWinName, edgesImg);
	waitKey();

	// закрытие окон 
	destroyAllWindows();

	// осовобождение памяти 
	img.release();
	grayImg.release();
	edgesImg.release();
	return 0;
}
