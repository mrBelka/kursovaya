#include <stdio.h> 
#include <opencv2/opencv.hpp> 

using namespace cv;

int main(int argc, char* argv[])
{
	const char *laplacianWinName = "Laplace";
	Mat img, grayImg, laplacianImg, laplacianImgAbs;
	int ddepth = CV_16S;

	// загрузка изображения 
	img = imread("lena.jpg", 1);
	// сглаживание с помощью фильтра Гаусса 
	GaussianBlur(img, img, Size(3, 3),
		0, 0, BORDER_DEFAULT);
	// преобразование в оттенки серого 
	cvtColor(img, grayImg, CV_RGB2GRAY);
	// применение оператора Лапласа 
	Laplacian(grayImg, laplacianImg, ddepth);
	convertScaleAbs(laplacianImg, laplacianImgAbs);

	// отображение результата 
	namedWindow(laplacianWinName, CV_WINDOW_AUTOSIZE);
	imshow(laplacianWinName, laplacianImgAbs);

	double sum = 0;
	for (int i = 0; i < laplacianImgAbs.rows; i++)
		for (int j = 0; j < laplacianImgAbs.cols; j++)
			sum += laplacianImgAbs.at<unsigned char>(i, j);
	sum /= (laplacianImgAbs.rows*laplacianImgAbs.cols);

	printf("%f", sum);

	waitKey();

	// закрытие окон 
	destroyAllWindows();
	// осовобождение памяти 
	img.release();
	grayImg.release();
	laplacianImg.release();
	laplacianImgAbs.release();
	return 0;
}
