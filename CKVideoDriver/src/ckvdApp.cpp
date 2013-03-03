#include "ckvdApp.h"
#include <algorithm>

#define SIDEBAR_WIDTH 200
#define MIN_WIDTH 640
#define MIN_HEIGHT 480
#define DEFAULT_SYPHON_APP "Arena"
#define DEFAULT_SYPHON_SERVER "Composition"
#define DEFAULT_FRAME_RATE 30

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
    
    void drawCornerTickBox(int x, int y, int width, int height, int l)
    {
        ofPushStyle();
        
        ofNoFill();
        ofSetLineWidth(l);
        
        ofRect(x, y, width, height);
        ofLine(x, y, x+width/5, y+height/5);
        ofLine(x+4*width/5, y+height/5, x+width, y);
        ofLine(x+4*width/5, y+4*height/5, x+width, y+height);
        ofLine(x, y+height, x+width/5, y+4*height/5);

        ofPopStyle();
    }
}

bool ckvdVideoGrabber::isFocused()
{
    return theApp()->getSelectedGrabber() == this;
}

void ckvdVideoGrabber::mousePressed(ofMouseEventArgs &e)
{
    if (!isOver(e.x, e.y))
        return;
    
    theApp()->setSelectedGrabber(this);
    ofxTangibleHandle::mousePressed(e);

    // hack to prevent other ofxTangibleHandles from processing event
    e.x = -999;
    e.y = -999;
}

void ckvdVideoGrabber::moveBy(float dx, float dy)
{
    int w = theApp()->getClientWidth();
    int h = theApp()->getHeight();
    dx = x + dx < 0 ? -x : x + dx >= w ? w-x-1 : dx;
    dy = y + dy < 0 ? -y : y + dy >= h ? h-y-1 : dy;
    ofxTangibleHandle::moveBy(dx, dy);
}


ckvdSingleColorGrabber::ckvdSingleColorGrabber()
: _fixture(0)
{
    static int count = 0;

    drawType = TANGIBLE_DRAW_AS_CENTER_RECT;

    int cx = 40*((count)%12+1);
    int cy = 35*((count/12)+1);

    setup(cx, cy, 20, 20);
    
    _fixture.setAddress(count*3);
    count++;
}

void ckvdSingleColorGrabber::draw()
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
        theApp()->getGrabberFont()->drawString(_fixture.getName(), x+width/2+3, y-height/2+3);
    }
    
    ofPopStyle();
}

void ckvdSingleColorGrabber::setColorFromFrame(ofImage& frame)
{
    _color = frame.getColor(x, y);
    _fixture.set_rgb(_color.r, _color.g, _color.b);
}

void ckvdSingleColorGrabber::listParams(vector<string>* pParams)
{
    if (pParams)
    {
        pParams->push_back("ADDRESS");
    }
}

int ckvdSingleColorGrabber::getParameterInt(const string& name) const
{
    if (name.compare("ADDRESS") == 0)
        return _fixture.getAddress();
    return -1;
}

void ckvdSingleColorGrabber::setParameterInt(const string& name, int val)
{
    if (name.compare("ADDRESS") == 0)
        _fixture.setAddress(val);
}


ckvdTileGrabber::ckvdTileGrabber()
: _fixture(0)
, _scale(16)
{
    static int count = 0;
    
    drawType = TANGIBLE_DRAW_AS_RECT;
    
    int cx = 20+96*((count)%5+1);
    int cy = 30+96*((count/5)+1);
    
    setup(cx, cy, 12*_scale, 12*_scale);
    
    _fixture.setChannel(1+count*2);
    count++;
}

void ckvdTileGrabber::draw()
{
    ofPushStyle();
    
    if (isFocused())
    {
        ofSetColor(255, 255, 100);
        drawCornerTickBox(x, y, width, height, 5);
        ofSetColor(60, 60, 60);
        drawCornerTickBox(x, y, width, height, 1);
        ofSetColor(255, 255, 100);
    }
    else
    {
        ofSetColor(255, 255, 255);
        drawCornerTickBox(x, y, width, height, 1);
    }
    
    if (theApp()->getGrabberFont())
    {
        string name = _fixture.getName();
        ofTrueTypeFont* pFont = theApp()->getGrabberFont();
        pFont->drawString(name, x+(width-pFont->stringWidth(name))/2, y+(height+pFont->stringHeight(name))/2);
    }
    
    ofPopStyle();
}


void ckvdTileGrabber::setColorFromFrame(ofImage& frame)
{
    _fixture.setVideoRect(x, y, width, height);
    _fixture.setSourceData(frame.getPixels(), frame.width, frame.height, 3);
}

void ckvdTileGrabber::listParams(vector<string>* pParams)
{
    if (pParams)
    {
        pParams->push_back("CHANNEL");
        pParams->push_back("SCALE");
    }
}

int ckvdTileGrabber::getParameterInt(const string& name) const
{
    if (name.compare("CHANNEL") == 0)
        return _fixture.getChannel();
    if (name.compare("SCALE") == 0)
        return _scale;
    return -1;
}

void ckvdTileGrabber::setParameterInt(const string& name, int val)
{
    if (name.compare("CHANNEL") == 0)
        _fixture.setChannel(val);
    if (name.compare("SCALE") == 0)
    {
        _scale = val;
        width = 12*_scale;
        height = 12*_scale;
    }
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
    vector<ofxUIWidget*> addTextInput(ofxUICanvas* pUI, string name, string text, int width, string inlineLabel = "")
    {
        vector<ofxUIWidget*> widgets;
        
        if (inlineLabel.size())
        {
            auto pLabel = new ofxUILabel(75, inlineLabel, OFX_UI_FONT_SMALL, 24);
            widgets.push_back(pUI->addWidgetDown(pLabel));
        }
        
        auto pInput = new ofxUITextInput(name, text, width, 0, 0, 0, OFX_UI_FONT_SMALL);
        pInput->setAutoClear(false);
        
        if (inlineLabel.size())
        {
            widgets.push_back(pUI->addWidgetRight(pInput));
        }
        else
        {
            widgets.push_back(pUI->addWidgetDown(pInput));
        }
        
        return widgets;
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

    _pUI->addWidgetDown(new ofxUILabelButton("+ PT", false));
    _pUI->addWidgetRight(new ofxUILabelButton("+ TILE", false));
    _pUI->addWidgetRight(new ofxUILabelButton("X", false));
    
    _pUI->addSpacer(1,12)->setDrawFill(false);
    
    _lastStaticWidget = _pUI->addWidgetDown(new ofxUILabel("SELECTED FIXTURE", OFX_UI_FONT_SMALL));
    
    ofAddListener(_pUI->newGUIEvent, this, &ckvdApp::guiEvent);
    //_pUI->loadSettings("GUI/guiSettings.xml");
    _pGrabberFont = _pUI->getFontSmall();
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

void ckvdApp::update()
{
    sizeToContent();
}

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
        //_pUI->getWidget("FIX_ADDR")->setVisible(bVisible);
        //_pUI->getWidget("ADDRESS")->setVisible(bVisible);
        _pUI->getWidget("X")->setVisible(bVisible);
    }

    mClientImage.grabScreen(0, 0, getClientWidth(), getHeight());
    
    _pPds->clearFixtures();
    
    for (auto iter = _grabbers.begin(); iter != _grabbers.end(); iter++)
    {
        (*iter)->setColorFromFrame(mClientImage);
        (*iter)->draw();
        
        _pPds->addFixture((*iter)->getFixture());
    }
    
    _pPds->go();
    
}

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
            if (_pSelectedGrabber && !_pUI->hasKeyboardFocus())
                deleteSelected();
            break;
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

void ckvdApp::setSelectedGrabber(ckvdVideoGrabber* pGrabber)
{
    _pSelectedGrabber = pGrabber;
 
    for (auto it = _contextWidgets.begin(); it != _contextWidgets.end(); ++it)
    {
        _pUI->removeWidget(*it);
    }
    _contextWidgets.clear();
    _pUI->setPlacer(_lastStaticWidget);
    
    if (!pGrabber)
        return;
    
    vector<string> params;
    _pSelectedGrabber->listParams(&params);
    
    for (auto it = params.begin(); it != params.end(); ++it)
    {
        std::stringstream s;
        s << _pSelectedGrabber->getParameterInt(*it);
        auto widgets = addTextInput(_pUI, *it, s.str(), 90, *it);
        _contextWidgets.insert(_contextWidgets.begin(), widgets.begin(), widgets.end());
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
        setSelectedGrabber(NULL);
    }
}

void ckvdApp::exit()
{
    //_pUI->saveSettings("GUI/guiSettings.xml");
    delete _pUI;
}

namespace
{
    void updateWidgetParameterInt(const string& paramName, ofxUITextInput* pInput, ckvdVideoGrabber* pGrabber)
    {
        if (pInput && pInput->getTextString() != "" && pGrabber)
        {
            int addr;
            std::istringstream(pInput->getTextString()) >> addr;
            pGrabber->setParameterInt(paramName, addr);
        }
    }
}

void ckvdApp::guiEvent(ofxUIEventArgs &e)
{
	if(e.widget->getName() == "+ PT")
    {
        ofxUIButton* pButton = (ofxUIButton*)e.widget;
        if (pButton && pButton->getValue())
            _grabbers.push_back(new ckvdSingleColorGrabber());
    }
	if(e.widget->getName() == "+ TILE")
    {
        ofxUIButton* pButton = (ofxUIButton*)e.widget;
        if (pButton && pButton->getValue())
            _grabbers.push_back(new ckvdTileGrabber());
    }
	if(e.widget->getName() == "X")
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
    else if(e.widget->getName() == "ADDRESS")
    {
        updateWidgetParameterInt("ADDRESS", (ofxUITextInput*)e.widget, _pSelectedGrabber);
    }
    else if(e.widget->getName() == "CHANNEL")
    {
        updateWidgetParameterInt("CHANNEL", (ofxUITextInput*)e.widget, _pSelectedGrabber);
    }
    else if(e.widget->getName() == "SCALE")
    {
        updateWidgetParameterInt("SCALE", (ofxUITextInput*)e.widget, _pSelectedGrabber);
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



