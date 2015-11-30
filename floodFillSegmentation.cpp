#include <stdio.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#define SEGMENTS_HEIGHT 20
#define SEGMENTS_WIDTH 20
#define DIFFERENCE 500000

using namespace cv;

int main(int argc, char **argv) {

	Mat image = imread("lena.jpg", CV_LOAD_IMAGE_COLOR);
	int imheight = image.rows;
	int imwidth = image.cols;

	Point startPoint;
	startPoint.x = image.cols / 2;
	startPoint.y = image.rows / 2;
	Scalar loDiff(5, 5, 255);
	Scalar upDiff(200, 200, 255);
	Scalar fillColor(0, 255, 0);
	int neighbors = 8;
	Rect domain;
	int area = floodFill(image, startPoint, fillColor, &domain, loDiff, upDiff, neighbors);
	rectangle(image, domain, Scalar(255, 0, 0));

	// разбиение сегментов на прямоугольники
	int x, y, w, h;
	int t = 0, tmp, l;

	for (int i = 0; i < image.rows; i += SEGMENTS_HEIGHT){
		if (i >= image.rows) break;
		tmp = image.at<unsigned char>(i, 0) + image.at<unsigned char>(i, 1) * 256 + image.at<unsigned char>(i, 2) * 256 * 256;
		l = 0;
		for (int j = 0; j < image.cols * 3; j += 3) {
			if (abs(image.at<unsigned char>(i, j) + image.at<unsigned char>(i, j + 1) * 256 + image.at<unsigned char>(i, j + 2) * 256 * 256 - tmp) > DIFFERENCE || j == image.cols * 3 - 3) {
				x = l;
				y = i;
				h = image.cols - 1 > i + SEGMENTS_HEIGHT ? i + SEGMENTS_HEIGHT : image.cols;
				w = j - 4;
				l = j;
				tmp = image.at<unsigned char>(i, j) + image.at<unsigned char>(i, j + 1) * 256 + image.at<unsigned char>(i, j + 2) * 256 * 256;


				//если часом вышли за границы
				if (h >= imheight)h = imheight - 1;
				if (w / 3 >= imwidth)w = 3 * (imwidth - 1);
				// не анализируем слишком маленькие сегменты
				if ((w - x) / 3<SEGMENTS_WIDTH || h - y<SEGMENTS_HEIGHT)
					continue;
				t++;
				rectangle(image, Rect(x / 3, y, (w - x) / 3, h - y), Scalar(255, 0, 0));
			}
		}
	}

	printf("Number of segment %d",t);
	imshow("result", image);
	waitKey(0);
}
