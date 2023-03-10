#include "ClipSpace.h"
#include <vector>
using namespace al::graphics;
using namespace std;

float ClipSpace::signedDistanceRatio(const Vertex4f& src,
                                     const Vertex4f& dst,
                                     int ixyz,
                                     int planeSign)
{
  const float d1 = (src.get(ixyz) - src.w()*planeSign);
  const float d2 = (dst.get(ixyz) - dst.w()*planeSign);
  return d1/(d1-d2);
}

vector<Triangle> ClipSpace::clip(const Triangle& triangle,
                                 int ixyz,
                                 int planeSign)
{
  vector<Triangle> triangles;

  vector<Vertex4f> insidePoints;
  vector<Vertex4f> outsidePoints;
  vector<int> insideIndices;
  vector<int> outsideIndices;

  for (int i=0; i<3; ++i) {
    const Vertex4f& pt = triangle.vertex(i);
    const float xyz = pt.get(ixyz);
    const float w = pt.w();
    const bool outside = (planeSign<0 && xyz<-w) || (planeSign>0 && xyz>w);
    if (outside) {
      outsidePoints.push_back(pt);
      outsideIndices.push_back(i);
    }
    else {
      insidePoints.push_back(pt);
      insideIndices.push_back(i);
    }
  }

  if (3==outsidePoints.size()) {
    // Triangle is outside this plane
  }

  else if (3==insidePoints.size()) {
    // Triangle is completely inside this plane
    triangles.push_back(triangle);
  }

  // 2 points outside, create a smaller triangle
  else if (2==outsidePoints.size() && 1==insidePoints.size()) {
    Vertex4f& a = insidePoints[0];
    Vertex4f& b = outsidePoints[0];
    Vertex4f& c = outsidePoints[1];
    int ai = insideIndices[0];
    int bi = outsideIndices[0];
    float bt = ClipSpace::signedDistanceRatio(b, a, ixyz, planeSign);
    float ct = ClipSpace::signedDistanceRatio(c, a, ixyz, planeSign);
    Vertex4f b1 = b.lerp(a, bt);
    Vertex4f c1 = c.lerp(a, ct);

    // Preserve winding order
    // B follows A
    if ( ((ai+1)%3)==bi ) {
       triangles.push_back(Triangle(a, b1, c1));
    }
    // C follows A
    else {
       triangles.push_back(Triangle(a, c1, b1));
    }
  }

  // 1 point outside, create 2 smaller triangles
  else if (1==outsidePoints.size() && 2==insidePoints.size()) {
    Vertex4f& a = insidePoints[0];
    Vertex4f& b = outsidePoints[0];
    Vertex4f& c = insidePoints[1];
    int ai = insideIndices[0];
    int bi = outsideIndices[0];
    float abt = ClipSpace::signedDistanceRatio(b, a, ixyz, planeSign);
    float cbt = ClipSpace::signedDistanceRatio(b, c, ixyz, planeSign);
    Vertex4f a1 = b.lerp(a, abt);
    Vertex4f c1 = b.lerp(c, cbt);

    // Preserve winding order
    // B follows A
    if ( ((ai+1)%3)==bi ) {
      triangles.push_back(Triangle(a, a1, c1));
      triangles.push_back(Triangle(a, c1, c));
    }
    // C follows A
    else {
      triangles.push_back(Triangle(a, c, c1));
      triangles.push_back(Triangle(a, c1, a1));
    }
  }

  return triangles;
}


vector<Triangle> ClipSpace::clip(const vector<Triangle>& triangles,
                                 int ixyz,
                                 int planeSign)
{
  vector<Triangle> triangles2;
  std::vector<Triangle> tmp;
  for (size_t i=0; i<triangles.size(); ++i) {
    tmp = ClipSpace::clip(triangles[i], ixyz, planeSign);
    triangles2.insert( triangles2.end(), tmp.begin(), tmp.end() );
  }
  return triangles2;
}

vector<Triangle> ClipSpace::clipByNearPlane(const vector<Triangle>& triangles)
{
  return ClipSpace::clip(triangles, 2, -1); // near
}

vector<Triangle>
ClipSpace::clipByNearFarPlanes(const vector<Triangle>& triangles)
{
  vector<Triangle> r;
  r = ClipSpace::clip(triangles, 2, -1); // near
  r = ClipSpace::clip(r, 2, 1);  // far
  return r;
}

vector<Triangle> ClipSpace::clipByAllPlanes(const vector<Triangle>& triangles)
{
  vector<Triangle> r;
  r = ClipSpace::clip(triangles, 2, -1); // near
  r = ClipSpace::clip(r, 2, 1);  // far
  r = ClipSpace::clip(r, 0, 1);  // right
  r = ClipSpace::clip(r, 0, -1); // left
  r = ClipSpace::clip(r, 1, 1);  // top
  r = ClipSpace::clip(r, 1, -1); // bottom
  return r;
}