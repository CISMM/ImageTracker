#pragma once

#include <algorithm>
#include <string>

#include "itkImage.h"
#include "itkLightObject.h"
#include "itkVector.h"

#include "FileSet.h"
#include "ImageSetReader.h"

enum PixelDataType
{ 
    ScalarChar, 
    ScalarShort, 
    ScalarFloat, 
    VectorFloat
};

typedef itk::Image< unsigned char, 2 > ImageTypeUC2;
typedef itk::Image< unsigned short, 2 > ImageTypeUS2;
typedef itk::Image< float, 2 > ImageTypeF2;
typedef itk::Image< itk::Vector< float, 2 >, 2 > ImageTypeV2F2;

typedef ImageSetReader< ImageTypeUC2, ImageTypeF2 > ImageSetReaderUC2;
typedef ImageSetReader< ImageTypeUS2, ImageTypeF2 > ImageSetReaderUS2;
typedef ImageSetReader< ImageTypeF2, ImageTypeF2 > ImageSetReaderF2;
typedef ImageSetReader< ImageTypeV2F2, ImageTypeV2F2 > ImageSetReaderV2F2;

PixelDataType GetInputPixelDataType(ImageSetReaderBase* reader);

// template < class TOutputType >
// PixelDataType GetInputPixelDataType(ImageSetReader< ImageTypeUC2, TOutputType >* reader)
// {
//     return ScalarChar;
// }
// 
// template < class TOutputType >
// PixelDataType GetInputPixelDataType(ImageSetReader< ImageTypeUS2, TOutputType >* reader)
// {
//     return ScalarShort;
// }
// 
// template < class TOutputType >
// PixelDataType GetInputPixelDataType(ImageSetReader< ImageTypeF2, TOutputType >* reader)
// {
//     return ScalarFloat;
// }
// 
// template < class TOutputType >
// PixelDataType GetInputPixelDataType(ImageSetReader< ImageTypeV2F2, TOutputType >* reader)
// {
//     return VectorFloat;
// }

/**
 * /class DataSource
 * /brief Encapsulates input data information.
 * An ImageTracker DataSource is a collection of image files.  A DataSource has a
 * name, data type, and list of files associated.  Subclasses provide support for
 * the common types of input pixel data ImageTracker handles (e.g. scalar char,
 * scalar short, scalar float, and vector images).
 */
class DataSource :
    public itk::LightObject
{
public:
    typedef DataSource Self;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    
    itkNewMacro(Self);
    
    const std::string& GetName() const
    {
        return this->name;
    }

    void SetName(const std::string& name)
    {
        this->name = name;
    }

    const FileSet& GetFiles() const
    {
        return this->files;
    }

    void SetFiles(const FileSet& files)
    {
        this->files = files;
        this->video->SetFiles(this->files);
    }

    int size() const
    {
        return this->video->size();
    }

    /**
     * Return the ith image from this data source. If i overruns the number
     * of images in the data source, return the last image.
     */
    itk::DataObject* GetImage(unsigned int i);
    const itk::DataObject* GetImage(unsigned int i) const;
    
    /**
     * Provides direct access to the image reader controlled by this DataSource.
     */
    ImageSetReaderBase* GetImages()
    { return this->video; }

    /**
     * Determine the type of input pixel that this data source handles.
     */
    PixelDataType GetPixelDataType();
    
    /**
     * Set the type of input pixel this data source handles.
     */
    void SetPixelDataType(PixelDataType type);
    
protected:
    DataSource();
    virtual ~DataSource();

private:
    std::string name;
    FileSet files;
    ImageSetReaderBase* video;
};
