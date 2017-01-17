//
//  ckvdHandle.cpp
//  CKVideoDriver
//
//  Created by Michael Dewberry on 3/6/13.
//
//

#include "ckvdApp.h"
#include "ckvdHandle.h"

#define pi 3.141592

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

bool ckvdHandle::isFocused()
{
    //return theApp()->isHandleSelected(getId());
    return theApp()->getSelectedGrabber() == this;
}

void ckvdHandle::listParams(vector<string>* pParams)
{
    if (pParams)
    {
        pParams->push_back("X");
        pParams->push_back("Y");
    }
}

int ckvdHandle::getParameterInt(const string& name) const
{
    if (name.compare("X") == 0)
        return x;
    if (name.compare("Y") == 0)
        return y;
    return -1;
}

bool ckvdHandle::setParameterInt(const string& name, int val)
{
    if (name.compare("X") == 0) {
        x = val;
        return true;
    }
    if (name.compare("Y") == 0) {
        y = val;
        return true;
    }
    return false;
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
    
    drawType = TANGIBLE_DRAW_AS_CENTERED_RECT;
    
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
    ckvdHandle::listParams(pParams);

    if (pParams)
    {
        pParams->push_back("SUPPLY");
        pParams->push_back("ADDRESS");
    }
}

int ckvdSingleColorGrabber::getParameterInt(const string& name) const
{
    int v = ckvdHandle::getParameterInt(name);
    if (v >= 0)
        return v;
    
    if (name.compare("ADDRESS") == 0)
        return _fixture.getAddress();
    if (name.compare("SUPPLY") == 0)
        return getSupplyNumber();
    return -1;
}

bool ckvdSingleColorGrabber::setParameterInt(const string& name, int val)
{
    if (ckvdHandle::setParameterInt(name, val))
        return true;
        
    if (name.compare("ADDRESS") == 0) {
        _fixture.setAddress(val);
        return true;
    }
    if (name.compare("SUPPLY") == 0) {
        setSupplyNumber(val);
        return true;
    }
    return false;
}


ckvdStripGrabber::ckvdStripGrabber(int length)
: _fixture(0, length)
{
    static int count = 0;

    drawType = TANGIBLE_DRAW_AS_CENTERED_RECT;

    int cx = 20+96*((count)%5+1);
    int cy = 30+96*((count/5)+1);

    setup(cx, cy, 20, 20);

    count++;
}

void ckvdStripGrabber::draw()
{
    ofPushStyle();

    ofNoFill();
    ofSetLineWidth(1);
    ofSetRectMode(OF_RECTMODE_CENTER);

    if (isFocused())
    {
        ofSetColor(255, 255, 100);
        ofSetLineWidth(5);
        ofRect(x, y, width, height);
        ofSetColor(60, 60, 60);
        ofSetLineWidth(1);
        ofRect(x, y, width, height);
        ofSetRectMode(OF_RECTMODE_CORNER);
        ofSetColor(255, 255, 100);
        ofRect(x - 1, y - 1, _fixture.getLength() + 1, 3);
        ofSetColor(255, 255, 100);
    }
    else
    {
        ofSetColor(255, 255, 255);
        ofRect(x, y, width, height);
        ofSetRectMode(OF_RECTMODE_CORNER);
        ofRect(x - 1, y - 1, _fixture.getLength() + 1, 3);
    }

    if (theApp()->getGrabberFont())
    {
        theApp()->getGrabberFont()->drawString(_fixture.getName(), x+width/2+3, y-height/2+3);
    }

    ofPopStyle();
}

void ckvdStripGrabber::setColorFromFrame(ofImage& frame)
{
    _fixture.set_data(frame.getPixels() + 3 * (int(x) + frame.width * int(y)));
}

void ckvdStripGrabber::listParams(vector<string>* pParams)
{
    ckvdHandle::listParams(pParams);

    if (pParams)
    {
        pParams->push_back("SUPPLY");
        pParams->push_back("ADDRESS");
        pParams->push_back("LENGTH");
    }
}

int ckvdStripGrabber::getParameterInt(const string& name) const
{
    int v = ckvdHandle::getParameterInt(name);
    if (v >= 0)
        return v;

    if (name.compare("ADDRESS") == 0)
        return _fixture.getStartAddress();
    if (name.compare("SUPPLY") == 0)
        return getSupplyNumber();
    if (name.compare("LENGTH") == 0)
        return _fixture.getLength();
    return -1;
}

bool ckvdStripGrabber::setParameterInt(const string& name, int val)
{
    if (ckvdHandle::setParameterInt(name, val))
        return true;

    if (name.compare("ADDRESS") == 0) {
        _fixture.setStartAddress(val);
        return true;
    }
    if (name.compare("SUPPLY") == 0) {
        setSupplyNumber(val);
        return true;
    }
    if (name.compare("LENGTH") == 0) {
        _fixture.setLength(val);
        return true;
    }
    return false;
}



ckvdTileGrabber::ckvdTileGrabber(int scale)
: _fixture(0)
, _scale(scale)
{
    static int count = 0;
    
    drawType = TANGIBLE_DRAW_AS_CENTERED_RECT;
    
    int cx = 20+96*((count)%5+1);
    int cy = 30+96*((count/5)+1);
    
    setup(cx, cy, 12*_scale, 12*_scale);
    
    int channel = count*2;
    int supply = channel / 16;
    _fixture.setChannel((channel%16) + 1);
    _nSupply = supply%4 + 1;
    count++;
}

void ckvdTileGrabber::draw()
{
    ofPushStyle();
    
    if (isFocused())
    {
        ofSetColor(255, 255, 100);
        drawCornerTickBox(x - width/2, y - height/2, width, height, 5);
        ofSetColor(60, 60, 60);
        drawCornerTickBox(x - width/2, y - height/2, width, height, 1);
        ofSetColor(255, 255, 100);
    }
    else
    {
        ofSetColor(255, 255, 255);
        drawCornerTickBox(x - width/2, y - height/2, width, height, 1);
    }

    int r = (width + height) / 5;
    int d = 10;
    float c = cos((270 + _fixture.getRotation()) * pi/180.0);
    float s = sin((270 + _fixture.getRotation()) * pi/180.0);
    ofLine((int)floor(x + r * c), int(floor(y + r * s)), int(floor(x + (r + d) * c)), int(floor(y + (r + d) * s)));
    
    if (theApp()->getGrabberFont())
    {
        ostringstream ss;
        ss << "#" << getParameterInt("SUPPLY") << " " << _fixture.getName();
        ofTrueTypeFont* pFont = theApp()->getGrabberFont();
        pFont->drawString(ss.str(), x - pFont->stringWidth(ss.str())/2, y + pFont->stringHeight(ss.str())/2);
    }
    
    ofPopStyle();
}

void ckvdTileGrabber::setColorFromFrame(ofImage& frame)
{
    _fixture.setVideoRect(x - width/2, y - height/2, width, height);
    _fixture.setSourceData(frame.getPixels(), frame.width, frame.height, 3);
}

void ckvdTileGrabber::listParams(vector<string>* pParams)
{
    ckvdHandle::listParams(pParams);

    if (pParams)
    {
        pParams->push_back("SUPPLY");
        pParams->push_back("CHANNEL");
        pParams->push_back("SCALE");
        pParams->push_back("ROTATION");
    }
}

int ckvdTileGrabber::getParameterInt(const string& name) const
{
    int v = ckvdHandle::getParameterInt(name);
    if (v >= 0)
        return v;
    
    if (name.compare("CHANNEL") == 0)
        return _fixture.getChannel();
    if (name.compare("SCALE") == 0)
        return _scale;
    if (name.compare("SUPPLY") == 0)
        return getSupplyNumber();
    if (name.compare("ROTATION") == 0)
        return _fixture.getRotation();
    return -1;
}

bool ckvdTileGrabber::setParameterInt(const string& name, int val)
{
    if (ckvdHandle::setParameterInt(name, val))
        return true;
    
    if (name.compare("CHANNEL") == 0 && val >= 1 && val <= 15) {
        _fixture.setChannel(val);
        return true;
    }
    if (name.compare("SCALE") == 0)
    {
        _scale = val;
        width = 12*_scale;
        height = 12*_scale;
        return true;
    }
    if (name.compare("SUPPLY") == 0)
    {
        setSupplyNumber(val);
        return true;
    }
    if (name.compare("ROTATION") == 0)
    {
        _fixture.setRotation(val);
    }
    return false;
}
