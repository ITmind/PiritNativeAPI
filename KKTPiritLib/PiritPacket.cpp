#include "PiritPacket.h"
#include <sstream>
#include <algorithm>

PiritPacket::PiritPacket(wstring id, wstring command, wstring data, wstring error)
{
	this->id = id;
	this->command = command;
	this->data = data;
	this->error = error;
	this->pass = L"PIRI";
}

PiritPacket::PiritPacket(wstring command, wstring data) : PiritPacket(L"1", command, data, L"00") {}

PiritPacket::PiritPacket(wstring command, vector<wstring> data): PiritPacket(command,L"")
{
	wstring datastr = L"";
	for (auto x : data) {
		if(datastr != L"")
			datastr += DATA_SPLIT_CHAR;
		datastr += x;	
	}
	this -> data = datastr;
	
}


PiritPacket::PiritPacket(wstring answer)
{
	if (answer.length() < 4 || answer[0] != STX) {
		error = L"03";
		return;
	}
	
	//парсим ответ. не всегда может что то прийти
	size_t i = 1;
	for (; i < answer.length(); i++)
	{
		if (answer[i] == ETX) break;
		if(i == 1)
			id = answer[i];
		else if(i == 2)
			command = answer[i];
		else if (i == 3)
			command += answer[i];
		else if (i == 4)
			error = answer[i];
		else if (i == 5)
			error += answer[i];
		else
		{
			data += answer[i];
		}
	}
	crc = answer[i + 1];
	crc += answer[i + 2];

	if (crc == L"00")
		error = L"03";

}

PiritPacket::~PiritPacket()
{
}


PiritPacket PiritPacket::ErrorPacket(wstring err)
{
	return PiritPacket(id,command,data, err);
}

PiritPacket PiritPacket::SuccessPacket()
{
	return PiritPacket(id, command, data, L"00");
}

wstring PiritPacket::toStr()
{
	wstring result;

	result += STX;
	result += pass;
	result += id;
	result += command;
	result += data;
	result += ETX;

	return result;
}

vector<wstring> PiritPacket::GetData()
{
	std::vector<wstring> elems;
	std::wstringstream ss(data);
	std::wstring param;
	while (std::getline(ss, param, L'\x1C')) {
		elems.push_back(param);
	}
	return elems;

}

bool PiritPacket::CorrespondsTo(PiritPacket request)
{
	if (request.id == id && request.command == command)
		return true;

	return false;
}