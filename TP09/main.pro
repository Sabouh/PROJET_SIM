#GLEW_PATH = ../../ext/glew-1.9.0
GLM_PATH  = ../../ext/glm-0.9.4.1

TEMPLATE  = app
TARGET    = tp09

#LIBS     += -Wl,-rpath $${GLEW_PATH}/lib -L$${GLEW_PATH}/lib
LIBS     += -lGLEW -lGL -lGLU -lm
INCLUDEPATH  += $${GLEW_PATH}/include  $${GLM_PATH}

SOURCES   = shader.cpp grid.cpp trackball.cpp camera.cpp viewer.cpp main.cpp 
HEADERS   = shader.h grid.h trackball.h camera.h viewer.h

CONFIG   += qt opengl warn_on thread uic4 release
QT       *= xml opengl core
