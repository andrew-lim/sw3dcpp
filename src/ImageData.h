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
  u32 w_, h_, length_;
  u8* data_;
  bool needCleanUp_;
public:
  static const int BYTES_PER_PIXEL = 4;

  ImageData();
  ImageData(u8* data, u32 w, u32 h, bool copyData);
  ImageData(u32 w, u32 h);
  ImageData(const ImageData& other);
  ImageData& operator=(const ImageData& other);
  ~ImageData();

  u32 width() const { return w_; }
  u32 height() const { return h_; }
  u32 length() const { return length_; }
  u8* data() { return data_; }

  void create(u32 w, u32 h);
  void assign(u8* data, u32 w, u32 h);
  void copy(u8* data, u32 w, u32 h);
  void cleanup();

  u32 getPixel(u32 x, u32 y) const;
  void setPixel(u32 x, u32 y, u32 pixel);
  static u32 makePixel(u8 c1, u8 c2, u8 c3, u8 c4=0);
  void clear();
  void fill(u32 pixel);
  static void swapBytes(void* in, int i1, int i2);
  void swapPixelBytes(int i1, int i2);
  void fillAlpha(u8 alpha);
  void blit(ImageData& dst, u32 dstX, u32 dstY,
            u32 srcX=0, u32 srcY=0, u32 srcW=0, u32 srcH=0);

}; // class ImageData

inline ImageData::ImageData()
: w_(), h_(), length_(), data_(), needCleanUp_()
{
}

inline ImageData::ImageData(u8* data, u32 w, u32 h, bool copyData)
: w_(), h_(), length_(), data_(), needCleanUp_()
{
  if (copyData) {
    copy(data, w, h);
  }
  else {
    assign(data, w, h);
  }
}

inline ImageData::ImageData(u32 w, u32 h)
: w_(), h_(), length_(), data_(), needCleanUp_()
{
  create(w, h);
}

inline ImageData::ImageData(const ImageData& other)
: w_(), h_(), length_(), data_(), needCleanUp_()
{
	copy(other.data_, other.w_, other.h_);
}

inline ImageData& ImageData::operator=(const ImageData& other)
{
	copy(other.data_, other.w_, other.h_);
	return *this;
}

inline ImageData::~ImageData()
{
  cleanup();
}

inline void ImageData::create(u32 w, u32 h)
{
  cleanup();
  w_ = w;
  h_ = h;
  length_ = w * h;
  data_ = new u8[length_*BYTES_PER_PIXEL]{0};

  needCleanUp_ = true;
}

inline void ImageData::assign(u8* data, u32 w, u32 h)
{
  cleanup();
  data_ = data;
  w_ = w;
  h_ = h;
  length_ = w * h;
}

inline void ImageData::copy(u8* data, u32 w, u32 h)
{
  cleanup();
  create(w, h);
  memcpy(data_, data, length_*BYTES_PER_PIXEL);
}

inline void ImageData::cleanup()
{
  if (needCleanUp_ && data_) {
    delete[] data_;
    w_ = h_ = length_ = 0;
    data_ = 0;
    needCleanUp_ = false;
  }
}

inline u32 ImageData::getPixel(u32 x, u32 y) const
{
  const int i = (y*w_+x)*BYTES_PER_PIXEL;
  u32 pixel;
  memcpy(&pixel, &data_[i], 4);
  return pixel;

  // Old way below is unsafe because of possible alignment issues
  // u32& pixel = *((u32*)&data_[i]);
  // https://stackoverflow.com/a/38990924/1645045
}

inline void ImageData::setPixel(u32 x, u32 y, u32 pixel)
{
  const int i = (y*w_+x)*BYTES_PER_PIXEL;
  memcpy(&data_[i], &pixel, 4);
}

inline u32 ImageData::makePixel(u8 c1, u8 c2, u8 c3, u8 c4)
{
  const u8 bytes[4] = {c1, c2, c3, c4};
  u32 pixel;
  memcpy(&pixel, bytes, BYTES_PER_PIXEL);
  return pixel;
}

inline void ImageData::clear()
{
  memset(data_, 0, length_*BYTES_PER_PIXEL);
}

inline void ImageData::fill(u32 pixel)
{
  for (u32 i=0; i<length_; ++i) {
    const int index = i*4;
    memcpy(&data_[index], &pixel, 4);
  }
}

inline void ImageData::swapBytes(void* in, int i1, int i2)
{
  u8* bytes = (u8*) in;
  u8& a = bytes[i1];
  u8& b = bytes[i2];
  const u8 tmp = a;
  a = b;
  b = tmp;
}

inline void ImageData::swapPixelBytes(int i1, int i2)
{
  for (u32 i=0; i<length_; ++i) {
    const int index = i*BYTES_PER_PIXEL;
    swapBytes(&data_[index], i1, i2);
  }
}

inline void ImageData::fillAlpha(u8 alpha)
{
  for (u32 i=0; i<length_; ++i) {
    const int index = i*BYTES_PER_PIXEL;
    data_[index + 3] = alpha;
  }
}

} // namespace al
} // namespace graphics
#endif
