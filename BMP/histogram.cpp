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
    
    
    double diff = 1;
    for (int i=0; i < ImgSize ; i++){
        sum += Output[i];
    }
    T = sum / ImgSize;
    
    while (diff >= 1){
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
        diff = abs(T - T_next);
        T = T_next;
    }
    return  T;
}

int main()
{
    BITMAPFILEHEADER hf; // 14πŸ¿Ã∆Æ
    BITMAPINFOHEADER hInfo; // 40πŸ¿Ã∆Æ
    RGBQUAD hRGB[256]; // 1024πŸ¿Ã∆Æ
    FILE* fp;
    fp = fopen("coin.bmp", "rb");
    if (fp == NULL) {
        printf("File not found!\n");
        return -1;
    }
    fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
    fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);
    fread(hRGB, sizeof(RGBQUAD), 256, fp);
    int ImgSize = hInfo.biWidth * hInfo.biHeight;
    BYTE * Image = (BYTE *)malloc(ImgSize);
    BYTE * Output = (BYTE*)malloc(ImgSize);
    fread(Image, sizeof(BYTE), ImgSize, fp);
    fclose(fp);

    int Histo[256] = { 0 };
    int AHisto[256] = { 0 };

    ObtainHistogram(Image, Histo, hInfo.biWidth, hInfo.biHeight);
    ObtainAHistogram(Histo, AHisto);
    HistogramEqualization(Image, Output, AHisto, hInfo.biWidth, hInfo.biHeight);
    int Thres = GonzalezBinThresh(Output,Histo,ImgSize); // 임계치 T
    Binarization(Image, Output, hInfo.biWidth, hInfo.biHeight, Thres);
    


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
