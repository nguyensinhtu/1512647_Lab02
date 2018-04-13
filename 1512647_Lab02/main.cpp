#include "GeometricTransformer.h"
#include <iostream>

int main(int argc, char **argv) {

	Mat dstImage;
	GeometricTransformer abc;
	Mat image = imread(argv[1], CV_LOAD_IMAGE_ANYCOLOR);

	if (!image.data) {
		cout << "ko load duoc anh" << endl;
		return -1;
	}

	//namedWindow("window", WINDOW_AUTOSIZE);
	//imshow("window", image);
	//waitKey(0);

	PixelInterpolate * interpolator = new NearestNeighborInterpolate();
	PixelInterpolate * interpolator1 = new BilinearInterpolate();
	//abc.Scale(image, dstImage, 1.7, 1.7, interpolator1);
	abc.RotateKeepImage(image, dstImage, 70, interpolator1);
	return 0;
}