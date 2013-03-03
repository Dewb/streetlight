#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxSyphon.h"
#include "ofxUI.h"
#include "ofxTangibleHandle.h"
#include "kinet.h"
#include <vector>


class ckvdSyphonClient : public ofxSyphonClient
{
public:
    void readToPixels(ofPixels& pixels) { mTex.readToPixels(pixels); }
};


class ckvdPixelGrabber : public ofxTangibleHandle
{
public:
    ckvdPixelGrabber() : fixture(0) {}
    virtual void setColorFromFrame(ofImage& frame) = 0;
    bool isFocused();
    virtual void mousePressed(ofMouseEventArgs &e);

    FixtureRGB fixture;
};


class ckvdSinglePixelGrabber : public ckvdPixelGrabber
{
public:
    ckvdSinglePixelGrabber();
    virtual void draw();
    virtual void setColorFromFrame(ofImage& frame);
    virtual void moveBy(float dx, float dy);
    
protected:
    ofColor _color;
};


class ckvdManyPixelGrabber : public ckvdPixelGrabber
{
public:
    void setSize(int x, int y) { _pixelsX = x; _pixelsY = y; }
    virtual void draw();
    virtual void setColorFromFrame(ofImage& frame);
    
protected:
    int _pixelsX;
    int _pixelsY;
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
    
    void setSelectedGrabber(ckvdPixelGrabber* pGrabber);
    ckvdPixelGrabber* getSelectedGrabber() { return _pSelectedGrabber; }
    ofTrueTypeFont* getGrabberFont() { return _pGrabberFont; }
    
    void deleteSelected();
    
protected:
    
    void sizeToContent();
    
	ckvdSyphonClient mClient;
    ofImage mClientImage;
    
    PowerSupply* _pPds;
    
    ofxUICanvas* _pUI;
    std::vector<ckvdPixelGrabber*> _grabbers;
    
    ckvdPixelGrabber* _pSelectedGrabber;
    ofTrueTypeFont* _pGrabberFont;
};

ckvdApp* theApp();


#endif

