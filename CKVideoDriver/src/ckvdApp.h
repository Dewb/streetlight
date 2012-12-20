#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxSyphon.h"
#include "kinet.h"

class pixelSyphonClient : public ofxSyphonClient
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

    int getClientWidth() { return floor(mClient.getWidth()); }
    int getClientHeight() { return floor(mClient.getHeight()); }
protected:
	pixelSyphonClient mClient;
    ofImage mClientImage;
    
    int _x;
    int _y;
    
    PowerSupply* _pPds;
    FixtureRGB* _pFix;
};

#endif

