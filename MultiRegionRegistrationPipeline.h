#pragma once

#include "CommonTypes.h"
#include "FileSet.h"
#include "ImageSetReader.h"
#include "MultiRegionRegistration.h"

class MultiRegionRegistrationPipeline
{
public:
    typedef ImageSetReader< CommonTypes::InputImageType, CommonTypes::InternalImageType > ReaderType;

    MultiRegionRegistrationPipeline(void);
    ~MultiRegionRegistrationPipeline(void);

    void SetSource(const FileSet& source) { this->source = source; }
    void SetDestination(const FileSet& destination) { this->destination = destination; }
    const FileSet& GetSource() { return this->source; }
    const FileSet& GetDestination() { return this->destination; }
    void SetRegistrar(MultiRegionRegistration* registrar);
    MultiRegionRegistration* GetRegistrar(void);
    void Update(bool findFeatures = false);

private:
    FileSet source;
    FileSet destination;
    MultiRegionRegistration* registrar;
};
