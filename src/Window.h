#ifndef WINDOW_H
#define WINDOW_H
#include <windows.h>
class Window {
public:
  Window( LPCTSTR text, HWND hwndParent, DWORD style, DWORD exstyle ) ;
  virtual ~Window();
  virtual LRESULT handleMessage( UINT, WPARAM, LPARAM );
  static void centerWindow( HWND hwnd );
  static void drawText( HDC hDC, LPCTSTR lpString, RECT& rc );
  static void setClientSize( HWND hwnd, int clientWidth, int clientHeight );
protected:
  HWND hwnd ; // initialized in WM_NCCREATE
private:
  bool canDestroy ;  
  static LRESULT CALLBACK wndproc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );
  Window( const Window& );
  Window& operator= ( const Window& );  
};
#endif
