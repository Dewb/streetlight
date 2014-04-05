#include "ofMain.h"
#include "ckvdApp.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main( ){
    ofAppGlutWindow window;
	
    ckvdApp* pApp = new ckvdApp();
    pApp->connect();
    
    ofSetupOpenGL(&window, pApp->getWidth(), pApp->getHeight(), OF_WINDOW);
    ofRunApp(pApp);
}
