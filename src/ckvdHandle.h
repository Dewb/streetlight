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
    virtual void listParams(vector<string>* pParams);
    virtual int getParameterInt(const string& name) const;
    virtual bool setParameterInt(const string& name, int val);
    virtual string type() const = 0;
};

class ckvdVideoGrabber : public ckvdHandle
{
public:
    ckvdVideoGrabber() { _nSupply = 1; }
    virtual void mousePressed(ofMouseEventArgs &e);
    virtual void moveBy(float dx, float dy);
    
    virtual void setColorFromFrame(ofImage& frame) = 0;
    virtual Fixture* getFixture() = 0;
    
    int getSupplyNumber() const { return _nSupply; }
    void setSupplyNumber(int n) { _nSupply = n; }

protected:
    int _nSupply;
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
    virtual bool setParameterInt(const string& name, int val);
    
    virtual string type() const { return "point"; }
    
protected:
    FixtureRGB _fixture;
    ofColor _color;
};

class ckvdStripGrabber : public ckvdVideoGrabber
{
public:
    ckvdStripGrabber(int length = 50);
    virtual void draw();

    virtual void setColorFromFrame(ofImage& frame);
    virtual Fixture* getFixture() { return &_fixture; }

    virtual void listParams(vector<string>* pParams);
    virtual int getParameterInt(const string& name) const;
    virtual bool setParameterInt(const string& name, int val);

    virtual string type() const { return "strip"; }

protected:
    FixtureStrip _fixture;
};

class ckvdTileGrabber : public ckvdVideoGrabber
{
public:
    ckvdTileGrabber(int scale = 8);
    virtual void draw();
    
    virtual void setColorFromFrame(ofImage& frame);
    virtual Fixture* getFixture() { return &_fixture; }
    
    virtual void listParams(vector<string>* pParams);
    virtual int getParameterInt(const string& name) const;
    virtual bool setParameterInt(const string& name, int val);
    
    virtual string type() const { return "tile"; }
    
protected:
    // Uncomment this line for iColorTile MX 12x12
    FixtureTile _fixture;
    // Uncomment this line for iColorModule FX 6x6
    //FixtureTile6 _fixture;
    // Uncomment this for hybrid DC tiles
    //FixtureTileDC _fixture;
    int _scale;
};




#endif /* defined(__CKVideoDriver__ckvdWidget__) */
