#include <qapplication.h>
#include <QString>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "viewer.h"

using namespace std;

char *getFilename(int ,char **argv) {
  //if(argc<2) {
    //cout << "Usage: " << argv[0] << " offFile" << endl;
    //exit(0);
  //}

  return argv[0];
}


int main(int argc,char** argv) {
  QApplication application(argc,argv);

  Viewer viewer(getFilename(argc,argv));

  viewer.setWindowTitle("Exercice 09 - Terrain rendering");
  viewer.show();
  
  return application.exec();
}
