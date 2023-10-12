#include <windows.h>
#include <stdexcept>
#include <algorithm>
#include <ctime>
#include <tchar.h>
#include <cstdlib>
#include "Game.h"
#include "Window.h"
#include "Bitmap.h"
#include "BufferDC.h"
#include "ImageData.h"
#include "Graphics3D.h"
#include "Vertex.h"
#include "Triangle.h"
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
#include "TriangleDrawer.h"
#include <iostream>
#include "Matrix.h"
using namespace std ;
using namespace al::graphics;

#define CLIENT_WIDTH 848
#define CLIENT_HEIGHT 480
#define MOVE_SPEED 4
#define ROT_SPEED (G3D::deg2rad(2))
#define LIGHT_SPEED_DEG 1.0
#define LIGHT_ROT_SPEED (G3D::deg2rad(LIGHT_SPEED_DEG))
#define WM_MM_TIMER WM_USER + 1    //  Custom message sent by the timer.
#define SOLID_COLOR (ImageData::makePixel(0, 255, 0, 255))
const int DESIRED_FPS = 60;
const int UPDATE_INTERVAL = 1000/DESIRED_FPS;
#define TIMER_DELAY UPDATE_INTERVAL //  Refresh rate in milliseconds.

enum LightSource {
  LIGHT_SOURCE_EYE,
  LIGHT_SOURCE_RIGHT,
  LIGHT_SOURCE_LEFT
};

#define LIGHT_NORMAL_EYE   (Vector4f(0.0f, 0.0f, 1.0f))
#define LIGHT_NORMAL_RIGHT (Vector4f(1.0f, 0.0f, 1.0f))
#define LIGHT_NORMAL_LEFT  (Vector4f(-1.0f, 0.0f, 1.0f))

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
    , MISolidRainbow
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
    , MIShowFPS
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
    , MILightsOff
    , MILightsFlat
    , MILightsSmooth
    , MILightsDynamic
    , MILightSourceEye
    , MILightSourceRight
    , MILightSourceLeft
    , MIClipAll
    , MIClipNearOnly
    , MIClipNearFar
    , MITexture1 = 1100
    , MIMeshDefault = 1200
    , MIMesh0
    , MIMesh1
    , MIControls
    , MIAbout
  };

  bool _drawWireframeOnly;
  bool _drawWireframe;
  int _textureID;

private:
  LRESULT  handleMessage( UINT, WPARAM, LPARAM );
  void     repaint() ;
  void     generateVertexIDs(vector<Triangle>& mesh,
                             vector<Vector4f>& vertexPositions);
  void     generateMeshNormals(vector<Triangle>& mesh);
  void     createDefaultMesh();
  void     createScaledMesh(vector<Triangle>&, bool);
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
  void loadOBJ(const wchar_t* filename);
  void showOpenOBJDialog();
  void showControls();
  void showAbout();
  LRESULT onDropFiles( WPARAM, LPARAM );
  static void CALLBACK TimeProc(UINT uID, UINT uMsg, DWORD_PTR dwUser,
                                DWORD_PTR d1, DWORD_PTR d2);

  BITMAPINFOHEADER _bmih;
  BITMAPINFO _dbmi;
  BufferDC          bufferDC ;
  ImageData         screenImageData;

  bool              _backfaceCullingOn;
  bool              _zbufferOn, _fpsOn;
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
       _lightsMenu, _helpMenu;
  OBJLoader _loader;
  uint32_t _backgroundColor = 0;
  Camera _camera;
  static const int CLIP_NEAR_ONLY = 0;
  static const int CLIP_NEAR_AND_FAR = 1;
  static const int CLIP_ALL = 2;
  int _clipMode = CLIP_NEAR_ONLY;
  bool _lightsOn = true;
  TriangleDrawer _triangleDrawer;
  LightSource _lightSource = LIGHT_SOURCE_EYE;
  Vector4f _lightNormal;
  bool dynamicLights = false;
};

//------------------------------------------------------------------------------
//  GameImpl Constructors & Destructors
//------------------------------------------------------------------------------

HWND GameImpl::hwndMain;

GameImpl::GameImpl()
  : Window( TEXT("Andrew Lim's' 3D Software Rasterizer"), NULL,
           (WS_OVERLAPPEDWINDOW)&~(WS_MAXIMIZEBOX|WS_THICKFRAME),
           WS_EX_ACCEPTFILES)
  , _backfaceCullingOn(true)
  , _zbufferOn(true)
  , _fpsOn(true)
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
  , _lightsMenu(true)
  , _helpMenu(true)
  , _loader(true)
{
  _textureID = 0;
  _drawWireframeOnly = false;
  _drawWireframe = true;
  _triangleDrawer.drawMode = TriangleDrawer::DRAW_PERSPECTIVE;

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

  _lightNormal = G3D::normalize( LIGHT_NORMAL_EYE );

//  startTimer();
  resetGame();
}

GameImpl::~GameImpl()
{}

//------------------------------------------------------------------------------
//  GameImpl Member Functions
//------------------------------------------------------------------------------

// Callback function for the multimedia timer.
void CALLBACK GameImpl::TimeProc(UINT uID, UINT uMsg, DWORD_PTR dwUser,
                                 DWORD_PTR d1, DWORD_PTR d2)
{
  PostMessage( hwndMain, WM_MM_TIMER, 0, 0 );
}

// Starts the multimedia timer.
void GameImpl::startTimer()
{
  TIMECAPS tc;
  timeGetDevCaps(&tc, sizeof(TIMECAPS));
  DWORD resolution = std::min( std::max(tc.wPeriodMin, (UINT)0 ),tc.wPeriodMax);
  timeBeginPeriod( resolution );
  iTimerId = timeSetEvent( TIMER_DELAY,resolution,TimeProc,0,TIME_PERIODIC );
}

// Stops the multimedia timer.
void GameImpl::stopTimer() {
  timeKillEvent( iTimerId );
  timeEndPeriod( TIMER_DELAY );
}

void GameImpl::createMenus()
{
  _mainMenu.add("Reset to origin\tR", MIReset ) ;
  _mainMenu.add("Open OBJ...", MIOpenOBJ ) ;
  _mainMenu.addSeparator() ;
  _mainMenu.add("Exit", MIExit ) ;

  _optionsMenu.add("Nothing / Wireframe Only", MINothing);
  _optionsMenu.add("Solid", MISolid);
  _optionsMenu.add("Solid Rainbow", MISolidRainbow);
  _optionsMenu.add("Affine", MIAffine);
  _optionsMenu.add("Perspective Correct", MIPerspectiveCorrect);
  _optionsMenu.addSeparator();
  _optionsMenu.add("Wireframe", MIWireframe);
  _optionsMenu.add("Backface Culling", MIBackfacCulling);
  _optionsMenu.add("Z Buffer", MIZBuffer);
  _optionsMenu.add("Show FPS / Info", MIShowFPS);
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

  _clipMenu.add("Near Plane Only", MIClipNearOnly);
  _clipMenu.add("Near && Far Planes", MIClipNearFar);
  _clipMenu.add("All 6 Planes", MIClipAll);

  _lightsMenu.add("Off", MILightsOff);
  _lightsMenu.add("Flat", MILightsFlat);
  _lightsMenu.add("Smooth", MILightsSmooth);
  _lightsMenu.addSeparator();
  _lightsMenu.add("Light Source: Eye", MILightSourceEye);
  _lightsMenu.add("Light Source: Right", MILightSourceRight);
  _lightsMenu.add("Light Source: Left", MILightSourceLeft);
  _lightsMenu.addSeparator();
  _lightsMenu.add("Dynamic", MILightsDynamic);

  _helpMenu.add("Controls...", MIControls);
  _helpMenu.add("About...", MIAbout);

  _menuBar.add( _mainMenu, "Main" ) ;
  _menuBar.add( _optionsMenu, "Options" ) ;
  _menuBar.add( _textureMenu, "Debug" ) ;
  _menuBar.add( _meshMenu, "Scale" ) ;
  _menuBar.add( _clipMenu, "Clip");
  _menuBar.add( _lightsMenu, "Lights");
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
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], -1, 0, 0, 0, 0);
    std::string strTo( size_needed, 0 );
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], -1, &strTo[0], size_needed, 0, 0);
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

static std::wstring filepath = L"Default Cube";

void GameImpl::loadOBJ(const wchar_t* path)
{
  filepath = path;
  string shortpath = shortPathName(path);
  try {
    cout << "shortpath=" << shortpath << endl;
    vector<Triangle> mesh;
    _loader.load(shortpath, 1.0f);
    if (0==_loader.triangles.size()) {
      throw runtime_error("No triangles loaded");
    }
    stopTimer();
    _loadedMesh = _loader.triangles;
    _scale = _oldScale = 1;
    generateMeshNormals(_loadedMesh);
    createScaledMesh(_loadedMesh, false);
    resetGame();
  }
  catch (exception& ex) {
    string msg = "Error loading ";
    msg += shortpath;
    msg += " Exception=" + string(ex.what());
    MessageBox( NULL, msg.c_str(), "Error", MB_OK ) ;
  }
}

void GameImpl::showOpenOBJDialog()
{
  OpenFileDialog dialog ;
  dialog.setFilter((TCHAR*)TEXT("OBJ Files\0*.obj\0\0"));
  dialog.filter = (wchar_t*)L"OBJ Files\0*.obj\0\0";
  if ( dialog.showDialogW( this->hwnd ) ) {
    loadOBJ(dialog.fileName);
  }
}

LRESULT GameImpl::onDropFiles( WPARAM wParam, LPARAM lParam ) {
  printf("onDropFiles called\n");
  HDROP hDrop = (HDROP) wParam ;
  wchar_t filePath[ MAX_PATH ];
  if ( DragQueryFileW( hDrop, 0, filePath, MAX_PATH ) ) {
    loadOBJ( filePath );
  }
  return 0 ;
}

void GameImpl::showControls()
{
  MessageBox(hwnd, TEXT("Camera Controls:\n")
                   TEXT("WASD - move forwards, backwards and strafe sideways\n")
                   TEXT("Q/E - move down/up\n")
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
        createScaledMesh(_defaultMesh, true);
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
          _drawWireframeOnly = true;
          _drawWireframe = true;
          break;

        case MISolid:
          _drawWireframeOnly = false;
          _triangleDrawer.drawMode = TriangleDrawer::DRAW_SOLID;
          break ;

          case MISolidRainbow:
          _drawWireframeOnly = false;
          _triangleDrawer.drawMode = TriangleDrawer::DRAW_RGB_SHADED;
          break ;

        case MIPerspectiveCorrect:
          _drawWireframeOnly = false;
          _triangleDrawer.drawMode = TriangleDrawer::DRAW_PERSPECTIVE;
          break ;

        case MIAffine:
          _drawWireframeOnly = false;
          _triangleDrawer.drawMode = TriangleDrawer::DRAW_AFFINE;
          break ;

        case MIWireframe:
          _drawWireframe = !_drawWireframe;
          break;

        case MIZBuffer:
          _zbufferOn = !_zbufferOn;
          break;

        case MIShowFPS:
          _fpsOn = !_fpsOn;
          break;

        case MIBackfacCulling:
          _backfaceCullingOn = !_backfaceCullingOn;
          break;

        case MI640x360: _clientWidth=640, _clientHeight=360; resetGame(); break;
        case MI640x480: _clientWidth=640, _clientHeight=480; resetGame(); break;
        case MI800x600: _clientWidth=800, _clientHeight=600; resetGame(); break;
        case MI848x480: _clientWidth=848, _clientHeight=480; resetGame(); break;
        case MI1024x768: _clientWidth=1024, _clientHeight=768;resetGame();break;
        case MI1280x720: _clientWidth=1280, _clientHeight=720;resetGame();break;
        case MI1366x768: _clientWidth=1366, _clientHeight=768;resetGame();break;
        case MI1920x1080:_clientWidth=1920,_clientHeight=1080;resetGame();break;

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
        case MIBlueBackground:
          _backgroundColor=ImageData::makePixel(250, 145, 64); // BGR
          break;

        case MIClipAll: _clipMode = CLIP_ALL; break;
        case MIClipNearFar: _clipMode = CLIP_NEAR_AND_FAR; break;
        case MIClipNearOnly: _clipMode = CLIP_NEAR_ONLY; break;

        case MILightsOff:
          _triangleDrawer.lightsStyle = TriangleDrawer::LIGHTS_STYLE_NONE;
          break;
        case MILightsFlat:
          _triangleDrawer.lightsStyle = TriangleDrawer::LIGHTS_STYLE_FLAT;
          break;
        case MILightsSmooth:
          _triangleDrawer.lightsStyle = TriangleDrawer::LIGHTS_STYLE_SMOOTH;
          break;
        case MILightsDynamic: dynamicLights = !dynamicLights; break;

        case MILightSourceRight:
          _lightSource = LIGHT_SOURCE_RIGHT;
          _lightNormal = G3D::normalize( LIGHT_NORMAL_RIGHT );
          break;

        case MILightSourceLeft:
          _lightSource = LIGHT_SOURCE_LEFT;
          _lightNormal = G3D::normalize( LIGHT_NORMAL_LEFT );
          break;

        case MILightSourceEye:
          _lightSource = LIGHT_SOURCE_EYE;
          _lightNormal = G3D::normalize( LIGHT_NORMAL_EYE );
          break;

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
        _optionsMenu.setMenuItemChecked( MINothing, _drawWireframeOnly );
        _optionsMenu.setMenuItemChecked(
          MISolid, _triangleDrawer.drawMode == TriangleDrawer::DRAW_SOLID
        );
        _optionsMenu.setMenuItemChecked(
          MISolidRainbow,
          _triangleDrawer.drawMode == TriangleDrawer::DRAW_RGB_SHADED
        );
        _optionsMenu.setMenuItemChecked(
          MIAffine, _triangleDrawer.drawMode == TriangleDrawer::DRAW_AFFINE
        );
        _optionsMenu.setMenuItemChecked(
          MIPerspectiveCorrect,
          _triangleDrawer.drawMode == TriangleDrawer::DRAW_PERSPECTIVE
        );
        _optionsMenu.setMenuItemChecked( MIWireframe, _drawWireframe );
        _optionsMenu.setMenuItemChecked( MIZBuffer, _zbufferOn );
        _optionsMenu.setMenuItemChecked( MIBackfacCulling, _backfaceCullingOn );
        _optionsMenu.setMenuItemChecked( MIShowFPS, _fpsOn );
        _optionsMenu.setMenuItemChecked(
          MI640x360, _clientWidth==640 && _clientHeight==360
        );
        _optionsMenu.setMenuItemChecked(
          MI640x480, _clientWidth==640 && _clientHeight==480
        );
        _optionsMenu.setMenuItemChecked(
          MI800x600, _clientWidth==800 && _clientHeight==600
        );
        _optionsMenu.setMenuItemChecked(
          MI848x480, _clientWidth==848 && _clientHeight==480
        );
        _optionsMenu.setMenuItemChecked(
          MI1024x768, _clientWidth==1024 && _clientHeight==768
        );
        _optionsMenu.setMenuItemChecked(
          MI1280x720, _clientWidth==1280 && _clientHeight==720
        );
        _optionsMenu.setMenuItemChecked(
          MI1366x768, _clientWidth==1366 && _clientHeight==768
        );
        _optionsMenu.setMenuItemChecked(
          MI1920x1080, _clientWidth==1920 && _clientHeight==1080
        );
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
      else if (hMenu == _lightsMenu) {
        _lightsMenu.setMenuItemChecked(MILightsOff,
           _triangleDrawer.lightsStyle==TriangleDrawer::LIGHTS_STYLE_NONE);
        _lightsMenu.setMenuItemChecked(MILightsFlat,
           _triangleDrawer.lightsStyle==TriangleDrawer::LIGHTS_STYLE_FLAT);
        _lightsMenu.setMenuItemChecked(MILightsSmooth,
           _triangleDrawer.lightsStyle==TriangleDrawer::LIGHTS_STYLE_SMOOTH);
        _lightsMenu.setMenuItemChecked(MILightsDynamic, dynamicLights);
        _lightsMenu.setMenuItemChecked(MILightSourceLeft,
          _lightSource==LIGHT_SOURCE_LEFT);
        _lightsMenu.setMenuItemChecked(MILightSourceEye,
          _lightSource==LIGHT_SOURCE_EYE);
        _lightsMenu.setMenuItemChecked(MILightSourceRight,
          _lightSource==LIGHT_SOURCE_RIGHT);
      }
      else if (hMenu == _clipMenu) {
        _clipMenu.setMenuItemChecked( MIClipAll, _clipMode==CLIP_ALL);
        _clipMenu.setMenuItemChecked( MIClipNearOnly,_clipMode==CLIP_NEAR_ONLY);
        _clipMenu.setMenuItemChecked( MIClipNearFar,
                                      _clipMode==CLIP_NEAR_AND_FAR);
      }

      return 0 ;
    }

    case WM_PAINT:
    {
      PAINTSTRUCT ps ;
      HDC hDC = BeginPaint( hwnd, &ps ) ;
      if (_fpsOn) {
        drawWorld(bufferDC);
        drawFPS(bufferDC);
        bufferDC.draw( hDC );
      }
      else {
        drawWorld(hDC);
      }
      EndPaint( hwnd, &ps ) ;
      return 0 ;
    }

    case WM_DROPFILES: return onDropFiles( wParam, lParam );

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

// Collects all unique vertex positions in vertexPositions.
// The triangle vertices in the mesh will update their .id property to hold the
// index of the vertex position.
void GameImpl::generateVertexIDs(vector<Triangle>& mesh,
                                 vector<Vector4f>& vertexPositions)
{
  vertexPositions.clear();
  for (size_t i=0; i<mesh.size(); i++) {
    Triangle& triangle = mesh[i];
    for (size_t j=0; j<3; j++) {
      Vertex4f& vertex = triangle[j];
      Vector4f& vertexPos = vertex.pos;
      std::vector<Vector4f>::iterator it = find(vertexPositions.begin(),
                                                vertexPositions.end(),
                                                vertexPos);
      // Vertex position already inserted
      if (it != vertexPositions.end()) {
        vertex.id = (int)(it - vertexPositions.begin());
      }
      // New vertex position
      else {
        vertexPositions.push_back(vertexPos);
        vertex.id = (int)(vertexPositions.size()-1);
      }
    }
  }
}

void GameImpl::generateMeshNormals(vector<Triangle>& mesh)
{
  vector<Vector4f> vertexPositions;
  generateVertexIDs(mesh, vertexPositions);
  printf("No. of vertices = %d\n", vertexPositions.size());

  vector<Vector3f> vertexNormals;
  vertexNormals.resize(vertexPositions.size());
  printf("vertexNormals.size() = %d\n", vertexNormals.size());

  // Calculate triangle face normals
  for (size_t i=0; i<mesh.size(); ++i) {
    Triangle* t = &mesh[i];
    Vertex4f* vertices = t->vertices;
    Vector4f normal = G3D::triangleNormal(vertices[0].pos,
                                          vertices[1].pos,
                                          vertices[2].pos);
    t->normal = Vector3f(normal[0], normal[1], normal[2]);

    Vector3f& vertexNormal1 = vertexNormals[ vertices[0].id ];
    vertexNormal1 = vertexNormal1.add( t->normal );

    Vector3f& vertexNormal2 = vertexNormals[ vertices[1].id ];
    vertexNormal2 = vertexNormal2.add( t->normal );

    Vector3f& vertexNormal3 = vertexNormals[ vertices[2].id ];
    vertexNormal3 = vertexNormal3.add( t->normal );
  }

  // Normalize vertices normal
  for (size_t i=0; i<vertexNormals.size(); i++) {
    vertexNormals[i] = G3D::normalize(vertexNormals[i]);
  }

  // Populate mesh triangle vertices with normals
  for (size_t i=0; i<mesh.size(); ++i) {
    Triangle* t = &mesh[i];
    Vertex4f* vertices = t->vertices;
    for (int iv=0; iv<3; iv++) {
      int vid = vertices[iv].id;
      if (vertices[iv].id == -1) {
        printf("Vertex %d for triangle %d has no id\n", iv, i);
         throw runtime_error("vertex has no id, see console");
      }
      if (vid<0 || vid>=(int)vertexNormals.size()) {
         printf("Vertex %d for triangle %d has no id\n", iv, i);
         throw runtime_error("vid out of range");
      }
      Vector3f normal = vertexNormals[ vid ];
      vertices[iv].normal= normal;
    }
  }
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

  // RGBA
  uint32_t pink   = ImageData::makePixel(255, 192, 203, 255);
  uint32_t green  = ImageData::makePixel(0, 255, 0, 255);
  uint32_t red    = ImageData::makePixel(255, 0, 0, 255);
  uint32_t blue   = ImageData::makePixel(0, 0, 255, 255);
  uint32_t yellow = ImageData::makePixel(255, 255, 0, 255);
  uint32_t violet = ImageData::makePixel(98, 88, 124, 255);

  // BGRA
  ImageData::swapBytes(&pink, 0, 2);
  ImageData::swapBytes(&green, 0, 2);
  ImageData::swapBytes(&red, 0, 2);
  ImageData::swapBytes(&blue, 0, 2);
  ImageData::swapBytes(&yellow, 0, 2);
  ImageData::swapBytes(&violet, 0, 2);

  Triangle t;

  // South
  t = Triangle(leftX, topY, frontZ, leftX, bottomY, frontZ,
               rightX, topY, frontZ, pink);
  t.setTexUVs(0.0, 0.0, 0.0, -1.0, 1.0, -0.0);
  _defaultMesh.push_back(t);

  t = Triangle(leftX, bottomY, frontZ, rightX, bottomY, frontZ,
               rightX, topY, frontZ, pink);
  t.setTexUVs(0.0, -1.0, 1.0, -1.0, 1.0, -0.0);
  _defaultMesh.push_back(t);

  // East
  t = Triangle(rightX, topY, frontZ, rightX, bottomY, frontZ,
               rightX, topY, backZ, green);
  t.setTexUVs(0.0, 0.0, 0.0, -1.0, 1.0, 0.0);
  _defaultMesh.push_back(t);
  t = Triangle(rightX, bottomY, frontZ, rightX, bottomY, backZ,
               rightX, topY, backZ, green);
  t.setTexUVs(0.0, -1.0, 1.0, -1.0, 1.0, 0.0);
  _defaultMesh.push_back(t);

  // North
  t = Triangle(rightX, topY, backZ, rightX, bottomY, backZ,
               leftX, topY, backZ, blue);
  t.setTexUVs(0.0, 0.0, 0.0, -1.0, 1.0, 0.0);
  _defaultMesh.push_back(t);
  t = Triangle(rightX, bottomY, backZ, leftX, bottomY, backZ,
               leftX, topY, backZ, blue);
  t.setTexUVs(0.0, -1.0, 1.0, -1.0, 1.0, 0.0);
  _defaultMesh.push_back(t);

  // West
  t = Triangle(leftX, topY, backZ, leftX, bottomY, backZ,
               leftX, topY, frontZ, red);
  t.setTexUVs(0.0, 0.0, 0.0, -1.0, 1.0, 0.0);
  _defaultMesh.push_back(t);
  t = Triangle(leftX, bottomY, backZ, leftX, bottomY, frontZ,
               leftX, topY, frontZ, red);
  t.setTexUVs(0.0, -1.0, 1.0, -1.0, 1.0, 0.0);
  _defaultMesh.push_back(t);

  // Top
  t = Triangle(leftX, topY, backZ, leftX, topY, frontZ,
               rightX, topY, backZ, yellow);
  t.setTexUVs(0.0, 0.0, 0.0, -1.0, 1.0, 0.0);
  _defaultMesh.push_back(t);
  t = Triangle(leftX, topY, frontZ, rightX, topY, frontZ,
               rightX, topY, backZ, yellow);
  t.setTexUVs(0.0, -1.0, 1.0, -1.0, 1.0, 0.0);
  _defaultMesh.push_back(t);

  // Bottom
  t = Triangle(leftX, bottomY, frontZ, leftX, bottomY, backZ,
               rightX, bottomY, frontZ, violet);
  t.setTexUVs(0.0, 0.0, 0.0, -1.0, 1.0, 0.0);
  _defaultMesh.push_back(t);
  t = Triangle(leftX, bottomY, backZ, rightX, bottomY, backZ,
               rightX, bottomY, frontZ, violet);
  t.setTexUVs(0.0, -1.0, 1.0, -1.0, 1.0, 0.0);
  _defaultMesh.push_back(t);

  generateMeshNormals(_defaultMesh);

  createScaledMesh(_defaultMesh, true);
}

void GameImpl::createScaledMesh(vector<Triangle>& sourceMesh,
                                bool isDefaultMesh)
{
  _scaledMesh.clear();
  for (size_t i=0; i<sourceMesh.size(); ++i) {
    Triangle t = sourceMesh[i];
    if (!isDefaultMesh) {
      t.color= SOLID_COLOR;
    }
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

  static float lightRotation = 0;
  Vector4f newLightNormal(_lightNormal);
  if (dynamicLights) {
    newLightNormal = G3D::normalize(LIGHT_NORMAL_EYE);
    lightRotation += LIGHT_SPEED_DEG;
    // Fast forward the rotation
    if (lightRotation>180 && lightRotation<270) {
      lightRotation = 270;
    }
    if (lightRotation >= 360) {
      lightRotation = 0;
    }
    Matrix4f rotLight = Matrix4f::makeYRotation(G3D::deg2rad(lightRotation));
    newLightNormal = G3D::normalize(rotLight*newLightNormal);
  }
  else {
    lightRotation = 0;
  }
  _triangleDrawer.lightDirection = newLightNormal;

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
  const float MAX_LOOK_UP = G3D::deg2rad(55);
  const float MAX_LOOK_DOWN = G3D::deg2rad(-55);
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
  if (!_fpsOn) {
    return;
  }
  string info;
  RECT rc = {0} ;
  GetClientRect( hwnd, &rc ) ;
  info += "FPS: " + to_string(_currentFPS) + "\n";
  info += "Triangles: " + to_string(_scaledMesh.size());
  DrawText(hdc, info.c_str(), -1, &rc, DT_LEFT|DT_TOP);
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
    if (_loadedMesh.empty()) {
      createScaledMesh(_defaultMesh, true);
    }
    else {
      createScaledMesh(_loadedMesh, false);
    }
  }
  _mesh = _scaledMesh;

  // Rotate mesh locally first
  for (size_t i=0; i<_mesh.size(); i++) {
    Triangle& t = _mesh[i];
    t.rotateX(G3D::deg2rad(_xrot));
    t.rotateY(G3D::deg2rad(_yrot));
  }

  const float fovydeg = 90.0f;
  const float fovyrad = G3D::deg2rad(fovydeg);
  const float aspectRatio = _clientWidth/(float)_clientHeight;
  const float znear = 1.f;
  const float zfar = 1000.f;

  // Convert player world coordinates to 3D right-handed eye coordinates
  // In player world coordinates, the player is looking down the x-axis and
  // the z-axis is up
  const float eyeX = _camera.y;
  const float eyeY = _camera.z;
  const float eyeZ = -_camera.x;

  // Movement vector. Shift the world by the opposite of player's position
  const Matrix4f translation = Matrix4f::makeTranslation(-eyeX, -eyeY, -eyeZ);

  const Matrix4f perspective = Matrix4f::makePerspective(fovyrad, aspectRatio,
                                                         znear, zfar);
  const Matrix4f rotY = Matrix4f::makeYRotation(_camera.rot);
  const Matrix4f rotX = Matrix4f::makeXRotation(_camera.vrot);
  const Matrix4f rotXY = rotX * rotY;
  const Matrix4f worldTransform = rotXY * translation;
  const Matrix4f proj = perspective * worldTransform;

  for (size_t i=0; i<_mesh.size(); ++i) {
    Triangle* t = &_mesh[i];

    Vertex4f* vertices = t->vertices;

    // Clip Coordinates
    Vector4f clip1 = proj * vertices[0].pos;
    Vector4f clip2 = proj * vertices[1].pos;
    Vector4f clip3 = proj * vertices[2].pos;

    // Create a triangle with clip coordinates
    Vertex4f clipVertex1 = Vertex4f(clip1, t->uv(0));
    Vertex4f clipVertex2 = Vertex4f(clip2, t->uv(1));
    Vertex4f clipVertex3 = Vertex4f(clip3, t->uv(2));
    Triangle triangleToClip(clipVertex1, clipVertex2, clipVertex3);

    // Copy attributes from mesh triangle
    triangleToClip.color = t->color;
    if (_triangleDrawer.drawMode == TriangleDrawer::DRAW_RGB_SHADED) {
      triangleToClip.vertices[0].rgb = Vector3f(255, 0, 0);
      triangleToClip.vertices[1].rgb = Vector3f(0, 255, 0);
      triangleToClip.vertices[2].rgb = Vector3f(0, 0, 255);
    }

    // The face and vertex normals need to be rotated
    triangleToClip.vertices[0].normal = rotXY * vertices[0].normal;
    triangleToClip.vertices[1].normal = rotXY * vertices[1].normal;
    triangleToClip.vertices[2].normal = rotXY * vertices[2].normal;
    triangleToClip.normal = rotXY * t->normal;

    vector<Triangle> trianglesToClip;
    trianglesToClip.push_back(triangleToClip);

    // Clip the triangle to the frustrum volume
    vector<Triangle> clippedTriangles;
    switch (_clipMode)
    {
      case CLIP_ALL:
        clippedTriangles = ClipSpace::clipByAllPlanes(trianglesToClip);
        break;
      case CLIP_NEAR_AND_FAR:
        clippedTriangles = ClipSpace::clipByNearFarPlanes(trianglesToClip);
        break;
      default:
        clippedTriangles = ClipSpace::clipByNearPlane(trianglesToClip);
        break;
    }

    for (size_t i=0; i<clippedTriangles.size(); ++i) {
      Triangle triangle = clippedTriangles[i];
      triangle.color = triangleToClip.color;

      // Go into normalized device coordinates (NDC)
      triangle.perspectiveDivide();

      float xprod = 0;
      if (_backfaceCullingOn) {
        xprod = Graphics3D::crossProduct2D<Vector4f>(
          triangle.vertex(0).pos,
          triangle.vertex(1).pos,
          triangle.vertex(2).pos
        );
        if (xprod<0) {
          continue;
        }
      }

      G3D::ndcToWindow(triangle[0].pos, _clientWidth, _clientHeight);
      G3D::ndcToWindow(triangle[1].pos, _clientWidth, _clientHeight);
      G3D::ndcToWindow(triangle[2].pos, _clientWidth, _clientHeight);

      _triangleDrawer.depthBuffer = _zbufferOn ? &zbuffer : 0;

      if (_drawWireframeOnly) {

      }

      else if (_triangleDrawer.drawMode == TriangleDrawer::DRAW_SOLID ||
               _triangleDrawer.drawMode == TriangleDrawer::DRAW_RGB_SHADED ) {
        _triangleDrawer.triangle(screenImageData, triangle);
      }

      else {
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
          _triangleDrawer.textureImageData = textureData;
          _triangleDrawer.triangle(screenImageData, triangle);
        }
      }

      if (_drawWireframe) {
        _triangleDrawer.wireframeTriangle(screenImageData, triangle,
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
