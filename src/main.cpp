#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main( ){
    ckvdApp* pApp = new ckvdApp();
    pApp->connect();
    
    ofSetupOpenGL(pApp->getWidth(), pApp->getHeight(), OF_WINDOW);
    ofRunApp(pApp);
}
