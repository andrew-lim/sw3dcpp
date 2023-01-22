#ifndef MENU_H
#define MENU_H
#include <windows.h>

class Menu {
public:
  Menu(bool bPopUp = false);
  virtual ~Menu();
  operator HMENU() const { return hMenu; }
  
  void add( LPCTSTR itemText, int id ) ;
  void add( const Menu& menu, LPCTSTR text ) ;
  void addSeparator() ;
  void attachToWindow( HWND hwnd ) ;
  void setMenuItemChecked( int id, bool state ) ;
  void setMenuItemEnabled( int id, bool state ) ;  
private:
  HMENU hMenu ;
  Menu( const Menu& );
  Menu& operator= ( const Menu& );    
};

#endif
