//*****************************************************************************
//
// Image.cpp : Defines the class operations on images
//
// Author - Parag Havaldar
// Code used by students as starter code to display and modify images
//
//*****************************************************************************

#include "Image.h"


// Constructor and Desctructors
MyImage::MyImage() 
{
	Data = NULL;
	Width = -1;
	Height = -1;
	ImagePath[0] = 0;
}

MyImage::~MyImage()
{
	if ( Data )
		delete Data;
}


// Copy constructor
MyImage::MyImage( MyImage *otherImage)
{
	Height = otherImage->Height;
	Width  = otherImage->Width;
	Data   = new char[Width*Height*3];
	strcpy(otherImage->ImagePath, ImagePath );

	for ( int i=0; i<(Height*Width*3); i++ )
	{
		Data[i]	= otherImage->Data[i];
	}


}



// = operator overload
MyImage & MyImage::operator= (const MyImage &otherImage)
{
	Height = otherImage.Height;
	Width  = otherImage.Width;
	Data   = new char[Width*Height*3];
	strcpy( (char *)otherImage.ImagePath, ImagePath );

	for ( int i=0; i<(Height*Width*3); i++ )
	{
		Data[i]	= otherImage.Data[i];
	}
	
	return *this;

}


// MyImage::ReadImage
// Function to read the image given a path
bool MyImage::ReadImage()
{

	// Verify ImagePath
	if (ImagePath[0] == 0 || Width < 0 || Height < 0 )
	{
		fprintf(stderr, "Image or Image properties not defined");
		fprintf(stderr, "Usage is `Image.exe Imagefile w h`");
		return false;
	}
	
	// Create a valid output file pointer
	FILE *IN_FILE;
	IN_FILE = fopen(ImagePath, "rb");
	if ( IN_FILE == NULL ) 
	{
		fprintf(stderr, "Error Opening File for Reading");
		return false;
	}

	// Create and populate RGB buffers
	int i;
	char *Rbuf = new char[Height*Width]; 
	char *Gbuf = new char[Height*Width]; 
	char *Bbuf = new char[Height*Width]; 

	for (i = 0; i < Width*Height; i ++)
	{
		Rbuf[i] = fgetc(IN_FILE);
	}
	for (i = 0; i < Width*Height; i ++)
	{
		Gbuf[i] = fgetc(IN_FILE);
	}
	for (i = 0; i < Width*Height; i ++)
	{
		Bbuf[i] = fgetc(IN_FILE);
	}
	
	// Allocate Data structure and copy````````````````````````````````````
	Data = new char[Width*Height*3];
	for (i = 0; i < Height*Width; i++)
	{
		Data[3*i]	= Bbuf[i];
		Data[3*i+1]	= Gbuf[i];
		Data[3*i+2]	= Rbuf[i];
	}

	// Clean up and return
	delete Rbuf;
	delete Gbuf;
	delete Bbuf;
	fclose(IN_FILE);

	return true;

}



// MyImage functions defined here
bool MyImage::WriteImage()
{
	// Verify ImagePath
	// Verify ImagePath
	if (ImagePath[0] == 0 || Width < 0 || Height < 0 )
	{
		fprintf(stderr, "Image or Image properties not defined");
		return false;
	}
	
	// Create a valid output file pointer
	FILE *OUT_FILE;
	OUT_FILE = fopen(ImagePath, "wb");
	if ( OUT_FILE == NULL ) 
	{
		fprintf(stderr, "Error Opening File for Writing");
		return false;
	}

	// Create and populate RGB buffers
	int i;
	char *Rbuf = new char[Height*Width]; 
	char *Gbuf = new char[Height*Width]; 
	char *Bbuf = new char[Height*Width]; 

	for (i = 0; i < Height*Width; i++)
	{
		Bbuf[i] = Data[3*i];
		Gbuf[i] = Data[3*i+1];
		Rbuf[i] = Data[3*i+2];
	}

	
	// Write data to file
	for (i = 0; i < Width*Height; i ++)
	{
		fputc(Rbuf[i], OUT_FILE);
	}
	for (i = 0; i < Width*Height; i ++)
	{
		fputc(Gbuf[i], OUT_FILE);
	}
	for (i = 0; i < Width*Height; i ++)
	{
		fputc(Bbuf[i], OUT_FILE);
	}
	
	// Clean up and return
	delete Rbuf;
	delete Gbuf;
	delete Bbuf;
	fclose(OUT_FILE);

	return true;

}




// Here is where you would place your code to modify an image
// eg Filtering, Transformation, Cropping, etc.
bool MyImage::Modify(int y, int u, int v, int q)
{
	//for testing, create a log file
	std::ofstream log;
	log.open("log.txt");
	//store the calculation results of YUV values. NOTE: float is used for precison
	double* yuv;
	yuv=new double[Width*Height * 3];
	/*function: RGB2YUV*/
	for (int i = 0; i < Width*Height; i++)
	{
		double valueY, valueU, valueV;
		//bgr->YUV NOTE:given Data is signed char, convert the range to 0-255
		valueY = 0.114*unsigned char(Data[3 * i]) + 0.587*unsigned char(Data[3 * i + 1]) + 0.299*unsigned char(Data[3 * i + 2]);
		valueU = -0.322*unsigned char(Data[3 * i]) - 0.274*unsigned char(Data[3 * i + 1]) + 0.596*unsigned char(Data[3 * i + 2]);
		valueV = 0.312*unsigned char(Data[3 * i]) - 0.523*unsigned char(Data[3 * i + 1]) + 0.211*unsigned char(Data[3 * i + 2]);
		yuv[3 * i] = valueY;
		yuv[3 * i + 1] = valueU;
		yuv[3 * i + 2] = valueV;
	}
	
	//subsampling
	//subsamples for YUV 3 channels
	double* subsamplesY, *subsamplesU, *subsamplesV;
	int numY = int(ceil(float(Width)/y))*Height ;
	int numU = int(ceil(float(Width)/u))*Height ;
	int numV = int(ceil(float(Width)/v))*Height ;
	subsamplesY = new double[numY];
	subsamplesU = new double[numU];
	subsamplesV = new double[numV];
	int idx1 = 0, idx2=0, idx3=0;
	for (int i = 0; i < Width*Height; i++)
	{
		if ((i%Width)% y == 0 ) {
			subsamplesY[idx1] = yuv[3*i];
			idx1++;
		}
		if ((i%Width)% u == 0) {
			subsamplesU[idx2] = yuv[3*i+1];
			idx2++;
		}
		if ((i%Width)% v == 0) {
			subsamplesV[idx3] = yuv[3*i+2];
			idx3++;
		}

	}
	/*function: upsampling*/
	
	float* upsamples = new float[Width*Height * 3];
	int a = 0,b=0,c=0;
	for (int i = 0; i < Width*Height; i++)
	{
		double weight = double(i %y)/ y;//weight for upsampling
		if ((i%Width) %y == 0) {//locations where hasve subsamples
			upsamples[3 * i ] = subsamplesY[a];
			a++;
			
		}
		//right boundaries copy from left subsamples
		else if ((i%Width) > (numY / Height - 1)*y) {
			upsamples[3 * i ] = subsamplesY[a - 1];
		}
		else {
			//interpolation
			upsamples[3 * i] = (1 - weight)*subsamplesY[a - 1] + weight*subsamplesY[a];
		}


		weight = double(i %u) / u;//weight for upsampling
		if ((i%Width) %u ==0) {
			upsamples[3 * i+1] = subsamplesU[b];
			++b;
		}
		else if ((i%Width) > (numU / Height - 1)*u) {
			upsamples[3 * i + 1] = subsamplesU[b-1];
		}
		else {
			upsamples[3 * i + 1] = (1 - weight)*subsamplesU[b - 1] + weight*subsamplesU[b];
		}

		weight = double(i %v) / v;
		if ((i%Width) %v == 0) {
			upsamples[3 * i + 2] = subsamplesV[c];
			++c;
		}
		else if ((i%Width) > (numV / Height - 1)*v) {
			upsamples[3 * i + 2] = subsamplesV[c -1 ];
		}
		else {
			upsamples[3 * i + 2] = (1 - weight)*subsamplesV[c-1] + weight*subsamplesV[c];
		}

	}
	
	/*function: YUV2RGB*/
	float avgB = 0, avgG = 0, avgR = 0;
	for (int i = 0; i < Width*Height; i++)
	{
		//calculation of error rate
		//float errorB = 0, errorG = 0, errorR = 0;
		//errorB = (char(upsamples[3 * i]) - 1.106*char(upsamples[3 * i + 1]) + 1.703*char(upsamples[3 * i + 2]) - Data[3 * i])/ float(Data[3 * i]+1);
		//errorG = (upsamples[3 * i] - 0.272*upsamples[3 * i + 1] - 0.647*upsamples[3 * i + 2] - Data[3 * i+1])/ float(Data[3*i+1]+1);
		//errorR = (char(upsamples[3 * i]) + 0.956*char(upsamples[3 * i + 1]) + 0.621*char(upsamples[3 * i + 2]) - Data[3 * i+2])/ float(Data[3*i+2]+1);
		//avgB += errorB;
		//avgG += errorG;
		//avgR += errorR;

		//YUV->bgr
		double blue,red,green;
		blue = upsamples[3 * i]- 1.106*upsamples[3 * i + 1] + 1.703*upsamples[3 * i + 2] ;
		green = upsamples[3 * i] - 0.272*upsamples[3 * i + 1] - 0.647*upsamples[3 * i + 2];
		red = upsamples[3 * i] + 0.956*upsamples[3 * i + 1]  + 0.621*upsamples[3 * i + 2] ;
		//dealing with distortion
		if (blue < 0) blue= 0;
		if (blue > 255) blue = 255;
		if (green < 0) green = 0;
		if (green > 255) green = 255;
		if (red < 0) red = 0;
		if (red > 255) red = 255;
		//assign Image values
		Data[3 * i ] = char(blue);
		Data[3 * i + 1] = char(green);
		Data[3 * i + 2] = char(red);
		
	}
	//avgB /= Width*Height;
	//avgG /= Width*Height;
	//avgR /= Width*Height;
	//log << avgB << "  " << avgG << " " << avgR << std::endl;
	
	/*function quantization*/
	
	for (int i = 0; i < 3*Width*Height; i++)
	{
		int level = floor(unsigned char(Data[i]) / (256.0 / q));
		Data[i] =  level*(256 / q) ;
		//log << level << std::endl;
	}
	log.close();

	delete yuv;
	delete subsamplesU;
	delete subsamplesV;
	delete subsamplesY;
	delete upsamples;
	return false;
	
}