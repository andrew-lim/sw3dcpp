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
using namespace std ;
using namespace al::graphics;
using namespace glm;

#define CLIENT_WIDTH 640
#define CLIENT_HEIGHT 360
#define MOVE_SPEED 4
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
  windowWidth = windowWidth-1;
  windowHeight = windowHeight-1;

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
    , MISolid
    , MIAffine
    , MIPerspectiveCorrect
    , MIWireframe
    , MI640x360
    , MI640x480
    , MI800x600
    , MI1024x768
    , MI1366x768
    , MI1920x1080
    , MITexture1
  };
  
  enum DrawType {
    DrawPerspectiveCorrect = 0,
    DrawAffine = 1,
    DrawSolid = 2
  };
  
  int _drawType = DrawPerspectiveCorrect;
  bool _drawWireframe;
  int _textureID;

private:
  LRESULT  handleMessage( UINT, WPARAM, LPARAM );  
  void     repaint() ;
  void     createMesh();
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
  static void CALLBACK TimeProc(UINT uID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR d1, DWORD_PTR d2);
  
  BITMAPINFOHEADER _bmih;
  BITMAPINFO _dbmi;
  BufferDC          bufferDC ;
  ImageData         screenImageData;
  bool              _backfaceCullingOn;
  vector<ImageData> _textureImageDatas;
  
  int _clientWidth = CLIENT_WIDTH;
  int _clientHeight = CLIENT_HEIGHT;

  int           iTimerId;            //  Multimedia timer id.
  float _xrot, _yrot;
  DWORD  _currentFPS, _fps, _pastFps, _past;
  static HWND hwndMain;
  std::map<int, BOOL> _keys;
  bool _backCullingOn;
  float _worldX = 0;
  float _worldY = 0;
  float _worldZ = -200;
  float _xStep = 4;
  float _yStep = 4;
  float _zStep = 4;
  vector<Triangle*> mesh;
  Menu _menuBar, _mainMenu, _optionsMenu, _textureMenu;
};

//------------------------------------------------------------------------------
//  GameImpl Constructors & Destructors
//------------------------------------------------------------------------------

HWND GameImpl::hwndMain;

GameImpl::GameImpl()
  : Window( TEXT("Andrew's 3D Software Engine"), NULL,
           WS_OVERLAPPEDWINDOW&~(WS_MAXIMIZEBOX|WS_THICKFRAME), 0 )
  , _backfaceCullingOn(true)
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
{
  _textureID = 0;
  _drawWireframe = true;
  
  srand( (unsigned int)time(0) );
  printf("DESIRED_FPS=%d\n", DESIRED_FPS);
  printf("TIMER_DELAY=%d\n", TIMER_DELAY);
  
  _pastFps = _past = GetTickCount();
  printf("_pastFps=%lu\n, _past=%lu\n", _pastFps, _past);
  
  GameImpl::hwndMain = hwnd;
  
  Tests::run();
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
  _mainMenu.add("Reset", MIReset ) ;
  _mainMenu.addSeparator() ;
  _mainMenu.add("Exit", MIExit ) ;
  
  _optionsMenu.add("Solid", MISolid);
  _optionsMenu.add("Affine", MIAffine);
  _optionsMenu.add("Perspective Correct", MIPerspectiveCorrect);
  _optionsMenu.addSeparator();
  _optionsMenu.add("Wireframe", MIWireframe);
  _optionsMenu.addSeparator();
  _optionsMenu.add("640x360", MI640x360);
  _optionsMenu.add("640x480", MI640x480);
  _optionsMenu.add("800x600", MI800x600);
  _optionsMenu.add("1024x768", MI1024x768);
  _optionsMenu.add("1366x768", MI1366x768);
  _optionsMenu.add("1920x1080", MI1920x1080);
  
  for (size_t i=0; i<_textureImageDatas.size(); ++i) {
    TCHAR s[128];
    sprintf(s, "Texture %d", (int)(i+1));
    _textureMenu.add(s, MITexture1 + i);
  }

  _menuBar.add( _mainMenu, "Main" ) ;
  _menuBar.add( _optionsMenu, "Options" ) ;
  _menuBar.add( _textureMenu, "Texture" ) ;
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
  paths.push_back("..\\res\\texture1.bmp");
  paths.push_back("..\\res\\texture2.bmp");
  paths.push_back("..\\res\\texture3.bmp");
  paths.push_back("..\\res\\texture4.bmp");
  paths.push_back("..\\res\\texture5.bmp");
  paths.push_back("..\\res\\texture6.bmp");
  paths.push_back("..\\res\\texture7.bmp");
  for (size_t i=0 ;i<paths.size(); ++i) {
    string path = paths[i];
    Bitmap bmp;
    printf("Loading image %s\n", path.c_str());
    if (!bmp.loadFile(path.c_str())) {
      string error = "Error loading " + path;
      throw std::runtime_error(error.c_str());
    }
      
    uint8_t* bmpbytes = new uint8_t[bmp.getWidth() * bmp.getHeight() * 4];
    uint32_t result = bmp.getBits(bmpbytes);
    if (result == 0) {
      string error = "0 scanlines loaded from " + path;
      throw std::runtime_error(error.c_str());
    }
    _textureImageDatas.push_back(
      ImageData(bmpbytes, bmp.getWidth(), bmp.getHeight(), true)
    );
    delete[] bmpbytes;
  }
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
        _textureID = wID - MITexture1;
      }
      
      switch ( wID )
      {
        case MIReset: {
          resetGame();
          break;
        }
        
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

        case MI640x360: _clientWidth=640, _clientHeight=360; resetGame(); break;
        case MI640x480: _clientWidth=640, _clientHeight=480; resetGame(); break;
        case MI800x600: _clientWidth=800, _clientHeight=600; resetGame(); break;
        case MI1024x768: _clientWidth=1024, _clientHeight=768; resetGame(); break;
        case MI1366x768: _clientWidth=1366, _clientHeight=768; resetGame(); break;
        case MI1920x1080: _clientWidth=1920, _clientHeight=1080; resetGame(); break;
        
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
        _optionsMenu.setMenuItemChecked( MISolid, _drawType == DrawSolid );
        _optionsMenu.setMenuItemChecked( MIAffine, _drawType == DrawAffine );
        _optionsMenu.setMenuItemChecked( MIPerspectiveCorrect, _drawType == DrawPerspectiveCorrect );
        _optionsMenu.setMenuItemChecked( MIWireframe, _drawWireframe );
        _optionsMenu.setMenuItemChecked( MI640x360, _clientWidth==640 && _clientHeight==360 );
        _optionsMenu.setMenuItemChecked( MI640x480, _clientWidth==640 && _clientHeight==480);
        _optionsMenu.setMenuItemChecked( MI800x600, _clientWidth==800 && _clientHeight==600);
        _optionsMenu.setMenuItemChecked( MI1024x768, _clientWidth==1024 && _clientHeight==768);
      }
      else if (hMenu == _textureMenu) {
//        _textureMenu.setMenuItemChecked( MITexture1+_textureID, true);
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

void GameImpl::createMesh()
{
  for (size_t i=0; i<mesh.size(); ++i) {
    delete mesh[i];
  }
  mesh.clear();
  
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
  
  Triangle* t;
  
  // South
  t = new Triangle(leftX, topY, frontZ, leftX, bottomY, frontZ, rightX, topY, frontZ, pink);
  t->setTexUVs(0.0, 0.0, 0.0, 1.0, 1.0, 0.0);
  mesh.push_back(t);

  t = new Triangle(leftX, bottomY, frontZ, rightX, bottomY, frontZ, rightX, topY, frontZ, pink);
  t->setTexUVs(0.0, 1.0, 1.0, 1.0, 1.0, 0.0);
   mesh.push_back(t);
  
  // East
  t = new Triangle(rightX, topY, frontZ, rightX, bottomY, frontZ, rightX, topY, backZ, green);
  t->setTexUVs(0.0, 0.0, 0.0, 1.0, 1.0, 0.0);
    mesh.push_back(t);
  t = new Triangle(rightX, bottomY, frontZ, rightX, bottomY, backZ, rightX, topY, backZ, green);
  t->setTexUVs(0.0, 1.0, 1.0, 1.0, 1.0, 0.0);
    mesh.push_back(t);
  
  // North
  t = new Triangle(rightX, topY, backZ, rightX, bottomY, backZ, leftX, topY, backZ, blue);
  t->setTexUVs(0.0, 0.0, 0.0, 1.0, 1.0, 0.0);
    mesh.push_back(t);
  t = new Triangle(rightX, bottomY, backZ, leftX, bottomY, backZ, leftX, topY, backZ, blue);
  t->setTexUVs(0.0, 1.0, 1.0, 1.0, 1.0, 0.0);
    mesh.push_back(t);

  // West
  t = new Triangle(leftX, topY, backZ, leftX, bottomY, backZ, leftX, topY, frontZ, red);
  t->setTexUVs(0.0, 0.0, 0.0, 1.0, 1.0, 0.0);
    mesh.push_back(t);
  t = new Triangle(leftX, bottomY, backZ, leftX, bottomY, frontZ, leftX, topY, frontZ, red);
  t->setTexUVs(0.0, 1.0, 1.0, 1.0, 1.0, 0.0);
    mesh.push_back(t);

  // Top
  t = new Triangle(leftX, topY, backZ, leftX, topY, frontZ, rightX, topY, backZ, yellow);
  t->setTexUVs(0.0, 0.0, 0.0, 1.0, 1.0, 0.0);
    mesh.push_back(t);
  t = new Triangle(leftX, topY, frontZ, rightX, topY, frontZ, rightX, topY, backZ, yellow);
  t->setTexUVs(0.0, 1.0, 1.0, 1.0, 1.0, 0.0);
    mesh.push_back(t);

  // Bottom
  t = new Triangle(leftX, bottomY, frontZ, leftX, bottomY, backZ, rightX, bottomY, frontZ, violet);
  t->setTexUVs(0.0, 0.0, 0.0, 1.0, 1.0, 0.0);
    mesh.push_back(t);
  t = new Triangle(leftX, bottomY, backZ, rightX, bottomY, backZ, rightX, bottomY, frontZ, violet);
  t->setTexUVs(0.0, 1.0, 1.0, 1.0, 1.0, 0.0);
    mesh.push_back(t);

  for (size_t i=0; i<mesh.size(); i++) {
    Triangle* t = mesh[i];
    t->rotateX(glm::radians(_xrot));
    t->rotateY(glm::radians(_yrot));
  }
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
  _xrot = 0;
  _yrot = 0;
  _worldX = 0;
  _worldY = 0;
  _worldZ = -200;
  bufferDC.create(_clientWidth, _clientHeight);
  prepareScreenImageData();
  startTimer();
}

void GameImpl::updateGame()
{
  if (_keys[VK_UP]) {
    _xrot += -1;
  }
  if (_keys[VK_DOWN]) {
    _xrot += 1;
  }
  if (_keys[VK_LEFT]) {
    _yrot += -1;
  }
  if (_keys[VK_RIGHT]) {
    _yrot += 1;
  }
  
  const int oldWorldZ = _worldZ;
  if (_keys['W']) {
    _worldZ += _zStep;
  }
  else if (_keys['S']) {
    _worldZ -= _zStep;
  }
  if (_worldZ>-100) {
    _worldZ = oldWorldZ;
  }
  
  if (_keys['A']) {
    _worldX += _xStep;
  }
  if (_keys['D']) {
    _worldX -= _xStep;
  }
  if (_keys['E']) {
    _worldY += _yStep;
  }
  if (_keys['Q']) {
    _worldY -= _yStep;
  }
}

void GameImpl::drawFPS(HDC hdc)
{
  TCHAR fpsText[64];
  sprintf(fpsText, "FPS: %lu", _currentFPS);
  
  RECT rc = {0} ;
  GetClientRect( hwnd, &rc ) ;
  
  DrawText(hdc, fpsText, -1, &rc, DT_LEFT|DT_TOP);
}

void GameImpl::drawWorld(HDC hdc)
{
  _fps++;
  screenImageData.clear();
  
  createMesh();
  
  for (size_t i=0; i<mesh.size(); ++i) {
    Triangle* t = mesh[i];
    
    float fovydeg = 90.0f;
    float fovyrad = glm::radians(fovydeg);
    float znear = 1.f;
    float zfar = 1000.f;
    
    glm::mat4 translate = glm::translate(glm::mat4(1), glm::vec3(_worldX, _worldY, _worldZ));
    glm::mat4 proj = glm::perspective(fovyrad, _clientWidth/(float)_clientHeight, znear, zfar);
    glm::mat4 viewProj = proj * translate;

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
    vector<Triangle> clippedTriangles = ClipSpace::clipTrianglesByAllPlanes(trianglesToClip);

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
        Graphics2D::affineTriangle(screenImageData,
                                   win1[0], win1[1], triangle.getTexU(0), triangle.getTexV(0),
                                   win2[0], win2[1], triangle.getTexU(1), triangle.getTexV(1),
                                   win3[0], win3[1], triangle.getTexU(2), triangle.getTexV(2),
                                   _textureImageDatas[_textureID]);
      }

      else if (_drawType==DrawPerspectiveCorrect) {
        Graphics2D::texturedTriangle(screenImageData,
                                     win1[0], win1[1], triangle.getW(0), triangle.getTexU(0), triangle.getTexV(0),
                                     win2[0], win2[1], triangle.getW(1), triangle.getTexU(1), triangle.getTexV(1),
                                     win3[0], win3[1], triangle.getW(2), triangle.getTexU(2), triangle.getTexV(2),
                                     _textureImageDatas[_textureID]);
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
