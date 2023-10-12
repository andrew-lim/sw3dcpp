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
#include "Matrix.h"

using namespace al::graphics;
using namespace std;

#define CUBE_FILE "../res/obj/cube.obj"

static void printVector4i(const Vector4i v, const char* name)
{
   printf("%s\t=(%d, %d, %d, %d)\n", name, v[0], v[1], v[2], v[3]);
}


void testVector4f()
{
  printf("--- testVector4f() start\n");
  Vector4f a(4, 3, 2, 1);
  if (a.x()!=4 || a.y()!=3 || a.z()!=2 || a.w()!=1) {
    throw std::runtime_error("testVector4f() failed a is wrong");
  }
  Vector4f b = a.mul(4);
  Vector4f c = b.div(2);
  if (c.x()!=8 || c.y()!=6 || c.z()!=4 || c.w()!=2) {
    throw std::runtime_error("testVector4f() failed C is wrong");
  }
  Vector4f d = Vector4f(4, 3, 2, 1);
  Vector4f e = d.mul(Vector4f(4, 4, 4, 4));
  Vector4f f = e.div(Vector4f(2, 2, 2, 2));
  if (f.x()!=8 || f.y()!=6 || f.z()!=4 || f.w()!=2) {
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

void testCrossProduct()
{
  printf("--- testCrossProduct() start\n");
  Vector4f a(2, 3, 4);
  Vector4f b(5, 6, 7);
  Vector4f c = a.cross(b);
  printf("c = (%f, %f, %f, %f)\n", c.x(), c.y(), c.z(), c.w());
  if (c.x()!=-3 || c.y()!=6 || c.z()!=-3) {
    throw runtime_error("Vector4f c is not (-3,6,-3)");
  }
  Vector4f d = G3D::crossProduct(a, b);
  if (d.x()!=-3 || d.y()!=6 || d.z()!=-3) {
    throw runtime_error("Vector4f d is not (-3,6,-3)");
  }

  float dot = G3D::dot(a,b);
  printf("dot(a,b)=%f\n", dot);
  float length = G3D::length(c);
  printf("length(c)=%f\n", length);

  G3D::normalize(d);
  printf("normalize(d) = (%f, %f, %f, %f)\n", d.x(), d.y(), d.z(), d.w());
  printf("length(d) after normalized=%f\n", G3D::length(d));

  printf("--- testCrossProduct() end\n\n");
}

static void printMatrix(const Matrix4f& m, const char* name)
{
  printf("--- Matrix4f START ---\n");
  printf("    Name: %s\n", name);
  for (int row=0; row<4; row++) {
    for (int col=0; col<4; col++) {
      printf("%.02f ", m(col, row));
    }
    printf("\n");
  }
  printf("--- Matrix4f END  ---\n");
}

static void printMatrix(const Matrix4i& m, const char* name)
{
  printf("--- Matrix4f START ---\n");
  printf("    Name: %s\n", name);
  for (int row=0; row<4; row++) {
    for (int col=0; col<4; col++) {
      printf("%3d ", m(col, row));
    }
    printf("\n");
  }
  printf("--- Matrix4f END  ---\n");
}

void testMatrix()
{
  printf("--- testMatrix() start\n");

/*
    Version 1 - Vertex as Column Vector
     ----------------------------------
    Translate   Scale       Vertex     New Vertex
    1 0 0 4     3 0 0 0     2          10
    0 1 0 0  x  0 3 0 0  x  2       =  6
    0 0 1 0     0 0 3 0     0          0
    0 0 0 1     0 0 0 1     1          1

    World
    3 0 0 4
    0 3 0 0
    0 0 3 0
    0 0 0 1
*/
  Matrix4f translate;
  translate.set(3, 0, 4);
  printMatrix(translate, "translate");

  Matrix4f scale;
  scale.set(0, 0, 3);
  scale.set(1, 1, 3);
  scale.set(2, 2, 3);
  printMatrix(scale, "scale");

  if (translate.get(3,0) != 4) {
    printf("translate.get(3,0)=%f\n", translate.get(3,0));
    throw runtime_error("translate.get(3,0) should be 4");
  }

  if (translate.get(12) != 4) {
    printf("translate.get(12)=%f\n", translate.get(13));
    throw runtime_error("translate.get(12) should be 4");
  }

  Matrix4f world = translate.mul(scale);
  printMatrix(world, "world");

  if (world.get(3,0) != 4) {
    printf("world.get(3,0)=%f\n", world.get(3,0));
    throw runtime_error("world.get(3,0) should be 4");
  }

  if (world.get(12) != 4) {
    printf("world.get(12)=%f\n", world.get(13));
    throw runtime_error("world.get(12) should be 4");
  }

  Matrix4f translate2 = Matrix4f::makeTranslation(4, 0, 0);
  printMatrix(translate2, "translate2");

  Matrix4f rotX = Matrix4f::makeXRotation(G3D::deg2rad(90));
  printMatrix(rotX, "rotX");

  Matrix4f rotY = Matrix4f::makeYRotation(G3D::deg2rad(90));
  printMatrix(rotY, "rotY");

  Vector4f vertex(2, 2, 0, 1);
  Vector4f result = world.mul(vertex);
  printf("result=(%f, %f, %f, %f)\n", result[0], result[1], result[2], result[3]);
  if (result[0]!=10 || result[1]!=6 || result[2]!=0 || result[3]!=1) {
    throw runtime_error("result wrong");
  }


  // Matrix A         Matrix B           A x B
  // 5  7  9  10       3  10  12 18      210  267  236  271
  // 2  3  3   8      12   1   4  9       93  149  104  149
  // 8 10  2   3       9  10  12  2      171  146  172  268
  // 3  3  4   8       3  12   4 10      105  169  128  169

  Matrix4i a;
  a.fill(5, 2, 8, 3, 7, 3, 10, 3, 9, 3, 2, 4, 10, 8, 3, 8);
  Matrix4i b(3, 12, 9, 3, 10, 1, 10, 12, 12, 4, 12, 4, 18, 9, 2, 10);
  Matrix4i axb = a.mul(b);
  Matrix4i axb2;
  axb2(0,0) = 210;  axb2(1,0) = 267; axb2(2,0) = 236; axb2(3,0) = 271;
  axb2(0,1) =  93;  axb2(1,1) = 149; axb2(2,1) = 104; axb2(3,1) = 149;
  axb2(0,2) = 171;  axb2(1,2) = 146; axb2(2,2) = 172; axb2(3,2) = 268;
  axb2(0,3) = 105;  axb2(1,3) = 169; axb2(2,3) = 128; axb2(3,3) = 169;

  if (!axb.equals(axb2)) {
    printMatrix(a, "a");
    printMatrix(b, "b");
    printMatrix(axb, "axb");
    throw std::runtime_error("axb != abx2");
  }

  // 172, 98, 92, 106
  Vector4i axv = a.mul(Vector4i(2, 4, 6, 8));
  if (axv!=Vector4i(172, 98, 92, 106)) {
    printVector4i(axv, "axv");
    throw std::runtime_error("axv != (172, 98, 92, 106)");
  }

  if (a[2][2] != 2) { throw std::runtime_error("a[2][2] != 2"); }
  if (b[3][1] != 9) { throw std::runtime_error("b[3][1] != 9"); }
  if (b[0][3] != 3) { throw std::runtime_error("b[0][3] != 3"); }

  printf("--- testMatrix() end\n\n");

}

void Tests::run()
{
  std::cout << "__cplusplus=" << __cplusplus << std::endl;
  testVector4f();
  testVertex4f();
  testGrid();
  testfmod();
  testClipLineToRect();
  testCrossProduct();
  testMatrix();
}