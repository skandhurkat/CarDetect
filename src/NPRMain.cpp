//NPRmain.cpp
//© Skand Hurkat, 2011

/*     This file is part of CarDetect.
 *
 *     CarDetect is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     CarDetect is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with CarDetect.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <ocr.h>

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        cerr << "Usage of the form \"NPR4 <imagepath>\"\n";
        return -1;
    }

    namedWindow("Original Image", CV_WINDOW_AUTOSIZE);
    namedWindow("ROI", CV_WINDOW_AUTOSIZE);
    Mat for_display;

    for(int iter=1; iter < argc; iter++)
    {
        Mat img = imread(argv[iter]);

        if(!img.data)
        {
            cerr << "Error reading image.\n";
            continue;
        }

		cout << "Reading Image " << argv[iter];

        Mat img_resized;

        char c;

        resize(img, img_resized, Size(320, 240));

        float x_scale = static_cast<float>(img.cols)/static_cast<float>(img_resized.cols);
        float y_scale = static_cast<float>(img.rows)/static_cast<float>(img_resized.rows);

        Mat grey, edges, temp;

        cvtColor(img_resized, grey, CV_BGR2GRAY);

        imshow("Original Image", img_resized);

        Sobel(grey, temp, CV_16S, 1, 0, 3);
        convertScaleAbs(temp, edges, 1);

        morphologyEx(edges, edges, MORPH_CLOSE, Mat(ceil(edges.rows/150)*2+1, ceil(edges.cols/50)*2+1, CV_8U));

		threshold(edges, edges, 175, 255, THRESH_BINARY);

        vector<vector<Point> > contours;
        vector<vector<Point> > contours_of_interest;
        vector<Vec4i> hierarchy;
        vector<RotatedRect> regions_of_interest;
        vector<Mat> images_of_interest;

        Mat contour_image = edges.clone();
        findContours(contour_image, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

        int iterate = 0;

        while(iterate >= 0)
        {
            RotatedRect rect = minAreaRect(Mat(contours[iterate]));
            if(abs(rect.angle)<10 || (abs(rect.angle)<100&&abs(rect.angle)>80))
                        //Rectangle should be horizontal
            {
                bool merges_with_another_region = false;                    //Could be that number plate is split into two pieces
                contours_of_interest.push_back(contours[iterate]);          //Insert into vector for analysis

                if(abs(rect.angle)>45)                                      //To ensure that rectangle is always horizontal, and with angle 0
                {
                    int tempp = rect.size.height;
                    rect.size.height = rect.size.width;
                    rect.size.width = tempp;
                    rect.angle -= 90;
                }
                if(rect.angle > 90)
                    rect.angle -= 180;
                else if(rect.angle < -90)
                    rect.angle += 180;

                regions_of_interest.push_back(rect);
                for(unsigned int i=0; i<regions_of_interest.size()-1; i++)
                {
                 if(abs(regions_of_interest[i].center.y-rect.center.y)<edges.rows/50 && abs(regions_of_interest[i].angle-rect.angle)<5 &&
                    abs(regions_of_interest[i].size.height-rect.size.height)<(regions_of_interest[i].size.height+rect.size.height)/15 &&
                    abs(abs(regions_of_interest[i].center.x-rect.center.x)-abs(regions_of_interest[i].size.width+rect.size.width)/2)<edges.cols/20)
                                                            //Regions lie horizontally within a certain distance of each other
                {
                    while(!contours[iterate].empty())                       //Push all contour points to merge the regions of interest
                    {
                        contours_of_interest[i].push_back(contours[iterate][contours[iterate].size()-1]);
                        contours[iterate].pop_back();
                    }
                    regions_of_interest[i] = minAreaRect(contours_of_interest[i]);      //Re-compute regions of interest

                    if(abs(regions_of_interest[i].angle)>45)                            //To ensure that rectangle is always horizontal, and with angle 0
                    {
                        int tempp = regions_of_interest[i].size.height;
                        regions_of_interest[i].size.height = regions_of_interest[i].size.width;
                        regions_of_interest[i].size.width = tempp;
                        regions_of_interest[i].angle -= 90;
                    }
                    if(regions_of_interest[i].angle > 90)
                        regions_of_interest[i].angle -= 180;
                    else if(regions_of_interest[i].angle < -90)
                        regions_of_interest[i].angle += 180;

                    merges_with_another_region = true;                      //We have successfully merged regions
                }
                else if(abs(regions_of_interest[i].center.x-rect.center.x)<edges.cols/50 && abs(regions_of_interest[i].angle-rect.angle)<5 &&
                    abs(regions_of_interest[i].size.width-rect.size.width)<(regions_of_interest[i].size.width+rect.size.width)/10 &&
                    abs(abs(regions_of_interest[i].center.y-rect.center.y)-abs(regions_of_interest[i].size.height+rect.size.height)/2)<edges.rows/15)
                                                                //Number plate split horizontally due to 2 line number plate
                {
                    while(!contours[iterate].empty())                       //Ditto
                    {
                        contours_of_interest[i].push_back(contours[iterate][contours[iterate].size()-1]);
                        contours[iterate].pop_back();
                    }
                    regions_of_interest[i] = minAreaRect(contours_of_interest[i]);

                    if(abs(regions_of_interest[i].angle)>45)                            //To ensure that rectangle is always horizontal, and with angle 0
                    {
                        int tempp = regions_of_interest[i].size.height;
                        regions_of_interest[i].size.height = regions_of_interest[i].size.width;
                        regions_of_interest[i].size.width = tempp;
                        regions_of_interest[i].angle -= 90;
                    }
                    if(regions_of_interest[i].angle > 90)
                        regions_of_interest[i].angle -= 180;
                    else if(regions_of_interest[i].angle < -90)
                        regions_of_interest[i].angle += 180;

                    merges_with_another_region = true;
                }
            }
                if(merges_with_another_region)                                      //This region has already been accounted for. Remove it from the vector
                {
                    regions_of_interest.pop_back();
                    contours_of_interest.pop_back();
                }
            }
            iterate = hierarchy[iterate][0];                                        //Find next contour at same hierarchical level
        }
        contour_image = img_resized.clone();
        for(unsigned int i=0; i<regions_of_interest.size(); i++)
        {
            if(abs(regions_of_interest[i].size.width*regions_of_interest[i].size.height)>(0.007*edges.rows*edges.cols)
                        && abs(regions_of_interest[i].size.width*regions_of_interest[i].size.height)<(0.055*edges.rows*edges.cols))
                                //Number plate should not be too small or too large.
            {
                ellipse(contour_image, RotatedRect(regions_of_interest[i].center, regions_of_interest[i].size,
                        regions_of_interest[i].angle-90), Scalar(rand()%256, rand()%256, rand()%256),
                        static_cast<int>(ceil(contour_image.rows/240)));
            }
            else
            {
                regions_of_interest.erase(regions_of_interest.begin()+i);
                contours_of_interest.erase(contours_of_interest.begin()+i);
                i--;
            }
        }
        imshow("ROI", contour_image);

        //Code for penalising the regions based on various parameters

        vector<float> penalties(regions_of_interest.size(), 0);

        for(unsigned int i=0; i<regions_of_interest.size(); i++)
        {
            float aspect_ratio = regions_of_interest[i].size.height/regions_of_interest[i].size.width;
            if(aspect_ratio < 4)
                penalties[i] = 16/(aspect_ratio*aspect_ratio);
            else if(aspect_ratio < 9)
                penalties[i] = 1;
            else
                penalties[i] = aspect_ratio - 8;

            Rect something = boundingRect(contours_of_interest[i]);
            Mat current_roi = img_resized.rowRange(something.y,something.y+something.height).colRange(something.x,something.x+something.width);
            Mat hsvFrame;
            vector<Mat> hsvChannels;
            cvtColor(current_roi, hsvFrame, CV_BGR2HSV);
            Scalar avg = mean(hsvFrame);
            penalties[i] *= min(abs((avg[0]<117)?(avg[0]-27):(180+27-avg[0])),avg[1]/5)+1;

            //More penalties to be added based on connected components, colour etc.
        }

        for(unsigned int i=0; i<regions_of_interest.size(); i++)
            for(unsigned int j=i; j<regions_of_interest.size(); j++)
            {
                float temp1; RotatedRect temp2; vector<Point> temp3;
                if(penalties[i]>penalties[j])
                {
                    temp1 = penalties[i];
                    penalties[i] = penalties[j];
                    penalties[j] = temp1;

                    temp2 = regions_of_interest[i];
                    regions_of_interest[i] = regions_of_interest[j];
                    regions_of_interest[j] = temp2;

                    temp3 = contours_of_interest[i];
                    contours_of_interest[i] = contours_of_interest[j];
                    contours_of_interest[j] = temp3;
                }
            }

        for(unsigned int i=0; i<regions_of_interest.size(); i++)
        {
            Rect something = boundingRect(contours_of_interest[i]);

            float extra = 0.02*(something.width+something.height);
            images_of_interest.push_back(img.rowRange(static_cast<int>((something.y-extra)>0?ceil((something.y-extra)*y_scale):0),
                                                    static_cast<int>(ceil((something.y+something.height+extra)*y_scale)<img.rows?
                                                                    ceil((something.y+something.height+extra)*y_scale):(img.rows-1)))
                                            .colRange(static_cast<int>((something.x-extra)>0?ceil((something.x-extra)*x_scale):0),
                                                    static_cast<int>(ceil((something.x+something.width+extra)*x_scale)<img.cols?
                                                                    ceil((something.x+something.width+extra)*x_scale):(img.cols-1)))
                                            .clone());
        }

        vector<Mat> for_ocr;
        vector<vector<string> > num_plate;
        for(unsigned int i=0; i<images_of_interest.size(); i++)             //Show images of interest. Just for initial debugging/logic development.
        {
			Mat hsvFrame;
            vector<Mat> hsvChannels;
            cvtColor(images_of_interest[i], hsvFrame, CV_BGR2HSV);
			split(hsvFrame, hsvChannels);

            Mat threshold_roi;
            threshold(hsvChannels[2], threshold_roi, 100, 255, CV_THRESH_BINARY|THRESH_OTSU);
            //imshow("threshold",threshold_roi);
            //waitKey();

            for_ocr.push_back(threshold_roi);
			//Code for ocr to put in here
            //ocr(hsvChannels[2]);
        }
        //send all roi to ocr and get the possible number plate candidates.
        if(for_ocr.empty())
            cout<<"No regions!"<<endl;
        else
            ocr(for_ocr, num_plate);

        cout << endl << endl;

        while((c=waitKey()) != '\r')            //Press enter to analyze next image
        {
            if(c == static_cast<char>(27))      //Press ESC to quit
                return 0;
        }
    }
    return 0;
}
