
#include <string>

#include "vtkActor.h"
#include "vtkConeSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"

int main(int argc, char** argv)
{
    vtkRenderWindow* window = vtkRenderWindow::New();
    vtkRenderer* render = vtkRenderer::New();
    window->AddRenderer(render);
    vtkRenderWindowInteractor* rwi = vtkRenderWindowInteractor::New();
    rwi->SetRenderWindow(window);
    
    vtkConeSource* cone = vtkConeSource::New();
    cone->SetResolution(20);
    
    vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
    mapper->SetInput(cone->GetOutput());
    
    vtkActor* actor = vtkActor::New();
    actor->SetMapper(mapper);
    
    actor->GetProperty()->SetAmbient(0.1);
    actor->GetProperty()->SetDiffuse(0.7);
    actor->GetProperty()->SetSpecular(0.3);
    actor->GetProperty()->SetSpecularPower(20.0);
    actor->GetProperty()->SetColor(0.8, 0.2, 0.2);
    // actor->GetProperty()->SetOpacity(0.80);
    
    render->AddActor(actor);
    window->Render();
    rwi->Start();
}
