//
//  main.cpp
//  BMP
//
//  Created by Phil on 2022/03/13.
//
#pragma pack(2)
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "Header.hpp"

int main()
{
    BITMAPFILEHEADER hf; // BMP 파일헤더 14Bytes
    BITMAPINFOHEADER hInfo; // BMP 인포헤더 40Bytes
    RGBQUAD hRGB[256]; // 팔레트 (256 * 4Bytes)
    FILE *fp;
    fp = fopen("LENNA.bmp", "rb");
    if (fp == NULL) {fprintf(stderr,"ERROR");
        exit(1);}
    fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
    fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);
    fread(hRGB, sizeof(RGBQUAD), 256, fp);
    int ImgSize = hInfo.biWidth * hInfo.biHeight;
    BYTE * Image = (BYTE *)malloc(ImgSize);
    // 정적할당으로표현하자면 BYTE Image[ImgSize];
    BYTE * Output1 = (BYTE *)malloc(ImgSize);
    BYTE * Output2 = (BYTE *)malloc(ImgSize);
    BYTE * Output3 = (BYTE *)malloc(ImgSize);
    fread(Image, sizeof(BYTE), ImgSize, fp);
    fclose(fp);
    /* 영상처리 */

    //1 그대로 2 밝게 3 반전
    for (int i = 0; i < ImgSize; i++) {
        Output1[i] = Image[i];
        Output2[i] = Image[i] + 50;
        Output3[i] = 255 - Image[i];
    }
        


    fp = fopen("output1.bmp", "wb");
    fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
    fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
    fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
    fwrite(Output1, sizeof(BYTE), ImgSize, fp);
    fclose(fp);

    fp = fopen("output2.bmp", "wb");
    fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
    fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
    fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
    fwrite(Output2, sizeof(BYTE), ImgSize, fp);
    fclose(fp);

    fp = fopen("output3.bmp", "wb");
    fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
    fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
    fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
    fwrite(Output3, sizeof(BYTE), ImgSize, fp);
    fclose(fp);
    free(Image);
    free(Output1);
    free(Output2);
    free(Output3);
}
