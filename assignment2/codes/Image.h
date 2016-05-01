//*****************************************************************************
//
// Image.h : Defines the class operations on images
//
// Author - Parag Havaldar
// Main Image class structure 
//
//*****************************************************************************
#define _USE_MATH_DEFINES
#include <cmath>
#ifndef IMAGE_DISPLAY
#define IMAGE_DISPLAY

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "afxwin.h"
#include <iostream>
#include <fstream>
// C RunTime Header Files
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// Class structure of Image 
// Use to encapsulate an RGB image
class MyImage 
{

private:
	int		Width;					// Width of Image
	int		Height;					// Height of Image
	char	ImagePath[_MAX_PATH];	// Image location
	char*	Data;					// RGB data of the image
	float ****DCTCoeffs;//DCT values
	float ****reducedDCTs;
	float *DWTCoeffs;//DWT values
	float *reducedDWTs;

public:
	// Constructor
	MyImage();
	// Copy Constructor
	MyImage::MyImage( MyImage *otherImage);
	// Destructor
	~MyImage();

	// operator overload
	MyImage & operator= (const MyImage & otherImage);

	// Reader & Writer functions
	void	setWidth( const int w)  { Width = w; }; 
	void	setHeight(const int h) { Height = h; }; 
	void	setImageData( const char *img ) { Data = (char *)img; };
	void	setImagePath( const char *path) { strcpy(ImagePath, path); }
	int		getWidth() { return Width; };
	int		getHeight() { return Height; };
	char*	getImageData() { return Data; };
	char*	getImagePath() { return ImagePath; }

	// Input Output operations
	bool	ReadImage();
	bool	WriteImage();

	// Modifications
	bool	Assignment1(int y, int u, int v, int q);
	//change char* Data into float
	float* MyImage::Char2RGB();
	//For the idx-th block, calculate F(u,v) 
	float* OperationDCT(int idx, int u, int v);
	//For the idx-th block, calculate f(x,y) 
	float* OperationIDCT(int idx, int x, int y);
	//Apply DCT to whole image
	bool MyDCT();
	// zero out coefficients
	bool DCTCompression(int numOfCoeff);
	//apply iDCT to whole image
	bool MyIDCT();
	float** GetArray(bool rc, int idx,float* inputArr);
	//1D DWT
	float* OperationDWT(int length, float* DWTVals);
	//1D iDWT
	float* OperationIDWT(int length, float* DWTVals);
	//Apply Haar DWT, row first then column
	bool MyDWT();
	bool DWTCompression(int numOfCoeff);
	//IDWT, column first then row
	bool MyIDWT();
	bool Release();
};

#endif //IMAGE_DISPLAY
