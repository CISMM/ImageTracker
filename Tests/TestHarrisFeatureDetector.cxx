#include "./TestHarrisFeatureDetector.h"
#include "itkImageFileReader.h"
#include "../CommonTypes.h"
#include "../HarrisFeatureDetector.h"
#include "../Logger.h"

TestHarrisFeatureDetector::TestHarrisFeatureDetector(void)
{
}

TestHarrisFeatureDetector::~TestHarrisFeatureDetector(void)
{
}

void TestHarrisFeatureDetector::run()
{
    Logger::logInfo("Running TestHarrisFeatureDetector");
    this->testDetector();
}

void TestHarrisFeatureDetector::testDetector()
{
    typedef itk::ImageFileReader<CommonTypes::InternalImageType> ReaderType;
    std::string file = "C:\\UNC_CS\\Research\\Nano\\data\\Fibrin\\lolo-2004-05-14-2narea\\mani420.png";

    Logger::logDebug("Loading image: " + file);
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(file.c_str());
    reader->Update();

    Logger::logDebug("Finding features in image....");
    HarrisFeatureDetector* detector = new HarrisFeatureDetector();
    int scales = 2;
    double scale = 1.0;
    int count = 500;
    for (int i = 0; i < scales; i++)
    {
        if (i > 0) scale = scale * 1.4;
        HarrisFeatureDetector::PointSetType::Pointer features = 
            detector->findFeatures(reader->GetOutput(), count, scale);
        test_(features->GetNumberOfPoints() <= count);
    }
}