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
  Pos pos;
  UV uv;
  Vector3f rgb;
  VertexG(T x, T y) : pos(x, y) {}
  VertexG(const Pos& pos = Pos(),
          const UV&  uv  = UV(),
          const Vector3f& rgb = Vector3f())
  : pos(pos), uv(uv), rgb(rgb)
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
  VertexG add(T r) const { return VertexG(pos.add(r), uv.add(r), rgb.add(r) ); }
  VertexG sub(T r) const { return VertexG(pos.sub(r), uv.sub(r), rgb.sub(r) ); }
  VertexG mul(T r) const { return VertexG(pos.mul(r), uv.mul(r), rgb.mul(r) ); }
  VertexG div(T r) const { return VertexG(pos.div(r), uv.div(r), rgb.div(r) ); }

  VertexG lerp(const VertexG& other, T t) const
  {
    return VertexG(
      pos.lerp(other.pos, t),
      uv.lerp(other.uv, t),
      rgb.lerp(other.rgb, t)
    );
  }

  VertexG add(const VertexG& r) const
  {
    return VertexG(pos.add(r.pos), uv.add(r.uv), rgb.add(r.rgb));
  }

  VertexG sub(const VertexG& r) const
  {
    return VertexG(pos.sub(r.pos), uv.sub(r.uv), rgb.sub(r.rgb));
  }

  VertexG mul(const VertexG& r) const
  {
    return VertexG(pos.mul(r.pos),uv.mul(r.uv), rgb.mul(r.rgb));
  }

  VertexG div(const VertexG& r) const
  {
    return VertexG(pos.div(r.pos), uv.div(r.uv), rgb.div(r.rgb));
  }
}; // class Pos

typedef VertexG<float, Vector4f, Vector2f> Vertex4f;
typedef VertexG<float, Vector2f, Vector2f> Vertex2f;
typedef VertexG<float, Vector3f, Vector2f> Vertex3f;

} // namespace al
} // namespace graphics


#endif
