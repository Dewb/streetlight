#ifndef _CKVD_APP
#define _CKVD_APP


#include "ofMain.h"
#include "ofxSyphon.h"
#include "ofxUI.h"
#include "ofxTangibleHandle.h"
#include "kinet.h"
#include <vector>

using std::string;
using std::vector;

class ckvdSyphonClient : public ofxSyphonClient
{
public:
    void readToPixels(ofPixels& pixels) { mTex.readToPixels(pixels); }
};


class ckvdVideoGrabber : public ofxTangibleHandle
{
public:
    ckvdVideoGrabber() {}
    bool isFocused();
    virtual void mousePressed(ofMouseEventArgs &e);
    virtual void moveBy(float dx, float dy);

    virtual void setColorFromFrame(ofImage& frame) = 0;
    virtual Fixture* getFixture() = 0;

    virtual void listParams(vector<string>* pParams) {}
    virtual int getParameterInt(const string& name) const { return -1; }
    virtual void setParameterInt(const string& name, int val) {}
};


class ckvdSingleColorGrabber : public ckvdVideoGrabber
{
public:
    ckvdSingleColorGrabber();
    virtual void draw();

    virtual void setColorFromFrame(ofImage& frame);
    virtual Fixture* getFixture() { return &_fixture; }

    virtual void listParams(vector<string>* pParams);
    virtual int getParameterInt(const string& name) const;
    virtual void setParameterInt(const string& name, int val);

protected:
    FixtureRGB _fixture;
    ofColor _color;
};

class ckvdTileGrabber : public ckvdVideoGrabber
{
public:
    ckvdTileGrabber();
    virtual void draw();

    virtual void setColorFromFrame(ofImage& frame);
    virtual Fixture* getFixture() { return &_fixture; }

    virtual void listParams(vector<string>* pParams);
    virtual int getParameterInt(const string& name) const;
    virtual void setParameterInt(const string& name, int val);

protected:
    FixtureTile _fixture;
    int _scale;
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
    ckvdVideoGrabber* getSelectedGrabber() { return _pSelectedGrabber; }
    ofTrueTypeFont* getGrabberFont() { return _pGrabberFont; }
    
    void deleteSelected();
    
protected:
    
    void sizeToContent();
    
	ckvdSyphonClient mClient;
    ofImage mClientImage;
    
    PowerSupply* _pPds;
    
    ofxUICanvas* _pUI;
    vector<ckvdVideoGrabber*> _grabbers;
    vector<ofxUIWidget*> _contextWidgets;
    ofxUIWidget* _lastStaticWidget;
    
    ckvdVideoGrabber* _pSelectedGrabber;
    ofTrueTypeFont* _pGrabberFont;
};

ckvdApp* theApp();


#endif

