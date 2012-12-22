//
//  kinet.h
//
//  Created by Michael Dewberry on 12/19/12.
//
//  Port of kinet.py by Giles Hall
//  http://github.com/vishnubob/kinet
//

#include <vector>
#include <list>

typedef std::pair<std::vector<char>::const_iterator, std::vector<char>::const_iterator> CKValueRange;

class Fixture
{
public:
    Fixture(int address=0) : _address(address) {}
    int getAddress() { return _address; }
    void setAddress(int address) { _address = address; }
    virtual CKValueRange getValues() const = 0;
protected:
    int _address;
};

class FixtureRGB : public Fixture
{
public:
    FixtureRGB(int address, char r=0, char g=0, char b=0);
    virtual CKValueRange getValues() const;
    
    unsigned char get_red() const;
    unsigned char get_green() const;
    unsigned char get_blue() const;
    
    void set_red(unsigned char r);
    void set_green(unsigned char g);
    void set_blue(unsigned char b);
    
    void set_rgb(unsigned char r, unsigned char g, unsigned char b);

protected:
    std::vector<char> _values;
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
    unsigned char* _frame;
    char* _host;
    int _port;
};