#include "TriangleDrawer.h"
#include "Graphics3D.h"
#include <algorithm>

using namespace al::graphics;
using namespace std;

void TriangleDrawer::wireframeTriangle(ImageData& imageData,
                                       const Triangle& t,
                                       u32 rgba)
{
  G3D::bline(imageData, t[0].x(), t[0].y(), t[1].x(), t[1].y(), rgba);
  G3D::bline(imageData, t[1].x(), t[1].y(), t[2].x(), t[2].y(), rgba);
  G3D::bline(imageData, t[2].x(), t[2].y(), t[0].x(), t[0].y(), rgba);
}

inline void TriangleDrawer::scanline(ImageData& imageData,
                                     const Triangle& triangle,
                                     int y,
                                     const Vertex4f& left,
                                     const Vertex4f& right)
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
      const float depth = tex.z() * w;
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
        case DRAW_SOLID:
        default:
          pixel = triangle.color;
          break;
      }
      const uint8_t* rgba = (uint8_t*) &pixel;
      const uint8_t alpha = rgba[3];
      if (alpha == 255) {
        imageData.pixel(x, y) = pixel;
        depthBuffer->unsafeSet(x, y, depth);
      }
    }
  }
}

void TriangleDrawer::triangle(ImageData& imageData, const Triangle& triangle)
{
  Vertex4f top = triangle.vertices[0];
  Vertex4f mid = triangle.vertices[1];
  Vertex4f bot = triangle.vertices[2];

  if (drawMode == DRAW_PERSPECTIVE) {
    top.uv = top.uv.div(top.pos.w);
    mid.uv = mid.uv.div(mid.pos.w);
    bot.uv = bot.uv.div(bot.pos.w);
    top.pos.z = top.pos.z / top.pos.w;
    mid.pos.z = mid.pos.z / mid.pos.w;
    bot.pos.z = bot.pos.z / bot.pos.w;
    top.pos.w = 1.0 / top.pos.w;
    mid.pos.w = 1.0 / mid.pos.w;
    bot.pos.w = 1.0 / bot.pos.w;
  }

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

  // Make sure mid is left of mid2 because we will
  // draw the horizontal scan line from left-to-right
  if (!isMidLeft) {
    std::swap(mid, mid2);
  }

  // Top Half Triangle
  if (dytopmid) {
    const Vertex4f leftStep  = isMidLeft ? (mid-top)/dytopmid : topbotstep;
    const Vertex4f rightStep = isMidLeft ? topbotstep : (mid2-top)/dytopmid;
    const int ystart = std::max(0, topy);
    const int yend   = std::min((int)imageData.height()-1, midy);
    for (int y=ystart; y<=yend; y++) {
      const int ysteps = y - topy;
      const Vertex4f left = top + leftStep * ysteps;
      const Vertex4f right = top + rightStep * ysteps;
      scanline(imageData, triangle, y, left, right);
    }
  }

  // Bottom Half Triangle
  if (dymidbot) {
    const Vertex4f leftStep  = isMidLeft ? (bot-mid)/dymidbot : topbotstep;
    const Vertex4f rightStep = isMidLeft ? topbotstep : (bot-mid2)/dymidbot;
    const int ystart = std::max(0, midy);
    const int yend   = std::min((int)imageData.height()-1, boty);
    for (int y=ystart; y<=yend; y++) {
      const int ysteps  = y - midy;
      const Vertex4f left = mid + leftStep * ysteps;
      const Vertex4f right = mid2 + rightStep * ysteps;
      scanline(imageData, triangle, y, left, right);
    }
  }
} // texturedTriangle