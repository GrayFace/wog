#pragma once


class Architecture64 : public Architecture86
{
public:
	Architecture64() {};
	~Architecture64() {};

	virtual unsigned int roundUpInstBytes( LPVOID pAddress, unsigned int minSize );
	virtual void writeAbs(LPVOID from, LPVOID to, HOpcode nOpcode);
};
