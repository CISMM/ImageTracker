#pragma once

#include <string>
#include <vector>

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageSource.h"
#include "itkObject.h"
#include "itkWarpImageFilter.h"

template < class TInputImage, class TOutputImage, class TDeformationField >
class WarpedImageGenerator : 
    public itk::Object
{
public:
    // standard itk typedefs
    typedef WarpedImageGenerator Self;
    typedef itk::Object Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;

    itkNewMacro(Self);
    itkTypeMacro(WarpedImageGenerator, itk::Object);

    // useful typedefs
    typedef TInputImage         InputImageType;
    typedef TOutputImage        OutputImageType;
    typedef TDeformationField   DeformationType;

    typedef itk::ImageFileReader< InputImageType >      ReaderType;
    typedef itk::ImageSource< DeformationType >         DeformationSourceType;
    typedef itk::WarpImageFilter< InputImageType, OutputImageType, DeformationType > WarpType;
    typedef itk::ImageFileWriter< DeformationType >    DeformationWriterType;
    typedef itk::ImageFileWriter< OutputImageType >     ImageWriterType;
    typedef std::vector< std::string >                  StringVector;

    // Getters/Setters
    itkGetMacro(InputFile, std::string);
    itkSetMacro(InputFile, std::string);

    StringVector GetOutputImageFiles() { return this->m_OutputImageFiles; }
    void SetOutputImageFiles(const StringVector& files) { this->m_OutputImageFiles = files; }

    StringVector GetOutputDeformationFiles() { return this->m_OutputDeformationFiles; }
    void SetOutputDeformationFiles(const StringVector& files) { this->m_OutputDeformationFiles = files; }

    void SetDeformer(DeformationSourceType * deformer)
    {
        this->m_Deformer = deformer;
    }

    DeformationSourceType* GetDeformer()
    {
        return this->m_Deformer;
    }

    /**
     * Run the image generation, saving image files and deformation fields.
     */
    void Update();

protected:
    WarpedImageGenerator() {}
    virtual ~WarpedImageGenerator() {}

private:
    WarpedImageGenerator(const Self&);  // not implemented
    void operator=(const Self&); // not implemented

    std::string     m_InputFile;
    StringVector    m_OutputImageFiles;
    StringVector    m_OutputDeformationFiles;
    typename DeformationSourceType::Pointer   m_Deformer;
};

/** Implementation **/

template <class TInputImage, class TOutputImage, class TDeformationField >
void WarpedImageGenerator<TInputImage, TOutputImage, TDeformationField>::
Update()
{
    std::cout << "WarpedImageGenerator::Update()" << std::endl;
    typename ReaderType::Pointer reader = ReaderType::New();
    typename WarpType::Pointer warper = WarpType::New();
    typename DeformationWriterType::Pointer deformWriter = DeformationWriterType::New();
    typename ImageWriterType::Pointer imageWriter = ImageWriterType::New();

    reader->SetFileName(this->GetInputFile().c_str());
    warper->SetInput(reader->GetOutput());
    imageWriter->SetInput(warper->GetOutput());

    // We use an integer index into the filename vectors because it is easier to
    // code...we need the integer index for the deformation field source.
    for (unsigned int i = 0; 
            (i < this->GetDeformer()->GetNumberOfOutputs() &&
             i < this->GetOutputImageFiles().size() &&
             i < this->GetOutputDeformationFiles().size());
            i ++)
    {
        std::cout << "\tImage file[" << i << "]: \t" << this->GetOutputImageFiles()[i] << 
                   "\n\tVector file[" << i << "]:\t" << this->GetOutputDeformationFiles()[i] << std::endl;
        warper->SetDeformationField(this->GetDeformer()->GetOutput(i));
        deformWriter->SetInput(this->GetDeformer()->GetOutput(i));
        deformWriter->SetFileName(this->GetOutputDeformationFiles()[i].c_str());
        imageWriter->SetFileName(this->GetOutputImageFiles()[i].c_str());

        deformWriter->Update();
        imageWriter->Update();
    }
}
