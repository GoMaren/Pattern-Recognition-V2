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

	waitKey(0);
	return 0;
}