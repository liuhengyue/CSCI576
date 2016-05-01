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





// assignment 1 codes for subsampling and quantization
 bool MyImage::Assignment1(int y, int u, int v, int q)
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

//define DCT operations
//For the idx-th block, calculate F(u,v) 
float* MyImage::OperationDCT(int idx, int u, int v) {
	float Cu=1;
	float Cv = 1;
	//for u,v=0 situation
	if (u == 0) {
		Cu = 1 / sqrt(2);
	}
	if (v == 0) {
		Cv = 1 / sqrt(2);
	}
	//DCT values for each channel
	float* dctValuesRGB= new float[3];
	dctValuesRGB[0] = 0;
	dctValuesRGB[1] = 0;
	dctValuesRGB[2] = 0;
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			int i = x + (idx % 64) * 8 + (y + (idx / 64) * 8) * Width;
			dctValuesRGB[0] += float(unsigned char(Data[3 * i+2])) * cos((2 * x + 1)*u*M_PI / 16)*cos((2 * y + 1)*v*M_PI / 16);
			dctValuesRGB[1] += float(unsigned char(Data[3 * i + 1])) * cos((2 * x + 1)*u*M_PI / 16)*cos((2 * y + 1)*v*M_PI / 16);
			dctValuesRGB[2] += float(unsigned char(Data[3 * i ])) * cos((2 * x + 1)*u*M_PI / 16)*cos((2 * y + 1)*v*M_PI / 16);
			
		}
	} 
	//multiply factor
	dctValuesRGB[0] *= Cu*Cv / 4;
	dctValuesRGB[1] *= Cu*Cv / 4;
	dctValuesRGB[2] *= Cu*Cv / 4;
	return dctValuesRGB;
}
//For the idx-th block, calculate f(x,y) 
float* MyImage::OperationIDCT(int idx, int x, int y) {
	float Cu = 1;
	float Cv = 1;
	//DCT values for each channel
	float* idctValuesRGB = new float[3];
	idctValuesRGB[0] = 0;
	idctValuesRGB[1] = 0;
	idctValuesRGB[2] = 0;
	for (int u = 0; u < 8; u++) {
		for (int v = 0; v < 8; v++) {
			//for u,v=0 situation
			if (u == 0) {
				Cu = 1 / sqrt(2);
			}
			else {
				Cu = 1;
			}
			if (v == 0) {
				Cv = 1 / sqrt(2);
			}
			else {
				Cv = 1;
			}
			
			idctValuesRGB[0] += Cu*Cv*reducedDCTs[idx][u][v][0] * cos((2 * x + 1)*u*M_PI / 16)*cos((2 * y + 1)*v*M_PI / 16);
			idctValuesRGB[1] += Cu*Cv*reducedDCTs[idx][u][v][1] * cos((2 * x + 1)*u*M_PI / 16)*cos((2 * y + 1)*v*M_PI / 16);
			idctValuesRGB[2] += Cu*Cv*reducedDCTs[idx][u][v][2] * cos((2 * x + 1)*u*M_PI / 16)*cos((2 * y + 1)*v*M_PI / 16);
		}
	}
	//multiply factor
	idctValuesRGB[0] /= 4;
	idctValuesRGB[1] /= 4;
	idctValuesRGB[2] /= 4;
	return idctValuesRGB;
}
//make sure RGB values in range of [0,255]
int LimitRange(float val) {
	if (val < 0) {
		return 0;
	}
	else if (val>255) {
		return 255;
	}
	else {
		return int(val);
	}
}
//Apply DCT to whole image and zero out coefficients
bool MyImage::MyDCT() {

	//for testing, create a log file
	//std::ofstream log;
	//log.open("log_assigment2.txt",std::ofstream::app);
	/* DCT */
	int numOfBlocks = Width*Height / (8 * 8);
	//store DCT values
	DCTCoeffs = new float***[numOfBlocks];
	//Blocks are arranged like:
	// 0    1    2    3   ...  63
	// 64  65  66  67 ...  127
	// 128 ... 
	for (int i = 0; i < numOfBlocks; i++) {
		DCTCoeffs[i] = new float**[8];
		
		//operation for each value  in a 8 by 8 block
		
		for (int u = 0; u < 8; u++) {
			DCTCoeffs[i][u] = new float*[8];
			for (int v = 0; v < 8; v++) {
				//rgb
				DCTCoeffs[i][u][v] = new float[3];
				DCTCoeffs[i][u][v] = OperationDCT(i, u, v);

			}
		}
		
	}
	
	 //test case for dct idct if working
	//float* testVals=OperationIDCT(0, 0, 0);
	//log << testVals[0] << " " << testVals[1] << " " << testVals[2] << std::endl;
	//log << int(Data[2]) << " " << int(Data[1]) << " " << int(Data[0]) << std::endl;

	return false;
}
bool MyImage::DCTCompression(int numOfCoeff) {
	//std::ofstream log;
	//log.open("log_assigment2.txt");
	int numOfBlocks = Width*Height / (8 * 8);
	//preserve all DCT values and create new zero out result
	reducedDCTs = new float***[numOfBlocks];
	//decode in zig-zag order with given coefficients number
	int m = round(numOfCoeff / numOfBlocks);
	//initialize output

	for (int i = 0; i < numOfBlocks; i++) {
		reducedDCTs[i] = new float**[8];
		for (int x = 0; x < 8; x++) {
			reducedDCTs[i][x] = new float*[8];
			for (int y = 0; y < 8; y++) {
				reducedDCTs[i][x][y] = new float[3];
				for (int ch = 0; ch < 3; ch++) {
					reducedDCTs[i][x][y][ch] = DCTCoeffs[i][x][y][ch];
				}
			}
		}
	}
	
	for (int i = 0; i < numOfBlocks; i++) {
		int k = 0;
		//diagonal j=sum(x, y) is constant
		//first 8 diagonals
		for (int j = 0; j < 8; j++) {
			if (j % 2 == 0) {
				for (int u = 0; u < j+1; u++) {
					for (int ch = 0; ch < 3; ch++) {
						if (k > m - 1) {
							reducedDCTs[i][u][j - u][ch] = 0;
						}
					}
					++k;
				}
			}
			else {
				for (int u = j; u >-1; u--) {
					for (int ch = 0; ch < 3; ch++) {
						if (k > m - 1) {
							reducedDCTs[i][u][j - u][ch] = 0;
						}
					}
					++k;
				}
			}
		}
		for (int j = 8; j < 15; j++) {
			if (j % 2 == 0) {
				for (int u = j - 7; u < 8; u++) {
					for (int ch = 0; ch < 3; ch++) {
						if (k > m - 1) {
							reducedDCTs[i][u][j - u][ch] = 0;
						}
					}
					++k;
				}
			}
			else {
				for (int u = 7; u >j - 8; u--) {
					for (int ch = 0; ch < 3; ch++) {
						if (k > m - 1) {
							reducedDCTs[i][u][j - u][ch] = 0;
						}
					}
					++k;
				}
			}
		}
	}
	return false;
}
//Apply iDCT to whole image and change MyImage Data
bool MyImage::MyIDCT() {
	//std::ofstream log;
	//log.open("log_assigment2.txt", std::ofstream::app);
	int numOfBlocks = Width*Height / (8 * 8);
	//iDCT for each block
	for (int i = 0; i < numOfBlocks; i++) {
		for (int x = 0; x < 8; x++) {
			for (int y = 0; y < 8; y++) {
				int loc = x + (i % 64) * 8 + (y + (i / 64) * 8) * 512;
				float* iDCTVals = OperationIDCT(i, x, y);
				//deal with overflow using LimitRange
				Data[3 * loc] = char(LimitRange(iDCTVals[2]));
				Data[3 * loc + 1] =char(LimitRange(iDCTVals[1]));
				Data[3 * loc + 2] = char(LimitRange(iDCTVals[0]));
				delete iDCTVals;
			}
		}
	}
	return false;
}
//1D DWT 
float* MyImage::OperationDWT(int length , float* DWTVals) {
	float* temps = new float[length];
	for (int i = 0; i < length ; i += 2) {
		temps[i / 2] = (DWTVals[i] + DWTVals[i + 1]) / 2.0;
		temps[(i + length) / 2] = (DWTVals[i] - DWTVals[i + 1]) / 2.0;
	}
	for (int j = 0; j < length; j++) {
		DWTVals[j] = temps[j];
	}
	delete[] temps;
	return DWTVals;


}
//1D iDWT
float* MyImage::OperationIDWT(int length, float* DWTVals) {
	float* temps = new float[length];
	for (int i = 0; i < length ; i += 2) {
		temps[i ] = (DWTVals[i/2] + DWTVals[i/2 + length/2]) ;
		temps[i + 1] = (DWTVals[i/2] - DWTVals[i/2 + length / 2]) ;
	}
	for (int j = 0; j < length; j++) {
		DWTVals[j] = temps[j];
	}
	delete[] temps;
	return DWTVals;

}
//rc=0 means get the idx-th row
float** MyImage::GetArray(bool rc, int idx, float* inputArr) {
	if (rc == false) {
		float** arrayBGR = new float*[3];
		for (int i = 0; i < 3; i++) {
			arrayBGR[i] = new float[Width];
			for (int col = 0; col < Width; col++) {
				arrayBGR[i][col] = inputArr[3 * col+idx*3*Width+i];
			}
		}
		return arrayBGR;
	} 
	else {
		float** arrayBGR = new float*[3];
		for (int i = 0; i < 3; i++) {
			arrayBGR[i] = new float[Height];
			for (int row = 0; row < Height; row++) {
				arrayBGR[i][row] = inputArr[3 * row*Width +3* idx  + i];
			}
		}
		return arrayBGR;
	}
	

}
float* MyImage::Char2RGB() {
	float* rgbs = new float[Width*Height * 3];
	for (int i = 0; i < Width*Height * 3; i++) {
		rgbs[i] = float(unsigned char(Data[i]));
	}
	return rgbs;
}
bool MyImage::MyDWT() {
	float* rgbs = Char2RGB();
	int len = Width;
	//for testing, create a log file
	//std::ofstream log;
	//log.open("log_assigment2.txt", std::of);
	/* DWT */
	//allocate memory for DWT coefficients
	DWTCoeffs = new float[Width*Height * 3];
	//initialization
	for (int i = 0; i < 3 * Width*Height; i++) {
		DWTCoeffs[i] = rgbs[i];
	}
	while (len > 1) {
		//for each row
		for (int row = 0; row < Height; row++) {
			float** arr = GetArray(false, row, DWTCoeffs);
			//for each channel
			for (int ch = 0; ch < 3; ch++) {
				float* DWTs = OperationDWT(len, arr[ch]);
				//assign DWT values
				for (int col = 0; col < Width; col++) {
					DWTCoeffs[3 * col + row * 3 * Width + ch] = DWTs[col];
				}
				delete[] DWTs;
			}
			delete arr;
		}

		//for each column
		for (int col = 0; col < Width; col++) {
			float** arr = GetArray(true, col, DWTCoeffs);
			for (int ch = 0; ch < 3; ch++) {
				float* DWTs = OperationDWT(len, arr[ch]);
				//assign DWT values
				for (int row = 0; row < Height; row++) {
					DWTCoeffs[3 * col + row * 3 * Width + ch] = DWTs[row];
				}
				delete[] DWTs;
			}
			delete arr;
		}
		len /= 2;
	}

	return false;
}
bool MyImage::DWTCompression(int numOfCoeff) {
	//std::ofstream log;
	//log.open("log_assigment2.txt");
	//initialized coefficients
	reducedDWTs = new float[3 * Width*Height];
	//zig-zag order
	int k = 0;//keep track of number of coefficient preserved
			  //diagonal j=sum(x, y) is constant
			  //first 512 diagonals
	for (int j = 0; j < Width; j++) {
		if (j % 2 == 0) {
			for (int u = 0; u < j+1; u++) {
				if (k > numOfCoeff - 1) {
					reducedDWTs[3 * u + (j - u) * 3 * Width] = 0;
					reducedDWTs[3 * u + (j - u) * 3 * Width + 1] = 0;
					reducedDWTs[3 * u + (j - u) * 3 * Width + 2] = 0;
				}
				else {
					reducedDWTs[3 * u + (j - u) * 3 * Width] = DWTCoeffs[3 * u + (j - u) * 3 * Width];
					reducedDWTs[3 * u + (j - u) * 3 * Width + 1] = DWTCoeffs[3 * u + (j - u) * 3 * Width+1];
					reducedDWTs[3 * u + (j - u) * 3 * Width + 2] = DWTCoeffs[3 * u + (j - u) * 3 * Width+2];
				}
				++k;
			}
		}
		else {
			for (int u = j; u >-1; u--) {
				if (k > numOfCoeff - 1) {
					reducedDWTs[3 * u + (j - u) * 3 * Width] = 0;
					reducedDWTs[3 * u + (j - u) * 3 * Width + 1] = 0;
					reducedDWTs[3 * u + (j - u) * 3 * Width + 2] = 0;
				}
				else {
					reducedDWTs[3 * u + (j - u) * 3 * Width] = DWTCoeffs[3 * u + (j - u) * 3 * Width];
					reducedDWTs[3 * u + (j - u) * 3 * Width + 1] = DWTCoeffs[3 * u + (j - u) * 3 * Width + 1];
					reducedDWTs[3 * u + (j - u) * 3 * Width + 2] = DWTCoeffs[3 * u + (j - u) * 3 * Width + 2];
				}
				++k;
			}
		}
	}
	//second half
	for (int j = Width; j < 2 * Width - 1; j++) {
		if (j % 2 == 0) {
			for (int u = j - Width + 1; u < Width; u++) {
				if (k > numOfCoeff - 1) {
					reducedDWTs[3 * u + (j - u) * 3 * Width] = 0;
					reducedDWTs[3 * u + (j - u) * 3 * Width + 1] = 0;
					reducedDWTs[3 * u + (j - u) * 3 * Width + 2] = 0;
				}
				else {
					reducedDWTs[3 * u + (j - u) * 3 * Width] = DWTCoeffs[3 * u + (j - u) * 3 * Width];
					reducedDWTs[3 * u + (j - u) * 3 * Width + 1] = DWTCoeffs[3 * u + (j - u) * 3 * Width + 1];
					reducedDWTs[3 * u + (j - u) * 3 * Width + 2] = DWTCoeffs[3 * u + (j - u) * 3 * Width + 2];
				}
				++k;
			}
		}
		else {
			for (int u = Width - 1; u >j - Width; u--) {
				if (k > numOfCoeff - 1) {
					reducedDWTs[3 * u + (j - u) * 3 * Width] = 0;
					reducedDWTs[3 * u + (j - u) * 3 * Width + 1] = 0;
					reducedDWTs[3 * u + (j - u) * 3 * Width + 2] = 0;
				}
				else {
					reducedDWTs[3 * u + (j - u) * 3 * Width] = DWTCoeffs[3 * u + (j - u) * 3 * Width];
					reducedDWTs[3 * u + (j - u) * 3 * Width + 1] = DWTCoeffs[3 * u + (j - u) * 3 * Width + 1];
					reducedDWTs[3 * u + (j - u) * 3 * Width + 2] = DWTCoeffs[3 * u + (j - u) * 3 * Width + 2];
				}
				++k;
			}
		}
	}

	return false;
}
bool MyImage::MyIDWT()  {
	/* iDWT */
	int len = 2;
	//float* temps = new float[3 * Width*Height];
	while (len < Height + 1) {
		//column first
		for (int col = 0; col < Width; col++) {
			float** arr = GetArray(true, col, reducedDWTs);
			//for each channel
			for (int ch = 0; ch < 3; ch++) {
				float* iDWTVals = OperationIDWT(len, arr[ch]);
				for (int row = 0; row < Height; row++) {
					reducedDWTs[3 * col + row * 3 * Width + ch] = iDWTVals[row];
				}

				delete[] iDWTVals;
			}
			delete[] arr;
		}
		//then for each row
		for (int row = 0; row < Height; row++) {
			float** arr = GetArray(false, row, reducedDWTs);
			for (int ch = 0; ch < 3; ch++) {
				float* iDWTVals = OperationIDWT(len, arr[ch]);
				for (int col = 0; col < Width; col++) {
					reducedDWTs[3 * col + row * 3 * Width + ch] = iDWTVals[col];
				}
				delete[] iDWTVals;
			}
			delete[] arr;
		}
		len *= 2;
	}
	for (int i = 0; i < 3 * Width*Height; i++) {
		Data[i] = char(LimitRange(reducedDWTs[i]));
	}
	//delete[] temps;
	return false;
}
bool MyImage::Release() {
	if (reducedDCTs) {
		for (int i = 0; i < 4096; i++) {
			for (int j = 0; j < 8; j++) {
				for (int k = 0; k < 8; k++) {
					delete[] reducedDCTs[i][j][k];
				}
			}
		}
		reducedDCTs = NULL;
	}
	if (reducedDWTs) {
		delete[] reducedDWTs;
	}
	return true;
}
