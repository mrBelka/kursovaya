#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace std;

using namespace cv;

/** @function main */
int main(int argc, char** argv)
{
	Mat src, src_gray, dst;
	int kernel_size = 3;
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;
	char* window_name = "Laplace Demo";

	int c;

	argv[1] = "lena2.jpg";

	/// Load an image
	src = imread(argv[1]);

	if (!src.data)
	{
		return -1;
	}

	/// Remove noise by blurring with a Gaussian filter
	GaussianBlur(src, src, Size(3, 3), 0, 0, BORDER_DEFAULT);

	/// Convert the image to grayscale
	cvtColor(src, src_gray, CV_RGB2GRAY);

	/// Create window
	namedWindow(window_name, CV_WINDOW_AUTOSIZE);

	/// Apply Laplace function
	Mat abs_dst;

	Laplacian(src_gray, dst, ddepth, kernel_size, scale, delta, BORDER_DEFAULT);
	convertScaleAbs(dst, abs_dst);

	int max = 0;
	for (int i = 0; i < abs_dst.rows; i++)
		for (int j = 0; j < abs_dst.cols; j++)
			if (abs_dst.at<unsigned char>(i, j) > max)max = abs_dst.at<unsigned char>(i, j);

	cout << max << endl;

	/// Show what you got
	imshow(window_name, abs_dst);

	waitKey(0);

	return 0;
}
