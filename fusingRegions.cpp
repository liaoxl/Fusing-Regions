/*
 * msPro.cpp
 * Copyright (C) 2013 moondark <liaoxl2012@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "segm/msImageProcessor.h"

using namespace std;
using namespace cv;

int color_dict[]={255,0,0, 0,255,0, 0,0,255, 0,255,255, 128,0,0, 0,128,0,
                  128,128,0, 0,0,128, 128,0,128, 0,128,128, 128,128,128,
                  64,0,0, 192,0,0, 64,128,0, 192,128,0};

int main(int argc, char* argv[])
{
    Mat img=imread("img.png");
    imshow("Origin", img);

    int img_height=img.size().height;
    int img_width=img.size().width;

    uchar* ms_data=new uchar[img_height*img_width*3];
    uchar* ptr_ms_data=ms_data;
    uchar* data=img.data;

    for(int i=0; i<img_height; i++)
    {
        for(int j=0; j<img_width; j++)
        {
            *(ptr_ms_data++) = *(data++);
            *(ptr_ms_data++) = *(data++);
            *(ptr_ms_data++) = *(data++);
        }
    }

    msImageProcessor* msIP=new msImageProcessor();

    msIP->DefineImage(ms_data, COLOR, img_height, img_width);

    msIP->SetSpeedThreshold(0.1);
    msIP->FuseRegions(50, 200);

    int lblCnt=msIP->GetRegionCount();

    RegionList* regionList=msIP->GetBoundaries();
    int* regionIndeces=regionList->GetRegionIndeces(0);
    int num_Regions=regionList->GetNumRegions();
    int boundary_Points_Count=0;

    assert(lblCnt == num_Regions);

    for(int i=0; i<num_Regions; i++)
    {
        boundary_Points_Count += regionList->GetRegionCount(i);
    }

    int* tmp_x=new int[boundary_Points_Count];
    int* tmp_y=new int[boundary_Points_Count];
    for(int i=0; i<boundary_Points_Count; i++)
    {
        tmp_x[i]=regionIndeces[i]%img_width;
        tmp_y[i]=regionIndeces[i]/img_width;
    }

    Mat dst_img=Mat::zeros(img_height, img_width, CV_8UC3);
    for(int i=0; i<boundary_Points_Count; i++)
    {
        dst_img.at<Vec3b>(tmp_y[i], tmp_x[i])[0]=255;
        dst_img.at<Vec3b>(tmp_y[i], tmp_x[i])[1]=255;
        dst_img.at<Vec3b>(tmp_y[i], tmp_x[i])[2]=255;
    }

    int* label=msIP->GetLabels();

    int colorCnt[15] = {0};

    for(int i=0; i<num_Regions; i++)
    {
        for(int j=0; j<15; j++)
        {
            colorCnt[j]=0;
        }
        Point p;
        for(p.y=0; p.y<img_height; p.y++)
        {
            for(p.x=0; p.x<img_width; p.x++)
            {
                if(label[p.y*img_width + p.x] == i)
                {
                    for(int j=0; j<15; j++)
                    {
                        if(img.at<Vec3b>(p)[0] == color_dict[3*j+2] &&
                                img.at<Vec3b>(p)[1] == color_dict[3*j+1] &&
                                img.at<Vec3b>(p)[2] == color_dict[3*j])
                        {
                            colorCnt[j]++;
                        }
                    }
                }
            }
        }
        int max_colorCnt=0;
        for(int j=1; j<15; j++)
        {
            if(colorCnt[j]>colorCnt[max_colorCnt])
            {
                max_colorCnt=j;
            }
        }
        for(p.y=0; p.y<img_height; p.y++)
        {
            for(p.x=0; p.x<img_width; p.x++)
            {
                if(label[p.y*img_width + p.x] == i)
                {
                    img.at<Vec3b>(p)[0] = color_dict[3*max_colorCnt+2];
                    img.at<Vec3b>(p)[1] = color_dict[3*max_colorCnt+1];
                    img.at<Vec3b>(p)[2] = color_dict[3*max_colorCnt];
                }
            }
        }
    }
    imshow("Meanshift", dst_img);
	imwrite("Contour.png", dst_img);
    imshow("FusingRegion", img);
	imwrite("fusedRegions.png", img);
    waitKey(0);

    cout << num_Regions << endl;

    delete [] ms_data;
    delete [] tmp_x;
    delete [] tmp_y;
    return 0;
}
