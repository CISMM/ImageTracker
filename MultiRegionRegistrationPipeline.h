#pragma once

#include "FileSet.h"
#include "MultiRegionRegistration.h"

class MultiRegionRegistrationPipeline
{
public:
    MultiRegionRegistrationPipeline(void);
    ~MultiRegionRegistrationPipeline(void);

    void SetSource(FileSet* source);
    void SetDestination(FileSet* destination);
    FileSet* GetSource(void);
    FileSet* GetDestination(void);
    void SetRegistrar(MultiRegionRegistration* registrar);
    MultiRegionRegistration* GetRegistrar(void);
    void Update(bool findFeatures = false);

private:
    FileSet* source;
    FileSet* destination;
    MultiRegionRegistration* registrar;
};
