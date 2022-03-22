//
//  Header.h
//  BMP
//
//  Created by Phil on 2022/03/13.
//


#include <cstdint>


typedef unsigned char BYTE;

typedef unsigned short WORD;

typedef unsigned int DWORD;

typedef long LONG;



typedef struct tagBITMAPFILEHEADER{

WORD bfType;

DWORD bfSize;

WORD bfReserved1;

WORD bfReserved2;

DWORD bfoffBits;

}BITMAPFILEHEADER;



typedef struct tagBITMAPINFOHEADER{

DWORD biSize;

LONG biWidth;

LONG biHeight;

WORD biplanes;

WORD biBitCount;

DWORD biCompression;

DWORD biSizeImage;

LONG biXPelsPerMeter;

LONG biYPelsPerMeter;

DWORD biClrUsed;

DWORD biClrImportant;

}BITMAPINFOHEADER;



typedef struct tagRGBQUAD{

BYTE rgbBlue;

BYTE rgbGreen;

BYTE rgbRed;

BYTE rgbReserved1;

}RGBQUAD;

