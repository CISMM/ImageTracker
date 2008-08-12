#pragma once

#include <string>

#include "itkImage.h"
#include "itkObject.h"

#include "FileSet.h"
#include "ImageFileSet.h"
#include "ItkImagePipeline.h"

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
    public ItkImagePipeline
{
public:

    // Standard ITK typedefs
    typedef RemovePartialOcclusionsPipeline Self;
    typedef ItkImagePipeline Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    
    itkNewMacro(Self);
    itkTypeMacro(RemovePartialOcclusionsPipeline, ItkImagePipeline);
    
    // Typedefs
    typedef ImageFileSet::ImageType ImageType;
    typedef ImageTypeUS2 OutputImageType;
        
    itkStaticConstMacro(ImageDimension, unsigned int,
                        ImageType::ImageDimension);
    // Getters/setters
    itkGetMacro(Metric, MetricType);
    itkSetMacro(Metric, MetricType);
    
    itkGetMacro(TransmitPercentile, int);
    itkSetMacro(TransmitPercentile, int);
    
    itkGetMacro(FourierPadding, double);
    itkSetMacro(FourierPadding, double);
    
    itkGetMacro(TransmissionFile, std::string);
    itkSetMacro(TransmissionFile, std::string);
        
    /**
     * Compute and remove the constant occlusions from the image sequence.
     */
    virtual void Update();

protected:
    RemovePartialOcclusionsPipeline()
    :   m_TransmissionFile(""),
        m_TransmitPercentile(90),
        m_FourierPadding(0.5),
        m_Metric(Median)
    {}
        
    virtual ~RemovePartialOcclusionsPipeline(){}
    
    ImageType::Pointer ComputeTransmissionMean(ImageFileSet* images);
    ImageType::Pointer ComputeTransmissionMedian(ImageFileSet* images);
    
private:
    // Purposefully not implemented
    RemovePartialOcclusionsPipeline(const Self& other);
    void operator=(const Self& other);
    
    // Attributes
    std::string m_TransmissionFile;
    int m_TransmitPercentile;
    double m_FourierPadding;
    MetricType m_Metric;
};
