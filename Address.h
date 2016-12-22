#ifndef ADDRESS_H
#define ADDRESS_H

class Address
{
public:

    Address();
    Address( unsigned char a,
             unsigned char b,
             unsigned char c,
             unsigned char d,
             unsigned short port );
    Address( unsigned int address,
             unsigned short port );
    unsigned int GetAddress() const;
    unsigned short GetPort() const;

    // unsigned char GetA() const;
    // unsigned char GetB() const;
    // unsigned char GetC() const;
    // unsigned char GetD() const;

private:
    void init(unsigned int address, unsigned short port);
    unsigned int address;
    unsigned short port;
};

#endif