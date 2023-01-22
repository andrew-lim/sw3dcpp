#include <stdexcept>
#include "Window.h"

//------------------------------------------------------------------------------
//  Constructors & Destructors
//------------------------------------------------------------------------------

Window::Window( LPCTSTR text, HWND hwndParent, DWORD dwStyle, DWORD dwExStyle )
  : hwnd( NULL )
  , canDestroy( false )
{
  WNDCLASS wc = {0};
  wc.lpszClassName = TEXT( "AndrewLim" );
  wc.hInstance     = GetModuleHandle(0) ;
  //wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
  wc.lpfnWndProc   = wndproc ;
  wc.hCursor       = LoadCursor(0,IDC_ARROW);
  
  RegisterClass(&wc);  
  
  CreateWindowEx(dwExStyle,wc.lpszClassName,text,dwStyle,
                 0,0,0,0,hwndParent,0,GetModuleHandle(0),this);
                 
  if ( NULL == hwnd )
    throw std::runtime_error( "Error creating window!" ) ;                 
}

Window::~Window()
{
  canDestroy = true ;
  DestroyWindow( hwnd ) ;
}

//------------------------------------------------------------------------------
//  Member Functions
//------------------------------------------------------------------------------

LRESULT Window::handleMessage( UINT msg, WPARAM wParam, LPARAM lParam )
{
  if ( msg == WM_CLOSE )
  {
    return 0 ; 
  }
  return ::DefWindowProc( hwnd, msg, wParam, lParam ) ;
}

void Window::centerWindow( HWND hwnd )
{
  if ( IsWindow(hwnd) )
  {
    RECT rc ;

    GetWindowRect ( hwnd, &rc ) ;

    SetWindowPos( hwnd, 0,
                  (GetSystemMetrics(SM_CXSCREEN) - rc.right)/2,
                  (GetSystemMetrics(SM_CYSCREEN) - rc.bottom)/2,
                  0, 0, SWP_NOZORDER|SWP_NOSIZE );
  }
}

void Window::drawText( HDC hDC, LPCTSTR lpString, RECT& rc )
{
  DrawText( hDC, lpString, -1, &rc, DT_SINGLELINE | DT_VCENTER | DT_CENTER );
}

void Window::setClientSize( HWND hwnd, int clientWidth, int clientHeight )
{
  if ( IsWindow( hwnd ) )
  {
    DWORD dwStyle = (DWORD)GetWindowLongPtr( hwnd, GWL_STYLE ) ;
    DWORD dwExStyle = (DWORD)GetWindowLongPtr( hwnd, GWL_EXSTYLE ) ;
    HMENU menu = GetMenu( hwnd ) ;

    RECT rc = { 0, 0, clientWidth, clientHeight } ;

    AdjustWindowRectEx( &rc, dwStyle, menu ? TRUE : FALSE, dwExStyle );

    SetWindowPos( hwnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top,
                  SWP_NOZORDER | SWP_NOMOVE ) ;
  }
}

//------------------------------------------------------------------------------
//  Static Functions & Data
//------------------------------------------------------------------------------

LRESULT CALLBACK 
Window::wndproc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
  Window* self = (Window*)(LONG_PTR)GetWindowLongPtr(hwnd, GWLP_USERDATA) ;
  
  if (msg == WM_NCCREATE)
  {
    self = (Window*)(((LPCREATESTRUCT)lParam)->lpCreateParams);    
    self->hwnd = hwnd ;    
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG)(LONG_PTR)(self));
  } 
  
  else if ( WM_DESTROY == msg )
  {
    if ( !self->canDestroy )    
    {
      throw std::runtime_error( "Window destroyed outside destructor!" ) ; 
    }
  }
  
  if ( self )
    return self->handleMessage(msg, wParam, lParam);    
    
  return DefWindowProc(hwnd, msg, wParam, lParam);
}
