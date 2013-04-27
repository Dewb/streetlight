#ifndef _CKVD_APP
#define _CKVD_APP


#include "ofMain.h"
#include "ofxSyphon.h"
#include "ofxUI.h"
#include "kinet.h"
#include "ckvdHandle.h"
#include <vector>
#include <memory>

using std::string;
using std::vector;
using std::map;

class ckvdSyphonClient : public ofxSyphonClient
{
public:
    void readToPixels(ofPixels& pixels) { mTex.readToPixels(pixels); }
};


class ckvdApp : public ofBaseApp
{
	
public:
    ckvdApp();
    ~ckvdApp();
	
    void connect();
	void setup();
	void update();
	void draw();
	
	void keyPressed(int key);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void exit();
    void guiEvent(ofxUIEventArgs& args);

    int getClientWidth();
    int getWidth();
    int getHeight();
    
    void setSelectedGrabber(ckvdVideoGrabber* pGrabber);
    ckvdVideoGrabber* getSelectedGrabber() const { return _pSelectedGrabber; }
    
    /*void setSelection(int id);
    void setSelection(vector<int> id);
    vector<int> getSelectedIds();
    */
    bool isHandleSelected(int id);
    
    
    ofTrueTypeFont* getGrabberFont() { return _pGrabberFont; }
    
    void deleteSelected();
    
protected:
    
    void sizeToContent();
    
	ckvdSyphonClient mClient;
    ofImage mClientImage;
    
    PowerSupply* _pPds;
    ofxUICanvas* _pUI;

    vector<ofxUIWidget*> _contextWidgets;
    ofxUIWidget* _lastStaticWidget;
    
    vector<ckvdVideoGrabber*> _grabbers;
    ckvdVideoGrabber* _pSelectedGrabber;

    ofTrueTypeFont* _pGrabberFont;
};

ckvdApp* theApp();


#endif

