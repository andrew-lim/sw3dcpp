#include "Triangle.h"
#include <glm/gtx/rotate_vector.hpp>
using namespace al::graphics;

Triangle::Triangle()
: color()
, textureID(-1)
{

}

Triangle::Triangle(const Vertex4f& a, const Vertex4f& b, const Vertex4f& c)
: color()
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
  glm::vec4 rot0 = glm::rotateX(vertices[0].pos.array<glm::vec4>(), rad);
  glm::vec4 rot1 = glm::rotateX(vertices[1].pos.array<glm::vec4>(), rad);
  glm::vec4 rot2 = glm::rotateX(vertices[2].pos.array<glm::vec4>(), rad);

  vertices[0].pos = Vector4f(rot0[0], rot0[1], rot0[2]);
  vertices[1].pos = Vector4f(rot1[0], rot1[1], rot1[2]);
  vertices[2].pos = Vector4f(rot2[0], rot2[1], rot2[2]);
}

void Triangle::rotateY(float rad)
{
  glm::vec4 rot0 = glm::rotateY(vertices[0].pos.array<glm::vec4>(), rad);
  glm::vec4 rot1 = glm::rotateY(vertices[1].pos.array<glm::vec4>(), rad);
  glm::vec4 rot2 = glm::rotateY(vertices[2].pos.array<glm::vec4>(), rad);

  vertices[0].pos = Vector4f(rot0[0], rot0[1], rot0[2]);
  vertices[1].pos = Vector4f(rot1[0], rot1[1], rot1[2]);
  vertices[2].pos = Vector4f(rot2[0], rot2[1], rot2[2]);
}

void Triangle::scale(float f)
{
  vertices[0] = vertices[0].scaleXYZ(f);
  vertices[1] = vertices[1].scaleXYZ(f);
  vertices[2] = vertices[2].scaleXYZ(f);
}