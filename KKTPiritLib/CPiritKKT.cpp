#include "CPiritKKT.h"
#include "ceSerial.h"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>
#include "convstring.h"
#include <algorithm>
#include <stdio.h>

string GetCurrentTimeCrossplatform() {
    stringstream date;
    time_t timer = time(0);   // get time now    
    struct tm* now = new tm();

#ifdef _WINDOWS
    errno_t err = localtime_s(now, &timer);
#else
    struct tm buf;
    now = localtime_r(&timer, &buf);

#endif // _WINDOWS

    date << put_time(now, "%d.%m.%y %H-%M-%S:");
    return date.str();
}

void CPiritKKT::Log(string text) {
    if (logFile != 0) {
        string result = GetCurrentTimeCrossplatform() + " --- " +text + "\n";
        fwrite(result.c_str(), sizeof(char), result.length(), logFile);
        fflush(logFile);
    }
}

void CPiritKKT::Log(wstring text) {
    Log(conv::unicode::ToCP866(text));
}

CPiritKKT::CPiritKKT()
{
    logFile = 0;
    toFile = false;
}

CPiritKKT::~CPiritKKT()
{
    if (_com.IsOpened()) {
        _com.Close();
    }
    
    if (logFile != 0) {
        fclose(logFile);
    }
}

string calcCRC(string packet)
{
    int crc = 0;

    for (int i = 1; i < packet.length(); i++) {
        crc = (crc ^ packet[i]) & 0xFF;
    }

    stringstream ss;
    ss << std::hex << crc;
    string temp;
    ss >> temp;

    transform(temp.begin(), temp.end(), temp.begin(), [](unsigned char c) -> unsigned char { return std::toupper(c); });

    return temp;

}

bool CPiritKKT::Connect(wstring wcomport, wstring logfilename)
{
    if (logfilename != L"") {
        string _filename = conv::unicode::ToCP866(logfilename);
        logFile = fopen(_filename.c_str(), "a+");
        if (logFile != 0) {
            toFile = true;
        }
        return true;
    }

    string comport = conv::unicode::ToCP866(wcomport);
#ifdef ceWINDOWS
    _com = ce::ceSerial(comport, 57600, 8, 'N', 1); // Windows
#else
    _com = ce::ceSerial(comport, 57600, 8, 'N', 1); // Linux
#endif

    if (_com.Open() != 0) {
          return 0;
    }

    bool successFlag = _com.WriteChar('\x05'); // write a character

    char c = _com.ReadChar(successFlag); // read a char
    if (successFlag && c == '\x06') 
        return true;

	return false;
}

void CPiritKKT::Disconnect()
{
    if (_com.IsOpened())
        _com.Close();
}

PiritPacket CPiritKKT::Send(PiritPacket request)
{
    string answer;
    char c;

    string req = conv::unicode::ToCP866(request.toStr());
    req += calcCRC(req);
    
    if (toFile)
    {
        Log(req);
        return request.SuccessPacket();
    }
    else {
        bool successFlag = _com.Write((char*)(req.c_str()));

        // � ����� ���� ������ ������ �������. ���� ������ ������ ��� ����� �����
        do
        {
            ce::ceSerial::Delay(50);
            c = _com.ReadChar(successFlag);
            //cout << "c = " << c << " hex = "<< hex << (int)c << "\n";
        } while (c != STX && c != '\x0');

        //���� �� ������ ������
        if (c == '\x0')
            return request.ErrorPacket(L"09");
        if (c != STX)
            return request.ErrorPacket();
        answer += c;

        do
        {
            c = _com.ReadChar(successFlag);
            if (successFlag)  answer += c;
        } while (c != ETX);

        //CRC
        c = _com.ReadChar(successFlag);
        if (successFlag)  answer += c;
        c = _com.ReadChar(successFlag);
        if (successFlag)  answer += c;


        //Log(answer);
        PiritPacket answerPacket = PiritPacket(conv::unicode::FromCP866(answer));
        if (answerPacket.CorrespondsTo(request))
            return answerPacket;

    }

    return request.ErrorPacket();
}

pirit_answer CPiritKKT::Test()
{
    Log("Test");
    PiritPacket request = PiritPacket(L"05");
    PiritPacket answer = Send(request);
    return KKT_ANSWER(answer.error);
}

pirit_answer CPiritKKT::StartWork()
{
    Log("StartWork");

    wstringstream date;
    time_t timer = time(0);   // get time now    
    struct tm* now = new tm();

#ifdef _WINDOWS
    errno_t err = localtime_s(now,&timer);
#else
    struct tm buf;
    now = localtime_r(&timer, &buf);
    
#endif // _WINDOWS

    date << put_time(now, L"%d%m%y");
    wstringstream time;
    time << put_time(now, L"%H%M%S");
    vector<wstring> data;
    data.push_back(date.str());
    data.push_back(time.str());

    PiritPacket request = PiritPacket(L"10", data);
    PiritPacket answer = Send(request);

    return pirit_answer{ answer.error, (answer.error == L"00" || answer.error == L"0B") ? true:false };
}

pirit_answer CPiritKKT::OpenShift(wstring CashierName)
{    
    PiritPacket request = PiritPacket(L"23", CashierName);
    PiritPacket answer = Send(request);
    return KKT_ANSWER(answer.error);
}

pirit_answer CPiritKKT::CloseShift(wstring CashierName)
{
    vector<wstring> data;
    data.push_back(CashierName);
    data.push_back(L"0");
    PiritPacket request = PiritPacket(L"21", data);
    PiritPacket answer = Send(request);

    return KKT_ANSWER(answer.error);
}

pirit_answer CPiritKKT::PrintXReport(wstring CashierName)
{
    vector<wstring> data;
    data.push_back(CashierName);
    data.push_back(L"0");
    PiritPacket request = PiritPacket(L"20", data);
    PiritPacket answer = Send(request);

    return KKT_ANSWER(answer.error);
}

pirit_answer CPiritKKT::OpenReceipt(wstring OperationType, wstring TaxationSystem, wstring CashierName)
{    
    vector<wstring> data;
    data.push_back(OperationType); //(����� �����) ����� � ��� ��������� 4 - ��� �������; 12 -�������
    data.push_back(L"1"); //(����� ����� 1..99) ����� ������
    data.push_back(CashierName); //(��� ���������) ��� ���������
    data.push_back(L""); //(����� �����) ����� ���������
    data.push_back(TaxationSystem); //(����� 0..5) ������� ���������������
    data.push_back(L""); //(������) ����� ������������, ������ 1
    data.push_back(L""); //(������) ����� ������������, ������ 1
    PiritPacket request = PiritPacket(PIRIT_COMMAND_OPENDOC, data);
    PiritPacket answer = Send(request);

    return KKT_ANSWER(answer.error);
}

pirit_answer CPiritKKT::CloseReceipt()
{
    vector<wstring> data;

    data.push_back(L"0"); //(����� �����) ���� �������
    data.push_back(L""); //(������) ����� ����������
    data.push_back(L""); //(�����) ������ �����
    data.push_back(L""); //(������)����� ��������
    data.push_back(L""); //(������) ����� ����������� �����
    data.push_back(L""); //(������) ����� ��������
    data.push_back(L""); //(������)�������� ��������������� ��������� ������������
    data.push_back(L""); //(������)�������� ��������������� ��������� ������������
    data.push_back(L""); //(������)[0..128] ����������
    data.push_back(L""); //(������)[0..12] ��� ����������

    PiritPacket request = PiritPacket(PIRIT_COMMAND_CLOSEDOC, data);
    PiritPacket answer = Send(request);

    return pirit_answer{ answer.error, answer.error == L"00" ? true : false, answer.GetData() };  
}

pirit_answer CPiritKKT::CancelReceipt()
{
    PiritPacket request = PiritPacket(PIRIT_COMMAND_CANCELDOC, L"");
    PiritPacket answer = Send(request);

    return KKT_ANSWER(answer.error);
}

pirit_answer CPiritKKT::AddGoods(wstring name, wstring num, wstring price, wstring DiscountAmount, wstring VATRate, wstring PaymentMethod, wstring CalculationSubject)
{
    vector<wstring> data;

    data.push_back(name.substr(0,55)); // 56 - ����������� ������. (������[0...256]) �������� ������
    data.push_back(L""); //������� ��� ��������� ��� ������/����� ���
    data.push_back(num); //���������� ������ � �������� �������
    data.push_back(price); //(������� �����[0..99999999.99]) ���� ������ �� ������� ��������
    data.push_back(VATRate); //(����� �����) ����� ������ ������ //0 -20%
    data.push_back(L""); //(������[0..4]) ����� �������� �������
    data.push_back(L""); //(����� ����� 1..16) ����� ������
    data.push_back(L""); //(����� �����) ������ ��������
    data.push_back(L""); //(������[0...38]) ������ ��������
    data.push_back(DiscountAmount); //(������� �����) ����� ������
    data.push_back(PaymentMethod); //(����� �����) ������� ������� ������� //������ ������ 4
    data.push_back(CalculationSubject); //(����� �����) ������� �������� �������
    data.push_back(L""); //(������[3]) ��� ������ ������������� ������
    data.push_back(L""); //(������[0...31]) ����� ���������� ����������
    PiritPacket request = PiritPacket(PIRIT_COMMAND_ADDGOODS, data);
    PiritPacket answer = Send(request);

    return KKT_ANSWER(answer.error);
}

pirit_answer CPiritKKT::Subtotal()
{
    PiritPacket request = PiritPacket(PIRIT_COMMAND_SUBTOTAL, L"");
    PiritPacket answer = Send(request);

    return KKT_ANSWER(answer.error);
}

pirit_answer CPiritKKT::Payment(wstring type, wstring summ)
{
    vector<wstring> data;

    data.push_back(type); //(����� ����� 0..15) ��� ���� �������
    data.push_back(summ); //(������� �����) �����, �������� �� ���������� �� ������� �������
    data.push_back(L""); //(������[0..44]) �������������� �����

    PiritPacket request = PiritPacket(PIRIT_COMMAND_PAYMENT, data);
    PiritPacket answer = Send(request);

    return KKT_ANSWER(answer.error);
}

//���������� ��������� �������� ������� �������� ���������� ��� ������� 02
//������� ������������ ���������� ������������� ������� � �������
pirit_answer CPiritKKT::GetDataKKT()
{
    //TODO 10 ������� �� ��������
    vector<wstring> commands = { L"1",L"2",L"3",L"4",L"5",L"6",L"7",L"8",L"9" };//, L"10", L"11", L"12", L"14", L"15", L"16", L"17", L"21", L"23", L"24", L"70"};
    vector<wstring> param;
    
    for (wstring num : commands) {
        vector<wstring> outparam;
        outparam.push_back(num);
        outparam.push_back(L"");
        //outparam.push_back(L"");
        PiritPacket request = PiritPacket(L"02", outparam); //��������� �����
        PiritPacket answer = Send(request);
        if (answer.error == L"00") {
            param.push_back(answer.GetData().at(1));
        }
        else
        {
            return pirit_answer{ answer.error, false, param };
        }
    }

    return pirit_answer{ L"00", true, param };
}