// ocr.h

//  This file is part of CarDetect.
//
//  CarDetect is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  CarDetect is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with CarDetect.  If not, see <http://www.gnu.org/licenses/>.


/**************CODE FOR OCR**************/
/*Project: NPR				            */
/****************************************/

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "ocr_spell_check.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace cv;
using namespace std;

//a comment: to reduce time, remove unnecessary windows and couts

void drawInnerContours(Mat& image, const vector<vector<Point> >& contours, vector<int> second_lvl,
        const vector<Vec4i>& hierarchy, Point offset=Point())
{
    for(int i=0; i<second_lvl.size(); i++)
    {
        drawContours(image, contours, second_lvl[i], 0, CV_FILLED, 8, hierarchy, 0, offset);
        //cout<<"lvl"<<second_lvl[i]<<endl;
    }
}

void ocr(vector<Mat> regions, vector<vector<string> > num_plate)
{
    //namedWindow("Original Image", CV_WINDOW_AUTOSIZE);

    //go through roi
    for(int iter=0; iter < regions.size(); iter++)
    {
        Mat org = regions[iter];

        if(!org.data)
        {
            cerr << "Error reading roi.\n";
            continue;
        }

        cout<<"Region number "<<iter<<endl;

        //reject region if aspect ratio is bad
        if((float)org.cols/(float)org.rows<1.5) continue;

        //resize to larger size
        Mat img;
        int ratio=1;
        if(org.cols<500) ratio=500/org.cols;
        resize(org, img, Size(), ratio, ratio, INTER_LINEAR);
        imshow("Resized",img);

        //declarations for contours
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;

        //find contours after invert the image
        threshold(img, img, 125, 255, THRESH_BINARY_INV);
        Mat contour_image = img.clone();
        //imshow("Threshold",org);
        findContours(contour_image, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
        cout << "Contours detected in region: " << contours.size() << endl;
        //imshow("Contours", contour_image);

        //variables for rects of characters
        //Mat seg = Mat::zeros(img.rows, img.cols, CV_8U);
        int i=0, j=0;
        Rect curr_seg;
        int avg_hgt = 0, avg_bdt = 0;//if you want to find avg bdt and hgt of chars (but i think code is robust enough not to require this)

        //variables for separating character rects
        Mat temp, col_sum, temp1, temp2;
        int col_avg=0, col_min = 4000, curr_min;
        vector<int> possible_div;
        int l,r,count=1;

        //declarations for number plates
        char alpha,beta;
        ifstream file("temp.txt");
        vector<string> number_plate;
        vector<int> char_x;

        //find second level hierarchy: (required for drawing inner contours)
        vector<int> second_level;
        for(i=0; i>=0; i=hierarchy[i][0])
            for(j=i+1; j<hierarchy[i][0]; j++)
            {
                curr_seg = boundingRect(contours[j]);
                if(curr_seg.height > 2 && curr_seg.width > 2)
                    second_level.push_back(j);
            }

        //start char detection here:
        for(i=0; i>=0; i=hierarchy[i][0])
        {
            curr_seg = boundingRect(contours[i]);
            //note that the number 1 will have a ht/width ratio of ~3 hence upperbound of 4
            //also if two chars merge, hgt/width ~0.5

            //create Mat with current contour drawn:
            temp = Mat::zeros(curr_seg.height,curr_seg.width,CV_8U);
            drawContours(temp, contours, i, 255, CV_FILLED, 8, hierarchy, 0, Point(-curr_seg.x, -curr_seg.y));
            drawInnerContours(temp, contours, second_level, hierarchy, Point(-curr_seg.x, -curr_seg.y));

            //minimum size requirement
            if( curr_seg.height > 30 && curr_seg.width > 10 )
            {
                //now check if it is sufficiently filled first:
                Scalar s = mean(temp);
                if(s[0] < 20) continue;
                //skip if very few pixels are white

                //check for aspect ratio: still a bit shaky
                if( float(curr_seg.height)/curr_seg.width > 1 && float(curr_seg.height)/curr_seg.width < 4 )
                {
                    //imshow("temp",temp);
                    //gocr requires inversion
                    bitwise_not(temp, temp);
                    imwrite("temp.pgm",temp);
                    system("gocr ./temp.pgm -o temp.txt");
                    //will have to fstream :(
                    //remember to bring back ptr to start!
                    file.seekg(0,ios::beg);
                    file.read(&alpha,1);
                    if(alpha=='\n') alpha='_';
                    //insertion sort
                    if(char_x.empty())
                    {
                        number_plate.push_back("");
                        char_x.push_back(curr_seg.x);
                        number_plate[0].push_back(alpha);
                    }
                    else
                    {
                        int x = 0;
                        while(curr_seg.x > *(char_x.begin()+x))
                        {
                            if(x == (char_x.size())) break;
                            x++;
                        }
                        char_x.insert(char_x.begin()+x, curr_seg.x);
                        for(int a=0; a<number_plate.size(); a++)
                            number_plate[a].insert(number_plate[a].begin()+x, alpha);
                    }
                    cout<<alpha<<endl;
                }
                //if chars get merged, they will have following aspect ratio:
                else if( float(curr_seg.height)/curr_seg.width > 0.4 && float(curr_seg.height)/curr_seg.width < 1 )
                {
                    //write code to separate joined chars here:
                    reduce(temp, col_sum, 0, CV_REDUCE_SUM, CV_32S);
                    col_min = 4000;
                    possible_div.push_back(0);
                    //consider minima only in middle region. 0.2 can be changed suitably, but it is sufficient
                    for(int x=(0.2*col_sum.cols); x<(0.8*col_sum.cols); x++)
                    {
                        col_min = min(col_min, col_sum.at<int>(0,x));
                        if(col_min==col_sum.at<int>(0,x)) possible_div[0] = x;
                    }
                    curr_min = col_min;
                    //need to change the values based on absolute minimum rather than hard coding
                    //find possible divisions into 2 chars
                    while(curr_min < 2000)
                    {
                        for(l=(possible_div[count-1]-5); l>0; l--)
                            if(col_sum.at<int>(0,l) > (2*curr_min))
                                break;
                        for(r=(possible_div[count-1]+5); r<col_sum.cols; r++)
                            if(col_sum.at<int>(0,r) > (2*curr_min))
                                break;
                        //define (l,r) as interval where you cannot find another min by assigning high value
                        for(int x=l; x<=r ; x++) col_sum.at<int>(0,x) = 5000;
                        curr_min = 4000;
                        possible_div.push_back(0);
                        for(int x=0.2*col_sum.cols; x<0.8*col_sum.cols; x++)
                        {
                            curr_min = min(curr_min, col_sum.at<int>(0,x));
                            if(curr_min==col_sum.at<int>(0,x)) possible_div[count] = x;
                        }
                        if(curr_min < 2000)
                            count++;
                        else possible_div.pop_back();
                    }

                    //imshow("temp", temp);
                    bitwise_not(temp, temp);
                    //find chars after separation:
                    //same code as above but this will be for 2 characters
                    int x;
                    int np_size = number_plate.size();
                    for(int a=0; a<possible_div.size(); a++)
                    {
                        temp1 = temp.colRange(0,possible_div[a]);
                        temp2 = temp.colRange(possible_div[a],temp.cols);
                        imwrite("temp1.pgm",temp1);
                        imwrite("temp2.pgm",temp2);
                        system("gocr ./temp1.pgm -o temp.txt");
                        file.seekg(0,ios::beg);
                        file.read(&alpha, 1);
                        system("gocr ./temp2.pgm -o temp.txt");
                        file.seekg(0,ios::beg);
                        file.read(&beta, 1);

                        //some small checks:
                        if(alpha=='\n' && beta=='\n')
                        {
                            if(possible_div.size()==1)
                            {
                                alpha='_';beta='_';
                            }
                            else continue;
                        }
                        if(alpha=='\n') alpha='_';
                        if(beta=='\n') beta='_';
                        if(char_x.empty()) //same as number_plate being empty
                        {
                            number_plate.push_back("");
                            number_plate[a].push_back(alpha);
                            number_plate[a].push_back(beta);
                        }
                        else
                        {
                            x = 0;
                            while(curr_seg.x > *(char_x.begin()+x))
                            {
                                if(x == (char_x.size()-1)) break;
                                x++;
                            }
                            //for each existing numplate, add at same place
                            for(j=0; j<np_size; j++)
                            {
                                if(a!=(possible_div.size()-1)) number_plate.push_back(number_plate[a*np_size+j]);
                                number_plate[a*np_size+j].insert(number_plate[a*np_size+j].begin()+x, beta);
                                number_plate[a*np_size+j].insert(number_plate[a*np_size+j].begin()+x, alpha);
                            }
                        }
                    }
                    //insert positions later, positions will be same
                    if(char_x.empty())
                    {
                        char_x.push_back(curr_seg.x);
                        char_x.push_back(curr_seg.x+possible_div[0]);
                    }
                    else
                    {
                        char_x.insert(char_x.begin()+x, (curr_seg.x+possible_div[0]));
                        char_x.insert(char_x.begin()+x, curr_seg.x);
                    }
                    //reassign values!
                    possible_div.clear();
                    count = 1;
                }
            }
            //check for min size requirement ends here
        }
        cout<<"np_size "<<number_plate.size()<<endl;
        file.close();
        //output possible numberplate candidates:
        cout<<"Number plate candidates:"<<endl;
        for(int a=0; a<number_plate.size(); a++)
        {
            cout<<number_plate[a]<<endl;
            cout<<"np cost "<<SpellCheckCost(number_plate[a])<<endl;
        }
        //add code for sanity check and then to return
        //now check if the number plates made can be valid
        //if yes, break
        num_plate.push_back(number_plate);
        waitKey();
    }
}

