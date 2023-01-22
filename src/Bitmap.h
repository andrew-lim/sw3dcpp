// Copyright © 2005-2006 Andrew Lim

#ifndef BITMAP_H
#define BITMAP_H
#include <windows.h>

class Bitmap {
public:

  Bitmap();
  ~Bitmap();
  void  blit( HDC hdcDest, int nXDest=0, int nYDest=0, DWORD dwRop=SRCCOPY );
  void  blit( HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight,
              int nXSrc, int nYSrc, DWORD dwRop= SRCCOPY );
  bool  loadFile(LPCTSTR fileName );
  bool  loadResource( LPCTSTR resourceName );
  int   getWidth() const { return width ; }
  int   getHeight() const { return height ; }
  bool  isLoaded() const { return hBitmap != NULL ; }
  int getBits(LPVOID lpvBits);
private:
  
  Bitmap( const Bitmap& ) ;
  Bitmap& operator=( const Bitmap& ) ;
  
  int     width, height;
  HDC     hDC ;
  HBITMAP hBitmap ;
  HBITMAP hBitmapOld ;
  void    cleanup();
  
};

#endif
