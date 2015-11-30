/*
Copyright (C) 2006 Pedro Felzenszwalb

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/

#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "image.h"
#include "misc.h"
#include "pnmfile.h"
#include "segment-image.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#define SEGMENTS_HEIGHT 20
#define SEGMENTS_WIDTH 20
#define DIFFERENCE 500000


using namespace cv;

int main(int argc, char **argv) {
	//загружаем картинку в jpeg
	Mat imag = imread("lena.jpg");
	

	//конвертируем ее в ppm
	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_PXM_BINARY);
	compression_params.push_back(1);

	imwrite("bez.ppm", imag, compression_params); 

	//freopen("output.txt","w",stdout);

	float sigma = 0.8;
	float k = 800;
	int min_size = 800;

	printf("loading input image.\n");
	image<rgb> *input = loadPPM("bez.ppm");

	printf("processing\n");
	int num_ccs;
	image<rgb> *seg = segment_image(input, sigma, k, min_size, &num_ccs);

	// разбиение сегментов на прямоугольники
	int x, y, w, h;
	int t = 0, tmp, l;

	for (int i = 0; i < seg->width(); i += SEGMENTS_HEIGHT){
		if (i >= seg->width()) break;
		tmp = imRef(seg, i, 0).r + imRef(seg, i, 0).g * 256 + imRef(seg, i, 0).b * 256 * 256;
		l = 0;
		for (int j = 0; j < seg->height(); j ++) {
			if (abs(imRef(seg, i, j).r + imRef(seg, i, j).g * 256 + imRef(seg, i, j).b * 256 * 256 - tmp) > DIFFERENCE || j == seg->height()) {
				x = l;
				y = i;
				h = seg->height() - 1 > i + SEGMENTS_HEIGHT ? i + SEGMENTS_HEIGHT : seg->height();
				w = j;
				t++;
				l = j;
				tmp = imRef(seg, i, j).r + imRef(seg, i, j).g * 256 + imRef(seg, i, j).b * 256 * 256;


				//если часом вышли за границы
				if (h >= seg->height())h = seg->height() - 1;
				if (w >= seg->width())w =  seg->width() - 1;
				// не анализируем слишком маленькие сегменты
				if(w - x<SEGMENTS_WIDTH || h - y<SEGMENTS_HEIGHT)
					continue;

				rectangle(imag, Rect(x, y, w - x, h - y), Scalar(255, 0, 0));
			}
		}
	}

	printf("Number of segment %d", t);
	imshow("result", imag);

	savePPM(seg, "123.ppm");
	waitKey(0);
	return 0;
}

