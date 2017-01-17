#include "ckvdApp.h"
#include "ofxXmlSettings.h"

#include <algorithm>

#define SIDEBAR_WIDTH 260
#define MIN_WIDTH 800
#define MIN_HEIGHT 600
#define MAX_WIDTH 1200
#define MAX_HEIGHT 900
#define DEFAULT_SYPHON_APP "Arena"
#define DEFAULT_SYPHON_SERVER "Composition"
#define DEFAULT_FRAME_RATE 30
#define DEFAULT_PDS_IP_0 "10.0.0.101"
#define DEFAULT_PDS_IP_1 "10.0.0.102"
#define DEFAULT_PDS_IP_2 "10.0.0.103"
#define DEFAULT_PDS_IP_3 "10.0.0.104"
#define DEFAULT_PDS_IP_4 "10.0.0.105"
#define DEFAULT_PDS_IP_5 "10.0.0.106"
#define DEFAULT_PDS_IP_6 "10.0.100.101"
#define DEFAULT_PDS_IP_7 "10.0.100.102"
#define DEFAULT_PDS_IP_8 "10.0.100.103"
#define DEFAULT_PDS_IP_9 "10.0.100.104"
#define DEFAULT_PDS_IP_10 "10.0.100.105"
#define DEFAULT_PDS_IP_11 "10.0.100.106"

#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#endif

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
#ifdef _WIN32
	WORD wVersion;
	WSADATA wsaData;
	wVersion = MAKEWORD(2, 2);
	int err = WSAStartup(wVersion, &wsaData);
	if (err != 0) {
		fprintf(stderr, "WSAStartup failed with error: %d\n", err);
	}
#endif

    assert(_theApp == NULL);
    _theApp = this;
    _supplies.push_back(new PowerSupply(DEFAULT_PDS_IP_0));
    _supplies.push_back(new PowerSupply(DEFAULT_PDS_IP_1));
    _supplies.push_back(new PowerSupply(DEFAULT_PDS_IP_2));
    _supplies.push_back(new PowerSupply(DEFAULT_PDS_IP_3));
    _supplies.push_back(new PowerSupply(DEFAULT_PDS_IP_4));
    _supplies.push_back(new PowerSupply(DEFAULT_PDS_IP_5));
    _supplies.push_back(new PowerSupply(DEFAULT_PDS_IP_6));
    _supplies.push_back(new PowerSupply(DEFAULT_PDS_IP_7));
    _supplies.push_back(new PowerSupply(DEFAULT_PDS_IP_8));
    _supplies.push_back(new PowerSupply(DEFAULT_PDS_IP_9));
    _supplies.push_back(new PowerSupply(DEFAULT_PDS_IP_10));
    _supplies.push_back(new PowerSupply(DEFAULT_PDS_IP_11));
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

void ckvdApp::loadSettings() {
	 ofxXmlSettings settings;
        if(!settings.loadFile("settings.xml"))
            return;
        
        for (auto grabber : _grabbers) {
            delete grabber;
        }
        _grabbers.clear();
        
        settings.pushTag("settings");

        settings.pushTag("syphon");
        updateSyphonApp(settings.getValue("app", ""));
        updateSyphonServer(settings.getValue("server", ""));
        settings.popTag();
        
        settings.pushTag("supplies");
        for(int i = 0; i < settings.getNumTags("supply"); i++){
            settings.pushTag("supply", i);
            if (i < 12) {
                updatePowerSupplyAddress(i, settings.getValue("host", ""));
            }
            settings.popTag();
        }
        settings.popTag();
        
        settings.pushTag("grabbers");
        for(int i = 0; i < settings.getNumTags("grabber"); i++){
            settings.pushTag("grabber", i);
            ckvdVideoGrabber* pGrabber = NULL;
            
            string typeName = settings.getValue("type", "");
            if (typeName == "point") {
                pGrabber = new ckvdSingleColorGrabber();
            } else if (typeName == "tile") {
                pGrabber = new ckvdTileGrabber();
            } else if (typeName == "strip") {
                pGrabber = new ckvdStripGrabber();
            }
            
            settings.pushTag("parameters");
            if (pGrabber) {
                for(int j = 0; j < settings.getNumTags("param"); j++) {
                    settings.pushTag("param", j);
                    pGrabber->setParameterInt(settings.getValue("name", ""), settings.getValue("value", 0));
                    settings.popTag();
                }
                _grabbers.push_back(pGrabber);
            }
            settings.popTag();
            
            settings.popTag();
        }
        settings.popTag();
        
}

ckvdApp::~ckvdApp()
{
    freeVector(_supplies);
    freeVector(_grabbers);
}

void ckvdApp::connect()
{
    mClient.setApplicationName(DEFAULT_SYPHON_APP);
    mClient.setServerName(DEFAULT_SYPHON_SERVER);
}

namespace
{
    vector<ofxUIWidget*> addTextInput(ofxUICanvas* pUI, string name, string text, int width, string inlineLabel = "", bool addRight = false)
    {
        vector<ofxUIWidget*> widgets;
        
        if (inlineLabel.size())
        {
            auto pLabel = new ofxUILabel(75, inlineLabel, OFX_UI_FONT_SMALL, 24);
            widgets.push_back(addRight ? pUI->addWidgetRight(pLabel) : pUI->addWidgetDown(pLabel));
        }
        
        auto pInput = new ofxUITextInput(name, text, width, 0, 0, 0, OFX_UI_FONT_SMALL);
        pInput->setAutoClear(false);
        
        if (inlineLabel.size() || addRight)
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
	ofSetWindowTitle("Streetlight // Syphon to LEDs");
    mClientImage.setUseTexture(false);
    ofSetFrameRate(DEFAULT_FRAME_RATE);
    
    _pUI = new ofxUICanvas(getClientWidth(), 0, SIDEBAR_WIDTH, getHeight());
    _pUI->setFont("GUI/Exo-Regular.ttf", true, true, false, 0.0, OFX_UI_FONT_RESOLUTION);

    _pUI->addWidgetDown(new ofxUILabel("SYPHON SERVER APP", OFX_UI_FONT_SMALL));
    
    addTextInput(_pUI, "SYPHON_APP", DEFAULT_SYPHON_APP, 180);
    _pUI->addWidgetDown(new ofxUILabel("SYPHON SERVER NAME", OFX_UI_FONT_SMALL));
    addTextInput(_pUI, "SYPHON_SERVER", DEFAULT_SYPHON_SERVER, 180);
    addTextInput(_pUI, "FRAME_RATE", "30", 40);
    _pUI->addWidgetRight(new ofxUILabel("FRAMES PER SEC", OFX_UI_FONT_SMALL));
    
    _pUI->addSpacer(1,6)->setDrawFill(false);
    
    _pUI->addWidgetDown(new ofxUILabel("CONFIG", OFX_UI_FONT_SMALL));
    _pUI->addWidgetRight(new ofxUILabelButton("LOAD", false));
    _pUI->addWidgetRight(new ofxUILabelButton("SAVE", false));
    
    _pUI->addSpacer(1,12)->setDrawFill(false);
    
    _pUI->addWidgetDown(new ofxUILabel("POWER SUPPLY ADDRESSES", OFX_UI_FONT_SMALL));
    addTextInput(_pUI, "PDS_IP_0", DEFAULT_PDS_IP_0, 120);
    addTextInput(_pUI, "PDS_IP_6", DEFAULT_PDS_IP_6, 120, "", true);
    addTextInput(_pUI, "PDS_IP_1", DEFAULT_PDS_IP_1, 120);
    addTextInput(_pUI, "PDS_IP_7", DEFAULT_PDS_IP_7, 120, "", true);
    addTextInput(_pUI, "PDS_IP_2", DEFAULT_PDS_IP_2, 120);
    addTextInput(_pUI, "PDS_IP_8", DEFAULT_PDS_IP_8, 120, "", true);
    addTextInput(_pUI, "PDS_IP_3", DEFAULT_PDS_IP_3, 120);
    addTextInput(_pUI, "PDS_IP_9", DEFAULT_PDS_IP_9, 120, "", true);
    addTextInput(_pUI, "PDS_IP_4", DEFAULT_PDS_IP_4, 120);
    addTextInput(_pUI, "PDS_IP_10", DEFAULT_PDS_IP_10, 120, "", true);
    addTextInput(_pUI, "PDS_IP_5", DEFAULT_PDS_IP_5, 120);
    addTextInput(_pUI, "PDS_IP_11", DEFAULT_PDS_IP_11, 120, "", true);

    _pUI->addSpacer(1,12)->setDrawFill(false);

    _pUI->addWidgetDown(new ofxUILabelButton("+ PT", false));
    _pUI->addWidgetRight(new ofxUILabelButton("+ TILE", false));
    _pUI->addWidgetRight(new ofxUILabelButton("+ STRIP", false));
    _pUI->addWidgetRight(new ofxUILabelButton("DELETE", false));
    
    _pUI->addSpacer(1,12)->setDrawFill(false);
    
    _lastStaticWidget = _pUI->addWidgetDown(new ofxUILabel("SELECTED FIXTURE", OFX_UI_FONT_SMALL));
    
    ofAddListener(_pUI->newGUIEvent, this, &ckvdApp::guiEvent);
   
    _pGrabberFont = _pUI->getFontSmall();

	mClient.initialize("","");

	loadSettings();
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
    ofDisableAlphaBlending();
    ofBackground(0,0,0,1.0);

	mClient.maybeBind();
    mClient.getTexture().draw(0, 0);
    
    if (_pUI)
    {
        ofSetColor(60,60,80);
        ofRect(getClientWidth(), 0, SIDEBAR_WIDTH, getHeight());
        ofSetColor(255,255,255);
        
        bool bVisible = _pSelectedGrabber != NULL;
        _pUI->getWidget("SELECTED FIXTURE")->setVisible(bVisible);
        _pUI->getWidget("DELETE")->setVisible(bVisible);
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
        {
            auto fixture = (*iter)->getFixture();
            
            // hack: switch to new protocol if supply contains a tile
            //if (dynamic_cast<FixtureTile*>(fixture) != NULL)
            //    _supplies[nSupply]->switchToNewProtocol();
            
            _supplies[nSupply]->addFixture(fixture);
        }
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
    for (auto it = _contextWidgets.begin(); it != _contextWidgets.end(); ++it)
    {
        _pUI->removeWidget(*it);
    }
    _contextWidgets.clear();
    _pUI->setPlacer(_lastStaticWidget);
 
    _pSelectedGrabber = pGrabber;
    
    if (!pGrabber)
        return;
    
    vector<string> params;
    _pSelectedGrabber->listParams(&params);
    
    for (auto it = params.begin(); it != params.end(); ++it)
    {
        std::stringstream s;
        if (*it == "X" || *it == "Y")
            continue;
        
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
        setSelectedGrabber(NULL);
        delete _pSelectedGrabber;
    }
}

void ckvdApp::exit()
{
    //_pUI->saveSettings("GUI/guiSettings.xml");
    delete _pUI;
}

namespace
{
    int updateWidgetParameterInt(const string& paramName, ofxUITextInput* pInput, ckvdVideoGrabber* pGrabber)
    {
        if (pInput && pInput->getTextString() != "" && pGrabber)
        {
            int addr;
            std::istringstream(pInput->getTextString()) >> addr;
            pGrabber->setParameterInt(paramName, addr);
            
            std::ostringstream validatedAddr;
            validatedAddr << pGrabber->getParameterInt(paramName);
            
            pInput->setTextString(validatedAddr.str());
            
            return addr;
        }
    }
}

void ckvdApp::updateSyphonApp(const string& app)
{
    auto widget = (ofxUITextInput*)_pUI->getWidget("SYPHON_APP");
    if (widget)
        widget->setTextString(app);
    
    mClient.setApplicationName(app);
}

void ckvdApp::updateSyphonServer(const string& server)
{
    auto widget = (ofxUITextInput*)_pUI->getWidget("SYPHON_SERVER");
    if (widget)
        widget->setTextString(server);
    
    mClient.setServerName(server);
}


void ckvdApp::updatePowerSupplyAddress(int index, const string& address)
{
    std::ostringstream widgetName;
    widgetName << "PDS_IP_" << index;
    auto widget = (ofxUITextInput*)_pUI->getWidget(widgetName.str());
    if (widget)
        widget->setTextString(address);
    
    if (address != "" && index >= 0 && index < _supplies.size())
    {
        delete _supplies[index];
        _supplies[index] = new PowerSupply(address.c_str());
    }
}

static int g_lastScale = 8;

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
            _grabbers.push_back(new ckvdTileGrabber(g_lastScale));
    }
    if(e.widget->getName() == "+ STRIP")
    {
        ofxUIButton* pButton = (ofxUIButton*)e.widget;
        if (pButton && pButton->getValue())
            _grabbers.push_back(new ckvdStripGrabber());
    }
	if(e.widget->getName() == "DELETE")
    {
        ofxUIButton* pButton = (ofxUIButton*)e.widget;
        if (pButton && pButton->getValue())
        {
            deleteSelected();
        }
    }
    else if(e.widget->getName() == "PDS_IP_0")
    {
        ofxUITextInput* pInput = (ofxUITextInput*)e.widget;
        if (pInput)
            updatePowerSupplyAddress(0, pInput->getTextString());
    }
    else if(e.widget->getName() == "PDS_IP_1")
    {
        ofxUITextInput* pInput = (ofxUITextInput*)e.widget;
        if (pInput)
            updatePowerSupplyAddress(1, pInput->getTextString());
    }
    else if(e.widget->getName() == "PDS_IP_2")
    {
        ofxUITextInput* pInput = (ofxUITextInput*)e.widget;
        if (pInput)
            updatePowerSupplyAddress(2, pInput->getTextString());
    }
    else if(e.widget->getName() == "PDS_IP_3")
    {
        ofxUITextInput* pInput = (ofxUITextInput*)e.widget;
        if (pInput)
            updatePowerSupplyAddress(3, pInput->getTextString());
    }
    else if(e.widget->getName() == "PDS_IP_4")
    {
        ofxUITextInput* pInput = (ofxUITextInput*)e.widget;
        if (pInput)
            updatePowerSupplyAddress(4, pInput->getTextString());
    }
    else if(e.widget->getName() == "PDS_IP_5")
    {
        ofxUITextInput* pInput = (ofxUITextInput*)e.widget;
        if (pInput)
            updatePowerSupplyAddress(5, pInput->getTextString());
    }
    else if(e.widget->getName() == "PDS_IP_6")
    {
        ofxUITextInput* pInput = (ofxUITextInput*)e.widget;
        if (pInput)
            updatePowerSupplyAddress(6, pInput->getTextString());
    }
    else if(e.widget->getName() == "PDS_IP_7")
    {
        ofxUITextInput* pInput = (ofxUITextInput*)e.widget;
        if (pInput)
            updatePowerSupplyAddress(7, pInput->getTextString());
    }
    else if(e.widget->getName() == "PDS_IP_8")
    {
        ofxUITextInput* pInput = (ofxUITextInput*)e.widget;
        if (pInput)
            updatePowerSupplyAddress(8, pInput->getTextString());
    }
    else if(e.widget->getName() == "PDS_IP_9")
    {
        ofxUITextInput* pInput = (ofxUITextInput*)e.widget;
        if (pInput)
            updatePowerSupplyAddress(9, pInput->getTextString());
    }
    else if(e.widget->getName() == "PDS_IP_10")
    {
        ofxUITextInput* pInput = (ofxUITextInput*)e.widget;
        if (pInput)
            updatePowerSupplyAddress(10, pInput->getTextString());
    }
    else if(e.widget->getName() == "PDS_IP_11")
    {
        ofxUITextInput* pInput = (ofxUITextInput*)e.widget;
        if (pInput)
            updatePowerSupplyAddress(11, pInput->getTextString());
    }
    else if (e.widget->getName() == "ADDRESS" ||
             e.widget->getName() == "CHANNEL" ||
             e.widget->getName() == "SCALE" ||
             e.widget->getName() == "SUPPLY" ||
             e.widget->getName() == "ROTATION" ||
             e.widget->getName() == "X" ||
             e.widget->getName() == "Y"
             )
    {
        int result = updateWidgetParameterInt(e.widget->getName(), (ofxUITextInput*)e.widget, _pSelectedGrabber);

        if (e.widget->getName() == "SCALE")
            g_lastScale = result;
    }
    else if(e.widget->getName() == "SYPHON_APP")
    {
        ofxUITextInput* pInput = (ofxUITextInput*)e.widget;
        if (pInput)
        {
            updateSyphonApp(pInput->getTextString());
        }
    }
    else if(e.widget->getName() == "SYPHON_SERVER")
    {
        ofxUITextInput* pInput = (ofxUITextInput*)e.widget;
        if (pInput)
        {
            updateSyphonServer(pInput->getTextString());
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
    else if (e.widget->getName() == "LOAD")
    {
       loadSettings();
    }
    else if (e.widget->getName() == "SAVE")
    {
        ofxXmlSettings settings;
        settings.addTag("settings");
        settings.pushTag("settings");
        
        settings.setValue("syphon:app",    mClient.getApplicationName());
        settings.setValue("syphon:server", mClient.getServerName());
        //settings.setValue("framerate", ofGetFrameRate());
        
        settings.addTag("supplies");
        settings.pushTag("supplies");
        for (int ii = 0; ii < _supplies.size(); ii++)
        {
            settings.addTag("supply");
            settings.pushTag("supply", ii);
            settings.setValue("host", _supplies[ii]->getHost());
            settings.popTag();
        }
        settings.popTag();
        
        settings.addTag("grabbers");
        settings.pushTag("grabbers");
        for (int ii = 0; ii < _grabbers.size(); ii++)
        {
            auto pGrabber = _grabbers[ii];
            vector<string> params;
            
            pGrabber->listParams(&params);
            
            settings.addTag("grabber");
            settings.pushTag("grabber", ii);
            settings.setValue("type", pGrabber->type());
            settings.addTag("parameters");
            settings.pushTag("parameters");
            
            for (int jj = 0; jj < params.size(); jj++)
            {
                settings.addTag("param");
                settings.pushTag("param", jj);
                settings.setValue("name", params[jj]);
                settings.setValue("value", pGrabber->getParameterInt(params[jj]));
                settings.popTag();
            }
            settings.popTag();
            settings.popTag();
        }
        settings.popTag();
        
        settings.saveFile("settings.xml");
    }
}



