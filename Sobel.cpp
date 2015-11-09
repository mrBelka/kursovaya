#include <stdio.h> 
#include <opencv2/opencv.hpp> 
using namespace cv;

int main(int argc, char* argv[])
{
	const char *initialWinName = "Initial Image",
		*gradWinName = "Gradient";
	int ddepth = CV_16S;
	double alpha = 0.5, beta = 0.5;
	Mat img, grayImg, xGrad, yGrad,
		xGradAbs, yGradAbs, grad;

	// загрузка изображения 
	img = imread("lena1.jpg", 1);

	// сглаживание помощью фильтра Гаусса 
	GaussianBlur(img, img, Size(3, 3),0, 0, BORDER_DEFAULT);

	// преобразование в оттенки серого 
	cvtColor(img, grayImg, CV_RGB2GRAY);

	// вычисление производных по двум направлениям 
	Sobel(grayImg, xGrad, ddepth, 1, 0); // по Ox 
	Sobel(grayImg, yGrad, ddepth, 0, 1); // по Oy 

	// преобразование градиентов в 8-битные 
	convertScaleAbs(xGrad, xGradAbs);
	convertScaleAbs(yGrad, yGradAbs);

	// поэлементное вычисление взвешенной 
	// суммы двух массивов 
	addWeighted(xGradAbs, alpha, yGradAbs, beta, 0, grad);

	// отображение результата ;
	namedWindow(gradWinName, CV_WINDOW_AUTOSIZE);
	imshow(gradWinName, grad);

	double sum = 0;
	for (int i = 0; i < grad.rows; i++)
		for (int j = 0; j < grad.cols; j++)
			sum += grad.at<unsigned char>(i, j);
	sum /= (grad.rows*grad.cols);

	printf("%f",sum);

	waitKey();

	// закрытие окон 
	destroyAllWindows();

	// осовобождение памяти 
	img.release();
	grayImg.release();
	xGrad.release();
	yGrad.release();
	xGradAbs.release();
	yGradAbs.release();

	return 0;
}
