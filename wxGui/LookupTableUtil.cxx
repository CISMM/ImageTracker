#include "LookupTableUtil.h"

#include "vtkColorTransferFunction.h"

vtkScalarsToColors* MakeLookupTable(ColorMap type, double rangeStart, double rangeEnd)
{
    switch (type)
    {
        case IsoGray:
            return MakeIsoGrayLut(rangeStart, rangeEnd);
            break;
        case CIE:
            return MakeCieLut(rangeStart, rangeEnd);
            break;
        case RedGreen:
            return MakeRedGreenLut(rangeStart, rangeEnd);
            break;
        case BlueYellow:
            return MakeBlueYellowLut(rangeStart, rangeEnd);
            break;
        case RedYellowGray:
            return MakeRedYellowGrayLut(rangeStart, rangeEnd);
            break;
        default:
            return MakeIsoGrayLut(rangeStart, rangeEnd);
            break;
    }
}

vtkScalarsToColors* MakeIsoGrayLut(double rangeStart, double rangeEnd)
{
    vtkColorTransferFunction* lut = vtkColorTransferFunction::New();
    lut->AddHSVPoint(rangeStart, 0.0, 0.0, 0.80);
    lut->AddHSVPoint(rangeStart, 0.0, 0.0, 0.80);
    return lut;
}

vtkScalarsToColors* MakeCieLut(double rangeStart, double rangeEnd)
{
    vtkColorTransferFunction* lut = vtkColorTransferFunction::New();
    lut->AddHSVPoint(rangeStart, 0.53, 1.0, 0.75);
    lut->AddHSVPoint(rangeEnd, 0.04, 0.55, 0.75);
    return lut;
}

vtkScalarsToColors* MakeRedGreenLut(double rangeStart, double rangeEnd)
{
    vtkColorTransferFunction* lut = vtkColorTransferFunction::New();
    lut->AddHSVPoint(rangeStart, 0.0, 0.85, 0.75);
    lut->AddHSVPoint(rangeEnd, 0.33, 0.85, 0.75);
    return lut;
}

vtkScalarsToColors* MakeBlueYellowLut(double rangeStart, double rangeEnd)
{
    vtkColorTransferFunction* lut = vtkColorTransferFunction::New();
    lut->AddHSVPoint(rangeStart, 0.667, 0.70, 0.90);
    lut->AddHSVPoint(rangeEnd, 0.167, 0.70, 0.90);
    return lut;
}

vtkScalarsToColors* MakeRedYellowGrayLut(double rangeStart, double rangeEnd)
{
    double middle = (rangeStart + rangeEnd) / 2.0;
    vtkColorTransferFunction* lut = vtkColorTransferFunction::New();
    lut->AddHSVPoint(rangeStart, 0.0, 0.85, 0.75);
    lut->AddHSVPoint(middle, 0.167, 0.85, 0.75);
    lut->AddHSVPoint(rangeEnd, 0.167, 0.0, 0.75);
    return lut;
}
