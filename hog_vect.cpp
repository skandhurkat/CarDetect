//hog_vect.cpp
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

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <vector>

#include "globals.h"
#include "hog_vect.h"

using namespace cv;
using namespace std;

extern unsigned __stdcall hog_vect(void* param)
{
    vector<Mat> l_imgs;
    vector<vector<float> > l_hog_vectors;
    while(true)
    {
        if(terminate_program)
        {
            cerr << "HoG thread received terminate signal. Aborting!" << endl;
            return 0;
        }
        if(WaitForSingleObject(img_mutex, INFINITY) == WAIT_FAILED)
        {
            cerr << "Wait for img_mutex failed. Terminating\n";
// TODO (Skand Hurkat#1#): Clean up the various threads before terminating
            return 1;
        }
        if(WaitForSingleObject(haarrectangles_mutex, INFINITY) == WAIT_FAILED)
        {
            cerr << "Wait for haarrectangles_mutex failed. Terminating\n";
// TODO (Skand Hurkat#1#): Clean up the various threads before terminating
            return 1;
        }
        for(int i=0; i<haarrectangles.size(); i++)
        {
            //code for taking in the images
        }
    }
}
