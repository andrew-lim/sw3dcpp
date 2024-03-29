//  Author: Andrew Lim (https://github.com/andrew-lim)
#ifndef ANDREWLIM_IMAGEDATA_H
#define ANDREWLIM_IMAGEDATA_H
#include <cstring> // for memcpy
#include <cstdint> // for uint8_t, uint32_t

namespace al { namespace graphics {

typedef std::uint8_t  u8;
typedef std::uint32_t u32;

// Manages an array of bytes containing pixels
// Each pixel is assumed to be made of 4 bytes (32 bits)
class ImageData
{
private:
  
  u32 _w, _h;
  u8* _data;
  bool _needCleanUp;

public:

  static const int BYTES_PER_PIXEL = 4;
  static const int BITS_PER_PIXEL = 32;
  
  ImageData();
  ImageData(u8* data, u32 w, u32 h, bool copyData);
  ImageData(u32 w, u32 h);
  ImageData(const ImageData& other);
  ImageData& operator=(const ImageData& other);
  ~ImageData();

  u32 width() const { return _w; }
  u32 height() const { return _h; }
  u8* data() { return _data; }

  void create(u32 w, u32 h);
  void assign(u8* data, u32 w, u32 h);
  void copy(u8* data, u32 w, u32 h);
  void cleanup();

  const u32& pixel(u32 x, u32 y) const
  {
    const int i = (y*_w+x)*BYTES_PER_PIXEL;
    u32& pixel = *((u32*)&_data[i]);
    return pixel;
  }

  u32& pixel(u32 x, u32 y)
  {
    const int i = (y*_w+x)*BYTES_PER_PIXEL;
    u32& pixel = *((u32*)&_data[i]);
    return pixel;

    // below works but looks pretty hacky
    // const_cast - https://stackoverflow.com/a/856839/1645045
    // return const_cast<u32&>(const_cast<const ImageData*>(this)->pixel(x,y));
  }

  void setPixel(u32 x, u32 y, u32 pixel)
  {
    this->pixel(x,y) = pixel;
  }

  static u32 makePixel(u8 c1, u8 c2, u8 c3, u8 c4=0)
  {
    const u8 bytes[BYTES_PER_PIXEL] = {c1, c2, c3, c4};
    return *((u32*)&bytes);
  }

  void clear();
  void fill(u32 pixel);

  void blit(ImageData& dst, u32 dstX, u32 dstY,
            u32 srcX=0, u32 srcY=0, u32 srcW=0, u32 srcH=0);

  static void swapBytes(void* in, int i1, int i2)
  {
    u8* bytes = (u8*) in;
    u8& a = bytes[i1];
    u8& b = bytes[i2];
    const u8 tmp = a;
    a = b;
    b = tmp;
  }

  void swapPixelBytes(int i1, int i2);
  void fillAlpha(u8 alpha);

}; // class ImageData
} // namespace al
} // namespace graphics
#endif
