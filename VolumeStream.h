#pragma once
#include "stream.h"
#pragma pack(push, 1)

typedef struct _BSNTFS
{
    BYTE        jmp[3];
    CHAR        Name[8];
    // BPB starts here for both NTFS and FAT
    WORD        BytesPerSector;
    BYTE        SectorsPerCluster;
    WORD        ReservedSectors;
    BYTE        bUnused[3];
    WORD        wUnusedByNTFS;
    BYTE        MediaDescriptor;
    BYTE        bUnused2[2];
    WORD        SectorsPerTrack;
    WORD        NumberOfHeads;
    DWORD       HiddenSectors;
    DWORD       dwUnused;
    DWORD       dwUnused2;
    LONGLONG    TotalSectors;
    LONGLONG    MFTClusterNumber;
    LONGLONG    MFTMirrorCluster;
    BYTE        BytesPerFileRecordSegment;
    BYTE        ClustersPerIndexBuffer;
    BYTE        SerialNumber[8];
    DWORD       CheckSum;
    BYTE        BootStrap[426];
    WORD        EOSMarker;
}BSNTFS, *LPBSNTFS;

typedef struct _FAT16BS
{
    BYTE        jmp[3];
    CHAR        Name[8];
    // BIOS Parameter Block
    WORD        BytesPerSector;
    BYTE        SectorsPerCluster;
    WORD        ReservedSectors;
    BYTE        NumFats;
    WORD        RootEntries;
    WORD        SmallSectors;
    BYTE        MediaDescriptor;
    WORD        SectorsPerFat;
    WORD        SectorsPerTrack;
    WORD        NumberOfHeads;
    DWORD       HiddenSectors;
    DWORD       LargeSectors;
    // Extended BIOS Parameter Block
    WORD        PhysicalDriveNumber;
    BYTE        bReserved;
    BYTE        ExtendedBootSignature;
    DWORD       SerialNumber;
    BYTE        Label[11];
    BYTE        FSType[8];
    BYTE        BootStrapCode[448];
    WORD        EOSMarker;
}FAT16BS, *LPFAT16BS;

typedef struct _FAT32BS
{
    BYTE        jmp[3];
    CHAR        Name[8];
    // BIOS Parameter Block
    WORD        BytesPerSector;
    BYTE        SectorsPerCluster;
    WORD        ReservedSectors;
    BYTE        NumFats;
    WORD        RootEntries;
    WORD        SmallSectors;
    BYTE        MediaDescriptor;
    WORD        OldSectorsPerFat;
    WORD        SectorsPerTrack;
    WORD        NumberOfHeads;
    DWORD       HiddenSectors;
    DWORD       LargeSectors;
    // FAT32 extension to BPB starts here
    DWORD       SectorsPerFat;
    WORD        ExtendedFlags;
    WORD        FileSystem;
    DWORD       RootCluster;
    WORD        FSInfoSector;
    WORD        BackUpBootSector;
    BYTE        bReserved[12];
    //Extended FAT32 BPB
    WORD        PhysicalDriveNumber;
    BYTE        bReserved2;
    BYTE        ExtendedBootSignature;
    DWORD       SerialNumber;
    BYTE        Label[11];
    BYTE        FSType[8];
    BYTE        BootCode[420];
    BYTE        EOSMarker[2];
           
}FAT32BS, *LPFAT32BS;

typedef struct _bootsector
{
    union
    {
        FAT16BS fat16;
        FAT32BS fat32;
        BSNTFS  ntfs;
        UCHAR buffer[512];
    };
}BOOTSECTOR, *LPBOOTSECTOR;

typedef struct _MBR
{
    BYTE   jmp[3];
    CHAR    Name[8];
    // BPB starts here for both NTFS and FAT
    WORD    bytesPerSector;
    BYTE    SectorsPerCluster;
    //WORD    reserved1;
    BYTE  extra[498];
}MBR, *LPMBR;
#pragma pack(pop)

class CVolumeStreamA : public CStreamA
{
public:
    CVolumeStreamA();
    ~CVolumeStreamA();    
    BOOL    Open(LPSTR pszStream);
    BOOL    Close(void) ;
    BOOL    Write(PUCHAR pBuffer, DWORD dwSize, DWORD Offset);
    BOOL    Write(PUCHAR pBuffer, DWORD dwSize);
    BOOL    ReadChar(CHAR * pch);
    BOOL    Read(PUCHAR pBuffer, DWORD len);
    BOOL    Read(PUCHAR pBuffer, DWORD len, DWORD position);
    BOOL    ReadEx(PUCHAR pBuffer, DWORD len, LONGLONG position);
    DWORD   SetPosition(DWORD new_position);
    BOOL    SetPositionEx(DWORD lopart, DWORD hipart);
    BOOL    GetTotalSectors(PULONG pLoPart, PULONG pHiPart);
    DWORD   GetClusterSize() { return m_bufferSize; }
    DWORD   GetSectorSize() { return m_sectorSize; }
    LPSTR   Name() { return m_stream_name; }
    BOOL    Exists(LPSTR vol);
    DWORD   SetOpenFlags(DWORD dwFlags) { m_openFlags = dwFlags; }
protected:
    void Init()
    {
        m_currPos       = 0;
        m_pBuffer       = NULL;
        m_fError        = FALSE;
        m_stream_name   = NULL;
        m_handle        = NULL;
        m_openFlags     = GENERIC_READ;
        m_clustBuffer   = NULL;
    }
    DWORD       m_currPos;
    PUCHAR      m_pBuffer;
    BOOL        m_fError;
    LPSTR       m_stream_name;
    DWORD       m_bufferSize;
    PUCHAR      m_clustBuffer;
    DWORD       m_sectorSize;
    HANDLE      m_handle;
    DWORD       m_openFlags;
private:
    MBR         m_mbr;
    BOOTSECTOR  m_bootSector;
};