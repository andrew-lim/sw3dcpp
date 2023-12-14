#ifndef FONT_H
#define FONT_H
#include <windows.h>
#include <string>

/**
  HFONT Wrapper.

  Usage:
  {
    // create a Font object.
    Font font ;

    // Create Courier New 10-point font.
    font.create( "Courier New", 10 );

    // Get handle to font.
    HFONT hFont = font.getHandle() ;

  } // Out of scope; Font destroyed
 */
class Font {
public:
  Font();
  Font( LPCTSTR name, long height, bool bold = false, bool italic = false ) ;
  Font( const Font& f );
  Font( HFONT hFont );
  ~Font();
  Font& operator=( const Font& f );
  Font& operator=( HFONT hFont );
  bool create(LPCTSTR name, long height, bool bold = false, bool italic=false) ;
  void free() ;
  HFONT getHandle() const { return hfont ; }
  int getHeight() const ;
  bool isBold() const ;
  bool isItalic() const ;
  void setWindowFont( HWND hwnd, bool redraw );
  bool showFontDialog( HWND hwndOwner ) ;
  std::string getFontName() ;
private:
  HFONT hfont ;
};

#endif

