#pragma once

#include "vtkScalarsToColors.h"

enum ColorMap
{
    IsoGray,
    CIE,
    RedGreen,
    BlueYellow,
    RedYellowGray
};

vtkScalarsToColors* MakeLookupTable(ColorMap type, double rangeStart, double rangeEnd);

vtkScalarsToColors* MakeIsoGrayLut(double rangeStart, double rangeEnd);

vtkScalarsToColors* MakeCieLut(double rangeStart, double rangeEnd);

vtkScalarsToColors* MakeRedGreenLut(double rangeStart, double rangeEnd);

vtkScalarsToColors* MakeBlueYellowLut(double rangeStart, double rangeEnd);

vtkScalarsToColors* MakeRedYellowGrayLut(double rangeStart, double rangeEnd);
