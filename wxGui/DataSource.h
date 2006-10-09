#pragma once

#include <string>
#include "FileSet.h"
#include "ImageSetReader.h"

#include "itkImage.h"
#include "itkVTKImageExport.h"

#include "vtkImageActor.h"
#include "vtkImageFlip.h"
#include "vtkImageImport.h"
#include "vtkRenderer.h"

class DataSource
{
public:
    DataSource(const std::string& newName = "New DataSource") : 
        name(newName),
        files(),
        index(0)
    {
    }
    
    virtual ~DataSource() {}
    
    const std::string& GetName()
    { return this->name; }
    
    void SetName(const std::string& name)
    { this->name = name; }
    
    unsigned int GetIndex()
    { return this->index; }
    
    void SetIndex(unsigned int idx)
    { this->index = idx; }
    
    const FileSet& GetFiles()
    { return this->files; }
    
    void SetFiles(const FileSet& files)
    { this->files = files; }
    
    virtual const vtkRenderer* GetRenderer();
    
protected:
    
private:
    std::string name;
    FileSet files;
    unsigned int index;
};

class ScalarDataSource 
    : public DataSource
{
public:
    
    typedef ScalarDataSource Self;
    typedef DataSource Superclass;
    typedef itk::Image< unsigned char, 2 > ImageType;
    typedef itk::VTKImageExport< ImageType > ExportType;
    
    ScalarDataSource() : DataSource()
    {
        this->itkexp = ExportType::New();
        this->import = vtkImageImport::New();
        this->flip = vtkImageFlip::New();
        this->actor = vtkImageActor::New();
        this->renderer = vtkRenderer::New();
        
        this->flip->SetInputConnection(this->import->GetOutputPort());
        this->actor->SetInputConnection(this->flip->GetOutputPort());
        this->renderer->AddActor(this->actor);
    }
    
    virtual ~ScalarDataSource()
    {
        this->renderer->Delete();
    }
    
    virtual const vtkRenderer* GetRenderer()
    { return this->renderer; }
    
    virtual void SetIndex(unsigned int idx)
    {
        Superclass::SetIndex(idx);
        if (idx < this->images.size())
        {
            this->itkexp->SetInput(this->images[idx]);
            this->itkexp->Update();
            ConnectITKToVTK<ImageType>(this->itkexp, this->import);
            this->import->UpdateWholeExtent();
            this->flip->UpdateWholeExtent();
            this->actor->UpdateWholeExtent();
        }
    }
    
protected:
    
private:
    ImageSetReader< ImageType > images(this->GetFiles());
    ExportType::Pointer itkexp;
    vtkImageImport* import;
    vtkImageFlip* flip;
    vtkImageActor* actor;
    vtkRenderer* renderer;
};