#include "Font.h"

//------------------------------------------------------------------------------
// Font Constructors & Destructors
//------------------------------------------------------------------------------

Font::Font()
  : hfont((HFONT)GetStockObject(DEFAULT_GUI_FONT)) {
}

Font::Font( LPCTSTR name, long height, bool bold, bool italic )
  : hfont((HFONT)GetStockObject(DEFAULT_GUI_FONT)) {
  create( name, height, bold, italic );
}

Font::Font( HFONT hFont )
  : hfont((HFONT)GetStockObject(DEFAULT_GUI_FONT)) {
  operator=( hFont );
}

Font::Font( const Font& font )
  : hfont((HFONT)GetStockObject(DEFAULT_GUI_FONT)) {
  operator=( font.hfont );
}

Font::~Font() {
  free() ;
}

//------------------------------------------------------------------------------
// Font Operators
//------------------------------------------------------------------------------

Font& Font::operator=( const Font& font ) {
  return operator=( font.hfont );
}

Font& Font::operator=( HFONT hfontToCopy ) {
  if ( hfontToCopy ) {
    LOGFONT lf ;
    // Get font properties.
    if ( GetObject( hfontToCopy, sizeof(LOGFONT), &lf ) ) {
      // Create a new font with the same properties.
      if ( (hfontToCopy = CreateFontIndirect( &lf )) ) {
        // Delete old font and make handle point to new font.
        free() ;
        hfont = hfontToCopy ;
      }
    }
  }
  return *this;
}

//------------------------------------------------------------------------------
// Font Member Functions
//------------------------------------------------------------------------------

bool Font::create( LPCTSTR name, long height, bool bold, bool italic ) {
  // Add bold flag if necessary.
  int fnWeight = bold ? FW_BOLD	: 0;

  // Calculate height.
  HDC hDC = GetDC(NULL);
  height = -MulDiv(height, GetDeviceCaps(hDC, LOGPIXELSY), 72);
  ReleaseDC(NULL, hDC);

  // Create new font.
  HFONT hfontNew = CreateFont( height, 0, 0, 0,fnWeight, italic ? TRUE : FALSE,
                               0, 0, 0, 0, 0, 0, 0, name);

  // If new font created, free old font and make font handle point to new font.
  if ( hfontNew ) {
    free() ;
    hfont = hfontNew ;
    return true ;
  }

  return false ;
}

void Font::free() {
  if ( hfont ) {
    DeleteObject( hfont );
    hfont = NULL ;
  }
}

int Font::getHeight() const {
  LOGFONT lf = { 0 } ;
  GetObject( this->getHandle(), sizeof(LOGFONT), &lf );
  HDC hdc = GetDC( NULL );
  HFONT old = (HFONT) SelectObject( hdc, this->getHandle() );
  TEXTMETRIC tm ;
  GetTextMetrics(hdc, &tm);
  lf.lfHeight= MulDiv( tm.tmHeight - tm.tmInternalLeading, 72,
                       GetDeviceCaps(hdc, LOGPIXELSY));
  SelectObject( hdc, old );
  ReleaseDC( NULL, hdc );
  return lf.lfHeight ;
}

bool Font::isBold() const {
  LOGFONT lf = { 0 } ;
  GetObject( this->getHandle(), sizeof(LOGFONT), &lf );
  return lf.lfWeight >= FW_BOLD ;
}

bool Font::isItalic() const {
  LOGFONT lf = { 0 } ;
  GetObject( this->getHandle(), sizeof(LOGFONT), &lf );
  return lf.lfItalic != 0 ;

  // Don't do lf.lfItalic == TRUE, despite what MSDN says.
  // It doesn't seem to work.
}

void Font::setWindowFont( HWND hwnd, bool redraw ) {
  SendMessage( hwnd, WM_SETFONT, (WPARAM)hfont, redraw ? TRUE : FALSE );
}

bool Font::showFontDialog( HWND hwndOwner ) {
  LOGFONT lf ;
  GetObject( hfont, sizeof(lf), &lf );

  CHOOSEFONT cf = { 0 } ;
  cf.lStructSize    = sizeof (CHOOSEFONT) ;
  cf.hwndOwner      = hwndOwner ;
  cf.hDC            = NULL ;
  cf.lpLogFont      = &lf ;
  cf.Flags          = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS;

  if ( ChooseFont(&cf) ) {
    HFONT hfontNew = CreateFontIndirect( &lf ) ;
    if ( hfontNew ) {
      free() ;
      hfont = hfontNew ;
      return true ;
    }
  }

  return false ;
}

std::string Font::getFontName() {
  LOGFONT lf ;
  GetObject( hfont, sizeof(lf), &lf );
  return lf.lfFaceName ;
}
