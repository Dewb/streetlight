//
//  kinet.cpp
//
//  Created by Michael Dewberry on 12/19/12.
//
//  Based on kinet.py by Giles Hall
//  http://github.com/vishnubob/kinet
//

#include "kinet.h"
#include <sys/socket.h>
#include <netdb.h>
#include <sstream>
#include <iostream>
#include <math.h>
#include <fcntl.h>

#define ENABLE_LOGGING 0

const unsigned char oldHeaderBytes[] = {
    0x04, 0x01, 0xdc, 0x4a, 0x01, 0x00, 0x01, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0x00
};

const KinetProtocol oldProtocol(21, 512, 1, oldHeaderBytes);

const unsigned char newHeaderBytes[] = {
    0x04, 0x01, 0xdc, 0x4a, 0x01, 0x00, 0x08, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x01, 0xFF, 0x00, 0xFF, 0x0F
};

const KinetProtocol newProtocol(24, 512, 16, newHeaderBytes);


void dump_buffer(unsigned n, const uint8_t* buf)
{
    int c = 1;
    while (c < n)
    {
        fprintf(stderr, "%02X ", *buf++);
        if (c%16 == 0)
            fprintf(stderr, "\n");
        c++;
    }
    if (c%16 != 1)
        fprintf(stderr, "\n");
}

PowerSupply::PowerSupply()
{
    _connected = false;
    _frame = NULL;
    _proto = &oldProtocol;
    initializeBuffer();
}

PowerSupply::PowerSupply(const string strHost, const string strPort)
{    
    _connected = false;
    _frame = NULL;
    _proto = &oldProtocol;
    initializeBuffer();
    connect(strHost, strPort);
}

void PowerSupply::switchToNewProtocol()
{
    if (_proto != &newProtocol)
    {
        _proto = &newProtocol;
        initializeBuffer();
    }
}

void PowerSupply::initializeBuffer()
{
    if (_frame)
        free(_frame);
        
    _frame = (uint8_t*)malloc(_proto->getBufferSize() * sizeof(uint8_t));
    memset(_frame, _proto->getBufferSize(), 0);
    for (int c = 0; c < _proto->getNumChannels(); c++)
    {
        memcpy(_frame + c * _proto->getPacketSize(), _proto->getHeaderBytes(), _proto->getHeaderSize());
        if (_proto->getNumChannels() > 1)
            _frame[c * _proto->getPacketSize() + 16] = c + 1;
    }
}

bool PowerSupply::connect(const string strHost, const string strPort)
{
    struct addrinfo hints;
    struct addrinfo *pResult, *pr;
    int result;
    int sock;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    
    result = getaddrinfo(strHost.c_str(), strPort.c_str(), &hints, &pResult);
    if (result != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(result));
        return false;
    }

    for (pr = pResult; pr != NULL; pr = pr->ai_next) {
        sock = socket(pr->ai_family, pr->ai_socktype, pr->ai_protocol);
        if (sock == -1)
            continue;
        
        if (::connect(sock, pr->ai_addr, pr->ai_addrlen) == 0)
            break;
        
        close(sock);
    }

    if (pr == NULL)
    {
        fprintf(stderr, "Could not connect to socket\n");
        return false;
    }
    
    _host = strHost;
    _port = ((sockaddr_in*)(pr->ai_addr))->sin_port;
    
    freeaddrinfo(pResult);
    
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);
    
    _socket = sock;
    _connected = true;
    
    return true;
}

PowerSupply::~PowerSupply()
{
    free(_frame);
}

string PowerSupply::getHost()
{
    return _host;
}

int PowerSupply::getPort()
{
    return _port;
}

void PowerSupply::addFixture(Fixture* pFix)
{
    _fixtures.push_back(pFix);
}

void PowerSupply::clearFixtures()
{
    _fixtures.clear();
}

void PowerSupply::go()
{
    if (!_connected)
    {
        fprintf(stderr, "Power supply is not connected.\n");
        return;
    }
    
    for (auto fix = _fixtures.begin(); fix != _fixtures.end(); fix++)
    {
        (*fix)->updateFrame(_frame);
    }
    
    for (int channel = 0; channel < _proto->getNumChannels(); channel++)
    {
        send(_socket, _frame + _proto->getPacketSize() * channel, _proto->getPacketSize(), 0);
        
#if ENABLE_LOGGING
        std::cout << "Channel " << channel+1 << "\n";
        dump_buffer(_proto->getHeaderSize() + 3, _frame + _proto->getPacketSize() * channel);
#endif
    }
}

FixtureRGB::FixtureRGB(int address, uint8_t r, uint8_t g, uint8_t b)
: Fixture()
, _address(address)
{
    _values[0] = r;
    _values[1] = g;
    _values[2] = b;
}

void FixtureRGB::updateFrame(uint8_t* packets) const
{
    memcpy(packets + oldProtocol.getHeaderSize() + _address, _values, 3);
}

std::string FixtureRGB::getName() const
{
    std::ostringstream out;
    out << getAddress();
    return out.str();
}

uint8_t FixtureRGB::get_red() const
{
    return _values[0];
}

uint8_t FixtureRGB::get_green() const
{
    return _values[1];
}

uint8_t FixtureRGB::get_blue() const
{
    return _values[2];
}

void FixtureRGB::set_red(uint8_t r)
{
    _values[0] = r;
}

void FixtureRGB::set_green(uint8_t g)
{
    _values[1] = g;
}

void FixtureRGB::set_blue(uint8_t b)
{
    _values[2] = b;
}

void FixtureRGB::set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    set_red(r);
    set_green(g);
    set_blue(b);
}

FixtureTile::FixtureTile(int startChannel, int width, int height)
: _startChannel(startChannel)
, _fixtureWidth(width)
, _fixtureHeight(height)
, _videoX(0)
, _videoY(0)
, _videoW(width)
, _videoH(height)
, _sourceData(NULL)
{
}

void FixtureTile::setVideoRect(int x, int y, int w, int h)
{
    _videoX = x;
    _videoY = y;
    _videoW = w;
    _videoH = h;
}

void FixtureTile::setSourceData(const uint8_t* sourceData, int sourceWidth, int sourceHeight, int sourceChannels)
{
    _sourceData = sourceData;
    _sourceWidth = sourceWidth;
    _sourceHeight = sourceHeight;
    _sourceChannels = sourceChannels;
}

void FixtureTile::updateFrame(uint8_t* packets) const
{
    if (_sourceData == NULL)
        return;
    
    int tileX, tileY;
    uint8_t* pIndex;
    
    tileX = _fixtureWidth-1;
    tileY = _fixtureHeight/2-1;
    pIndex = packets + newProtocol.getPacketSize() * (_startChannel-1) + newProtocol.getHeaderSize();
    
    int xscale = (int)floor(_videoW/(_fixtureWidth*1.0));
    int yscale = (int)floor(_videoH/(_fixtureHeight*1.0));

    while (tileY < _fixtureHeight)
    {
#if ENABLE_LOGGING
        std::cout << "Writing x: " << tileX << " y: " << tileY << " to address " << (void*)pIndex << "\n";
#endif
        int scale = 8;
        memcpy(pIndex, _sourceData + (_videoX + tileX*xscale + (_videoY + tileY*yscale) * _sourceWidth) * _sourceChannels, 3);
        pIndex+=3;
        tileX--;

        if (tileX < 0)
        {
            tileX =  _fixtureWidth-1;
            if (tileY == 0) {
                tileY = _fixtureHeight/2;
                pIndex = packets + newProtocol.getPacketSize() * (_startChannel) + newProtocol.getHeaderSize();
            } else if (tileY < _fixtureHeight/2) {
                tileY--;
            } else {
                tileY++;
            }
        }
    }
}

FixtureTile6::FixtureTile6(int startChannel, int width, int height)
: FixtureTile(startChannel, width, height)
{
}

void FixtureTile6::updateFrame(uint8_t* packets) const
{
    if (_sourceData == NULL)
        return;
    
    int tileX, tileY, startx, endx;
    uint8_t* pIndex;
    
    startx = 0;
    endx = _fixtureWidth/2-1;
    tileX = startx;
    tileY = _fixtureHeight-1;
    pIndex = packets + newProtocol.getPacketSize() * (_startChannel-1) + newProtocol.getHeaderSize();
    
    int xscale = (int)floor(_videoW/(_fixtureWidth*1.0));
    int yscale = (int)floor(_videoH/(_fixtureHeight*1.0));
    
    while(1)
    {
#if ENABLE_LOGGING
        std::cout << "Writing x: " << tileX << " y: " << tileY << " to address " << (void*)pIndex << "\n";
#endif
        int scale = 8;
        memcpy(pIndex, _sourceData + (_videoX + tileX*xscale + (_videoY + tileY*yscale) * _sourceWidth) * _sourceChannels, 3);
        pIndex+=3;
        tileX++;
        
        if (tileX > endx)
        {
            tileY--;
            if (tileY < 0) {
                if (endx == _fixtureWidth - 1)
                    break;
                startx = endx + 1;
                endx = _fixtureWidth - 1;
                tileY = _fixtureHeight - 1;
                pIndex = packets + newProtocol.getPacketSize() * (_startChannel) + newProtocol.getHeaderSize();
            }
            tileX = startx;
        }
    }
    
}

std::string FixtureTile::getName() const
{
    std::ostringstream out;
    out << "C" << _startChannel << "/" << (_startChannel + 1);
    return out.str();
}

