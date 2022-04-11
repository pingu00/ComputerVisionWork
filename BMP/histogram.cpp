//
//  histogram.cpp
//  BMP
//
//  Created by Phil on 2022/03/27.
//
#pragma pack(2)
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#pragma warning(disable:4996)
#include "Header.hpp"

void InverseImage(BYTE* Img, BYTE *Out, int W, int H)
{
    int ImgSize = W * H;
    for (int i = 0; i < ImgSize; i++)
    {
        Out[i] = 255 - Img[i];
    }
}
void BrightnessAdj(BYTE* Img, BYTE* Out, int W, int H, int Val)
{
    int ImgSize = W * H;
    for (int i = 0; i < ImgSize; i++)
    {
        if (Img[i] + Val > 255)
        {
            Out[i] = 255;
        }
        else if (Img[i] + Val < 0)
        {
            Out[i] = 0;
        }
        else     Out[i] =Img[i] + Val;
    }
}
void ContrastAdj(BYTE* Img, BYTE* Out, int W, int H, double Val)
{
    int ImgSize = W * H;
    for (int i = 0; i < ImgSize; i++)
    {
        if (Img[i] * Val > 255.0)
        {
            Out[i] = 255;
        }
        else     Out[i] = (BYTE)(Img[i] * Val);
    }
}

void ObtainHistogram(BYTE* Img, int* Histo, int W, int H)
{
    int ImgSize = W * H;
    for (int i = 0; i < ImgSize; i++) {
        Histo[Img[i]]++;
    }
}

void ObtainAHistogram(int* Histo, int* AHisto) //누적합
{
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j <= i; j++) {
            AHisto[i] += Histo[j];
        }
    }
}

void HistogramStretching(BYTE * Img, BYTE * Out, int * Histo, int W, int H)
{
    int ImgSize = W * H;
    BYTE Low = '\0', High = '\0';
    for (int i = 0; i < 256; i++) {// 가장작은값의 밝기값 low
        if (Histo[i] != 0) {
            Low = i;
            break;
        }
    }
    for (int i = 255; i >= 0; i--) {//가장 큰값의 밝기
        if (Histo[i] != 0) {
            High = i;
            break;
        }
    }
    for (int i = 0; i < ImgSize; i++) {
        Out[i] = (BYTE)((Img[i] - Low) / (double)(High - Low) * 255.0); //백분율 같은 느낌 255%만점으로 바꾸는 과정이라 생각하셈
    }
}
void HistogramEqualization(BYTE* Img, BYTE* Out, int* AHisto, int W, int H)
{
    int ImgSize = W * H;
    int Nt = W * H, Gmax = 255;
    double Ratio = Gmax / (double)Nt;
    BYTE NormSum[256];
    for (int i = 0; i < 256; i++) {
        NormSum[i] = (BYTE)(Ratio * AHisto[i]);// 누적합을 정규화 시켜주는 과정
    }
    for (int i = 0; i < ImgSize; i++)
    {
        Out[i] = NormSum[Img[i]];
    }
}

void Binarization(BYTE * Img, BYTE * Out, int W, int H, BYTE Threshold)
{
    int ImgSize = W * H;
    for (int i = 0; i < ImgSize; i++) {
        if (Img[i] < Threshold) Out[i] = 0;
        else Out[i] = 255;
    }
}

int GonzalezBinThresh(BYTE * Output, int * Histo, int ImgSize )
{   int sum= 0 ;
    int T;
    int eps = 3; //diffrence of T and T_next under eps ->  exit
    int diff = eps + 1;//First setting
    
    
    for (int i=0; i < ImgSize ; i++){
        sum += Output[i];
    }
    T = sum / ImgSize;
    printf("First Thres : %d\n",T);
    while (diff > eps){
        int T_next;
        int sumlow=0;
        int sumhigh=0;
        int countl = 0;
        int counth = 0;
        for ( int i=0; i< ImgSize ; i++){
            if (Output[i]<T){
                sumlow += Output[i];
                countl++;
            }
            else{
                sumhigh += Output[i];
                counth++;
            }
        }
        T_next = (sumlow/countl + sumhigh/counth)/2 ;
        printf("Next Thres : %d\n",T_next);
        diff = abs(T - T_next);
        T = T_next;
    }
    return  T;
}
void AverageConv(BYTE* Img, BYTE* Out, int W, int H) // 박스평활화
{
    double Kernel[3][3] = {0.11111, 0.11111, 0.11111,
                                        0.11111, 0.11111, 0.11111,
                                        0.11111, 0.11111, 0.11111    };
    double SumProduct = 0.0;
    for (int i = 1; i < H-1; i++) { // Y좌표 (행)
        for (int j = 1; j < W-1; j++) { // X좌표 (열)
            for (int m = -1; m <= 1; m++) { // Kernel 행
                for (int n = -1; n <= 1; n++) { // Kernel 열
                    SumProduct += Img[(i+m)*W + (j+n)] * Kernel[m+1][n+1];
                }
            }
            Out[i * W + j] = (BYTE)SumProduct;
            SumProduct = 0.0;
        }
    }
}

void GaussAvrConv(BYTE* Img, BYTE* Out, int W, int H) // 가우시안평활화
{
    double Kernel[3][3] = {0.0625, 0.125, 0.0625,
                                        0.125, 0.25, 0.125,
                                        0.0625, 0.125, 0.0625 };
    double SumProduct = 0.0;
    for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
        for (int j = 1; j < W - 1; j++) { // X좌표 (열)
            for (int m = -1; m <= 1; m++) { // Kernel 행
                for (int n = -1; n <= 1; n++) { // Kernel 열
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
                }
            }
            Out[i * W + j] = (BYTE)SumProduct;
            SumProduct = 0.0;
        }
    }
}

void Prewitt_X_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt 마스크 X
{
    double Kernel[3][3] = { -1.0, 0.0, 1.0,
                                        -1.0, 0.0, 1.0,
                                        -1.0, 0.0, 1.0 };
    double SumProduct = 0.0;
    for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
        for (int j = 1; j < W - 1; j++) { // X좌표 (열)
            for (int m = -1; m <= 1; m++) { // Kernel 행
                for (int n = -1; n <= 1; n++) { // Kernel 열
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
                }
            }
            // 0 ~ 765  =====> 0 ~ 255
            Out[i * W + j] = abs((long)SumProduct) / 3;
            SumProduct = 0.0;
        }
    }
}

void Prewitt_Y_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt 마스크 X
{
    double Kernel[3][3] = { -1.0, -1.0, -1.0,
                                        0.0, 0.0, 0.0,
                                        1.0, 1.0, 1.0 };
    double SumProduct = 0.0;
    for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
        for (int j = 1; j < W - 1; j++) { // X좌표 (열)
            for (int m = -1; m <= 1; m++) { // Kernel 행
                for (int n = -1; n <= 1; n++) { // Kernel 열
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
                }
            }
            // 0 ~ 765  =====> 0 ~ 255
            Out[i * W + j] = abs((long)SumProduct) / 3;
            SumProduct = 0.0;
        }
    }
}

void Sobel_X_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt 마스크 X
{
    double Kernel[3][3] = { -1.0, 0.0, 1.0,
                                        -2.0, 0.0, 2.0,
                                        -1.0, 0.0, 1.0 };
    double SumProduct = 0.0;
    for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
        for (int j = 1; j < W - 1; j++) { // X좌표 (열)
            for (int m = -1; m <= 1; m++) { // Kernel 행
                for (int n = -1; n <= 1; n++) { // Kernel 열
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
                }
            }
            // 0 ~ 1020  =====> 0 ~ 255
            Out[i * W + j] = abs((long)SumProduct) / 4;
            SumProduct = 0.0;
        }
    }
}

void Sobel_Y_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt 마스크 X
{
    double Kernel[3][3] = { -1.0, -2.0, -1.0,
                                        0.0, 0.0, 0.0,
                                        1.0, 2.0, 1.0 };
    double SumProduct = 0.0;
    for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
        for (int j = 1; j < W - 1; j++) { // X좌표 (열)
            for (int m = -1; m <= 1; m++) { // Kernel 행
                for (int n = -1; n <= 1; n++) { // Kernel 열
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
                }
            }
            // 0 ~ 765  =====> 0 ~ 255
            Out[i * W + j] = abs((long)SumProduct) / 4;
            SumProduct = 0.0;
        }
    }
}

void Laplace_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt 마스크 X
{
    double Kernel[3][3] = { -1.0, -1.0, -1.0,
                                        -1.0, 8.0, -1.0,
                                        -1.0, -1.0, -1.0 };
    double SumProduct = 0.0;
    for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
        for (int j = 1; j < W - 1; j++) { // X좌표 (열)
            for (int m = -1; m <= 1; m++) { // Kernel 행
                for (int n = -1; n <= 1; n++) { // Kernel 열
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
                }
            }
            // 0 ~ 2040  =====> 0 ~ 255
            Out[i * W + j] = abs((long)SumProduct) / 8;
            SumProduct = 0.0;
        }
    }
}

void Laplace_Conv_DC(BYTE* Img, BYTE* Out, int W, int H) // Prewitt 마스크 X
{
    double Kernel[3][3] = { -1.0, -1.0, -1.0,
                                        -1.0, 9.0, -1.0,
                                        -1.0, -1.0, -1.0 };
    double SumProduct = 0.0;
    for (int i = 1; i < H - 1; i++) { // Y좌표 (행)
        for (int j = 1; j < W - 1; j++) { // X좌표 (열)
            for (int m = -1; m <= 1; m++) { // Kernel 행
                for (int n = -1; n <= 1; n++) { // Kernel 열
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
                }
            }
            //Out[i * W + j] = abs((long)SumProduct) / 8;
            if (SumProduct > 255.0) Out[i * W + j] = 255;
            else if (SumProduct < 0.0) Out[i * W + j] = 0;
            else Out[i * W + j] = (BYTE)SumProduct;
            SumProduct = 0.0;
        }
    }
}
void Prewitt_BOTH_Conv(BYTE* Img,BYTE* temp, BYTE* Out, int W, int H){
    for( int i = 1; i < W * H; i++){
        if ( Img[i] < temp[i]){
            Out[i] = temp[i];lenna_impulse.bmp
        }
        else Out[i] = Img[i];
    }
}


int main()
{
    BITMAPFILEHEADER hf; // 14
    BITMAPINFOHEADER hInfo; // 40
    RGBQUAD hRGB[256]; // 1024
    FILE* fp;
    fp = fopen("LENNA.bmp", "rb");
    if (fp == NULL) {
        printf("File not found!\n");
        return -1;
    }
    fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
    fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);
    fread(hRGB, sizeof(RGBQUAD), 256, fp);
    int ImgSize = hInfo.biWidth * hInfo.biHeight;
    BYTE * Image = (BYTE *)malloc(ImgSize);
    BYTE * Temp = (BYTE *)malloc(ImgSize);
    BYTE * Output = (BYTE*)malloc(ImgSize);
    fread(Image, sizeof(BYTE), ImgSize, fp);
    fclose(fp);

//    int Histo[256] = { 0 };
//    int AHisto[256] = { 0 };

//    ObtainHistogram(Image, Histo, hInfo.biWidth, hInfo.biHeight);
//    ObtainAHistogram(Histo, AHisto);
//    HistogramEqualization(Image, Output, AHisto, hInfo.biWidth, hInfo.biHeight);
//    int Thres = GonzalezBinThresh(Output,Histo,ImgSize); // 임계치 T
//    Binarization(Image, Output, hInfo.biWidth, hInfo.biHeight, Thres);
    
    Prewitt_X_Conv(Image, Temp, hInfo.biWidth, hInfo.biHeight);
    Prewitt_Y_Conv(Image, Output, hInfo.biWidth, hInfo.biHeight);
    Prewitt_BOTH_Conv(Output, Temp, Output, hInfo.biWidth, hInfo.biHeight);
    //GaussAvrConv(Image, Output, hInfo.biWidth, hInfo.biHeight);
    
    /*Sobel_X_Conv(Image, Temp, hInfo.biWidth, hInfo.biHeight);
    Sobel_Y_Conv(Image, Output, hInfo.biWidth, hInfo.biHeight);
    for (int i = 0; i < ImgSize; i++) {
        if (Temp[i] > Output[i])     Output[i] = Temp[i];
    }
    Binarization(Output, Output, hInfo.biWidth, hInfo.biHeight, 40);*/
//    GaussAvrConv(Image, Temp, hInfo.biWidth, hInfo.biHeight);
//    Laplace_Conv_DC(Temp, Output, hInfo.biWidth, hInfo.biHeight);
    
    //HistogramStretching(Image, Output, Histo, hInfo.biWidth, hInfo.biHeight);
    //InverseImage(Image, Output, hInfo.biWidth, hInfo.biHeight);
    //BrightnessAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 70);
    //ContrastAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 0.5);
    
    fp = fopen("output.bmp", "wb");
    fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
    fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
    fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
    fwrite(Output, sizeof(BYTE), ImgSize, fp);
    fclose(fp);
    free(Image);
    free(Output);
    return 0;
}
