#pragma once
#include <string>
#include <vector>
using namespace std;

//описание структур для native api 1с драйвера
//https://its.1c.ru/db/metod8dev#content:5970:hdoc
namespace _1cv8 {
	wstring ConvertToXMLData(wstring datetime);

	struct CPosition
	{
		wstring Name;
		wstring Quantity;
		wstring PriceWithDiscount;
		wstring AmountWithDiscount;
		wstring DiscountAmount;
		wstring Department;
		wstring VATRate;
		wstring VATAmount;
		wstring PaymentMethod;
		wstring CalculationSubject;
		wstring CalculationAgent;
		wstring AgentData;
		wstring VendorData;
		wstring MeasurementUnit;
		wstring GoodCodeData;
		wstring CountryOfOrigin;
		wstring CustomsDeclaration;
		wstring AdditionalAttribute;
		wstring ExciseAmount;
	};

	struct CPayments
	{
		wstring Cash;
		wstring ElectronicPayment;
		wstring PrePayment;
		wstring PostPayment;
		wstring Barter;
	};

	struct CCheckPackage
	{
		wstring CashierName;
		wstring CashierINN;
		wstring OperationType;
		//CCorrectionData CorrectionData;
		wstring TaxationSystem;

		//не обязательные
		wstring CustomerInfo;
		wstring CustomerINN;
		wstring CustomerEmail;
		wstring CustomerPhone;
		wstring SenderEmail;
		wstring SaleAddress;
		wstring SaleLocation;
		wstring AgentType;
		//CAgentData AgentData;
		//CVendorData VendorData;
		//CUserAttribute UserAttribute;
		wstring AdditionalAttribute;

		vector<CPosition> Positions;
		CPayments Payments;

		CCheckPackage(wstring xml);

	};

	struct CText
	{
		wstring Text;
	};

	struct CBarcode
	{
		wstring Type;
		wstring Value;
	};

	struct COperationCounters
	{
		wstring CheckCount;
		wstring TotalChecksAmount;
		wstring CorrectionCheckCount;
		wstring TotalCorrectionChecksAmount;
		wstring toXML();
	};

	struct COutputParameters
	{
		wstring ShiftNumber;
		wstring CheckNumber;
		wstring ShiftClosingCheckNumber;
		wstring DateTime;
		wstring ShiftState;
		COperationCounters CountersOperationType1;
		COperationCounters CountersOperationType2;
		COperationCounters CountersOperationType3;
		COperationCounters CountersOperationType4;
		wstring CashBalance;
		wstring BacklogDocumentsCounter;
		wstring BacklogDocumentFirstNumber;
		wstring BacklogDocumentFirstDateTime;
		wstring FNError;
		wstring FNOverflow;
		wstring FNFail;

		COutputParameters();
		COutputParameters(vector<wstring> fromKKT);
		wstring toXML();
	};

	struct CInputParameters {
		wstring CashierName;
		wstring CashierINN;
		wstring SaleAddress;
		wstring SaleLocation;

		CInputParameters(wstring xml);
	};

	struct CDocumentOutputParameters {
		wstring ShiftNumber;
		wstring CheckNumber;
		wstring ShiftClosingCheckNumber;
		wstring AddressSiteInspections;
		wstring FiscalSign;
		wstring DateTime;

		//CDocumentOutputParameters(wstring data);
		wstring toXML();
	};

	struct CTableParametersKKT
	{
		wstring KKTNumber;
		wstring KKTSerialNumber;
		wstring FirmwareVersion;
		wstring Fiscal;
		wstring FFDVersionFN;
		wstring FFDVersionKKT;
		wstring FNSerialNumber;
		wstring DocumentNumber;
		wstring DateTime;
		wstring CompanyName;
		wstring INN;
		wstring SaleAddress;
		wstring SaleLocation;
		wstring TaxationSystems;
		wstring IsOffline;
		wstring IsEncrypted;
		wstring IsService;
		wstring IsExcisable;
		wstring IsGambling;
		wstring IsLottery;
		wstring AgentTypes;
		wstring BSOSing;
		wstring IsOnlineOnly;
		wstring IsAutomaticPrinter;
		wstring IsAutomatic;
		wstring AutomaticNumber;
		wstring OFDCompany;
		wstring OFDCompanyINN;
		wstring FNSURL;
		wstring SenderEmail;

		CTableParametersKKT();
		CTableParametersKKT(vector<wstring> fromKKT20command);
		wstring toXML();
	};
	
	struct CDriverDescription {
		wstring Name;
		wstring Description;
		wstring EquipmentType;
		wstring IntegrationComponent;
		wstring MainDriverInstalled;
		wstring DriverVersion;
		wstring IntegrationComponentVersion;
		wstring DownloadURL;
		wstring LogIsEnabled;
		wstring LogPath;

		CDriverDescription();
		wstring toXML();
	};

	struct CParam {
		wstring Name;
		wstring Caption;
		wstring Description;
		wstring TypeValue;
		wstring FieldFormat;
		wstring DefaultValue;
		wstring ReadOnly;
		wstring ChoiceList;
		wstring PageCaption;
		wstring GroupCaption;
		CParam(wstring Name, wstring Caption, wstring TypeValue, wstring DefaultValue);
	};

	struct CTableParameters {
		vector<CParam> parametrs;
		wstring Port;
		wstring Speed;
		bool WriteLog;
		wstring LogFileName;
		
		CTableParameters();
		wstring toXML();
	};

	

}