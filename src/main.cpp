#include <windows.h>
#include <exception>
#include "Game.h"

int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrev, LPSTR args, int nShow )
{
  try
  {
    Game game ;
    return game.run() ;
  }
  catch( std::exception& ex )
  {
    MessageBox( NULL, ex.what(), "Exception Caught!", MB_OK ) ;
    return 0 ;
  }
}

