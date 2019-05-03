#include "comm_inc.h"
#include "systeminfo.h"
static const LPTSTR __unknown_arch = "Architecture Not Defined";
static const LPTSTR __unknown_proc = "Processor Not Defined";

CHAR * WEEKDAYSA[7] = 
{
    "Sunday",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
};

CHAR * MONTHSA[12] = 
{
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December"
};

WCHAR * WEEKDAYSW[7] = 
{
    L"Sunday",
    L"Monday",
    L"Tuesday",
    L"Wednesday",
    L"Thursday",
    L"Friday",
    L"Saturday",
};

WCHAR * MONTHSW[12] = 
{
    L"January",
    L"February",
    L"March",
    L"April",
    L"May",
    L"June",
    L"July",
    L"August",
    L"September",
    L"October",
    L"November",
    L"December"
};

LPCTSTR CSystemInformation::Architecture()
{
  switch(this->m_architecture)
  {
    case PROCESSOR_ARCHITECTURE_INTEL:
    {
      return arch_name(PROCESSOR_ARCHITECTURE_INTEL);
    }
    break;
    case PROCESSOR_ARCHITECTURE_MIPS :
    {
      return arch_name(PROCESSOR_ARCHITECTURE_MIPS);
    }
    break;
    case PROCESSOR_ARCHITECTURE_UNKNOWN :
    {
      return arch_name(PROCESSOR_ARCHITECTURE_UNKNOWN);
    }
    break;
    case PROCESSOR_ARCHITECTURE_SHX :
    {
      return arch_name(PROCESSOR_ARCHITECTURE_SHX);
    }
    case PROCESSOR_ARCHITECTURE_ARM :
    {
      return arch_name(PROCESSOR_ARCHITECTURE_ARM);
    }
    break;
  default:
  return __unknown_arch;
  }
}

LPCTSTR CSystemInformation::ProcessorType()
{
  switch(m_processorType)
  {
    case PROCESSOR_INTEL_386 :
    {
      return arch_name(PROCESSOR_INTEL_386);
    }
    break;
    
    case PROCESSOR_INTEL_486 :
    {
      return arch_name(PROCESSOR_INTEL_486);
    }
    break;
    
    case PROCESSOR_INTEL_PENTIUM :
    {
      return arch_name(PROCESSOR_INTEL_PENTIUM);
    }
    break;
#ifdef PROCESSOR_INTEL_PENTIUMII
    case PROCESSOR_INTEL_PENTIUMII: 
    {
      return arch_name(PROCESSOR_INTEL_PENTIUMII);
    }
    break;
#endif 
    case PROCESSOR_MIPS_R4000 :
    {
      return arch_name(PROCESSOR_MIPS_R4000);
    }
    break;
  
    case PROCESSOR_HITACHI_SH3 :
    {
      return arch_name(PROCESSOR_HITACHI_SH3);
    }
    break;
  
    case PROCESSOR_HITACHI_SH4 :
    {
      return arch_name(PROCESSOR_HITACHI_SH4);
    }
    break;
  
    case PROCESSOR_STRONGARM :
    {
      return arch_name(PROCESSOR_STRONGARM);
    }
    break;
  
    case PROCESSOR_ARM720 :
    {
      return arch_name(PROCESSOR_ARM720);
    }
    break;
#ifdef PROCESSOR_MIPS_R5000
    case PROCESSOR_MIPS_R5000 :
    {
      return arch_name(PROCESSOR_MIPS_R5000);
    }
    break;
#endif  

#ifdef PROCESSOR_SHx_SH3DSP
    case PROCESSOR_SHx_SH3DSP :
    {
      return arch_name(PROCESSOR_SHx_SH3DSP);
    }
    break;
#endif    
    default:
    return __unknown_proc;
  }
}
    // Date time objects
//

CDateTime::CDateTime()
{
    m_timeA = NULL;
    m_timeW = NULL;
    m_dateA = NULL;
    m_dateW = NULL;
    Init();
}

CDateTime::~CDateTime()
{
    delete_o(m_timeA);
    delete_o(m_timeW);
    delete_o(m_dateA);
    delete_o(m_dateW);
}

LPSTR   CDateTime::GetYearStringA()
{
    CHAR date[MAX_PATH];
    date[0] = 0;
    
    if(m_dateA)
    {
        delete m_dateA;
    }
    
    if (!SUCCEEDED(StringCchPrintfA(date, MAX_PATH, "%d", m_sysTime.wYear)))
    {
        return NULL;
    }
    m_dateA = new TStringA(date);
    return m_dateA->GetString();
}

LPWSTR  CDateTime::GetYearStringW()
{
    WCHAR date[MAX_PATH];
    date[0] = 0;
    
    if(m_dateW)
    {
        delete m_dateW;
    }
    
    if (!SUCCEEDED(StringCchPrintfW(date, MAX_PATH, L"%d", m_sysTime.wYear)))
    {
        return NULL;
    }
    m_dateW = new TStringW(date);
    return m_dateW->GetString();
}


LPSTR CDateTime::GetDateStringA()
{
    CHAR date[MAX_PATH];
    TStringA ts;
    
    //if (!m_dateA)
    //{
    //    m_dateA = new TStringA();
    //}
    GetSystemTime(&m_sysTime);
    if (m_sysTime.wDayOfWeek >= 0 && m_sysTime.wDayOfWeek < 7)
    {
        ts += WEEKDAYSA[m_sysTime.wDayOfWeek];
        ts += " ";
    }
    
    if (m_sysTime.wMonth > 0 && m_sysTime.wMonth <= 12)
    {
        ts += MONTHSA[m_sysTime.wMonth - 1];
        ts += " ";
    }

    date[0] = '\0';
    if (SUCCEEDED(StringCchPrintfA(date, MAX_PATH, "%d", m_sysTime.wDay)))
    {
        ts += date;
        ts += ",";
        ts += " ";
    }
    date[0] = '\0';
    if (SUCCEEDED(StringCchPrintfA(date, MAX_PATH, "%d", m_sysTime.wYear)))
    {
        ts += date;
        ts += " ";
    }
    m_dateA = ts.Clone();
    return m_dateA->GetString();
}

LPWSTR CDateTime::GetDateStringW()
{
    WCHAR date[MAX_PATH];
    TStringW ts;
    
    if (!m_dateW)
    {
        m_dateW = new TStringW();
    }
    
    if (m_sysTime.wDayOfWeek >= 0 && m_sysTime.wDayOfWeek < 7)
    {
        ts += WEEKDAYSW[m_sysTime.wDayOfWeek];
        ts += L" ";
    }
    
    if (m_sysTime.wMonth > 0 && m_sysTime.wMonth <= 12)
    {
        ts += MONTHSW[m_sysTime.wMonth - 1];
        ts += L" ";
    }

    date[0] = L'\0';
    if (SUCCEEDED(StringCchPrintfW(date, MAX_PATH, L"%d", m_sysTime.wDay)))
    {
        ts += date;
        ts += L",";
        ts += L" ";
    }
    date[0] = L'\0';
    if (SUCCEEDED(StringCchPrintfW(date, MAX_PATH, L"%d", m_sysTime.wYear)))
    {
        ts += date;
        ts += L",";
        ts += L" ";
    }
    m_dateW = ts.Clone();
    return m_dateW->GetString();
}

LPSTR CDateTime::GetTimeStringA()
{
    CHAR time[MAX_PATH];
    time[0] = 0;
    UINT size = GetTimeFormat(LOCALE_USER_DEFAULT, LOCALE_NOUSEROVERRIDE, &m_sysTime, NULL, time, MAX_PATH);
    if (m_timeA)
    {
        delete m_timeA;
    }
    m_timeA = new TStringA();
    size += sizeof(CHAR);
    if (!m_timeA->InitEmptyString(size))
    {
        return NULL;
    }
    else
    {
        if (!GetTimeFormat(
            LOCALE_USER_DEFAULT, 
            LOCALE_NOUSEROVERRIDE, 
            &m_sysTime, 
            NULL, 
            m_timeA->GetString(), 
            size)
            )
        {
            return NULL;
        }
    }
    return m_timeA->GetString();
}


void CDateTime::Init()
{
    GetLocalTime(&m_sysTime);
}