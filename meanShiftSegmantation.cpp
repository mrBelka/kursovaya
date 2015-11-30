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

	Mat imageSegment;
	int spatialRadius = 35;
	int colorRadius = 60;
	int pyramidLevels = 3;
	pyrMeanShiftFiltering(image, imageSegment, spatialRadius, colorRadius, pyramidLevels);

	// разбиение сегментов на прямоугольники
	int x, y, w, h;
	int t = 0, tmp, l;

	for (int i = 0; i < imageSegment.rows; i += SEGMENTS_HEIGHT){
		if (i >= imageSegment.rows) break;
		tmp = imageSegment.at<unsigned char>(i, 0) + imageSegment.at<unsigned char>(i, 1) * 256 + imageSegment.at<unsigned char>(i, 2) * 256 * 256;
		l = 0;
		for (int j = 0; j < imageSegment.cols * 3; j += 3) {
			if (abs(imageSegment.at<unsigned char>(i, j) + imageSegment.at<unsigned char>(i, j + 1) * 256 + imageSegment.at<unsigned char>(i, j + 2) * 256 * 256 - tmp) > DIFFERENCE || j == imageSegment.cols * 3 - 3) {
				x = l;
				y = i;
				h = imageSegment.cols - 1 > i + SEGMENTS_HEIGHT ? i + SEGMENTS_HEIGHT : imageSegment.cols;
				w = j - 4;
				t++;
				l = j;
				tmp = imageSegment.at<unsigned char>(i, j) + imageSegment.at<unsigned char>(i, j + 1) * 256 + imageSegment.at<unsigned char>(i, j + 2) * 256 * 256;


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
	imshow("watershed transform", imageSegment);
	imshow("result", image);
	waitKey(0);
}
