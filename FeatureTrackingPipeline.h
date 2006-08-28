#pragma once

#include "CommonTypes.h"
#include "FileSet.h"
#include "ImageSetReader.h"
#include "HarrisFeatureDetector.h"
#include "MultiRegionRegistration.h"

class FeatureTrackingPipeline
{
public:
    typedef CommonTypes::InputImageType InputImageType;
    typedef CommonTypes::InternalImageType InternalImageType;
    typedef ImageSetReader< InputImageType, InternalImageType > ReaderType;

    FeatureTrackingPipeline(void) :
        init(false),
        index(0)
    {
        this->detector = new HarrisFeatureDetector();
        this->registrar = new MultiRegionRegistration();
    }

    ~FeatureTrackingPipeline(void)
    {
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
        
        return this->detector->harrisWeightImage(this->reader[this->index]);
    }

    int GetMaxFeatures() { return this->detector->GetMaxCount(); }
    double GetScale() { return this->detector->GetSigma(); }
    int GetMaxDistance() { return this->registrar->GetRadiusOfInterest(); }
    const FileSet& GetSource() { return this->source; }
    const FileSet& GetDestination() { return this->destination; }

    void SetMaxFeatures(int count) { this->detector->SetMaxCount(count); }
    void SetScale(double scale) { this->detector->SetSigma(scale); }
    void SetMaxDistance(int pixels) { this->registrar->SetRadiusOfInterest(pixels); }
    
    void SetSource(const FileSet& source) 
    { 
        this->source = source;
        this->init = false;
    }
    void SetDestination(const FileSet& dest) 
    { 
        this->destination = dest;
        this->init = false;
    }

private:
    void InitializePipeline();

    bool init;
    unsigned int index;
    FileSet source;
    FileSet destination;

    ReaderType reader;
    HarrisFeatureDetector* detector;
    MultiRegionRegistration* registrar;
};
