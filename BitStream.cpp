#include "comm_inc.h"
#include "BitStream.h"

#define INTEL     1
#define POWER_PC  2
#define MAC_OS    3
#define SUN_SPARC 4

#define MACHINE INTEL

#define in_error (m_inited == FALSE || m_byte_size == -1 || m_curr_index >= m_bs_size)
#define exit_no_init() if (in_error) return -1
#ifndef MACHINE
#error You must define the machine so we know what size to make a byte
#endif
/*
#define gen_byte_val(__N__) (unsigned short)((0xFF) << __N__)
#define gen_byte_mask(__N__) ~(gen_byte_val(__N__))
#define mask_bits_low(__N__)gen_byte_mask(__N__)
#define mask_bits_high(__N__) (0xFF) << (8 - __N__)

#define gen_word_val(__N__) (unsigned short)((0xFFFF) << __N__)
#define gen_word_mask(__N__) ~(gen_word_val(__N__))

#define gen_long_val(__N__)(unsigned long)((0xFFFFFFFF) << __N__)
#define gen_long_mask(__N__) ~(gen_long_val(__N__))

#define is_bit_set(__V__, __N__) (__V__ & (1 << (__N__ - 1)))

*/
CBitStream::CBitStream(void)
: m_curr_index(0L)
, m_bits_used(0L)
, m_bitStream(NULL)
, m_bigEndian(FALSE)
, m_bs_size(0L)
, m_fileStream(NULL)
, m_inited(FALSE)
, m_byte_size(0L)
, m_free_stream(false)
{
  InitByteSize();
}

CBitStream::CBitStream(char * file)
: m_curr_index(0L)
, m_bits_used(0L)
, m_bitStream(NULL)
, m_bigEndian(FALSE)
, m_bs_size(0L)
, m_fileStream(NULL)
, m_inited(FALSE)
, m_byte_size(0L)
, m_free_stream(false)
{
  InitByteSize();
}

CBitStream::~CBitStream(void)
{
  //Cleanup();
}

BOOL CBitStream::OpenStream(char * stream)
{
  if (!stream)
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    m_inited = FALSE;
    goto DONE;
  }
  Cleanup();
  m_fileStream = new CFileStream();
  if (m_fileStream && m_fileStream->Open(stream))
  {
    m_bs_size = m_fileStream->Size();
    if (m_bs_size <= 0)
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      goto DONE;
    }
    m_bitStream = MyAlloc(unsigned char *, m_bs_size);
    if (m_bitStream)
    {
      m_free_stream = true;
      m_inited  = TRUE;
      m_curr_index   = 0L;
      m_bits_used     = 0L;
      m_fileStream->Read(m_bitStream, m_bs_size);
    }
  }
DONE:
  if (!m_inited)
  {
    Cleanup();
  }
  return m_inited;
}

void CBitStream::Cleanup(void)
{
  if (m_fileStream)
  {
    delete m_fileStream;
  }
  if (m_bitStream)//m_free_stream)
  {
    if (m_free_stream)
    {
      MyFree(m_bitStream);
    }
  }
  m_bitStream = NULL;
  m_inited      = FALSE;
  m_curr_index   = 0;
  m_bits_used   = 0;
  m_bs_size      = 0;
}
bool CBitStream::ReadBits(int num_bits, int * pval)
{
  int   bytes_required  = -1;
  int   ret             = 0;
  if (in_error) 
  {
    goto DONE;
  }
  int curr_num_bits = num_bits;
  bytes_required = (num_bits/m_byte_size) + ((num_bits % m_byte_size) != 0); 
  while(bytes_required)
  {
    unsigned char temp = (unsigned char) m_bitStream[m_curr_index];
    unsigned char mask = (unsigned char) (mask_bits_high(curr_num_bits >= m_byte_size ? m_byte_size : (curr_num_bits + m_bits_used)));
    unsigned char shift_val;
    temp = (unsigned char) ((temp  & mask) >> m_bits_used);
    m_bits_used = (curr_num_bits >= m_byte_size) ? 0 : curr_num_bits + m_bits_used;
    m_curr_index += (curr_num_bits >= m_byte_size) ? 1 : 0;
    curr_num_bits -= (curr_num_bits >= m_byte_size) ? m_byte_size : curr_num_bits;
    
    if (curr_num_bits >= m_byte_size)
    {
      shift_val = m_byte_size;
      m_bits_used = 0;
    }
    else
    {
      shift_val  = curr_num_bits;
    }
    ret = (ret | temp) << shift_val;
    bytes_required --;
  }
  *pval = ret;
DONE:  
  return (bytes_required == 0);
}

void CBitStream::SyncByte(void)
{
  if (m_bits_used > 0)
  {
    m_curr_index ++;
    m_bits_used = 0;
  }
}

int CBitStream::SetBuffer(unsigned char  * buff, int size)
{
  Cleanup();
  m_bitStream   = buff;
  m_inited      = TRUE;
  m_curr_index  = 0L;
  m_bits_used   = 0L;
  m_bs_size     = size;
  InitByteSize();
  return 0;
}

void CBitStream::Reset(void)
{
  m_curr_index   = 0;
  m_bits_used   = 0;
}

int CBitStream::MoveTo(int pos)
{
  int ret = m_curr_index;
  m_curr_index = pos;
  return ret;
}

void CBitStream::InitByteSize(void)
{
  switch(MACHINE)
  {
    case INTEL:
    case POWER_PC:
    case MAC_OS:
    case SUN_SPARC:
      m_byte_size = 8;
    break;
    default:
      m_byte_size = -1;
    break;
  }
}
