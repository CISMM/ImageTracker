#include "DataSource.h"

#include <typeinfo>

#include "Logger.h"

DataSource::DataSource() :
    name("NewDataSource"),
    files()
{
    // create a default video
    video = new ImageSetReaderUS2();
}

DataSource::~DataSource()
{
    // We have to destroy our video
    if (this->video != NULL)
    {
        delete(this->video);
    }
}

itk::DataObject* DataSource::GetImage(unsigned int i)
{
     Logger::verbose << "DataSource::GetImage(" << i << ") const" << std::endl;
    if (this->video->size() == 0)
    {
        Logger::warning << "DataSource::GetImage(): no images; video->size() = " << this->video->size() << std::endl;
        return NULL;
    }
    else
    {
        unsigned int idx = std::min((int) i, this->video->size()-1);
        return this->video->GetImage(idx);
    }
}

const itk::DataObject* DataSource::GetImage(unsigned int i) const
{
    Logger::verbose << "DataSource::GetImage(" << i << ") const" << std::endl;
    if (this->video->size() == 0)
    {
        Logger::warning << "DataSource::GetImage(): no images; video->size() = " << this->video->size() << std::endl;
        return NULL;
    }
    else
    {
        unsigned int idx = std::min((int) i, this->size()-1);
        return const_cast<const itk::DataObject* >(this->video->GetImage(idx));
    }
}

void DataSource::SetPixelDataType(PixelDataType type)
{
    Logger::verbose << "DataSource::SetPixelDataType: type => " << type << std::endl;
    if (type != this->GetPixelDataType())
    {
        if (this->video != 0)
        {
            delete(this->video);
            this->video = 0;
        }
        switch (type)
        {
            case ScalarChar:
                Logger::verbose << "DataSource: setting ImageSetReader for ScalarChar pixels" << std::endl;
                video = new ImageSetReaderUC2(this->files);
                break;
            case ScalarShort:
                Logger::verbose << "DataSource: setting ImageSetReader for ScalarShort pixels" << std::endl;
                video = new ImageSetReaderUS2(this->files);
                break;
            case ScalarFloat:
                Logger::verbose << "DataSource: setting ImageSetReader for ScalarFloat pixels" << std::endl;
                video = new ImageSetReaderF2(this->files);
                break;
            case VectorFloat:
                Logger::verbose << "DataSource: setting ImageSetReader for VectorFloat pixels" << std::endl;
                video = new ImageSetReaderV2F2(this->files);
                break;
        }
    }
}

PixelDataType DataSource::GetPixelDataType()
{
    return GetInputPixelDataType(this->video);
}

PixelDataType GetInputPixelDataType(ImageSetReaderBase* reader)
{
    ImageSetReaderUC2* pUC2;
    ImageSetReaderUS2* pUS2;
    ImageSetReaderF2* pF2;
    ImageSetReaderV2F2* pV2F2;
    
    if (( pUC2 = dynamic_cast<ImageSetReaderUC2*>(reader)) != NULL)
    {
        Logger::verbose << "GetInputPixelDataType: ImageSetReader reads ScalarChar pixels" << std::endl;
        return ScalarChar;
    }
    else if (( pUS2 = dynamic_cast<ImageSetReaderUS2*>(reader)) != NULL)
    {
        Logger::verbose << "GetInputPixelDataType: ImageSetReader reads ScalarShort pixels" << std::endl;
        return ScalarShort;
    }
    else if (( pF2 = dynamic_cast<ImageSetReaderF2*>(reader)) != NULL)
    {
        Logger::verbose << "GetInputPixelDataType: ImageSetReader reads ScalarFloat pixels" << std::endl;
        return ScalarFloat;
    }
    else if (( pV2F2 = dynamic_cast<ImageSetReaderV2F2*>(reader)) != NULL)
    {
        Logger::verbose << "GetInputPixelDataType: ImageSetReader reads VectorFloat pixels" << std::endl;
        return VectorFloat;
    }
    
    Logger::warning << "GetInputPixelDataType: Did not recognize type of ImageSetReader: " << std::endl;
    return ScalarChar;    // default
}
