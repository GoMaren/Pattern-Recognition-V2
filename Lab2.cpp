#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <vector>
#include <chrono> 
#include <cmath>
#include <cassert>

using namespace cv;
using namespace std;
using namespace std::chrono;

int* GetNeighborsPlus(int i, int j, int h, int w, int* Nt)
{
	int* Neighbors = new int[4];

	int Index = 0;
	(*Nt) = 0;

	if (i != 0)
	{
		Neighbors[Index] = (i - 1) * w + (j);
		(*Nt)++;
	}
	else
		Neighbors[Index] = -1;
	Index++;

	if (j != 0)
	{
		Neighbors[Index] = (i)*w + (j - 1);
		(*Nt)++;
	}
	else
		Neighbors[Index] = -1;
	Index++;

	if (h - i != 1)
	{
		Neighbors[Index] = (i + 1) * w + (j);
		(*Nt)++;
	}
	else
		Neighbors[Index] = -1;
	Index++;

	if (w - j != 1)
	{
		Neighbors[Index] = (i)*w + (j + 1);
		(*Nt)++;
	}
	else
		Neighbors[Index] = -1;

	return Neighbors;
}

int SqrColorDiff(int* c1, int* c2)
{
	int diff = 0;
	for (int i = 0; i < 3; ++i)
		diff += (c1[i] - c2[i]) * (c1[i] - c2[i]);
	return diff;
}

int main()
{
	Mat image;
	image = imread("1.png", IMREAD_UNCHANGED);
	namedWindow("Original image", WINDOW_AUTOSIZE);
	imshow("Original image", image);

	int height = image.size().height;
	int width = image.size().width;

	Mat chanels[3];
	split(image, chanels);

	// Old-school c-style arrays
	int*** colors = new int** [height];
	for (int i = 0; i < height; ++i)
	{
		colors[i] = new int* [width];
		for (int j = 0; j < width; ++j)
		{
			colors[i][j] = new int[3];
			for (int c = 0; c < 3; ++c)
				colors[i][j][c] = int(chanels[c].at<uchar>(i, j));
		}
	}

	// Target colors
	const int modKs = 2;
	int Ks[modKs][3] = { {0, 255, 0}, {255, 0, 0} };
	//int Ks[modKs][3] = { {255, 255, 255}, {0, 0, 0} };

	// Sqr color diffs
	int*** cDiff = new int** [height];
	for (int i = 0; i < height; ++i)
	{
		cDiff[i] = new int* [width];
		for (int j = 0; j < width; ++j)
		{
			cDiff[i][j] = new int[3];
			for (int k = 0; k < modKs; ++k)
				cDiff[i][j][k] = SqrColorDiff(colors[i][j], Ks[k]);
		}
	}

	// Initialize phi
	const int modNei = 4;
	int modT = height * width;
	float*** phi = new float** [modT];
	for (int ij = 0; ij < modT; ++ij)
	{
		phi[ij] = new float* [modNei];
		for (int ind = 0; ind < modNei; ++ind)
		{
			phi[ij][ind] = new float[modKs]();
			for (int k = 0; k < modKs; ++k)
			{
				phi[ij][ind][k] = 0.;
			}
		}
	}

	waitKey(0);
	return 0;
}