#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;

int main(int argc, char **argv) {
	
	// блок ввода изображения
	Mat image = imread("lena.jpg", CV_LOAD_IMAGE_COLOR);

	// алгоритм сегментации изображения по водоразделам (WaterShed)
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
	imshow("watershed transform", wshed);

	// разбиение сегментов на прямоугольники
	int x[10000], y[10000], w[10000], h[10000];
	int t = 0, tmp, l;

	for (int i = 0; i < wshed.rows; i += 10){
		if (i >= wshed.rows) break;
		tmp = wshed.at<unsigned char>(i, 0) + wshed.at<unsigned char>(i, 1) * 256 + wshed.at<unsigned char>(i, 2) * 256 * 256;
		l = 0;
		for (int j = 0; j < wshed.cols*3; j+=3) {
			if (wshed.at<unsigned char>(i, j) + wshed.at<unsigned char>(i, j+1) * 256 + wshed.at<unsigned char>(i, j+2) * 256 *256 != tmp || j == wshed.cols*3 - 3) {
				x[t] = l;
				y[t] = i;
				h[t] = wshed.cols - 1 > i + 10 ? i + 10 : wshed.cols;
				w[t] = j - 4;
				t++;
				l = j;
				tmp = wshed.at<unsigned char>(i, j) + wshed.at<unsigned char>(i, j+1) * 256 + wshed.at<unsigned char>(i, j+2) * 256 * 256;
			}
		}
	}

	/*for (int i = 0; i < t; i++){
		printf("%d %d %d %d %d\n",i,x[i],y[i],w[i],h[i]);
	}*/

	Mat src, src_gray, dst;
	int kernel_size = 3;
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;
	int m;

	for (int i = 0; i < t; i++){
		if (w[i] - x[i] <= 10 || h[i] - y[i] == 0)continue;
		//printf("%d %d %d %d",);
		src = image(Rect(x[i]/3, y[i], (w[i] - x[i])/3, h[i] - y[i]));  //Mat image_roi = image(region_of_interest);

		/// Remove noise by blurring with a Gaussian filter
		GaussianBlur(src, src, Size(3, 3), 0, 0, BORDER_DEFAULT);

		/// Convert the image to grayscale
		cvtColor(src, src_gray, CV_RGB2GRAY);

		/// Create window
		//namedWindow(window_name, CV_WINDOW_AUTOSIZE);

		/// Apply Laplace function
		Mat abs_dst;

		Laplacian(src_gray, dst, ddepth, kernel_size, scale, delta, BORDER_DEFAULT);
		convertScaleAbs(dst, abs_dst);

		printf("Area #%d %d %d %d %d\n", i, x[i], y[i], w[i], h[i]);
		m = 0;
		for (int i = 0; i < abs_dst.rows; i++)
			for (int j = 0; j < abs_dst.cols; j++)
				m += abs_dst.at<unsigned char>(i, j);

		printf("Laplace number is %f\n\n", m*1.0 / (abs_dst.rows*abs_dst.cols));

		if (m*1.0 / (abs_dst.rows*abs_dst.cols) < 256){
			rectangle(image, Rect(x[i] / 3, y[i], (w[i] - x[i]) / 3, h[i] - y[i]), Scalar(255, 0, 0));
			rectangle(wshed, Rect(x[i] / 3, y[i], (w[i] - x[i]) / 3, h[i] - y[i]), Scalar(255, 0, 0));
		}
	}

	imshow("watershed transfm", image);
	imshow("segments", wshed);

	waitKey(0);
	return 0;
}

