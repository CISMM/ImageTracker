#pragma once

#include <string>

#include "itkImage.h"
#include "itkObject.h"

#include "FileSet.h"
#include "ImageSetReader.h"

enum MetricType 
{
    Mean,
    Median
};

/**
 * \class RemovePartialOcclusionsPipeline
 * \brief Computes and removes the constant occlusions from a bright-field microscopy image sequence.
 */
class RemovePartialOcclusionsPipeline :
    public itk::Object
{
public:

    // Standard ITK typedefs
    typedef RemovePartialOcclusionsPipeline Self;
    typedef itk::Object Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    
    itkNewMacro(Self);
    
    // Typedefs
    typedef itk::Image< unsigned short, 2 > InputImageType;
    typedef itk::Image< float, 2 > InternalImageType;
    typedef itk::Image< unsigned short, 2 > OutputImageType;
    typedef InputImageType::PixelType PixelType;
    typedef ImageSetReader< InputImageType, InternalImageType > VideoType;
    
    itkStaticConstMacro(ImageDimension, unsigned int,
                        InputImageType::ImageDimension);
    // Getters/setters
    itkGetMacro(Metric, MetricType);
    itkSetMacro(Metric, MetricType);
    
    itkGetMacro(TransmitPercentile, int);
    itkSetMacro(TransmitPercentile, int);
    
    itkGetMacro(FourierPadding, double);
    itkSetMacro(FourierPadding, double);
    
    itkGetMacro(TransmissionFile, std::string);
    itkSetMacro(TransmissionFile, std::string);
    
    const FileSet& GetInputFiles() const
    { return this->inputFiles; }

    void SetInputFiles(const FileSet& files)
    { this->inputFiles = files; }
    
    const FileSet& GetOutputFiles() const
    { return this->outputFiles; }

    void SetOutputFiles(const FileSet& files)
    { this->outputFiles = files; }
    
    /**
     * Compute and remove the constant occlusions from the image sequence.
     */
    void Update();

protected:
    RemovePartialOcclusionsPipeline()
    :   inputFiles(),
        outputFiles(),
        m_TransmissionFile(""),
        m_TransmitPercentile(90),
        m_FourierPadding(0.5),
        m_Metric(Median)
    {}
        
    virtual ~RemovePartialOcclusionsPipeline(){}
    
    InternalImageType::Pointer ComputeTransmissionMean(VideoType& video);
    InternalImageType::Pointer ComputeTransmissionMedian(VideoType& video);
    
private:
    // Purposefully not implemented
    RemovePartialOcclusionsPipeline(const Self& other);
    void operator=(const Self& other);
    
    // Attributes
    FileSet inputFiles;
    FileSet outputFiles;
    std::string m_TransmissionFile;
    int m_TransmitPercentile;
    double m_FourierPadding;
    MetricType m_Metric;
};
