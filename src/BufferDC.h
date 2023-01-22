#ifndef DANTE_BUFFER_DC
#define DANTE_BUFFER_DC
#include <windows.h>

/*
*  The BufferDC class can be used to create device contexts suitable for
*  offscreen drawing.
*
*  Author: Dante Shamest
*/
class BufferDC
{
public:
 BufferDC();
 bool create( int width, int height ) ;
 void destroy() ;
 void draw( HDC hDC );

 virtual ~BufferDC();
 operator HDC()  { return _hdc; }
 int getWidth()  { return _width; }
 int getHeight() { return _height; }
protected:
 HDC     _hdc;
 int     _width;
 int     _height;
 HBITMAP _hBitmapOld;
private:
 BufferDC( const BufferDC& );
 void operator=( const BufferDC& );
};

#endif
