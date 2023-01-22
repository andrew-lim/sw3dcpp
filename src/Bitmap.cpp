// Copyright © 2005-2006 Andrew Lim
#include <cstdio>
#include "Bitmap.h"

//------------------------------------------------------------------------------
// Constructors & Destructors
//------------------------------------------------------------------------------

Bitmap::Bitmap()
  : width( 0 )
  , height( 0 )
  , hDC( NULL )
  , hBitmap( NULL )
  , hBitmapOld( NULL )
{}

Bitmap::~Bitmap()
{
  cleanup();
}

//------------------------------------------------------------------------------
// Member Functions
//------------------------------------------------------------------------------

void Bitmap::blit( HDC hdcDest, int nXDest, int nYDest, DWORD dwRop )
{
  BitBlt( hdcDest, nXDest, nYDest, width, height, hDC, 0, 0, dwRop );
}

void Bitmap::blit( HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight,
                   int nXSrc, int nYSrc, DWORD dwRop )
{
  BitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, hDC, nXSrc, nYSrc, dwRop);
}

void Bitmap::cleanup()
{
  //----------------------------------------------------------------------------
  // Select the monochrome bitmap back into the compatible DC, at the same time
  // saving the bitmap.
  //----------------------------------------------------------------------------
  if ( hDC && hBitmap ) {
    hBitmap = reinterpret_cast<HBITMAP>( SelectObject(hDC,hBitmapOld) );
  }
  
  //---------------------------
  // Delete the compatible DC.
  //---------------------------
  if ( hDC ) {
    DeleteDC( hDC );
    hDC = 0;
  }
  
  //--------------------
  // Delete the bitmap.
  //--------------------
  if ( hBitmap ) {
    DeleteObject( hBitmap );
    hBitmap = 0;
  }
}

bool Bitmap::loadFile(LPCTSTR fileName)
{
  cleanup();

  //--------------------------------
  // Load the bitmap from the file.
  //--------------------------------
  hBitmap = reinterpret_cast<HBITMAP>( LoadImage( GetModuleHandle(0),
                                                  fileName,IMAGE_BITMAP,0,0,
                                                  LR_LOADFROMFILE ) );

  //-----------------------------
  // Check if bitmap was loaded.
  //-----------------------------
  if ( !hBitmap ) return false;
  
  //-------------------------------------
  // Save the bitmap's width and height.
  //-------------------------------------
  BITMAP bm;
  GetObject( hBitmap, sizeof( bm ), &bm );
  width  = bm.bmWidth ;
  height = bm.bmHeight ;

  //-----------------------------------------------------
  // Create the screen compatible DC to hold the bitmap.
  //-----------------------------------------------------
  HDC screenDC = GetDC( HWND_DESKTOP );                                 
  hDC = CreateCompatibleDC( screenDC );        
  ReleaseDC( HWND_DESKTOP, screenDC );
  if ( !hDC ) { cleanup(); return false; }
  
  //------------------------------------------------------------------------
  // Select the bitmap into the compatible DC, at the same time saving the 
  // monochrome bitmap.
  //------------------------------------------------------------------------
  hBitmapOld = reinterpret_cast<HBITMAP>( SelectObject(hDC,hBitmap) );
  
  return true ;
}

bool Bitmap::loadResource( LPCTSTR resourceName )
{
  cleanup();

  //--------------------------------
  // Load the bitmap from the file.
  //--------------------------------
  hBitmap = reinterpret_cast<HBITMAP>( LoadBitmap(GetModuleHandle(0),
                                       resourceName) );

  //-----------------------------
  // Check if bitmap was loaded.
  //-----------------------------
  if ( !hBitmap ) return false;

  //-------------------------------------
  // Save the bitmap's width and height.
  //-------------------------------------
  BITMAP bm;
  GetObject( hBitmap, sizeof( bm ), &bm );
  width  = bm.bmWidth ;
  height = bm.bmHeight ;

  //-----------------------------------------------------
  // Create the screen compatible DC to hold the bitmap.
  //-----------------------------------------------------
  HDC screenDC = GetDC( HWND_DESKTOP );
  hDC = CreateCompatibleDC( screenDC );
  ReleaseDC( HWND_DESKTOP, screenDC );
  if ( !hDC ) { cleanup(); return false; }

  //------------------------------------------------------------------------
  // Select the bitmap into the compatible DC, at the same time saving the
  // monochrome bitmap.
  //------------------------------------------------------------------------
  hBitmapOld = reinterpret_cast<HBITMAP>( SelectObject(hDC,hBitmap) );

  return true ;
}

int Bitmap::getBits(LPVOID lpvBits)
{
  BITMAP bm;
  GetObject( hBitmap, sizeof( bm ), &bm );
  width  = bm.bmWidth ;
  height = bm.bmHeight ;
  
  BITMAPINFO info = {0};
  
  info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  info.bmiHeader.biWidth = width = bm.bmWidth;
  info.bmiHeader.biHeight = -bm.bmHeight; // flip vertical axis
  info.bmiHeader.biPlanes = 1;
  info.bmiHeader.biBitCount = bm.bmBitsPixel;
  info.bmiHeader.biCompression = BI_RGB;
  //info.bmiHeader.biSizeImage = 0;// ((width * bm.bmBitsPixel + 31) / 32) * 4 * height;
  
  return GetDIBits(hDC, hBitmap, 0, height, lpvBits, &info, DIB_RGB_COLORS);
}
