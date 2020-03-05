//
//  main.cpp
//  OpenCV
//
//  Created by Yudhiesh Ravindranath on 03/03/2020.
//  Copyright © 2020 Yudhiesh Ravindranath. All rights reserved.
//

//#include "pch.h"

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/core.hpp"


using namespace cv;
using namespace std;

Mat GreyImg(Mat input) {
    Mat greyImg = Mat::zeros(input.size(), CV_8UC1);
    for (int i = 0; i < input.rows; i++)
    {
        for (int j = 0; j < input.cols * 3; j = j + 3)
        {
            greyImg.at<uchar>(i, j / 3) = ((input.at<uchar>(i, j)) + (input.at<uchar>(i, j + 1)) + (input.at<uchar>(i, j + 2))) / 3;
        }
    }
    return greyImg;
}

int checkPixelValue(int value) {
    int finalValue;
    if (value > 255) {
        finalValue = 255;
    }
    else if (value < 0) {
        finalValue = 0;
    }
    else finalValue = value;

    return finalValue;
}

Mat Convolution(int *mask, int maskLength, bool AVG, Mat input) {

    Mat BlurImg = Mat::zeros(input.size(), CV_8UC1);
    int winsize = sqrt((double)maskLength);
    int winValue = (winsize - 1) / 2;
    for (int i = winValue; i < (input.rows - winValue); i++)
    {
        for (int j = winValue; j < (input.cols - winValue); j++)
        {
            int sum = 0;
            int a = 0;
            int totalAvgValue = 0;
            int pixelValue = 0;
            for (int ii = (-winValue); ii <= (+winValue); ii++)
            {
                for (int jj = (-winValue); jj <= (+winValue); jj++)
                {
                    totalAvgValue += mask[a];
                    pixelValue = input.at<uchar>(i + ii, j + jj) * mask[a];
                    sum += checkPixelValue(pixelValue);
                    //cout << pixelValue << endl;
                    //cout << sum << endl;
                    a++;
                }
                if (AVG == true)
                {
                    BlurImg.at<uchar>(i, j) = sum / totalAvgValue;
                }
                else if(AVG == false)
                {
                    BlurImg.at<uchar>(i, j) = sum;
                }
            }
        }
    }
    return BlurImg;
}

void takeDFT(Mat& source, Mat& destination){
    //DFT produces real and imaginary objects
    //Mat object for holding both informations
    Mat imgComplex[2] = {source, Mat::zeros(source.size(),CV_32F)};
    //Merge the two
    Mat dftReady;
    Mat dftOriginal;
    merge(imgComplex, 2, dftReady);
    dft(dftReady, dftOriginal, DFT_COMPLEX_OUTPUT);
    //Complex output
    destination = dftOriginal;
}



void angleDTF(Mat& source){
    int middleX = source.cols/2;
    int middleY = source.rows/2;
    
    Mat q1(source, Rect(0,0,middleX, middleY));
    Mat q2(source, Rect(middleX, 0, middleX, middleY));
    Mat q3(source, Rect(0,middleY,middleX, middleY));
    Mat q4(source, Rect(middleX,middleY,middleX, middleY));
    
    Mat temp;
    q1.copyTo(temp);
    q4.copyTo(q1);
    temp.copyTo(q4);
    
    q2.copyTo(temp);
    q3.copyTo(q2);
    temp.copyTo(q3);
    
    
}

void showDFT(Mat& source){
    
    //Set up array of size 2
    Mat splitArray[2] = { Mat::zeros(source.size(),CV_32F), Mat::zeros(source.size(), CV_32F)};
    split(source, splitArray);
    //DFT has 2 channels
    //Splitting it into 2 arrays
    
    //Take magnitude
    Mat dftMagnitude;
    
    magnitude(splitArray[0], splitArray[1],dftMagnitude);
    
    //add 1 to every value in order to use LOG
    dftMagnitude += Scalar::all(1);
    
    log(dftMagnitude, dftMagnitude);
    
    //normalize function
    //min -0 , max -1
    //from min to max
    normalize(dftMagnitude, dftMagnitude, 0, 1 , NORM_MINMAX);
    angleDTF(dftMagnitude);
    imshow("Magnitude DFT", dftMagnitude);
  //  imshow("Angle DFT", dftMagnitude);
    waitKey();
}

 
int main()
{

    Mat img = imread("/Users/yudhiesh/Desktop/scenery.jpg");
    //A for DFT
    Mat greyImg = GreyImg(img);
    Mat imgDFT = GreyImg(img);
    
    int maskValue[] = {0,1,0,1,-4,1,0,0,1,0};
    //int maskValue[] = { 1,2,1,2,4,2,1,2,1 };
   // int maskValue[] = { 1,1,1,1,1,1,1,1,1 };
    int maskLength = sizeof(maskValue) / sizeof(maskValue[0]);
    //Mat processImage = Convolution(maskValue, maskLength, true, greyImg);
    Mat processImage = Convolution(maskValue,maskLength, false, greyImg);
    
    Mat originalFloat;
    imgDFT.convertTo(originalFloat, CV_32FC1, 1.0 / 255.0);
    Mat dftMag;
    takeDFT(originalFloat, dftMag);
    showDFT(dftMag);
    
    imshow("Grey IU", greyImg);
    imshow("Blur IU", processImage);
    
    waitKey();
    //return 0;
}

//show me the angle/phase and magnitude  using fourier transform
//1 img send through discrete fourier transform and show phase/angle and magnitude
//using dft function to

