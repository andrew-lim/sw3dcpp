#include "ImageData.h"

using namespace al::graphics;

void ImageData::blit(ImageData& dst, u32 dstX, u32 dstY,
                     u32 srcX, u32 srcY, u32 srcW, u32 srcH)
{
  const ImageData& src = *this;
  if (srcW==0) {
    srcW = src.width();
  }
  if (srcH==0) {
    srcH = src.height();
  }
  for (u32 x=0; x>=0 && x<srcW &&  x<dst.width(); ++x) {
    for (u32 y=0; y>=0 && y<srcH && y<dst.height(); ++y) {
      dst.setPixel(dstX+x, dstY+y, src.getPixel(srcX+x, srcY+y));
    }
  }
}
