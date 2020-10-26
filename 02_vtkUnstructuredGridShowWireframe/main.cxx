#include <iostream>
#include <array>
#include <vector>


#include <vtkAutoInit.h>

VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkRenderingFreeType);
VTK_MODULE_INIT(vtkInteractionStyle);

#include <vtkActor.h>
#include <vtkPoints.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkType.h>
#include <vtkCellType.h>
#include <vtkDataSetMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>
#include <vtkNamedColors.h>

using std::array;
using std::cout;
using std::endl;
using std::vector;

void doIt();

int main(int argc, char * argv[]) {
    cout << "Starting..." << endl;

    doIt();

    cout << "Done." << endl;

    return 0;
}

void doIt() {

    vtkNew<vtkNamedColors> colors;

    array<array<double, 3>, 8> pointsCoordinates = {{
        {{0, 0, 0}},
        {{1, 0, 0}},
        {{1, 1, 0}},
        {{0, 1, 0}},
        {{0, 0, 1}},
        {{1, 0, 1}},
        {{1, 1, 1}},
        {{0, 1, 1}}
    }};

    vtkNew<vtkPoints> points;

    std::vector<vtkIdType> pointIds;

    for (size_t kk = 0; kk < pointsCoordinates.size(); ++kk) {
        pointIds.push_back(points->InsertNextPoint(pointsCoordinates[kk].data()));
    }

    vtkNew<vtkUnstructuredGrid> unstructuredGrid;
    unstructuredGrid->SetPoints(points);
    unstructuredGrid->Allocate(100);
    // by construction the points are defined in the order, so that they define proper hexa element.
    unstructuredGrid->InsertNextCell(VTK_HEXAHEDRON, 8, pointIds.data());

    // Wireframe + Surface (solution is taken from here: https://public.kitware.com/pipermail/vtkusers/2004-July/025339.html )
    vtkNew<vtkDataSetMapper> mapperSurface;
    mapperSurface->SetInputData(unstructuredGrid);

    vtkNew<vtkDataSetMapper> mapperWireframe;
    mapperWireframe->SetInputData(unstructuredGrid);

    // prevent flickering
    mapperSurface->SetResolveCoincidentTopologyPolygonOffsetParameters(0,1);
    mapperSurface->SetResolveCoincidentTopologyToPolygonOffset();

    mapperSurface->ScalarVisibilityOff();


    mapperWireframe->SetResolveCoincidentTopologyPolygonOffsetParameters(1,1);
    mapperWireframe->SetResolveCoincidentTopologyToPolygonOffset();

    //
    // Same color for inside and outside edges
    // ---------------------------------------
    mapperWireframe->ScalarVisibilityOff();

    vtkNew<vtkActor> actorSurface;
    actorSurface->SetMapper(mapperSurface);
    actorSurface->GetProperty()->SetRepresentationToSurface();
    actorSurface->GetProperty()->SetColor(1, 0, 0);
    actorSurface->GetProperty()->EdgeVisibilityOn();
    actorSurface->GetProperty()->SetEdgeColor(0, 0, 1);

    vtkNew<vtkActor> actorWireframe;
    actorWireframe->SetMapper(mapperWireframe);
    actorWireframe->GetProperty()->SetRepresentationToWireframe();

    actorWireframe->GetProperty()->SetColor(0, 1, 0);
    // actorWireframe->GetProperty()->SetEdgeColor(0, 1, 0);
    // actorWireframe->GetProperty()->SetAmbient(1.0);
    // actorWireframe->GetProperty()->SetDiffuse(0.0);
    // actorWireframe->GetProperty()->SetSpecular(0.0);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actorSurface);
    // renderer->AddActor(actorWireframe);
    // renderer->SetBackground(1, 0, 0);

    vtkNew<vtkRenderWindow> window;
    window->AddRenderer(renderer);
    window->SetSize(600, 600);

    vtkNew<vtkRenderWindowInteractor> interactor;
    interactor->SetRenderWindow(window);
    interactor->Initialize();
    interactor->Start();
}
