//*****************************************************************************
//
// Main.cpp : Defines the entry point for the application.
// Used to read in a standard RGB image and display it.
// Two images are displayed on the screen.
// Left Pane: Input Image, Right Pane: Modified Image
//
// Author - Parag Havaldar
// Code used by students as a starter code to display and modify images
//
//*****************************************************************************


// Include class files
#include "Image.h"

#define MAX_LOADSTRING 100

//define default image size for assignment 1
#define WIDTH 512
#define LENGTH 512
// Global Variables:
MyImage			inImage, outImage1, outImage2;				// image objects
HINSTANCE		hInst;							// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// The title bar texthe
//Decoding coefficient
int decodeCoeff;
//record iteration of animation
int iter=0;
//record status of processing
int sts = 0;
// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);



// Main entry point for a windows application
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	// Read in the image and its copy
	char ImagePath[_MAX_PATH];
	sscanf(lpCmdLine, "%s %d" , &ImagePath, &decodeCoeff);
	inImage.setWidth(WIDTH);
	inImage.setHeight(LENGTH);
	if ( strstr(ImagePath, ".rgb" )==NULL )
	{ 
		AfxMessageBox( "Image has to be a '.rgb' file\nUsage - Image.exe image.rgb 262144");
		//return FALSE;
	}
	else
	{
		inImage.setImagePath(ImagePath);
		if (!inImage.ReadImage())
		{
			AfxMessageBox("Could not read image\nUsage - Image.exe image.rgb 262144");
			//return FALSE;
		}
		else
			outImage1 = inImage;
			outImage2 = inImage;


	}

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_IMAGE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_IMAGE);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_IMAGE);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = (LPCSTR)IDC_IMAGE;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}


//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, "Assignment2", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
// TO DO: part useful to render video frames, may place your own code here in this function
// You are free to change the following code in any way in order to display the video

	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);
	RECT rt;
	GetClientRect(hWnd, &rt);

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case ID_MODIFY_IMAGE:
					sts = 1;
					InvalidateRect(hWnd, &rt, false);
					SendMessage(hWnd, WM_PAINT, wParam, lParam);
					
					
					if (decodeCoeff == -1) {
						//preserve all the coefficients
						outImage1.MyDCT();
						outImage2.MyDWT();
						int step = 0;
						while(step<64) {
							//ValidateRect(hWnd, &rt);
							//SMessage(hWnd, WM_NOTIFY, wParam, lParam);
							outImage1.DCTCompression((step+1)  * 4096);
							outImage1.MyIDCT();
							outImage2.DWTCompression((step + 1) * 4096);
							outImage2.MyIDWT();
							outImage1.Release();
							outImage2.Release();
							iter = step + 1;
							//step = iter % 64;
							InvalidateRect(hWnd, &rt, false);
							SendMessage(hWnd, WM_PAINT, wParam, lParam);
							//InvalidateRect(hWnd, &rt, false);
							//SendMessage(hWnd, WM_COMMAND, wParam, lParam);
							//program delay 2s 
							//InvalidateRect(hWnd, &rt, false);
							//PostMessage(hWnd, WM_COMMAND, wParam, lParam);
							//Sleep(2000);
							step++;
							
						}
					}
					else {
						outImage1.MyDCT();
						outImage1.DCTCompression(decodeCoeff);
						outImage1.MyIDCT();
						outImage2.MyDWT();
						outImage2.DWTCompression(decodeCoeff);
						outImage2.MyIDWT();
						sts = 2;
						InvalidateRect(hWnd, &rt, false);
						SendMessage(hWnd, WM_PAINT, wParam, lParam);
						
					}
				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			{
				hdc = BeginPaint(hWnd, &ps);
				// TO DO: Add any drawing code here...
				char buf[100];
				char text[1000];
				if (decodeCoeff == -1) {
					strcpy(text, "       iteration(s).   \n ");
					DrawText(hdc, text, strlen(text), &rt, DT_LEFT);
					itoa(iter, buf, 10);
					strcpy(text, buf);
					DrawText(hdc, text, strlen(text), &rt, DT_LEFT);
				}
				else {
					strcpy(text, "              coefficients.   \n ");
					DrawText(hdc, text, strlen(text), &rt, DT_LEFT);
					itoa(decodeCoeff, buf, 10);
					strcpy(text, buf);
					DrawText(hdc, text, strlen(text), &rt, DT_LEFT);
				}
				strcpy(text, "\n\n\n                                      DCT result (Left)                                                                              DWT result (Right)\n");
				DrawText(hdc, text, strlen(text), &rt, DT_LEFT);
				if (sts==1) {
					strcpy(text, "\n processing...");
					DrawText(hdc, text, strlen(text), &rt, DT_BOTTOM);
				}
				if(sts==2) {
					strcpy(text, "\n                  Done.\n");
					DrawText(hdc, text, strlen(text), &rt, DT_BOTTOM);
				}
				BITMAPINFO bmi;
				CBitmap bitmap;
				memset(&bmi,0,sizeof(bmi));
				bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
				bmi.bmiHeader.biWidth = inImage.getWidth();
				bmi.bmiHeader.biHeight = -inImage.getHeight();  // Use negative height.  DIB is top-down.
				bmi.bmiHeader.biPlanes = 1;
				bmi.bmiHeader.biBitCount = 24;
				bmi.bmiHeader.biCompression = BI_RGB;
				bmi.bmiHeader.biSizeImage = inImage.getWidth()*inImage.getHeight();
				
				SetDIBitsToDevice(hdc,
						0, 100, outImage1.getWidth(), outImage1.getHeight(),
						0, 0, 0, outImage1.getHeight(),
						outImage1.getImageData(), &bmi, DIB_RGB_COLORS);
				

				SetDIBitsToDevice(hdc,
								  outImage2.getWidth()+50,100,outImage2.getWidth(),outImage2.getHeight(),
								  0,0,0,outImage2.getHeight(),
								  outImage2.getImageData(),&bmi,DIB_RGB_COLORS);
				
				EndPaint(hWnd, &ps);
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}




// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}


