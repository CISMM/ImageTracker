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

    /*
     * Saves a group of transforms to a file.  The transforms are saved in a format
     * that can be read by LoadTransforms().  The file format is:
     *  TransformClass paramCount: param1, param2, param3, ... paramN
     *  ...
     */
    static void SaveTransforms(TransformVector* transforms, std::string fileName);

    /*
     * Load a set of transforms in from a file.  The file format must conform to
     * what is written by a call to SaveTransforms().
     */
    static TransformVector* LoadTransforms(std::string fileName);

    /*
     * Logs the data stored in a group of transforms to the Logger.
     */
    static void LogTransforms(TransformVector& transforms);

private:
    static const std::string objectDelim;
    static const std::string attribDelim;
};

namespace TransformGroupConst
{
    static const std::string objectDelim = ";";
    static const std::string attribDelim = ",";
}
