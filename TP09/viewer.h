#ifndef VIEWER_H
#define VIEWER_H

// GLEW lib: needs to be included first!!
#include <GL/glew.h> 

// OpenGL library 
#include <GL/gl.h>

// OpenGL Utility library
#include <GL/glu.h>

// OpenGL Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <QGLFormat>
#include <QGLWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>
#include <stack>

#include "camera.h"
#include "shader.h"
#include "grid.h"

class Viewer : public QGLWidget {
 public:
  Viewer(char *filename,const QGLFormat &format=QGLFormat::defaultFormat());
  ~Viewer();
  
 protected :
  virtual void paintGL();
  virtual void initializeGL();
  virtual void resizeGL(int width,int height);
  virtual void keyPressEvent(QKeyEvent *ke);
  virtual void mousePressEvent(QMouseEvent *me);
  virtual void mouseMoveEvent(QMouseEvent *me);

 private:
  // OpenGL objects creation
  void createVAO();
  void deleteVAO();
  void createFBO();
  void updateFBO();
  void deleteFBO();
  void createShaders();
  void updateTex(GLuint tex,GLenum filter,GLenum wrap,unsigned int w,
         unsigned int h,GLint iformat,GLenum format,bool isShadowmap=false);
  void createTextures();

  // drawing functions 
  void createHeightMap(GLuint id);
  void drawSceneFromCamera(GLuint id);
  void drawSceneFromLight(GLuint id);
  void renderFinalImage(GLuint id);
  void testShowTerrain(GLuint id);

  QTimer        *_timer;    // timer that controls the animation
  unsigned int   _currentshader; // current shader index

  Grid   *_grid;   // the grid
  Camera *_cam;    // the camera

  glm::vec3 _light;  // light direction
  glm::vec3 _motion; // motion offset for the noise texture 
  bool      _mode;   // camera motion or light motion
  bool      _showShadowMap;

  std::vector<std::string> _vertexFilenames;   // all vertex filenames
  std::vector<std::string> _fragmentFilenames; // all fragment filenames
  std::vector<Shader *>    _shaders;           // all the shaders 

  // vbo/vao ids 
  GLuint _vaoTerrain;
  GLuint _vaoQuad;
  GLuint _terrain[2];
  GLuint _quad;
  
  // fbo ids 
  GLuint _fboTerrain;

  // texture ids (1st fbo)
  GLuint _texHeight;
  GLuint _texShadow;
  GLuint _fboPreProcess;
  GLuint _texNormal;
  GLuint _montagneTexId;

  // texture ids (2nd fbo)
  GLuint _texBuff1;
  GLuint _texBuff2;
  GLuint _texBuff3;

  // fbo id and associated depth texture
  GLuint _fbo;
  GLuint _texDepth; // this will be our shadowmap!
  unsigned int _depthResol; // this is the resolution of the shadowmap (squared texture)

  unsigned int _ndResol;
};

#endif // VIEWER_H
