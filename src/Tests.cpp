#include "Tests.h"
#include "Vertex.h"
#include "Vector4f.h"
#include <stdexcept>
#include <cstdio>
#include "tiny_obj_loader.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "OBJLoader.h"
#include "Grid.h"
#include "Vector3f.h"
#include "Graphics3D.h"

using namespace al::graphics;
using namespace std;

#define CUBE_FILE "../res/obj/cube.obj"

void testVector4f()
{
  printf("--- testVector4f() start\n");
  Vector4f a(4, 3, 2, 1);
  if (a.x!=4 || a.y!=3 || a.z!=2 || a.w!=1) {
    throw std::runtime_error("testVector4f() failed a is wrong");
  }
  Vector4f b = a.mul(4);
  Vector4f c = b.div(2);
  if (c.x!=8 || c.y!=6 || c.z!=4 || c.w!=2) {
    throw std::runtime_error("testVector4f() failed C is wrong");
  }
  Vector4f d = Vector4f(4, 3, 2, 1);
  Vector4f e = d.mul(Vector4f(4, 4, 4, 4));
  Vector4f f = e.div(Vector4f(2, 2, 2, 2));
  if (f.x!=8 || f.y!=6 || f.z!=4 || f.w!=2) {
    throw std::runtime_error("testVector4f() failed e is wrong");
  }
  Vector4f g(8, 6, 4, 2);
  if (f != g) {
    throw std::runtime_error("testVector4f() f==g failed");
  }

  Vector4f h = g.lerp(Vector4f(20, 20, 20, 20), 0.5);

  Vector4f i(14, 13, 12, 11);
  if (h!=i) {
    throw std::runtime_error("testVector4f() failed h should equal i");
  }

  printf("--- testVector4f() end\n\n");
}

void testVertex4f()
{
  printf("--- testVertex4f() start\n");

  // a = a + (b-a)*t

  Vertex4f va(Vector4f(0, 1, 0), Vector2f(0,0));
  Vertex4f vb(Vector4f(0, 0, 0), Vector2f(0,1));
  Vertex4f vc(Vector4f(1, 1, 0), Vector2f(1,1));

  Vertex4f lerpab = va.lerp(vb, 0.5);

  Vector4f correctPos(0, 0.5, 0);
  Vector2f correctTex(0, 0.5);

  if (correctPos!=lerpab.pos) {
    throw std::runtime_error("correctPos!=lerpab.pos()");
  }
  if (correctTex!=lerpab.uv) {
    throw std::runtime_error("correctTex!=lerpab.texcoords()");
  }

  printf("--- testVertex4f() end\n\n");
}

void testGrid()
{
  printf("--- testGrid() start\n");
  Grid<float> grid(4, 3);
  grid.fill(0.8f);
  if (grid(1,2)!=0.8f) {
    printf("grid(1,2)=%f\n", grid(1,2));
    throw std::runtime_error("grid(1,2) should be 0.8");
  }
  grid.fill(0.3f);
  if (grid(3,2)!=0.3f) {
    printf("grid(3,2)=%f\n", grid(3,2));
    throw std::runtime_error("grid(3,2) should be 0.3");
  }
  printf("--- testGrid() end\n\n");
}

void testfmod()
{
  printf("--- testfmod() start\n");
  float a = fmod(2.0f, 1.0f);
  float b = fmod(1.5f, 1.0f);
  if (a != 0.0f) {
    throw runtime_error("a should be 0.0f");
  }
  if (b != 0.5f) {
    throw runtime_error("b should be 0.5f");
  }
  printf("--- testfmod() end\n\n");
}

void testClipLineToRect()
{
  printf("--- testClipLineToRect() start\n");
  int screenWidth = 320;
  int screenHeight = 240;
  int xmin=0;
  int ymin=0;
  int xmax=screenWidth-1;
  int ymax=screenHeight-1;
  int a[2] = {-10, 10};
  int b[2] = {300, 250};

  if (Graphics3D::clipLineToRect(a, b, xmin, ymin, xmax, ymax)) {
    bool correct = a[0] == 0  &&
                   a[1] == 17 &&
                   b[0] == 285 &&
                   b[1] == 239;
    if (!correct) {
      printf("Before clipping:\n\ta={%d, %d}\n\tb=[%d, %d]\n",
             a[0], a[1], b[0], b[1]);
      printf("After clipping:\n\ta={%d, %d}\n\tb=[%d, %d]\n",
           a[0], a[1], b[0], b[1]);
      throw runtime_error("clipLineToRect returned unexpected values");
    }

  }
  else {
    throw runtime_error("clipLineToRect returned false");
  }
  printf("--- testClipLineToRect() end\n\n");
}

void Tests::run()
{
  std::cout << "__cplusplus=" << __cplusplus << std::endl;
  testVector4f();
  testVertex4f();
  testGrid();
  testfmod();
  testClipLineToRect();
}