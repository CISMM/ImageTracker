#include ".\ImageCanvas.h"
#include "..\ItkMagickIO.h"
#include "..\Logger.h"

#include "itkExceptionObject.h"

ImageCanvas::ImageCanvas(wxWindow *parent, wxWindowID id, 
        const wxPoint &position, const wxSize &size, 
        long style, const wxString &name)
        : wxScrolledWindow(parent, id, position, size, style, name)
{
    wxImage::AddHandler(new wxPNGHandler());
    wxImage::AddHandler(new wxTIFFHandler());
    wxImage::AddHandler(new wxJPEGHandler());
    this->theImage = (wxImage *) NULL;
    //this->originalImage = (wxImage *) NULL;

    this->mode = IMAGE_TRUE_SIZE;
}

ImageCanvas::~ImageCanvas(void)
{
    this->clearImage();
}


void ImageCanvas::LoadFile(const wxString &name, wxBitmapType type)
{
    this->clearImage();

    this->LoadItkImage(ItkMagickIO::Read(name.c_str()));

    //try
    //{
    //    ReaderType::Pointer reader = ReaderType::New();
    //    reader->SetFileName(name.c_str());
    //    reader->Update();
    //    this->LoadItkImage(reader->GetOutput());
    //}
    //catch (itk::ExceptionObject &err)
    //{
    //    wxString text;
    //    text.Printf("Error occurred while trying to read image: %s", name.c_str());
    //    Logger::logError(text.c_str());
    //    Logger::logError(err.GetDescription());
    //}
    
    return;

    //this->theImage = new wxImage(name);

    //if (!this->theImage->Ok())
    //{
    //    this->clearImage();
    //}
    //else
    //{
    //    //this->originalImage = new wxImage(this->theImage);
    //}

    //this->Refresh();
}

void ImageCanvas::LoadItkImage(ImageCanvas::RGBImage::Pointer itkImg)
{
    this->clearImage();
    this->theImage = this->itkToWxImage(itkImg.GetPointer());
    
    if (!this->theImage->Ok())
    {
        this->clearImage();
    }
    else
    {
        //this->originalImage = new wxImage(this->theImage);
    }

    this->Refresh();
}

void ImageCanvas::LoadItkImage(InternalImageType::Pointer itkImg)
{
    this->clearImage();
    
    // CasterType::Pointer caster = CasterType::New();
    //caster->SetInput(itkImg);
    //caster->Update();

    RescalerType::Pointer rescaler = RescalerType::New();
    rescaler->SetOutputMinimum(0);
    rescaler->SetOutputMaximum(255);
    rescaler->SetInput(itkImg);
    rescaler->Update();
    
    DisplayImageType::Pointer pImg = rescaler->GetOutput();
    this->theImage = this->itkToWxImage(pImg.GetPointer());
    
    if (!this->theImage->Ok())
    {
        this->clearImage();
    }
    else
    {
        //this->originalImage = new wxImage(this->theImage);
    }

    this->Refresh();
}

void ImageCanvas::OnDraw(wxDC &dc)
{
    if (this->theImage && this->theImage->Ok())
    {
        wxBitmap* bitmap = new wxBitmap(this->theImage);
        dc.DrawBitmap(*bitmap, 0, 0, FALSE);
        delete bitmap;
    }
}

const wxImage * ImageCanvas::GetImage()
{
    return this->theImage;
}

void ImageCanvas::clearImage()
{
    if (this->theImage)
    {
        this->theImage->Destroy();
        //this->originalImage->Destroy();
        delete this->theImage;
        //delete this->originalImage;
    }
    this->theImage = (wxImage *) NULL;
    //this->originalImage = (wxImage *) NULL;
}

wxImage* ImageCanvas::itkToWxImage(const ImageCanvas::RGBImage *image)
{
    ImageCanvas::RGBImage::RegionType region = image->GetLargestPossibleRegion();
    ImageCanvas::RGBImage::SizeType size = region.GetSize();
    int nx = size[0];
    int ny = size[1];

    const unsigned int nPixels = nx * ny;
    const ImageCanvas::UCBufferPointer &buffer = new unsigned char[nPixels*3];
    unsigned char * data = buffer;

    ImageCanvas::RGBIterator it(image, region);
    it.Begin();
    
    for ( ; !it.IsAtEnd(); ++it)
    {
        data[0] = it.Get().GetRed();
        data[1] = it.Get().GetGreen();
        data[2] = it.Get().GetBlue();
        ++data; ++data; ++data;
    }

    wxImage* wxImg = new wxImage(nx, ny, buffer);
    data = (unsigned char *) NULL;
    return wxImg;
}

wxImage* ImageCanvas::itkToWxImage(const ImageCanvas::DisplayImageType *image)
{
    ImageCanvas::DisplayImageType::RegionType region = image->GetLargestPossibleRegion();
    ImageCanvas::DisplayImageType::SizeType size = region.GetSize();
    int nx = size[0];
    int ny = size[1];

    const unsigned int nPixels = nx * ny;
    const ImageCanvas::UCBufferPointer &buffer = new unsigned char[nPixels*3];
    unsigned char * data = buffer;

    ImageCanvas::DisplayImageIterator it(image, region);
    it.Begin();
    
    for ( ; !it.IsAtEnd(); ++it)
    {
        data[0] = it.Get();
        data[1] = it.Get();
        data[2] = it.Get();
        ++data; ++data; ++data;
    }

    wxImage* wxImg = new wxImage(nx, ny, buffer);
    data = (unsigned char *) NULL;
    return wxImg;
}

void ImageCanvas::SetMode(int mode)
{
    this->mode = mode;

    //if (this->mode == IMAGE_TRUE_SIZE)
    //{
    //    this->resetImageSize();
    //    this->Refresh();
    //}
    //else
    //{
    //    this->scaleImageToCanvas();
    //    this->Refresh();
    //}
}

void ImageCanvas::resetImageSize()
{
    //if (!this->theImage)
    //    return;

    //this->theImage = new wxImage(this->originalImage);
}

void ImageCanvas::scaleImageToCanvas()
{
    // Exit early if we have no image.
    //if (!this->theImage)
    //    return;

    //int newW = this->originalImage->GetWidth();
    //int newH = this->originalImage->GetHeight();

    //wxSize originalSize = wxSize(this->originalImage->GetWidth(), this->originalImage->GetHeight());
    //wxSize canvasSize = this->GetClientSize();
    //
    //if (originalSize.GetWidth() > 0 && originalSize.GetHeight() > 0 &&
    //    canvasSize.GetWidth() > 0 && canvasSize.GetHeight() > 0)
    //{
    //    float widthRatio = (float) canvasSize.GetWidth() / originalSize.GetWidth();
    //    float heightRatio = (float) canvasSize.GetHeight() / originalSize.GetHeight();

    //    if (widthRatio < heightRatio)
    //    {
    //        newW = canvasSize.GetWidth();
    //        newH = (int) (widthRatio * originalSize.GetHeight());
    //    }
    //    else
    //    {
    //        newH = canvasSize.GetHeight();
    //        newW = (int) (heightRatio * originalSize.GetWidth());
    //    }
    //}

    //this->theImage = new wxImage(this->originalImage);
    //this->theImage->Rescale(newW, newH);
}

BEGIN_EVENT_TABLE(ImageCanvas,wxWindow)
EVT_SIZE(ImageCanvas::OnSizeEvent)
END_EVENT_TABLE()

void ImageCanvas::OnSizeEvent(wxSizeEvent &event)
{
    event.Skip();

    //if (!this->theImage)
    //    return;

    //if (this->mode == IMAGE_SCALE_TO_FIT)
    //{
    //    this->scaleImageToCanvas();
    //    this->Refresh();
    //}

    //wxString text;
    //wxSize size = this->GetClientSize();
    //text.Printf("Canvas: %i x %i", size.GetWidth(), size.GetHeight());
    //Logger::logVerbose(text.c_str());
    //text.Printf("Image: %i x %i", this->theImage->GetWidth(), this->theImage->GetHeight());
    //Logger::logVerbose(text.c_str());
}
