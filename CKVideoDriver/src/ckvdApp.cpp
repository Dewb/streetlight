#include "ckvdApp.h"

//--------------------------------------------------------------
ckvdApp::ckvdApp()
: _x(-1)
, _y(-1)
{
    _pPds = new PowerSupply("10.0.0.150");
    _pFix = new FixtureRGB(0);
    _pPds->addFixture(_pFix);
}

ckvdApp::~ckvdApp()
{
    delete _pPds;
}

void ckvdApp::connect()
{
	mClient.setup();
    mClient.setApplicationName("Arena");
    mClient.setServerName("Composition");
}

void ckvdApp::setup()
{
	ofSetWindowTitle("CKVideoDriver");
    mClientImage.setUseTexture(false);
}

//--------------------------------------------------------------
void ckvdApp::update()
{
    int imgW = floor(mClient.getWidth());
    int imgH = floor(mClient.getHeight());
    if (imgW > 0 && imgH > 0)
        ofSetWindowShape(imgW, imgH);
}

//--------------------------------------------------------------
void ckvdApp::draw()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ofSetColor(255, 255, 255);
 
    mClient.draw(0, 0);

    if (_x > 0 && _y > 0)
    {
        mClientImage.grabScreen(0, 0, mClient.getWidth(), mClient.getHeight());
        ofColor color = mClientImage.getColor(_x, _y);
        
        //mClient.readToPixels(mPixels);
        //ofColor color = mPixels.getColor(_x, _y);
        
        int d = 8;
        ofNoFill();
        ofRect(_x-d, _y-d, d*2, d*2);
        
        ofLine(_x, _y-d, _x, _y-d*1.5);
        ofLine(_x, _y+d, _x, _y+d*1.5);
        ofLine(_x-d, _y, _x-d*1.5, _y);
        ofLine(_x+d, _y, _x+d*1.5, _y);
    
        ofSetColor(color);
        ofFill();
        d--;
        ofRect(_x-d-1, _y-d, d*2+1, d*2+1);
        
        _pFix->set_rgb(color.r, color.g, color.b);
        _pPds->go();
    }
}


//--------------------------------------------------------------
void ckvdApp::keyPressed(int key)
{
}

void ckvdApp::mouseReleased(int x, int y, int button)
{
    _x = x;
    _y = y;
}

void ckvdApp::windowResized(int w, int h)
{
    int imgW = floor(mClient.getWidth());
    int imgH = floor(mClient.getHeight());
    if (imgW > 0 && imgH > 0 && (w > imgW || h > imgH))
        ofSetWindowShape(imgW, imgH);
}

