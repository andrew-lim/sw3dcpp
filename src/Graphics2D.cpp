#include "Graphics2D.h"
#include <cmath>
#include "Vertex.h"
using namespace al::graphics;

inline static int clipLeft(float x)
{
  int newx = (int)(x + 0.5);
  if (newx < 0) {
    newx = 0;
  }
  return newx;
}

inline static int clipRight(float x, int viewWidth)
{
  int newx = (int)(x + 0.5);
  if (newx >= viewWidth) {
    newx = viewWidth - 1;
  }
  return newx;
}

// Gets the pixel from an ImageData bitmap by converting UV coordinates
// to bitmap coordinates
u32& Graphics2D::pixelAtUV(ImageData& imageData, float u, float v)
{
  // UV coordinates start from the bottom-left of a texture
  // But ImageData coordinates start from the top-left
  // https://stackoverflow.com/a/33324409/1645045
  v = 1.0f-v;

  const int imageW = (int) imageData.width();
  const int imageH = (int) imageData.height();

  int x = (u*imageW);
  int y = (v*imageH);

  // Wrap out of bounds coordinates
  x = x % imageW;
  y = y % imageH;

  // Wrap negative coordinates
  if (x<0) {
    x = imageW - std::abs(x);
  }
  if (y<0) {
    y = imageH - std::abs(y);
  }

  return imageData.pixel(x,y);
}


void Graphics2D::bline(ImageData& imageData,
                       int x0, int y0, int x1, int y1, u32 rgba)
{
  int dx = std::abs((int)(x1 - x0));
  int dy = std::abs((int)(y1 - y0));
  int sx = (x0 < x1) ? 1 : -1;
  int sy = (y0 < y1) ? 1 : -1;
  int err = dx - dy;
  while(true) {
    if (x0>=0 && y0>=0 &&
        x0<(int)imageData.width() && y0<(int)imageData.height()) {
      imageData.pixel(x0, y0) = rgba;
    }

    if ((x0 == x1) && (y0 == y1)) break;
    int e2 = 2*err;
    if (e2 > -dy) { err -= dy; x0  += sx; }
    if (e2 < dx) { err += dx; y0  += sy; }
  }
}

void Graphics2D::triangleWire(ImageData& imageData,
                              int x1, int y1, int x2, int y2, int x3, int y3,
                              u32 rgba)
{
  Graphics2D::bline(imageData, x1, y1, x2, y2, rgba);
  Graphics2D::bline(imageData, x2, y2, x3, y3, rgba);
  Graphics2D::bline(imageData, x3, y3, x1, y1, rgba);
}

void Graphics2D::fillTriangle(ImageData& imageData,
                              int x1, int y1, int x2, int y2, int x3, int y3,
                              u32 rgba)
{
  Vertex top(x1, y1);
  Vertex mid(x2, y2);
  Vertex bot(x3, y3);

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

  const float dytopmid = mid.y() - top.y();  // Top to Mid
  const float dytopbot = bot.y() - top.y(); // Top to Bottom
  const float dymidbot = bot.y() - mid.y();  // Mid to Bottom

  // Check if triangle has 0 height
  if (dytopbot == 0.0) {
    return;
  }

  // Top to Bottom Steps
  const Vertex topbotstep((bot.x() - top.x()) / std::abs(dytopbot));

  // The middle point on the top-bottom line
  Vertex mid2(
    top.x()+dytopmid*topbotstep.x(),
    top.y()+dytopmid
  );

  // Make sure mid is left of mid2 because we will
  // draw the horizontal scan line from left-to-right
  if (mid.x() > mid2.x()) {
    std::swap(mid, mid2);
  }

  // Top Half Triangle
  if (dytopmid) {
    const Vertex leftStep((mid.x() - top.x()) / std::abs(dytopmid));
    const Vertex rightStep((mid2.x() - top.x()) / std::abs(dytopmid));
    const int ystart = std::max(0, (int)top.y());
    const int yend = std::min((int)imageData.height()-1, (int)mid.y());
    for (int y=ystart; y<=yend; y++) {
      const int ysteps = y-top.y();

      // Left Point
      const Vertex left(top.x()+ysteps*leftStep.x());

      // Right Point
      const Vertex right(top.x()+ysteps*rightStep.x());

      // Draw the horizontal line between left and right
      const int xstart = std::max(0, (int)left.x());
      const int xend = std::min((int)right.x(), (int)imageData.width());
      for (int x=xstart; x<xend; x++) {
        imageData.pixel(x, y) = rgba;
      }
    }
  }

  // Bottom Half Triangle
  if (dymidbot) {
    const Vertex leftStep((bot.x() - mid.x()) / std::abs(dymidbot));
    const Vertex rightStep((bot.x() - mid2.x()) / std::abs(dymidbot));
    const int ystart = std::max(0, (int)mid.y());
    const int yend = std::min((int)imageData.height()-1, (int)bot.y());
    for (int y=ystart; y<=yend; y++) {
      const int ysteps  = y - mid.y();

      // Left Point
      const Vertex left(mid.x()+ysteps*leftStep.x());

      // Right Point
      const Vertex right(mid2.x()+ysteps*rightStep.x());

      // Draw the horizontal line between left and right
      const int xstart = std::max(0, (int)left.x());
      const int xend = std::min((int)right.x(), (int)imageData.width());
      for (int x=xstart; x<xend; x++) {
        imageData.pixel(x, y) = rgba;
      }
    }
  }
}

void Graphics2D::affineTriangle(ImageData& imageData,
                                int x1, int y1, float u1, float v1,
                                int x2, int y2, float u2, float v2,
                                int x3, int y3, float u3, float v3,
                                ImageData& textureImageData)
 {
  Vertex top(Vertex::xyuv(x1, y1, u1, v1));
  Vertex mid(Vertex::xyuv(x2, y2, u2, v2));
  Vertex bot(Vertex::xyuv(x3, y3, u3, v3));

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

  const float dytopmid = mid.y() - top.y();  // Top to Mid
  const float dytopbot = bot.y() - top.y(); // Top to Bottom
  const float dymidbot = bot.y() - mid.y();  // Mid to Bottom

  // Check if triangle has 0 height
  if (dytopbot == 0.0) {
    return;
  }

  // Top to Bottom Steps
  const Vertex topbotstep(Vertex::xyuv(
   (bot.x() - top.x()) / std::abs(dytopbot),
   0,
   (bot.u() - top.u()) / std::abs(dytopbot),
   (bot.v() - top.v()) / std::abs(dytopbot)
  ));

  // The middle point on the top-bottom line
  Vertex mid2(Vertex::xyuv(
    top.x()+dytopmid*topbotstep.x(),
    top.y()+dytopmid,
    top.u()+dytopmid*topbotstep.u(),
    top.v()+dytopmid*topbotstep.v()
  ));

  // Make sure mid is left of mid2 because we will
  // draw the horizontal scan line from left-to-right
  if (mid.x() > mid2.x()) {
    std::swap(mid, mid2);
  }

  // Top Half Triangle
  if (dytopmid) {
    const Vertex leftStep(Vertex::xyuv(
      (mid.x() - top.x()) / std::abs(dytopmid),
      0,
      (mid.u() - top.u()) / std::abs(dytopmid),
      (mid.v() - top.v()) / std::abs(dytopmid)
    ));

    const Vertex rightStep(Vertex::xyuv(
      (mid2.x() - top.x()) / std::abs(dytopmid),
      0,
      (mid2.u() - top.u()) / std::abs(dytopmid),
      (mid2.v() - top.v()) / std::abs(dytopmid)
    ));

    const int ystart = std::max(0, (int)top.y());
    const int yend = std::min((int)imageData.height()-1, (int)mid.y());
    for (int y=ystart; y<=yend; y++) {
      const int ysteps = y-top.y();

      // Left Point
      const Vertex left(Vertex::xyuv(
        top.x()+ysteps*leftStep.x(),
        0,
        top.u()+ysteps*leftStep.u(),
        top.v()+ysteps*leftStep.v()
      ));

      // Right Point
      const Vertex right(Vertex::xyuv(
        top.x()+ysteps*rightStep.x(),
        0,
        top.u()+ysteps*rightStep.u(),
        top.v()+ysteps*rightStep.v()
      ));

      // Draw the horizontal line between left and right
      const int dx = right.x()-left.x();
      if (dx != 0) {
        const float ustep = (right.u()-left.u()) / dx;
        const float vstep = (right.v()-left.v()) / dx;
        const int xstart = std::max(0, (int)left.x());
        const int xend = std::min((int)right.x(), (int)imageData.width());
        for (int x=xstart; x<xend; x++) {
          const int xsteps = x-left.x();
          const float u = left.u() + xsteps * ustep;
          const float v = left.v() + xsteps * vstep;
          const uint32_t pixel = Graphics2D::pixelAtUV(textureImageData, u, v);
          imageData.pixel(x, y) = pixel;
        }
      }
    }
  }

  // Bottom Half Triangle
  if (dymidbot) {
     const Vertex leftStep(Vertex::xyuv(
      (bot.x() - mid.x()) / std::abs(dymidbot),
      0,
      (bot.u() - mid.u()) / std::abs(dymidbot),
      (bot.v() - mid.v()) / std::abs(dymidbot)
    ));

    const Vertex rightStep(Vertex::xyuv(
      (bot.x() - mid2.x()) / std::abs(dymidbot),
      0,
      (bot.u() - mid2.u()) / std::abs(dymidbot),
      (bot.v() - mid2.v()) / std::abs(dymidbot)
    ));
    const int ystart = std::max(0, (int)mid.y());
    const int yend = std::min((int)imageData.height()-1, (int)bot.y());
    for (int y=ystart; y<=yend; y++) {
      const int ysteps  = y - mid.y();

      // Left Point
      const Vertex left(Vertex::xyuv(
        mid.x()+ysteps*leftStep.x(),
        0,
        mid.u()+ysteps*leftStep.u(),
        mid.v()+ysteps*leftStep.v()
      ));

      // Right Point
      const Vertex right(Vertex::xyuv(
        mid2.x()+ysteps*rightStep.x(),
        0,
        mid2.u()+ysteps*rightStep.u(),
        mid2.v()+ysteps*rightStep.v()
      ));

      // Draw the horizontal line between left and right
      const int dx = right.x()-left.x();
      if (dx != 0) {
        const float ustep = (right.u()-left.u()) / dx;
        const float vstep = (right.v()-left.v()) / dx;
        const int xstart = std::max(0, (int)left.x());
        const int xend = std::min((int)right.x(), (int)imageData.width());
        for (int x=xstart; x<xend; x++) {
          const int xsteps = x-left.x();
          const float u = left.u() + xsteps * ustep;
          const float v = left.v() + xsteps * vstep;
          const uint32_t pixel = Graphics2D::pixelAtUV(textureImageData, u, v);
          imageData.pixel(x, y) = pixel;
        }
      }
    }
  }
} // affineTriangle

void Graphics2D::
texturedTriangle(ImageData& imageData,
                 float x1, float y1, float w1, float u1, float v1,
                 float x2, float y2, float w2, float u2, float v2,
                 float x3, float y3, float w3, float u3, float v3,
                 ImageData& textureImageData,
                 Grid<float>* zbuffer)
{
  // Texture mapping perspective correction using w
  u1 = u1 / w1;
  u2 = u2 / w2;
  u3 = u3 / w3;
  v1 = v1 / w1;
  v2 = v2 / w2;
  v3 = v3 / w3;
  w1 = 1.0 / w1;
  w2 = 1.0 / w2;
  w3 = 1.0 / w3;

  Vertex top(Vertex::xywuv(x1, y1, w1, u1, v1));
  Vertex mid(Vertex::xywuv(x2, y2, w2, u2, v2));
  Vertex bot(Vertex::xywuv(x3, y3, w3, u3, v3));

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

  const float dytopmid = mid.y() - top.y();  // Top to Mid
  const float dytopbot = bot.y() - top.y(); // Top to Bottom
  const float dymidbot = bot.y() - mid.y();  // Mid to Bottom

  // Check if triangle has 0 height
  if (dytopbot == 0.0) {
    return;
  }

  // Top to Bottom Steps
  const Vertex topbotstep(Vertex::xywuv(
   (bot.x() - top.x()) / std::abs(dytopbot),
   0,
   (bot.w() - top.w()) / std::abs(dytopbot),
   (bot.u() - top.u()) / std::abs(dytopbot),
   (bot.v() - top.v()) / std::abs(dytopbot)
  ));

  // The middle point on the top-bottom line
  Vertex mid2(Vertex::xywuv(
    top.x()+dytopmid*topbotstep.x(),
    top.y()+dytopmid,
    top.w()+dytopmid*topbotstep.w(),
    top.u()+dytopmid*topbotstep.u(),
    top.v()+dytopmid*topbotstep.v()
  ));

  // Make sure mid is left of mid2 because we will
  // draw the horizontal scan line from left-to-right
  if (mid.x() > mid2.x()) {
    std::swap(mid, mid2);
  }

  // Top Half Triangle
  if (dytopmid) {
    const Vertex leftStep(Vertex::xywuv(
      (mid.x() - top.x()) / std::abs(dytopmid),
      0,
      (mid.w() - top.w()) / std::abs(dytopmid),
      (mid.u() - top.u()) / std::abs(dytopmid),
      (mid.v() - top.v()) / std::abs(dytopmid)
    ));

    const Vertex rightStep(Vertex::xywuv(
      (mid2.x() - top.x()) / std::abs(dytopmid),
      0,
      (mid2.w() - top.w()) / std::abs(dytopmid),
      (mid2.u() - top.u()) / std::abs(dytopmid),
      (mid2.v() - top.v()) / std::abs(dytopmid)
    ));

    int ystart = top.y();
    int yend   = mid.y();
    if (ystart<0) ystart = 0;
    if (yend>=(int)imageData.height()) yend = imageData.height()-1;
    for (int y=ystart; y<=yend; y++) {
      const int ysteps = y-top.y();

      // Left Point
      const Vertex left(Vertex::xywuv(
        top.x()+ysteps*leftStep.x(),
        0,
        top.w()+ysteps*leftStep.w(),
        top.u()+ysteps*leftStep.u(),
        top.v()+ysteps*leftStep.v()
      ));

      // Right Point
      const Vertex right(Vertex::xywuv(
        top.x()+ysteps*rightStep.x(),
        0,
        top.w()+ysteps*rightStep.w(),
        top.u()+ysteps*rightStep.u(),
        top.v()+ysteps*rightStep.v()
      ));

      // Draw the horizontal line between left and right
      const int dx = right.x()-left.x();
      if (dx != 0) {
        const float ustep = (right.u()-left.u()) / dx;
        const float vstep = (right.v()-left.v()) / dx;
        const float wstep = (right.w()-left.w()) / dx;
        const int xstart = clipLeft(left.x());
        const int xend   = clipRight(right.x(), (int)imageData.width());
        for (int x=xstart; x<=xend; x++) {
          const int xsteps = x-left.x();
          const float u = left.u() + xsteps * ustep;
          const float v = left.v() + xsteps * vstep;
          const float w = left.w() + xsteps * wstep;
          const float z = 1/w;
          const float texu = u * z;
          const float texv = v * z;
          if (zbuffer) {
            const int oldz = zbuffer->get(x, y);
            if (oldz==0 || z<oldz) {
              zbuffer->set(x, y, z);
            }
            else {
              continue;
            }
          }
          imageData.pixel(x, y) = Graphics2D::pixelAtUV(textureImageData,
                                                        texu, texv);
        }
      }
    }
  }

  // Bottom Half Triangle
  if (dymidbot) {
     const Vertex leftStep(Vertex::xywuv(
      (bot.x() - mid.x()) / std::abs(dymidbot),
      0,
      (bot.w() - mid.w()) / std::abs(dymidbot),
      (bot.u() - mid.u()) / std::abs(dymidbot),
      (bot.v() - mid.v()) / std::abs(dymidbot)
    ));

    const Vertex rightStep(Vertex::xywuv(
      (bot.x() - mid2.x()) / std::abs(dymidbot),
      0,
      (bot.w() - mid2.w()) / std::abs(dymidbot),
      (bot.u() - mid2.u()) / std::abs(dymidbot),
      (bot.v() - mid2.v()) / std::abs(dymidbot)
    ));

    int ystart = mid.y();
    int yend   = bot.y();
    if (ystart<0) ystart = 0;
    if (yend>=(int) imageData.height()) yend = imageData.height()-1;
    for (int y=ystart; y<=yend; y++) {
      const int ysteps  = y - mid.y();

      // Left Point
      const Vertex left(Vertex::xywuv(
        mid.x()+ysteps*leftStep.x(),
        0,
        mid.w()+ysteps*leftStep.w(),
        mid.u()+ysteps*leftStep.u(),
        mid.v()+ysteps*leftStep.v()
      ));

      // Right Point
      const Vertex right(Vertex::xywuv(
        mid2.x()+ysteps*rightStep.x(),
        0,
        mid2.w()+ysteps*rightStep.w(),
        mid2.u()+ysteps*rightStep.u(),
        mid2.v()+ysteps*rightStep.v()
      ));

      // Draw the horizontal line between left and right
      const int dx = right.x()-left.x();
      if (dx != 0) {
        const float ustep = (right.u()-left.u()) / dx;
        const float vstep = (right.v()-left.v()) / dx;
        const float wstep = (right.w()-left.w()) / dx;
        const int xstart = clipLeft(left.x());
        const int xend   = clipRight(right.x(), (int)imageData.width());
        for (int x=xstart; x<=xend; x++) {
          const int xsteps = x-left.x();
          const float u = left.u() + xsteps * ustep;
          const float v = left.v() + xsteps * vstep;
          const float w = left.w() + xsteps * wstep;
          const float z = 1/w;
          const float texu = u * z;
          const float texv = v * z;
          if (zbuffer) {
            const int oldz = zbuffer->get(x, y);
            if (oldz==0 || z<oldz) {
              zbuffer->set(x, y, z);
            }
            else {
              continue;
            }
          }
          imageData.pixel(x, y) = Graphics2D::pixelAtUV(textureImageData,
                                                        texu, texv);
        }
      }
    }
  }
} // texturedTriangle