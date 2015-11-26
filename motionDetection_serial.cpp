#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;

//параметры
//сегментация
#define SPATIALRADIUS 35
#define COLORRADIUS 100
#define PYRAMIDLEVELS 2
//разбиение на прямоугольники
#define SEGMENTS_MAX 20000
#define SEGMENTS_HEIGHT 20
#define DIFFERENCE 500000
//обработка прямоугольников
#define KERNEL_SIZE 3
#define SCALE 1
#define DELTA 0
//анализ
#define LAPLACE_CRITICAL_VALUE 15

int main(int argc, char **argv) {
	// ввод изображения
	Mat image = imread("lena.jpg", CV_LOAD_IMAGE_COLOR);

	// сегментация алгоритмом MeanShift
	Mat wshed;
	int spatialRadius = SPATIALRADIUS;
	int colorRadius = COLORRADIUS;
	int pyramidLevels = PYRAMIDLEVELS;
	pyrMeanShiftFiltering(image, wshed, spatialRadius, colorRadius, pyramidLevels);
	imshow("MeanShiftSegmantation", wshed);

	// разбиение сегментов на прямоугольники
	int x[SEGMENTS_MAX], y[SEGMENTS_MAX], w[SEGMENTS_MAX], h[SEGMENTS_MAX];
	int t = 0, tmp, l;

	for (int i = 0; i < wshed.rows; i += SEGMENTS_HEIGHT){
		if (i >= wshed.rows) break;
		tmp = wshed.at<unsigned char>(i, 0) + wshed.at<unsigned char>(i, 1) * 256 + wshed.at<unsigned char>(i, 2) * 256 * 256;
		l = 0;
		for (int j = 0; j < wshed.cols*3; j+=3) {
			if (abs(wshed.at<unsigned char>(i, j) + wshed.at<unsigned char>(i, j + 1) * 256 + wshed.at<unsigned char>(i, j + 2) * 256 * 256 - tmp) > DIFFERENCE || j == wshed.cols * 3 - 3) {
				x[t] = l;
				y[t] = i;
				h[t] = wshed.cols - 1 > i + SEGMENTS_HEIGHT ? i + SEGMENTS_HEIGHT : wshed.cols;
				w[t] = j - 4;
				t++;
				l = j;
				tmp = wshed.at<unsigned char>(i, j) + wshed.at<unsigned char>(i, j+1) * 256 + wshed.at<unsigned char>(i, j+2) * 256 * 256;
			}
		}
	}

	// обработка прямоугольных областей
	Mat src, src_gray, dst;
	int kernel_size = KERNEL_SIZE;
	int scale = SCALE;
	int delta = DELTA;
	int ddepth = CV_16S;
	int m;

	// каждый прямоугольник
	for (int i = 0; i < t; i++){
		// если высота прямоугольника меньше заданной (характерно для граничных областей)
		// или он вырождается в прямую, то его обрабатывать не нужно
		if (w[i] - x[i] <= SEGMENTS_HEIGHT || h[i] - y[i] == 0)
			continue;
		
		// выделяем данный прямоугольник из исходного изображения
		src = image(Rect(x[i]/3, y[i], (w[i] - x[i])/3, h[i] - y[i]));

		// удаляем шумы с помощью фильтра Гаусса
		GaussianBlur(src, src, Size(3, 3), 0, 0, BORDER_DEFAULT);

		// переводим изображение в серые тона
		cvtColor(src, src_gray, CV_RGB2GRAY);

		Mat abs_dst;
		Laplacian(src_gray, dst, ddepth, kernel_size, scale, delta, BORDER_DEFAULT);
		convertScaleAbs(dst, abs_dst);

		//выводим информацию о прямоугольнике
		printf("Area #%d %d %d %d %d\n", i, x[i], y[i], w[i], h[i]);

		//просчитываем среднее значение яркости в данном прямоугольнике
		m = 0;
		for (int i = 0; i < abs_dst.rows; i++)
			for (int j = 0; j < abs_dst.cols; j++)
				m += abs_dst.at<unsigned char>(i, j);

		double laplace_num = m*1.0 / (abs_dst.rows*abs_dst.cols);
		printf("Laplace number is %f\n\n", laplace_num);

		// если среднее значение яркости меньше критического значения фильтра,
		// то область считаем размытой
		if (laplace_num < LAPLACE_CRITICAL_VALUE){
			rectangle(image, Rect(x[i] / 3, y[i], (w[i] - x[i]) / 3, h[i] - y[i]), Scalar(255, 0, 0));
			rectangle(wshed, Rect(x[i] / 3, y[i], (w[i] - x[i]) / 3, h[i] - y[i]), Scalar(255, 0, 0));
		}
	}

	// выводим исходное изображение с указанием областей, где предполагается движение
	imshow("Result", image);
	// сегментированое изображение с выделением областей, где предполагается движение
	imshow("Result_seg", wshed);

	waitKey(0);
	return 0;
}

