//
//  ckvdHandle.cpp
//  CKVideoDriver
//
//  Created by Michael Dewberry on 3/6/13.
//
//

#include "ckvdApp.h"
#include "ckvdHandle.h"

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
        pParams->push_back("SUPPLY");
        pParams->push_back("ADDRESS");
    }
}

int ckvdSingleColorGrabber::getParameterInt(const string& name) const
{
    if (name.compare("ADDRESS") == 0)
        return _fixture.getAddress();
    if (name.compare("SUPPLY") == 0)
        return getSupplyNumber();
    return -1;
}

void ckvdSingleColorGrabber::setParameterInt(const string& name, int val)
{
    if (name.compare("ADDRESS") == 0)
        _fixture.setAddress(val);
    if (name.compare("SUPPLY") == 0)
        setSupplyNumber(val);
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
        pParams->push_back("SUPPLY");
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
    if (name.compare("SUPPLY") == 0)
        return getSupplyNumber();
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
    if (name.compare("SUPPLY") == 0)
        setSupplyNumber(val);
}
