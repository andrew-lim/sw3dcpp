#ifndef ANDREW_LIM_GRAPHICS_LINE_DRAWER_H
#define ANDREW_LIM_GRAPHICS_LINE_DRAWER_H
#include "ImageData.h"

namespace al { namespace graphics {

class LineDrawer
{
public:

  // Bresenham line drawing
  static void bline(ImageData& imageData,
                    int x0, int y0, int x1, int y1, u32 rgba=0);

}; // class LineDrawer
} // namespace al
} // namespace graphics

#endif