#include "Graphics3D.h"
#include <cmath>
#include "Vertex.h"
using namespace al::graphics;
using namespace std;

// Clips a line segment to an axis-aligned rectangle
// Returns true if clipping is successful
// Returns false if line segment lies outside the rectangle
bool Graphics3D::clipLineToRect(int a[2], int b[2],
                                int xmin, int ymin, int xmax, int ymax)
{
  int mins[2] = {xmin, ymin};
  int maxs[2] = {xmax, ymax};
  int normals[2] = {1, -1};
  for (int axis=0; axis<2; axis++) {
    for (int plane=0; plane<2; plane++) {
      // Check both points
      for (int pt=1; pt<=2; pt++) {
        int* pt1 = pt==1 ? a : b;
        int* pt2 = pt==1 ? b : a;

        // If both points are outside the same plane, the line is
        // outside the rectangle
        if ( (a[0]<xmin && b[0]<xmin) || (a[0]>xmax && b[0]>xmax) ||
             (a[1]<ymin && b[1]<ymin) || (a[1]>ymax && b[1]>ymax)) {
          return false;
        }

        const int n = normals[plane];
        if ( (n==1 && pt1[axis]<mins[axis]) ||   // check left/top plane
             (n==-1 && pt1[axis]>maxs[axis]) ) { // check right/bottom plane

          // Calculate interpolation factor t using ratio of signed distance
          // of each point from the plane
          const float p = (n==1) ? mins[axis] : maxs[axis];
          const float q1 = pt1[axis];
          const float q2 = pt2[axis];
          const float d1 = n * (q1-p);
          const float d2 = n * (q2-p);
          const float t = d1 / (d1-d2);

          // t should always be between 0 and 1
          if (t<0 || t >1) {
            return false;
          }

          // Interpolate to find the new point
          pt1[0] = (int)(pt1[0] +  (pt2[0] - pt1[0]) * t );
          pt1[1] = (int)(pt1[1] +  (pt2[1] - pt1[1]) * t );
        }
      }
    }
  }
  return true;
}

void Graphics3D::bline(ImageData& imageData,
                       int x0, int y0, int x1, int y1, u32 rgba)
{
  const int imageW = (int) imageData.width();
  const int imageH = (int) imageData.height();

  int a[2] = {x0, y0};
  int b[2] = {x1, y1};

  if (clipLineToRect(a, b, 0, 0, imageW-1, imageH-1)) {
    x0 = a[0];
    y0 = a[1];
    x1 = b[0];
    y1 = b[1];
  }
  else {
    return;
  }

  int dx = std::abs((int)(x1 - x0));
  int dy = std::abs((int)(y1 - y0));
  int sx = (x0 < x1) ? 1 : -1;
  int sy = (y0 < y1) ? 1 : -1;
  int err = dx - dy;

  while(true) {
    imageData.pixel(x0, y0) = rgba;
    if ((x0 == x1) && (y0 == y1)) break;
    int e2 = 2*err;
    if (e2 > -dy) { err -= dy; x0  += sx; }
    if (e2 < dx) { err += dx; y0  += sy; }
  }
}

void Graphics3D::triangleWire(ImageData& imageData,
                              int x1, int y1, int x2, int y2, int x3, int y3,
                              u32 rgba)
{
  Graphics3D::bline(imageData, x1, y1, x2, y2, rgba);
  Graphics3D::bline(imageData, x2, y2, x3, y3, rgba);
  Graphics3D::bline(imageData, x3, y3, x1, y1, rgba);
}

void Graphics3D::fillTriangle(ImageData& imageData,
                              int x1, int y1, int x2, int y2, int x3, int y3,
                              u32 rgba)
{
  Vector2f top(x1, y1);
  Vector2f mid(x2, y2);
  Vector2f bot(x3, y3);

  // Sort the points vertically
  if (mid.y < top.y) {
    std::swap(mid, top);
  }
  if (bot.y < top.y) {
    std::swap(bot, top);
  }
  if (bot.y < mid.y) {
    std::swap(bot, mid);
  }

  const int dytopmid = mid.y - top.y;  // Top to Mid
  const int dytopbot = bot.y - top.y; // Top to Bottom
  const int dymidbot = bot.y - mid.y;  // Mid to Bottom

  // Check if triangle has 0 height
  if (dytopbot == 0.0) {
    return;
  }

  // Top to Bottom Steps
  const Vector2f topbotstep((bot.x - top.x) / dytopbot);

  // The middle point on the top-bottom line
  Vector2f mid2(
    top.x+dytopmid*topbotstep.x,
    top.y+dytopmid
  );

  // Make sure mid is left of mid2 because we will
  // draw the horizontal scan line from left-to-right
  if (mid.x > mid2.x) {
    std::swap(mid, mid2);
  }

  // Top Half Triangle
  if (dytopmid) {
    const Vector2f leftStep((mid.x - top.x) / dytopmid);
    const Vector2f rightStep((mid2.x - top.x) / dytopmid);
    const int ystart = std::max(0, (int)top.y);
    const int yend = std::min((int)imageData.height()-1, (int)mid.y);
    for (int y=ystart; y<=yend; y++) {
      const int ysteps = y-top.y;
      const Vector2f left(top.x+ysteps*leftStep.x);
      const Vector2f right(top.x+ysteps*rightStep.x);
      const int xstart = std::max(0, (int)left.x);
      const int xend = std::min((int)right.x, (int)imageData.width());
      for (int x=xstart; x<xend; x++) {
        imageData.pixel(x, y) = rgba;
      }
    }
  }

  // Bottom Half Triangle
  if (dymidbot) {
    const Vector2f leftStep((bot.x - mid.x) / dymidbot);
    const Vector2f rightStep((bot.x - mid2.x) / dymidbot);
    const int ystart = std::max(0, (int)mid.y);
    const int yend = std::min((int)imageData.height()-1, (int)bot.y);
    for (int y=ystart; y<=yend; y++) {
      const int ysteps  = y - mid.y;
      const Vector2f left(mid.x+ysteps*leftStep.x);
      const Vector2f right(mid2.x+ysteps*rightStep.x);
      const int xstart = std::max(0, (int)left.x);
      const int xend = std::min((int)right.x, (int)imageData.width());
      for (int x=xstart; x<xend; x++) {
        imageData.pixel(x, y) = rgba;
      }
    }
  }
}

inline void Graphics3D::affineScanLine(ImageData& imageData,
                                       int y,
                                       const Vertex3f& left,
                                       const Vertex3f& right,
                                       ImageData& textureImageData,
                                       Grid<float>* zbuffer)
{
  const int leftx = left.x();
  const int rightx = right.x();
  const int dx = rightx - leftx;
  if (dx) {
    const Vertex3f scanlinestep = (right-left)/dx;
    const int xstart = clampZero(leftx);
    const int xend   = clampSize(rightx, (int)imageData.width()) ;
    for (int x=xstart; x<=xend; x++) {
      const int xsteps = x-leftx;
      const Vertex3f tex = left + scanlinestep * xsteps;
      const float texu = tex.u();
      const float texv = tex.v();
      imageData.pixel(x, y) = Graphics3D::pixelAtUV(textureImageData,
                                                    texu, texv);
    }
  }
}

void Graphics3D::affineTriangle(ImageData& imageData,
                                float x1, float y1, float u1, float v1,
                                float x2, float y2, float u2, float v2,
                                float x3, float y3, float u3, float v3,
                                ImageData& textureImageData)
 {
  Vertex3f top( {x1, y1}, {u1, v1} );
  Vertex3f mid( {x2, y2}, {u2, v2} );
  Vertex3f bot( {x3, y3}, {u3, v3} );

  // Sort the points vertically
  if (mid.y() < top.y()) {
    std::swap(mid, top);
  }
  if (bot.y() < top.y()) {
    std::swap(bot, top);
  }
  if (bot.y() < mid.y()) {
    std::swap(bot, mid);
  }

  const int topy = top.y();
  const int midy = mid.y();
  const int boty = bot.y();

  const int dytopmid = midy - topy; // Top to Mid Distance
  const int dytopbot = boty - topy; // Top to Bottom Distance
  const int dymidbot = boty - midy; // Mid to Bottom Distance

  // Check if triangle has 0 height
  if (!dytopbot) {
    return;
  }

  // Top to Bottom Steps
  const Vertex3f topbotstep = (bot - top) / dytopbot;

  // The middle point on the top-bottom line
  Vertex3f mid2 = top + topbotstep * dytopmid;

  // Make sure mid is left of mid2 because we will
  // draw the horizontal scan line from left-to-right
  if (mid.x() > mid2.x()) {
    std::swap(mid, mid2);
  }

  // Top Half Triangle
  if (dytopmid) {
    const Vertex3f leftStep = (mid-top)/dytopmid;
    const Vertex3f rightStep = (mid2-top)/dytopmid;
    const int ystart = std::max(0, (int)top.y());
    const int yend = std::min((int)imageData.height()-1, (int)mid.y());
    for (int y=ystart; y<=yend; y++) {
      const int ysteps = y-top.y();
      const Vertex3f left = top + leftStep * ysteps;
      const Vertex3f right = top + rightStep * ysteps;
      affineScanLine(imageData, y, left, right, textureImageData, 0);
    }
  }

  // Bottom Half Triangle
  if (dymidbot) {
    const Vertex3f leftStep = (bot-mid)/dymidbot;
    const Vertex3f rightStep = (bot-mid2)/dymidbot;
    const int ystart = std::max(0, (int)mid.y());
    const int yend = std::min((int)imageData.height()-1, (int)bot.y());
    for (int y=ystart; y<=yend; y++) {
      const int ysteps  = y - mid.y();
      const Vertex3f left = mid + leftStep * ysteps;
      const Vertex3f right = mid2 + rightStep * ysteps;
      affineScanLine(imageData, y, left, right, textureImageData, 0);
    }
  }
} // affineTriangle

inline void Graphics3D::texturedScanLine(ImageData& imageData,
                                         int y,
                                         const Vertex3f& left,
                                         const Vertex3f& right,
                                         ImageData& textureImageData,
                                         Grid<float>* zbuffer)
{
  const int leftx = left.x();
  const int rightx = right.x();
  const int dx = rightx - leftx;
  if (dx) {
    const Vertex3f scanlinestep = (right-left)/dx;
    const int xstart = clampZero(leftx);
    const int xend   = clampSize(rightx, (int)imageData.width()) ;
    for (int x=xstart; x<=xend; x++) {
      const int xsteps = x-leftx;
      const Vertex3f tex = left + scanlinestep * xsteps;
      const float z = 1/tex.z();
      if (zbuffer && z<zbuffer->unsafeGet(x,y)) {
        zbuffer->unsafeSet(x, y, z);
      }
      else {
        continue;
      }
      const float texu = tex.u() * z;
      const float texv = tex.v() * z;
      imageData.pixel(x, y) = Graphics3D::pixelAtUV(textureImageData,
                                                    texu, texv);
    }
  }
}

void Graphics3D::
texturedTriangle(ImageData& imageData,
                 float x1, float y1, float w1, float u1, float v1,
                 float x2, float y2, float w2, float u2, float v2,
                 float x3, float y3, float w3, float u3, float v3,
                 ImageData& textureImageData,
                 Grid<float>* zbuffer)
{
  Vertex3f top( {x1, y1, 1.0f/w1}, {u1/w1, v1/w1} );
  Vertex3f mid( {x2, y2, 1.0f/w2}, {u2/w2, v2/w2} );
  Vertex3f bot( {x3, y3, 1.0f/w3}, {u3/w3, v3/w3} );

  // Sort the points vertically
  if (mid.y() < top.y()) {
    std::swap(mid, top);
  }
  if (bot.y() < top.y()) {
    std::swap(bot, top);
  }
  if (bot.y() < mid.y()) {
    std::swap(bot, mid);
  }

  const int topy = top.y();
  const int midy = mid.y();
  const int boty = bot.y();

  const int dytopmid = midy - topy; // Top to Mid Distance
  const int dytopbot = boty - topy; // Top to Bottom Distance
  const int dymidbot = boty - midy; // Mid to Bottom Distance

  // Check if triangle has 0 height
  if (!dytopbot) {
    return;
  }

  // Top to Bottom Steps
  const Vertex3f topbotstep = (bot-top) / dytopbot;

  // The middle point on the top-bottom edge
  Vertex3f mid2 = top + topbotstep * dytopmid;

  // Make sure mid is left of mid2 because we will
  // draw the horizontal scan line from left-to-right
  if (mid.x() > mid2.x()) {
    std::swap(mid, mid2);
  }

  // Top Half Triangle
  if (dytopmid) {
    const Vertex3f leftStep = (mid - top) / dytopmid;
    const Vertex3f rightStep = (mid2 - top) / dytopmid;
    const int ystart = clampZero(topy);
    const int yend   = clampSize(midy, imageData.height());
    for (int y=ystart; y<=yend; y++) {
      const int ysteps = y - topy;
      const Vertex3f left = top + leftStep * ysteps;
      const Vertex3f right = top + rightStep * ysteps;
      texturedScanLine(imageData, y, left, right, textureImageData, zbuffer);
    }
  }

  // Bottom Half Triangle
  if (dymidbot) {
    const Vertex3f leftStep = (bot - mid) / dymidbot;
    const Vertex3f rightStep = (bot - mid2) / dymidbot;
    const int ystart = clampZero(midy);
    const int yend   = clampSize(boty, imageData.height());
    for (int y=ystart; y<=yend; y++) {
      const int ysteps  = y - midy;
      const Vertex3f left = mid + leftStep * ysteps;
      const Vertex3f right = mid2 + rightStep * ysteps;
      texturedScanLine(imageData, y, left, right, textureImageData, zbuffer);
    }
  }
} // texturedTriangle