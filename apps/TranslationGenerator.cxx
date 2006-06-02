
#include <string>
#include <vector>

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkVector.h"

#include "TranslationDeformationFieldGenerator.h"
#include "WarpedImageGenerator.h"

int main (int argc, char** argv)
{
    std::cout << argv[0] << std::endl;
    // Ensure we have enough parameters
    if (argc < 6)
    {
        std::cout << "Usage: \n" << argv[0] << " imageIn dirOut count Tx Ty" << std::endl;
        return 1;
    }

    // Typedefs
    enum { Dimension = 2 };
    typedef unsigned short PixelType;
    typedef itk::Image<PixelType, Dimension> ImageType;
    typedef itk::Vector<double, 2> VectorType;
    typedef itk::Image<VectorType, Dimension> DeformationType;

    typedef itk::ImageFileReader<ImageType> ReaderType;
    typedef TranslationDeformationFieldGenerator< DeformationType > SourceType;
    typedef WarpedImageGenerator< ImageType, ImageType, DeformationType > GeneratorType;

    // Grab command line arguments
    std::cout << "\tParsing command line arguments...." << std::endl;
    std::string imgName     = argv[1];
    std::string dir         = argv[2];
    int count               = atoi(argv[3]);
    int tx                  = atoi(argv[4]);
    int ty                  = atoi(argv[5]);

    std::cout << "\tImage: " << imgName << "\n\tDir: " << dir << 
        "\n\tCount: " << count << "\n\ttxty: [" << tx << "," << ty << "]" << std::endl;

    // Set up output file information
    std::string imgBase     = "translate";
    std::string imgExt      = "tif";
    std::string vecBase     = "field";
    std::string vecExt      = "mha";
    char cFile[256];
    std::string format = "%s/%s-%002d.%s";

    // create output file lists
    std::cout << "\tCreating file list...." << std::endl;
    GeneratorType::StringVector imgFiles;
    GeneratorType::StringVector vecFiles;

    for (int i = 0; i < count; i++)
    {
        sprintf(cFile, format.c_str(), dir.c_str(), imgBase.c_str(), i, imgExt.c_str());
        std::string imgFile(cFile);
        imgFiles.push_back(imgFile);
        std::cout << "\tImageFile: \t" << imgFile << std::endl;

        sprintf(cFile, format.c_str(), dir.c_str(), vecBase.c_str(), i, vecExt.c_str());
        std::string vecFile(cFile);
        vecFiles.push_back(vecFile);
        std::cout << "\tVectorFile:\t" << vecFile << std::endl;
    }

    // Create pipeline objects
    std::cout << "\tCreating pipeline objects..." << std::endl;
    SourceType::Pointer source = SourceType::New();
    GeneratorType::Pointer generator = GeneratorType::New();
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(imgName.c_str());
    reader->Update();

    // Set up the regions for the deformation field source
    std::cout << "\tSetting up image regions..." << std::endl;
    SourceType::RegionType::IndexType   imgIdx;
    SourceType::RegionType::SizeType    imgSize;
    SourceType::RegionType::IndexType   tranIdx;
    SourceType::RegionType::SizeType    tranSize;

    // Use the index and size of the input image to set up deformation field source
    for (unsigned int d = 0; d < Dimension; d++)
    {
        imgIdx[d] =    reader->GetOutput()->GetLargestPossibleRegion().GetIndex()[d];
        imgSize[d] =   reader->GetOutput()->GetLargestPossibleRegion().GetSize()[d];
        tranIdx[d] =   (imgIdx[d] + 0.15 * imgSize[d]);
        tranSize[d] =  (0.3 * imgSize[d]);
    }

    SourceType::RegionType imgRegion;
    SourceType::RegionType tranRegion;
    SourceType::PointType txty;

    imgRegion.SetIndex(imgIdx);
    imgRegion.SetSize(imgSize);
    tranRegion.SetIndex(tranIdx);
    tranRegion.SetSize(tranSize);
    txty[0] = tx;
    txty[1] = ty;

    // Set deformation field source parametrs
    std::cout << "\tSetting up deformation field source..." << std::endl;
    source->SetNumberOfOutputs(count);
    source->SetMaximumTranslation(txty);
    source->SetImageRegion(imgRegion);
    source->SetTranslationRegion(tranRegion);

    // Update deformation field source
    std::cout << "\tUpdating deformation field source..." << std::endl;
    source->Update();

    // Set up warped image generator
    std::cout << "\tSetting up warped image generator..." << std::endl;
    generator->SetDeformer(source);
    generator->SetInputFile(imgName);
    generator->SetOutputImageFiles(imgFiles);
    generator->SetOutputDeformationFiles(vecFiles);

    // Run!
    std::cout << "\tRunning..." << std::endl;
    generator->Update();

    std::cout << "Done." << std::endl;
}
