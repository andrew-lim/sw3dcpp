#include <windows.h>
#include <stdexcept>
#include <algorithm>
#include <ctime>
#include <tchar.h>
#include <cstdlib>
#include <array>
#include "Game.h"
#include "Window.h"
#include "Bitmap.h"
#include "BufferDC.h"
#include "ImageData.h"
#include "Graphics2D.h"
#include "Vertex.h"
#include "Triangle.h"
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi
#include <glm/gtx/string_cast.hpp>
#include <vector>
#include <exception>
#include <map>
#include "Menu.h"
#include "Tests.h"
#include "ClipSpace.h"
#include "OBJLoader.h"
#include "FileDialog.h"
#include "lodepng.h"
#include "Camera.h"
#include <iostream>
using namespace std ;
using namespace al::graphics;
using namespace glm;

#define CLIENT_WIDTH 848
#define CLIENT_HEIGHT 480
#define MOVE_SPEED 4
#define ROT_SPEED (deg2rad(2))
#define WM_MM_TIMER WM_USER + 1    //  Custom message sent by the timer.
const int DESIRED_FPS = 60;
const int UPDATE_INTERVAL = 1000/DESIRED_FPS;
#define TIMER_DELAY UPDATE_INTERVAL             //  Refresh rate in milliseconds.

// glm::mat4 camera(float Translate, glm::vec2 const& Rotate)
// {
// 	glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.f);
// 	glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Translate));
// 	View = glm::rotate(View, Rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
// 	View = glm::rotate(View, Rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
// 	glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
// 	return Projection * View * Model;
// }

glm::vec4 clipToNDC(glm::vec4 clip)
{
  float w = clip[3];
  glm::vec4 ndc;
  if (w!=0.0) {
    ndc = glm::vec4(
      clip[0]/w, clip[1]/w, clip[2]/w, 1.0/w
    );
  }
  return ndc;
}

glm::vec4 ndcToWindow(glm::vec4 point, float windowWidth, float windowHeight)
{
  float xNDC = point[0];
  float yNDC = point[1];
  float zNDC = point[2];

  // Hack to make sure right and bottom edges are really clipped
  // to prevent drawing outside viewport
//  windowWidth = windowWidth-1;
//  windowHeight = windowHeight-1;

  int xWindow = (windowWidth/2*xNDC + windowWidth/2);
  int yWindow = ((-yNDC)*windowHeight/2 + windowHeight/2);
  float zWindow = zNDC;
  return glm::vec4(xWindow, yWindow, zWindow, 1);
}

//------------------------------------------------------------------------------
//  GameImpl Definition
//------------------------------------------------------------------------------

class GameImpl : public Window {
public:
  GameImpl();
  ~GameImpl();
  int run() ;

private:
  enum MenuIDs {
      MIExit = 1000
    , MIReset
    , MIOpenOBJ
    , MINothing
    , MISolid
    , MIAffine
    , MIPerspectiveCorrect
    , MIWireframe
    , MIZBuffer
    , MIBackfacCulling
    , MIBlackBackground
    , MIBlueBackground
    , MI640x360
    , MI640x480
    , MI800x600
    , MI848x480
    , MI1024x768
    , MI1280x720
    , MI1366x768
    , MI1920x1080
    , MIScale001
    , MIScale01
    , MIScale05
    , MIScale1
    , MIScale2
    , MIScale4
    , MIScale8
    , MIScale16
    , MIScale32
    , MIScale64
    , MIScale128
    , MIScale256
    , MIClipAll
    , MIClipNearOnly
    , MITexture1 = 1100
    , MIMeshDefault = 1200
    , MIMesh0
    , MIMesh1
    , MIControls
    , MIAbout
  };
  
  enum DrawType {
    DrawPerspectiveCorrect = 0,
    DrawAffine = 1,
    DrawSolid = 2,
    DrawNothing = 3
  };
  
  int _drawType = DrawPerspectiveCorrect;
  bool _drawWireframe;
  int _textureID;

private:
  LRESULT  handleMessage( UINT, WPARAM, LPARAM );
  void     repaint() ;
  void     createDefaultMesh();
  void     createScaledMesh(vector<Triangle>&);
  void     drawFPS(HDC hdc);
  void     drawWorld(HDC hdc);
  void updateGame();
  void resetGame();
  void fpsChanged(int fps);
  void updateFPS();
  void startTimer();
  void stopTimer();
  void prepareScreenImageData();
  void loadImages();
  void createMenus();
  void showOpenOBJDialog();
  void showControls();
  void showAbout();
  static void CALLBACK TimeProc(UINT uID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR d1, DWORD_PTR d2);
  
  BITMAPINFOHEADER _bmih;
  BITMAPINFO _dbmi;
  BufferDC          bufferDC ;
  ImageData         screenImageData;

  bool              _backfaceCullingOn;
  bool              _zbufferOn;
  vector<ImageData> _textureImageDatas;

  int _clientWidth = CLIENT_WIDTH;
  int _clientHeight = CLIENT_HEIGHT;

  int           iTimerId;            //  Multimedia timer id.
  float _xrot, _yrot;
  DWORD  _currentFPS, _fps, _pastFps, _past;
  static HWND hwndMain;
  std::map<int, BOOL> _keys;
  float _xStep = 4;
  float _yStep = 4;
  float _zStep = 4;
  float _scale = 1;
  float _oldScale = 1;
  Grid<float> zbuffer;
  vector<Triangle> _mesh, _defaultMesh, _loadedMesh, _scaledMesh;
  Menu _menuBar, _mainMenu, _optionsMenu, _textureMenu, _meshMenu, _clipMenu,
       _helpMenu;
  OBJLoader _loader;
  uint32_t _backgroundColor = 0;
  Camera _camera;
  static const int CLIP_NEAR_ONLY = 0;
  static const int CLIP_ALL = 1;
  int _clipMode = CLIP_NEAR_ONLY;
};

//------------------------------------------------------------------------------
//  GameImpl Constructors & Destructors
//------------------------------------------------------------------------------

HWND GameImpl::hwndMain;

GameImpl::GameImpl()
  : Window( TEXT("Andrew Lim's' 3D Software Rasterizer"), NULL,
           WS_OVERLAPPEDWINDOW&~(WS_MAXIMIZEBOX|WS_THICKFRAME), 0 )
  , _backfaceCullingOn(true)
  , _zbufferOn(true)
  , _xrot()
  , _yrot()
  , _currentFPS()
  , _fps()
  , _pastFps()
  , _past()
  , _menuBar()
  , _mainMenu( true )
  , _optionsMenu( true )
  , _textureMenu(true)
  , _meshMenu(true)
  , _clipMenu(true)
  , _helpMenu(true)
  , _loader(true)
{
  _textureID = 0;
  _drawWireframe = true;
//  _backgroundColor=ImageData::makeLittlePixel(64, 145, 250);
  
  srand( (unsigned int)time(0) );
  printf("DESIRED_FPS=%d\n", DESIRED_FPS);
  printf("TIMER_DELAY=%d\n", TIMER_DELAY);
  
  _pastFps = _past = GetTickCount();
  printf("_pastFps=%lu\n, _past=%lu\n", _pastFps, _past);
  
  GameImpl::hwndMain = hwnd;
  
  Tests::run();
  createDefaultMesh();
  loadImages();
  createMenus();

  setClientSize(this->hwnd, _clientWidth, _clientHeight);
  centerWindow(this->hwnd);
  ShowWindow( hwnd, SW_SHOWNORMAL ) ;
  
//  startTimer();
  resetGame();

}

GameImpl::~GameImpl()
{}

//------------------------------------------------------------------------------
//  Operators
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//  GameImpl Member Functions
//------------------------------------------------------------------------------

/*
 *  Callback function for the multimedia timer.
 */
void CALLBACK GameImpl::TimeProc(UINT uID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR d1, DWORD_PTR d2) {
  PostMessage( hwndMain, WM_MM_TIMER, 0, 0 );
}

/*
 *  Starts the multimedia timer.
 */
void GameImpl::startTimer() {
  TIMECAPS tc;
  timeGetDevCaps(&tc, sizeof(TIMECAPS));
  DWORD resolution = std::min( std::max(tc.wPeriodMin, (UINT)0 ),tc.wPeriodMax);
  timeBeginPeriod( resolution );
  iTimerId = timeSetEvent( TIMER_DELAY,resolution,TimeProc,0,TIME_PERIODIC );
}

/*
 *  Stops the multimedia timer.
 */
void GameImpl::stopTimer() {
  timeKillEvent( iTimerId );
  timeEndPeriod( TIMER_DELAY );
}

void GameImpl::createMenus()
{
  _mainMenu.add("Reset\tR", MIReset ) ;
  _mainMenu.add("Open OBJ...", MIOpenOBJ ) ;
  _mainMenu.addSeparator() ;
  _mainMenu.add("Exit", MIExit ) ;

  _optionsMenu.add("Nothing / Wireframe Only", MINothing);
  _optionsMenu.add("Solid", MISolid);
  _optionsMenu.add("Affine", MIAffine);
  _optionsMenu.add("Perspective Correct", MIPerspectiveCorrect);
  _optionsMenu.addSeparator();
  _optionsMenu.add("Wireframe", MIWireframe);
  _optionsMenu.add("Backface Culling", MIBackfacCulling);
  _optionsMenu.add("Z Buffer", MIZBuffer);
  _optionsMenu.addSeparator();
  _optionsMenu.add("640x360", MI640x360);
  _optionsMenu.add("640x480", MI640x480);
  _optionsMenu.add("800x600", MI800x600);
  _optionsMenu.add("848x480", MI848x480);
  _optionsMenu.add("1024x768", MI1024x768);
  _optionsMenu.add("1280x720", MI1280x720);
  _optionsMenu.add("1366x768", MI1366x768);
  _optionsMenu.add("1920x1080", MI1920x1080);
  _optionsMenu.addSeparator();
  _optionsMenu.add("Black Background", MIBlackBackground);
  _optionsMenu.add("Blue Background", MIBlueBackground);

  _textureMenu.add("Show Default Cube", MIMeshDefault ) ;
  for (size_t i=0; i<_textureImageDatas.size(); ++i) {
    TCHAR s[128];
    sprintf(s, "Cube Texture %d", (int)(i+1));
    _textureMenu.add(s, MITexture1 + i);
  }

  _meshMenu.add("Scale x0.01", MIScale001);
  _meshMenu.add("Scale x0.1", MIScale01);
  _meshMenu.add("Scale x0.5", MIScale05);
  _meshMenu.add("Scale x1", MIScale1);
  _meshMenu.add("Scale x2", MIScale2);
  _meshMenu.add("Scale x4", MIScale4);
  _meshMenu.add("Scale x8", MIScale8);
  _meshMenu.add("Scale x16", MIScale16);
  _meshMenu.add("Scale x32", MIScale32);
  _meshMenu.add("Scale x64", MIScale64);
  _meshMenu.add("Scale x128", MIScale128);
  _meshMenu.add("Scale x256", MIScale256);

  _clipMenu.add("Clip All Planes", MIClipAll);
  _clipMenu.add("Clip Near Plane Only", MIClipNearOnly);

  _helpMenu.add("Controls...", MIControls);
  _helpMenu.add("About...", MIAbout);

  _menuBar.add( _mainMenu, "Main" ) ;
  _menuBar.add( _optionsMenu, "Options" ) ;
  _menuBar.add( _textureMenu, "Debug" ) ;
  _menuBar.add( _meshMenu, "Scale" ) ;
  _menuBar.add( _clipMenu, "Clip");
  _menuBar.add( _helpMenu, "Help");
  _menuBar.attachToWindow( hwnd ) ;
  _optionsMenu.setMenuItemChecked( MIPerspectiveCorrect, true ) ;
}

void GameImpl::prepareScreenImageData()
{
  _bmih = {0};
  _bmih.biSize     = sizeof(BITMAPINFOHEADER);
  _bmih.biWidth    = _clientWidth;
  _bmih.biHeight   = -_clientHeight;
  _bmih.biPlanes   = 1;
  _bmih.biBitCount = 32;
  _bmih.biCompression  = BI_RGB ;
  _bmih.biSizeImage    = 0;
  _bmih.biXPelsPerMeter    =   10;
  _bmih.biYPelsPerMeter    =   10;
//  bmih.biClrUsed    =0;
//  bmih.biClrImportant =0;

  _dbmi = {0};
  _dbmi.bmiHeader = _bmih;
//  dbmi.bmiColors->rgbBlue = 0;
//  dbmi.bmiColors->rgbGreen = 0;
//  dbmi.bmiColors->rgbRed = 0;
//  dbmi.bmiColors->rgbReserved = 0;
  screenImageData.create(_clientWidth, _clientHeight);
}

void GameImpl::loadImages()
{
  vector<string> paths;
  paths.push_back("..\\res\\texture1.png");
  paths.push_back("..\\res\\texture2.bmp");
  paths.push_back("..\\res\\texture3.bmp");
  paths.push_back("..\\res\\texture4.bmp");
  paths.push_back("..\\res\\texture5.bmp");
  paths.push_back("..\\res\\texture6.bmp");
  paths.push_back("..\\res\\texture7.bmp");
  for (size_t i=0 ;i<paths.size(); ++i) {
    string path = paths[i];
    printf("Loading image %s\n", path.c_str());

    OBJLoader loader;
    ImageData textureData;

    // PNG
    if (path.find(".png") != std::string::npos) {

      loader.loadPNG(path, textureData);
      _textureImageDatas.push_back(textureData);
    }
    // BMP
    else {
      loader.loadBMP(path, textureData);
      _textureImageDatas.push_back(textureData);
    }

  }
}

// Convert a wide Unicode string to an UTF8 string
// https://stackoverflow.com/a/3999597/1645045
std::string utf8_encode(const std::wstring &wstr)
{
    if( wstr.empty() ) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], -1, NULL, 0, NULL, NULL);
    std::string strTo( size_needed, 0 );
    WideCharToMultiByte                  (CP_UTF8, 0, &wstr[0], -1, &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

std::string shortPathName(const wchar_t* path)
{
  wchar_t shortpath[256] = {0};

  int ok = GetShortPathNameW(path, shortpath, 256);
  if (!ok) {
    printf("GetShortPathName failed with result %d\n", ok);
    int lastError = GetLastError();
    printf("GetLastError()=%d\n", lastError);
  }

  return utf8_encode(shortpath);
}

static std::wstring filepath;

void GameImpl::showOpenOBJDialog()
{
  OpenFileDialog dialog ;
  dialog.setFilter((TCHAR*)TEXT("OBJ Files\0*.obj\0\0"));
  dialog.filter = (wchar_t*)L"OBJ Files\0*.obj\0\0";
  if ( dialog.showDialogW( this->hwnd ) ) {
    try {
      wchar_t* path = dialog.fileName;
      filepath = path;
      string shortpath = shortPathName(path);
      cout << "shortpath=" << shortpath << endl;

      vector<Triangle> mesh;
      _loader.load(shortpath, 1.0f);
      if (0==_loader.triangles.size()) {
        throw runtime_error("No triangles loaded");
      }
      stopTimer();
      _loadedMesh = _loader.triangles;
      _scale = _oldScale = 1;
      createScaledMesh(_loadedMesh);
      resetGame();
    }
    catch (exception& ex) {
      string msg = "Error loading ";
      msg += dialog.getFileName();
      msg += " Exception=" + string(ex.what());
      MessageBox( NULL, msg.c_str(), "Error", MB_OK ) ;
    }
  }
}

void GameImpl::showControls()
{
  MessageBox(hwnd, TEXT("Camera Controls:\n")
                   TEXT("WASD - move forwards, backwards and strafe sideways\n")
                   TEXT("Arrow Keys - rotate camera\n")
                   TEXT("R - back to origin"), TEXT("Controls"), MB_OK);
}

void GameImpl::showAbout()
{
  MessageBox(hwnd, TEXT("Andrew Lim's' 3D Software Rasterizer\n")
                   TEXT("https://github.com/andrew-lim/sw3dcpp"),
                   TEXT("About"), MB_OK);
}

LRESULT GameImpl::handleMessage( UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch ( msg )
  {
    case WM_CLOSE:
      stopTimer();
      PostQuitMessage( 0 ) ; 
      return 0 ;
      
    case WM_COMMAND:
    {
      WORD wID = LOWORD( wParam ) ;
      
      if (wID >= MITexture1) {
        int tmp = wID - MITexture1;
        if (tmp>=0 && tmp<(int)_textureImageDatas.size()) {
          _textureID = tmp;
        }
      }

      if (wID == MIMeshDefault) {
        _loadedMesh.clear();
        createScaledMesh(_defaultMesh);
        _scale = 1;
        resetGame();
      }
      
      switch ( wID )
      {
        case MIReset: {
          resetGame();
          break;
        }

        case MIOpenOBJ:
          showOpenOBJDialog();
          break;

        case MINothing:
          _drawType = DrawNothing;
          _drawWireframe = true;
          break;
        
        case MISolid:
          _drawType = DrawSolid;
          break ; 
        
        case MIPerspectiveCorrect:
          _drawType = DrawPerspectiveCorrect;
          break ; 
        
        case MIAffine:
          _drawType = DrawAffine;
          break ; 
          
        case MIWireframe:
          _drawWireframe = !_drawWireframe;
          break;

        case MIZBuffer:
          _zbufferOn = !_zbufferOn;
          break;

        case MIBackfacCulling:
          _backfaceCullingOn = !_backfaceCullingOn;
          break;

        case MI640x360: _clientWidth=640, _clientHeight=360; resetGame(); break;
        case MI640x480: _clientWidth=640, _clientHeight=480; resetGame(); break;
        case MI800x600: _clientWidth=800, _clientHeight=600; resetGame(); break;
        case MI848x480: _clientWidth=848, _clientHeight=480; resetGame(); break;
        case MI1024x768: _clientWidth=1024, _clientHeight=768; resetGame(); break;
        case MI1280x720: _clientWidth=1280, _clientHeight=720; resetGame(); break;
        case MI1366x768: _clientWidth=1366, _clientHeight=768; resetGame(); break;
        case MI1920x1080: _clientWidth=1920, _clientHeight=1080; resetGame(); break;

        case MIScale001: _scale=0.01f; break;
        case MIScale01: _scale=0.1f; break;
        case MIScale05: _scale=0.5f; break;
        case MIScale1: _scale=1; break;
        case MIScale2: _scale=2; break;
        case MIScale4: _scale=4; break;
        case MIScale8: _scale=8; break;
        case MIScale16: _scale=16; break;
        case MIScale32: _scale=32; break;
        case MIScale64: _scale=64; break;
        case MIScale128: _scale=128; break;
        case MIScale256: _scale=256; break;

        case MIBlackBackground: _backgroundColor = 0; break;
        case MIBlueBackground: _backgroundColor=ImageData::makeLittlePixel(64, 145, 250); break;

        case MIClipAll: _clipMode = CLIP_ALL; break;
        case MIClipNearOnly: _clipMode = CLIP_NEAR_ONLY; break;

        case MIControls: showControls(); break;
        case MIAbout: showAbout(); break;
        
        case MIExit: {
          stopTimer();
          PostQuitMessage( 0 ); 
          break;
        }
      }
      return 0 ; 
    }
    
    case WM_KEYUP:
    {
      WORD key = static_cast<WORD>( wParam );
      _keys[key] = false;
      return 0 ; 
    }
    
    case WM_KEYDOWN:
    {
      WORD key = static_cast<WORD>( wParam );
      _keys[key] = true;
      if (_keys['R']) {
        resetGame();
      }
      return 0 ; 
    }
    
    case WM_MM_TIMER:
    {   
      updateFPS();
      updateGame();
      InvalidateRect(hwnd,NULL,FALSE);
      return 0;
    }
    
    case WM_INITMENUPOPUP:
    {
      HMENU hMenu = (HMENU) wParam ;

      if ( hMenu == _optionsMenu )
      {
        _optionsMenu.setMenuItemChecked( MINothing, _drawType == DrawNothing );
        _optionsMenu.setMenuItemChecked( MISolid, _drawType == DrawSolid );
        _optionsMenu.setMenuItemChecked( MIAffine, _drawType == DrawAffine );
        _optionsMenu.setMenuItemChecked( MIPerspectiveCorrect, _drawType == DrawPerspectiveCorrect );
        _optionsMenu.setMenuItemChecked( MIWireframe, _drawWireframe );
        _optionsMenu.setMenuItemChecked( MIZBuffer, _zbufferOn );
        _optionsMenu.setMenuItemChecked( MIBackfacCulling, _backfaceCullingOn );
        _optionsMenu.setMenuItemChecked( MI640x360, _clientWidth==640 && _clientHeight==360 );
        _optionsMenu.setMenuItemChecked( MI640x480, _clientWidth==640 && _clientHeight==480);
        _optionsMenu.setMenuItemChecked( MI800x600, _clientWidth==800 && _clientHeight==600);
        _optionsMenu.setMenuItemChecked( MI1024x768, _clientWidth==1024 && _clientHeight==768);
      }
      else if (hMenu == _textureMenu) {
        // _textureMenu.setMenuItemChecked( MITexture1+_textureID, true);
      }
      else if (hMenu == _meshMenu) {
        _meshMenu.setMenuItemChecked( MIScale001, _scale==0.01f);
        _meshMenu.setMenuItemChecked( MIScale01, _scale==0.1f);
        _meshMenu.setMenuItemChecked( MIScale05, _scale==0.5f);
        _meshMenu.setMenuItemChecked( MIScale1, _scale==1.0f);
        _meshMenu.setMenuItemChecked( MIScale1, _scale==1.0f);
        _meshMenu.setMenuItemChecked( MIScale2, _scale==2.0f);
        _meshMenu.setMenuItemChecked( MIScale4, _scale==4.0f);
        _meshMenu.setMenuItemChecked( MIScale8, _scale==8.0f);
        _meshMenu.setMenuItemChecked( MIScale16, _scale==16.0f);
        _meshMenu.setMenuItemChecked( MIScale32, _scale==32.0f);
        _meshMenu.setMenuItemChecked( MIScale64, _scale==64.0f);
      }
      else if (hMenu == _clipMenu) {
        _clipMenu.setMenuItemChecked( MIClipAll, _clipMode==CLIP_ALL);
        _clipMenu.setMenuItemChecked( MIClipNearOnly, _clipMode==CLIP_NEAR_ONLY);
      }

      return 0 ; 
    }

    case WM_PAINT:
    {
      PAINTSTRUCT ps ;
      HDC hDC = BeginPaint( hwnd, &ps ) ;
      drawWorld(bufferDC);
      drawFPS(bufferDC);
      bufferDC.draw( hDC );
      
      EndPaint( hwnd, &ps ) ;
      return 0 ;
    }

    
    default: 
      return ::DefWindowProc( hwnd, msg, wParam, lParam ) ;
  }
}

int GameImpl::run()
{
  MSG msg ;
  while ( GetMessage( &msg, 0, 0, 0 ) > 0 )
  {
    TranslateMessage( &msg ) ; 
    DispatchMessage( &msg ) ;
  }
  return (int) msg.wParam ;
}

void GameImpl::repaint()
{
  InvalidateRect( hwnd, NULL, FALSE ) ;
}

void GameImpl::createDefaultMesh()
{
  _defaultMesh.clear();

  float leftX   = -64;
  float rightX  = 64;
  float topY    = 64;
  float bottomY = -64;
  float frontZ  = 64;
  float backZ   = frontZ - 128;

  uint32_t pink   = ImageData::makeLittlePixel(255, 192, 203, 255);
  uint32_t green  = ImageData::makeLittlePixel(0, 255, 0, 255);
  uint32_t red    = ImageData::makeLittlePixel(255, 0, 0, 255);
  uint32_t blue   = ImageData::makeLittlePixel(0, 0, 255, 255);
  uint32_t yellow = ImageData::makeLittlePixel(255, 255, 0, 255);
  uint32_t violet = ImageData::makeLittlePixel(98, 88, 124, 255);

  Triangle t;

  // South
  t = Triangle(leftX, topY, frontZ, leftX, bottomY, frontZ, rightX, topY, frontZ, pink);
  t.setTexUVs(0.0, 0.0, 0.0, -2.0, 2.0, -0.0);
  _defaultMesh.push_back(t);

  t = Triangle(leftX, bottomY, frontZ, rightX, bottomY, frontZ, rightX, topY, frontZ, pink);
  t.setTexUVs(0.0, -2.0, 2.0, -2.0, 2.0, -0.0);
  _defaultMesh.push_back(t);

  // East
  t = Triangle(rightX, topY, frontZ, rightX, bottomY, frontZ, rightX, topY, backZ, green);
  t.setTexUVs(0.0, 0.0, 0.0, -2.0, 2.0, 0.0);
  _defaultMesh.push_back(t);
  t = Triangle(rightX, bottomY, frontZ, rightX, bottomY, backZ, rightX, topY, backZ, green);
  t.setTexUVs(0.0, -2.0, 2.0, -2.0, 2.0, 0.0);
  _defaultMesh.push_back(t);

  // North
  t = Triangle(rightX, topY, backZ, rightX, bottomY, backZ, leftX, topY, backZ, blue);
  t.setTexUVs(0.0, 0.0, 0.0, -2.0, 2.0, 0.0);
  _defaultMesh.push_back(t);
  t = Triangle(rightX, bottomY, backZ, leftX, bottomY, backZ, leftX, topY, backZ, blue);
  t.setTexUVs(0.0, -2.0, 2.0, -2.0, 2.0, 0.0);
  _defaultMesh.push_back(t);

  // West
  t = Triangle(leftX, topY, backZ, leftX, bottomY, backZ, leftX, topY, frontZ, red);
  t.setTexUVs(0.0, 0.0, 0.0, -2.0, 2.0, 0.0);
  _defaultMesh.push_back(t);
  t = Triangle(leftX, bottomY, backZ, leftX, bottomY, frontZ, leftX, topY, frontZ, red);
  t.setTexUVs(0.0, -2.0, 2.0, -2.0, 2.0, 0.0);
  _defaultMesh.push_back(t);

  // Top
  t = Triangle(leftX, topY, backZ, leftX, topY, frontZ, rightX, topY, backZ, yellow);
  t.setTexUVs(0.0, 0.0, 0.0, -2.0, 2.0, 0.0);
  _defaultMesh.push_back(t);
  t = Triangle(leftX, topY, frontZ, rightX, topY, frontZ, rightX, topY, backZ, yellow);
  t.setTexUVs(0.0, -2.0, 2.0, -2.0, 2.0, 0.0);
  _defaultMesh.push_back(t);

  // Bottom
  t = Triangle(leftX, bottomY, frontZ, leftX, bottomY, backZ, rightX, bottomY, frontZ, violet);
  t.setTexUVs(0.0, 0.0, 0.0, -2.0, 2.0, 0.0);
  _defaultMesh.push_back(t);
  t = Triangle(leftX, bottomY, backZ, rightX, bottomY, backZ, rightX, bottomY, frontZ, violet);
  t.setTexUVs(0.0, -2.0, 2.0, -2.0, 2.0, 0.0);
  _defaultMesh.push_back(t);

  createScaledMesh(_defaultMesh);
}

void GameImpl::createScaledMesh(vector<Triangle>& sourceMesh)
{
  _scaledMesh.clear();
  for (size_t i=0; i<sourceMesh.size(); ++i) {
    Triangle t = sourceMesh[i];
    t.scale(_scale);
    _scaledMesh.push_back(t);
  }
  _oldScale = _scale;
}

void GameImpl::fpsChanged(int fps)
{
  _currentFPS = fps;
}

void GameImpl::updateFPS()
{
  int now = GetTickCount();
  int timeElapsed = now - _past ;
  if (timeElapsed > UPDATE_INTERVAL) {
    // do something
    
  }
  // fps
  if ( now - _pastFps >= 1000 ) {
      _pastFps = now ;
      fpsChanged(_fps);
      _fps = 0 ;
  }
  _past = now;
}

void GameImpl::resetGame()
{
  stopTimer();
  setClientSize(this->hwnd, _clientWidth, _clientHeight);

  _camera.resetToOrigin();
  _camera.zeroRotation();
  _camera.x = -200;
  _camera.moveSpeed = MOVE_SPEED;
  _camera.rotSpeed = ROT_SPEED;

  _xrot = 0;
  _yrot = 0;
  bufferDC.create(_clientWidth, _clientHeight);
  zbuffer = Grid<float>(_clientWidth, _clientHeight);
  prepareScreenImageData();
  startTimer();
}

void GameImpl::updateGame()
{
  const bool left  = _keys[VK_LEFT];
  const bool right = _keys[VK_RIGHT];
  const bool forwards = _keys['W'];
  const bool backwards = _keys['S'];
  const bool strafeLeft = _keys['A'];
  const bool strafeRight = _keys['D'];
  const bool up = _keys['E'];
  const bool down = _keys['Q'];
  const bool lookup = _keys[VK_UP];
  const bool lookdown = _keys[VK_DOWN];
  float distance = 0;
  float vdistance = 0;
  float strafeMagnitude = 0;
  float drotation = 0;
  float strafeRotation = 0;

  // let timeBasedFactor = timeElapsed / UPDATE_INTERVAL;
  float timeBasedFactor = 1;

  // Rotation to add
  if (left) {
    drotation = _camera.rotSpeed;
  }
  else if (right) {
    drotation = -_camera.rotSpeed;
  }

  // Distance to travel
  if (forwards) {
    distance = _camera.moveSpeed;
  }
  else if (backwards) {
    distance = -_camera.moveSpeed;
  }

  // Vertical Distance
  if (up) {
    vdistance = _camera.moveSpeed;
  }
  else if (down) {
    vdistance = -_camera.moveSpeed;
  }

  // Calculate movement vector without strafing first
  _camera.rot += drotation * timeBasedFactor;
  float dx = cos(_camera.rot) * distance * timeBasedFactor;
  float dy = sin(_camera.rot) * distance * timeBasedFactor;
  float dz = vdistance;

  // Strafing left/right is just adding/subtracting 90 degrees to the
  // player's rotation
   if (strafeLeft) {
     strafeMagnitude = _camera.moveSpeed;
     strafeRotation  = _camera.rot + M_PI / 2;
   }
   else if (strafeRight) {
     strafeMagnitude = _camera.moveSpeed;
     strafeRotation  = _camera.rot - M_PI / 2;
   }

   // Add strafing vector to movement vector
   dx += cos(strafeRotation) * strafeMagnitude * timeBasedFactor ;
   dy += sin(strafeRotation) * strafeMagnitude * timeBasedFactor;

  // Cartesian Y to Window Y
  dy = -dy;

  const float newX = _camera.x + dx;
  const float newY = _camera.y + dy;
  const float newZ = _camera.z + dz;

  _camera.x = newX;
  _camera.y = newY;
  _camera.z = newZ;

  // Vertical Look
  const float MAX_LOOK_UP = deg2rad(55);
  const float MAX_LOOK_DOWN = deg2rad(-55);
  if (lookup) {
    _camera.vrot += _camera.rotSpeed * timeBasedFactor;
    if (_camera.vrot > MAX_LOOK_UP) {
      _camera.vrot = MAX_LOOK_UP;
    }
  }
  else if (lookdown) {
    _camera.vrot -= _camera.rotSpeed * timeBasedFactor;
    if (_camera.vrot < MAX_LOOK_DOWN) {
      _camera.vrot = MAX_LOOK_DOWN;
    }
  }
}

void GameImpl::drawFPS(HDC hdc)
{
  TCHAR fpsText[64];
  sprintf(fpsText, "FPS: %lu", _currentFPS);
  
  RECT rc = {0} ;
  GetClientRect( hwnd, &rc ) ;

  DrawText(hdc, fpsText, -1, &rc, DT_LEFT|DT_TOP);

  if (!filepath.empty()) {
    DrawTextW(hdc, filepath.c_str(), -1, &rc, DT_LEFT|DT_BOTTOM|DT_SINGLELINE);
  }
}

void GameImpl::drawWorld(HDC hdc)
{
  screenImageData.fill(_backgroundColor);

  zbuffer.fill(numeric_limits<float>::infinity());

  if (_oldScale != _scale) {
    printf("Scale changed, recreating scaled mesh\n");
    createScaledMesh(_loadedMesh.empty() ? _defaultMesh : _loadedMesh);
  }
  _mesh = _scaledMesh;
  for (size_t i=0; i<_mesh.size(); i++) {
    Triangle& t = _mesh[i];
    t.rotateX(deg2rad(_xrot));
    t.rotateY(deg2rad(_yrot));
  }

  for (size_t i=0; i<_mesh.size(); ++i) {
    Triangle* t = &_mesh[i];

    float fovydeg = 90.0f;
    float fovyrad = deg2rad(fovydeg);
    float znear = 1.f;
    float zfar = 1000.f;

    glm::vec3 moveVector = Camera::xyzToZXY(-_camera.x, -_camera.y, -_camera.z);
    glm::mat4 translate = glm::translate(glm::mat4(1), moveVector);
    glm::mat4 proj = glm::perspective(fovyrad, _clientWidth/(float)_clientHeight, znear, zfar);
    glm::mat4 rotateY = glm::rotate(glm::mat4(1), _camera.rot, glm::vec3(0.0f, -1.0f, 0.0f));
    glm::mat4 rotateX = glm::rotate(glm::mat4(1), _camera.vrot, glm::vec3(-1.0f, 0.0f, 0.0f));
    glm::mat4 viewProj = proj * rotateX * rotateY * translate;

    Vertex* vertices = t->getVertices();
    glm::vec4 v1 = vertices[0].pos().vec4();
    glm::vec4 v2 = vertices[1].pos().vec4();
    glm::vec4 v3 = vertices[2].pos().vec4();

    glm::vec4 clip1 = viewProj * v1;
    glm::vec4 clip2 = viewProj * v2;
    glm::vec4 clip3 = viewProj * v3;

    // Create a triangle with clip coordinates
    Vertex clipVertex1 = Vertex(Vector4f(clip1), t->texcoord(0));
    Vertex clipVertex2 = Vertex(Vector4f(clip2), t->texcoord(1));
    Vertex clipVertex3 = Vertex(Vector4f(clip3), t->texcoord(2));
    Triangle triangleToClip(clipVertex1, clipVertex2, clipVertex3);
    triangleToClip.color() = t->color();

    vector<Triangle> trianglesToClip;
    trianglesToClip.push_back(triangleToClip);

    // Clip the triangle to the frustrum volume
    vector<Triangle> clippedTriangles = (_clipMode == CLIP_ALL)
      ? ClipSpace::clipByAllPlanes(trianglesToClip)
      : ClipSpace::clipByNearPlane(trianglesToClip)
      ;

    for (size_t i=0; i<clippedTriangles.size(); ++i) {
      Triangle triangle = clippedTriangles[i];

      glm::vec4 ndc1 = clipToNDC(triangle.vertex(0).pos().vec4());
      glm::vec4 ndc2 = clipToNDC(triangle.vertex(1).pos().vec4());
      glm::vec4 ndc3 = clipToNDC(triangle.vertex(2).pos().vec4());

      float xprod = 0;
      if (_backfaceCullingOn) {
        if ((xprod=Graphics2D::crossProduct2D<glm::vec4>(ndc1, ndc2, ndc3))<0) {
          continue;
        }
      }

      glm::vec4 win1 = ndcToWindow(ndc1, _clientWidth, _clientHeight);
      glm::vec4 win2 = ndcToWindow(ndc2, _clientWidth, _clientHeight);
      glm::vec4 win3 = ndcToWindow(ndc3, _clientWidth, _clientHeight);

      static bool runOnce = false;
      if (!runOnce && i==0) {
        runOnce = true;
        printf("fovydeg=%f\n,", fovydeg);
        printf("fovyrad=%f\n,", fovyrad);
        printf("znear=%f\n,", znear);
        printf("zfar=%f\n,", zfar);
        printf("viewport width=%d", _clientWidth);
        printf("viewport height=%d", _clientHeight);
        printf("t->color()=%d\n", t->color());
        printf("v1 = %s\n", glm::to_string(v1).c_str());
        printf("translate = %s\n", glm::to_string(translate).c_str());
        printf("proj = %s\n", glm::to_string(proj).c_str());
        printf("viewProj = %s\n", glm::to_string(viewProj).c_str());
        printf("clip1 = %s\n", glm::to_string(clip1).c_str());
        printf("ndc1 = %s\n", glm::to_string(ndc1).c_str());
        printf("win1 = %s\n", glm::to_string(win1).c_str());
        printf("xprod=%f\n", xprod);
      }

      if (_drawType==DrawSolid) {
        Graphics2D::fillTriangle(screenImageData,
                                 win1[0], win1[1],
                                 win2[0], win2[1],
                                 win3[0], win3[1],
                                 t->color());
      }

      else if (_drawType==DrawAffine) {
        ImageData* textureData = &_textureImageDatas[_textureID];
        if (-1 != t->textureID) {
          if (_loader.textures.count(t->textureID)) {
            ImageData* tmptextureData = &(_loader.textures[t->textureID]);
            if (tmptextureData) {
              textureData = tmptextureData;
            }
          }
        }
        Graphics2D::affineTriangle(screenImageData,
                                   win1[0], win1[1], triangle.getTexU(0), triangle.getTexV(0),
                                   win2[0], win2[1], triangle.getTexU(1), triangle.getTexV(1),
                                   win3[0], win3[1], triangle.getTexU(2), triangle.getTexV(2),
                                   *textureData);
      }

      else if (_drawType==DrawPerspectiveCorrect) {
        ImageData* textureData = &_textureImageDatas[_textureID];
        if (-1 != t->textureID) {
          if (_loader.textures.count(t->textureID)) {
            ImageData* tmptextureData = &(_loader.textures[t->textureID]);
            if (tmptextureData) {
              textureData = tmptextureData;
            }
          }
        }
        if (textureData) {
          float x1 = win1[0], y1 = win1[1];
          float x2 = win2[0], y2 = win2[1];
          float x3 = win3[0], y3 = win3[1];
//          textureData->blit(screenImageData, 100, 100, 0, 0, 100, 100);
           Graphics2D::texturedTriangle(screenImageData,
                                      x1, y1, triangle.getW(0), triangle.getTexU(0), triangle.getTexV(0),
                                      x2, y2, triangle.getW(1), triangle.getTexU(1), triangle.getTexV(1),
                                      x3, y3, triangle.getW(2), triangle.getTexU(2), triangle.getTexV(2),
                                      *textureData,
                                      (_zbufferOn ? &zbuffer:0));

        }
      }

      else {
        // draw nothing
      }

      if (_drawWireframe) {
        Graphics2D::triangleWire(screenImageData,
                                 win1[0], win1[1],
                                 win2[0], win2[1],
                                 win3[0], win3[1],
                                 ImageData::makePixel(0, 255, 255));
      }
    }
  } // for mesh.size()
  
  SetDIBitsToDevice(hdc,
    0, 0, screenImageData.width(), screenImageData.height(), 
    0, 0, 0, screenImageData.height(),
    screenImageData.data(), &_dbmi, 0
  );

  _fps++;
}

//------------------------------------------------------------------------------
//  Game Constructors & Destructors
//------------------------------------------------------------------------------

Game::Game()
  : pimpl( new GameImpl )
{}

Game::~Game()
{
  delete pimpl ;
}

//------------------------------------------------------------------------------
//  Game Member Functions
//------------------------------------------------------------------------------

int Game::run()
{
  return pimpl->run() ;
}
