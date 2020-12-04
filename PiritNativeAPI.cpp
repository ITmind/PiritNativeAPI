#ifdef __linux__
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#endif

#include <wchar.h>
#include <string>
#include <locale.h>
#include "PiritNativeAPI.h"
#include <fcntl.h>
#include <stdio.h>

#include "CPiritKKT.h"
#include "1cv8.h"


#define DEVICEID L"device1"

static const wchar_t* g_PropNames[] = {
    L"Port",
    L"Speed",
    L"WriteLog",
    L"LogFileName"
};
static const wchar_t* g_MethodNames[] = {
    L"ShowMsgBox",
    L"GetDataKKT",
    L"OperationFN",
    L"OpenShift",
    L"CloseShift",
    L"ProcessCheck",
    L"ProcessCorrectionCheck",
    L"PrintTextDocument",
    L"CashInOutcome",
    L"PrintXReport",
    L"PrintCheckCopy",
    L"GetCurrentStatus",
    L"ReportCurrentStatusOfSettlements",
    L"OpenCashDrawer",
    L"GetLineLength",
    L"GetInterfaceRevision",
    L"GetDescription",
    L"GetLastError",
    L"GetParameters",
    L"SetParameter",
    L"Open",
    L"Close",
    L"DeviceTest",
    L"GetAdditionalActions",
    L"DoAdditionalAction"
};

static const wchar_t* g_PropNamesRu[] = {
    L"Порт",
    L"Скорость",
    L"ПисатьЛог",
    L"ИмяФайла"
};
static const wchar_t* g_MethodNamesRu[] = {
    L"ПоказатьСообщение",
    L"ПолучитьПараметрыККТ",
    L"ОперацияФН",
    L"ОткрытьСмену",
    L"ЗакрытьСмену",
    L"СформироватьЧек",
    L"СформироватьЧекКоррекции",
    L"НапечататьТекстовыйДокумент",  
    L"НапечататьЧекВнесенияВыемки",
    L"НапечататьОтчетБезГашения",
    L"НапечататьКопиюЧека",
    L"ПолучитьТекущееСостояние",
    L"ОтчетОТекущемСостоянииРасчетов",
    L"ОткрытьДенежныйЯщик",
    L"ПолучитьШиринуСтроки",
    L"ПолучитьРевизиюИнтерфейса",
    L"ПолучитьОписание",
    L"ПолучитьОшибку",
    L"ПолучитьПараметры",
    L"УстановитьПараметр",
    L"Подключить",
    L"Отключить",
    L"ТестУстройства",
    L"ПолучитьДополнительныеДействия",
    L"ВыполнитьДополнительноеДействие"
};

static const wchar_t g_kClassNames[] = L"CAddInNative"; //|OtherClass1|OtherClass2";
static AppCapabilities g_capabilities = eAppCapabilitiesInvalid;

uint32_t convToShortWchar(WCHAR_T** Dest, const wchar_t* Source, uint32_t len = 0);
uint32_t convFromShortWchar(wchar_t** Dest, const WCHAR_T* Source, uint32_t len = 0);
uint32_t copyWCHAR_T(WCHAR_T** Dest, const WCHAR_T* Source, uint32_t len = 0);
uint32_t getLenShortWcharStr(const WCHAR_T* Source);

//---------------------------------------------------------------------------//
long GetClassObject(const WCHAR_T * wsName, IComponentBase ** pInterface)
{   
    if(!*pInterface)
    {
        *pInterface= new CAddInNative();
        return (long)*pInterface;
    }
    return 0;
}
//---------------------------------------------------------------------------//
long DestroyObject(IComponentBase** pIntf)
{
   if(!*pIntf)
      return -1;

   delete *pIntf;
   *pIntf = 0;
   return 0;
}
//---------------------------------------------------------------------------//
const WCHAR_T* GetClassNames()
{
    static WCHAR_T* names = 0;
    if (!names)
        ::convToShortWchar(&names, g_kClassNames);
    return names;
}

AppCapabilities SetPlatformCapabilities(const AppCapabilities capabilities)
{
    g_capabilities = capabilities;
    return eAppCapabilitiesLast;
}

//---------------------------------------------------------------------------//
//CAddInNative
CAddInNative::CAddInNative()
{

    m_iMemory = 0;
    m_iConnect = 0;
    addInParam = _1cv8::CTableParameters();
    lastError = AddInError{ 0, L"нет ошибок" };

}
//---------------------------------------------------------------------------//
CAddInNative::~CAddInNative()
{
}
//---------------------------------------------------------------------------//
bool CAddInNative::Init(void* pConnection)
{ 
    m_iConnect = (IAddInDefBase*)pConnection;
    return m_iConnect != NULL;
}
//---------------------------------------------------------------------------//
long CAddInNative::GetInfo()
{ 
    return 2000; 
}
//---------------------------------------------------------------------------//
void CAddInNative::Done()
{
}
//---------------------------------------------------------------------------//
bool CAddInNative::RegisterExtensionAs(WCHAR_T** wsLanguageExt)
{ 
    const wchar_t* wsExtension = L"PiritKKT";
    int iActualSize = ::wcslen(wsExtension) + 1;
    WCHAR_T* dest = 0;

    if (m_iMemory)
    {
        if (m_iMemory->AllocMemory((void**)wsLanguageExt, iActualSize * sizeof(WCHAR_T)))
            ::convToShortWchar(wsLanguageExt, wsExtension, iActualSize);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------//
long CAddInNative::GetNProps()
{ 
    return eLastProp;
}
//---------------------------------------------------------------------------//
long CAddInNative::FindProp(const WCHAR_T* wsPropName)
{ 
    long plPropNum = -1;
    wchar_t* propName = 0;

    ::convFromShortWchar(&propName, wsPropName);
    plPropNum = getIndexInArr(g_PropNames, propName, eLastProp);

    if (plPropNum == -1)
        plPropNum = getIndexInArr(g_PropNamesRu, propName, eLastProp);

    delete[] propName;

    return plPropNum;
}
//---------------------------------------------------------------------------//
const WCHAR_T* CAddInNative::GetPropName(long lPropNum, long lPropAlias)
{ 
    if (lPropNum >= eLastProp)
        return NULL;

    wchar_t* wsCurrentName = NULL;
    WCHAR_T* wsPropName = NULL;
    int iActualSize = 0;

    switch (lPropAlias)
    {
    case 0: // First language
        wsCurrentName = (wchar_t*)g_PropNames[lPropNum];
        break;
    case 1: // Second language
        wsCurrentName = (wchar_t*)g_PropNamesRu[lPropNum];
        break;
    default:
        return 0;
    }

    iActualSize = wcslen(wsCurrentName) + 1;

    if (m_iMemory && wsCurrentName)
    {
        if (m_iMemory->AllocMemory((void**)&wsPropName, iActualSize * sizeof(WCHAR_T)))
            ::convToShortWchar(&wsPropName, wsCurrentName, iActualSize);
    }

    return wsPropName;
}
//---------------------------------------------------------------------------//
bool CAddInNative::CreateVarFromWchar(tVariant* var, WCHAR_T* str) {
    uint32_t iActualSize = getLenShortWcharStr(str)+1;
    if (m_iMemory->AllocMemory((void**)&var->pwstrVal, iActualSize * sizeof(WCHAR_T)))
    {
        copyWCHAR_T(&var->pwstrVal, str, iActualSize);
        var->wstrLen = iActualSize-1; //не учитываем терминальный символ
        TV_VT(var) = VTYPE_PWSTR;
        return true;
    }
    return false;
}

bool CAddInNative::CreateVarFromString(tVariant* var, wstring str) {
    WCHAR_T* wstr = nullptr;
    convToShortWchar(&wstr, str.c_str());
    return CAddInNative::CreateVarFromWchar(var, wstr);
}

bool CAddInNative::GetPropVal(const long lPropNum, tVariant* pvarPropVal)
{ 
    switch (lPropNum)
    {
    /*case ePort: {
        return CreateVarFromWchar(pvarPropVal, L"test");

    }*/
    /*case eInt:
        
        TV_VT(pvarPropVal) = VTYPE_I4;
        TV_I1(pvarPropVal) = Intt;

        return true;*/

    default:
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------//
bool CAddInNative::SetPropVal(const long lPropNum, tVariant* varPropVal)
{ 
    switch (lPropNum)
    {
    case ePort:
        if (TV_VT(varPropVal) != VTYPE_PWSTR)
            return false;
        //Text = TV_WSTR(varPropVal);
        //copyWCHAR_T(&Text, TV_WSTR(varPropVal));
        return true;
    /*case eInt:
        if (TV_VT(varPropVal) != VTYPE_I4)
            return false;
        Intt = TV_I1(varPropVal);
        return true;*/
    default:
        return false;
    }

    return false;
}
//---------------------------------------------------------------------------//
bool CAddInNative::IsPropReadable(const long lPropNum)
{ 
    return true;
}
//---------------------------------------------------------------------------//
bool CAddInNative::IsPropWritable(const long lPropNum)
{
    return true;
}
//---------------------------------------------------------------------------//
long CAddInNative::GetNMethods()
{ 
    return eLastMethod;
}
//---------------------------------------------------------------------------//
long CAddInNative::FindMethod(const WCHAR_T* wsMethodName)
{ 
    long plMethodNum = -1;
    wchar_t* name = 0;

    ::convFromShortWchar(&name, wsMethodName);

    plMethodNum = getIndexInArr(g_MethodNames, name, eLastMethod);

    if (plMethodNum == -1)
        plMethodNum = getIndexInArr(g_MethodNamesRu, name, eLastMethod);

    delete[] name;

    return plMethodNum;
}
//---------------------------------------------------------------------------//
const WCHAR_T* CAddInNative::GetMethodName(const long lMethodNum, 
                            const long lMethodAlias)
{ 
    if (lMethodNum >= eLastMethod)
        return NULL;

    wchar_t* wsCurrentName = NULL;
    WCHAR_T* wsMethodName = NULL;
    int iActualSize = 0;

    switch (lMethodAlias)
    {
    case 0: // First language
        wsCurrentName = (wchar_t*)g_MethodNames[lMethodNum];
        break;
    case 1: // Second language
        wsCurrentName = (wchar_t*)g_MethodNamesRu[lMethodNum];
        break;
    default:
        return 0;
    }

    iActualSize = wcslen(wsCurrentName) + 1;

    if (m_iMemory && wsCurrentName)
    {
        if (m_iMemory->AllocMemory((void**)&wsMethodName, iActualSize * sizeof(WCHAR_T)))
            ::convToShortWchar(&wsMethodName, wsCurrentName, iActualSize);
    }

    return wsMethodName;
}
//---------------------------------------------------------------------------//
long CAddInNative::GetNParams(const long lMethodNum)
{ 
    switch (lMethodNum)
    {
    case eMethShowMsgBox:
        return 0;
    case eGetDataKKT:
        return 2;
    case eOperationFN:
        return 3;
    case eOpenShift:
        return 3;
    case eCloseShift:
        return 3;
    case eProcessCheck:
        return 4;
    case eProcessCorrectionCheck:
        return 3;
    case ePrintTextDocument:
        return 2;
    case eCashInOutcome:
        return 3;
    case ePrintXReport:
        return 2;
    case ePrintCheckCopy:
        return 2;
    case eGetCurrentStatus:
        return 3;
    case eReportCurrentStatusOfSettlements:
        return 3;
    case eOpenCashDrawer:
        return 1;
    case eGetLineLength:
        return 2;
    case eGetInterfaceRevision:
        return 0;
    case eGetDescription:
    case eGetLastError: 
    case eGetParameters:
    case eOpen:
    case eClose:
    case eGetAdditionalActions:
    case eDoAdditionalAction:
        return 1;
    case eSetParameter:
    case eDeviceTest:
        return 2;
    default:
        return 0;
    }

    return 0;
}
//---------------------------------------------------------------------------//
bool CAddInNative::GetParamDefValue(const long lMethodNum, const long lParamNum,
                          tVariant *pvarParamDefValue)
{ 
    TV_VT(pvarParamDefValue) = VTYPE_EMPTY;
    return false;
} 
//---------------------------------------------------------------------------//
bool CAddInNative::HasRetVal(const long lMethodNum)
{ 
    switch (lMethodNum)
    {
    case eMethShowMsgBox:
        return false;
    default:
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------//
bool CAddInNative::CallAsProc(const long lMethodNum,
                    tVariant* paParams, const long lSizeArray)
{ 
    switch (lMethodNum)
    {
    case eMethShowMsgBox:
        if (eAppCapabilities1 <= g_capabilities)
        {
            IAddInDefBaseEx* cnn = (IAddInDefBaseEx*)m_iConnect;
            IMsgBox* imsgbox = (IMsgBox*)cnn->GetInterface(eIMsgBox);
            if (imsgbox)
            {
                //imsgbox->Alert(L"alert");
            }
        }
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------//
bool CAddInNative::CallAsFunc(const long lMethodNum,
                tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{ 
    switch (lMethodNum)
    {
    case eGetDataKKT:
    {
        if (lSizeArray != 2 || !paParams)
            return false;

        if (TV_VT(paParams) != VTYPE_PWSTR) {
            return false;
        }

        /*auto kkt_data = kkt.GetDataKKT();
        if (!kkt_data.result) {
            lastError = AddInError{ 1, L"Error #" + kkt_data.kod };
            TV_VT(pvarRetValue) = VTYPE_BOOL;
            TV_BOOL(pvarRetValue) = false;
            return true;
        }
        _1cv8::CTableParametersKKT outtable(kkt_data.data);*/
        _1cv8::CTableParametersKKT outtable = _1cv8::CTableParametersKKT();
        auto result = outtable.toXML();
        if (kkt.toFile) {
            kkt.Log("eGetDataKKT");
            kkt.Log(result);
        }

        tVariant* outParams = paParams + 1;
        CreateVarFromString(outParams, result);

        TV_VT(pvarRetValue) = VTYPE_BOOL;
        TV_BOOL(pvarRetValue) = true;
    }

    return true;
    case eOperationFN:
    {
        //ничего не будем делать
        if (kkt.toFile) {
            kkt.Log("eOperationFN");
        }
        TV_VT(pvarRetValue) = VTYPE_BOOL;
        TV_BOOL(pvarRetValue) = true;
    }
    return true;
    case eOpenShift:
    {
        if (lSizeArray != 3 || !paParams)
            return false;
        auto paParams2 = paParams + 1;
        auto paParams3 = paParams + 2;
        
        //из-за разного размера wchar_t (wstring) в Linux (4 байта) и Windows (2 байта) необходимо приводить возврат 1с (2 байта) в нативную разрядность
        wchar_t* wParam = nullptr;
        convFromShortWchar(&wParam, TV_WSTR(paParams2));
        _1cv8::CInputParameters inputParam(wParam);
        
        if (kkt.toFile) {
            kkt.Log("OpenShift");
        }

        kkt.Log(wParam);

        delete wParam;
        
        pirit_answer kkt_data = kkt.StartWork();
        if (!kkt_data.result) {
            lastError = AddInError{ 1, L"Error #" + kkt_data.kod };
            TV_VT(pvarRetValue) = VTYPE_BOOL;
            TV_BOOL(pvarRetValue) = false;
            return true;
        }
        
        kkt_data = kkt.OpenShift(inputParam.CashierName);
        if (!kkt_data.result) {
            lastError = AddInError{ 1, L"Error #" + kkt_data.kod };
            TV_VT(pvarRetValue) = VTYPE_BOOL;
            TV_BOOL(pvarRetValue) = false;
            return true;
        }
        _1cv8::COutputParameters outtable = _1cv8::COutputParameters();
        CreateVarFromString(paParams3, outtable.toXML());

        TV_VT(pvarRetValue) = VTYPE_BOOL;
        TV_BOOL(pvarRetValue) = true;
    }
    return true;
    case eCloseShift:
    {
        if (lSizeArray != 3 || !paParams)
            return false;
        auto paParams2 = paParams + 1;
        auto paParams3 = paParams + 2;

        wchar_t* wParam = nullptr;
        convFromShortWchar(&wParam, TV_WSTR(paParams2));
        _1cv8::CInputParameters inputParam(wParam);
        
        if (kkt.toFile) {
            kkt.Log("CloseShift");
            kkt.Log(wParam);
        }

        
        delete wParam;

        auto kkt_data = kkt.CloseShift(inputParam.CashierName);
        if (!kkt_data.result) {
            lastError = AddInError{ 1, L"Error #" + kkt_data.kod };
            TV_VT(pvarRetValue) = VTYPE_BOOL;
            TV_BOOL(pvarRetValue) = false;
            return true;
        }

        _1cv8::COutputParameters outtable = _1cv8::COutputParameters();
        auto resul = outtable.toXML();
        kkt.Log(resul);
        CreateVarFromString(paParams3, resul);

        TV_VT(pvarRetValue) = VTYPE_BOOL;
        TV_BOOL(pvarRetValue) = true;
    }
    return true;
    case eProcessCheck:
    {
        if (lSizeArray != 4 || !paParams)
            return false;
        auto paParams2 = paParams + 1;
        auto paParams3 = paParams + 2;
        auto paParams4 = paParams + 3;

        wchar_t* wParam = nullptr;
        convFromShortWchar(&wParam, TV_WSTR(paParams3));
        _1cv8::CCheckPackage inputParam(wParam);
        kkt.Log("ProcessCheck");
        kkt.Log(wParam);
        delete wParam;
        
        _1cv8::CDocumentOutputParameters out = _1cv8::CDocumentOutputParameters();
        bool result = PrintCheck(inputParam, out);
        if (!result) {
            //lastError = AddInError{ 0, L"PrintCheck error" };
            TV_VT(pvarRetValue) = VTYPE_BOOL;
            TV_BOOL(pvarRetValue) = false;
            return true;
        }
        auto outresult = out.toXML();
        kkt.Log(outresult);
        CreateVarFromString(paParams4, outresult);

        TV_VT(pvarRetValue) = VTYPE_BOOL;
        TV_BOOL(pvarRetValue) = true;
    }
    return true;
    case eProcessCorrectionCheck:
        kkt.Log("ProcessCorrectionCheck");
        lastError = AddInError{ 0, L"ProcessCorrectionCheck not implement" };
        TV_VT(pvarRetValue) = VTYPE_BOOL;
        TV_BOOL(pvarRetValue) = false;
        return true;

    case ePrintTextDocument:
        kkt.Log("PrintTextDocument");
        lastError = AddInError{ 0, L"ePrintTextDocument not implement" };
        TV_VT(pvarRetValue) = VTYPE_BOOL;
        TV_BOOL(pvarRetValue) = false;
        return true;

    case eCashInOutcome:
        kkt.Log("CashInOutcome");
        lastError = AddInError{ 0, L"eCashInOutcome not implement" };
        TV_VT(pvarRetValue) = VTYPE_BOOL;
        TV_BOOL(pvarRetValue) = false;
        return true;

    case ePrintXReport: {
        if (lSizeArray != 2 || !paParams)
            return false;
        auto paParams2 = paParams + 1;

        wchar_t* wParam = nullptr;
        convFromShortWchar(&wParam, TV_WSTR(paParams2));
        _1cv8::CInputParameters inputParam(wParam);
        delete wParam;

        auto kkt_data = kkt.PrintXReport(inputParam.CashierName);
        if (!kkt_data.result) {
            lastError = AddInError{ 1, L"Error #" + kkt_data.kod };
            TV_VT(pvarRetValue) = VTYPE_BOOL;
            TV_BOOL(pvarRetValue) = false;
            return true;
        }

        TV_VT(pvarRetValue) = VTYPE_BOOL;
        TV_BOOL(pvarRetValue) = true;

        return true;
    }
    case ePrintCheckCopy:
        lastError = AddInError{ 0, L"ePrintCheckCopy not implement" };
        TV_VT(pvarRetValue) = VTYPE_BOOL;
        TV_BOOL(pvarRetValue) = false;
        return true;

    case eGetCurrentStatus:
    {
        if (lSizeArray != 3 || !paParams)
            return false;
        auto paParams2 = paParams + 1;
        auto paParams3 = paParams + 2;

        wchar_t* wParam = nullptr;
        convFromShortWchar(&wParam, TV_WSTR(paParams2));
        _1cv8::CInputParameters inputParam(wParam);
        delete wParam;

        //TODO: необходимо получать номер смены и номер чека
        _1cv8::COutputParameters outtable = _1cv8::COutputParameters();
        CreateVarFromString(paParams3, outtable.toXML());

        TV_VT(pvarRetValue) = VTYPE_BOOL;
        TV_BOOL(pvarRetValue) = true;
    }
        return true;

    case eReportCurrentStatusOfSettlements:
        lastError = AddInError{ 0, L"eReportCurrentStatusOfSettlements not implement" };
        TV_VT(pvarRetValue) = VTYPE_BOOL;
        TV_BOOL(pvarRetValue) = false;
        return true;

    case eOpenCashDrawer:
        lastError = AddInError{ 0, L"eOpenCashDrawer not implement" };
        TV_VT(pvarRetValue) = VTYPE_BOOL;
        TV_BOOL(pvarRetValue) = false;
        return true;

    case eGetLineLength:
    {
        tVariant* outParams = paParams + 1;
        
        //TV_VT(outParams) = VTYPE_I8;
        TV_I4(outParams) = 36;

        //if (m_iMemory->AllocMemory((void**)&outParams, sizeof(LONG64)))
        //{
        //    TV_VT(outParams) = VTYPE_I8;
        //    TV_I8(outParams) = 36;
        //}

        TV_VT(pvarRetValue) = VTYPE_BOOL;
        TV_BOOL(pvarRetValue) = true;
    }
        return true;

    case eGetInterfaceRevision:
        TV_VT(pvarRetValue) = VTYPE_I4;
        TV_I4(pvarRetValue) = 3001;
        return true;

    case eGetDescription:
    {
        if (lSizeArray != 1 || !paParams)
            return false;

        _1cv8::CDriverDescription outtable = _1cv8::CDriverDescription();
        CreateVarFromString(paParams, outtable.toXML());
        TV_VT(pvarRetValue) = VTYPE_BOOL;
        TV_BOOL(pvarRetValue) = true;
    }

    return true;
    case eGetLastError:
    {
        if (lSizeArray != 1 || !paParams)
            return false;

        CreateVarFromString(paParams, lastError.description);
        TV_VT(pvarRetValue) = VTYPE_I4;
        TV_I4(pvarRetValue) = lastError.kod;
    }

    return true;
    case eGetParameters:
    {
        if (lSizeArray != 1 || !paParams)
            return false;
        CreateVarFromString(paParams, addInParam.toXML());
        TV_VT(pvarRetValue) = VTYPE_BOOL;
        TV_BOOL(pvarRetValue) = true;
    }

    return true;

    case eSetParameter:
    {
        if (lSizeArray != 2 || !paParams)
            return false;

        tVariant* paramVal = paParams + 1;

        wchar_t* wParam = nullptr;
        convFromShortWchar(&wParam, TV_WSTR(paParams));
        wstring paramName = wParam;

        //wstring paramName = TV_WSTR(paParams);
        
        if (paramName == L"Port") {
            convFromShortWchar(&wParam, TV_WSTR(paramVal));
            wstring wparamVal = wParam;
            addInParam.Port = wparamVal;
        }
        else if (paramName == L"WriteLog"){
            wstring wparamVal = wParam;
            addInParam.WriteLog = TV_BOOL(paramVal);

        }
        else if (paramName == L"Speed"){
            convFromShortWchar(&wParam, TV_WSTR(paramVal));
            wstring wparamVal = wParam;
            addInParam.Speed = wparamVal;

        }
        else if(paramName == L"LogFileName"){
            convFromShortWchar(&wParam, TV_WSTR(paramVal));
            wstring wparamVal = wParam;
            addInParam.LogFileName = wparamVal;

        }

        delete wParam;
        
        //на будщее, что бы не забыть
        /* for (_1cv8::CParam param: addInParam.parametrs)
         {
             if (param.Name == paramName) {
                 param.DefaultValue = to_wstring(TV_INT(paramVal));
             }
         }*/

        TV_VT(pvarRetValue) = VTYPE_BOOL;
        TV_BOOL(pvarRetValue) = true;
    }

    return true;
    case eOpen:
    {
        if (lSizeArray != 1 || !paParams)
            return false;

        //здесь устанавливаем нужные параметры
        kkt.Connect(addInParam.Port, addInParam.WriteLog ? addInParam.LogFileName : L"");
        CreateVarFromString(paParams, DEVICEID);

        TV_VT(pvarRetValue) = VTYPE_BOOL;
        TV_BOOL(pvarRetValue) = true;
    }

    return true;
    case eClose:
    {
        if (lSizeArray != 1 || !paParams)
            return false;

        kkt.Disconnect();

        TV_VT(pvarRetValue) = VTYPE_BOOL;
        TV_BOOL(pvarRetValue) = true;
    }

    return true;
    case eDeviceTest:
    {
        if (lSizeArray != 2 || !paParams)
            return false;

        CreateVarFromString(paParams + 1, L"");

        auto kkt_data = kkt.Test();
        if (!kkt_data.result) {
            CreateVarFromString(paParams, L"Устройство не отвечает");
        }
        else {
            CreateVarFromString(paParams, L"Устройство подключено");
        }
        
        TV_VT(pvarRetValue) = VTYPE_BOOL;
        TV_BOOL(pvarRetValue) = true;
    }

    return true;
    case eGetAdditionalActions:
    {
        if (lSizeArray != 1 || !paParams)
            return false;

        CreateVarFromString(paParams, LR"delimiter(<?xml version="1.0" encoding="UTF-8"?>
 <Actions>
      <Action Name="TestSetting" Caption="Тестовая копка"/> 
 </Actions>)delimiter");

        TV_VT(pvarRetValue) = VTYPE_BOOL;
        TV_BOOL(pvarRetValue) = true;
    }

    return true;
    case eDoAdditionalAction:
    {
        TV_VT(pvarRetValue) = VTYPE_BOOL;
        TV_BOOL(pvarRetValue) = false;
    }

    return true;
    }

    return false; 
}
//---------------------------------------------------------------------------//
void CAddInNative::SetLocale(const WCHAR_T* loc)
{
#ifndef __linux__
    _wsetlocale(LC_ALL, loc);
#else
    int size = 0;
    char *mbstr = 0;
    wchar_t *tmpLoc = 0;
    convFromShortWchar(&tmpLoc, loc);
    size = wcstombs(0, tmpLoc, 0)+1;
    mbstr = new char[size];

    if (!mbstr)
    {
        delete[] tmpLoc;
        return;
    }

    memset(mbstr, 0, size);
    size = wcstombs(mbstr, tmpLoc, wcslen(tmpLoc));
    setlocale(LC_ALL, mbstr);
    delete[] tmpLoc;
    delete[] mbstr;
#endif
}
//---------------------------------------------------------------------------//
bool CAddInNative::setMemManager(void* mem)
{
    m_iMemory = (IMemoryManager*)mem;
    return m_iMemory != 0;
}
//---------------------------------------------------------------------------//
uint32_t convToShortWchar(WCHAR_T** Dest, const wchar_t* Source, uint32_t len)
{
    if (!len)
        len = ::wcslen(Source)+1;

    if (!*Dest)
        *Dest = new WCHAR_T[len];

    WCHAR_T* tmpShort = *Dest;
    wchar_t* tmpWChar = (wchar_t*) Source;
    uint32_t res = 0;

    ::memset(*Dest, 0, len*sizeof(WCHAR_T));
    do
    {
        *tmpShort++ = (WCHAR_T)*tmpWChar++;
        ++res;
    }
    while (len-- && *tmpWChar);

    return res;
}
//---------------------------------------------------------------------------//
uint32_t convFromShortWchar(wchar_t** Dest, const WCHAR_T* Source, uint32_t len)
{
    if (!len)
        len = getLenShortWcharStr(Source)+1;

    if (*Dest) {
        delete* Dest;
        *Dest = nullptr;
    }

    if (!*Dest)
        *Dest = new wchar_t[len];

    wchar_t* tmpWChar = *Dest;
    WCHAR_T* tmpShort = (WCHAR_T*)Source;
    uint32_t res = 0;

    ::memset(*Dest, 0, len*sizeof(wchar_t));
    do
    {
        *tmpWChar++ = (wchar_t)*tmpShort++;
        ++res;
    }
    while (len-- && *tmpShort);

    return res;
}

uint32_t copyWCHAR_T(WCHAR_T** Dest, const WCHAR_T* Source, uint32_t len)
{
    if (!len)
        len = getLenShortWcharStr(Source) + 1;

    if (!*Dest)
        *Dest = new WCHAR_T[len];

    WCHAR_T* tmpWChar = *Dest;
    WCHAR_T* tmpShort = (WCHAR_T*)Source;
    uint32_t res = 0;

    ::memset(*Dest, 0, len * sizeof(WCHAR_T));
    do
    {
        *tmpWChar++ = (WCHAR_T)*tmpShort++;
        ++res;
    } while (len-- && *tmpShort);

    return res;
}
//---------------------------------------------------------------------------//
uint32_t getLenShortWcharStr(const WCHAR_T* Source)
{
    uint32_t res = 0;
    WCHAR_T *tmpShort = (WCHAR_T*)Source;

    while (*tmpShort++)
        ++res;

    return res;
}
//---------------------------------------------------------------------------//

void CAddInNative::addError(uint32_t wcode, const wchar_t* source,
    const wchar_t* descriptor, long code)
{
    if (m_iConnect)
    {
        WCHAR_T* err = 0;
        WCHAR_T* descr = 0;

        ::convToShortWchar(&err, source);
        ::convToShortWchar(&descr, descriptor);

        m_iConnect->AddError(wcode, err, descr, code);
        delete[] err;
        delete[] descr;
    }
}
//---------------------------------------------------------------------------//
long CAddInNative::getIndexInArr(const wchar_t* names[], const wchar_t* name,
    const uint32_t size) const
{
    long ret = -1;
    for (uint32_t i = 0; i < size; i++)
    {
        if (!wcscmp(names[i], name))
        {
            ret = i;
            break;
        }
    }
    return ret;
}

bool CAddInNative::PrintCheck(_1cv8::CCheckPackage& xmlcheck, _1cv8::CDocumentOutputParameters& out) {
#define CHECK(func) answer = func;if (!answer.result) {lastError = AddInError{ 1, L###func + answer.kod }; return false;}

    pirit_answer answer;

    wstring operationType;
    if (xmlcheck.OperationType == L"1") operationType = L"2"; //чек на продажу
    else if (xmlcheck.OperationType == L"2") operationType = L"3"; //чек на продажу (возврат)
    else if (xmlcheck.OperationType == L"3") operationType = L"6"; //расход ДС
    else if (xmlcheck.OperationType == L"4") operationType = L"7"; //расход ДС (возврат)

    CHECK(kkt.OpenReceipt(operationType, xmlcheck.TaxationSystem, xmlcheck.CashierName));

    for (auto pos : xmlcheck.Positions) {
        wstring VATRate = L"0";
        if (pos.VATRate == L"none") VATRate = L"3";
        else if (pos.VATRate == L"20") VATRate = L"0";
        else if (pos.VATRate == L"10") VATRate = L"1";
        else if (pos.VATRate == L"0") VATRate = L"2";
        else if (pos.VATRate == L"20/120") VATRate = L"4";
        else if (pos.VATRate == L"10/110") VATRate = L"5";

        //size_t SkokaRezat = std::min((size_t)50, pos.Name.size());
        //pos.Name.resize(pos.Name.size() - SkokaRezat);
        CHECK(kkt.AddGoods(pos.Name, pos.Quantity, pos.PriceWithDiscount, pos.DiscountAmount
            , VATRate, pos.PaymentMethod, pos.CalculationSubject));
    }

    CHECK(kkt.Subtotal());
    //0 - наличные
    //1 - безналичные
    //13 - аванс
    //14 - кредит
    //15 - иная
    if (xmlcheck.Payments.Cash != L"0")
        CHECK(kkt.Payment(L"0", xmlcheck.Payments.Cash));
    if (xmlcheck.Payments.ElectronicPayment != L"0")
        CHECK(kkt.Payment(L"1", xmlcheck.Payments.ElectronicPayment));
    if (xmlcheck.Payments.PrePayment != L"0")
        CHECK(kkt.Payment(L"13", xmlcheck.Payments.PrePayment));
    if (xmlcheck.Payments.PostPayment != L"0")
        CHECK(kkt.Payment(L"14", xmlcheck.Payments.PostPayment));
    if (xmlcheck.Payments.Barter != L"0")
        CHECK(kkt.Payment(L"15", xmlcheck.Payments.Barter));

    //LOG(PiritKKT.CancelReceipt());
    CHECK(kkt.CloseReceipt());

    //TODO: перенести в конструктор
	out.CheckNumber = answer.data[0];
	out.ShiftNumber = answer.data[5];
	out.ShiftClosingCheckNumber = answer.data[6];
	out.FiscalSign = answer.data[4];
	out.DateTime = answer.data[7];
	out.DateTime += answer.data[8];
    out.DateTime = _1cv8::ConvertToXMLData(out.DateTime);
    
    return true;
}