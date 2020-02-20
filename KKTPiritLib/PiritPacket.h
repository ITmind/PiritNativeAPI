#pragma once
#include <string>
#include <vector>
using namespace std;

#define STX '\x02'
#define ETX '\x03'
#define AVIABLE '\x06'
#define DATA_SPLIT_CHAR L"\x1C"

class PiritPacket
{
private:
	wstring crc;
	wstring id;
	wstring pass;
	wstring command;
	wstring data;

	PiritPacket(wstring id, wstring command, wstring data, wstring error);

public:
	wstring error;

	PiritPacket(wstring answer);
	PiritPacket(wstring command, wstring data);
	PiritPacket(wstring command, vector<wstring> data);
	~PiritPacket();

	//если в ответ нет ничего
	PiritPacket ErrorPacket(wstring err = L"03");
	wstring toStr();
	vector<wstring> GetData();
	bool CorrespondsTo(PiritPacket request);
	
};

