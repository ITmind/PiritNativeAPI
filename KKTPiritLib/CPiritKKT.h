#pragma once
#include <queue>
#include <string>
#include "PiritPacket.h"
#include "ceSerial.h"
using namespace std;
#define KKT_ANSWER(err) pirit_answer{ err, err == L"00" ? true : false }
#define PIRIT_COMMAND_OPENDOC   L"30"
#define PIRIT_COMMAND_CLOSEDOC  L"31"
#define PIRIT_COMMAND_CANCELDOC L"32"
#define PIRIT_COMMAND_ADDGOODS  L"42"
#define PIRIT_COMMAND_SUBTOTAL  L"44"
#define PIRIT_COMMAND_PAYMENT   L"47"

#define ADDPARAM(param) data.push_back(param)

typedef struct {
	wstring kod;
	bool result;
	vector<wstring> data;
} pirit_answer;

class CPiritKKT
{
private:
	ce::ceSerial _com;
	
	//string _fileName;
	FILE* logFile;

public:
	bool toFile = false;
	//queue<PiritPacket> packetQueue;
	CPiritKKT();
	~CPiritKKT();

	bool Connect(wstring wcomport, wstring logfilename);
	void Disconnect();

	PiritPacket Send(PiritPacket packet);
	void Log(string text);
	void Log(wstring text);

	//команды ККТ
	pirit_answer Test();
	pirit_answer StartWork();
	pirit_answer OpenShift(wstring CashierName);
	pirit_answer CloseShift(wstring CashierName);
	pirit_answer PrintXReport(wstring CashierName);
	pirit_answer OpenReceipt(wstring OperationType, wstring TaxationSystem, wstring CashierName);
	pirit_answer CloseReceipt();
	pirit_answer CancelReceipt();
	pirit_answer AddGoods(wstring name, wstring num, wstring price, wstring DiscountAmount, wstring VATRate, wstring PaymentMethod, wstring CalculationSubject);
	pirit_answer Subtotal();
	pirit_answer Payment(wstring summ, wstring type);
	pirit_answer GetDataKKT();
};

