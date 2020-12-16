#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>

using namespace std;

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

void test_1()
{
	int width = 400;
	int height = 300;
	int modT = width * height;
	int modNei = 4;

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

		delete[] neighbors;
	}

	cout << "Test 1 passed"<< endl;
}

void test_2()
{
	int width = 2;
	int height = 2;
	int modKs = 2;

	int*** cDiff = new int** [height];
	for (int i = 0; i < height; ++i)
	{
		cDiff[i] = new int* [width];
		for (int j = 0; j < width; ++j)
		{
			cDiff[i][j] = new int[3];
			for (int k = 0; k < modKs; ++k)
				cDiff[i][j][k] = i - j;
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

	// Main loop
	float g[2][2] = { {0, -1}, {-1, 0} };
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

	for (int ij = 0; ij < modT; ++ij)
		for (int ind = 0; ind < modNei; ++ind)
			for (int k = 0; k < modKs; ++k)
				assert(phi[ij][ind][k] == 0);

	cout << "Test 2 passed" << endl;
}

int main()
{
	test_1();
	test_2();
	
	return 0;
}