// Консоль для тестирования разного кода
// 

#include <iostream>
#include <sstream>
#include "CPiritKKT.h"
#include "PiritPacket.h"
#include "1cv8.h"
#include "convstring.h"

#define LOG(func) answer = func;\
                  if (answer.result) wcout << #func << L"\n";\
                  else {wcout << #func << L" error: " << answer.kod << L"\n"; return 0;}

bool PrintCheck(CPiritKKT& PiritKKT, _1cv8::CCheckPackage xmlcheck) {
    pirit_answer answer;

    wstring operationType; 
    if (xmlcheck.OperationType == L"1") operationType = L"2"; //чек на продажу
    else if(xmlcheck.OperationType == L"2") operationType = L"3"; //чек на продажу (возврат)
    else if(xmlcheck.OperationType == L"3") operationType = L"6"; //расход ДС
    else if(xmlcheck.OperationType == L"4") operationType = L"7"; //расход ДС (возврат)
    
    LOG(PiritKKT.OpenReceipt(operationType, xmlcheck.TaxationSystem, xmlcheck.CashierName));

    for (auto pos : xmlcheck.Positions) {
        wstring VATRate = L"0";
        if (pos.VATRate == L"none") VATRate = L"4"; 
        else if (pos.VATRate == L"20") VATRate = L"1"; 
        else if (pos.VATRate == L"10") VATRate = L"2"; 
        else if (pos.VATRate == L"0") VATRate = L"3"; 
        else if (pos.VATRate == L"20/120") VATRate = L"5";
        else if (pos.VATRate == L"10/110") VATRate = L"6";

        //string Name=
        LOG(PiritKKT.AddGoods(pos.Name, pos.Quantity, pos.PriceWithDiscount, pos.DiscountAmount
                              , VATRate, pos.PaymentMethod, pos.CalculationSubject)
        );
    }

    LOG(PiritKKT.Subtotal());
    //0 - наличные
    //1 - безналичные
    //13 - аванс
    //14 - кредит
    //15 - иная
    if(xmlcheck.Payments.Cash != L"0")
        LOG(PiritKKT.Payment(L"0", xmlcheck.Payments.Cash));
    if (xmlcheck.Payments.ElectronicPayment != L"0")
        LOG(PiritKKT.Payment(L"1", xmlcheck.Payments.ElectronicPayment));
    if (xmlcheck.Payments.PrePayment != L"0")
        LOG(PiritKKT.Payment(L"13", xmlcheck.Payments.PrePayment));
    if (xmlcheck.Payments.PostPayment != L"0")
        LOG(PiritKKT.Payment(L"14", xmlcheck.Payments.PostPayment));
    if (xmlcheck.Payments.Barter != L"0")
        LOG(PiritKKT.Payment(L"15", xmlcheck.Payments.Barter));

    //LOG(PiritKKT.CancelReceipt());
    auto answer2 = LOG(PiritKKT.CloseReceipt());

    if (answer2.result) {
        _1cv8::CDocumentOutputParameters out;
        out.CheckNumber = answer.data[0];
        out.ShiftNumber = answer.data[5];
        out.ShiftClosingCheckNumber = answer.data[6];
        out.FiscalSign = answer.data[4];
        out.DateTime = answer.data[7];
        out.DateTime += answer.data[8];
    }
    
    return 1;
}

_1cv8::COutputParameters ShiftClose(CPiritKKT& PiritKKT)
{
    _1cv8::COutputParameters out = _1cv8::COutputParameters();

    auto answer = PiritKKT.CloseShift(L"Кулик");
    if (answer.result) {
        //out.ShiftNumber = 
    }
    
    return out;
}

int main()
{
    setlocale(LC_ALL, "");
    pirit_answer answer;

    wcout << L"PIRIT KKT" << L"\n";
    wcout << L"*****************" << L"\n";
    wcout << L"тест 123 test" << L"\n";
    
    wstring wCashierName = L"Тест123test";
    string CashierName = conv::utf8::convert(wCashierName);
    string cp866CashierName = conv::unicode::ToCP866(wCashierName);
    wstring UnicodeCashierName = conv::unicode::FromCP866(cp866CashierName);
    //auto s2 = CashierName.c_str();
 
    wstring check = LR"delimiter(
<?xml version="1.0" encoding="UTF-8"?>
<CheckPackage>
	<Parameters OperationType="1" TaxationSystem="0" CashierName="Печенкина М. В." CustomerEmail="" CustomerPhone="">
		<AgentData/>
		<PurveyorData/>
	</Parameters>
	<Positions>
		<FiscalString Name="Тачка строительная Кратон WB-180H" Quantity="1" PriceWithDiscount="3720.08" AmountWithDiscount="3720.08" DiscountAmount="0" Department="1" VATRate="18/118" PaymentMethod="1" CalculationSubject="10"  VATAmount="620.01"/>
		<FiscalString Name="Тачка строительная Кратон WB-180H" Quantity="1" PriceWithDiscount="1860.05" AmountWithDiscount="1860.05" DiscountAmount="0" Department="1" VATRate="18/118" PaymentMethod="1" CalculationSubject="10"  VATAmount="310.01"/>
		<FiscalString Name="Тачка строительная Кратон WB-180DH" Quantity="1" PriceWithDiscount="2347.5" AmountWithDiscount="2347.5" DiscountAmount="0" Department="1" VATRate="18/118" PaymentMethod="1"  CalculationSubject="10"  VATAmount="391.25"/>
		<FiscalString Name="Тачка строительная Кратон WB-180DH" Quantity="1" PriceWithDiscount="2347.49" AmountWithDiscount="2347.49" DiscountAmount="0" Department="1" VATRate="18/118" PaymentMethod="1" CalculationSubject="10" VATAmount="391.25"/>
	</Positions>
	<Payments Cash="10275.12" ElectronicPayment="0" PostPayment="0" PrePayment="0" Barter="0"/>
</CheckPackage>
)delimiter";

    wstring inputxml = LR"delimiter(
 <?xml version="1.0" encoding="UTF-8"?>
 <InputParameters> 
	<Parameters CashierName="Иванов И.П." CashierINN="32456234523452"/>
 </InputParameters>
)delimiter";
    
    wcout << inputxml;

    //_1cv8::CInputParameters input(inputxml);
    //_1cv8::CCheckPackage xmlcheck = _1cv8::CCheckPackage(check);
     
    //CPiritKKT PiritKKT = CPiritKKT();
    //PiritKKT.Connect("COM3");
    //LOG(PiritKKT.StartWork());
    //LOG(PiritKKT.CloseShift());
    //LOG(PiritKKT.OpenShift(utf16to8(input.CashierName)));
    
    //PrintCheck(PiritKKT, xmlcheck);

    //LOG(PiritKKT.StartWork());

    return 0;
    
}