///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// This file contains the macros that expand into classes and commands for the FTP program, do not modify unless
// you fully understand the real contents of the file
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#define FtpCommandHelp(__F__) \

#define Declare_FtpCommand(__P__, __H__, __E__) \
class CFtpCommand##__P__: public CFtpCommand \
{ \
public :\
  CFtpCommand##__P__::CFtpCommand##__P__()  \
  { \
    m_fLoggedOn = FALSE; \
    m_buffsize = 0xFFFF; \
    m_buffer = MyAlloc(PUCHAR, m_buffsize); \
    m_c2sSize = 0x200;\
    m_Command2Send = MyAlloc(LPTSTR,m_c2sSize); \
    m_userNameLen = 0x200; \
    m_userName = MyAlloc(LPTSTR, m_userNameLen); \
    m_cmdLen = sizeof(#__P__);\
    INIT_STR(m_Command, #__P__, BUFF_SIZE(#__P__))\
    INIT_STR(m_Help, __H__, BUFF_SIZE(#__H__)) \
    INIT_STR(m_Example, __E__, BUFF_SIZE(#__E__)); \
  m_command = __P__; \
  }\
  CFtpCommand##__P__::~CFtpCommand##__P__()  \
  {\
    FREE_STR(m_Command); \
    FREE_STR(m_Help); \
    FREE_STR(m_Example); \
    MyFree(m_userName); \
    MyFree(m_Command2Send); \
    MyFree(m_buffer); \
  }\
  int DoCommand(); \
  int DoCommand(LPTSTR); \
  void ShowCommand() \
  { \
    if (m_Command) \
      printf("%-15s", m_Command);  \
  } \
  void DoHelp() \
  { \
    if (m_Command && m_Help)  \
    { \
      printf("\n%s: %s\n", m_Command, m_Help);  \
      if (m_Example) printf("Example:\n%s\n", m_Example); \
    } \
  } \
};


#define FtpCommandClass(__F__) \
  CFtpCommand##__F__

#define FtpCommandId(__F__) \
  CFtpCommand##__F__##::

#define Declare_FtpCommandClass(__F__) \
  FtpCommandClass(__F__) ftpCommand##__F__

#define ReferenceFtpCommandClass(__F__) \
  ftpCommand##__F__

#define DeclareAndAddFtpCommand(__F__) \
  CFtpCommand##__F__* ftpCommand##__F__## = new CFtpCommand##__F__(); \
  if (ftpCommand##__F__## ) \
  {\
    ftpCommand##__F__##->SetConnection(m_connection);\
    this->add(ftpCommand##__F__, __F__); \
  }


#define BeginIfCommand(__F__, __S__) \
  if (0 == _strnicmp(#__F__,__S__, strlen(#__F__))) return __F__;

#define IsFtpCommand(__F__, __S__) \
  else if (0 == _strnicmp(#__F__,__S__, strlen(#__F__))) return __F__;
#define EndIfCommand else return UNKNOWN;
