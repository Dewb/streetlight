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

    int getWidth();
    int getHeight();
    
    static void setSelectedGrabber(ckvdPixelGrabber* pGrabber) { _pSelectedGrabber = pGrabber; }
    static ckvdPixelGrabber* getSelectedGrabber() { return _pSelectedGrabber; }
    static ofTrueTypeFont* getGrabberFont() { return _pGrabberFont; }
    
protected:
	ckvdSyphonClient mClient;
    ofImage mClientImage;
    
    PowerSupply* _pPds;
    
    ofxUICanvas* _pUI;
    std::vector<ckvdPixelGrabber*> _grabbers;
    
    static ckvdPixelGrabber* _pSelectedGrabber;
    static ofTrueTypeFont* _pGrabberFont;
};


#endif

