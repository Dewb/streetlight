#include "ckvdApp.h"
#include <algorithm>

#define SIDEBAR_WIDTH 200
#define MIN_WIDTH 640
#define MIN_HEIGHT 480
#define DEFAULT_SYPHON_APP "Arena"
#define DEFAULT_SYPHON_SERVER "Composition"
#define DEFAULT_FRAME_RATE 30

bool ckvdPixelGrabber::isFocused()
{
    return theApp()->getSelectedGrabber() == this;
}

void ckvdPixelGrabber::mousePressed(ofMouseEventArgs &e)
{
    if (!isOver(e.x, e.y))
        return;
    
    theApp()->setSelectedGrabber(this);
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
    
    if (theApp()->getGrabberFont())
    {
        char label[10];
        snprintf(label, 10, "%d", fixture.getAddress());
        theApp()->getGrabberFont()->drawString(label, x+width/2+3, y-height/2+3);
    }
    ofPopStyle();
}

void ckvdSinglePixelGrabber::setColorFromFrame(ofImage& frame)
{
    _color = frame.getColor(x, y);
    fixture.set_rgb(_color.r, _color.g, _color.b);
}

void ckvdSinglePixelGrabber::moveBy(float dx, float dy)
{
    int w = theApp()->getClientWidth();
    int h = theApp()->getHeight();
    dx = x + dx < 0 ? -x : x + dx >= w ? w-x-1 : dx;
    dy = y + dy < 0 ? -y : y + dy >= h ? h-y-1 : dy;
    ofxTangibleHandle::moveBy(dx, dy);
}


void ckvdManyPixelGrabber::draw()
{
    ofxTangibleHandle::draw();
}

ckvdApp* _theApp = NULL;
ckvdApp* theApp()
{
    return _theApp;
}

//--------------------------------------------------------------
ckvdApp::ckvdApp()
: _pPds(new PowerSupply("10.0.2.2"))
, _pSelectedGrabber(NULL)
, _pGrabberFont(NULL)
{
    assert(_theApp == NULL);
    _theApp = this;
}


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
    mClient.setApplicationName(DEFAULT_SYPHON_APP);
    mClient.setServerName(DEFAULT_SYPHON_SERVER);
}

namespace
{
    void addTextInput(ofxUICanvas* pUI, string name, string text, int width, string inlineLabel = "")
    {
        if (inlineLabel.size())
        {
            auto pLabel = new ofxUILabel(75, inlineLabel, OFX_UI_FONT_SMALL, 24);
            pUI->addWidgetDown(pLabel);
        }
        
        auto pInput = new ofxUITextInput(name, text, width, 0, 0, 0, OFX_UI_FONT_SMALL);
        pInput->setAutoClear(false);
        
        if (inlineLabel.size())
        {
            pUI->addWidgetRight(pInput);
        }
        else
        {
            pUI->addWidgetDown(pInput);
        }
    }
}

void ckvdApp::setup()
{
	ofSetWindowTitle("CKVideoDriver");
    mClientImage.setUseTexture(false);
    ofSetFrameRate(DEFAULT_FRAME_RATE);
    
    _pUI = new ofxUICanvas(getClientWidth(), 0, SIDEBAR_WIDTH, getHeight());
    _pUI->setFont("GUI/Exo-Regular.ttf", true, true, false, 0.0, OFX_UI_FONT_RESOLUTION);

    _pUI->addWidgetDown(new ofxUILabel("SYPHON SERVER APP", OFX_UI_FONT_SMALL));
    
    addTextInput(_pUI, "SYPHON_APP", DEFAULT_SYPHON_APP, 180);
    _pUI->addWidgetDown(new ofxUILabel("SYPHON SERVER NAME", OFX_UI_FONT_SMALL));
    addTextInput(_pUI, "SYPHON_SERVER", DEFAULT_SYPHON_SERVER, 180);
    _pUI->addWidgetDown(new ofxUILabel("OUTPUT FRAME RATE", OFX_UI_FONT_SMALL));
    addTextInput(_pUI, "FRAME_RATE", "30", 40);
    
    _pUI->addSpacer(1,12)->setDrawFill(false);
    
    _pUI->addWidgetDown(new ofxUILabel("PDS IP ADDRESS", OFX_UI_FONT_SMALL));
    addTextInput(_pUI, "PDS_IP", "10.0.2.2", 100);

    _pUI->addSpacer(1,12)->setDrawFill(false);

    _pUI->addWidgetDown(new ofxUILabel("FIXTURES", OFX_UI_FONT_SMALL));
    _pUI->addWidgetDown(new ofxUILabelButton("+ ADD", false));
    _pUI->addWidgetRight(new ofxUILabelButton("- DELETE", false));
    
    _pUI->addSpacer(1,12)->setDrawFill(false);
    
    _pUI->addWidgetDown(new ofxUILabel("SELECTED FIXTURE", OFX_UI_FONT_SMALL));
    addTextInput(_pUI, "FIX_ADDR", "0", 90, "ADDRESS");
    
    ofAddListener(_pUI->newGUIEvent, this, &ckvdApp::guiEvent);
    //_pUI->loadSettings("GUI/guiSettings.xml");
    _pGrabberFont = _pUI->getFontSmall();
    
    ckvdPixelGrabber* pGrabber = new ckvdSinglePixelGrabber();
    _grabbers.push_back(pGrabber);
}

void ckvdApp::sizeToContent()
{
    int imgW = getWidth();
    int imgH = getHeight();
    
    ofSetWindowShape(imgW, imgH);
    if (_pUI)
    {
        _pUI->getRect()->setX(getClientWidth());
        _pUI->getRect()->setHeight(getHeight());
    }
}

//--------------------------------------------------------------
void ckvdApp::update()
{
    sizeToContent();
}

//--------------------------------------------------------------
void ckvdApp::draw()
{
    ofBackground(0,0,0);
 
    mClient.draw(0, 0);
    
    if (_pUI)
    {
        ofSetColor(60,60,80);
        ofRect(getClientWidth(), 0, SIDEBAR_WIDTH, getHeight());
        ofSetColor(255,255,255);
        
        bool bVisible = _pSelectedGrabber != NULL;
        _pUI->getWidget("SELECTED FIXTURE")->setVisible(bVisible);
        _pUI->getWidget("FIX_ADDR")->setVisible(bVisible);
        _pUI->getWidget("ADDRESS")->setVisible(bVisible);
    }

    mClientImage.grabScreen(0, 0, getClientWidth(), getHeight());
    
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
    switch (key)
	{
		case OF_KEY_UP:
            if (_pSelectedGrabber) _pSelectedGrabber->moveBy(0,-1); break;
		case OF_KEY_DOWN:
            if (_pSelectedGrabber) _pSelectedGrabber->moveBy(0,1); break;
		case OF_KEY_LEFT:
            if (_pSelectedGrabber) _pSelectedGrabber->moveBy(-1,0); break;
		case OF_KEY_RIGHT:
            if (_pSelectedGrabber) _pSelectedGrabber->moveBy(1,0); break;
        case OF_KEY_DEL:
        case OF_KEY_BACKSPACE:
            deleteSelected(); break;
    }
}

void ckvdApp::mouseReleased(int x, int y, int button)
{
}

void ckvdApp::windowResized(int w, int h)
{
    sizeToContent();
}

int ckvdApp::getClientWidth()
{
    int w = floor(mClient.getWidth());
    return w > MIN_WIDTH ? w : MIN_WIDTH;
}

int ckvdApp::getWidth()
{
    return getClientWidth() + SIDEBAR_WIDTH;
}

int ckvdApp::getHeight()
{
    int h = floor(mClient.getHeight());
    return h > MIN_HEIGHT ? h : MIN_HEIGHT;
}

void ckvdApp::setSelectedGrabber(ckvdPixelGrabber* pGrabber)
{
    _pSelectedGrabber = pGrabber;
    auto pText = (ofxUITextInput*)(_pUI->getWidget("FIX_ADDR"));
    if (pText)
    {
        std::stringstream s;
        s << _pSelectedGrabber->fixture.getAddress();
        pText->setTextString(s.str());
    }
}

void ckvdApp::deleteSelected()
{
    if (!_pSelectedGrabber)
        return;
    
    auto iter = std::find(_grabbers.begin(), _grabbers.end(), _pSelectedGrabber);
    if (iter != _grabbers.end())
    {
        _grabbers.erase(std::remove(iter, _grabbers.end(), _pSelectedGrabber), _grabbers.end());
        delete _pSelectedGrabber;
        _pSelectedGrabber = NULL;
    }
}

void ckvdApp::exit()
{
    //_pUI->saveSettings("GUI/guiSettings.xml");
    delete _pUI;
}

void ckvdApp::guiEvent(ofxUIEventArgs &e)
{
	if(e.widget->getName() == "+ ADD")
    {
        ofxUIButton* pButton = (ofxUIButton*)e.widget;
        if (pButton && pButton->getValue())
            _grabbers.push_back(new ckvdSinglePixelGrabber());
    }
	if(e.widget->getName() == "- DELETE")
    {
        ofxUIButton* pButton = (ofxUIButton*)e.widget;
        if (pButton && pButton->getValue())
        {
            deleteSelected();
        }
    }
    else if(e.widget->getName() == "PDS_IP")
    {
        ofxUITextInput* pInput = (ofxUITextInput*)e.widget;
        if (pInput && pInput->getTextString() != "")
        {
            delete _pPds;
            _pPds = new PowerSupply(pInput->getTextString().c_str());
        }
    }
    else if(e.widget->getName() == "FIX_ADDR")
    {
        ofxUITextInput* pInput = (ofxUITextInput*)e.widget;
        if (pInput && pInput->getTextString() != "" && _pSelectedGrabber)
        {
            int addr;
            std::istringstream(pInput->getTextString()) >> addr;
            _pSelectedGrabber->fixture.setAddress(addr);
        }
    }
    else if(e.widget->getName() == "SYPHON_APP")
    {
        ofxUITextInput* pInput = (ofxUITextInput*)e.widget;
        if (pInput)
        {
            mClient.setApplicationName(pInput->getTextString());
        }
    }
    else if(e.widget->getName() == "SYPHON_SERVER")
    {
        ofxUITextInput* pInput = (ofxUITextInput*)e.widget;
        if (pInput)
        {
            mClient.setServerName(pInput->getTextString());
        }
    }
    else if (e.widget->getName() == "FRAME_RATE")
    {
        ofxUITextInput* pInput = (ofxUITextInput*)e.widget;
        if (pInput)
        {
            int rate;
            std::istringstream(pInput->getTextString()) >> rate;
            ofSetFrameRate(rate);
        }
    }
}



