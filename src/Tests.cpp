#include "Tests.h"
#include "Vertex.h"
#include <stdexcept>
#include <cstdio>

using namespace al::graphics;

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
  
  printf("--- testVector4f end\n");
}

void testVertex()
{
  printf("--- testVertex() start\n");
  
  // a = a + (b-a)*t
  
  Vertex va(Vector4f(0, 1, 0), Vector4f(0,0));
  Vertex vb(Vector4f(0, 0, 0), Vector4f(0,1));
  Vertex vc(Vector4f(1, 1, 0), Vector4f(1,1));
  
  printf("va.pos = %s\n", va.pos().toString().c_str());
  printf("vb.pos = %s\n", vb.pos().toString().c_str());
  
  Vertex lerpab = va.lerp(vb, 0.5);
  printf("lerpab.pos()=%s\n", lerpab.pos().toString().c_str());
  printf("lerpab.texcoords()=%s\n", lerpab.texcoords().toString().c_str());
  
  Vector4f correctPos(0, 0.5, 0);
  Vector4f correctTex(0, 0.5);
  
  if (correctPos!=lerpab.pos()) {
    throw std::runtime_error("correctPos!=lerpab.pos()");
  }
  if (correctTex!=lerpab.texcoords()) {
    throw std::runtime_error("correctTex!=lerpab.texcoords()");
  }
   
  printf("--- testVertex() end\n");
}

void Tests::run()
{
  testVector4f();
  testVertex();
}