#include "1cv8.h"
#include "CPiritKKT.h"
#include "pugixml.hpp"
#include <map>
#include <sstream>

//т.к. 1с работает c wchar_t, то весь код тоже завяжем на это

namespace _1cv8 {
#define ADDNODE(name) node = root.append_child(L#name); node.text() = name.c_str()
#define ADDATTR(node, name) attr = node.append_attribute(L#name); attr.set_value(name.c_str());

#define ifPARSE(name)   if(!wcscmp(attrname, L#name)) name = attr.value()
#define elifPARSE(name) else ifPARSE(name)

#define ifPARSEto(_struct, name)   if(!wcscmp(attrname, L#name)) _struct.name = attr.value()
#define elifPARSEto(_struct, name) else ifPARSEto(_struct, name)

    wstring CDocumentOutputParameters::toXML()
    {
        wstringstream ss;

        pugi::xml_document doc;
        pugi::xml_node root = doc.append_child(L"DocumentOutputParameters");
        pugi::xml_node parameters = root.append_child(L"Parameters");
        pugi::xml_attribute attr;
        ADDATTR(parameters, ShiftNumber);
        ADDATTR(parameters, CheckNumber);
        ADDATTR(parameters, ShiftClosingCheckNumber);
        ADDATTR(parameters, FiscalSign);
        ADDATTR(parameters, AddressSiteInspections);
        ADDATTR(parameters, DateTime);

        doc.save(ss);
        return ss.str();
    }

    CInputParameters::CInputParameters(wstring xml)
    {
        pugi::xml_document doc;
        pugi::xml_parse_result parseresult = doc.load_string(xml.c_str());

        if (!parseresult)
        {
            throw "parse CInputParameters failed";
        }

        for (pugi::xml_node check : doc.children(L"InputParameters"))
        {
            for (pugi::xml_node child : check.children())
            {
                if (!wcscmp(child.name(), L"Parameters"))
                {
                    for (pugi::xml_attribute attr : child.attributes())
                    {
                        auto attrname = attr.name();
                        ifPARSE(CashierName);
                        elifPARSE(CashierINN);
                        elifPARSE(SaleAddress);
                        elifPARSE(SaleLocation);
                    }
                }

            }
        }
    }
   
    CCheckPackage::CCheckPackage(wstring xml)
    {
        pugi::xml_document doc;
        pugi::xml_parse_result parseresult = doc.load_string(xml.c_str());

        if (!parseresult)
        {
            throw "parse CCheckPackage failed";
        }

        for (pugi::xml_node check : doc.children(L"CheckPackage"))
        {

            for (pugi::xml_node child : check.children())
            {
                if (!wcscmp(child.name(), L"Positions")) {

                    for (pugi::xml_node check_position : child.children())
                    {
                        if (!wcscmp(check_position.name(), L"FiscalString")) {
                            _1cv8::CPosition position = _1cv8::CPosition();

                            for (pugi::xml_attribute attr : check_position.attributes())
                            {
                                auto attrname = attr.name();
                                ifPARSEto(position, Name);
                                elifPARSEto(position, Quantity);
                                elifPARSEto(position, PriceWithDiscount);
                                elifPARSEto(position, AmountWithDiscount);
                                elifPARSEto(position, DiscountAmount);
                                elifPARSEto(position, Department);
                                elifPARSEto(position, VATRate);
                                elifPARSEto(position, VATAmount);
                                elifPARSEto(position, PaymentMethod);
                                elifPARSEto(position, CalculationSubject);
                                elifPARSEto(position, CalculationAgent);
                                elifPARSEto(position, AgentData);
                                elifPARSEto(position, VendorData);
                                elifPARSEto(position, MeasurementUnit);
                                elifPARSEto(position, GoodCodeData);
                                elifPARSEto(position, CountryOfOrigin);
                                elifPARSEto(position, CustomsDeclaration);
                                elifPARSEto(position, AdditionalAttribute);
                                elifPARSEto(position, ExciseAmount);

                            }

                            Positions.push_back(position);
                        }
                    }
                }
                else if (!wcscmp(child.name(), L"Payments"))
                {
                    for (pugi::xml_attribute attr : child.attributes())
                    {
                        auto attrname = attr.name();
                        ifPARSEto(Payments, Cash);
                        elifPARSEto(Payments, ElectronicPayment);
                        elifPARSEto(Payments, PrePayment);
                        elifPARSEto(Payments, PostPayment);
                        elifPARSEto(Payments, Barter);
                    }
                }
                else if (!wcscmp(child.name(), L"Parameters"))
                {
                    for (pugi::xml_attribute attr : child.attributes())
                    {
                        auto attrname = attr.name();
                        ifPARSE(CashierName);
                        elifPARSE(CashierINN);
                        elifPARSE(OperationType);
                        //elifPARSE(result, CorrectionData);
                        elifPARSE(TaxationSystem);
                        elifPARSE(CustomerInfo);
                        elifPARSE(CustomerINN);
                        elifPARSE(CustomerEmail);
                        elifPARSE(CustomerPhone);
                        elifPARSE(SenderEmail);
                        elifPARSE(SaleAddress);
                        elifPARSE(SaleLocation);
                        elifPARSE(AgentType);
                        elifPARSE(AdditionalAttribute);
                    }
                }

            }
        }
    }

    COutputParameters::COutputParameters()
    {
        ShiftNumber = L"1";
        CheckNumber = L"1";
        ShiftClosingCheckNumber = L"1";
        DateTime = L"010120235959";
        ShiftState = L"1";
        CashBalance = L"0";
        BacklogDocumentsCounter = L"0";
        BacklogDocumentFirstNumber = L"0";
        BacklogDocumentFirstDateTime = L"0";
        FNError = L"False";
        FNOverflow = L"False";
        FNFail = L"False";
    }

	COutputParameters::COutputParameters(vector<wstring> fromKKT):COutputParameters()
	{
        try {
            ShiftNumber = fromKKT.at(5);
            CheckNumber = fromKKT.at(0);
            ShiftClosingCheckNumber = fromKKT.at(6);
            DateTime = fromKKT.at(7) + fromKKT.at(8);
        }
        catch (const std::out_of_range & ex)
        {
            //std::cout << "out_of_range Exception Caught :: " << ex.what() << std::endl;
        }
	}

	wstring COutputParameters::toXML()
    {
        wstringstream ss;

        pugi::xml_document doc;
        pugi::xml_node root = doc.append_child(L"OutputParameters");
        pugi::xml_node parameters = root.append_child(L"Parameters");
        pugi::xml_attribute attr;
        ADDATTR(parameters, ShiftNumber);
        ADDATTR(parameters, ShiftNumber);
        ADDATTR(parameters, CheckNumber);
        ADDATTR(parameters, ShiftClosingCheckNumber);
        ADDATTR(parameters, DateTime);
        ADDATTR(parameters, ShiftState);
        ADDATTR(parameters, CashBalance);
        ADDATTR(parameters, BacklogDocumentsCounter);
        ADDATTR(parameters, BacklogDocumentFirstNumber);
        ADDATTR(parameters, BacklogDocumentFirstDateTime);
        ADDATTR(parameters, FNError);
        ADDATTR(parameters, FNOverflow);
        ADDATTR(parameters, FNFail);
        
        doc.save(ss);
        return ss.str();
    }

    CTableParametersKKT::CTableParametersKKT()
    {
        KKTNumber = L"1";
        KKTSerialNumber = L"1";
        FirmwareVersion = L"0";
        Fiscal = L"true";
        FFDVersionFN = L"1.1";
        FFDVersionKKT = L"1.0.5";
        FNSerialNumber = L"1";
        DocumentNumber = L"1";
        DateTime = L"010120235959";
        CompanyName = L"Company";
        INN = L"";
        SaleAddress = L"";
        SaleLocation = L"";
        TaxationSystems = L"0";
        IsOffline = L"False";
        IsEncrypted = L"False";
        IsService = L"True";
        IsExcisable = L"False";
        IsGambling = L"False";
        IsLottery = L"False";
        AgentTypes = L"6";
        BSOSing = L"False";
        IsOnlineOnly = L"False";
        IsAutomaticPrinter = L"False";
        IsAutomatic = L"False";
        AutomaticNumber = L"0";
        OFDCompany = L"OFD";
        OFDCompanyINN = L"OFD INN";
        FNSURL = L"OFD site";
        SenderEmail = L"SenderEmail";
    }

    CTableParametersKKT::CTableParametersKKT(vector<wstring> fromKKT20command):CTableParametersKKT()
    {
        try {
            //т.к. пририт возвращает не все, просто запишем любые значения
            KKTNumber = fromKKT20command.at(3);
            KKTSerialNumber = fromKKT20command.at(0);
            FirmwareVersion = fromKKT20command.at(1);
            DateTime = fromKKT20command.at(5);
            INN = fromKKT20command.at(2);
        }
        catch (const std::out_of_range & ex)
        {
            //std::cout << "out_of_range Exception Caught :: " << ex.what() << std::endl;
        }
    }

    wstring CTableParametersKKT::toXML()
    {
        wstringstream ss;

        pugi::xml_document doc;
        pugi::xml_node parameters = doc.append_child(L"Parameters");
        //pugi::xml_node parameters = root.append_child(L"Parameters");
        pugi::xml_attribute attr;
        ADDATTR(parameters, KKTNumber);
        ADDATTR(parameters, KKTSerialNumber);
        ADDATTR(parameters, FirmwareVersion);
        ADDATTR(parameters, Fiscal);
        ADDATTR(parameters, FFDVersionFN);
        ADDATTR(parameters, FFDVersionKKT);
        ADDATTR(parameters, FNSerialNumber);
        ADDATTR(parameters, DocumentNumber);
        ADDATTR(parameters, DateTime);
        ADDATTR(parameters, CompanyName);
        ADDATTR(parameters, INN);
        ADDATTR(parameters, SaleAddress);
        ADDATTR(parameters, SaleLocation);
        ADDATTR(parameters, TaxationSystems);
        ADDATTR(parameters, IsOffline);
        ADDATTR(parameters, IsEncrypted);
        ADDATTR(parameters, IsService);
        ADDATTR(parameters, IsExcisable);
        ADDATTR(parameters, IsGambling);
        ADDATTR(parameters, IsLottery);
        ADDATTR(parameters, AgentTypes);
        ADDATTR(parameters, BSOSing);
        ADDATTR(parameters, IsOnlineOnly);
        ADDATTR(parameters, IsAutomaticPrinter);
        ADDATTR(parameters, IsAutomatic);
        ADDATTR(parameters, AutomaticNumber);
        ADDATTR(parameters, OFDCompany);
        ADDATTR(parameters, OFDCompanyINN);
        ADDATTR(parameters, FNSURL);
        ADDATTR(parameters, SenderEmail);

        doc.save(ss);
        return ss.str();
    }

    wstring COperationCounters::toXML()
    {
        wstringstream ss;

        pugi::xml_document doc;
        pugi::xml_node node;
        pugi::xml_node root = doc.append_child(L"COperationCounters");
        pugi::xml_attribute attr;

        doc.save(ss);
        return ss.str();
    }

    CDriverDescription::CDriverDescription()
    {
        Name = L"PiritKKT OpenSource driver";
        Description = L"crossplatform driver";
        EquipmentType = L"ККТ";
        IntegrationComponent = L"False";
        MainDriverInstalled = L"False";
        DriverVersion = L"0.1";
        IntegrationComponentVersion = L"0.0.1.1";
        DownloadURL = L"www.github.com";
        LogIsEnabled = L"False";
        LogPath = L"";
    }

    wstring CDriverDescription::toXML()
    {
        wstringstream ss;

        pugi::xml_document doc;
        pugi::xml_node parameters = doc.append_child(L"DriverDescription");
        pugi::xml_attribute attr;
        ADDATTR(parameters, Name);
        ADDATTR(parameters, Description);
        ADDATTR(parameters, EquipmentType);
        ADDATTR(parameters, IntegrationComponent);
        ADDATTR(parameters, MainDriverInstalled);
        ADDATTR(parameters, DriverVersion);
        ADDATTR(parameters, IntegrationComponentVersion);
        ADDATTR(parameters, DownloadURL);
        ADDATTR(parameters, LogIsEnabled);
        ADDATTR(parameters, LogPath);

        doc.save(ss);
        return ss.str();
    }
    CTableParameters::CTableParameters()
    {
        Port = L"COM3";
        Speed = L"57600";
        //parametrs.push_back(CParam(L"Port", L"COM Порт", L"Number", L"3"));
        //parametrs.push_back(CParam(L"Speed", L"Скорость", L"Number", L"57600"));
    }

    wstring CTableParameters::toXML()
    {
        wstringstream ss;

        pugi::xml_document doc;
        pugi::xml_attribute attr;
        pugi::xml_node root = doc.append_child(L"Settings");
        pugi::xml_node page = root.append_child(L"Page");
        wstring Caption = L"Параметры2";
        ADDATTR(page, Caption);
        pugi::xml_node paramnode = page.append_child(L"Parameter");
        attr = paramnode.append_attribute(L"Name");
        attr.set_value(L"Port");
        attr = paramnode.append_attribute(L"Caption");
        attr.set_value(L"COM Порт");
        attr = paramnode.append_attribute(L"TypeValue");
        attr.set_value(L"String");
        attr = paramnode.append_attribute(L"DefaultValue");
        attr.set_value(Port.c_str());

        //for (CParam param : parametrs) {
        //    pugi::xml_node paramnode = page.append_child(L"Parameter");
        //    ADDATTR(paramnode, param.Name);
        //    ADDATTR(paramnode, param.Caption);
        //    //ADDATTR(paramnode, param.Description);
        //    ADDATTR(paramnode, param.TypeValue);
        //    //ADDATTR(paramnode, param.FieldFormat);
        //    ADDATTR(paramnode, param.DefaultValue);
        //    //ADDATTR(paramnode, param.ReadOnly);
        //    ADDATTR(paramnode, param.ChoiceList);
        //    //ADDATTR(paramnode, param.PageCaption);
        //    //ADDATTR(paramnode, param.GroupCaption);
        //}        

        doc.save(ss);
        return ss.str();
    }
    CParam::CParam(wstring Name, wstring Caption, wstring TypeValue, wstring DefaultValue)
    {
        this->Name = Name;
        this->Caption = Caption;
        this->TypeValue = TypeValue;
        this->DefaultValue = DefaultValue;
    }
}
