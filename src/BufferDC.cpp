#include "BufferDC.h"

BufferDC::BufferDC()
 : _hdc(0),
   _width(0),
   _height(0),
   _hBitmapOld(0)
{}

BufferDC::~BufferDC()
{
  destroy() ;
}

bool BufferDC::create( int width, int height )
{
  destroy() ;
  
  HBITMAP hBitmap;
   
  HDC screenDC = GetDC( HWND_DESKTOP );        
  _hdc         = CreateCompatibleDC( screenDC );
  hBitmap      = CreateCompatibleBitmap( screenDC, width, height );
  _hBitmapOld  = reinterpret_cast<HBITMAP>( SelectObject(_hdc,hBitmap) );
  ReleaseDC( HWND_DESKTOP, screenDC );     
  
  _width = width ;
  _height = height ;
  
  return hBitmap != 0 ;
}

void BufferDC::destroy()
{
 if( _hdc )
 {
   _hBitmapOld = reinterpret_cast<HBITMAP>( SelectObject(_hdc,_hBitmapOld) );
   DeleteDC( _hdc );
   DeleteObject( _hBitmapOld );
 }
 _hdc = NULL;
}

void BufferDC::draw( HDC hDC )
{
   BitBlt( hDC, 0, 0, _width, _height, _hdc, 0, 0, SRCCOPY ) ;  
}
