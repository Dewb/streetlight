//
//  kinet.cpp
//
//  Created by Michael Dewberry on 12/19/12.
//
//  Port of kinet.py by Giles Hall
//  http://github.com/vishnubob/kinet
//

#include "kinet.h"
#include <sys/socket.h>
#include <netdb.h>

#define OLD_PROTOCOL 1

#ifdef OLD_PROTOCOL

#define DATA_SIZE 512
#define HEADER_SIZE 21

const unsigned char ck_header_bytes[] = { 0x04, 0x01, 0xdc, 0x4a, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
                                          0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00};
#else

#define DATA_SIZE 512
#define HEADER_SIZE 24
const unsigned char ck_header_bytes[] = { 0x04, 0x01, 0xdc, 0x4a, 0x01, 0x00, 0x08, 0x01,
                                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                          0x04, 0x00, 0x00, 0x01, 0xFF, 0x00, 0xFF, 0x0F};
#endif


void dump_buffer(unsigned n, const unsigned char* buf)
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

PowerSupply::PowerSupply(const char* strHost, const char* strPort)
{
    struct addrinfo hints;
    struct addrinfo *pResult, *pr;
    int result;
    int sock;
    
    _connected = false;
    _frame = NULL;
    
    _host = (char*)malloc((strlen(strHost)+1)*sizeof(char));
    strncpy(_host, strHost, strlen(strHost));
    
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    
    result = getaddrinfo(strHost, strPort, &hints, &pResult);
    if (result != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(result));
        return;
    }

    for (pr = pResult; pr != NULL; pr = pr->ai_next) {
        sock = socket(pr->ai_family, pr->ai_socktype, pr->ai_protocol);
        if (sock == -1)
            continue;
        
        if (connect(sock, pr->ai_addr, pr->ai_addrlen) == 0)
            break;
        
        close(sock);
    }

    if (pr == NULL)
    {
        fprintf(stderr, "Could not connect to socket\n");
        return;
    }
    
    _port = ((sockaddr_in*)(pr->ai_addr))->sin_port;
    
    freeaddrinfo(pResult);
    
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);
    
    _socket = sock;
    _connected = true;
    _frame = (unsigned char*)malloc((HEADER_SIZE+DATA_SIZE)*sizeof(unsigned char));
    memcpy(_frame, &ck_header_bytes, HEADER_SIZE);
}

PowerSupply::~PowerSupply()
{
    free(_frame);
    free(_host);
}

char* PowerSupply::getHost()
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
    
    unsigned char* data = _frame + HEADER_SIZE;
    memset(data, 0, DATA_SIZE);
    for (auto fix = _fixtures.begin(); fix != _fixtures.end(); fix++)
    {
        int addr = (*fix)->getAddress();
        auto range = (*fix)->getValues();
        int index = 0;
        for(; range.first != range.second; range.first++)
        {
            int n = addr+index;
            if (n >= 512)
            {
                fprintf(stderr, "Fixture address %d is out of range.\n", n);
                break;
            }
            data[n] = *(range.first);
            index++;
        }
    }
    
    //dump_buffer(120, _frame);
    
    //for (uint8_t channel=1; channel<=10; channel++)
    //{
    //    _frame[16] = channel;
       send(_socket, _frame, HEADER_SIZE + DATA_SIZE, 0);  
    //}
}

FixtureRGB::FixtureRGB(int address, char r, char g, char b)
: Fixture(address)
{
    _values.push_back(r);
    _values.push_back(g);
    _values.push_back(b);
}

CKValueRange FixtureRGB::getValues() const
{
    return std::make_pair(_values.begin(), _values.end());
}

unsigned char FixtureRGB::get_red() const
{
    return _values[0];
}

unsigned char FixtureRGB::get_green() const
{
    return _values[1];
}

unsigned char FixtureRGB::get_blue() const
{
    return _values[2];
}

void FixtureRGB::set_red(unsigned char r)
{
    _values[0] = r;
}

void FixtureRGB::set_green(unsigned char g)
{
    _values[1] = g;
}

void FixtureRGB::set_blue(unsigned char b)
{
    _values[2] = b;
}

void FixtureRGB::set_rgb(unsigned char r, unsigned char g, unsigned char b)
{
    set_red(r);
    set_green(g);
    set_blue(b);
}



