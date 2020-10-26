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

typedef std::array<double, 3> PointCoords;
typedef std::array<size_t, 3> GridPointCoords;
typedef std::array<GridPointCoords, 8> HexNodesGridPointCoords;
typedef std::array<size_t, 8> HexNodesPointIds;

class CubeMeshGenerator {

    private:
        const size_t _xCount;
        const size_t _yCount;
        const size_t _zCount;
        vector<PointCoords> _pointsCoordinates;
        vector<HexNodesPointIds> _hexahedrons;

    public:
        CubeMeshGenerator(
            const size_t xCount,
            const size_t yCount,
            const size_t zCount
        ) 
        : _xCount(xCount),
          _yCount(yCount),
           _zCount(zCount)//,
        //   _pointsCoordinates(xCount*yCount*zCount),
        //   _hexahedrons((xCount - 1) * (yCount - 1) * (zCount - 1)) // unsafe, if one of the numbers is 0. Better do it in the body and first check the numbers.
        {
            createCubePointsCoordinates();
            createHexas();
        }

        size_t getPointsCount() {
            return _xCount * _yCount * _zCount;
        }

        size_t getHexasCount() {
            return (_xCount - 1)*(_yCount - 1)*(_zCount - 1);
        }

        const PointCoords & getPoint(const size_t pointId) {
            return _pointsCoordinates[pointId];
        }

        const HexNodesPointIds & getHex(const size_t hexId) {
            return _hexahedrons[hexId];
        }

    private:

        void createCubePointsCoordinates() {
            for (size_t kkZ = 0; kkZ < _zCount; kkZ++) {
                for (size_t kkY = 0; kkY < _yCount; kkY++) {
                    for (size_t kkX = 0; kkX < _xCount; kkX++) {
                        _pointsCoordinates.push_back(PointCoords({{(double)kkX, (double)kkY, (double)kkZ}}));
                    }
                }
            }
        }

        const size_t pointGridCoordsToPointId(size_t nx, size_t ny, size_t nz) {
            return nz * _xCount * _yCount + ny * _xCount + nx;
        }

        void createHexas() {
            // p0 (nx    , ny    , nz    )
            // p1 (nx + 1, ny    , nz    )
            // p2 (nx + 1, ny + 1, nz    )
            // p3 (nx    , ny + 1, nz    )
            // p4 (nx    , ny    , nz + 1)
            // p5 (nx + 1, ny    , nz + 1)
            // p6 (nx + 1, ny + 1, nz + 1)
            // p7 (nx    , ny + 1, nz + 1)

            for (size_t kkz = 0; kkz < _zCount - 1; ++kkz) {
                for (size_t kky = 0; kky < _yCount - 1; ++kky) {
                    for (size_t kkx = 0; kkx < _xCount - 1; ++kkx) {
                        _hexahedrons.push_back( 
                            HexNodesPointIds({{
                                pointGridCoordsToPointId(kkx    , kky    , kkz    ),
                                pointGridCoordsToPointId(kkx + 1, kky    , kkz    ),
                                pointGridCoordsToPointId(kkx + 1, kky + 1, kkz    ),
                                pointGridCoordsToPointId(kkx    , kky + 1, kkz    ),
                                pointGridCoordsToPointId(kkx    , kky    , kkz + 1),
                                pointGridCoordsToPointId(kkx + 1, kky    , kkz + 1),
                                pointGridCoordsToPointId(kkx + 1, kky + 1, kkz + 1),
                                pointGridCoordsToPointId(kkx    , kky + 1, kkz + 1)
                            }})
                        );
                    }
                }
            }

        }


};


void doIt();

int main(int argc, char * argv[]) {
    cout << "Starting..." << endl;

    doIt();

    cout << "Done." << endl;

    return 0;
}

void doIt() {

    CubeMeshGenerator meshGenerator(5, 5, 5);

    vtkNew<vtkPoints> points;

    for (size_t kk = 0; kk < meshGenerator.getPointsCount(); ++kk) {
        points->InsertPoint(kk, meshGenerator.getPoint(kk).data());
    }

    vtkNew<vtkUnstructuredGrid> unstructuredGrid;
    unstructuredGrid->SetPoints(points);
    unstructuredGrid->Allocate(meshGenerator.getHexasCount());

    vtkIdType pointIds[8] = {0};
    for (size_t kk = 0; kk < meshGenerator.getHexasCount(); ++kk) {
        const HexNodesPointIds & pointIds1 = meshGenerator.getHex(kk);
        for (size_t nn = 0; nn < 8; ++nn) {
            pointIds[nn] = (vtkIdType)pointIds1[nn];
        }
        unstructuredGrid->InsertNextCell(VTK_HEXAHEDRON, 8, pointIds);
    }

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




