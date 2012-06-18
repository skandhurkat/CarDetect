/* This is the main file in the program. What it does is listed as follows:
 * It accepts arguments from the user and loads the various files.
 * It then creates a thread for haarcascade classification (Viola and Jones)
 * The main thread at regular intervals behaves just like a media player attempting
 * to display a media file in real time. The haarcascade thread accepts the img file,
 * copies it to local storage and stores its output in a haarrectangles vector.
 * The main thread then looks at the haarrectangles vector and marks the rectangles based
 * on the detection of the haarcascade thread. In this way, a real-time solution shall be
 * achieved.
 */

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <getopt.h>
#include <ctime>
#include "globals.h"
#include "haardetect.h"

using namespace std;
using namespace cv;
//using namespace global;

String program_name;
String input_file_name = "";
String cascade_name = "";
bool do_rough_search = false;
bool find_largest_object = false;

double scale = 1;

int cascade_flags = 0;

CascadeClassifier cascade;

Mat img;

vector<Rect> haarrectangles;

bool terminate_program = false;

//mutexes for multithreading:
HANDLE img_mutex;
HANDLE haarrectangles_mutex;

HANDLE haarthread;
unsigned int haarthreadid;

void usage(int exit_code)
{
    String usage_string = "Usage is of the form " + program_name + "--cascade <cascade_name> --input <input file> --scale <scale> [options]\n\
scale must be greater than 1\n\
options are:\n\
--findlargest\n\
--roughsearch\n";
    if(exit_code)
        cerr << usage_string << endl;
    else
        cout << usage_string << endl;
    exit(exit_code);
}

int main(int argc, char** argv)
{
    program_name = argv[0];
    const char* const short_options = "i:c:s:hlr";
    const option long_options[] =
    {
        {"input", required_argument, NULL, 'i'},
        {"cascade", required_argument, NULL, 'c'},
        {"scale", required_argument, NULL, 's'},
        {"findlargest", no_argument, NULL, 'l'},
        {"roughsearch", no_argument, NULL, 'r'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };

    int opt;
    while((opt = getopt_long(argc, argv, short_options, long_options, NULL)) != -1)
    {
        switch(opt)
        {
        case 'h':   usage(0);
                    break;
        case 'i':   input_file_name = optarg;
                    break;
        case 'c':   cascade_name = optarg;
                    break;
        case 's':   scale = atof(optarg);
                    break;
        case 'l':   find_largest_object = true;
                    break;
        case 'r':   do_rough_search = true;
                    break;
        case '?':
        default:    usage(1);
                    break;
        }
    }

    //catch errors in input
    cerr << "haarrectangles scale: " << scale << endl;
    if(scale < 1)
    {
        usage(1);
    }
    cerr << "Cascade path: " << cascade_name << endl;
    if(cascade_name == "")
    {
        usage(1);
    }
    cerr << "Input video file: " << input_file_name << endl;
    if(input_file_name == "")
    {
        usage(1);
    }
    if(do_rough_search)
        cerr << "Rough search option selected." << endl;
    else
        cerr << "Not executing rough search." << endl;

    if(find_largest_object)
        cerr << "Looking for largest object only." << endl;
    else
        cerr << "Looking for objects of all sizes." << endl;

    cascade.load(cascade_name);
    if(cascade.empty())
    {
        cerr << "Error opening cascade " << cascade_name << endl;
        return 1;
    }

    cascade_flags = (do_rough_search?CV_HAAR_DO_ROUGH_SEARCH:0)|(find_largest_object?CV_HAAR_FIND_BIGGEST_OBJECT:0);

    Mat l_img;
    namedWindow("Video", CV_WINDOW_AUTOSIZE);
    VideoCapture cap(input_file_name);
    if(!cap.isOpened())
    {
        cerr << "Error opening " << input_file_name << endl;
        return 1;
    }
    int fps = cap.get(CV_CAP_PROP_FPS);
    cerr << "Video runs at " << fps << " frames per second" << endl;
    int video_width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    int video_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
    cerr << "Video size is " << video_width << 'x' << video_height << endl;
    clock_t t1, t2 = clock();
    int ms_taken, wait_time;

    if(SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS))
    {
        cerr << "Process priority set to Realtime" << endl;
    }

    img_mutex = CreateMutex(NULL, TRUE, NULL);
    haarrectangles_mutex = CreateMutex(NULL, FALSE, NULL);
    if(!img_mutex)
    {
        cerr << "Could not create image mutex. Please contact the developer.\n";
        return 1;
    }
    if(!haarrectangles_mutex)
    {
        cerr << "Could not create haarrectangles mutex. Please contact the developer.\n";
        return 1;
    }

    haarthread = (HANDLE)_beginthreadex(NULL, 0, haardetect, NULL, 0, &haarthreadid);
    if(haarthread == NULL)
    {
        cerr << "Could not create haar thread. Terminating" << endl;
        return 1;
    }
    DWORD haarthreadstatus;
    if(SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL))
    {
        cerr << "Main thread priority set to time critical" << endl;
    }

    clock_t t = clock();
    while(cap.grab())
    {
        t1 = clock();             //for timekeeping
        wait_time = 2000/fps - (t1-t2)*1000/CLOCKS_PER_SEC;
        while(wait_time <= 0)
        {
            if(!cap.grab())
            {
                terminate_program = true;
                WaitForSingleObject(haarthread, INFINITY);
                return 0;
            }
            else
            {
                img.release();
                cap.retrieve(img);
            }
            wait_time += 1000/fps;
        }
        GetExitCodeThread(haarthread, &haarthreadstatus);
        if(haarthreadstatus != STILL_ACTIVE)
        {
            cerr << "Haar thread exited with code " << haarthreadstatus <<". Terminating." << endl;
            return 1;
        }
        img.release();
        cap.retrieve(img);
        l_img = img;
        if(!ReleaseMutex(img_mutex))
        {
            cerr << "No idea why I cannot release the img_mutex! Terminating\n";
// TODO (Skand Hurkat#1#): Clean up the various threads before terminating
            terminate_program = true;
            WaitForSingleObject(haarthread, INFINITY);
            return 1;
        }
        if(waitKey(wait_time) == 27)
        {
// TODO (Skand Hurkat#1#): Clean up the various threads before terminating
            terminate_program = true;
            WaitForSingleObject(haarthread, INFINITY);
            return 0;
        }

        if(WaitForSingleObject(haarrectangles_mutex, INFINITY) == WAIT_FAILED)
        {
            cerr << "Wait for haarrectangles_mutex failed. Terminating\n";
// TODO (Skand Hurkat#1#): Clean up the various threads before terminating
            terminate_program = true;
            WaitForSingleObject(haarthread, INFINITY);
            return 1;
        }

        for(unsigned int i=0; i<haarrectangles.size(); i++)
        {
            rectangle(l_img, Point(haarrectangles[i].x, haarrectangles[i].y),
                      Point(haarrectangles[i].x+haarrectangles[i].width, haarrectangles[i].y+haarrectangles[i].height), Scalar(0,0,255), 5);
        }

        if(!ReleaseMutex(haarrectangles_mutex))
        {
            cerr << "No idea why I cannot release the haarrectangles_mutex! Terminating\n";
// TODO (Skand Hurkat#1#): Clean up the various threads before terminating
            terminate_program = true;
            WaitForSingleObject(haarthread, INFINITY);
            return 1;
        }

        imshow("Video", l_img);

        l_img.release();
//        ms_taken = (getTickCount()-t1)/getTickFrequency()/1000;

        if(WaitForSingleObject(img_mutex, INFINITY) == WAIT_FAILED)
        {
            cerr << "Wait for img_mutex failed. Terminating\n";
// TODO (Skand Hurkat#1#): Clean up the various threads before terminating
            terminate_program = true;
            WaitForSingleObject(haarthread, INFINITY);
            return 1;
        }
        t2 = t1;
    }
    cerr << "The process took " << (clock()-t)/CLOCKS_PER_SEC << " seconds" << endl;
    img.release();
    cap.release();

    return 0;
}
