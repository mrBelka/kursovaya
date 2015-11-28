#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <iostream>
#include <conio.h>
#include <stdio.h>
#include <cmath>
#include <windows.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;

//параметры
//сегментация
#define SPATIALRADIUS 35
#define COLORRADIUS 100
#define PYRAMIDLEVELS 2
//сонаправленные линии
#define CONTOUR_LENGHT_CRITICAL_VALUE 100
#define CONTOUR_KOEF 2
//разбиение на прямоугольники
#define SEGMENTS_MAX 20000
#define SEGMENTS_HEIGHT 10
#define SEGMENTS_WIDTH 10
#define DIFFERENCE 500000
//обработка прямоугольников
#define KERNEL_SIZE 3
#define SCALE 1
#define DELTA 0
//анализ
#define LAPLACE_CRITICAL_VALUE 15


int main(int argc, char **argv) {
	// ввод изображения
	Mat image = imread("lena5.jpg", CV_LOAD_IMAGE_COLOR);
	int imheight = image.rows;
	int imwidth = image.cols;

	//поиск границ на изображение детектором Кенни
	Mat gray;
	cvtColor(image, gray, CV_BGR2GRAY);
	Canny(gray, gray, 100, 200, 3);
	//выделение границ  
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(gray, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	//отрисовка контуров
	Mat drawing = Mat::zeros(gray.size(), CV_8UC3);
	Mat image_cont = image.clone();
	vector<vector<Point> > cont(contours.size(), vector<Point>());
	Scalar color;

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	int cont_size = 0;
	int cnt[6];
	double d;
	for (int i = 0; i < 6; i++)cnt[i] = 0;

	//обработка каждого контура
	for (int i = 0; i < contours.size(); i++)
	{
		//из массива точек контура выбираем первую и среднюю (из-за устройства массива)
		if (contours[i].size() < 2)continue;
		if (contours[i].size() == 2){
			cont[i].push_back(contours[i][0]);
			cont[i].push_back(contours[i][1]);
		}
		else{
			for (int j = 0; j < contours[i].size() / 2; j++) {
				if (j == 0)cont[i].push_back(contours[i][j]);
				if (j == contours[i].size() / 2 - 1)cont[i].push_back(contours[i][j]);
			}
		}

		//рассчитываем направление каждой полученной прямой
		int x0 = cont[i][0].x;
		int x1 = cont[i][1].x;
		int y0 = cont[i][0].y;
		int y1 = cont[i][1].y;
			//printf("%d %d %d\n", i, cont[i][0].x, cont[i][0].y);
			//printf("%d %d %d\n", i, cont[i][1].x, cont[i][1].y);
		if (x0 > x1){
			d = (y0 - y1)*1.0 / (x0 - x1);
		}
		else if(x1 > x0){
			d = (y1 - y0)*1.0 / (x1 - x0);
		}

		//увеличиваем соответсвующий счетчик
		if ((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0)>CONTOUR_LENGHT_CRITICAL_VALUE){
			if (d >= sqrt(3.0)) {
				cnt[0]++;
				color = Scalar(255,0,0);
			}
			else if (d < sqrt(3.0) && d>=sqrt(3.0) / 3) {
				cnt[1]++;
				color = Scalar(0, 255, 0);
			}
			else if (d < sqrt(3.0) / 3 && d >= 0) {
				cnt[2]++;
				color = Scalar(0, 0, 255);
			}
			else if (d<0 && d>=-sqrt(3.0) / 3){
				cnt[3]++;
				color = Scalar(255, 255, 0);
			}
			else if (d<-sqrt(3.0) / 3 && d>=-sqrt(3.0)){
				cnt[4]++;
				color = Scalar(255, 0, 255);
			}
			else{
				cnt[5]++;
				color = Scalar(0, 255, 255);
			}

			//колчество контуров
			cont_size++;

			//отрисовка контура
			drawContours(drawing, cont, i, color, 2, 8, hierarchy, 0, Point());
			drawContours(image_cont, cont, i, color, 2, 8, hierarchy, 0, Point());
		}
	}

	//выведем данные о вердикте программы после детектора Кенни
	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 15));
	printf("RESULTS\n");

	// подсчет результатов
	int max = cnt[0];
	int pos = 0;
	for (int i = 1; i < 6; i++)
		if (max < cnt[i]){
			max = cnt[i];
			pos = i;
		}
	double div = 0;
	for (int i = 0; i < 6; i++){
		if (pos == i)continue;
		if(max*1.0/cnt[i]>div)
			div = max*1.0 / cnt[i];
	}
	//если количество линии одного направление хотя бы в CANNY_KOEF раз превосходит
	//кол-во линий ближайшего по численности направления, то движение в первом направлении есть
	if (div >= CONTOUR_KOEF){
		printf("1st FILTER: Codirectional line, Verdict: ");
		SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 2));
		printf("YES");
		SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 15));
		printf(", Probability: %3.1f\n", max*1.0 / cont_size);
		//знак минус перед углам из-за того, что картинка отражена по оси Ox
		printf("    Additional: Direction of moving is between %d and %d degrees\n", -(90 - pos * 30), -(90 - (pos + 1) * 30));
	}
	else {
		printf("FILTER: Codirectional line, Verdict: ");
		SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 4));
		printf("NO");
		SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 15));
		printf(", Probability: %3.1f\n", 1 - max*1.0 / cont_size);
	}

	// сегментация алгоритмом MeanShift
	Mat wshed;
	int spatialRadius = SPATIALRADIUS;
	int colorRadius = COLORRADIUS;
	int pyramidLevels = PYRAMIDLEVELS;
	pyrMeanShiftFiltering(image, wshed, spatialRadius, colorRadius, pyramidLevels);
	//imshow("MeanShiftSegmantation", wshed);

	// разбиение сегментов на прямоугольники
	int x[SEGMENTS_MAX], y[SEGMENTS_MAX], w[SEGMENTS_MAX], h[SEGMENTS_MAX];
	int t = 0, tmp, l;

	for (int i = 0; i < wshed.rows; i += SEGMENTS_HEIGHT){
		if (i >= wshed.rows) break;
		tmp = wshed.at<unsigned char>(i, 0) + wshed.at<unsigned char>(i, 1) * 256 + wshed.at<unsigned char>(i, 2) * 256 * 256;
		l = 0;
		for (int j = 0; j < wshed.cols * 3; j += 3) {
			if (abs(wshed.at<unsigned char>(i, j) + wshed.at<unsigned char>(i, j + 1) * 256 + wshed.at<unsigned char>(i, j + 2) * 256 * 256 - tmp) > DIFFERENCE || j == wshed.cols * 3 - 3) {
				x[t] = l;
				y[t] = i;
				h[t] = wshed.cols - 1 > i + SEGMENTS_HEIGHT ? i + SEGMENTS_HEIGHT : wshed.cols;
				w[t] = j - 4;
				t++;
				l = j;
				tmp = wshed.at<unsigned char>(i, j) + wshed.at<unsigned char>(i, j + 1) * 256 + wshed.at<unsigned char>(i, j + 2) * 256 * 256;
			}
		}
	}

	imshow("Source", image);

	// обработка прямоугольных областей
	Mat src, src_gray, dst;
	int kernel_size = KERNEL_SIZE;
	int scale = SCALE;
	int delta = DELTA;
	int ddepth = CV_16S;
	int m;

	int segms_num = 0;
	int blur_size = 0;
	double sum = 0;

	// каждый прямоугольник
	for (int i = 0; i < t; i++){

		//если часом вышли за границы
		if (h[i] >= imheight)h[i] = imheight - 1;
		if (w[i] / 3 >= imwidth)w[i] = 3 * (imwidth - 1);
		// не анализируем слишком маленькие сегменты
		if ((w[i] - x[i])/3<SEGMENTS_WIDTH || h[i] - y[i]<SEGMENTS_HEIGHT)
			continue;

		segms_num++;

		// выделяем данный прямоугольник из исходного изображения
		src = image(Rect(x[i] / 3, y[i], (w[i] - x[i]) / 3, h[i] - y[i]));

		// удаляем шумы с помощью фильтра Гаусса
		GaussianBlur(src, src, Size(3, 3), 0, 0, BORDER_DEFAULT);

		// переводим изображение в серые тона
		cvtColor(src, src_gray, CV_RGB2GRAY);

		Mat abs_dst;
		Laplacian(src_gray, dst, ddepth, kernel_size, scale, delta, BORDER_DEFAULT);
		convertScaleAbs(dst, abs_dst);

		//выводим информацию о прямоугольнике
		//printf("Area #%d %d %d %d %d\n", i, x[i], y[i], w[i], h[i]);

		//просчитываем среднее значение яркости в данном прямоугольнике
		m = 0;
		for (int i = 0; i < abs_dst.rows; i++)
			for (int j = 0; j < abs_dst.cols; j++)
				m += abs_dst.at<unsigned char>(i, j);

		double laplace_num = m*1.0 / (abs_dst.rows*abs_dst.cols);
		//printf("Laplace number is %f\n\n", laplace_num);

		// если среднее значение яркости меньше критического значения фильтра,
		// то область считаем размытой
		if (laplace_num < LAPLACE_CRITICAL_VALUE){
			blur_size++;
			//потому что важнее обратное значение
			sum += (1 - laplace_num / LAPLACE_CRITICAL_VALUE);
			rectangle(image, Rect(x[i] / 3, y[i], (w[i] - x[i]) / 3, h[i] - y[i]), Scalar(255, 0, 0));
			rectangle(wshed, Rect(x[i] / 3, y[i], (w[i] - x[i]) / 3, h[i] - y[i]), Scalar(255, 0, 0));
		}
	}

	printf("2st FILTER: Blur detection\n");
	printf("    Verdict: ");
	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 2));
	printf("YES");
	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 15));
	printf(" for %d segments, Average probability: %3.1f\n", blur_size, sum / blur_size);
	printf("    Verdict: ");
	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 4));
	printf("NO");
	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 15));
	printf(" for %d segments\n", segms_num - blur_size);
	// выводим изображение с контурами	
	//imshow("Result window", drawing);
	imshow("Result1", image_cont);
	// выводим исходное изображение с указанием областей, где предполагается движение
	imshow("Result2", image);
	// сегментированое изображение с выделением областей, где предполагается движение
	//imshow("Result_seg", wshed);

	waitKey(0);
	return 0;
}

