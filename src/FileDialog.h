#ifndef DANTE_FILEDIALOG_H
#define DANTE_FILEDIALOG_H
#include <windows.h>

/**
 *  Abstract class <code>FileDialog</code> displays a dialog window from
 *  which the user can select a file.
 *
 *  @author Andrew Lim
 */
class FileDialog {

protected:

  TCHAR m_fileTitle[MAX_PATH] ;
  TCHAR m_fileName[MAX_PATH] ;
  TCHAR* m_filter ;

public:

  /**
   *  Creates a <code>FileDialog</code> with owner window
   *  <code>hwndOwner</code>.
   *
   *  @param hwndOwner  the owner window.
   */
  FileDialog()
  {
    m_filter = (TCHAR*)TEXT("*.*\0*.*\0") ;

    m_fileTitle[0] = TEXT('\0');
    m_fileName[0]  = TEXT('\0');
  }

  /**
   *  Destroys the <code>FileDialog</code>.
   */
  ~FileDialog()
  {
  }

  /**
   *  Returns the name of the most recent selected file.
   */
  const TCHAR* getFileName() const;

  /**
   *  Returns the title of the most recent selected file.
   */
  const TCHAR* getFileTitle() const;

  void setFilter(TCHAR* filter);

};

//------------------------------------------------------------------------------
//
//  Inlined Members
//
//------------------------------------------------------------------------------

inline void FileDialog::setFilter(TCHAR* filter)
{
  m_filter = filter;
}

inline const TCHAR* FileDialog::getFileName() const
{
  return m_fileName ;
}

inline const TCHAR* FileDialog::getFileTitle() const
{
  return m_fileTitle ;
}

/**
 *  Represents a common dialog box that displays the control that allows the
 *  user to open a file.
 *
 *  @author Andrew Lim
 */
class OpenFileDialog : public FileDialog
{
public:

  bool showDialog( HWND hwndOwner = NULL );

};

/**
 *  Represents a common dialog box that allows the user to specify options for
 *  saving a file.
 *
 *  @author Andrew Lim
 */
class SaveFileDialog : public FileDialog
{
public:

  bool showDialog( HWND hwndOwner = NULL );

};

#endif
