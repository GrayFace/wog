#pragma once

class Architecture86 : public Architecture
{
public:
	Architecture86() {};
	~Architecture86() {};

	virtual unsigned int roundUpInstBytes(LPVOID pAddress, unsigned int minSize);
	virtual void writeNear(LPVOID from, LPVOID to, HOpcode nOpcode);
	virtual void writeAbs(LPVOID from, LPVOID to, HOpcode nOpcode);
};
