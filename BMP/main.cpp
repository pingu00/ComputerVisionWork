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
//#include <Windows.h>
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
    BYTE Low = 0, High = 0;
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
    double Ratio = Gmax / (double)Nt;// 0이랑 1사이의 비율값을 가짐
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

//int GonzalezBinThresh(BYTE * Output, int * Histo, int ImgSize )
//{   int sum= 0 ;
//    int T;
//    int eps = 3; //diffrence of T and T_next under eps ->  exit
//    int diff = eps + 1;//First setting
//
//
//    for (int i=0; i < ImgSize ; i++){
//        sum += Output[i];
//    }
//    T = sum / ImgSize;
//    printf("First Thres : %d\n",T);
//    while (diff > eps){
//        int T_next;
//        int sumlow=0;
//        int sumhigh=0;
//        int countl = 0;
//        int counth = 0;
//        for ( int i=0; i< ImgSize ; i++){
//            if (Output[i]<T){
//                sumlow += Output[i];
//                countl++;
//            }
//            else{
//                sumhigh += Output[i];
//                counth++;
//            }
//        }
//        T_next = (sumlow/countl + sumhigh/counth)/2 ;
//        printf("Next Thres : %d\n",T_next);
//        diff = abs(T - T_next);
//        T = T_next;
//    }
//    return  T;
//}
int GonzalezBinThresh(BYTE * Output, int * Histo, int ImgSize )
{
    int T;
    int eps = 3; //diffrence of T and T_next under eps ->  exit
    int diff = eps + 1;//First setting

    int Low = 0 , High = 0;
    for (int i = 0; i < 256; i++) {
        if (Histo[i] != 0) {
            Low = i;
            break;
        }
    }
    for (int i = 255; i>= 0; i--) {
        if (Histo[i] != 0) {
            High = i;
            break;
        }
    }

    T = Low + High / 2;
    printf("First Thres : %d\n",T);
    while (diff > eps){
        int T_next;
        int sumlow=0;
        int sumhigh=0;
        int countl = 0;
        int counth = 0;
        for ( int i=0; i< 256 ; i++){
            if (i<T){
                sumlow += Histo[i] * i;
                countl += Histo[i];
            }
            else{
                sumhigh += Histo[i]*i;
                counth += Histo[i];
            }
        }
        T_next = (int)((double)sumlow/countl + (double)sumhigh/counth)/2 ;
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
void Prewitt_BOTH_Conv(BYTE* Img,BYTE* temp, BYTE* Out, int W, int H)
{
    for( int i = 1; i < W * H; i++){
        if ( Img[i] < temp[i]){
            Out[i] = temp[i];
        }
        else Out[i] = Img[i];
    }
}
void swap(BYTE* a, BYTE* b)
{
    BYTE temp = *a;
    *a = *b;
    *b = temp;
}

BYTE Median(BYTE* arr, int size)
{
    const int S = size;
    for (int i = 0; i < size - 1; i++) // pivot index
    {
        for (int j = i + 1; j < size; j++) // object index
        {
            if (arr[i] > arr[j])     swap(&arr[i], &arr[j]);
        }
    }
    return arr[S/2];
}

BYTE MaxPooling(BYTE* arr, int size)
{
    const int S = size;
    for (int i = 0; i < size - 1; i++) // pivot index
    {
        for (int j = i + 1; j < size; j++)
        {
            if (arr[i] > arr[j])     swap(&arr[i], &arr[j]);
        }
    }
    return arr[S-1];
}
BYTE MinPooling(BYTE* arr, int size)
{
    
    for (int i = 0; i < size - 1; i++) // pivot index
    {
        for (int j = i + 1; j < size; j++)
        {
            if (arr[i] > arr[j])     swap(&arr[i], &arr[j]);
        }
    }
    return arr[0];
}

int push(short* stackx, short* stacky, int arr_size, short vx, short vy, int* top)
{
    if (*top >= arr_size) return(-1);
    (*top)++;
    stackx[*top] = vx;
    stacky[*top] = vy;
    return(1);
}

int pop(short* stackx, short* stacky, short* vx, short* vy, int* top)
{
    if (*top == 0) return(-1);
    *vx = stackx[*top];
    *vy = stacky[*top];
    (*top)--;
    return(1);
}


// GlassFire 알고리즘을 이용한 라벨링 함수
void m_BlobColoring(BYTE* CutImage, int height, int width) // grassfire algorythm 스택형. 구조는 알고있되 외울 필요는없다.
{
    int i, j, m, n, top, area, Out_Area, index, BlobArea[1000];
    long k;
    short curColor = 0, r, c;
    //    BYTE** CutImage2;
    Out_Area = 1;
    
    // 스택으로 사용할 메모리 할당
    short* stackx = new short[height * width];
    short* stacky = new short[height * width];
    short* coloring = new short[height * width];

    int arr_size = height * width;

    // 라벨링된 픽셀을 저장하기 위해 메모리 할당

    for (k = 0; k < height * width; k++) coloring[k] = 0;  // 메모리 초기화

    for (i = 0; i < height; i++)
    {
        index = i * width;
        for (j = 0; j < width; j++)
        {
            // 이미 방문한 점이거나 픽셀값이 255가 아니라면 처리 안함
            if (coloring[index + j] != 0 || CutImage[index + j] != 255) continue;
            r = i; c = j; top = 0; area = 1;
            curColor++;

            while (1)
            {
            GRASSFIRE:
                for (m = r - 1; m <= r + 1; m++)
                {
                    index = m * width;
                    for (n = c - 1; n <= c + 1; n++)
                    {
                        //관심 픽셀이 영상경계를 벗어나면 처리 안함
                        if (m < 0 || m >= height || n < 0 || n >= width) continue;

                        if ((int)CutImage[index + n] == 255 && coloring[index + n] == 0)
                        {
                            coloring[index + n] = curColor; // 현재 라벨로 마크
                            if (push(stackx, stacky, arr_size, (short)m, (short)n, &top) == -1) continue;
                            r = m; c = n; area++;
                            goto GRASSFIRE;
                        }
                    }
                }
                if (pop(stackx, stacky, &r, &c, &top) == -1) break;
            }
            if (curColor < 1000) BlobArea[curColor] = area;
        }
    }

    float grayGap = 255.0f / (float)curColor;

    // 가장 면적이 넓은 영역을 찾아내기 위함
    for (i = 1; i <= curColor; i++)
    {
        if (BlobArea[i] >= BlobArea[Out_Area]) Out_Area = i;// 가장 넓은 영역에 해당하는 레이블넘버가 들어감
    }
    // CutImage 배열 클리어~ㄱ하얗게
    for (k = 0; k < width * height; k++) CutImage[k] = 255;

    // coloring에 저장된 라벨링 결과중 (Out_Area에 저장된) 영역이 가장 큰 것만 CutImage에 저장
    for (k = 0; k < width * height; k++)
    {
        // 용도에 맞게 3개중에 하나만 선택
        if (coloring[k] == Out_Area) CutImage[k] = 0;  // 가장 큰 것만 저장 (size filtering)
        //if (BlobArea[coloring[k]] > 500) CutImage[k] = 0;  // 특정 면적이상되는 영역만 출력
        //CutImage[k] = (unsigned char)(coloring[k] * grayGap);
    }

    delete[] coloring;
    delete[] stackx;
    delete[] stacky;
}
// 라벨링 후 가장 넓은 영역에 대해서만 뽑아내는 코드 포함

void BinaryImageEdgeDetection(BYTE* Bin, BYTE* Out, int W, int H)
{
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            if (Bin[i * W + j] == 0) // 전경화소라면
            {
                if (!(Bin[(i - 1) * W + j] == 0 && Bin[(i + 1) * W + j] == 0 &&
                    Bin[i * W + j - 1] == 0 && Bin[i * W + j + 1] == 0)) // 4방향 화소 중 하나라도 전경이 아니라면
                    Out[i * W + j] = 255;
            }
        }
    }
}

BYTE DetermThGonzalez(int* H)
{
    BYTE ep = 3;
    BYTE Low, High;
    BYTE Th, NewTh;
    int G1 = 0, G2 = 0, cnt1 = 0, cnt2 = 0, mu1, mu2;

    for (int i = 0; i < 256; i++) {
        if (H[i] != 0) {
            Low = i;
            break;
        }
    }
    for (int i = 255; i >= 0; i--) {
        if (H[i] != 0) {
            High = i;
            break;
        }
    }

    Th = (Low + High) / 2;

    //printf("%d\n", Th);
    while(1)
    {
        for (int i = Th + 1; i <= High; i++) {
            G1 += (H[i] * i);
            cnt1 += H[i];
        }
        for (int i = Low; i <= Th; i++) {
            G2 += (H[i] * i);
            cnt2 += H[i];
        }
        if (cnt1 == 0) cnt1 = 1;
        if (cnt2 == 0) cnt2 = 1;

        mu1 = G1 / cnt1;
        mu2 = G2 / cnt2;

        NewTh = (mu1 + mu2) / 2;

        if (abs(NewTh - Th) < ep)
        {
            Th = NewTh;
            break;
        }
        else
        {
            Th = NewTh;
        }
        G1 = G2 = cnt1 = cnt2 = 0;
        //printf("%d\n", Th);
    }
    return Th;
}

void VerticalFlip(BYTE * Img, int W, int H)
{
    for (int i = 0; i < H / 2; i++) { // y좌표
        for (int j = 0; j < W; j++) { // x좌표
            swap(&Img[i*W + j], &Img[(H-1-i)*W + j]);
        }
    }
}

void HorizontalFlip(BYTE* Img, int W, int H)
{
    for (int i = 0; i < W / 2; i++) { // x좌표
        for (int j = 0; j < H; j++) { // y좌표
            swap(&Img[j * W + i], &Img[j * W + (W-1-i)]);
        }
    }
}

void Translation(BYTE* Image, BYTE* Output, int W, int H, int Tx, int Ty)
{
    // Translation
    Ty *= -1;
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            if ((i + Ty < H && i + Ty >= 0) && (j + Tx < W && j + Tx >= 0))
                Output[(i + Ty) * W + (j + Tx)] = Image[i * W + j];
        }
    }
}

void Scaling(BYTE* Image, BYTE* Output, int W, int H, double SF_X, double SF_Y)
{
    // Scaling
    int tmpX, tmpY;
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            tmpX = (int)(j / SF_X);
            tmpY = (int)(i / SF_Y);
            if (tmpY < H && tmpX < W)
                Output[i * W + j] = Image[tmpY * W + tmpX];
        }
    }

}

//void Rotation(BYTE* Image, BYTE* Output, int W, int H, int Angle)
//{
//    int tmpX, tmpY;
//    double Radian = Angle * 3.141592 / 180.0;
//    for (int i = 0; i < H; i++) {
//        for (int j = 0; j < W; j++) {
//            tmpX = (int)(cos(Radian) * j + sin(Radian) * i);
//            tmpY = (int)(-sin(Radian) * j + cos(Radian) * i);
//            if ((tmpY < H && tmpY >= 0) && (tmpX < W && tmpX >= 0))
//                Output[i * W + j] = Image[tmpY * W + tmpX];
//        }
//    }
//}

void MedianFiltering(BYTE* Image, BYTE* Output, int W, int H, int size)
{
    int Length = size;  // 마스크의 한 변의 길이
    int Margin = Length / 2;
    int WSize = Length * Length;
    BYTE* temp = (BYTE*)malloc(sizeof(BYTE) * WSize);
    int i, j, m, n;
    for (i = Margin; i < H - Margin; i++) {
        for (j = Margin; j < W - Margin; j++) {
            for (m = -Margin; m <= Margin; m++) {
                for (n = -Margin; n <= Margin; n++) {
                    temp[(m + Margin) * Length + (n + Margin)] = Image[(i + m) * W + j + n];
                }
            }
            Output[i * W + j] = Median(temp, WSize);
        }
    }
    free(temp);
}

// Img: 사각형을 그릴 이미지배열, W: 영상 가로사이즈, H: 영상 세로사이즈,
// LU_X: 사각형의 좌측상단 X좌표, LU_Y: 사각형의 좌측상단 Y좌표,
// RD_X: 사각형의 우측하단 X좌표, LU_Y: 사각형의 우측하단 Y좌표.
void DrawRectOutline(BYTE* Img, int W, int H, int LU_X, int LU_Y, int RD_X, int RD_Y)
{
    for (int i = LU_X; i < RD_X; i++)     Img[LU_Y * W + i] = 255;
    for (int i = LU_X; i < RD_X; i++)     Img[RD_Y * W + i] = 255;
    for (int i = LU_Y; i < RD_Y; i++)     Img[i * W + LU_X] = 255;
    for (int i = LU_Y; i < RD_Y; i++)     Img[i * W + RD_X] = 255;
}

// Img: 가로/세로 라인을 그릴 이미지배열, W: 영상 가로사이즈, H: 영상 세로사이즈,
// Cx: 가로/세로 라인이 교차되는 지점의 X좌표
// Cy: 가로/세로 라인이 교차되는 지점의 Y좌표
void DrawCrossLine(BYTE* Img, int W, int H, int Cx, int Cy)
{
    for (int i = 0; i < W - 1; i++) // horizontal line
        Img[Cy * W + i] = 255;
    for (int i = 0; i < H - 1; i++) // vertical line
        Img[i * W + Cx] = 255;
}

void Obtain2DCenter(BYTE* Image, int W, int H, int* Cx, int* Cy)
{
    int SumX = 0, SumY = 0;
    int cnt = 0;
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            if (Image[i * W + j] == 0) // 동공영역이면
            {
                SumX += j;
                SumY += i;
                cnt++;
            }
        }
    }
    if (cnt == 0)     cnt = 1;
    *Cx = SumX / cnt;
    *Cy = SumY / cnt;
    //printf("%d    %d\n", Cx, Cy);
}

void Obtain2DBoundingBox(BYTE* Image, int W, int H, int* LUX, int* LUY, int* RDX, int* RDY)
{
    int flag = 0;
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            if (Image[i * W + j] == 0) {
                *LUY = i;
                flag = 1;
                break;
            }
        }
        if (flag == 1) break;
    }
    flag = 0;
    for (int i = H - 1; i >= 0; i--) {
        for (int j = 0; j < W; j++) {
            if (Image[i * W + j] == 0) {
                *RDY = i;
                flag = 1;
                break;
            }
        }
        if (flag == 1) break;
    }
    flag = 0;

    for (int j = 0; j < W; j++) {
        for (int i = 0; i < H; i++) {
            if (Image[i * W + j] == 0) {
                *LUX = j;
                flag = 1;
                break;
            }
        }
        if (flag == 1) break;
    }
    flag = 0;
    for (int j = W - 1; j >= 0; j--) {
        for (int i = 0; i < H; i++) {
            if (Image[i * W + j] == 0) {
                *RDX = j;
                flag = 1;
                break;
            }
        }
        if (flag == 1) break;
    }
}
void SaveBMPFile(BITMAPFILEHEADER hf, BITMAPINFOHEADER hInfo,
    RGBQUAD* hRGB, BYTE* Output, int W, int H, const char* FileName)
{
    FILE * fp = fopen(FileName, "wb");
    fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
    fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
    fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
    fwrite(Output, sizeof(BYTE), W*H, fp);
    fclose(fp);
}
int main()
{
    BITMAPFILEHEADER hf; // 14
    BITMAPINFOHEADER hInfo; // 40
    RGBQUAD hRGB[256]; // 1024
    FILE* fp;
    fp = fopen("pupil1.bmp", "rb");
    if (fp == NULL) {
        printf("File not found!\n");
        return -1;
    }
    fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
    fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);
    fread(hRGB, sizeof(RGBQUAD), 256, fp);
    int ImgSize = hInfo.biWidth * hInfo.biHeight;
    int H = hInfo.biHeight, W = hInfo.biWidth;
    BYTE * Image = (BYTE *)malloc(ImgSize);
    BYTE * Temp = (BYTE *)malloc(ImgSize);
    BYTE * Output = (BYTE*)malloc(ImgSize);
    fread(Image, sizeof(BYTE), ImgSize, fp);
    fclose(fp);
//
//    int Histo[256] = { 0 };
//    int AHisto[256] = { 0 };


/* Median filtering */
//    int Length = 5;  // 마스크의 한 변의 길이
//
//    int Margin = Length / 2;//필터를 적용시킬수 없는 구간을 마스킹하지 않기위해 마진을 준다
//
//    int WSize = Length * Length;//마스크의 양변의 크기를 곱하여 사이즈를 구한다.
//
//    BYTE* temp = (BYTE*)malloc(sizeof(BYTE) * WSize);//마스크 메모리를 할당한다.
//
//    int W = hInfo.biWidth, H = hInfo.biHeight; // 입력받은 bmp file의 width 와 height를 받아 선언한다.
//
//    int i, j, m, n; // for문을 위한 인자 선언
//
//    for (i = Margin; i < H - Margin; i++) {//마스크의 열 이동
//
//        for (j = Margin; j < W - Margin; j++) {// 마스크의 행이동
//
//            for (m = -Margin; m <= Margin; m++) {// 마스크 내부의 열 이동
//
//                for (n = -Margin; n <= Margin; n++) { // 마스크 내부의 행이동
//
//                    temp[(m + Margin) * Length + (n + Margin)] = Image[(i+m)*W + j+n]; //마스크 현구간에있는 Image의 모든 일차원 인덱스에서의 값을 temp에 입력해준다. 그때 일차원 배열을 이차원으로 바꿔주는 계산식이 포함되어있다. 공식 [Y좌표값 * W( width) + X 좌표] 를 이용하여 이차원에서 원하는 값이 일차원에서는 몇번쨰 인덱스인지 치환해준다.
//
//                }
//
//            }
//
//            Output[i * W + j] = Median(temp, WSize);//순차적으로 입력된 temp 배열에서 중간값을 return해주는 함수인 median을 이용해서 마스크의 중간지점에 마스크의 WSize 중 중간값에 해당하는 값을 oupput에 입력 해준다.
//
//        }
//
//    }
//
//    free(temp);//마스크를위해 할당해줬던 메모리를 해방시켜준다.

//    AverageConv(Image, Output, hInfo.biWidth, hInfo.biHeight);
//    checkConv(Image, Output, hInfo.biWidth, hInfo.biHeight);

    
//    ObtainHistogram(Image, Histo, hInfo.biWidth, hInfo.biHeight);
//    ObtainAHistogram(Histo, AHisto);
//    HistogramEqualization(Image, Temp, AHisto, hInfo.biWidth, hInfo.biHeight);
//    int Thres = GonzalezBinThresh(Image,Histo,ImgSize); // 임계치 T
//    Binarization(Image, Output, hInfo.biWidth, hInfo.biHeight, Thres);
    
//    Prewitt_X_Conv(Image, Output, hInfo.biWidth, hInfo.biHeight);
//    Prewitt_Y_Conv(Image, Output, hInfo.biWidth, hInfo.biHeight);
//    Prewitt_BOTH_Conv(Output, Temp, Output, hInfo.biWidth, hInfo.biHeight);
    //GaussAvrConv(Image, Output, hInfo.biWidth, hInfo.biHeight);
    
//    Sobel_X_Conv(Image, Temp, hInfo.biWidth, hInfo.biHeight);
//    Sobel_Y_Conv(Image, Output, hInfo.biWidth, hInfo.biHeight);
//    for (int i = 0; i < ImgSize; i++) {
//        if (Temp[i] > Output[i])     Output[i] = Temp[i];
//    }
//    Binarization(Image, Output, hInfo.biWidth, hInfo.biHeight, 50);
//    GaussAvrConv(Image, Temp, hInfo.biWidth, hInfo.biHeight);
//    Laplace_Conv_DC(Temp, Output, hInfo.biWidth, hInfo.biHeight);
    
    //HistogramStretching(Image, Output, Histo, hInfo.biWidth, hInfo.biHeight);
//    InverseImage(Output, Output, hInfo.biWidth, hInfo.biHeight);
    //BrightnessAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 70);
    //ContrastAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 0.5);
    
//    m_BlobColoring(Output, hInfo.biWidth, hInfo.biHeight);
    // 경계검출
    Binarization(Image, Temp, W, H, 50);

    InverseImage(Temp, Temp, W, H);

    m_BlobColoring(Temp, H, W);
    for (int i = 0; i < ImgSize; i++)
        Output[i] = Image[i];

    for (int i = 0; i < H; i++) {

        for (int j = 0; j < W; j++) {

            if (Temp[i * W + j] == 0) {

                if (!(Temp[(i - 1) * W + j] == 0 &&

                    Temp[(i + 1) * W + j] == 0 &&

                    Temp[i * W + (j - 1)] == 0 &&

                    Temp[i * W + (j + 1)] == 0))

                    Output[i * W + j] = 255;

            }

        }

    }
    
    
SaveBMPFile(hf, hInfo, hRGB, Output, hInfo.biWidth, hInfo.biHeight, "binaryeyes.bmp");
}
