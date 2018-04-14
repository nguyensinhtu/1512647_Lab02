#include "GeometricTransformer.h"
#include <iostream>

int main(int argc, char **argv) {

	if (argc < 5) {
		cout << "loi tham so dong lenh : " << endl;
		return -1;
	}

	Mat srcImage, dstImage;
	GeometricTransformer *geometricTransformer = new GeometricTransformer();
	PixelInterpolate *pixelInterpolate;
	if (strcmp(argv[2], "--bl") == 0) {
		pixelInterpolate = new BilinearInterpolate();
	}
	else if (strcmp(argv[2], "--nn") == 0){
		pixelInterpolate = new NearestNeighborInterpolate();
	}
	else {
		cout << "not found : " << argv[2] << endl;
		return -1;
	}

	srcImage = imread(argv[3], CV_LOAD_IMAGE_ANYCOLOR);

	if (!srcImage.data) {
		cout << "can not load image " << endl;
		return -1;
	}

	float num = atof(argv[4]);

	int res = 0;
	if (strcmp(argv[1], "--zoom") == 0) {
		res = geometricTransformer->Scale(srcImage, dstImage, num, num, pixelInterpolate);
	}
	else if (strcmp(argv[1], "--rotate") == 0) {
		res = geometricTransformer->RotateKeepImage(srcImage, dstImage, num, pixelInterpolate);
	}
	else if (strcmp(argv[1], "--rotateN") == 0) {
		res = geometricTransformer->RotateUnkeepImage(srcImage, dstImage, num, pixelInterpolate);
	}
	else {
		cout << "not found : " << argv[1] << endl;
		return -1;
	}

	if (res == 0) {
		cout << "thao tac that bai" << endl;
	}

	return 0;
}