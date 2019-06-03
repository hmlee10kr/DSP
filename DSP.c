#pragma warning(disable:4996)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#define filter 11.0
#define PI 3.141592

int main()
{
	FILE* fpInputImage = 0;
	FILE* fpOutputImage = 0;
	FILE* fpOutputremove = 0;
	FILE* fpOutLPF = 0;
	FILE* fpOutHPF = 0;
	unsigned char** ppInputImageBuffer = 0;
	unsigned char** RemoveImage = 0;
	unsigned char** LPFImage = 0;
	unsigned char** HPFImage = 0;
	unsigned char** Original = 0;
	unsigned char** NoiseImage = 0;
	int **Noise = 0;
	int **Remove = 0;
	int **LPF = 0;
	int **HPF = 0;

	double lpf[3][3] = { { 1 / 9, 1 / 9, 1 / 9 }, { 1 / 9, 1 / 9, 1 / 9 }, { 1 / 9, 1 / 9, 1 / 9 } };
	int hpf[3][3] = { { 1, 2, 1 }, { 0, 0, 0 }, { -1, -2, -1 } };
	int i, j, k,l;

	// input file open
	fpInputImage = fopen("einstein.img", "rb");

	// memory allocaiton
	ppInputImageBuffer = (unsigned char**)malloc(sizeof(unsigned char*)* 512);
	RemoveImage = (unsigned char**)malloc(sizeof(unsigned char*)* 512);
	Original = (unsigned char**)malloc(sizeof(unsigned char*)* 512);
	LPFImage = (unsigned char**)malloc(sizeof(unsigned char*)* 512);
	HPFImage = (unsigned char**)malloc(sizeof(unsigned char*)* 512);
	NoiseImage = (unsigned char**)malloc(sizeof(unsigned char*)* 512);
	Noise = (int**)malloc(sizeof(int*)* 512);
	Remove = (int**)malloc(sizeof(int*)* 512);
	LPF = (int**)malloc(sizeof(int*)* 512);
	HPF = (int**)malloc(sizeof(int*)* 512);
	for (i = 0; i < 512; i++)
	{
		ppInputImageBuffer[i] = (unsigned char*)malloc(sizeof(unsigned char*)* 512);
		RemoveImage[i] = (unsigned char*)malloc(sizeof(unsigned char*)* 512);
		Original[i] = (unsigned char*)malloc(sizeof(unsigned char*)* 512);
		LPFImage[i] = (unsigned char*)malloc(sizeof(unsigned char*)* 512);
		HPFImage[i] = (unsigned char*)malloc(sizeof(unsigned char*)* 512);
		NoiseImage[i] = (unsigned char*)malloc(sizeof(unsigned char*)* 512);
		Noise[i] = (int*)malloc(sizeof(int*)* 512);
		Remove[i] = (int*)malloc(sizeof(int*)* 512);
		LPF[i] = (int*)malloc(sizeof(int*)* 512);
		HPF[i] = (int*)malloc(sizeof(int*)* 512);
	}

	for (i = 0; i < 512; i++)
	{
		for (j = 0; j < 512; j++)
		{
			RemoveImage[i][j] = 0;
			Original[i][j] = 0;
			LPFImage[i][j] = 0;
			HPFImage[i][j] = 0;
			NoiseImage[i][j] = 0;
			Noise[i][j] = 0;
			Remove[i][j] = 0;
			LPF[i][j] = 0;
			HPF[i][j] = 0;
		}
	}
	// input file read to memory from the file
	for (i = 0; i < 512; i++)
	{
		fread(ppInputImageBuffer[i], sizeof(unsigned char), 512, fpInputImage);
	}
	fclose(fpInputImage);

	fpInputImage = fopen("einstein.img", "rb");
	for (i = 0; i < 512; i++)
	{
		fread(Original[i], sizeof(unsigned char), 512, fpInputImage);
	}
	
	//noise
	for (i = 0; i < 512; i++)
	{
		for (j = 0; j < 512; j++)
		{
			Noise[i][j] = (Original[i][j] + 128 * cos(2 * PI * j / 11.0) + 128);
		}
	}

	for (i = 0; i < 512; i++)
	{
		for (j = 0; j < 512; j++)
		{
			if (Noise[i][j] < 0)
				Noise[i][j] = 0;
			if (Noise[i][j] > 255)
				Noise[i][j] = 255;
		}
	}

	for (i = 0; i < 512; i++)
	{
		for (j = 0; j < 512; j++)
		{
			NoiseImage[i][j] = (unsigned char)Noise[i][j];
		}
	}

	//remove
	for (i = 0; i < 512; i++)
	{
		for (j = (filter - 1) / 2; j <= 512 - ((filter - 1) / 2); j++)
		{
			for (k = (-1)*((filter - 1) / 2); k <= ((filter - 1) / 2); k++)
			{
				Remove[i][j] += (1 / 11.0)*(double)NoiseImage[i][j + k];
			}
		}
	}

	for (i = 0; i < 512; i++)
	{
		for (j = 0; j < 512; j++)
		{
			if (Remove[i][j] < 0)
				Remove[i][j] = 0;
			if (Remove[i][j] > 255)
				Remove[i][j] = 255;
		}
	}

	for (i = 0; i < 512; i++)
	{
		for (j = 0; j < 512; j++)
		{
			RemoveImage[i][j] = (unsigned char)Remove[i][j];
		}
	}

	//LPF
	for (i = 1; i < 511; i++)
	{
		for (j = 1; j < 511; j++)
		{
			for (k = i-1; k <= i+1; k++){
				for (l = j-1; l <= j+1; l++){
					LPF[i][j] += (double)Original[k][l]*(1/9.0);
				}
			}
		}
	}

	for (i = 0; i < 512; i++)
	{
		for (j = 0; j < 512; j++)
		{
			if (LPF[i][j] < 0)
				LPF[i][j] = 0;
			if (LPF[i][j] > 255)
				LPF[i][j] = 255;
		}
	}

	for (i = 0; i < 512; i++)
	{
		for (j = 0; j < 512; j++)
		{
			LPFImage[i][j] = (unsigned char)LPF[i][j];
		}
	}


	//HPF
	for (i = 1; i < 511; i++)
	{
		for (j = 1; j < 511; j++)
		{
			for (k = -1; k <= 1; k++){
				for (l = -1; l <= 1; l++){
						HPF[i][j] += Original[i+k][j+l]*hpf[k+1][l+1];
				}
			}
		}
	}

	for (i = 0; i < 512; i++)
	{
		for (j = 0; j < 512; j++)
		{
			if (HPF[i][j] < 0)
				HPF[i][j] = 0;
			if (HPF[i][j] > 255)
				HPF[i][j] = 255;
		}
	}

	for (i = 0; i < 512; i++)
	{
		for (j = 0; j < 512; j++)
		{
			HPFImage[i][j] = (unsigned char)HPF[i][j];
		}
	}
	
	// output fileopen
	fpOutputImage = fopen("result.img", "wb");
	fpOutputremove = fopen("reverse.img", "wb");
	fpOutLPF = fopen("LPF.img", "wb");
	fpOutHPF = fopen("HPF.img", "wb");

	//write the file
	for (i = 0; i < 512; i++){
		fwrite(NoiseImage[i], sizeof(unsigned char), 512, fpOutputImage);
		fwrite(RemoveImage[i], sizeof(unsigned char), 512, fpOutputremove);
		fwrite(LPFImage[i], sizeof(unsigned char), 512, fpOutLPF);
		fwrite(HPFImage[i], sizeof(unsigned char), 512, fpOutHPF);
	}
	
	for (i = 0; i < 512; i++){
		free(ppInputImageBuffer[i]);
	}

	free(ppInputImageBuffer);
	fclose(fpInputImage);
	fclose(fpOutputImage);
	return 0;
}