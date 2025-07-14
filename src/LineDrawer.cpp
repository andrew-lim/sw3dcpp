#include "LineDrawer.h"
#include "Graphics3D.h"

using namespace al::graphics;
using namespace std;

void LineDrawer::bline(ImageData& imageData, int x0, int y0, int x1, int y1,
                       u32 rgba)
{
  const int imageW = (int) imageData.width();
  const int imageH = (int) imageData.height();

  int a[2] = {x0, y0};
  int b[2] = {x1, y1};

  if (G3D::clipLineToRect(a, b, 0, 0, imageW-1, imageH-1)) {
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
    imageData.setPixel(x0, y0, rgba);
    if ((x0 == x1) && (y0 == y1)) break;
    int e2 = 2*err;
    if (e2 > -dy) { err -= dy; x0  += sx; }
    if (e2 < dx) { err += dx; y0  += sy; }
  }
}