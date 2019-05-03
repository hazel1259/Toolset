#pragma once
#include "filestream.h"
class CBitStream
{
public:
  CBitStream(void);
  CBitStream(char * stream);
  ~CBitStream(void);
private:
  int             m_curr_index;
  int             m_bits_used;
  unsigned char * m_bitStream;
  BOOL            m_bigEndian;
  int             m_bs_size;
public:
  BOOL OpenStream(char * stream);
private:
  CFileStream *   m_fileStream;
  BOOL m_inited;
public:
  void Cleanup(void);
  bool ReadBits(int num_bits, int * pval);
private:
  int m_byte_size;
public:
  void SyncByte(void);
  int SetBuffer(unsigned char  * buff, int size);
private:
  bool m_free_stream;
public:
  void Reset(void);
  int MoveTo(int pos);
  void InitByteSize(void);
};
