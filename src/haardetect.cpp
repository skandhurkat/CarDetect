// haardetect.cpp
// Copiright 2011, 2012 Skand Hurkat

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


#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

#include <globals.h>
#include <haardetect.h>

using namespace cv;
using namespace std;

int haardetect(void)
{
    Mat l_img;
    Mat small_img;
    vector<Rect> detected;
    Rect temp;
    while(true)
    {
        if(terminate_program)
        {
            cerr << "Haar thread received terminate signal. Aborting!" << endl;
            return 0;
        }
        img_mutex.lock();
        cvtColor(img, l_img, CV_BGR2GRAY);
        img_mutex.unlock();
        resize(l_img, small_img, Size(0,0), 1/scale, 1/scale);
        equalizeHist(small_img, small_img);
        cascade.detectMultiScale(small_img, detected, 1.1);

        haarrectangles_mutex.lock();
        haarrectangles.clear();
        for(unsigned int i=0; i<detected.size(); i++)
        {
            temp = detected[i];
            temp.x *= scale;
            temp.y *= scale;
            temp.height *= scale;
            temp.width *= scale;
            haarrectangles.push_back(temp);
        }
        haarrectangles_mutex.unlock();
        detected.clear();
    }
    return 0;
}
