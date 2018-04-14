#include "GeometricTransformer.h"

// scale va rotate quanh goc toa do nay
int dX, dY;

void AffineTransform::Translate(float dx, float dy)
{
	// tao ma tran tinh tien;
	Mat matrix = (Mat_<float>(3, 3) << 1, 0, 0, 0, 1, 0, dx, dy, 1);

	_matrixTransform = _matrixTransform*matrix;
}

void AffineTransform::Rotate(float angle)
{
	float x = cos(angle * M_PI / 180.0);
	float x2 = sin(angle *M_PI / 180.0);
	//Mat matrix = (Mat_<float>(3, 3) << x, x2, 0, -x2, x, 0, (1 - x)*dX + x2*dY, -x2*dX + (1 - x)*dY, 1);
	Mat matrix = (Mat_<float>(3, 3) << x, x2, 0, -x2, x, 0, 0, 0, 1);
	_matrixTransform = _matrixTransform*matrix;
}

void AffineTransform::Scale(float sx, float sy)
{
	Mat matrix = (Mat_<float>(3, 3) << sx, 0, 0, 0, sy, 0, (1 - sx)*dX, (1 - sy)*dY, 1);

	_matrixTransform = _matrixTransform*matrix;
}

void AffineTransform::TransformPoint(float & x, float & y)
{
	Mat matrix = (Mat_<float>(1, 3) << x, y, 1);
	Mat res = matrix*_matrixTransform;
	x = res.at<float>(0, 0);
	y = res.at<float>(0, 1);
}

void AffineTransform::IdentifyMatrix()
{
	Mat matrix = (Mat_<float>(3, 3) << 1, 0, 0, 0, 1, 0, 0, 0, 1);
	_matrixTransform = matrix;
}

void AffineTransform::inv()
{
	_matrixTransform =  _matrixTransform.inv();
}



AffineTransform::AffineTransform()
{
	// khởi tạo ma trận
	_matrixTransform = (Mat_<float>(3, 3) << 1, 0, 0, 0, 1, 0, 0, 0, 1);
}

AffineTransform::~AffineTransform()
{
	_matrixTransform.release();
}

void NearestNeighborInterpolate::Interpolate(float tx, float ty, uchar * pSrc, int srcWidthStep, int nChannels, uchar * pDstRow)
{
	int x = round(tx); int y = round(ty);

	for (int i = 0; i < nChannels; ++i) {
		uchar  v = (uchar)(pSrc[x*srcWidthStep + y*nChannels + i]);
		pDstRow[i] = v;
	}
}

NearestNeighborInterpolate::NearestNeighborInterpolate()
{
}

NearestNeighborInterpolate::~NearestNeighborInterpolate()
{
}

void BilinearInterpolate::Interpolate(float tx, float ty, uchar * pSrc, int srcWidthStep, int nChannels, uchar * pDstRow)
{
	int x = round(tx); float a = tx - x;
	int y = round(ty); float b = ty - y;

	for (int i = 0; i < nChannels; ++i) {
		pDstRow[i] = (uchar)((1 - a)*(1 - b)*((int)pSrc[x*srcWidthStep + y*nChannels + i]) + a*(1 - b)*((int)pSrc[(x + 1)*srcWidthStep + y*nChannels + i]) +
			b*(1 - a)*((int)pSrc[x*srcWidthStep + (y + 1)*nChannels + i]) + a*b*((int)pSrc[(x + 1)*srcWidthStep + (y + 1)*nChannels + i]));
	}
}

BilinearInterpolate::BilinearInterpolate()
{
}

BilinearInterpolate::~BilinearInterpolate()
{
}

int GeometricTransformer::Transform(const Mat & beforeImage, Mat & afterImage, AffineTransform * transformer, PixelInterpolate * interpolator)
{
	if (!beforeImage.data) {
		return 0;
	}

	int stepwidth = beforeImage.step[0];
	cout << beforeImage.size() << endl;
	int nch = beforeImage.step[1];
	uchar *data = (uchar*)beforeImage.data;
	uchar* afterdata = (uchar*)afterImage.data;

	int width = beforeImage.cols;
	int height = beforeImage.rows;

	int stepwidth2 = afterImage.step[0];
	int height1 = afterImage.rows;
	int width1 = afterImage.cols;

	// lay ma tran nghich dao
	transformer->inv();

	int maxx = 0, maxy = 0;
	for (int y = 0; y < height1; ++y, afterdata += stepwidth2) {
		uchar *pRow = afterdata;
		for (int x = 0; x < width1; ++x, pRow += nch) {
			float yy = y, xx = x;
			transformer->TransformPoint(yy, xx);

			if (round(xx) >= width || round(yy) >= height || xx < 0 || yy < 0)
				continue;
			interpolator->Interpolate(yy, xx, data, stepwidth, nch, pRow);
		}
	}
	return 1;
}

int GeometricTransformer::RotateKeepImage(const Mat & srcImage, Mat & dstImage, float angle, PixelInterpolate * interpolator)
{
	if (!srcImage.data) {
		return 0;
	}

	
	int width = srcImage.cols; 
	int height = srcImage.rows; 

	float x = cos(angle * M_PI / 180.0);
	float x2 = sin(angle *M_PI / 180.0);

	int dstWidth = (int)(width*x + height*x2);
	int dstHeight = (int)(width*x2 + height*x);

	dstImage.create(dstHeight, dstWidth, srcImage.type());

	AffineTransform *aff = new AffineTransform();
	aff->Translate((dstHeight / 2) - (height / 2), (dstWidth / 2) - (width / 2));
	aff->Translate(-(dstHeight / 2), -(dstWidth / 2));
	aff->Rotate(angle);
	aff->Translate(dstHeight / 2, dstWidth / 2);
	//aff->Translate(height - height, width - width);
	GeometricTransformer::Transform(srcImage, dstImage, aff, interpolator);

	if (!dstImage.data) {
		return 0;
	}

	namedWindow("Rotate keep image", WINDOW_AUTOSIZE);
	cout << dstImage.size() << endl;
	imshow("Rotate keep image", dstImage);
	waitKey(0);
	return 1;
}

int GeometricTransformer::RotateUnkeepImage(const Mat & srcImage, Mat & dstImage, float angle, PixelInterpolate * interpolator)
{
	int width = srcImage.cols;
	int height = srcImage.rows;

	//dX = height / 2;
	//dY = width / 2;
	dstImage.create(height, width, srcImage.type());
	AffineTransform *aff = new AffineTransform();
	
	aff->Translate(-(height / 2), - (width / 2));

	aff->Rotate(angle);

	aff->Translate(height / 2, width / 2);

	GeometricTransformer::Transform(srcImage, dstImage, aff, interpolator);

	if (!dstImage.data) {
		return 0;
	}

	namedWindow("Rotate unkeep image", WINDOW_AUTOSIZE);
	imshow("Rotate unkeep image", dstImage);
	waitKey(0);
	return 1;
}

int GeometricTransformer::Scale(const Mat & srcImage, Mat & dstImage, float sx, float sy, PixelInterpolate * interpolator)
{
	if (!srcImage.data) {
		return 0;
	}

	int width = srcImage.cols;
	int height = srcImage.rows;
	
	dX = height / 2;
	dY = width / 2;
	dstImage.create(width, height, srcImage.type());
	AffineTransform *aff = new AffineTransform();
	aff->Scale(sx, sy);
	
	GeometricTransformer::Transform(srcImage, dstImage, aff, interpolator);

	if (!dstImage.data) {
		return 0;
	}

	namedWindow("Scale", WINDOW_AUTOSIZE);
	imshow("Scale", dstImage);
	waitKey(0);
	//dstImage.create
	return 1;
}
