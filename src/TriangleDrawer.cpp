#include "TriangleDrawer.h"
#include "LineDrawer.h"
#include "Graphics3D.h"
#include <algorithm>
#include <cassert>
using namespace al::graphics;
using namespace std;

void TriangleDrawer::wireframeTriangle(ImageData& imageData,
                                       const Triangle& t,
                                       u32 rgba)
{
  LineDrawer::bline(imageData, t[0].x(), t[0].y(), t[1].x(), t[1].y(), rgba);
  LineDrawer::bline(imageData, t[1].x(), t[1].y(), t[2].x(), t[2].y(), rgba);
  LineDrawer::bline(imageData, t[2].x(), t[2].y(), t[0].x(), t[0].y(), rgba);
}

void TriangleDrawer::triangle(ImageData& imageData, const Triangle& triangle)
{
  switch (algo) {
    case SCANLINE: scanlineTriangle(imageData, triangle); return;
    case BARYCENTRIC: barycentricTriangle(imageData, triangle); return;
    case BARYCENTRIC_O1: barycentricO1(imageData, triangle); return;
    case BARYCENTRIC_O2: barycentricO2(imageData, triangle); return;
  }
}

inline void TriangleDrawer::scanline(ImageData& imageData,
                                     const Triangle& triangle,
                                     const Vertex4f& left,
                                     const Vertex4f& right,
                                     int y)
{
  const int leftx = left.x();
  const int rightx = right.x();
  const int dx = rightx - leftx;
  if (dx) {
    const Vertex4f scanlinestep = (right-left)/dx;
    const int xstart = std::max(0, leftx);
    const int xend   = std::min((int)imageData.width()-1, rightx) ;
    for (int x=xstart; x<=xend; x++) {
      const int xsteps = x-leftx;
      const Vertex4f tex = left + scanlinestep * xsteps;
      const float w = 1/tex.w();
      const float depth = w;
      if (depthBuffer) {
        if (depth>=depthBuffer->unsafeGet(x,y)) {
          continue;
        }
      }
      uint32_t pixel;
      switch (drawMode) {
        case DRAW_PERSPECTIVE:
          pixel = G3D::pixelAtUV(*textureImageData, tex.u()*w, tex.v()*w);
          break;
        case DRAW_AFFINE:
          pixel = G3D::pixelAtUV(*textureImageData, tex.u(), tex.v());
          break;
        case DRAW_RGB_SHADED:
          pixel = G3D::colorForRGB(
            (int)(tex.rgb.x()*w),
            (int)(tex.rgb.y()*w),
            (int)(tex.rgb.z()*w)
          );
          break;
        default:
        case DRAW_SOLID:
          pixel = triangle.color;
          break;
      }
      const uint8_t* rgba = (uint8_t*) &pixel;
      const uint8_t alpha = rgba[3];
      if (alpha) {
        if (lightsStyle != LIGHTS_STYLE_NONE) {
          float lightStrength = 1.0;
          if (lightsStyle ==  LIGHTS_STYLE_FLAT) {
            const Vector3f faceNormal = G3D::normalize(triangle.normal);
            const float dot = G3D::dot(lightDirection, faceNormal);
            lightStrength = std::max(0.5f, dot);
          }
          else {
            Vector3f vertexNormal = G3D::normalize(tex.normal);
            const float dot = G3D::dot(lightDirection, vertexNormal);
            lightStrength = std::max(0.5f, dot);
          }
          pixel = G3D::lightPixel(pixel, lightStrength);
        }
        imageData.pixel(x, y) = pixel;
        if (depthBuffer) {
          depthBuffer->unsafeSet(x, y, depth);
        }
      }
    }
  }
}

void TriangleDrawer::
scanlineTriangle(ImageData& imageData, const Triangle& triangle)
{
  Vertex4f top = triangle.vertices[0];
  Vertex4f mid = triangle.vertices[1];
  Vertex4f bot = triangle.vertices[2];

  if (drawMode != DRAW_AFFINE) {
    top.uv /= top.pos.w();
    mid.uv /= mid.pos.w();
    bot.uv /= bot.pos.w();
    top.rgb /= top.pos.w();
    mid.rgb /= mid.pos.w();
    bot.rgb /= bot.pos.w();
  }

  top.pos.w() = 1.0 / top.pos.w();
  mid.pos.w() = 1.0 / mid.pos.w();
  bot.pos.w() = 1.0 / bot.pos.w();

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
  const Vertex4f topbotstep = (bot-top) / dytopbot;

  // The middle point on the top-bottom edge
  Vertex4f mid2 = top + topbotstep * dytopmid;

  // Is mid to the left of mid2
  // (i.e. top-bottom edge is right of mid)
  const bool isMidLeft = mid.x() <= mid2.x();

  // Top Half Triangle
  if (dytopmid) {
    const Vertex4f leftStep  = isMidLeft ? (mid-top)/dytopmid : topbotstep;
    const Vertex4f rightStep = isMidLeft ? topbotstep : (mid-top)/dytopmid;
    const int ystart = std::max(0, topy);
    const int yend   = std::min((int)imageData.height()-1, midy);
    for (int y=ystart; y<=yend; y++) {
      const int ysteps = y - topy;
      const Vertex4f left = top + leftStep * ysteps;
      const Vertex4f right = top + rightStep * ysteps;
      scanline(imageData, triangle, left, right, y);

      // 4th vertex mid2 logic test
      // At the end of the top half triangle, mid2 should be the same as either
      // left or right.
      // If this assert fails, try specify the -ffloat-store flag
      // https://stackoverflow.com/a/3765256/1645045
      #ifdef TEST_MID2
      if (y == yend && ysteps == dytopmid) {
        if (isMidLeft) {
          assert(right.x() == mid2.x());
        } else {
          assert(left.x() == mid2.x());
        }
      }
      #endif
    }
  }

  // Bottom Half Triangle
  if (dymidbot) {
    // Make sure mid is left of mid2 because we will
    // draw the horizontal scan line from left-to-right
    if (!isMidLeft) {
      std::swap(mid, mid2);
    }
    const Vertex4f leftStep  = isMidLeft ? (bot-mid)/dymidbot : topbotstep;
    const Vertex4f rightStep = isMidLeft ? topbotstep : (bot-mid2)/dymidbot;
    const int ystart = std::max(0, midy);
    const int yend   = std::min((int)imageData.height()-1, boty);
    for (int y=ystart; y<=yend; y++) {
      const int ysteps  = y - midy;
      const Vertex4f left = mid + leftStep * ysteps;
      const Vertex4f right = mid2 + rightStep * ysteps;
      scanline(imageData, triangle, left, right, y);
    }
  }
} // texturedTriangle

inline
void TriangleDrawer::barycentricPixel(ImageData& imageData,
                                      int x, int y,
                                      const Triangle& t,
                                      float alp, float bet, float gam,
                                      double w)
{
  const Vertex4f& a = t.vertices[0];
  const Vertex4f& b = t.vertices[1];
  const Vertex4f& c = t.vertices[2];
  const double depth = w;
  uint32_t pixel;
  switch (drawMode) {
    case DRAW_PERSPECTIVE: {
      const Vector2f uv = (a.uv*alp + b.uv*bet + c.uv*gam);
      pixel = G3D::pixelAtUV(*textureImageData, uv.u()*w, uv.v()*w);
      break;
    }
    case DRAW_AFFINE: {
      const Vector2f uv = (a.uv*alp + b.uv*bet + c.uv*gam);
      pixel = G3D::pixelAtUV(*textureImageData, uv.u(), uv.v());
      break;
    }
    case DRAW_RGB_SHADED: {
      const Vector3f rgb = (a.rgb*alp + b.rgb*bet + c.rgb*gam);
      pixel = G3D::colorForRGB(
        (int)(rgb.x()*w),
        (int)(rgb.y()*w),
        (int)(rgb.z()*w)
      );
      break;
    }
    default:
    case DRAW_SOLID:
      pixel = t.color;
      break;
  } // switch (drawMode)

  const uint8_t* rgba = (uint8_t*) &pixel;
  const uint8_t alpha = rgba[3];

  // Lighting
  if (alpha) {
    if (lightsStyle != LIGHTS_STYLE_NONE) {
      float lightStrength = 1.0;
      if (lightsStyle ==  LIGHTS_STYLE_FLAT) {
        const Vector3f faceNormal = G3D::normalize(t.normal);
        const float dot = G3D::dot(lightDirection, faceNormal);
        lightStrength = std::max(0.5f, dot);
      }
      else {
        // https://stackoverflow.com/a/15625175/1645045
        Vector3f vertexNormal = (a.normal*alp) + (b.normal*bet) +
                                (c.normal*gam);
        vertexNormal = G3D::normalize(vertexNormal);
        const float dot = G3D::dot(lightDirection, vertexNormal);
        lightStrength = std::max(0.5f, dot);
      }
      pixel = G3D::lightPixel(pixel, lightStrength);
    }
    imageData.pixel(x, y) = pixel;
    if (depthBuffer) {
      depthBuffer->unsafeSet(x, y, depth);
    }

  } // if (alpha)
}

void TriangleDrawer::
barycentricTriangle(ImageData& imageData, const Triangle& triangle)
{
  Triangle t(triangle);

  Vertex4f& a = t.vertices[0];
  Vertex4f& b = t.vertices[1];
  Vertex4f& c = t.vertices[2];

  if (drawMode != DRAW_AFFINE) {
    a.uv /= a.pos.w();
    b.uv /= b.pos.w();
    c.uv /= c.pos.w();
    a.rgb /= a.pos.w();
    b.rgb /= b.pos.w();
    c.rgb /= c.pos.w();
  }

  a.pos.w() = 1.0 / a.pos.w();
  b.pos.w() = 1.0 / b.pos.w();
  c.pos.w() = 1.0 / c.pos.w();

  const int w = imageData.width();
  const int h = imageData.height();

  // Triangle bounding box
  const float maxXf = std::max(a.x(), std::max(b.x(), c.x()));
  const float minXf = std::min(a.x(), std::min(b.x(), c.x()));
  const float maxYf = std::max(a.y(), std::max(b.y(), c.y()));
  const float minYf = std::min(a.y(), std::min(b.y(), c.y()));
  const int maxX = std::min(std::ceil(maxXf), (float)w);
  int minX = std::max((int)std::floor(minXf), 0);
  const int maxY = std::min(std::ceil(maxYf), (float)h);
  const int minY = std::max((int)std::floor(minYf), 0);

  double area = G3D::crossProduct2D(a.pos, b.pos, c.pos);

  for (int y = minY; y < maxY; ++y) {
    for (int x = minX; x < maxX; ++x) {
      Vector4f p(x + 0.5f, y + 0.5f, 0);

      // crossProduct2D is <= 0 if
      // 1. Point is in triangle
      // 2. Vertices are passed in counter-clockwise CCW winding order
      // 3. Coordinates are y-down screen coordinates
      float alp = G3D::crossProduct2D(b.pos, c.pos, p);
      if (alp > 0) continue;
      float bet = G3D::crossProduct2D(c.pos, a.pos, p);
      if (bet > 0) continue;
      float gam = G3D::crossProduct2D(a.pos, b.pos, p);
      if (gam > 0) continue;

      alp /= area;
      bet /= area;
      gam /= area;

      const double w = 1 / (a.w()*alp + b.w()*bet + c.w()*gam);
      const double depth = w;
      if (depthBuffer) {
        if (depth>=depthBuffer->unsafeGet(x,y)) {
          continue;
        }
      }

      barycentricPixel(imageData, x, y, t, alp, bet, gam, w);

    } // for x
  } // for y
} // barycentricTriangle



void TriangleDrawer::
barycentricO1(ImageData& imageData, const Triangle& triangle)
{
  Triangle t(triangle);

  Vertex4f& a = t.vertices[0];
  Vertex4f& b = t.vertices[1];
  Vertex4f& c = t.vertices[2];


  if (drawMode != DRAW_AFFINE) {
    a.uv /= a.pos.w();
    b.uv /= b.pos.w();
    c.uv /= c.pos.w();
    a.rgb /= a.pos.w();
    b.rgb /= b.pos.w();
    c.rgb /= c.pos.w();
  }

  a.pos.w() = 1.0 / a.pos.w();
  b.pos.w() = 1.0 / b.pos.w();
  c.pos.w() = 1.0 / c.pos.w();

  const int w = imageData.width();
  const int h = imageData.height();

  // Triangle bounding box
  const double maxXf = std::max(a.x(), std::max(b.x(), c.x()));
  const double minXf = std::min(a.x(), std::min(b.x(), c.x()));
  const double maxYf = std::max(a.y(), std::max(b.y(), c.y()));
  const double minYf = std::min(a.y(), std::min(b.y(), c.y()));
  const int maxX = std::min(std::ceil(maxXf), (double)w);
  const int minX = std::max(std::floor(minXf), 0.0);
  const int maxY = std::min(std::ceil(maxYf), (double)h);
  const int minY = std::max(std::floor(minYf), 0.0);

  double area = G3D::crossProduct2D(a.pos, b.pos, c.pos);

  Vector4f v0 = a.pos;
  Vector4f v1 = b.pos;
  Vector4f v2 = c.pos;

  // Compute the constant delta_s that will be used for the horizontal and
  // vertical steps
  const float deltacols[3] = {
    v1.y() - v2.y(),
    v2.y() - v0.y(),
    v0.y() - v1.y()
  };
  const float deltarows[3] = {
    v2.x() - v1.x(),
    v0.x() - v2.x(),
    v1.x() - v0.x()
  };

  const Vector4f p(minX + 0.5f , minY + 0.5f);
  float w0_row = G3D::crossProduct2D(v1, v2, p);
  float w1_row = G3D::crossProduct2D(v2, v0, p);
  float w2_row = G3D::crossProduct2D(v0, v1, p);

  for (int y = minY; y < maxY; ++y,
    w0_row += deltarows[0],
    w1_row += deltarows[1],
    w2_row += deltarows[2]
  ) {
    float w0 = w0_row;
    float w1 = w1_row;
    float w2 = w2_row;
    for (int x = minX; x < maxX; ++x,
      w0 += deltacols[0],
      w1 += deltacols[1],
      w2 += deltacols[2]
    ) {
      // crossProduct2D is <= 0 if
      // 1. Point is in triangle
      // 2. Vertices are passed in counter-clockwise CCW winding order
      // 3. Coordinates are y-down screen coordinates
      bool is_inside = w0 <= 0 && w1 <= 0 && w2 <= 0;
      if (!is_inside) {
        continue;
      }

      const float alp = w0 / area;
      const float bet = w1 / area;
      const float gam = w2 / area;

      const double w = 1 / (a.w()*alp + b.w()*bet + c.w()*gam);
      const double depth = w;
      if (depthBuffer) {
        if (depth>=depthBuffer->unsafeGet(x,y)) {
          continue;
        }
      }

      barycentricPixel(imageData, x, y, t, alp, bet, gam, w);

    } // for x
  } // for y
} // barycentricAddTriangle

inline int TriangleDrawer::barycentricScanline(ImageData& imageData,
                                               const Triangle& triangle,
                                               int leftx,
                                               int rightx,
                                               int y,
                                               float area,
                                               const float deltacols[3])
{
  const Vertex4f& a = triangle.vertices[0];
  const Vertex4f& b = triangle.vertices[1];
  const Vertex4f& c = triangle.vertices[2];
  int newMinX = -1;
  int finalX = 0;
  bool wasInside = false;
  const int xstart = std::max(0, leftx);
  const int xend   = std::min((int)imageData.width()-1, rightx) ;
  const Vector4f p(xstart + 0.5f , y + 0.5f);
  double w0 = G3D::crossProduct2D(b.pos, c.pos, p);
  double w1 = G3D::crossProduct2D(c.pos, a.pos, p);
  double w2 = G3D::crossProduct2D(a.pos, b.pos, p);
  for (int x=xstart; x<=xend; x++,
    w0 += deltacols[0],
    w1 += deltacols[1],
    w2 += deltacols[2]
  )
  {
    const bool inside = w0 <= 0 && w1 <= 0 && w2 <= 0;
    if (!inside) {
      if (wasInside) {
        finalX = x;
        break;
      }
      continue;
    }
    if (-1==newMinX) {
      newMinX = x;
    }
    wasInside = true;
    const double alp = w0 / area;
    const double bet = w1 / area;
    const double gam = w2 / area;
    const double w = 1 / (a.w()*alp + b.w()*bet + c.w()*gam);
    const double depth = w;
    if (depthBuffer) {
      if (depth>=depthBuffer->unsafeGet(x,y)) {
        continue;
      }
    }
    barycentricPixel(imageData, x, y, triangle, alp, bet, gam, w);
    finalX = x;
  }

  if (-1!=newMinX && newMinX!=finalX) {
    return newMinX;
  }
  return leftx;
} // barycentricScanline

void TriangleDrawer::
barycentricO2(ImageData& imageData, const Triangle& triangle)
{
  Vector2f top(triangle.vertices[0].pos);
  Vector2f mid(triangle.vertices[1].pos);
  Vector2f bot(triangle.vertices[2].pos);

  // Sort the points vertically
  if (mid.y() < top.y()) { std::swap(mid, top); }
  if (bot.y() < top.y()) { std::swap(bot, top); }
  if (bot.y() < mid.y()) { std::swap(bot, mid); }

  const int topy = top.y();
  const int midy = mid.y();
  const int boty = bot.y();

  // Setup triangle to pass to barycentricScanline
  Triangle t(triangle);
  Vertex4f& a = t.vertices[0];
  Vertex4f& b = t.vertices[1];
  Vertex4f& c = t.vertices[2];

  // Divide by 1/z for interpolation
  if (drawMode != DRAW_AFFINE) {
    a.uv /= a.pos.w();
    b.uv /= b.pos.w();
    c.uv /= c.pos.w();
    a.rgb /= a.pos.w();
    b.rgb /= b.pos.w();
    c.rgb /= c.pos.w();
  }
  a.pos.w() = 1.0 / a.pos.w();
  b.pos.w() = 1.0 / b.pos.w();
  c.pos.w() = 1.0 / c.pos.w();

  // Triangle bounding box
  const float maxXf = std::max(a.x(), std::max(b.x(), c.x()));
  const int maxX = std::min(std::ceil(maxXf), (float)imageData.width());
  const int maxY = ceil(boty);

  double area = G3D::crossProduct2D(a.pos, b.pos, c.pos);

  // Horizontal barycentric deltas
  const float deltacols[3] = {b.y() - c.y(), c.y() - a.y(), a.y() - b.y()};

  const int dytopmid = midy - topy; // Top to Mid Distance
  const int dytopbot = boty - topy; // Top to Bottom Distance

  // The middle point on the top-bottom edge
  const float mid2x = dytopbot // check to prevent divide by 0
                      ? (top.x() + ((bot.x() - top.x())/dytopbot) * dytopmid)
                      : bot.x();

  // Is mid to the left of mid2 (i.e. top-bottom edge is right of mid)
  const bool isMidLeft = mid.x() <= mid2x;

  // Make sure mid is left of mid2 because we will
  // draw the horizontal scan line from left-to-right
  if (!isMidLeft) {
    mid.x() = mid2x;
  }

  // Top Half Triangle
  int ystart = std::max(0, topy);
  int yend   = std::min((int)imageData.height()-1, midy);
  // Left edge slopes down - start drawing from top
  if (top.x() <= mid.x()) {
    int left = top.x();
    for (int y=ystart; y<=yend; y++) {
      const int newMinX = barycentricScanline(imageData, t, left, maxX, y, area,
                                              deltacols);
      if (y>ystart && y<yend) {
        left = std::max(left, newMinX);
      }
    }
  }
  // Left edge slopes up - start drawing from bottom
  else  {
    int left = mid.x();
    for (int y=yend; y>=ystart; y--) {
      const int newMinX = barycentricScanline(imageData, t, left, maxX, y, area,
                                              deltacols);
      if (y>ystart && y<yend) {
        left = std::max(left, newMinX);
      }
    }
  }

  // Bottom Half Triangle
  ystart = std::max(0, midy);
  yend   = std::min((int)imageData.height()-1, maxY);
  // Left edge slopes down - start drawing from top
  if (mid.x() <= bot.x()) {
    int left = mid.x();
    for (int y=ystart; y<=yend; y++) {
      const int newMinX = barycentricScanline(imageData, t, left, maxX, y, area,
                                              deltacols);
      if (y>ystart && y<yend) {
        left = std::max(left, newMinX);
      }
    }
  }
  // Left edge slopes up - start drawing from bottom
  else  {
    int left = bot.x();
    for (int y=yend; y>=ystart; y--) {
      const int newMinX = barycentricScanline(imageData, t, left, maxX, y, area,
                                              deltacols);
      if (y>ystart && y<yend) {
        left = std::max(left, newMinX);
      }
    }
  }
}