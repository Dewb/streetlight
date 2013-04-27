//
//  ckvdHandle.h
//  CKVideoDriver
//
//  Created by Michael Dewberry on 3/6/13.
//
//

#ifndef __CKVideoDriver__ckvdWidget__
#define __CKVideoDriver__ckvdWidget__

#include "ofxTangibleHandle.h"
#include "kinet.h"

class ckvdHandle : public ofxTangibleHandle
{
public:
    bool isFocused();
    virtual void listParams(vector<string>* pParams) {}
    virtual int getParameterInt(const string& name) const { return -1; }
    virtual void setParameterInt(const string& name, int val) {}
};

class ckvdVideoGrabber : public ckvdHandle
{
public:
    ckvdVideoGrabber() {}
    virtual void mousePressed(ofMouseEventArgs &e);
    virtual void moveBy(float dx, float dy);
    
    virtual void setColorFromFrame(ofImage& frame) = 0;
    virtual Fixture* getFixture() = 0;
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




#endif /* defined(__CKVideoDriver__ckvdWidget__) */
