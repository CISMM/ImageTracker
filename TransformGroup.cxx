
#include <iostream>
#include <fstream>

#include "itkLightObject.h"
#include "itkObjectFactory.h"
#include "itkTransform.h"
#include "itkCenteredRigid2DTransform.h"

#include "TransformGroup.h"

/**
 * Saves a group of transforms to a file.  The transforms are saved in a format
 * that can be read by LoadTransforms().  The file format is:
 *  TransformClass paramCount: param1, param2, param3, ... paramN
 *  ...
 */
void TransformGroup::SaveTransforms(TransformVector* transforms, std::string fileName)
{
    // Open file for writing.  Write over contents in file.
    std::ofstream fileOut(fileName.c_str(), std::ios::out | std::ios::trunc);

    // Ensure the file was openned properly
    if (fileOut.bad())
    {
        std::cerr << "TransformGroup::SaveTransforms() Warning: Could not open file " 
            << fileName << ". Transforms not saved." << std::endl;
        return;
    }

    TransformGroup::TransformVector::iterator tranIt = transforms->begin();
    TransformGroup::TransformPointer transform;
    int count = 0;

    // Write each transform to the file
    while (tranIt != transforms->end())
    {
        if (count != 0)
            fileOut << std::endl; // New line

        transform = *tranIt;
        const TransformType::ParametersType params = transform->GetParameters();
        int paramCount = params.GetNumberOfElements();
        
        // Prefix with transform class name
        fileOut << transform->GetNameOfClass() << " "
            << paramCount << TransformGroupConst::objectDelim << " ";
        for (int i=0; i < paramCount; i++)
        {
            fileOut << params.get(i);
            if (i < paramCount - 1)
            {
                fileOut << TransformGroupConst::attribDelim << " ";
            }
        }

        // Advance iterator
        *tranIt++;
        count++;
    }

    // Close file
    fileOut.close();

    // Log transform count, if debugging.
#ifdef DEBUG
    std::clog << count << " transforms written to " << fileName << "." << std::endl;
#endif
}

TransformGroup::TransformVector* TransformGroup::LoadTransforms(std::string fileName)
{
    TransformGroup::TransformVector* transforms = new TransformGroup::TransformVector();
    // Open file for reading
    std::ifstream inFile(fileName.c_str());

    // Ensure the file has been openned properly
    if (inFile.bad())
    {
        std::cerr << "TransformGroup::LoadTransforms() Warning: Could not open file "
            << fileName << ". No transforms loaded." << std::endl;
        return transforms;
    }

    // Read each line of the file, creating transforms
    // We'll use the itk ObjectFactory to create transform objects of the proper type.
    while (!inFile.eof())
    {
        std::string xFormName;
        int paramCount;
        std::string delim;
        double param;
        inFile >> xFormName;
        inFile >> paramCount;

        TransformGroup::TransformType::ParametersType* params = new TransformGroup::TransformType::ParametersType(paramCount);
        itk::CenteredRigid2DTransform<double>::Pointer xForm = itk::CenteredRigid2DTransform<double>::New();
        xForm->SetIdentity();

        for (int i = 0; i < paramCount; i++)
        {
            inFile >> delim;
            inFile >> param;
            params->put(i, param);
        }
        xForm->SetParameters(*params);
        transforms->push_back((TransformPointer) xForm);
    }

    inFile.close();

#ifdef DEBUG
    std::clog << transforms->size() << " transforms read from file " << fileName << "." << std::endl;
#endif

    return transforms;
}