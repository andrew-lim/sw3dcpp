#include "Tests.h"
#include "Vertex.h"
#include <stdexcept>
#include <cstdio>
#include "tiny_obj_loader.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "OBJLoader.h"
#include "Grid.h"

using namespace al::graphics;
using namespace std;

#define CUBE_FILE "../res/obj/cube.obj"

void testVector4f()
{
  printf("--- testVector4f start\n");
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
  printf("h=%s\n", h.toString().c_str());

  Vector4f i(14, 13, 12, 11);
  if (h!=i) {
    throw std::runtime_error("testVector4f() failed h should equal i");
  }
  
  printf("--- testVector4f end\n\n");
}

void testVertex()
{
  printf("--- testVertex() start\n");

  // a = a + (b-a)*t
  
  Vertex va(Vector4f(0, 1, 0), Vector2f(0,0));
  Vertex vb(Vector4f(0, 0, 0), Vector2f(0,1));
  Vertex vc(Vector4f(1, 1, 0), Vector2f(1,1));
  
  printf("va.pos = %s\n", va.pos().toString().c_str());
  printf("vb.pos = %s\n", vb.pos().toString().c_str());
  
  Vertex lerpab = va.lerp(vb, 0.5);
  printf("lerpab.pos()=%s\n", lerpab.pos().toString().c_str());
  printf("lerpab.texcoords()=%s\n", lerpab.texcoords().toString().c_str());
  
  Vector4f correctPos(0, 0.5, 0);
  Vector2f correctTex(0, 0.5);
  
  if (correctPos!=lerpab.pos()) {
    throw std::runtime_error("correctPos!=lerpab.pos()");
  }
  if (correctTex!=lerpab.texcoords()) {
    throw std::runtime_error("correctTex!=lerpab.texcoords()");
  }
   
  printf("--- testVertex() end\n\n");
}

void testTinyObj()
{
  printf("--- testTinyObj() start\n");
  std::string inputfile = "../res/obj/cube.obj";
  tinyobj::ObjReaderConfig reader_config;
  reader_config.mtl_search_path = "./"; // Path to material files

  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(inputfile, reader_config)) {
    if (!reader.Error().empty()) {
        std::string msg = "testTinyObj failed(): ";
        msg += reader.Error();
        throw std::runtime_error("");
    }
    exit(1);
  }

  if (!reader.Warning().empty()) {
    std::string msg = "testTinyObj failed() with warning: ";
    msg += reader.Warning();
    throw std::runtime_error("");
  }

  auto& attrib = reader.GetAttrib();
  auto& shapes = reader.GetShapes();
  auto& materials = reader.GetMaterials();

  printf("materials.size()=%d\n", (int)shapes.size());

  // Loop over shapes
  for (size_t s = 0; s < shapes.size(); s++) {
    size_t index_offset = 0;
    for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
      // Loop over faces(polygon)
      printf("Face %d\n", (int)f);
      size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

      // Loop over vertices in the face.
      for (size_t v = 0; v < fv; v++) {
        // access to vertex
        tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
        tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
        tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
        tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];
        printf("\tvertex (%f, %f, %f)\n", vx, vy, vz);

        // Check if `normal_index` is zero or positive. negative = no normal data
        if (idx.normal_index >= 0) {
          tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
          tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
          tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];
          printf("\tvertex normal (%f, %f, %f)\n", nx, ny, nz);
        }

        // Check if `texcoord_index` is zero or positive. negative = no texcoord data
        if (idx.texcoord_index >= 0) {
          tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
          tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
          printf("\tex (%f, %f)\n", tx, ty);
        }

        // Optional: vertex colors
         tinyobj::real_t r   = attrib.colors[3*size_t(idx.vertex_index)+0];
         tinyobj::real_t g = attrib.colors[3*size_t(idx.vertex_index)+1];
         tinyobj::real_t b  = attrib.colors[3*size_t(idx.vertex_index)+2];
         printf("\trgb (%f, %f, %f)\n", r, g, b);
      }
      index_offset += fv;

      // per-face material
      const std::vector<int>& material_ids = shapes[s].mesh.material_ids;
      int material_id = (int)material_ids[f];
      if (material_id != -1) {
        const tinyobj::material_t& material = materials[material_id];
        printf("face %d\tmaterial_id=%d\tname=%s\n", (int)f, material_id, material.name.c_str() ) ;
      }
    }
  }
  printf("--- testTinyObj() end\n\n");

}
void testOBJLoader()
{
  printf("--- testOBJLoader() start\n");
  al::graphics::OBJLoader objLoader(true);
  vector<Triangle> triangles;
  objLoader.load(CUBE_FILE);
  if (objLoader.triangles.size() != 12) {
    string msg = "Expected 12 triangle faces in cube.obj, instead found ";
    msg += std::to_string((int)objLoader.triangles.size());
    printf("triangles.size() = %d\n", (int)objLoader.triangles.size());
    throw std::runtime_error(msg.c_str());
  }
  printf("--- testOBJLoader() end\n\n");
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
  printf("a=%f, b=%f\n", a, b);
  if (a != 0.0f) {
    throw runtime_error("a should be 0.0f");
  }
  if (b != 0.5f) {
    throw runtime_error("b should be 0.5f");
  }
  printf("--- testfmod() end\n\n");
}

void Tests::run()
{
  testVector4f();
  testVertex();
  testTinyObj();
  testOBJLoader();
  testGrid();
  testfmod();
}