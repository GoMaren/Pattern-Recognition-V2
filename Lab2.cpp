#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <vector>
//#include <chrono> 
#include <cmath>
#include <cassert>

using namespace cv;
using namespace std;
//using namespace std::chrono;

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

	//auto start = high_resolution_clock::now();
	// Main loop
	const int loops = 30;
	float alpha = 1.25;
	float g[2][2] = { {0, -alpha}, {-alpha, 0} };
	for (int iter = 0; iter < loops; ++iter)
	{
		for (int ij = 0; ij < modT; ++ij)
			for (int k = 0; k < modKs; ++k)
			{
				// Getting indices
				int Nt = 0;
				int* neighbors = GetNeighborsPlus(ij / width, ij % width, height, width, &Nt);
				int kStar[4];
				for (int ind = 0; ind < modNei; ++ind)
					if (neighbors[ind] != -1)
					{
						// Calculating k*
						float masK[2] = { 0., 0. };
						for (int k_ = 0; k_ < modKs; ++k_)
							masK[k_] = g[k][k_] - phi[ij][ind][k] - phi[neighbors[ind]][(ind + 2) % modNei][k_];
						kStar[ind] = int(masK[1] > masK[0]);
					}

				// Calculating C(t)
				float Ct = 0.;
				for (int ind = 0; ind < modNei; ++ind)
					if (neighbors[ind] != -1)
						Ct += g[k][kStar[ind]] - phi[neighbors[ind]][(ind + 2) % modNei][kStar[ind]];

				Ct += -int(cDiff[ij / width][ij % width][k] > cDiff[ij / width][ij % width][1 - k]);
				Ct /= Nt;

				for (int ind = 0; ind < modNei; ++ind)
					if (neighbors[ind] != -1)
						phi[ij][ind][k] = g[k][kStar[ind]] - phi[neighbors[ind]][(ind + 2) % modNei][kStar[ind]] - Ct;

				delete[] neighbors;
			}
	}
	//auto stop = high_resolution_clock::now();
	//auto duration = duration_cast<microseconds>(stop - start);
	//cout << "Time used for " << loops << " iterations : " << float(duration.count()) / 1000000. << endl;

	// Best Ks
	int* raw = new int[modT];
	for (int ij = 0; ij < modT; ++ij)
	{
		// Getting indices
		int Nt = 0;
		int* neighbors = GetNeighborsPlus(ij / width, ij % width, height, width, &Nt);
		int ind = 0;
		while (neighbors[ind] == -1)
			ind++;
		int ind_ = (ind + 2) % modNei;
		int ij_ = neighbors[ind];

		// Test neighbors
		int Nt_ = 0;
		int* neighbors_ = GetNeighborsPlus(ij_ / width, ij_ % width, height, width, &Nt_);
		assert(neighbors_[ind_] == ij);
		delete[] neighbors_;

		float* kMax = new float[modKs]();

		for (int k = 0; k < modKs; ++k)
		{
			// Calculating kMax
			float* masK = new float[modKs]();

			for (int k_ = 0; k_ < modKs; ++k_)
				masK[k_] = g[k][k_] - phi[ij][ind][k] - phi[ij_][ind_][k_];

			kMax[k] = (masK[1] > masK[0]) ? masK[1] : masK[0];

			delete[] masK;
		}

		raw[ij] = int(kMax[1] > kMax[0]);

		delete[] neighbors;
		delete[] kMax;
	}

	Mat channel[3];
	for (int c = 0; c < 3; ++c)
	{
		channel[c] = Mat::zeros(Size(width, height), CV_8UC1);
		for (int i = 0; i < height; ++i)
			for (int j = 0; j < width; ++j)
				channel[c].at<uchar>(i, j) = (raw[i * width + j] == 0) ? uchar(Ks[0][c]) : uchar(Ks[1][c]);
	}

	vector<Mat> channels;
	for (int i = 0; i < 3; ++i)
		channels.push_back(channel[i]);

	Mat result;
	merge(channels, result);

	namedWindow("Result image", WINDOW_AUTOSIZE);
	imshow("Result image", result);

	imwrite("res1.png", result);

	waitKey(0);
	return 0;
}
