#pragma once
#include <wx/wx.h>
#include <wx/image.h>
#include "Magick++.h"
#include "itkCastImageFilter.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageRegionConstIterator.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkRGBPixel.h"

#include "../CommonTypes.h"

enum {
    IMAGE_TRUE_SIZE = 1000,
    IMAGE_SCALE_TO_FIT
};

class ImageCanvas : public wxScrolledWindow
{
public:
    typedef itk::RGBPixel<unsigned char> RGBPixel;
    typedef itk::Image<RGBPixel, 2> RGBImage;
    typedef unsigned char * UCBufferPointer;
    typedef itk::ImageRegionConstIterator<RGBImage> RGBIterator;

    typedef CommonTypes::InternalImageType InternalImageType;
    typedef unsigned char DisplayPixelType;
    typedef itk::Image<DisplayPixelType, 2> DisplayImageType;
    typedef itk::ImageRegionConstIterator<DisplayImageType> DisplayImageIterator;

    typedef itk::CastImageFilter<InternalImageType, DisplayImageType> CasterType;
    typedef itk::RescaleIntensityImageFilter<InternalImageType, DisplayImageType> RescalerType;
    typedef itk::ImageFileReader<InternalImageType> ReaderType;

    /*
     * Construct an image canvas.
     */
    ImageCanvas(wxWindow *parent, wxWindowID id = -1, 
        const wxPoint &position = wxDefaultPosition, 
        const wxSize &size = wxDefaultSize, 
        long style = wxHSCROLL | wxVSCROLL, 
        const wxString &name = _("Image"));

    /*
     * Destroy an image canvas.
     */
    ~ImageCanvas(void);

    /*
     * Handles displaying the associated image on this ImageCanvas.
     */
    virtual void OnDraw(wxDC &dc);

    /*
     * Load an image file by name.
     */
    void LoadFile(const wxString &name, wxBitmapType type);

    /*
     * Load an image from an itkImage object with RGB Pixels.
     */
    void LoadItkImage(RGBImage::Pointer img);

    /*
     * Load an image from an itkImage with intensity value pixels.
     */
    void LoadItkImage(InternalImageType::Pointer img);

    void LoadImage(Magick::Image image);

    /*
     * Returns a constant reference to the image associated with this ImageCanvas.
     * Good for getting information about the image.
     */
    const wxImage * GetImage();

    /*
     * Set the image scaling mode for the canvas to use.  (Not yet implemented.)
     */
    void SetMode(int mode);

private:
    /*
     * Convert an itkImage object to a wxImage object.
     */
    wxImage* itkToWxImage(const RGBImage *image);

    /*
     * Convert an itkImage object to a wxImage object.
     */
    wxImage* itkToWxImage(const DisplayImageType* image);

    /*
     * Destroy and dereference the image associated with this ImageCanvas.
     */
    void clearImage();

    /*
     * Respond to a canvas resize event.  (Not yet implemented.)
     */
    void OnSizeEvent(wxSizeEvent &event);

    /*
     * Return the image to its original size. (Not yet implemented.)
     */
    void resetImageSize();

    /*
     * Scale image to fit the canvas. (Not yet implemented.)
     */
    void scaleImageToCanvas();

    /*
     * A reference to the image associated with this ImageCanvas.
     */
    wxImage *theImage;
    //wxImage *originalImage;
    int mode;

DECLARE_EVENT_TABLE();
};
