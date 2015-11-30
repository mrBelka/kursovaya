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

	Mat imageGray, imageBin;
	cvtColor(image, imageGray, CV_BGR2GRAY);
	threshold(imageGray, imageBin, 200, 255, THRESH_BINARY);
	std::vector<std::vector<Point> > contours;
	std::vector<Vec4i> hierarchy;
	findContours(imageBin, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
	Mat markers(image.size(), CV_32SC1);
	markers = Scalar::all(0);
	int compCount = 0;
	for (int idx = 0; idx >= 0; idx = hierarchy[idx][0], compCount++)
	{
		drawContours(markers, contours, idx, Scalar::all(compCount + 1), -1, 8, hierarchy, INT_MAX);
	}
	std::vector<Vec3b> colorTab(compCount);
	for (int i = 0; i < compCount; i++)
	{
		colorTab[i] = Vec3b(rand() & 255, rand() & 255, rand() & 255);
	}

	watershed(image, markers);

	Mat wshed(markers.size(), CV_8UC3);
	for (int i = 0; i < markers.rows; i++)
	{
		for (int j = 0; j < markers.cols; j++)
		{
			int index = markers.at<int>(i, j);
			if (index == -1)  wshed.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
			else if (index == 0) wshed.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
			else  wshed.at<Vec3b>(i, j) = colorTab[index - 1];
		}
	}

	// разбиение сегментов на прямоугольники
	int x, y, w, h;
	int t = 0, tmp, l;

	for (int i = 0; i < wshed.rows; i += SEGMENTS_HEIGHT){
		if (i >= wshed.rows) break;
		tmp = wshed.at<unsigned char>(i, 0) + wshed.at<unsigned char>(i, 1) * 256 + wshed.at<unsigned char>(i, 2) * 256 * 256;
		l = 0;
		for (int j = 0; j < wshed.cols * 3; j += 3) {
			if (abs(wshed.at<unsigned char>(i, j) + wshed.at<unsigned char>(i, j + 1) * 256 + wshed.at<unsigned char>(i, j + 2) * 256 * 256 - tmp) > DIFFERENCE || j == wshed.cols * 3 - 3) {
				x = l;
				y = i;
				h = wshed.cols - 1 > i + SEGMENTS_HEIGHT ? i + SEGMENTS_HEIGHT : wshed.cols;
				w = j - 4;
				t++;
				l = j;
				tmp = wshed.at<unsigned char>(i, j) + wshed.at<unsigned char>(i, j + 1) * 256 + wshed.at<unsigned char>(i, j + 2) * 256 * 256;


				//если часом вышли за границы
				if (h >= imheight)h = imheight - 1;
				if (w / 3 >= imwidth)w = 3 * (imwidth - 1);
				// не анализируем слишком маленькие сегменты
				if ((w - x) / 3<SEGMENTS_WIDTH || h - y<SEGMENTS_HEIGHT)
					continue;

				rectangle(image, Rect(x / 3, y, (w - x) / 3, h - y), Scalar(255, 0, 0));
			}
		}
	}

	printf("Number of segment %d",t);
	imshow("watershed transform", wshed);
	imshow("result", image);
	waitKey(0);
}
