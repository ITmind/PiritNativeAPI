
#ifndef __ADDINNATIVE_H__
#define __ADDINNATIVE_H__

#ifndef __linux__
#include <wtypes.h>
#endif //__linux__

#include "ComponentBase.h"
#include "AddInDefBase.h"
#include "IMemoryManager.h"

///////////////////////////////////////////////////////////////////////////////
// class CAddInNative
class CAddInNative : public IComponentBase
{
public:
    enum Props
    {
        eText,
        eInt,
        eLastProp      // Always last
    };

    enum Methods
    {
        eMethShowMsgBox,
        eGetDataKKT,
        eOperationFN,
        eOpenShift,
        eCloseShift,
        eProcessCheck,
        eProcessCorrectionCheck,
        ePrintTextDocument,
        eCashInOutcome,
        ePrintXReport,
        ePrintCheckCopy,
        eGetCurrentStatus,
        eReportCurrentStatusOfSettlements,
        eOpenCashDrawer,
        eGetLineLength,
        eLastMethod      // Always last
    };

    CAddInNative(void) noexcept;
    virtual ~CAddInNative();
    // IInitDoneBase
    virtual bool ADDIN_API Init(void*);
    virtual bool ADDIN_API setMemManager(void* mem);
    virtual long ADDIN_API GetInfo();
    virtual void ADDIN_API Done();
    // ILanguageExtenderBase
    virtual bool ADDIN_API RegisterExtensionAs(WCHAR_T** wsLanguageExt);
    virtual long ADDIN_API GetNProps();
    virtual long ADDIN_API FindProp(const WCHAR_T* wsPropName) override;
    virtual const WCHAR_T* ADDIN_API GetPropName(long lPropNum, long lPropAlias);
    virtual bool ADDIN_API GetPropVal(const long lPropNum, tVariant* pvarPropVal);
    virtual bool ADDIN_API SetPropVal(const long lPropNum, tVariant* varPropVal);
    virtual bool ADDIN_API IsPropReadable(const long lPropNum);
    virtual bool ADDIN_API IsPropWritable(const long lPropNum);
    virtual long ADDIN_API GetNMethods();
    virtual long ADDIN_API FindMethod(const WCHAR_T* wsMethodName);
    virtual const WCHAR_T* ADDIN_API GetMethodName(const long lMethodNum, 
                            const long lMethodAlias);
    virtual long ADDIN_API GetNParams(const long lMethodNum);
    virtual bool ADDIN_API GetParamDefValue(const long lMethodNum, const long lParamNum,
                            tVariant *pvarParamDefValue);
    virtual bool ADDIN_API HasRetVal(const long lMethodNum);
    virtual bool ADDIN_API CallAsProc(const long lMethodNum,
                    tVariant* paParams, const long lSizeArray);
    virtual bool ADDIN_API CallAsFunc(const long lMethodNum,
                tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);
    operator IComponentBase*() { return (IComponentBase*)this; }
    // LocaleBase
    virtual void ADDIN_API SetLocale(const WCHAR_T* loc);
    bool CreateVarFromWchar(tVariant* var, WCHAR* str);
private:
    long getIndexInArr(const wchar_t* names[], const wchar_t* name, const uint32_t size) const;
    void addError(uint32_t wcode, const wchar_t* source,
        const wchar_t* descriptor, long code);

    // Attributes
    IAddInDefBase* m_iConnect;
    IMemoryManager* m_iMemory;

    wchar_t* Text;
    int8_t Intt;
};
#endif //__ADDINNATIVE_H__
