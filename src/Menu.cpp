#include <stdexcept>
#include "Menu.h"

//------------------------------------------------------------------------------
//  Constructors & Destructors
//------------------------------------------------------------------------------

Menu::Menu( bool bPopUp )
  : hMenu( bPopUp ? CreatePopupMenu() : CreateMenu() )
{
  if ( NULL == hMenu )
  {
    throw std::runtime_error( "Error creating menu!" ) ; 
  }
}

Menu::~Menu()
{
  DestroyMenu( hMenu ) ; 
}

//------------------------------------------------------------------------------
//  Member Functions
//------------------------------------------------------------------------------

void Menu::add( LPCTSTR itemText, int id )
{
  ::AppendMenu( hMenu, 
                MF_STRING, 
                id,
                itemText );
}

void Menu::add( const Menu& menu, LPCTSTR text )
{
  ::AppendMenu( hMenu, 
                MF_POPUP,
                reinterpret_cast<UINT_PTR>(menu.hMenu), 
                text );
}

void Menu::addSeparator()
{
  ::AppendMenu( hMenu, MF_SEPARATOR, 0, 0 ); 
}

void Menu::attachToWindow( HWND hwnd )
{
  ::SetMenu( hwnd, hMenu ); 
}

void Menu::setMenuItemChecked( int id, bool state )
{
  ::CheckMenuItem( hMenu, id, MF_BYCOMMAND|(state?MF_CHECKED:MF_UNCHECKED));
}  

void Menu::setMenuItemEnabled( int id, bool state )
{
  ::EnableMenuItem( hMenu, id, (state?MF_ENABLED:MF_GRAYED) ); 
}  


