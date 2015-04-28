#include "viewer.h"

#include <math.h>
#include <iostream>
#include <QTime>

using namespace std;

Viewer::Viewer(char *,const QGLFormat &format)
  : QGLWidget(format),
    _timer(new QTimer(this)),
    _currentshader(0),
    _light(glm::vec3(0,0,1)),
    _motion(glm::vec3(0,0,0)),
    _mode(false),
    _showShadowMap(false),
    _ndResol(512),
    _depthResol(512) {

  setlocale(LC_ALL,"C");

  _grid = new Grid(_ndResol,-1.0f,1.0f);
  _cam  = new Camera(1.0f,glm::vec3(0.0f,0.0f,0.0f));

  _timer->setInterval(10);
  connect(_timer,SIGNAL(timeout()),this,SLOT(updateGL()));
}

Viewer::~Viewer() {
  delete _timer;
  delete _grid;
  delete _cam;

  for(unsigned int i=0;i<_shaders.size();++i) {
    delete _shaders[i];
  }

  // delete all GPU objects  
  deleteVAO(); 
  deleteFBO();
}

void Viewer::createFBO() {
  // FBOs
  glGenFramebuffers(1,&_fboTerrain);
  glGenTextures(1,&_texDepth);

  // Textures 
  glGenTextures(1,&_texHeight);
  glGenTextures(1,&_montagneTexId);

  // create the texture for rendering depth values
  glBindTexture(GL_TEXTURE_2D,_texDepth);
  glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT24,_depthResol,_depthResol,0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // attach textures to framebuffer object
  glBindFramebuffer(GL_FRAMEBUFFER,_fbo);
  glBindTexture(GL_TEXTURE_2D,_texDepth);
  glFramebufferTexture2D(GL_FRAMEBUFFER_EXT,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,_texDepth,0);
}

void Viewer::deleteFBO() {
  // FBOs
  glDeleteFramebuffers(1,&_fboTerrain);
  glDeleteTextures(1,&_texDepth);

  // Textures 
  glDeleteTextures(1,&_texHeight);
  glDeleteTextures(1,&_montagneTexId);
  
}

void Viewer::createVAO() {
  const GLfloat quadData[] = {-1.0f,-1.0f,0.0f, 1.0f,-1.0f,0.0f, -1.0f,1.0f,0.0f, 
			      -1.0f,1.0f,0.0f, 1.0f,-1.0f,0.0f, 1.0f,1.0f,0.0f }; 

  // create the VAO associated with the grid (the terrain)
  glGenBuffers(2,_terrain);
  glGenVertexArrays(1,&_vaoTerrain);
  glBindVertexArray(_vaoTerrain);
  glBindBuffer(GL_ARRAY_BUFFER,_terrain[0]); // vertices 
  glBufferData(GL_ARRAY_BUFFER,_grid->nbVertices()*3*sizeof(float),_grid->vertices(),GL_STATIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,_terrain[1]); // indices 
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,_grid->nbFaces()*3*sizeof(int),_grid->faces(),GL_STATIC_DRAW); 

  // create the VAO associated with the quad 
  glGenBuffers(1,&_quad);
  glGenVertexArrays(1,&_vaoQuad);
  glBindVertexArray(_vaoQuad);
  glBindBuffer(GL_ARRAY_BUFFER,_quad); // vertices 
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadData),quadData,GL_STATIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);
  glEnableVertexAttribArray(0);
}

void Viewer::deleteVAO() {
  glDeleteBuffers(2,_terrain);
  glDeleteBuffers(1,&_quad);
  glDeleteVertexArrays(1,&_vaoTerrain);
  glDeleteVertexArrays(1,&_vaoQuad);
}

void Viewer::createTextures(){
    QImage image, image2,image3,image4;

    glEnable(GL_TEXTURE_2D);

    glGenTextures(1,&_rocheTexId);
    glGenTextures(1,&_eauTexId);
    glGenTextures(1,&_neigeTexId);
    glGenTextures(1,&_foretTexId);

    glBindTexture(GL_TEXTURE_2D,_rocheTexId);
    image = QGLWidget::convertToGLFormat(QImage("../TP09/textures/full.jpg"));
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,image.width(),image.height(),0,
             GL_RGBA,GL_UNSIGNED_BYTE,(const GLvoid *)image.bits());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D,_eauTexId);
    image2 = QGLWidget::convertToGLFormat(QImage("../TP09/textures/eau.jpg"));
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,image2.width(),image2.height(),0,
             GL_RGBA,GL_UNSIGNED_BYTE,(const GLvoid *)image2.bits());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D,_neigeTexId);
    image3 = QGLWidget::convertToGLFormat(QImage("../TP09/textures/neige.jpg"));
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,image3.width(),image3.height(),0,
             GL_RGBA,GL_UNSIGNED_BYTE,(const GLvoid *)image3.bits());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D,_foretTexId);
    image4 = QGLWidget::convertToGLFormat(QImage("../TP09/textures/foret.jpg"));
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,image4.width(),image4.height(),0,
             GL_RGBA,GL_UNSIGNED_BYTE,(const GLvoid *)image4.bits());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
}

void Viewer::updateTex(GLuint tex,GLenum filter,GLenum wrap,unsigned int w,
		       unsigned int h,GLint iformat, GLenum format,bool isShadowmap) {

  glBindTexture(GL_TEXTURE_2D,tex);
  glTexImage2D(GL_TEXTURE_2D, 0,iformat,w,h,0,format,GL_FLOAT,NULL);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

  if(isShadowmap) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
  }
}

void Viewer::updateFBO() {

  // create the noise texture 
  updateTex(_texHeight,GL_LINEAR,GL_CLAMP_TO_EDGE,_ndResol,_ndResol,GL_RGBA32F,GL_RGBA);

  // attach textures to the FBO dedicated to creating the terrain 
  glBindFramebuffer(GL_FRAMEBUFFER,_fboTerrain);
  glBindTexture(GL_TEXTURE_2D,_texHeight);
  glFramebufferTexture2D(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,_texHeight,0);
 
  // test if everything is ok
  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    cout << "Warning: FBO[0] not complete!" << endl;

  // disable FBO
  glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void Viewer::createShaders() {
  // *** height field *** 
  _vertexFilenames.push_back("../TP09/shaders/noise.vert");
  _fragmentFilenames.push_back("../TP09/shaders/noise.frag");
  // ******************************
  _vertexFilenames.push_back("../TP09/shaders/show-terrain.vert");
  _fragmentFilenames.push_back("../TP09/shaders/show-terrain.frag");
  // *********************************
  // displacement shader = 2
  _vertexFilenames.push_back("../TP09/shaders/displacement.vert");
  _fragmentFilenames.push_back("../TP09/shaders/displacement.frag");
  // ******************************
  // light shader = 3
  _vertexFilenames.push_back("../TP09/shaders/light.vert");
  _fragmentFilenames.push_back("../TP09/shaders/light.frag");
  // ******************************
  //appearance shader = 4
  _vertexFilenames.push_back("../TP09/shaders/apparence.vert");
  _fragmentFilenames.push_back("../TP09/shaders/apparence.frag");
  // ******************************
  //full shader = 5
  _vertexFilenames.push_back("../TP09/shaders/full.vert");
  _fragmentFilenames.push_back("../TP09/shaders/full.frag");
  // ******************************
}

void Viewer::createHeightMap(GLuint id) {
  // terrain creation

  // send the motion vector 
  glUniform3fv(glGetUniformLocation(id,"motion"),1,&(_motion[0]));

  // draw the quad 
  glBindVertexArray(_vaoQuad);
  glDrawArrays(GL_TRIANGLES,0,6);
  glBindVertexArray(0);
}

void Viewer::drawSceneFromCamera(GLuint id) {
    glUniformMatrix4fv(glGetUniformLocation(id,"mdvMat"),1,GL_FALSE,&(_cam->mdvMatrix()[0][0]));
    glUniformMatrix4fv(glGetUniformLocation(id,"projMat"),1,GL_FALSE,&(_cam->projMatrix()[0][0]));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,_texHeight);
    glUniform1i(glGetUniformLocation(id,"terrain"),0);

    glBindVertexArray(_vaoTerrain);
    glDrawElements(GL_TRIANGLES,3*_grid->nbFaces(),GL_UNSIGNED_INT,(void *)0);
    glBindVertexArray(0);
}

void Viewer::drawSceneFromLight(GLuint id) {
}

void Viewer::renderFinalImage(GLuint id) {
    glUniformMatrix4fv(glGetUniformLocation(id,"mdvMat"),1,GL_FALSE,&(_cam->mdvMatrix()[0][0]));
    glUniformMatrix4fv(glGetUniformLocation(id,"projMat"),1,GL_FALSE,&(_cam->projMatrix()[0][0]));
    glUniformMatrix3fv(glGetUniformLocation(id,"normalMat"),1,GL_FALSE,&(_cam->normalMatrix()[0][0]));
    glUniform3f(glGetUniformLocation(id,"light"), _light[0], _light[1], _light[2]);

    // matrice modele-vue-projection du point de vue la lumiere
    const float size = sqrt(2);
    glm::vec3 l   = glm::transpose(_cam->normalMatrix())*_light;
    glm::mat4 p   = glm::ortho<float>(-size,size,-size,size,-size,2*size);
    glm::mat4 v   = glm::lookAt(l, glm::vec3(0,0,0), glm::vec3(0,1,0));
    glm::mat4 m   = glm::mat4(1.0);
    const glm::mat4 mvpMat = p*v*m;
    glUniformMatrix4fv(glGetUniformLocation(id,"mvpMat"),1,GL_FALSE,&mvpMat[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,_texHeight);
    glUniform1i(glGetUniformLocation(id,"terrain"),0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,_eauTexId);
    glUniform1i(glGetUniformLocation(id,"eauTex"),1);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D,_foretTexId);
    glUniform1i(glGetUniformLocation(id,"foretTex"),5);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D,_rocheTexId);
    glUniform1i(glGetUniformLocation(id,"rocheTex"),3);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D,_neigeTexId);
    glUniform1i(glGetUniformLocation(id,"neigeTex"),4);

    // send the shadow map here ***
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D,_texDepth);
    glUniform1i(glGetUniformLocation(id,"shadowmap"),2);

    // draw VAO
    glBindVertexArray(_vaoTerrain);
    glDrawElements(GL_TRIANGLES,3*_grid->nbFaces(),GL_UNSIGNED_INT,(void *)0);
    glBindVertexArray(0);
}

void Viewer::testShowTerrain(GLuint id) {
}

void Viewer::paintGL() {
  // *** generate the height field *** 
  glBindFramebuffer(GL_FRAMEBUFFER,_fboTerrain);

  // draw into the first buffer 
  glDrawBuffer(GL_COLOR_ATTACHMENT0);

  // viewport at the size of the heightmap 
  glViewport(0,0,_ndResol,_ndResol);

  // disable depth test 
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);

  // clear color buffer 
  glClear(GL_COLOR_BUFFER_BIT);

  // activate the noise shader 
  glUseProgram(_shaders[0]->id());

  // generate the noise texture 
  createHeightMap(_shaders[0]->id());

  // restore previous state
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);

  // *** show the generated height field (to be replaced) *** 

 // render in the viewport now
  glBindFramebuffer(GL_FRAMEBUFFER,0);

  // restore viewport sizes 
  glViewport(0,0,width(),height());

  // clear buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // activate the buffer shader 
  glUseProgram( _shaders[5]->id() );

  //drawSceneFromLight( _shaders[3]->id() );
  renderFinalImage( _shaders[5]->id() );

  // disable shader 
  glUseProgram(0);
}

void Viewer::resizeGL(int width,int height) {
  _cam->initialize(width,height,false);
  glViewport(0,0,width,height);
  updateFBO();
  updateGL();
}

void Viewer::mousePressEvent(QMouseEvent *me) {
  const glm::vec2 p((float)me->x(),(float)(height()-me->y()));

  if(me->button()==Qt::LeftButton) {
    _cam->initRotation(p);
    _mode = false;
  } else if(me->button()==Qt::MidButton) {
    _cam->initMoveZ(p);
    _mode = false;
  } else if(me->button()==Qt::RightButton) {
    _light[0] = (p[0]-(float)(width()/2))/((float)(width()/2));
    _light[1] = (p[1]-(float)(height()/2))/((float)(height()/2));
    _light[2] = 1.0f-std::max(fabs(_light[0]),fabs(_light[1]));
    _light = glm::normalize(_light);
    _mode = true;
  } 

  updateGL();
}

void Viewer::mouseMoveEvent(QMouseEvent *me) {
  const glm::vec2 p((float)me->x(),(float)(height()-me->y()));
 
  if(_mode) {
    // light mode
    _light[0] = (p[0]-(float)(width()/2))/((float)(width()/2));
    _light[1] = (p[1]-(float)(height()/2))/((float)(height()/2));
    _light[2] = 1.0f-std::max(fabs(_light[0]),fabs(_light[1]));
    _light = glm::normalize(_light);
  } else {
    // camera mode
    _cam->move(p);
  }

  updateGL();
}

void Viewer::keyPressEvent(QKeyEvent *ke) {
  const float step = 0.05;
  if(ke->key()==Qt::Key_Z) {
    glm::vec2 v = glm::vec2(glm::transpose(_cam->normalMatrix())*glm::vec3(0,0,-1))*step;
    if(v[0]!=0.0 && v[1]!=0.0) v = glm::normalize(v)*step;
    else v = glm::vec2(0,1)*step;
    _motion[0] += v[0];
    _motion[1] += v[1];
  }

  if(ke->key()==Qt::Key_S) {
    glm::vec2 v = glm::vec2(glm::transpose(_cam->normalMatrix())*glm::vec3(0,0,-1))*step;
    if(v[0]!=0.0 && v[1]!=0.0) v = glm::normalize(v)*step;
    else v = glm::vec2(0,1)*step;
    _motion[0] -= v[0];
    _motion[1] -= v[1];
  }

  if(ke->key()==Qt::Key_Q) {
    _motion[2] += step;
  }

  if(ke->key()==Qt::Key_D) {
    _motion[2] -= step;
  }

  // key a: play/stop animation
  if(ke->key()==Qt::Key_A) {
    if(_timer->isActive()) 
      _timer->stop();
    else 
      _timer->start();
  }

  // key i: init camera
  if(ke->key()==Qt::Key_I) {
    _cam->initialize(width(),height(),true);
  }
  
  // key f: compute FPS
  if(ke->key()==Qt::Key_F) {
    int elapsed;
    QTime timer;
    timer.start();
    unsigned int nb = 500;
    for(unsigned int i=0;i<nb;++i) {
      paintGL();
    }
    elapsed = timer.elapsed();
    double t = (double)nb/((double)elapsed);
    cout << "FPS : " << t*1000.0 << endl;
  }

  // key r: reload shaders 
  if(ke->key()==Qt::Key_R) {
    for(unsigned int i=0;i<_vertexFilenames.size();++i) {
      _shaders[i]->reload(_vertexFilenames[i].c_str(),_fragmentFilenames[i].c_str());
    }
  }

  // key S: show the shadow map 
  if(ke->key()==Qt::Key_S) {
    _showShadowMap = !_showShadowMap;
  }

  updateGL();
}

void Viewer::initializeGL() {
  // make this window the current one
  makeCurrent();

  // init and chack glew
  if(glewInit()!=GLEW_OK) {
    cerr << "Warning: glewInit failed!" << endl;
  }

  if(!GLEW_ARB_vertex_program   ||
     !GLEW_ARB_fragment_program ||
     !GLEW_ARB_texture_float    ||
     !GLEW_ARB_draw_buffers     ||
     !GLEW_ARB_framebuffer_object) {
    cerr << "Warning: Shaders not supported!" << endl;
  }

  // init OpenGL settings
  glClearColor(0.4,0.7,0.9,1.0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glViewport(0,0,width(),height());

  // initialize camera
  _cam->initialize(width(),height(),true);

  // load shader files
  createShaders();

  // init and load all shader files
  for(unsigned int i=0;i<_vertexFilenames.size();++i) {
    _shaders.push_back(new Shader());
    _shaders[i]->load(_vertexFilenames[i].c_str(),_fragmentFilenames[i].c_str());
  }

  // init shaders 
  createShaders();

  // init VAO/VBO
  createVAO();
    
  // create/init FBO
  createFBO();
  createTextures();
  updateFBO();

  // starts the timer 
  _timer->start();
}

