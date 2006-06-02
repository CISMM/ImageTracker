#include "TransformGroup.h"

#include <iostream>
#include <fstream>

#include "itkLightObject.h"
#include "itkObjectFactory.h"
#include "itkTransform.h"
#include "itkCenteredRigid2DTransform.h"

#include "Logger.h"

void TransformGroup::LogTransforms(TransformVector* transforms)
{
    if (!transforms)
    {
        Logger::logWarn("LogTransforms(): The transform group was NULL.");
    }
    else
    {
        char vars[200];
        std::string text;
        TransformGroup::TransformPointer transform;
        Logger::logInfo("LogTransforms(): ");
        
        for (TransformGroup::TransformVector::iterator tranIt = transforms->begin();
            tranIt != transforms->end();
            tranIt++)
        {
            transform = *tranIt;
            const TransformType::ParametersType params = transform->GetParameters();
            int paramCount = params.GetNumberOfElements();
            
            // Prefix with transform class name
            sprintf(vars, "%s (%i): ", transform->GetNameOfClass(), paramCount);
            text = vars;
            for (int i=0; i < paramCount; i++)
            {
                sprintf(vars, "%9.5f, ", params.get(i));
                text += vars;
            }

            Logger::logInfo(text);
        }
    }
}

void TransformGroup::SaveTransforms(TransformVector* transforms, std::string fileName)
{
    // Open file for writing.  Write over contents in file.
    std::ofstream fileOut(fileName.c_str(), std::ios::out | std::ios::trunc);

    // Ensure the file was openned properly
    if (fileOut.bad())
    {
        char text[200];
        sprintf(text, "TransformGroup::SaveTransforms() Warning: Could not open file %s. Transforms not saved.", fileName.c_str());
        Logger::logError(text);
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
    char text[200];
    sprintf(text, "%i transforms written to %s.", count, fileName.c_str());
    Logger::logDebug(text);
}

TransformGroup::TransformVector* TransformGroup::LoadTransforms(std::string fileName)
{
    TransformGroup::TransformVector* transforms = new TransformGroup::TransformVector();
    // Open file for reading
    std::ifstream inFile(fileName.c_str());

    // Ensure the file has been openned properly
    if (inFile.bad())
    {
        char text[200];
        sprintf(text, 
            "TransformGroup::LoadTransforms() Warning: Coult not open file %s. No transforms loaded.", fileName.c_str());
        Logger::logError(text);
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
        TransformType::Pointer xForm = TransformType::New();
        // itk::CenteredRigid2DTransform<double>::Pointer xForm = itk::CenteredRigid2DTransform<double>::New();
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

    char text[200];
    sprintf(text, "%i transforms read from file %s.", transforms->size(), fileName.c_str());
    Logger::logDebug(text);

    return transforms;
}