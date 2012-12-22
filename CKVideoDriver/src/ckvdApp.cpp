#include "ckvdApp.h"

#define SIDEBAR_WIDTH 170

bool ckvdPixelGrabber::isFocused()
{
    return ckvdApp::getSelectedGrabber() == this;
}

void ckvdPixelGrabber::mousePressed(ofMouseEventArgs &e)
{
    if (!isOver(e.x, e.y))
        return;
    
    ckvdApp::setSelectedGrabber(this);
    ofxTangibleHandle::mousePressed(e);

    // hack to prevent other ofxTangibleHandles from processing event
    e.x = -999;
    e.y = -999;
}

ckvdSinglePixelGrabber::ckvdSinglePixelGrabber()
{
    static int count = 0;

    drawType = TANGIBLE_DRAW_AS_CENTER_RECT;

    int cx = 40*((count)%12+1);
    int cy = 35*((count/12)+1);

    setup(cx, cy, 20, 20);
    
    fixture.setAddress(count*3);
    count++;
}

namespace
{
    void drawCrosshair(int x, int y, int width, int height, int l)
    {
        ofPushStyle();
        
        ofNoFill();
        ofSetLineWidth(l);
        
        int dw = width/2;
        int dh = height/2;
        
        ofRect(x-dw, y-dh, width, height);
        
        ofLine(x, y-dh, x, y-dh*1.5);
        ofLine(x, y+dh, x, y+dh*1.5+1);
        ofLine(x-dw, y, x-dw*1.5-1, y);
        ofLine(x+dw, y, x+dw*1.5, y);

        ofPopStyle();
    }
}

void ckvdSinglePixelGrabber::draw()
{
    ofPushStyle();
    
    ofSetColor(_color);
    ofFill();
    ofRect(x-width/2, y-height/2, width, height);
    
    if (isFocused())
    {
        ofSetColor(255, 255, 100);
        drawCrosshair(x, y, width, height, 5);
        ofSetColor(60, 60, 60);
        drawCrosshair(x, y, width, height, 1);
        ofSetColor(255, 255, 100);
    }
    else
    {
        ofSetColor(255, 255, 255);
        drawCrosshair(x, y, width, height, 1);
    }
    
    if (ckvdApp::getGrabberFont())
    {
        char label[10];
        snprintf(label, 10, "%d", fixture.getAddress());
        ckvdApp::getGrabberFont()->drawString(label, x+width/2+3, y-height/2+3);
    }
    ofPopStyle();
}

void ckvdSinglePixelGrabber::setColorFromFrame(ofImage& frame)
{
    _color = frame.getColor(x, y);
    fixture.set_rgb(_color.r, _color.g, _color.b);
}

void ckvdManyPixelGrabber::draw()
{
    ofxTangibleHandle::draw();
}


//--------------------------------------------------------------
ckvdApp::ckvdApp()
{
    _pPds = new PowerSupply("10.0.0.150");
}

ckvdPixelGrabber* ckvdApp::_pSelectedGrabber = NULL;
ofTrueTypeFont* ckvdApp::_pGrabberFont = NULL;
namespace
{
    template <typename T>
    void freeVector(std::vector<T> v)
    {
        for (auto iter = v.begin(); iter != v.end(); iter++)
            delete *iter;
        v.clear();
    }
}

ckvdApp::~ckvdApp()
{
    delete _pPds;
    
    freeVector(_grabbers);
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
    ofSetFrameRate(30);
    
    ckvdPixelGrabber* pGrabber = new ckvdSinglePixelGrabber();
    _grabbers.push_back(pGrabber);
}

//--------------------------------------------------------------
void ckvdApp::update()
{
    int imgW = getWidth();
    int imgH = getHeight();
    if (imgW > 0 && imgH > 0)
        ofSetWindowShape(imgW, imgH);    
}

//--------------------------------------------------------------
void ckvdApp::draw()
{
    ofBackground(0,0,0);
 
    mClient.draw(0, 0);
    
    if (!_pUI && mClient.getWidth() > 0) // wait until we've loaded a client image before creating UI
    {
        _pUI = new ofxUICanvas(mClient.getWidth(), 0, SIDEBAR_WIDTH, getHeight());
        _pUI->setFont("GUI/Exo-Regular.ttf", true, true, false, 0.0, OFX_UI_FONT_RESOLUTION);
        _pUI->addWidgetDown(new ofxUILabel("PDS IP ADDRESS", OFX_UI_FONT_MEDIUM));
        _pUI->addTextInput("IP", "10.0.0.150", 150)->setAutoClear(false);
        _pUI->addWidgetDown(new ofxUILabelButton("+ ADD FIXTURE", false));
        ofAddListener(_pUI->newGUIEvent, this, &ckvdApp::guiEvent);
        //_pUI->loadSettings("GUI/guiSettings.xml");
        _pGrabberFont = _pUI->getFontSmall();
    }
    
    if (_pUI)
    {
        ofSetColor(60,60,80);
        ofRect(mClient.getWidth(), 0, SIDEBAR_WIDTH, getHeight());
        ofSetColor(255,255,255);
    }

    mClientImage.grabScreen(0, 0, mClient.getWidth(), mClient.getHeight());
    
    _pPds->clearFixtures();
    
    for (auto iter = _grabbers.begin(); iter != _grabbers.end(); iter++)
    {
        (*iter)->setColorFromFrame(mClientImage);
        (*iter)->draw();
        
        _pPds->addFixture(&((*iter)->fixture));
    }
    
    _pPds->go();
    
}


//--------------------------------------------------------------
void ckvdApp::keyPressed(int key)
{
}

void ckvdApp::mouseReleased(int x, int y, int button)
{
}

void ckvdApp::windowResized(int w, int h)
{
    int imgW = getWidth();
    int imgH = getHeight();
    if (imgW > 0 && imgH > 0 && (w > imgW || h > imgH))
    {
        ofSetWindowShape(imgW, imgH);
    }
}

int ckvdApp::getWidth()
{
    return floor(mClient.getWidth()) + SIDEBAR_WIDTH;
}

int ckvdApp::getHeight()
{
    return floor(mClient.getHeight());
}

void ckvdApp::exit()
{
    //_pUI->saveSettings("GUI/guiSettings.xml");
    delete _pUI;
}

void ckvdApp::guiEvent(ofxUIEventArgs &e)
{
	if(e.widget->getName() == "+ ADD FIXTURE")
    {
        ofxUIButton* pButton = (ofxUIButton*)e.widget;
        if (pButton && pButton->getValue())
            _grabbers.push_back(new ckvdSinglePixelGrabber());
    }
    else if(e.widget->getName() == "IP")
    {
        ofxUITextInput* pInput = (ofxUITextInput*)e.widget;
        if (pInput && pInput->getTextString() != "")
        {
            delete _pPds;
            _pPds = new PowerSupply(pInput->getTextString().c_str());
        }
    }
}


