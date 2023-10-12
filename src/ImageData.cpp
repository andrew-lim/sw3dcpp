#include "ImageData.h"

using namespace al::graphics;

ImageData::ImageData()
: _w(), _h(), _data(), _needCleanUp()
{
}

ImageData::ImageData(u8* data, u32 w, u32 h, bool copyData)
: _w(), _h(), _data(), _needCleanUp()
{
  if (copyData) {
    copy(data, w, h);
  }
  else {
    assign(data, w, h);
  }
}

ImageData::ImageData(u32 w, u32 h)
: _w(), _h(), _data(), _needCleanUp()
{
  create(w, h);
}

ImageData::ImageData(const ImageData& other)
: _w(), _h(), _data(), _needCleanUp()
{
	copy(other._data, other._w, other._h);
}

ImageData& ImageData::operator=(const ImageData& other)
{
	copy(other._data, other._w, other._h);
	return *this;
}

ImageData::~ImageData()
{
  cleanup();
}

void ImageData::create(u32 w, u32 h)
{
  cleanup();
  _w = w;
  _h = h;
  _data = new u8[w*h*BYTES_PER_PIXEL]{0};

  _needCleanUp = true;
}

void ImageData::assign(u8* data, u32 w, u32 h)
{
  cleanup();
  _data = data;
  _w = w;
  _h = h;
}

void ImageData::copy(u8* data, u32 w, u32 h)
{
  cleanup();
  create(w, h);
  memcpy(_data, data, w*h*BYTES_PER_PIXEL);
}

void ImageData::cleanup()
{
  if (_needCleanUp && _data) {
    delete[] _data;
    _w = _h  = 0;
    _data = 0;
    _needCleanUp = false;
  }
}

void ImageData::clear()
{
  memset(_data, 0, _w*_h*BYTES_PER_PIXEL);
}

void ImageData::fill(u32 pixel)
{
  u32* data32 = (u32*)_data;
  for (u32 i=0; i<_w*_h; ++i) {
    data32[i] = pixel;
  }
}

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
      dst.pixel(dstX+x, dstY+y) = src.pixel(srcX+x, srcY+y);
    }
  }
}

void ImageData::swapPixelBytes(int i1, int i2)
{
  u32* data32 = (u32*)_data;
  for (u32 i=0; i<_w*_h; ++i) {
    swapBytes((u8*)(&data32[i]), i1, i2);
  }
}

void ImageData::fillAlpha(u8 alpha)
{
  u32* data32 = (u32*)_data;
  for (u32 i=0; i<_w*_h; ++i) {
    u8* bytes = (u8*)(&data32[i]);
    bytes[3] = alpha;
  }
}