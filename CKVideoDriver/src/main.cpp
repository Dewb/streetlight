#include "ofMain.h"
#include "ckvdApp.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main( ){
    ofAppGlutWindow window;
	
    ckvdApp* pApp = new ckvdApp();
    pApp->connect();
    
    ofSetupOpenGL(&window, pApp->getClientWidth(), pApp->getClientHeight(), OF_WINDOW);
    ofRunApp(pApp);
}
