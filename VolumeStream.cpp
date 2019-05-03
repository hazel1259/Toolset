#include "comm_inc.h"
#include "VolumeStream.h"

CVolumeStreamA::CVolumeStreamA()
{
    Init();
}

CVolumeStreamA::~CVolumeStreamA()
{
    Close();
    if (m_clustBuffer)
    {
        Free((PVOID *)&m_clustBuffer);
    }
}   

// this must be executed as Administrator, otherwise it fails.
BOOL  CVolumeStreamA::Open(LPSTR pszStream)
{
    //m_handle = 
    char    fn [30];
    //snprintf (fn, sizeof fn, "\\\\.\\%s", pszStream);
    HRESULT hr = StringCchPrintfA(fn, sizeof(fn), "\\\\.\\%s", pszStream);
    if (FAILED(hr))
    {
        return FALSE;
    }
    m_handle = CreateFile (fn, m_openFlags,
                            FILE_SHARE_READ | FILE_SHARE_WRITE, 
                            NULL,
                            OPEN_EXISTING,
                            FILE_FLAG_NO_BUFFERING | FILE_FLAG_RANDOM_ACCESS,
                            NULL
                            );

    if (m_handle == INVALID_HANDLE_VALUE)
    {
          // show error message and exit
          //DoErrorMessage(GetLastError());
          return FALSE;
    }
    else
    {
        ZeroMemory(&m_mbr, sizeof(MBR));
        if (Read((PUCHAR)&m_mbr, sizeof(MBR)))
        {
            memcpy(&m_bootSector, &m_mbr, sizeof(BOOTSECTOR));
            m_sectorSize = m_mbr.bytesPerSector;
            m_bufferSize = m_mbr.bytesPerSector * m_mbr.SectorsPerCluster;
        }
    }
    return TRUE;
}

BOOL  CVolumeStreamA::Close(void)
{
    if (m_handle == NULL || m_handle == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }
    {
        CloseHandle(m_handle);
    }
    m_currPos = 0;
    m_handle = NULL;
    return TRUE;
}

BOOL CVolumeStreamA::GetTotalSectors(PULONG pLoPart, PULONG pHiPart)
{
    //LARGE_INTEGER li;
    *pLoPart = *pHiPart = 0;
    if (_strnicmp(m_mbr.Name, "NTFS", 4) == 0)
    {
        LPBSNTFS pnt = (LPBSNTFS)&m_mbr;
        LARGE_INTEGER li;
        li.QuadPart = pnt->TotalSectors;
        *pLoPart = li.LowPart;
        *pHiPart = li.HighPart;
        return TRUE;
    }
    
    if (_strnicmp(m_mbr.Name, "FAT16", 5) == 0)
    {
        LPFAT16BS pfat = (LPFAT16BS)&m_mbr;
        if(pfat->SmallSectors == 0)
        {
           *pLoPart = (ULONG)pfat->LargeSectors; 
        }
        else
        {
           *pLoPart = (ULONG)pfat->SmallSectors; 
        }
        return TRUE;
    }
    if (_strnicmp(m_mbr.Name, "FAT32", 5) == 0)
    {
        LPFAT32BS pfat= (LPFAT32BS)&m_mbr;
        *pLoPart = pfat->LargeSectors;
        return TRUE;
    }
    return FALSE;
}
BOOL  CVolumeStreamA::Write(PUCHAR pBuffer, DWORD dwSize, DWORD Offset)
{
    BOOL br = FALSE;
    if (SetPosition(Offset))
    {
        br = Write(pBuffer, dwSize);
    }
    return br;
}

BOOL  CVolumeStreamA::Write(PUCHAR pBuffer, DWORD dwSize)
{
    BOOL br = FALSE;
    if (m_handle != INVALID_HANDLE_VALUE)
    {
        DWORD  bytesRead = 0;
        br = WriteFile(m_handle, pBuffer, dwSize, &bytesRead, NULL);
    }
    return br;
}

BOOL  CVolumeStreamA::ReadChar(CHAR * pch)
{
    return FALSE;
}
// reads len bytes of memory
BOOL  CVolumeStreamA::Read(PUCHAR pBuffer, DWORD len)
{
    BOOL br = FALSE;
    if ((len % m_mbr.bytesPerSector) != 0)
    {
        return FALSE; // buffer lengths must at a minimum be sector aligned
    }
    if (m_handle != INVALID_HANDLE_VALUE)
    {
        DWORD  bytesRead = 0;
        br = ReadFile(m_handle, pBuffer, len, &bytesRead, NULL);
    }
    return br;
}

BOOL  CVolumeStreamA::Read(PUCHAR pBuffer, DWORD len, DWORD position)
{
    BOOL br = FALSE;
    if (SetPosition(position) == FALSE)
    {
        return br;
    }
    if ((len % m_mbr.bytesPerSector) != 0)
    {
        return FALSE; // buffer lengths must at a minimum be sector aligned
    }
    if (m_handle != INVALID_HANDLE_VALUE)
    {
        DWORD  bytesRead = 0;
        br = ReadFile(m_handle, pBuffer, len, &bytesRead, NULL);
    }
    return br;
}
// only allow reads to go up to 2GB for size to read
BOOL  CVolumeStreamA::ReadEx(PUCHAR pBuffer, DWORD len, LONGLONG position)
{
    BOOL br = FALSE;
    LARGE_INTEGER li;
    li.QuadPart = position;
    if ((len % m_mbr.bytesPerSector) != 0)
    {
        return FALSE; // buffer lengths must at a minimum be sector aligned
    }

    if (SetPositionEx(li.LowPart, li.HighPart))
    {
        if (m_handle != INVALID_HANDLE_VALUE)
        {
            DWORD  bytesRead = 0;
            br = ReadFile(m_handle, pBuffer, len, &bytesRead, NULL);
            //br = ReadFile(m_handle, pBuffer, len, NU
        }
    }
    return br;
}

DWORD CVolumeStreamA::SetPosition(DWORD new_position)
{
    if (new_position == -1)
    {
        m_currPos = SetFilePointer(m_handle, 0, NULL, FILE_END);
    }
    else
    {
        m_currPos =  SetFilePointer( m_handle, new_position, NULL, FILE_BEGIN);
    }  
    if (m_currPos == INVALID_SET_FILE_POINTER)
    {
        return FALSE;
    }
    return TRUE;
}

BOOL CVolumeStreamA::SetPositionEx(DWORD lopart, DWORD hipart)
{
    SetLastError(NO_ERROR);
    m_currPos =  SetFilePointer( m_handle, lopart, (PLONG)&hipart, FILE_BEGIN);
    if (m_currPos == INVALID_SET_FILE_POINTER)
    {
        DWORD dwError = GetLastError();
        if (dwError != NO_ERROR)
        {
            return FALSE;
        }
    }
    return TRUE;
}

BOOL CVolumeStreamA::Exists(LPSTR vol)
{
    DWORD dwSize = 0;
    LPSTR pszDrives = NULL;
    BOOL  br = FALSE;
    CHAR drive[4];
    drive[0] = vol[0];
    
    drive[1] = ':';
    drive[2] = '\\';
    drive[3] = '\0';
    dwSize = GetLogicalDriveStrings(dwSize, pszDrives);
    if (dwSize != 0)
    {
        if (Alloc((PVOID *)&pszDrives, dwSize + 1))
        {
            dwSize = GetLogicalDriveStrings(dwSize, pszDrives);
            if (dwSize != 0)
            {
                LPSTR psz = pszDrives;
                while(*psz)
                {
                    if (_stricmp(psz, drive) == 0)
                    {
                        //Free((PVOID *)&pszDrives);
                        br =TRUE;
                        break;
                    }
                    psz += slen(psz) + 1;
                }
                Free((PVOID *)&pszDrives);
            }
        }
    }
    return br;
}