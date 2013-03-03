//
//  kinet.h
//
//  Created by Michael Dewberry on 12/19/12.
//
//  Port of kinet.py by Giles Hall
//  http://github.com/vishnubob/kinet
//

#include <list>
#include <string>

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

class FixtureTile : public Fixture
{
public:
    FixtureTile(int startChannel, int width = 12, int height = 12);
    int getChannel() const { return _startChannel; }
    void setChannel(int channel) { _startChannel = channel; }
    void setVideoRect(int x, int y, int w, int h);
    void setSourceData(const uint8_t* sourceData, int sourceWidth, int sourceHeight, int sourceChannels);

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
};

class PowerSupply
{
public:
    PowerSupply(const char* strHost, const char* strPort = "6038");
    ~PowerSupply();
    
    char* getHost();
    int getPort();
    
    void addFixture(Fixture* pFix);
    void clearFixtures();
    
    void go();
        
protected:
    bool _connected;
    int _socket;
    std::list<Fixture*> _fixtures;
    uint8_t* _frame;
    char* _host;
    int _port;
};