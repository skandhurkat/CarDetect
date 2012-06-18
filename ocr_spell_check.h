//ocr_spell_check.h
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

#ifndef OCR_SPELL_CHECK_H_INCLUDED
#define OCR_SPELL_CHECK_H_INCLUDED

#include <string>
#include <cctype>
#include <cstdlib>
using namespace std;

int SpellCheckCost(string& szInputString)
{
    int i;
    int iStateCharacters = 0;
    int iDistrictCode = 0;
    int iAlphabetPrefix = 0;
    int iCarNumber = 0;
    int iPenalty = 0;
    int iFlag = 0;
    int iUnrecognized = 0;
    char cTemp;
    for(i=0; i<szInputString.size(); i++)
    {
        cTemp = szInputString[i];
        if(iFlag == 0)
        {
            if(isalpha(cTemp))
            {
                iStateCharacters++;
            }
            else if(cTemp == '_')
            {
                iPenalty++;
                iUnrecognized++;
                if(iStateCharacters <= 2)
                    iStateCharacters++;
                else iFlag++;
            }
            else
            {
                if(isdigit(cTemp))
                {
                    if(iStateCharacters >= 2)
                    {
                        iFlag++;
                    }
                    else
                    {
                        iPenalty += 2;
                        iFlag++;
                    }
                }
                else iPenalty += 100;
            }
        }
        if(iFlag == 1)
        {
            if(isdigit(cTemp))
            {
                iDistrictCode++;
            }
            else if(cTemp == '_')
            {
                iPenalty++;
                iUnrecognized++;
                if(iDistrictCode <= 2)
                    iDistrictCode++;
                else iFlag++;
            }
            else if(isalpha(cTemp))
            {
                if(iDistrictCode >= 2)
                {
                    iFlag++;
                }
                else if(cTemp == 't')
                {
                    szInputString[i] = '1';
                    iPenalty++;
                    iDistrictCode++;
                }
                else if(cTemp == 'J')
                {
                    szInputString[i] = '3';
                    iPenalty++;
                    iDistrictCode++;
                }
                else
                {
                    iPenalty += 2;
                    iFlag++;
                }
            }
            else iPenalty += 100;
        }
        if(iFlag == 2)
        {
            if(isalpha(cTemp))
            {
                if(cTemp != 't')
                    iAlphabetPrefix++;
                else iFlag++;
            }
            else if(cTemp == '_')
            {
                iPenalty++;
                iUnrecognized++;
                if(iAlphabetPrefix <= 2)
                    iAlphabetPrefix++;
                else iFlag++;
            }
            else
            {
                if(isdigit(cTemp))
                {
                    if(iAlphabetPrefix >= 1)
                    {
                        iFlag++;
                    }
                    else
                    {
                        iPenalty += 2;
                        iFlag++;
                    }
                }
                else iPenalty += 100;
            }
        }
        if(iFlag == 3)
        {
            if(isdigit(cTemp))
            {
                iCarNumber++;
            }
            else if(cTemp == '_')
            {
                iPenalty++;
                iUnrecognized++;
                if(iCarNumber <= 4)
                    iCarNumber++;
                else iFlag++;
            }
            else if(isalpha(cTemp))
            {
                if(iCarNumber >= 4)
                {
                    iFlag++;
                }
                else if(cTemp == 't')
                {
                    szInputString[i] = '1';
                    iPenalty++;
                    iDistrictCode++;
                }
                else if(cTemp == 'J')
                {
                    szInputString[i] = '3';
                    iPenalty++;
                    iDistrictCode++;
                }
                else
                {
                    iPenalty += 2;
                    iFlag++;
                }
            }
            else iPenalty += 100;
        }
        if(iFlag >= 4)
        {
            iPenalty += 3;
        }
    }
    iPenalty += 3*abs(iStateCharacters-2);
    iPenalty += 3*abs(iDistrictCode-2);
    if(iAlphabetPrefix > 2) iPenalty += 3*(iAlphabetPrefix-2);
    if(iDistrictCode > 4) iPenalty += 3*(iDistrictCode-4);
    if(iUnrecognized>szInputString.size()/2) iPenalty += 100 + 50*iUnrecognized/szInputString.size();
    return iPenalty;
}

#endif // OCR_SPELL_CHECK_H_INCLUDED
