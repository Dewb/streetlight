//
//  kinet.h
//
//  Created by Michael Dewberry on 12/19/12.
//
//  Port of kinet.py by Giles Hall
//  http://github.com/vishnubob/kinet
//

#ifndef _KINET_H
#define _KINET_H

#include <list>
#include <string>
#include <stdint.h>

using std::list;
using std::string;


// Just a data container for now. Todo: move all protocol logic into this class
class KinetProtocol
{
public:
    KinetProtocol(size_t headerSize, size_t dataSize, size_t numChannels, const unsigned char* headerBytes)
    : _headerSize(headerSize), _dataSize(dataSize), _numChannels(numChannels), _headerBytes(headerBytes)
    {
    }

    size_t getHeaderSize() const { return _headerSize; }
    size_t getDataSize() const { return _dataSize; }
    size_t getNumChannels() const { return _numChannels; }
    size_t getPacketSize() const { return _headerSize + _dataSize; }
    size_t getBufferSize() const { return getPacketSize() * _numChannels; }
    const unsigned char* getHeaderBytes() const { return _headerBytes; }

protected:
    size_t _headerSize;
    size_t _dataSize;
    size_t _numChannels;
    const unsigned char* _headerBytes;
};

class Fixture
{
public:
    virtual void updateFrame(uint8_t* packets) const = 0;
    virtual std::string getName() const = 0;
};

class FixtureRGB : public Fixture
{
public:
    FixtureRGB(int address, uint8_t r=0, uint8_t g=0, uint8_t b=0);
    int getAddress() const { return _address; }
    void setAddress(int address) { _address = address; }
   
    virtual void updateFrame(uint8_t* packets) const;
    virtual std::string getName() const;
    
    unsigned char get_red() const;
    unsigned char get_green() const;
    unsigned char get_blue() const;
    
    void set_red(uint8_t r);
    void set_green(uint8_t g);
    void set_blue(uint8_t b);
    
    void set_rgb(uint8_t r, uint8_t g, uint8_t b);

protected:
    int _address;
    unsigned char _values[3];
};

class FixtureStrip : public Fixture
{
public:
    FixtureStrip(int startAddress, int length);

    int getStartAddress() const { return _address; }
    void setStartAddress(int address) { _address = address; }

    int getLength() const { return _length; }
    void setLength(int length) { _length = length; }

    virtual void updateFrame(uint8_t* packets) const;
    virtual std::string getName() const;

    void set_data(uint8_t* bytes) { _bytes = bytes; }

protected:
    int _address;
    int _length;
    uint8_t* _bytes;
};

class FixtureTile : public Fixture
{
public:
    FixtureTile(int startChannel, int width = 12, int height = 12);
    int getChannel() const { return _startChannel; }
    void setChannel(int channel) { _startChannel = channel; }
    void setVideoRect(int x, int y, int w, int h);
    void setSourceData(const uint8_t* sourceData, int sourceWidth, int sourceHeight, int sourceChannels);
    int getRotation() const { return _rotation; }
    void setRotation(int degrees) { _rotation = degrees; }

    virtual void updateFrame(uint8_t* packets) const;
    virtual std::string getName() const;

protected:
    int _startChannel;
    int _fixtureWidth;
    int _fixtureHeight;
    int _videoX;
    int _videoY;
    int _videoW;
    int _videoH;
    const uint8_t* _sourceData;
    int _sourceWidth;
    int _sourceHeight;
    int _sourceChannels;
    int _rotation;
};

class FixtureTile6 : public FixtureTile
{
public:
    FixtureTile6(int startChannel, int width = 12, int height = 12);
    virtual void updateFrame(uint8_t* packets) const;
};

class FixtureTileDC : public FixtureTile
{
public:
    FixtureTileDC(int startChannel, int width = 12, int height = 12);
    virtual void updateFrame(uint8_t* packets) const;
};

class PowerSupply
{
public:
    PowerSupply();
    PowerSupply(const string strHost, const string strPort = "6038");
    ~PowerSupply();
    
    bool connect(const string strHost, const string strPort = "6038");
    
    string getHost();
    int getPort();
    
    void addFixture(Fixture* pFix);
    void clearFixtures();
    int getFixtureCount() const { return _fixtures.size(); }
    
    void go();
    
    void switchToNewProtocol();
        
protected:
    void initializeBuffer();
    
    bool _connected;
    int _socket;
    list<Fixture*> _fixtures;
    uint8_t* _frame;
    string _host;
    int _port;
    const KinetProtocol* _proto;
};

#endif