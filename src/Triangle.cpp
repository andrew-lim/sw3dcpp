#include "Triangle.h"
#include "Graphics3D.h"

using namespace al::graphics;

Triangle::Triangle()
: color(0)
, textureID(-1)
{
}

Triangle::Triangle(const Vertex4f& a, const Vertex4f& b, const Vertex4f& c)
: color(0)
, textureID(-1)
{
  vertices[0] = a;
  vertices[1] = b;
  vertices[2] = c;
}

Triangle::Triangle(float x1, float y1, float z1,
                   float x2, float y2, float z2,
                   float x3, float y3, float z3,
                   std::uint32_t color)
: color(color)
, textureID(-1)
{
  vertices[0].pos = Vector4f(x1, y1, z1);
  vertices[1].pos = Vector4f(x2, y2, z2);
  vertices[2].pos = Vector4f(x3, y3, z3);
}

void Triangle::setTexUVs(float u1, float v1,
                         float u2, float v2,
                         float u3, float v3)
{
  vertices[0].uv = Vector2f(u1, v1);
  vertices[1].uv = Vector2f(u2, v2);
  vertices[2].uv = Vector2f(u3, v3);
}

void Triangle::rotateX(float rad)
{
  vertices[0].pos = vertices[0].pos.rotateX(rad);
  vertices[1].pos = vertices[1].pos.rotateX(rad);
  vertices[2].pos = vertices[2].pos.rotateX(rad);
}

void Triangle::rotateY(float rad)
{
  vertices[0].pos = vertices[0].pos.rotateY(rad);
  vertices[1].pos = vertices[1].pos.rotateY(rad);
  vertices[2].pos = vertices[2].pos.rotateY(rad);
}

void Triangle::scale(float f)
{
  const Vector4f scaleVector = Vector4f(f ,f, f, 1);
  vertices[0].pos = vertices[0].pos.mul( scaleVector );
  vertices[1].pos = vertices[1].pos.mul( scaleVector );
  vertices[2].pos = vertices[2].pos.mul( scaleVector );
}

void Triangle::perspectiveDivide()
{
  G3D::perspectiveDivide(vertices[0].pos);
  G3D::perspectiveDivide(vertices[1].pos);
  G3D::perspectiveDivide(vertices[2].pos);
}