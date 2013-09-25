#include "ckvdApp.h"
#include <algorithm>

#define SIDEBAR_WIDTH 200
#define MIN_WIDTH 800
#define MIN_HEIGHT 600
#define MAX_WIDTH 1200
#define MAX_HEIGHT 900
#define DEFAULT_SYPHON_APP "Arena"
#define DEFAULT_SYPHON_SERVER "Composition"
#define DEFAULT_FRAME_RATE 30
#define DEFAULT_PDS_IP_1 "10.0.2.2"
#define DEFAULT_PDS_IP_2 "10.0.2.3"


ckvdApp* _theApp = NULL;
ckvdApp* theApp()
{
    return _theApp;
}

//--------------------------------------------------------------
ckvdApp::ckvdApp()
: _pSelectedGrabber(NULL)
, _pGrabberFont(NULL)
{
    assert(_theApp == NULL);
    _theApp = this;
    _supplies.push_back(new PowerSupply(DEFAULT_PDS_IP_1));
    _supplies.push_back(new PowerSupply(DEFAULT_PDS_IP_2));
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
    freeVector(_supplies);
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
    
    _pUI->addWidgetDown(new ofxUILabel("POWER SUPPLY 1 ADDRESS", OFX_UI_FONT_SMALL));
    addTextInput(_pUI, "PDS_IP_1", DEFAULT_PDS_IP_1, 100);
    _pUI->addWidgetDown(new ofxUILabel("POWER SUPPLY 2 ADDRESS", OFX_UI_FONT_SMALL));
    addTextInput(_pUI, "PDS_IP_2", DEFAULT_PDS_IP_2, 100);

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
    
    for (auto iter = _supplies.begin(); iter != _supplies.end(); iter++)
    {
        (*iter)->clearFixtures();
    }
    
    for (auto iter = _grabbers.begin(); iter != _grabbers.end(); iter++)
    {
        (*iter)->setColorFromFrame(mClientImage);
        (*iter)->draw();
        int nSupply = (*iter)->getParameterInt("SUPPLY") - 1;
        if (nSupply >= 0 && nSupply < _supplies.size())
            _supplies[nSupply]->addFixture((*iter)->getFixture());
    }

    for (auto iter = _supplies.begin(); iter != _supplies.end(); iter++)
    {
        if ((*iter)->getFixtureCount() > 0)
            (*iter)->go();
    }

    
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
    return w > MIN_WIDTH ? (w < MAX_WIDTH ? w : MAX_WIDTH) : MIN_WIDTH;
}

int ckvdApp::getWidth()
{
    return getClientWidth() + SIDEBAR_WIDTH;
}

int ckvdApp::getHeight()
{
    int h = floor(mClient.getHeight());
    return h > MIN_HEIGHT ? (h < MAX_HEIGHT ? h : MAX_HEIGHT) : MIN_HEIGHT;
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
    else if(e.widget->getName() == "PDS_IP_1")
    {
        ofxUITextInput* pInput = (ofxUITextInput*)e.widget;
        if (pInput && pInput->getTextString() != "")
        {
            delete _supplies[0];
            _supplies[0] = new PowerSupply(pInput->getTextString().c_str());
        }
    }
    else if(e.widget->getName() == "PDS_IP_2")
    {
        ofxUITextInput* pInput = (ofxUITextInput*)e.widget;
        if (pInput && pInput->getTextString() != "")
        {
            delete _supplies[1];
            _supplies[1] = new PowerSupply(pInput->getTextString().c_str());
        }
    }
    else if (e.widget->getName() == "ADDRESS" ||
             e.widget->getName() == "CHANNEL" ||
             e.widget->getName() == "SCALE" ||
             e.widget->getName() == "SUPPLY")
    {
        updateWidgetParameterInt(e.widget->getName(), (ofxUITextInput*)e.widget, _pSelectedGrabber);
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



