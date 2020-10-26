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

    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputData(unstructuredGrid);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);

    vtkNew<vtkRenderWindow> window;
    window->AddRenderer(renderer);
    window->SetSize(600, 600);

    vtkNew<vtkRenderWindowInteractor> interactor;
    interactor->SetRenderWindow(window);
    interactor->Initialize();
    interactor->Start();
}
