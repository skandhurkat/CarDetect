#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <vector>

#include "globals.h"
#include "haardetect.h"

using namespace cv;
using namespace std;

unsigned __stdcall haardetect(void* param)
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
        if(WaitForSingleObject(img_mutex, INFINITY) == WAIT_FAILED)
        {
            cerr << "Wait for img_mutex failed. Terminating\n";
// TODO (Skand Hurkat#1#): Clean up the various threads before terminating
            return 1;
        }
        cvtColor(img, l_img, CV_BGR2GRAY);
        if(!ReleaseMutex(img_mutex))
        {
            cerr << "No idea why I cannot release the img_mutex! Terminating\n";
// TODO (Skand Hurkat#1#): Clean up the various threads before terminating
            return 1;
        }
        resize(l_img, small_img, Size(0,0), 1/scale, 1/scale);
        equalizeHist(small_img, small_img);
        cascade.detectMultiScale(small_img, detected, 1.1);

        if(WaitForSingleObject(haarrectangles_mutex, INFINITY) == WAIT_FAILED)
        {
            cerr << "Wait for haarrectangles_mutex failed. Terminating\n";
// TODO (Skand Hurkat#1#): Clean up the various threads before terminating
            return 1;
        }
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
        if(!ReleaseMutex(haarrectangles_mutex))
        {
            cerr << "No idea why I cannot release the haarrectangles_mutex! Terminating\n";
// TODO (Skand Hurkat#1#): Clean up the various threads before terminating
            return 1;
        }
        detected.clear();
    }
    return 0;
}
