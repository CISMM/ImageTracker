#pragma once
#include "itkImageFileWriter.h"

#include "FileSet.h"
#include "FileSetImageReader.h"
#include "HarrisFeatureDetector.h"
#include "MultiRegionRegistration.h"

class FeatureTrackingPipeline
{
public:
    typedef itk::ImageFileWriter<MultiRegionRegistration::OutputImageType> WriterType;

    FeatureTrackingPipeline(void)
    {
        this->source = NULL;
        this->destination = NULL;
        this->destinationIt = NULL;
        this->reader = NULL;
        this->pCurrentImg = NULL;
        this->pNextImg = NULL;

        this->detector = new HarrisFeatureDetector();
        this->registrar = new MultiRegionRegistration();
        this->init = false;
    }

    ~FeatureTrackingPipeline(void)
    {
        if (this->reader)
            delete this->reader;
        if (this->detector != NULL)
            delete this->detector;
        if (this->registrar != NULL)
            delete this->registrar;
    }

    bool UpdateOne();
    void UpdateAll();

    HarrisFeatureDetector::ImageType::Pointer GetFeatureImage()
    {
        if (!this->init)
            this->InitializePipeline();
        
        return this->detector->harrisWeightImage(this->pCurrentImg);
    }

    int GetMaxFeatures() { return this->detector->GetMaxCount(); }
    double GetScale() { return this->detector->GetSigma(); }
    int GetMaxDistance() { return this->registrar->GetRadiusOfInterest(); }
    FileSet* GetSource() { return this->source; }
    FileSet* GetDestination() { return this->destination; }

    void SetMaxFeatures(int count) { this->detector->SetMaxCount(count); }
    void SetScale(double scale) { this->detector->SetSigma(scale); }
    void SetMaxDistance(int pixels) { this->registrar->SetRadiusOfInterest(pixels); }
    void SetSource(FileSet* source) 
    { 
        this->source = source;
        this->init = false;
    }
    void SetDestination(FileSet* dest) 
    { 
        this->destination = dest;
        this->init = false;
    }

private:
    void InitializePipeline();

    bool init;
    FileSet* source;
    FileSet* destination;

    FileSetImageReader* reader;
    HarrisFeatureDetector* detector;
    MultiRegionRegistration* registrar;
    WriterType::Pointer writer;
    FileSet::FileIterator destinationIt;

    FileSetImageReader::InternalImageType::Pointer pCurrentImg;
    FileSetImageReader::InternalImageType::Pointer pNextImg;
};
