#include "FileDialog.h"



bool OpenFileDialog::showDialog( HWND hwndOwner )
{
  OPENFILENAME ofn = {0};

  ofn.lStructSize    = sizeof(OPENFILENAME);
  ofn.lpstrTitle     = TEXT("") ;
  ofn.lpstrFileTitle = m_fileTitle ;
  ofn.hwndOwner      = hwndOwner ;
  ofn.lpstrFilter    = m_filter;
  ofn.lpstrFile      = m_fileName;
  ofn.nMaxFile       = MAX_PATH;
  ofn.nMaxFileTitle  = MAX_PATH;
  ofn.Flags          = OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_NOCHANGEDIR ;

  return ::GetOpenFileName(&ofn) != 0 ;
}

bool SaveFileDialog::showDialog( HWND hwndOwner )
{
  OPENFILENAME ofn = {0};

  ofn.lStructSize    = sizeof(OPENFILENAME);
  ofn.lpstrTitle     = TEXT("") ;
  ofn.lpstrFileTitle = m_fileTitle ;
  ofn.hwndOwner      = hwndOwner ;
  ofn.lpstrFilter    = m_filter;
  ofn.lpstrFile      = m_fileName;
  ofn.nMaxFile       = MAX_PATH;
  ofn.nMaxFileTitle  = MAX_PATH;
  ofn.Flags          = OFN_PATHMUSTEXIST|OFN_HIDEREADONLY|
                       OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;

  return ::GetSaveFileName(&ofn) != 0 ;
}
