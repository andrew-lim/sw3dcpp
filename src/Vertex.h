#ifndef ANDREW_LIM_VERTEX_H
#define ANDREW_LIM_VERTEX_H

#include "Vector2f.h"
#include "Vector3f.h"
#include "Vector4f.h"

namespace al { namespace graphics {

template <class T, class Pos, class UV>
class VertexG
{
public:
  int id = -1;
  Pos pos;
  UV uv;
  Vector3f rgb;
  Vector3f normal;
  VertexG(T x, T y)
  : id(-1), pos(x, y)
  {}
  VertexG(const Pos& pos = Pos(),
          const UV&  uv  = UV(),
          const Vector3f& rgb = Vector3f(),
          const Vector3f& normal = Vector3f())
  : id(-1), pos(pos), uv(uv), rgb(rgb), normal(normal)
  {}
  T get(int index) const { return pos.get(index); }
  T x() const { return pos.x(); }
  T y() const { return pos.y(); }
  T z() const { return pos.z(); }
  T w() const { return pos.w(); }
  T u() const { return uv.u(); }
  T v() const { return uv.v(); }
  VertexG operator+(const VertexG& r) const { return add(r); }
  VertexG operator+(T r) const { return add(r); }
  VertexG operator-(const VertexG& r) const { return sub(r); }
  VertexG operator-(T r) const { return sub(r); }
  VertexG operator/(const VertexG& r) const { return div(r); }
  VertexG operator/(T r) const { return div(r); }
  VertexG operator*(const VertexG& r) const { return mul(r); }
  VertexG operator*(T r) const { return mul(r); }
  VertexG add(T r) const { return VertexG(pos.add(r), uv.add(r), rgb.add(r), normal.add(r)); }
  VertexG sub(T r) const { return VertexG(pos.sub(r), uv.sub(r), rgb.sub(r), normal.sub(r)); }
  VertexG mul(T r) const { return VertexG(pos.mul(r), uv.mul(r), rgb.mul(r), normal.mul(r)); }
  VertexG div(T r) const { return VertexG(pos.div(r), uv.div(r), rgb.div(r), normal.div(r)); }

  VertexG lerp(const VertexG& other, T t) const
  {
    return VertexG(
      pos.lerp(other.pos, t),
      uv.lerp(other.uv, t),
      rgb.lerp(other.rgb, t),
      normal.lerp(other.normal, t)
    );
  }

  VertexG add(const VertexG& r) const
  {
    return VertexG(pos.add(r.pos), uv.add(r.uv), rgb.add(r.rgb), normal.add(r.normal));
  }

  VertexG sub(const VertexG& r) const
  {
    return VertexG(pos.sub(r.pos), uv.sub(r.uv), rgb.sub(r.rgb), normal.sub(r.normal));
  }

  VertexG mul(const VertexG& r) const
  {
    return VertexG(pos.mul(r.pos), uv.mul(r.uv), rgb.mul(r.rgb), normal.mul(r.normal));
  }

  VertexG div(const VertexG& r) const
  {
    return VertexG(pos.div(r.pos), uv.div(r.uv), rgb.div(r.rgb), normal.div(r.normal));
  }
}; // class Pos

typedef VertexG<float, Vector4f, Vector2f> Vertex4f;
typedef VertexG<float, Vector2f, Vector2f> Vertex2f;
typedef VertexG<float, Vector3f, Vector2f> Vertex3f;

} // namespace al
} // namespace graphics


#endif
