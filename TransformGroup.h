#pragma once

#include <vector>
#include <string>

#include "itkArray.h"
#include "itkTransform.h"
#include "itkCenteredRigid2DTransform.h"

#include "CommonTypes.h"

/**
 * Represents a group of transformations--a simple collection.  Enables
 * file persistence of transformations.
 */
class TransformGroup
{
public:
    typedef CommonTypes::TransformType TransformType;
    typedef TransformType::Pointer TransformPointer;
    typedef std::vector<TransformPointer> TransformVector;

    static void SaveTransforms(TransformVector* transforms, std::string fileName);
    static TransformVector* LoadTransforms(std::string fileName);

private:
    static const std::string objectDelim;
    static const std::string attribDelim;
};

namespace TransformGroupConst
{
    static const std::string objectDelim = ";";
    static const std::string attribDelim = ",";
}
