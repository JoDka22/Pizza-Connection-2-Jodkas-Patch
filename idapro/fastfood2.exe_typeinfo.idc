#define UNLOADED_FILE   1
#include <idc.idc>

extern ltf;  // load_type flags

static main(void)
{
  ltf = ARGV.count > 1 ? ARGV[1] : LOADTYPE_DEFAULT;
  LocalTypes();
}

static LocalTypes_0() 
{
  auto p_type, p_fields, p_cmt, p_fldcmts;

  p_type = "\x0D\x19\x07\x0A\x01\x0A\x01";
  p_fields = "\x0FEnclosingLevel\x0BFilterFunc\x0CHandlerFunc";
  load_type(ltf, 1, "_SCOPETABLE_ENTRY", p_type, p_fields);
  p_type = "\x0A\x01";
  load_type(ltf, 2, "PVOID", p_type);
  p_type = "'";
  load_type(ltf, 3, "DWORD", p_type);
  p_type = "\x0A\x0D\x01\x12_SCOPETABLE_ENTRY";
  load_type(ltf, 4, "PSCOPETABLE_ENTRY", p_type);
  p_type = "\x0D!\x0A\x0D\x01\x1C_EH3_EXCEPTION_REGISTRATION=\x06PVOID=\x12PSCOPET"
    "ABLE_ENTRY=\x06DWORD";
  p_fields = "\x05Next\x11ExceptionHandler\x0BScopeTable\x09TryLevel";
  load_type(ltf, 5, "_EH3_EXCEPTION_REGISTRATION", p_type, p_fields);
  p_type = "\x0D\x01\x1C_EH3_EXCEPTION_REGISTRATION";
  load_type(ltf, 6, "EH3_EXCEPTION_REGISTRATION", p_type);
  p_type = "\x0A\x0D\x01\x1C_EH3_EXCEPTION_REGISTRATION";
  load_type(ltf, 7, "PEH3_EXCEPTION_REGISTRATION", p_type);
  p_type = "\x0D\x19=\x06DWORD\x0A=\x13EXCEPTION_POINTERS\x0D\x01\x1C_EH3_EXCEPTIO"
    "N_REGISTRATION";
  p_fields = "\x08old_esp\x08exc_ptr\x0Dregistration";
  load_type(ltf, 8, "CPPEH_RECORD", p_type, p_fields);
  p_type = "\x0A}\x05CHAR";
  load_type(ltf, 9, "LPCSTR", p_type);
  p_type = "2";
  load_type(ltf, 10, "CHAR", p_type);
  p_type = "\x0A\x0D\x01\x07HKEY__";
  load_type(ltf, 11, "HKEY", p_type);
  p_type = "\x0D\x09\x07";
  p_fields = "\x07unused";
  load_type(ltf, 12, "HKEY__", p_type, p_fields);
  p_type = "\x0A\x0D\x01\x0CHINSTANCE__";
  load_type(ltf, 13, "HINSTANCE", p_type);
  p_type = "\x0D\x09\x07";
  p_fields = "\x07unused";
  load_type(ltf, 14, "HINSTANCE__", p_type, p_fields);
  p_type = "\x0A=\x05CHAR";
  load_type(ltf, 15, "LPSTR", p_type);
  p_type = "\x0D5=\x1CPRTL_CRITICAL_SECTION_DEBUG=\x05LONG=\x05LONG=\x07HANDLE=\x07"
    "HANDLE=\x0AULONG_PTR";
  p_fields = "\x0ADebugInfo\x0ALockCount\x0FRecursionCount\x0DOwningThread\x0ELockSe"
    "maphore\x0ASpinCount";
  load_type(ltf, 16, "_RTL_CRITICAL_SECTION", p_type, p_fields);
  p_type = "\x0A\x0D\x01\x1C_RTL_CRITICAL_SECTION_DEBUG";
  load_type(ltf, 17, "PRTL_CRITICAL_SECTION_DEBUG", p_type);
  p_type = "\x0DI=\x05WORD=\x05WORD\x0A\x0D\x01\x16_RTL_CRITICAL_SECTION=\x0BLIST_"
    "ENTRY=\x06DWORD=\x06DWORD=\x06DWORD=\x05WORD=\x05WORD";
  p_fields = "\x05Type\x16CreatorBackTraceIndex\x10CriticalSection\x11ProcessLocksLi"
    "st\x0BEntryCount\x10ContentionCount\x06Flags\x1ACreatorBackTraceIndexH"
    "igh\x0ASpareWORD";
  load_type(ltf, 18, "_RTL_CRITICAL_SECTION_DEBUG", p_type, p_fields);
  p_type = "#";
  load_type(ltf, 19, "WORD", p_type);
  p_type = "\x0D\x01\x0C_LIST_ENTRY";
  load_type(ltf, 20, "LIST_ENTRY", p_type);
  p_type = "\x0D\x11\x0A\x0D\x01\x0C_LIST_ENTRY\x0A\x0D\x01\x0C_LIST_ENTRY";
  p_fields = "\x06Flink\x06Blink";
  load_type(ltf, 21, "_LIST_ENTRY", p_type, p_fields);
  p_type = "\x07";
  load_type(ltf, 22, "LONG", p_type);
  p_type = "\x0A\x01";
  load_type(ltf, 23, "HANDLE", p_type);
  p_type = "'";
  load_type(ltf, 24, "ULONG_PTR", p_type);
  p_type = "\x0D\x01\x0DtagWNDCLASSA";
  load_type(ltf, 25, "WNDCLASSA", p_type);
  p_type = "\x0DQ=\x05UINT=\x08WNDPROC\x07\x07=\x0AHINSTANCE=\x06HICON=\x08HCURSOR"
    "=\x07HBRUSH=\x07LPCSTR=\x07LPCSTR";
  p_fields = "\x06style\x0ClpfnWndProc\x0BcbClsExtra\x0BcbWndExtra\x0AhInstance\x06h"
    "Icon\x08hCursor\x0EhbrBackground\x0DlpszMenuName\x0ElpszClassName";
  load_type(ltf, 26, "tagWNDCLASSA", p_type, p_fields);
  p_type = "'";
  load_type(ltf, 27, "UINT", p_type);
  p_type = "\x0A\x0CS=\x08LRESULT\x05=\x05HWND=\x05UINT=\x07WPARAM=\x07LPARAM";
  load_type(ltf, 28, "WNDPROC", p_type);
  p_type = "\x0A\x0D\x01\x07HWND__";
  load_type(ltf, 29, "HWND", p_type);
  p_type = "\x0D\x09\x07";
  p_fields = "\x07unused";
  load_type(ltf, 30, "HWND__", p_type, p_fields);
  p_type = "=\x09UINT_PTR";
  load_type(ltf, 31, "WPARAM", p_type);
  p_type = "'";
  load_type(ltf, 32, "UINT_PTR", p_type);
  p_type = "=\x09LONG_PTR";
  load_type(ltf, 33, "LPARAM", p_type);
  p_type = "\x07";
  load_type(ltf, 34, "LONG_PTR", p_type);
  p_type = "=\x09LONG_PTR";
  load_type(ltf, 35, "LRESULT", p_type);
  p_type = "\x0A\x0D\x01\x08HICON__";
  load_type(ltf, 36, "HICON", p_type);
  p_type = "\x0D\x09\x07";
  p_fields = "\x07unused";
  load_type(ltf, 37, "HICON__", p_type, p_fields);
  p_type = "=\x06HICON";
  load_type(ltf, 38, "HCURSOR", p_type);
  p_type = "\x0A\x0D\x01\x09HBRUSH__";
  load_type(ltf, 39, "HBRUSH", p_type);
  p_type = "\x0D\x09\x07";
  p_fields = "\x07unused";
  load_type(ltf, 40, "HBRUSH__", p_type, p_fields);
  p_type = "\x0D!=\x05LONG=\x05LONG=\x05LONG=\x05LONG";
  p_fields = "\x05left\x04top\x06right\x07bottom";
  load_type(ltf, 41, "tagRECT", p_type, p_fields);
  p_type = "\x0D1=\x05HWND=\x05UINT=\x07WPARAM=\x07LPARAM=\x06DWORD=\x06POINT";
  p_fields = "\x05hwnd\x08message\x07wParam\x07lParam\x05time\x03pt";
  load_type(ltf, 42, "tagMSG", p_type, p_fields);
  p_type = "\x0D\x01\x09tagPOINT";
  load_type(ltf, 43, "POINT", p_type);
  p_type = "\x0D\x11=\x05LONG=\x05LONG";
  p_fields = "\x02x\x02y";
  load_type(ltf, 44, "tagPOINT", p_type, p_fields);
  p_type = "\x0DA=\x06DWORD=\x06DWORD=\x07SIZE_T=\x07SIZE_T=\x07SIZE_T=\x07SIZE_T="
    "\x07SIZE_T=\x07SIZE_T";
  p_fields = "\x09dwLength\x0DdwMemoryLoad\x0CdwTotalPhys\x0CdwAvailPhys\x10dwTotalP"
    "ageFile\x10dwAvailPageFile\x0FdwTotalVirtual\x0FdwAvailVirtual";
  load_type(ltf, 45, "_MEMORYSTATUS", p_type, p_fields);
  p_type = "=\x0AULONG_PTR";
  load_type(ltf, 46, "SIZE_T", p_type);
  p_type = "\x0DA=\x05WORD=\x05WORD=\x05WORD=\x05WORD=\x05WORD=\x05WORD=\x05WORD=\x05"
    "WORD";
  p_fields = "\x06wYear\x07wMonth\x0BwDayOfWeek\x05wDay\x06wHour\x08wMinute\x08wSeco"
    "nd\x0EwMilliseconds";
  load_type(ltf, 47, "_SYSTEMTIME", p_type, p_fields);
  p_type = "\x0DQ=\x06DWORD=\x09FILETIME=\x09FILETIME=\x09FILETIME=\x06DWORD=\x06D"
    "WORD=\x06DWORD=\x06DWORD\x1B\x85\x02=\x05CHAR\x1B\x0F=\x05CHAR";
  p_fields = "\x11dwFileAttributes\x0FftCreationTime\x11ftLastAccessTime\x10ftLastWr"
    "iteTime\x0EnFileSizeHigh\x0DnFileSizeLow\x0CdwReserved0\x0CdwReserved1"
    "\x0AcFileName\x13cAlternateFileName";
  load_type(ltf, 48, "_WIN32_FIND_DATAA", p_type, p_fields);
  p_type = "\x0D\x01\x0A_FILETIME";
  load_type(ltf, 49, "FILETIME", p_type);
  p_type = "\x0D\x11=\x06DWORD=\x06DWORD";
  p_fields = "\x0EdwLowDateTime\x0FdwHighDateTime";
  load_type(ltf, 50, "_FILETIME", p_type, p_fields);
  p_type = "\x0D\x01\x0C_SYSTEMTIME";
  load_type(ltf, 51, "SYSTEMTIME", p_type);
  p_type = "=\x16PRTL_CRITICAL_SECTION";
  load_type(ltf, 52, "LPCRITICAL_SECTION", p_type);
  p_type = "\x0A\x0D\x01\x16_RTL_CRITICAL_SECTION";
  load_type(ltf, 53, "PRTL_CRITICAL_SECTION", p_type);
  p_type = "\x0A\x0D\x01\x08tagRECT";
  load_type(ltf, 54, "LPRECT", p_type);
  p_type = "\x0D\xBA\x01=\x06DWORD=\x05HWND=\x0AHINSTANCE=\x07LPCSTR=\x06LPSTR=\x06"
    "DWORD=\x06DWORD=\x06LPSTR=\x06DWORD=\x06LPSTR=\x06DWORD=\x07LPCSTR=\x07"
    "LPCSTR=\x06DWORD=\x05WORD=\x05WORD=\x07LPCSTR=\x07LPARAM=\x0ELPOFNHOOK"
    "PROC=\x07LPCSTR\x0A\x01=\x06DWORD=\x06DWORD";
  p_fields = "\x0ClStructSize\x0AhwndOwner\x0AhInstance\x0ClpstrFilter\x12lpstrCusto"
    "mFilter\x0FnMaxCustFilter\x0DnFilterIndex\x0AlpstrFile\x09nMaxFile\x0F"
    "lpstrFileTitle\x0EnMaxFileTitle\x10lpstrInitialDir\x0BlpstrTitle\x06Fl"
    "ags\x0CnFileOffset\x0FnFileExtension\x0ClpstrDefExt\x0AlCustData\x09lp"
    "fnHook\x0FlpTemplateName\x0BpvReserved\x0BdwReserved\x08FlagsEx";
  load_type(ltf, 55, "tagOFNA", p_type, p_fields);
  p_type = "\x0A\x0CS=\x09UINT_PTR\x05=\x05HWND=\x05UINT=\x07WPARAM=\x07LPARAM";
  load_type(ltf, 56, "LPOFNHOOKPROC", p_type);
  p_type = "\x0D1=\x04HDC=\x05BOOL=\x05RECT=\x05BOOL=\x05BOOL\x1B!=\x05BYTE";
  p_fields = "\x04hdc\x07fErase\x08rcPaint\x09fRestore\x0BfIncUpdate\x0CrgbReserved";
  load_type(ltf, 57, "tagPAINTSTRUCT", p_type, p_fields);
  p_type = "\x0A\x0D\x01\x06HDC__";
  load_type(ltf, 58, "HDC", p_type);
  p_type = "\x0D\x09\x07";
  p_fields = "\x07unused";
  load_type(ltf, 59, "HDC__", p_type, p_fields);
  p_type = "\x07";
  load_type(ltf, 60, "BOOL", p_type);
  p_type = "\x0D\x01\x08tagRECT";
  load_type(ltf, 61, "RECT", p_type);
  p_type = "\"";
  load_type(ltf, 62, "BYTE", p_type);
  p_type = "\x1D\x01\x0F_LARGE_INTEGER";
  load_type(ltf, 63, "LARGE_INTEGER", p_type);
  p_type = "\x1D\x19\x0D\x012_LARGE_INTEGER::$837407842DC9087486FDFA5FEB63B74E\x0D"
    "\x012_LARGE_INTEGER::$837407842DC9087486FDFA5FEB63B74E=\x09LONGLONG";
  p_fields = "\x01\x02u\x09QuadPart";
  load_type(ltf, 64, "_LARGE_INTEGER", p_type, p_fields);
  p_type = "\x0D\x11=\x06DWORD=\x05LONG";
  p_fields = "\x08LowPart\x09HighPart";
  load_type(ltf, 65, "_LARGE_INTEGER::$837407842DC9087486FDFA5FEB63B74E", p_type, p_fields);
  p_type = "\x05";
  load_type(ltf, 66, "LONGLONG", p_type);
  p_type = "\x0D\x01\x0EtagBITMAPINFO";
  load_type(ltf, 67, "BITMAPINFO", p_type);
  p_type = "\x0D\x11=\x11BITMAPINFOHEADER\x1B\x02=\x08RGBQUAD";
  p_fields = "\x0AbmiHeader\x0AbmiColors";
  load_type(ltf, 68, "tagBITMAPINFO", p_type, p_fields);
  p_type = "\x0D\x01\x14tagBITMAPINFOHEADER";
  load_type(ltf, 69, "BITMAPINFOHEADER", p_type);
  p_type = "\x0DY=\x06DWORD=\x05LONG=\x05LONG=\x05WORD=\x05WORD=\x06DWORD=\x06DWOR"
    "D=\x05LONG=\x05LONG=\x06DWORD=\x06DWORD";
  p_fields = "\x07biSize\x08biWidth\x09biHeight\x09biPlanes\x0BbiBitCount\x0EbiCompr"
    "ession\x0CbiSizeImage\x10biXPelsPerMeter\x10biYPelsPerMeter\x0AbiClrUs"
    "ed\x0FbiClrImportant";
  load_type(ltf, 70, "tagBITMAPINFOHEADER", p_type, p_fields);
  p_type = "\x0D\x01\x0BtagRGBQUAD";
  load_type(ltf, 71, "RGBQUAD", p_type);
  p_type = "\x0D!=\x05BYTE=\x05BYTE=\x05BYTE=\x05BYTE";
  p_fields = "\x08rgbBlue\x09rgbGreen\x07rgbRed\x0CrgbReserved";
  load_type(ltf, 72, "tagRGBQUAD", p_type, p_fields);
  p_type = "\x0A\x01";
  load_type(ltf, 73, "HGDIOBJ", p_type);
  p_type = "=\x05GUID";
  load_type(ltf, 74, "IID", p_type);
  p_type = "\x0D\x01\x06_GUID";
  load_type(ltf, 75, "GUID", p_type);
  p_type = "\x0D!'##\x1B\x09\"";
  p_fields = "\x06Data1\x06Data2\x06Data3\x06Data4";
  load_type(ltf, 76, "_GUID", p_type, p_fields);
  p_type = "\x0A\x0D\x01\x08tagOFNA";
  load_type(ltf, 77, "LPOPENFILENAMEA", p_type);
  p_type = "\x0A=\x11EXCEPTION_RECORD";
  load_type(ltf, 78, "PEXCEPTION_RECORD", p_type);
  p_type = "\x0D\x01\x12_EXCEPTION_RECORD";
  load_type(ltf, 79, "EXCEPTION_RECORD", p_type);
  p_type = "\x0D1=\x06DWORD=\x06DWORD\x0A\x0D\x01\x12_EXCEPTION_RECORD=\x06PVOID=\x06"
    "DWORD\x1B\x10=\x0AULONG_PTR";
  p_fields = "\x0EExceptionCode\x0FExceptionFlags\x10ExceptionRecord\x11ExceptionAdd"
    "ress\x11NumberParameters\x15ExceptionInformation";
  load_type(ltf, 80, "_EXCEPTION_RECORD", p_type, p_fields);
  p_type = "\x0A\x01";
  load_type(ltf, 81, "LPVOID", p_type);
  p_type = "\x0D\x91\x01=\x06DWORD=\x06LPSTR=\x06LPSTR=\x06LPSTR=\x06DWORD=\x06DWO"
    "RD=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD=\x05WOR"
    "D=\x05WORD=\x07LPBYTE=\x07HANDLE=\x07HANDLE=\x07HANDLE";
  p_fields = "\x03cb\x0BlpReserved\x0AlpDesktop\x08lpTitle\x04dwX\x04dwY\x08dwXSize\x08"
    "dwYSize\x0EdwXCountChars\x0EdwYCountChars\x10dwFillAttribute\x08dwFlag"
    "s\x0CwShowWindow\x0CcbReserved2\x0ClpReserved2\x0AhStdInput\x0BhStdOut"
    "put\x0AhStdError";
  load_type(ltf, 82, "_STARTUPINFOA", p_type, p_fields);
  p_type = "\x0A=\x05BYTE";
  load_type(ltf, 83, "LPBYTE", p_type);
  p_type = "\x0AA";
  load_type(ltf, 84, "LPCVOID", p_type);
  p_type = "\x0D\x19=\x06DWORD=\x07LPVOID=\x05BOOL";
  p_fields = "\x08nLength\x15lpSecurityDescriptor\x0FbInheritHandle";
  load_type(ltf, 85, "_SECURITY_ATTRIBUTES", p_type, p_fields);
  p_type = "\x0A\x0D\x01\x17_TIME_ZONE_INFORMATION";
  load_type(ltf, 86, "LPTIME_ZONE_INFORMATION", p_type);
  p_type = "\x0D9=\x05LONG\x1B!=\x06WCHAR=\x0BSYSTEMTIME=\x05LONG\x1B!=\x06WCHAR=\x0B"
    "SYSTEMTIME=\x05LONG";
  p_fields = "\x05Bias\x0DStandardName\x0DStandardDate\x0DStandardBias\x0DDaylightNa"
    "me\x0DDaylightDate\x0DDaylightBias";
  load_type(ltf, 87, "_TIME_ZONE_INFORMATION", p_type, p_fields);
  p_type = "=\x08wchar_t";
  load_type(ltf, 88, "WCHAR", p_type);
  p_type = "#";
  load_type(ltf, 89, "wchar_t", p_type);
  p_type = "=\x06DWORD";
  load_type(ltf, 90, "LCID", p_type);
  p_type = "\x0A=\x06WCHAR";
  load_type(ltf, 91, "LPWSTR", p_type);
  p_type = "\x0A=\x05WORD";
  load_type(ltf, 92, "LPWORD", p_type);
  p_type = "\x0A\x0CS\x07\x01";
  load_type(ltf, 93, "FARPROC", p_type);
  p_type = "\x0D\x19=\x05UINT\x1B\x03=\x05BYTE\x1B\x0D=\x05BYTE";
  p_fields = "\x0CMaxCharSize\x0CDefaultChar\x09LeadByte";
  load_type(ltf, 94, "_cpinfo", p_type, p_fields);
  p_type = "\x0A}\x05CHAR";
  load_type(ltf, 95, "PCNZCH", p_type);
  p_type = "\x0A\x0C3\x07\x01";
  load_type(ltf, 96, "_onexit_t", p_type);
  p_type = "'";
  load_type(ltf, 97, "size_t", p_type);
  p_type = "\x0A\x0C3\x07\x03\x0AA\x0AA";
  load_type(ltf, 98, "_CoreCrtNonSecureSearchSortCompareFunction", p_type);
  p_type = "\x0A2";
  load_type(ltf, 99, "va_list", p_type);
  p_type = "\x0A\x0C3\x01\x02\x0A\x01";
  load_type(ltf, 100, "_beginthread_proc_type", p_type);
  p_type = "\x07";
  load_type(ltf, 101, "intptr_t", p_type);
  p_type = "\x0D9\xF1\x80\x04\x07\x02\x03\x07\x04\x04\x04\x0A\x01\x01\x07\x04\x04\x04"
    "\x0A\x01\x01\x07\x04\x04\x04\x0A\x01\x01";
  p_fields = "\x0CmagicNumber\x09maxState\x0BpUnwindMap\x0BnTryBlocks\x0DpTryBlockMa"
    "p\x0EnIPMapEntries\x0EpIPtoStateMap";
  load_type(ltf, 102, "FuncInfoV1", p_type, p_fields);
  p_type = "\x0D\x11\xF1\x80\x04\x07\x04\x04\x04\x0A\x01\x01";
  p_fields = "\x08toState\x07action";
  load_type(ltf, 103, "UnwindMapEntry", p_type, p_fields);
  p_type = "\x0D\x01\x07_iobuf";
  load_type(ltf, 104, "FILE", p_type);
  p_type = "\x0DA\x0A2\x07\x0A2\x07\x07\x07\x07\x0A2";
  p_fields = "\x05_ptr\x05_cnt\x06_base\x06_flag\x06_file\x09_charbuf\x08_bufsiz\x0A"
    "_tmpfname";
  load_type(ltf, 105, "_iobuf", p_type, p_fields);
  p_type = "\x0D\x01\x01";
  load_type(ltf, 106, "Concurrency::details::SchedulerBase", p_type);
  p_type = "\x0D\x01\x01";
  load_type(ltf, 107, "streambuf", p_type);
  p_type = "\x0D\x01\x01";
  load_type(ltf, 108, "filebuf", p_type);
  p_type = "\x0D\x01\x01";
  load_type(ltf, 109, "istream", p_type);
  p_type = "\x0D\x01\x01";
  load_type(ltf, 110, "ostream", p_type);
  p_type = "\x0D\x01\x01";
  load_type(ltf, 111, "ios", p_type);
  p_type = "\x0D\x01\x01";
  p_cmt = "RTTI std::type_info class (#classinformer)";
  load_type(ltf, 112, "type_info", p_type, "", p_cmt);
  p_type = "\x0D\x01\x01";
  load_type(ltf, 113, "std::locale::facet", p_type);
  p_type = "\x0D\x01\x01";
  load_type(ltf, 114, "Concurrency::details::TaskStack", p_type);
  p_type = "\x0D\x01\x01";
  load_type(ltf, 115, "std::strstreambuf", p_type);
  p_type = "\x0D\x01\x01";
  load_type(ltf, 116, "std::ios_base", p_type);
  p_type = "\x0D\x01\x01";
  load_type(ltf, 117, "ifstream", p_type);
  p_type = "\x0D\x01\x01";
  load_type(ltf, 118, "ofstream", p_type);
  p_type = "\x0D\x19\x04\x04\x04";
  p_fields = "\x06mdisp\x06pdisp\x06vdisp";
  p_cmt = "RTTI Base class descriptor displacement container (#classinformer)";
  load_type(ltf, 119, "_PMD", p_type, p_fields, p_cmt);
  p_type = "\x0D!\x04\x04\x04\x04";
  p_fields = "\x0Asignature\x0Battributes\x0FnumBaseClasses\x0FbaseClassArray";
  p_cmt = "RTTI Class Hierarchy Descriptor (#classinformer)";
  load_type(ltf, 120, "_RTTIClassHierarchyDescriptor", p_type, p_fields, p_cmt);
  p_type = "\x0D!\x04\x04=\x05_PMD\x04";
  p_fields = "\x0FtypeDescriptor\x12numContainedBases\x04pmd\x0Battributes";
  p_cmt = "RTTI Base Class Descriptor (#classinformer)";
  load_type(ltf, 121, "_RTTIBaseClassDescriptor", p_type, p_fields, p_cmt);
  p_type = "\x0D)\x04\x04\x04\x04\x0A=\x1E_RTTIClassHierarchyDescriptor";
  p_fields = "\x0Asignature\x07offset\x09cdOffset\x0FtypeDescriptor\x10classDescript"
    "or";
  p_cmt = "RTTI Complete Object Locator (#classinformer)";
  load_type(ltf, 122, "_RTTICompleteObjectLocator", p_type, p_fields, p_cmt);
  p_type = "-\xFE\x01\x80@AAABAAABAAAAAAAAAAAABB@AAAAAAAAAABAABAAAAAAAAADBDDCBAABA"
    "CBAAAAAfAAAAAAAAAAAAAXAAAAAAAAABAA\x81RA@AAAAAAAB@DAA@AAAAAAAAHAAAAAAA"
    "AAAIAAAAAA\x83H@AAAAAAAAAAAAAA@BAAAAAABAGAAAAAAAAAGAAAADAAFAAABAABAAAA"
    "AADAAlAAAAABHAPB\x9F\xFF\xFF\xFF}BOO_ABAA\\AAAAAAAAAAAAAAAAAAEAAACAAAA"
    "BCYYGA_AO\x81q\x83\xF0@";
  p_fields = "\x08WM_NULL\x0AWM_CREATE\x0BWM_DESTROY\x08WM_MOVE\x08WM_SIZE\x0CWM_ACT"
    "IVATE\x0CWM_SETFOCUS\x0DWM_KILLFOCUS\x0AWM_ENABLE\x0DWM_SETREDRAW\x0BW"
    "M_SETTEXT\x0BWM_GETTEXT\x11WM_GETTEXTLENGTH\x09WM_PAINT\x09WM_CLOSE\x13"
    "WM_QUERYENDSESSION\x08WM_QUIT\x0DWM_QUERYOPEN\x0EWM_ERASEBKGND\x12WM_S"
    "YSCOLORCHANGE\x0EWM_ENDSESSION\x0EWM_SHOWWINDOW\x10WM_WININICHANGE\x11"
    "WM_SETTINGCHANGE\x11WM_DEVMODECHANGE\x0FWM_ACTIVATEAPP\x0EWM_FONTCHANG"
    "E\x0EWM_TIMECHANGE\x0EWM_CANCELMODE\x0DWM_SETCURSOR\x11WM_MOUSEACTIVAT"
    "E\x11WM_CHILDACTIVATE\x0DWM_QUEUESYNC\x11WM_GETMINMAXINFO\x0DWM_PAINTI"
    "CON\x12WM_ICONERASEBKGND\x0EWM_NEXTDLGCTL\x11WM_SPOOLERSTATUS\x0CWM_DR"
    "AWITEM\x0FWM_MEASUREITEM\x0EWM_DELETEITEM\x0EWM_VKEYTOITEM\x0EWM_CHART"
    "OITEM\x0BWM_SETFONT\x0BWM_GETFONT\x0DWM_SETHOTKEY\x0DWM_GETHOTKEY\x11W"
    "M_QUERYDRAGICON\x0FWM_COMPAREITEM\x0DWM_GETOBJECT\x0EWM_COMPACTING\x0E"
    "WM_COMMNOTIFY\x15WM_WINDOWPOSCHANGING\x14WM_WINDOWPOSCHANGED\x09WM_POW"
    "ER\x0CWM_COPYDATA\x11WM_CANCELJOURNAL\x0AWM_NOTIFY\x1AWM_INPUTLANGCHAN"
    "GEREQUEST\x13WM_INPUTLANGCHANGE\x09WM_TCARD\x08WM_HELP\x0FWM_USERCHANG"
    "ED\x10WM_NOTIFYFORMAT\x0FWM_CONTEXTMENU\x11WM_STYLECHANGING\x10WM_STYL"
    "ECHANGED\x11WM_DISPLAYCHANGE\x0BWM_GETICON\x0BWM_SETICON\x0CWM_NCCREAT"
    "E\x0DWM_NCDESTROY\x0EWM_NCCALCSIZE\x0DWM_NCHITTEST\x0BWM_NCPAINT\x0EWM"
    "_NCACTIVATE\x0EWM_GETDLGCODE\x0DWM_SYNCPAINT\x0FWM_NCMOUSEMOVE\x11WM_N"
    "CLBUTTONDOWN\x0FWM_NCLBUTTONUP\x13WM_NCLBUTTONDBLCLK\x11WM_NCRBUTTONDO"
    "WN\x0FWM_NCRBUTTONUP\x13WM_NCRBUTTONDBLCLK\x11WM_NCMBUTTONDOWN\x0FWM_N"
    "CMBUTTONUP\x13WM_NCMBUTTONDBLCLK\x11WM_NCXBUTTONDOWN\x0FWM_NCXBUTTONUP"
    "\x13WM_NCXBUTTONDBLCLK\x09WM_INPUT\x0CWM_KEYFIRST\x0BWM_KEYDOWN\x09WM_"
    "KEYUP\x08WM_CHAR\x0CWM_DEADCHAR\x0EWM_SYSKEYDOWN\x0CWM_SYSKEYUP\x0BWM_"
    "SYSCHAR\x0FWM_SYSDEADCHAR\x0BWM_UNICHAR\x0BWM_KEYLAST\x18WM_IME_STARTC"
    "OMPOSITION\x16WM_IME_ENDCOMPOSITION\x13WM_IME_COMPOSITION\x0FWM_IME_KE"
    "YLAST\x0EWM_INITDIALOG\x0BWM_COMMAND\x0EWM_SYSCOMMAND\x09WM_TIMER\x0BW"
    "M_HSCROLL\x0BWM_VSCROLL\x0CWM_INITMENU\x11WM_INITMENUPOPUP\x0EWM_MENUS"
    "ELECT\x0CWM_MENUCHAR\x0DWM_ENTERIDLE\x11WM_MENURBUTTONUP\x0CWM_MENUDRA"
    "G\x11WM_MENUGETOBJECT\x13WM_UNINITMENUPOPUP\x0FWM_MENUCOMMAND\x11WM_CH"
    "ANGEUISTATE\x11WM_UPDATEUISTATE\x10WM_QUERYUISTATE\x12WM_CTLCOLORMSGBO"
    "X\x10WM_CTLCOLOREDIT\x13WM_CTLCOLORLISTBOX\x0FWM_CTLCOLORBTN\x0FWM_CTL"
    "COLORDLG\x15WM_CTLCOLORSCROLLBAR\x12WM_CTLCOLORSTATIC\x0EWM_MOUSEFIRST"
    "\x0DWM_MOUSEMOVE\x0FWM_LBUTTONDOWN\x0DWM_LBUTTONUP\x11WM_LBUTTONDBLCLK"
    "\x0FWM_RBUTTONDOWN\x0DWM_RBUTTONUP\x11WM_RBUTTONDBLCLK\x0FWM_MBUTTONDO"
    "WN\x0DWM_MBUTTONUP\x11WM_MBUTTONDBLCLK\x0EWM_MOUSEWHEEL\x0FWM_XBUTTOND"
    "OWN\x0DWM_XBUTTONUP\x11WM_XBUTTONDBLCLK\x0FWM_MOUSEHWHEEL\x0DWM_MOUSEL"
    "AST\x10WM_PARENTNOTIFY\x11WM_ENTERMENULOOP\x10WM_EXITMENULOOP\x0CWM_NE"
    "XTMENU\x0AWM_SIZING\x12WM_CAPTURECHANGED\x0AWM_MOVING\x12WM_POWERBROAD"
    "CAST\x10WM_DEVICECHANGE\x0DWM_MDICREATE\x0EWM_MDIDESTROY\x0FWM_MDIACTI"
    "VATE\x0EWM_MDIRESTORE\x0BWM_MDINEXT\x0FWM_MDIMAXIMIZE\x0BWM_MDITILE\x0E"
    "WM_MDICASCADE\x12WM_MDIICONARRANGE\x10WM_MDIGETACTIVE\x0EWM_MDISETMENU"
    "\x11WM_ENTERSIZEMOVE\x10WM_EXITSIZEMOVE\x0DWM_DROPFILES\x12WM_MDIREFRE"
    "SHMENU\x17WM_POINTERDEVICECHANGE\x18WM_POINTERDEVICEINRANGE\x1BWM_POIN"
    "TERDEVICEOUTOFRANGE\x09WM_TOUCH\x13WM_NCPOINTERUPDATE\x11WM_NCPOINTERD"
    "OWN\x0FWM_NCPOINTERUP\x11WM_POINTERUPDATE\x0FWM_POINTERDOWN\x0DWM_POIN"
    "TERUP\x10WM_POINTERENTER\x10WM_POINTERLEAVE\x13WM_POINTERACTIVATE\x19W"
    "M_POINTERCAPTURECHANGED\x13WM_TOUCHHITTESTING\x10WM_POINTERWHEEL\x11WM"
    "_POINTERHWHEEL\x13WM_POINTERROUTEDTO\x15WM_POINTERROUTEDAWAY\x19WM_POI"
    "NTERROUTEDRELEASED\x12WM_IME_SETCONTEXT\x0EWM_IME_NOTIFY\x0FWM_IME_CON"
    "TROL\x17WM_IME_COMPOSITIONFULL\x0EWM_IME_SELECT\x0CWM_IME_CHAR\x0FWM_I"
    "ME_REQUEST\x0FWM_IME_KEYDOWN\x0DWM_IME_KEYUP\x0EWM_MOUSEHOVER\x0EWM_MO"
    "USELEAVE\x10WM_NCMOUSEHOVER\x10WM_NCMOUSELEAVE\x15WM_WTSSESSION_CHANGE"
    "\x10WM_TABLET_FIRST\x0FWM_TABLET_LAST\x0EWM_DPICHANGED\x1BWM_DPICHANGE"
    "D_BEFOREPARENT\x1AWM_DPICHANGED_AFTERPARENT\x14WM_GETDPISCALEDSIZE\x07"
    "WM_CUT\x08WM_COPY\x09WM_PASTE\x09WM_CLEAR\x08WM_UNDO\x10WM_RENDERFORMA"
    "T\x14WM_RENDERALLFORMATS\x14WM_DESTROYCLIPBOARD\x11WM_DRAWCLIPBOARD\x12"
    "WM_PAINTCLIPBOARD\x14WM_VSCROLLCLIPBOARD\x11WM_SIZECLIPBOARD\x13WM_ASK"
    "CBFORMATNAME\x11WM_CHANGECBCHAIN\x14WM_HSCROLLCLIPBOARD\x13WM_QUERYNEW"
    "PALETTE\x15WM_PALETTEISCHANGING\x12WM_PALETTECHANGED\x0AWM_HOTKEY\x09W"
    "M_PRINT\x0FWM_PRINTCLIENT\x0EWM_APPCOMMAND\x10WM_THEMECHANGED\x13WM_CL"
    "IPBOARDUPDATE\x19WM_DWMCOMPOSITIONCHANGED\x19WM_DWMNCRENDERINGCHANGED\x1F"
    "WM_DWMCOLORIZATIONCOLORCHANGED\x1CWM_DWMWINDOWMAXIMIZEDCHANGE\x1AWM_DW"
    "MSENDICONICTHUMBNAIL\"WM_DWMSENDICONICLIVEPREVIEWBITMAP\x15WM_GETTITLE"
    "BARINFOEX\x11WM_HANDHELDFIRST\x10WM_HANDHELDLAST\x0CWM_AFXFIRST\x0BWM_"
    "AFXLAST\x0FWM_PENWINFIRST\x0EWM_PENWINLAST\x08WM_USER\x07WM_APP";
  load_type(ltf, 123, "WM_MSG", p_type, p_fields);
  p_type = "\x0Dq\x0A\x01\x1B=2\x0A=\x13ddraw_window_child=\x05HWND=\x09uint32_t=\x09"
    "uint32_t\x1B%2=\x09uint32_t\x1B\x0D2=\x09uint32_t=\x09uint32_t\x1B\x09"
    "2=\x09uint32_t\x1B\x052";
  p_fields = "\x07vtable\x05tmp2\x06ddraw\x05hwnd\x06_tmp1\x0CbFullscreen\x05tmp3\x07"
    "hasMMX\x05tmp5\x06xLeft\x05xTop\x05tmp6\x0BscreenMode\x05tmp4";
  load_type(ltf, 124, "ddraw_window", p_type, p_fields);
  p_type = "\x0D)=\x0DLPDIRECTDRAW=\x15LPDIRECTDRAWSURFACE7=\x15LPDIRECTDRAWSURFAC"
    "E7=\x15LPDIRECTDRAWSURFACE7=\x15LPDIRECTDRAWSURFACE7";
  p_fields = "\x03pp\x0DddrawSurface\x0BbackBuffer\x0CbackBuffer2\x0EddrawSurface2";
  load_type(ltf, 125, "ddraw_window_child", p_type, p_fields);
  p_type = "'";
  load_type(ltf, 126, "uint32_t", p_type);
  p_type = "\x0A\x0D\x01\x0CIDirectDraw";
  load_type(ltf, 127, "LPDIRECTDRAW", p_type);
  p_type = "\x0D\x09\x0A\x0D\x01\x10IDirectDrawVtbl";
  p_fields = "\x07lpVtbl";
  load_type(ltf, 128, "IDirectDraw", p_type, p_fields);
  p_type = "\x0D\xB9\x01\x0A\x0CS=\x08HRESULT\x04\x0A=\x0CIDirectDrawJ}\x04IID\x0A"
    "=\x07LPVOID\x0A\x0CS=\x06ULONG\x02\x0A=\x0CIDirectDraw\x0A\x0CS=\x06UL"
    "ONG\x02\x0A=\x0CIDirectDraw\x0A\x0CS=\x08HRESULT\x02\x0A=\x0CIDirectDr"
    "aw\x0A\x0CS=\x08HRESULT\x05\x0A=\x0CIDirectDraw=\x06DWORD\x0A=\x14LPDI"
    "RECTDRAWCLIPPER\x0A=\x09IUnknown\x0A\x0CS=\x08HRESULT\x06\x0A=\x0CIDir"
    "ectDraw=\x06DWORD=\x0FLPPALETTEENTRY\x0A=\x14LPDIRECTDRAWPALETTE\x0A=\x09"
    "IUnknown\x0A\x0CS=\x08HRESULT\x05\x0A=\x0CIDirectDraw=\x10LPDDSURFACED"
    "ESC\x0A=\x14LPDIRECTDRAWSURFACE\x0A=\x09IUnknown\x0A\x0CS=\x08HRESULT\x04"
    "\x0A=\x0CIDirectDraw=\x14LPDIRECTDRAWSURFACE\x0A=\x14LPDIRECTDRAWSURFA"
    "CE\x0A\x0CS=\x08HRESULT\x06\x0A=\x0CIDirectDraw=\x06DWORD=\x10LPDDSURF"
    "ACEDESC=\x07LPVOID=\x16LPDDENUMMODESCALLBACK\x0A\x0CS=\x08HRESULT\x06\x0A"
    "=\x0CIDirectDraw=\x06DWORD=\x10LPDDSURFACEDESC=\x07LPVOID=\x19LPDDENUM"
    "SURFACESCALLBACK\x0A\x0CS=\x08HRESULT\x02\x0A=\x0CIDirectDraw\x0A\x0CS"
    "=\x08HRESULT\x04\x0A=\x0CIDirectDraw=\x09LPDDCAPS=\x09LPDDCAPS\x0A\x0C"
    "S=\x08HRESULT\x03\x0A=\x0CIDirectDraw=\x10LPDDSURFACEDESC\x0A\x0CS=\x08"
    "HRESULT\x04\x0A=\x0CIDirectDraw=\x08LPDWORD=\x08LPDWORD\x0A\x0CS=\x08H"
    "RESULT\x03\x0A=\x0CIDirectDraw\x0A=\x14LPDIRECTDRAWSURFACE\x0A\x0CS=\x08"
    "HRESULT\x03\x0A=\x0CIDirectDraw=\x08LPDWORD\x0A\x0CS=\x08HRESULT\x03\x0A"
    "=\x0CIDirectDraw=\x08LPDWORD\x0A\x0CS=\x08HRESULT\x03\x0A=\x0CIDirectD"
    "raw=\x07LPBOOL\x0A\x0CS=\x08HRESULT\x03\x0A=\x0CIDirectDraw\x0A=\x05GU"
    "ID\x0A\x0CS=\x08HRESULT\x02\x0A=\x0CIDirectDraw\x0A\x0CS=\x08HRESULT\x04"
    "\x0A=\x0CIDirectDraw=\x05HWND=\x06DWORD\x0A\x0CS=\x08HRESULT\x05\x0A=\x0C"
    "IDirectDraw=\x06DWORD=\x06DWORD=\x06DWORD\x0A\x0CS=\x08HRESULT\x04\x0A"
    "=\x0CIDirectDraw=\x06DWORD=\x07HANDLE";
  p_fields = "\x0FQueryInterface\x05This\x05riid\x07ppvObj\x07AddRef\x05This\x08Rele"
    "ase\x05This\x08Compact\x05This\x0ECreateClipper\x05This\x01\x01\x01\x0E"
    "CreatePalette\x05This\x01\x01\x01\x01\x0ECreateSurface\x05This\x01\x01"
    "\x01\x11DuplicateSurface\x05This\x01\x01\x11EnumDisplayModes\x05This\x01"
    "\x01\x01\x01\x0DEnumSurfaces\x05This\x01\x01\x01\x01\x11FlipToGDISurfa"
    "ce\x05This\x08GetCaps\x05This\x01\x01\x0FGetDisplayMode\x05This\x01\x0F"
    "GetFourCCCodes\x05This\x01\x01\x0EGetGDISurface\x05This\x01\x14GetMoni"
    "torFrequency\x05This\x01\x0CGetScanLine\x05This\x01\x17GetVerticalBlan"
    "kStatus\x05This\x01\x0BInitialize\x05This\x01\x13RestoreDisplayMode\x05"
    "This\x14SetCooperativeLevel\x05This\x01\x01\x0FSetDisplayMode\x05This\x01"
    "\x01\x01\x15WaitForVerticalBlank\x05This";
  load_type(ltf, 129, "IDirectDrawVtbl", p_type, p_fields);
  p_type = "\x07";
  load_type(ltf, 130, "HRESULT", p_type);
  p_type = "'";
  load_type(ltf, 131, "ULONG", p_type);
  p_type = "\x0A\x0D\x01\x13IDirectDrawClipper";
  load_type(ltf, 132, "LPDIRECTDRAWCLIPPER", p_type);
  p_type = "\x0D\x09\x0A\x0D\x01\x17IDirectDrawClipperVtbl";
  p_fields = "\x07lpVtbl";
  load_type(ltf, 133, "IDirectDrawClipper", p_type, p_fields);
  p_type = "\x0DI\x0A\x0CS=\x08HRESULT\x04\x0A=\x13IDirectDrawClipperJ}\x04IID\x0A"
    "=\x07LPVOID\x0A\x0CS=\x06ULONG\x02\x0A=\x13IDirectDrawClipper\x0A\x0CS"
    "=\x06ULONG\x02\x0A=\x13IDirectDrawClipper\x0A\x0CS=\x08HRESULT\x05\x0A"
    "=\x13IDirectDrawClipper=\x07LPRECT=\x0ALPRGNDATA=\x08LPDWORD\x0A\x0CS="
    "\x08HRESULT\x03\x0A=\x13IDirectDrawClipper\x0A=\x05HWND\x0A\x0CS=\x08H"
    "RESULT\x04\x0A=\x13IDirectDrawClipper=\x0DLPDIRECTDRAW=\x06DWORD\x0A\x0C"
    "S=\x08HRESULT\x03\x0A=\x13IDirectDrawClipper\x0A=\x05BOOL\x0A\x0CS=\x08"
    "HRESULT\x04\x0A=\x13IDirectDrawClipper=\x0ALPRGNDATA=\x06DWORD\x0A\x0C"
    "S=\x08HRESULT\x04\x0A=\x13IDirectDrawClipper=\x06DWORD=\x05HWND";
  p_fields = "\x0FQueryInterface\x05This\x05riid\x07ppvObj\x07AddRef\x05This\x08Rele"
    "ase\x05This\x0CGetClipList\x05This\x01\x01\x01\x08GetHWnd\x05This\x01\x0B"
    "Initialize\x05This\x01\x01\x12IsClipListChanged\x05This\x01\x0CSetClip"
    "List\x05This\x01\x01\x08SetHWnd\x05This";
  load_type(ltf, 134, "IDirectDrawClipperVtbl", p_type, p_fields);
  p_type = "\x0A\x0D\x01\x09_RGNDATA";
  load_type(ltf, 135, "LPRGNDATA", p_type);
  p_type = "\x0D\x11=\x0ERGNDATAHEADER\x1B\x022";
  p_fields = "\x04rdh\x07Buffer";
  load_type(ltf, 136, "_RGNDATA", p_type, p_fields);
  p_type = "\x0D\x01\x0F_RGNDATAHEADER";
  load_type(ltf, 137, "RGNDATAHEADER", p_type);
  p_type = "\x0D)=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD=\x05RECT";
  p_fields = "\x07dwSize\x06iType\x07nCount\x09nRgnSize\x08rcBound";
  load_type(ltf, 138, "_RGNDATAHEADER", p_type, p_fields);
  p_type = "\x0A=\x06DWORD";
  load_type(ltf, 139, "LPDWORD", p_type);
  p_type = "\x0D\x0D\x0A\x0D\x01\x0DIUnknownVtbl";
  p_fields = "\x07lpVtbl";
  load_type(ltf, 140, "IUnknown", p_type, p_fields);
  p_type = "\x0D\x1D\x0A\x0CS=\x08HRESULT\x04\x0A=\x09IUnknownJ}\x04IID\x0A\x0A\x01"
    "\x0A\x0CS=\x06ULONG\x02\x0A=\x09IUnknown\x0A\x0CS=\x06ULONG\x02\x0A=\x09"
    "IUnknown";
  p_fields = "\x0FQueryInterface\x05This\x05riid\x0AppvObject\x07AddRef\x05This\x08R"
    "elease\x05This";
  load_type(ltf, 141, "IUnknownVtbl", p_type, p_fields);
  p_type = "\x0A\x0D\x01\x10tagPALETTEENTRY";
  load_type(ltf, 142, "LPPALETTEENTRY", p_type);
  p_type = "\x0D!=\x05BYTE=\x05BYTE=\x05BYTE=\x05BYTE";
  p_fields = "\x06peRed\x08peGreen\x07peBlue\x08peFlags";
  load_type(ltf, 143, "tagPALETTEENTRY", p_type, p_fields);
  p_type = "\x0A\x0D\x01\x13IDirectDrawPalette";
  load_type(ltf, 144, "LPDIRECTDRAWPALETTE", p_type);
  p_type = "\x0D\x09\x0A\x0D\x01\x17IDirectDrawPaletteVtbl";
  p_fields = "\x07lpVtbl";
  load_type(ltf, 145, "IDirectDrawPalette", p_type, p_fields);
  p_type = "\x0D9\x0A\x0CS=\x08HRESULT\x04\x0A=\x13IDirectDrawPaletteJ}\x04IID\x0A"
    "=\x07LPVOID\x0A\x0CS=\x06ULONG\x02\x0A=\x13IDirectDrawPalette\x0A\x0CS"
    "=\x06ULONG\x02\x0A=\x13IDirectDrawPalette\x0A\x0CS=\x08HRESULT\x03\x0A"
    "=\x13IDirectDrawPalette=\x08LPDWORD\x0A\x0CS=\x08HRESULT\x06\x0A=\x13I"
    "DirectDrawPalette=\x06DWORD=\x06DWORD=\x06DWORD=\x0FLPPALETTEENTRY\x0A"
    "\x0CS=\x08HRESULT\x05\x0A=\x13IDirectDrawPalette=\x0DLPDIRECTDRAW=\x06"
    "DWORD=\x0FLPPALETTEENTRY\x0A\x0CS=\x08HRESULT\x06\x0A=\x13IDirectDrawP"
    "alette=\x06DWORD=\x06DWORD=\x06DWORD=\x0FLPPALETTEENTRY";
  p_fields = "\x0FQueryInterface\x05This\x05riid\x07ppvObj\x07AddRef\x05This\x08Rele"
    "ase\x05This\x08GetCaps\x05This\x01\x0BGetEntries\x05This\x01\x01\x01\x01"
    "\x0BInitialize\x05This\x01\x01\x01\x0BSetEntries\x05This";
  load_type(ltf, 146, "IDirectDrawPaletteVtbl", p_type, p_fields);
  p_type = "\x0A\x0D\x01\x0F_DDSURFACEDESC";
  load_type(ltf, 147, "LPDDSURFACEDESC", p_type);
  p_type = "\x0D\x81\x01=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD\x1D\x012_DDSURFAC"
    "EDESC::$091DBF7D405BE25E65B16620C008128B=\x06DWORD\x1D\x012_DDSURFACED"
    "ESC::$9AB659F8D6A45F8C7834A76B9C40973B=\x06DWORD=\x06DWORD=\x07LPVOID="
    "\x0BDDCOLORKEY=\x0BDDCOLORKEY=\x0BDDCOLORKEY=\x0BDDCOLORKEY=\x0EDDPIXE"
    "LFORMAT=\x08DDSCAPS";
  p_fields = "\x07dwSize\x08dwFlags\x09dwHeight\x08dwWidth\x01\x12dwBackBufferCount\x01"
    "\x10dwAlphaBitDepth\x0BdwReserved\x0AlpSurface\x12ddckCKDestOverlay\x0E"
    "ddckCKDestBlt\x11ddckCKSrcOverlay\x0DddckCKSrcBlt\x10ddpfPixelFormat\x08"
    "ddsCaps";
  load_type(ltf, 148, "_DDSURFACEDESC", p_type, p_fields);
  p_type = "\x1D\x11=\x05LONG=\x06DWORD";
  p_fields = "\x07lPitch\x0DdwLinearSize";
  load_type(ltf, 149, "_DDSURFACEDESC::$091DBF7D405BE25E65B16620C008128B", p_type, p_fields);
  p_type = "\x1D\x19=\x06DWORD=\x06DWORD=\x06DWORD";
  p_fields = "\x0EdwMipMapCount\x12dwZBufferBitDepth\x0EdwRefreshRate";
  load_type(ltf, 150, "_DDSURFACEDESC::$9AB659F8D6A45F8C7834A76B9C40973B", p_type, p_fields);
  p_type = "\x0D\x01\x0C_DDCOLORKEY";
  load_type(ltf, 151, "DDCOLORKEY", p_type);
  p_type = "\x0D\x11=\x06DWORD=\x06DWORD";
  p_fields = "\x15dwColorSpaceLowValue\x16dwColorSpaceHighValue";
  load_type(ltf, 152, "_DDCOLORKEY", p_type, p_fields);
  p_type = "\x0D\x01\x0F_DDPIXELFORMAT";
  load_type(ltf, 153, "DDPIXELFORMAT", p_type);
  p_type = "\x0DA=\x06DWORD=\x06DWORD=\x06DWORD\x1D\x012_DDPIXELFORMAT::$F1D3FB4D7"
    "8950D0942225445130999CB\x1D\x012_DDPIXELFORMAT::$6A86D2BA2D533C5D3D5AB"
    "1F1491969D5\x1D\x012_DDPIXELFORMAT::$4689F3D0B997E93FAF3E67A693E85360\x1D"
    "\x012_DDPIXELFORMAT::$4C86B66084EB9B6F3AE81991D3FADB38\x1D\x012_DDPIXE"
    "LFORMAT::$23DF69239FC04D9BE22118E1AD8451FB";
  p_fields = "\x07dwSize\x08dwFlags\x09dwFourCC";
  load_type(ltf, 154, "_DDPIXELFORMAT", p_type, p_fields);
  p_type = "\x1D9=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD=\x06"
    "DWORD";
  p_fields = "\x0EdwRGBBitCount\x0EdwYUVBitCount\x12dwZBufferBitDepth\x10dwAlphaBitD"
    "epth\x14dwLuminanceBitCount\x0FdwBumpBitCount\x18dwPrivateFormatBitCou"
    "nt";
  load_type(ltf, 155, "_DDPIXELFORMAT::$F1D3FB4D78950D0942225445130999CB", p_type, p_fields);
  p_type = "\x1D1=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD";
  p_fields = "\x0BdwRBitMask\x0BdwYBitMask\x12dwStencilBitDepth\x13dwLuminanceBitMas"
    "k\x10dwBumpDuBitMask\x0DdwOperations";
  load_type(ltf, 156, "_DDPIXELFORMAT::$6A86D2BA2D533C5D3D5AB1F1491969D5", p_type, p_fields);
  p_type = "\x1D)=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD\x0D\x01U_DDPIXELFORMAT::"
    "$4689F3D0B997E93FAF3E67A693E85360::$A78036EB239B85FA27F661E6E98FFEA9";
  p_fields = "\x0BdwGBitMask\x0BdwUBitMask\x0BdwZBitMask\x10dwBumpDvBitMask\x10Multi"
    "SampleCaps";
  load_type(ltf, 157, "_DDPIXELFORMAT::$4689F3D0B997E93FAF3E67A693E85360", p_type, p_fields);
  p_type = "\x0D\x11=\x05WORD=\x05WORD";
  p_fields = "\x0DwFlipMSTypes\x0CwBltMSTypes";
  load_type(ltf, 158, "_DDPIXELFORMAT::$4689F3D0B997E93FAF3E67A693E85360::$A78036EB239B85FA27F661E6E98FFEA9", p_type, p_fields);
  p_type = "\x1D!=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD";
  p_fields = "\x0BdwBBitMask\x0BdwVBitMask\x11dwStencilBitMask\x17dwBumpLuminanceBit"
    "Mask";
  load_type(ltf, 159, "_DDPIXELFORMAT::$4C86B66084EB9B6F3AE81991D3FADB38", p_type, p_fields);
  p_type = "\x1D)=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD";
  p_fields = "\x12dwRGBAlphaBitMask\x12dwYUVAlphaBitMask\x18dwLuminanceAlphaBitMask\x0E"
    "dwRGBZBitMask\x0EdwYUVZBitMask";
  load_type(ltf, 160, "_DDPIXELFORMAT::$23DF69239FC04D9BE22118E1AD8451FB", p_type, p_fields);
  p_type = "\x0D\x01\x09_DDSCAPS";
  load_type(ltf, 161, "DDSCAPS", p_type);
  p_type = "\x0D\x09=\x06DWORD";
  p_fields = "\x07dwCaps";
  load_type(ltf, 162, "_DDSCAPS", p_type, p_fields);
  p_type = "\x0A\x0D\x01\x13IDirectDrawSurface";
  load_type(ltf, 163, "LPDIRECTDRAWSURFACE", p_type);
  p_type = "\x0D\x09\x0A\x0D\x01\x17IDirectDrawSurfaceVtbl";
  p_fields = "\x07lpVtbl";
  load_type(ltf, 164, "IDirectDrawSurface", p_type, p_fields);
  p_type = "\x0D\xA1\x02\x0A\x0CS=\x08HRESULT\x04\x0A=\x13IDirectDrawSurfaceJ}\x04"
    "IID\x0A=\x07LPVOID\x0A\x0CS=\x06ULONG\x02\x0A=\x13IDirectDrawSurface\x0A"
    "\x0CS=\x06ULONG\x02\x0A=\x13IDirectDrawSurface\x0A\x0CS=\x08HRESULT\x03"
    "\x0A=\x13IDirectDrawSurface=\x14LPDIRECTDRAWSURFACE\x0A\x0CS=\x08HRESU"
    "LT\x03\x0A=\x13IDirectDrawSurface=\x07LPRECT\x0A\x0CS=\x08HRESULT\x07\x0A"
    "=\x13IDirectDrawSurface=\x07LPRECT=\x14LPDIRECTDRAWSURFACE=\x07LPRECT="
    "\x06DWORD=\x0ALPDDBLTFX\x0A\x0CS=\x08HRESULT\x05\x0A=\x13IDirectDrawSu"
    "rface=\x0DLPDDBLTBATCH=\x06DWORD=\x06DWORD\x0A\x0CS=\x08HRESULT\x07\x0A"
    "=\x13IDirectDrawSurface=\x06DWORD=\x06DWORD=\x14LPDIRECTDRAWSURFACE=\x07"
    "LPRECT=\x06DWORD\x0A\x0CS=\x08HRESULT\x04\x0A=\x13IDirectDrawSurface=\x06"
    "DWORD=\x14LPDIRECTDRAWSURFACE\x0A\x0CS=\x08HRESULT\x04\x0A=\x13IDirect"
    "DrawSurface=\x07LPVOID=\x19LPDDENUMSURFACESCALLBACK\x0A\x0CS=\x08HRESU"
    "LT\x05\x0A=\x13IDirectDrawSurface=\x06DWORD=\x07LPVOID=\x19LPDDENUMSUR"
    "FACESCALLBACK\x0A\x0CS=\x08HRESULT\x04\x0A=\x13IDirectDrawSurface=\x14"
    "LPDIRECTDRAWSURFACE=\x06DWORD\x0A\x0CS=\x08HRESULT\x04\x0A=\x13IDirect"
    "DrawSurface=\x0ALPDDSCAPS\x0A=\x14LPDIRECTDRAWSURFACE\x0A\x0CS=\x08HRE"
    "SULT\x03\x0A=\x13IDirectDrawSurface=\x06DWORD\x0A\x0CS=\x08HRESULT\x03"
    "\x0A=\x13IDirectDrawSurface=\x0ALPDDSCAPS\x0A\x0CS=\x08HRESULT\x03\x0A"
    "=\x13IDirectDrawSurface\x0A=\x14LPDIRECTDRAWCLIPPER\x0A\x0CS=\x08HRESU"
    "LT\x04\x0A=\x13IDirectDrawSurface=\x06DWORD=\x0DLPDDCOLORKEY\x0A\x0CS="
    "\x08HRESULT\x03\x0A=\x13IDirectDrawSurface\x0A=\x04HDC\x0A\x0CS=\x08HR"
    "ESULT\x03\x0A=\x13IDirectDrawSurface=\x06DWORD\x0A\x0CS=\x08HRESULT\x04"
    "\x0A=\x13IDirectDrawSurface=\x07LPLONG=\x07LPLONG\x0A\x0CS=\x08HRESULT"
    "\x03\x0A=\x13IDirectDrawSurface\x0A=\x14LPDIRECTDRAWPALETTE\x0A\x0CS=\x08"
    "HRESULT\x03\x0A=\x13IDirectDrawSurface=\x10LPDDPIXELFORMAT\x0A\x0CS=\x08"
    "HRESULT\x03\x0A=\x13IDirectDrawSurface=\x10LPDDSURFACEDESC\x0A\x0CS=\x08"
    "HRESULT\x04\x0A=\x13IDirectDrawSurface=\x0DLPDIRECTDRAW=\x10LPDDSURFAC"
    "EDESC\x0A\x0CS=\x08HRESULT\x02\x0A=\x13IDirectDrawSurface\x0A\x0CS=\x08"
    "HRESULT\x06\x0A=\x13IDirectDrawSurface=\x07LPRECT=\x10LPDDSURFACEDESC="
    "\x06DWORD=\x07HANDLE\x0A\x0CS=\x08HRESULT\x03\x0A=\x13IDirectDrawSurfa"
    "ce=\x04HDC\x0A\x0CS=\x08HRESULT\x02\x0A=\x13IDirectDrawSurface\x0A\x0C"
    "S=\x08HRESULT\x03\x0A=\x13IDirectDrawSurface=\x14LPDIRECTDRAWCLIPPER\x0A"
    "\x0CS=\x08HRESULT\x04\x0A=\x13IDirectDrawSurface=\x06DWORD=\x0DLPDDCOL"
    "ORKEY\x0A\x0CS=\x08HRESULT\x04\x0A=\x13IDirectDrawSurface=\x05LONG=\x05"
    "LONG\x0A\x0CS=\x08HRESULT\x03\x0A=\x13IDirectDrawSurface=\x14LPDIRECTD"
    "RAWPALETTE\x0A\x0CS=\x08HRESULT\x03\x0A=\x13IDirectDrawSurface=\x07LPV"
    "OID\x0A\x0CS=\x08HRESULT\x07\x0A=\x13IDirectDrawSurface=\x07LPRECT=\x14"
    "LPDIRECTDRAWSURFACE=\x07LPRECT=\x06DWORD=\x0ELPDDOVERLAYFX\x0A\x0CS=\x08"
    "HRESULT\x03\x0A=\x13IDirectDrawSurface=\x06DWORD\x0A\x0CS=\x08HRESULT\x04"
    "\x0A=\x13IDirectDrawSurface=\x06DWORD=\x14LPDIRECTDRAWSURFACE";
  p_fields = "\x0FQueryInterface\x05This\x05riid\x07ppvObj\x07AddRef\x05This\x08Rele"
    "ase\x05This\x13AddAttachedSurface\x05This\x01\x14AddOverlayDirtyRect\x05"
    "This\x01\x04Blt\x05This\x01\x01\x01\x01\x01\x09BltBatch\x05This\x01\x01"
    "\x01\x08BltFast\x05This\x01\x01\x01\x01\x01\x16DeleteAttachedSurface\x05"
    "This\x01\x01\x15EnumAttachedSurfaces\x05This\x01\x01\x13EnumOverlayZOr"
    "ders\x05This\x01\x01\x01\x05Flip\x05This\x01\x01\x13GetAttachedSurface"
    "\x05This\x01\x01\x0DGetBltStatus\x05This\x01\x08GetCaps\x05This\x01\x0B"
    "GetClipper\x05This\x01\x0CGetColorKey\x05This\x01\x01\x06GetDC\x05This"
    "\x01\x0EGetFlipStatus\x05This\x01\x13GetOverlayPosition\x05This\x01\x01"
    "\x0BGetPalette\x05This\x01\x0FGetPixelFormat\x05This\x01\x0FGetSurface"
    "Desc\x05This\x01\x0BInitialize\x05This\x01\x01\x07IsLost\x05This\x05Lo"
    "ck\x05This\x01\x01\x01\x01\x0AReleaseDC\x05This\x01\x08Restore\x05This"
    "\x0BSetClipper\x05This\x01\x0CSetColorKey\x05This\x01\x01\x13SetOverla"
    "yPosition\x05This\x01\x01\x0BSetPalette\x05This\x01\x07Unlock\x05This\x01"
    "\x0EUpdateOverlay\x05This\x01\x01\x01\x01\x01\x15UpdateOverlayDisplay\x05"
    "This\x01\x14UpdateOverlayZOrder\x05This";
  load_type(ltf, 165, "IDirectDrawSurfaceVtbl", p_type, p_fields);
  p_type = "\x0A=\x08DDBLTFX";
  load_type(ltf, 166, "LPDDBLTFX", p_type);
  p_type = "\x0D\x01\x09_DDBLTFX";
  load_type(ltf, 167, "DDBLTFX", p_type);
  p_type = "\x0D\xB9\x01=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWO"
    "RD=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD\x1D\x01,_DDBLTFX::$EA6EC206"
    "95B0AA8ECF92A441DE55A935=\x06DWORD\x1D\x01,_DDBLTFX::$702C52B3655764E6"
    "3C007A6B73D01C19=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD\x1D\x01,_DDBL"
    "TFX::$3639C047B3E88B857708792B1F3FEC83=\x06DWORD\x1D\x01,_DDBLTFX::$C0"
    "828DA952883A43DEF8D462ACC90C87\x1D\x01,_DDBLTFX::$4C109671A06E28B8127C"
    "40041AB5795C=\x0BDDCOLORKEY=\x0BDDCOLORKEY";
  p_fields = "\x07dwSize\x07dwDDFX\x06dwROP\x08dwDDROP\x10dwRotationAngle\x10dwZBuff"
    "erOpCode\x0DdwZBufferLow\x0EdwZBufferHigh\x12dwZBufferBaseDest\x15dwZD"
    "estConstBitDepth\x01\x14dwZSrcConstBitDepth\x01\x19dwAlphaEdgeBlendBit"
    "Depth\x11dwAlphaEdgeBlend\x0BdwReserved\x19dwAlphaDestConstBitDepth\x01"
    "\x18dwAlphaSrcConstBitDepth\x01\x01\x11ddckDestColorkey\x10ddckSrcColo"
    "rkey";
  load_type(ltf, 168, "_DDBLTFX", p_type, p_fields);
  p_type = "\x1D\x11=\x06DWORD=\x14LPDIRECTDRAWSURFACE";
  p_fields = "\x0DdwZDestConst\x11lpDDSZBufferDest";
  load_type(ltf, 169, "_DDBLTFX::$EA6EC20695B0AA8ECF92A441DE55A935", p_type, p_fields);
  p_type = "\x1D\x11=\x06DWORD=\x14LPDIRECTDRAWSURFACE";
  p_fields = "\x0CdwZSrcConst\x10lpDDSZBufferSrc";
  load_type(ltf, 170, "_DDBLTFX::$702C52B3655764E63C007A6B73D01C19", p_type, p_fields);
  p_type = "\x1D\x11=\x06DWORD=\x14LPDIRECTDRAWSURFACE";
  p_fields = "\x11dwAlphaDestConst\x0FlpDDSAlphaDest";
  load_type(ltf, 171, "_DDBLTFX::$3639C047B3E88B857708792B1F3FEC83", p_type, p_fields);
  p_type = "\x1D\x11=\x06DWORD=\x14LPDIRECTDRAWSURFACE";
  p_fields = "\x10dwAlphaSrcConst\x0ElpDDSAlphaSrc";
  load_type(ltf, 172, "_DDBLTFX::$C0828DA952883A43DEF8D462ACC90C87", p_type, p_fields);
  p_type = "\x1D!=\x06DWORD=\x06DWORD=\x06DWORD=\x14LPDIRECTDRAWSURFACE";
  p_fields = "\x0CdwFillColor\x0CdwFillDepth\x0CdwFillPixel\x0DlpDDSPattern";
  load_type(ltf, 173, "_DDBLTFX::$4C109671A06E28B8127C40041AB5795C", p_type, p_fields);
  p_type = "\x0A=\x0BDDBLTBATCH";
  load_type(ltf, 174, "LPDDBLTBATCH", p_type);
  p_type = "\x0D\x01\x0C_DDBLTBATCH";
  load_type(ltf, 175, "DDBLTBATCH", p_type);
  p_type = "\x0D)=\x07LPRECT=\x14LPDIRECTDRAWSURFACE=\x07LPRECT=\x06DWORD=\x0ALPDD"
    "BLTFX";
  p_fields = "\x08lprDest\x09lpDDSSrc\x07lprSrc\x08dwFlags\x0AlpDDBltFx";
  load_type(ltf, 176, "_DDBLTBATCH", p_type, p_fields);
  p_type = "\x0A\x0CS=\x08HRESULT\x04=\x14LPDIRECTDRAWSURFACE=\x10LPDDSURFACEDESC="
    "\x07LPVOID";
  load_type(ltf, 177, "LPDDENUMSURFACESCALLBACK", p_type);
  p_type = "\x0A=\x08DDSCAPS";
  load_type(ltf, 178, "LPDDSCAPS", p_type);
  p_type = "\x0A=\x0BDDCOLORKEY";
  load_type(ltf, 179, "LPDDCOLORKEY", p_type);
  p_type = "\x0A\x07";
  load_type(ltf, 180, "LPLONG", p_type);
  p_type = "\x0A=\x0EDDPIXELFORMAT";
  load_type(ltf, 181, "LPDDPIXELFORMAT", p_type);
  p_type = "\x0A=\x0CDDOVERLAYFX";
  load_type(ltf, 182, "LPDDOVERLAYFX", p_type);
  p_type = "\x0D\x01\x0D_DDOVERLAYFX";
  load_type(ltf, 183, "DDOVERLAYFX", p_type);
  p_type = "\x0Da=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD\x1D\x010_DDOVE"
    "RLAYFX::$3639C047B3E88B857708792B1F3FEC83=\x06DWORD\x1D\x010_DDOVERLAY"
    "FX::$C0828DA952883A43DEF8D462ACC90C87=\x0BDDCOLORKEY=\x0BDDCOLORKEY=\x06"
    "DWORD=\x06DWORD";
  p_fields = "\x07dwSize\x19dwAlphaEdgeBlendBitDepth\x11dwAlphaEdgeBlend\x0BdwReserv"
    "ed\x19dwAlphaDestConstBitDepth\x01\x18dwAlphaSrcConstBitDepth\x01\x10d"
    "ckDestColorkey\x0FdckSrcColorkey\x07dwDDFX\x08dwFlags";
  load_type(ltf, 184, "_DDOVERLAYFX", p_type, p_fields);
  p_type = "\x1D\x11=\x06DWORD=\x14LPDIRECTDRAWSURFACE";
  p_fields = "\x11dwAlphaDestConst\x0FlpDDSAlphaDest";
  load_type(ltf, 185, "_DDOVERLAYFX::$3639C047B3E88B857708792B1F3FEC83", p_type, p_fields);
  p_type = "\x1D\x11=\x06DWORD=\x14LPDIRECTDRAWSURFACE";
  p_fields = "\x10dwAlphaSrcConst\x0ElpDDSAlphaSrc";
  load_type(ltf, 186, "_DDOVERLAYFX::$C0828DA952883A43DEF8D462ACC90C87", p_type, p_fields);
  p_type = "\x0A\x0CS=\x08HRESULT\x03=\x10LPDDSURFACEDESC=\x07LPVOID";
  load_type(ltf, 187, "LPDDENUMMODESCALLBACK", p_type);
  p_type = "\x0A=\x07DDCAPS";
  load_type(ltf, 188, "LPDDCAPS", p_type);
  p_type = "=\x0BDDCAPS_DX7";
  load_type(ltf, 189, "DDCAPS", p_type);
  p_type = "\x0D\x01\x0C_DDCAPS_DX7";
  load_type(ltf, 190, "DDCAPS_DX7", p_type);
  p_type = "\x0D\xC9\x03=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWO"
    "RD=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWO"
    "RD=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWO"
    "RD=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD\x1B\x09=\x06DWORD"
    "=\x08DDSCAPS=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWO"
    "RD=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD\x1B\x09"
    "=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD\x1B\x09=\x06DWORD=\x06DWORD=\x06"
    "DWORD=\x06DWORD\x1B\x09=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD=\x06DW"
    "ORD=\x06DWORD=\x06DWORD=\x06DWORD\x1B\x09=\x06DWORD=\x09DDSCAPS2";
  p_fields = "\x07dwSize\x07dwCaps\x08dwCaps2\x0BdwCKeyCaps\x09dwFXCaps\x0EdwFXAlpha"
    "Caps\x0AdwPalCaps\x09dwSVCaps\x19dwAlphaBltConstBitDepths\x19dwAlphaBl"
    "tPixelBitDepths\x1BdwAlphaBltSurfaceBitDepths\x1DdwAlphaOverlayConstBi"
    "tDepths\x1DdwAlphaOverlayPixelBitDepths\x1FdwAlphaOverlaySurfaceBitDep"
    "ths\x13dwZBufferBitDepths\x0EdwVidMemTotal\x0DdwVidMemFree\x15dwMaxVis"
    "ibleOverlays\x16dwCurrVisibleOverlays\x11dwNumFourCCCodes\x13dwAlignBo"
    "undarySrc\x0FdwAlignSizeSrc\x14dwAlignBoundaryDest\x10dwAlignSizeDest\x13"
    "dwAlignStrideAlign\x07dwRops\x0BddsOldCaps\x14dwMinOverlayStretch\x14d"
    "wMaxOverlayStretch\x16dwMinLiveVideoStretch\x16dwMaxLiveVideoStretch\x14"
    "dwMinHwCodecStretch\x14dwMaxHwCodecStretch\x0CdwReserved1\x0CdwReserve"
    "d2\x0CdwReserved3\x0AdwSVBCaps\x0EdwSVBCKeyCaps\x0CdwSVBFXCaps\x0AdwSV"
    "BRops\x0AdwVSBCaps\x0EdwVSBCKeyCaps\x0CdwVSBFXCaps\x0AdwVSBRops\x0AdwS"
    "SBCaps\x0EdwSSBCKeyCaps\x0CdwSSBFXCaps\x0AdwSSBRops\x10dwMaxVideoPorts"
    "\x11dwCurrVideoPorts\x0BdwSVBCaps2\x0BdwNLVBCaps\x0CdwNLVBCaps2\x0FdwN"
    "LVBCKeyCaps\x0DdwNLVBFXCaps\x0BdwNLVBRops\x08ddsCaps";
  load_type(ltf, 191, "_DDCAPS_DX7", p_type, p_fields);
  p_type = "\x0D\x01\x0A_DDSCAPS2";
  load_type(ltf, 192, "DDSCAPS2", p_type);
  p_type = "\x0D!=\x06DWORD=\x06DWORD=\x06DWORD\x1D\x01-_DDSCAPS2::$19AC68468C4510"
    "B3DC631A4E89752068";
  p_fields = "\x07dwCaps\x08dwCaps2\x08dwCaps3";
  load_type(ltf, 193, "_DDSCAPS2", p_type, p_fields);
  p_type = "\x1D\x11=\x06DWORD=\x06DWORD";
  p_fields = "\x08dwCaps4\x0EdwVolumeDepth";
  load_type(ltf, 194, "_DDSCAPS2::$19AC68468C4510B3DC631A4E89752068", p_type, p_fields);
  p_type = "\x0A=\x05BOOL";
  load_type(ltf, 195, "LPBOOL", p_type);
  p_type = "-\x0A\x80AABDHp\x83@\x84@\x88@";
  p_fields = "\x11DDSCL_FULLSCREEN\x12DDSCL_ALLOWREBOOT\x16DDSCL_NOWINDOWCHANGES\x0D"
    "DDSCL_NORMAL\x10DDSCL_EXCLUSIVE\x11DDSCL_ALLOWMODEX\x16DDSCL_SETDEVICE"
    "WINDOW\x15DDSCL_SETFOCUSWINDOW\x14DDSCL_MULTITHREADED";
  load_type(ltf, 196, "DDSCL", p_type, p_fields);
  p_type = "\x0A\x0D\x01\x10_DDSURFACEDESC2";
  load_type(ltf, 197, "LPDDSURFACEDESC2", p_type);
  p_type = "\x0D\x89\x01=\x06DWORD=\x06DWORD=\x06DWORD=\x06DWORD\x1D\x013_DDSURFAC"
    "EDESC2::$091DBF7D405BE25E65B16620C008128B\x1D\x013_DDSURFACEDESC2::$E5"
    "6E877DD0D4D3B03846BC7047D947ED\x1D\x013_DDSURFACEDESC2::$F25F5019EA70F"
    "6B9B11866A0D418A260=\x06DWORD=\x06DWORD=\x07LPVOID\x1D\x013_DDSURFACED"
    "ESC2::$5B12F1D1A21F9ED7C15CB39448B5A42D=\x0BDDCOLORKEY=\x0BDDCOLORKEY="
    "\x0BDDCOLORKEY\x1D\x013_DDSURFACEDESC2::$640C42AFF919FFD3058B9E962436B"
    "842=\x09DDSCAPS2=\x06DWORD";
  p_fields = "\x07dwSize\x08dwFlags\x09dwHeight\x08dwWidth\x01\x01\x01\x10dwAlphaBit"
    "Depth\x0BdwReserved\x0AlpSurface\x01\x0EddckCKDestBlt\x11ddckCKSrcOver"
    "lay\x0DddckCKSrcBlt\x01\x08ddsCaps\x0FdwTextureStage";
  load_type(ltf, 198, "_DDSURFACEDESC2", p_type, p_fields);
  p_type = "\x1D\x11=\x05LONG=\x06DWORD";
  p_fields = "\x07lPitch\x0DdwLinearSize";
  load_type(ltf, 199, "_DDSURFACEDESC2::$091DBF7D405BE25E65B16620C008128B", p_type, p_fields);
  p_type = "\x1D\x11=\x06DWORD=\x06DWORD";
  p_fields = "\x12dwBackBufferCount\x08dwDepth";
  load_type(ltf, 200, "_DDSURFACEDESC2::$E56E877DD0D4D3B03846BC7047D947ED", p_type, p_fields);
  p_type = "\x1D\x19=\x06DWORD=\x06DWORD=\x06DWORD";
  p_fields = "\x0EdwMipMapCount\x0EdwRefreshRate\x0EdwSrcVBHandle";
  load_type(ltf, 201, "_DDSURFACEDESC2::$F25F5019EA70F6B9B11866A0D418A260", p_type, p_fields);
  p_type = "\x1D\x11=\x0BDDCOLORKEY=\x06DWORD";
  p_fields = "\x12ddckCKDestOverlay\x11dwEmptyFaceColor";
  load_type(ltf, 202, "_DDSURFACEDESC2::$5B12F1D1A21F9ED7C15CB39448B5A42D", p_type, p_fields);
  p_type = "\x1D\x11=\x0EDDPIXELFORMAT=\x06DWORD";
  p_fields = "\x10ddpfPixelFormat\x06dwFVF";
  load_type(ltf, 203, "_DDSURFACEDESC2::$640C42AFF919FFD3058B9E962436B842", p_type, p_fields);
  p_type = "\x0A\x0D\x01\x14IDirectDrawSurface7";
  load_type(ltf, 204, "LPDIRECTDRAWSURFACE7", p_type);
  p_type = "\x0D\x09\x0A\x0D\x01\x18IDirectDrawSurface7Vtbl";
  p_fields = "\x07lpVtbl";
  load_type(ltf, 205, "IDirectDrawSurface7", p_type, p_fields);
  p_type = "\x0D\x89\x03\x0A\x0CS=\x08HRESULT\x04\x0A=\x14IDirectDrawSurface7J}\x04"
    "IID\x0A=\x07LPVOID\x0A\x0CS=\x06ULONG\x02\x0A=\x14IDirectDrawSurface7\x0A"
    "\x0CS=\x06ULONG\x02\x0A=\x14IDirectDrawSurface7\x0A\x0CS=\x08HRESULT\x03"
    "\x0A=\x14IDirectDrawSurface7=\x15LPDIRECTDRAWSURFACE7\x0A\x0CS=\x08HRE"
    "SULT\x03\x0A=\x14IDirectDrawSurface7=\x07LPRECT\x0A\x0CS=\x08HRESULT\x07"
    "\x0A=\x14IDirectDrawSurface7=\x07LPRECT=\x15LPDIRECTDRAWSURFACE7=\x07L"
    "PRECT=\x06DWORD=\x0ALPDDBLTFX\x0A\x0CS=\x08HRESULT\x05\x0A=\x14IDirect"
    "DrawSurface7=\x0DLPDDBLTBATCH=\x06DWORD=\x06DWORD\x0A\x0CS=\x08HRESULT"
    "\x07\x0A=\x14IDirectDrawSurface7=\x06DWORD=\x06DWORD=\x15LPDIRECTDRAWS"
    "URFACE7=\x07LPRECT=\x06DWORD\x0A\x0CS=\x08HRESULT\x04\x0A=\x14IDirectD"
    "rawSurface7=\x06DWORD=\x15LPDIRECTDRAWSURFACE7\x0A\x0CS=\x08HRESULT\x04"
    "\x0A=\x14IDirectDrawSurface7=\x07LPVOID=\x1ALPDDENUMSURFACESCALLBACK7\x0A"
    "\x0CS=\x08HRESULT\x05\x0A=\x14IDirectDrawSurface7=\x06DWORD=\x07LPVOID"
    "=\x1ALPDDENUMSURFACESCALLBACK7\x0A\x0CS=\x08HRESULT\x04\x0A=\x14IDirec"
    "tDrawSurface7=\x15LPDIRECTDRAWSURFACE7=\x06DWORD\x0A\x0CS=\x08HRESULT\x04"
    "\x0A=\x14IDirectDrawSurface7=\x0BLPDDSCAPS2\x0A=\x15LPDIRECTDRAWSURFAC"
    "E7\x0A\x0CS=\x08HRESULT\x03\x0A=\x14IDirectDrawSurface7=\x06DWORD\x0A\x0C"
    "S=\x08HRESULT\x03\x0A=\x14IDirectDrawSurface7=\x0BLPDDSCAPS2\x0A\x0CS="
    "\x08HRESULT\x03\x0A=\x14IDirectDrawSurface7\x0A=\x14LPDIRECTDRAWCLIPPE"
    "R\x0A\x0CS=\x08HRESULT\x04\x0A=\x14IDirectDrawSurface7=\x06DWORD=\x0DL"
    "PDDCOLORKEY\x0A\x0CS=\x08HRESULT\x03\x0A=\x14IDirectDrawSurface7\x0A=\x04"
    "HDC\x0A\x0CS=\x08HRESULT\x03\x0A=\x14IDirectDrawSurface7=\x06DWORD\x0A"
    "\x0CS=\x08HRESULT\x04\x0A=\x14IDirectDrawSurface7=\x07LPLONG=\x07LPLON"
    "G\x0A\x0CS=\x08HRESULT\x03\x0A=\x14IDirectDrawSurface7\x0A=\x14LPDIREC"
    "TDRAWPALETTE\x0A\x0CS=\x08HRESULT\x03\x0A=\x14IDirectDrawSurface7=\x10"
    "LPDDPIXELFORMAT\x0A\x0CS=\x08HRESULT\x03\x0A=\x14IDirectDrawSurface7=\x11"
    "LPDDSURFACEDESC2\x0A\x0CS=\x08HRESULT\x04\x0A=\x14IDirectDrawSurface7="
    "\x0DLPDIRECTDRAW=\x11LPDDSURFACEDESC2\x0A\x0CS=\x08HRESULT\x02\x0A=\x14"
    "IDirectDrawSurface7\x0A\x0CS=\x08HRESULT\x06\x0A=\x14IDirectDrawSurfac"
    "e7=\x07LPRECT=\x11LPDDSURFACEDESC2=\x06DWORD=\x07HANDLE\x0A\x0CS=\x08H"
    "RESULT\x03\x0A=\x14IDirectDrawSurface7=\x04HDC\x0A\x0CS=\x08HRESULT\x02"
    "\x0A=\x14IDirectDrawSurface7\x0A\x0CS=\x08HRESULT\x03\x0A=\x14IDirectD"
    "rawSurface7=\x14LPDIRECTDRAWCLIPPER\x0A\x0CS=\x08HRESULT\x04\x0A=\x14I"
    "DirectDrawSurface7=\x06DWORD=\x0DLPDDCOLORKEY\x0A\x0CS=\x08HRESULT\x04"
    "\x0A=\x14IDirectDrawSurface7=\x05LONG=\x05LONG\x0A\x0CS=\x08HRESULT\x03"
    "\x0A=\x14IDirectDrawSurface7=\x14LPDIRECTDRAWPALETTE\x0A\x0CS=\x08HRES"
    "ULT\x03\x0A=\x14IDirectDrawSurface7=\x07LPRECT\x0A\x0CS=\x08HRESULT\x07"
    "\x0A=\x14IDirectDrawSurface7=\x07LPRECT=\x15LPDIRECTDRAWSURFACE7=\x07L"
    "PRECT=\x06DWORD=\x0ELPDDOVERLAYFX\x0A\x0CS=\x08HRESULT\x03\x0A=\x14IDi"
    "rectDrawSurface7=\x06DWORD\x0A\x0CS=\x08HRESULT\x04\x0A=\x14IDirectDra"
    "wSurface7=\x06DWORD=\x15LPDIRECTDRAWSURFACE7\x0A\x0CS=\x08HRESULT\x03\x0A"
    "=\x14IDirectDrawSurface7\x0A=\x07LPVOID\x0A\x0CS=\x08HRESULT\x03\x0A=\x14"
    "IDirectDrawSurface7=\x06DWORD\x0A\x0CS=\x08HRESULT\x03\x0A=\x14IDirect"
    "DrawSurface7=\x06DWORD\x0A\x0CS=\x08HRESULT\x04\x0A=\x14IDirectDrawSur"
    "face7=\x11LPDDSURFACEDESC2=\x06DWORD\x0A\x0CS=\x08HRESULT\x06\x0A=\x14"
    "IDirectDrawSurface7J}\x05GUID=\x07LPVOID=\x06DWORD=\x06DWORD\x0A\x0CS="
    "\x08HRESULT\x05\x0A=\x14IDirectDrawSurface7J}\x05GUID=\x07LPVOID=\x08L"
    "PDWORD\x0A\x0CS=\x08HRESULT\x03\x0A=\x14IDirectDrawSurface7J}\x05GUID\x0A"
    "\x0CS=\x08HRESULT\x03\x0A=\x14IDirectDrawSurface7=\x08LPDWORD\x0A\x0CS"
    "=\x08HRESULT\x02\x0A=\x14IDirectDrawSurface7\x0A\x0CS=\x08HRESULT\x03\x0A"
    "=\x14IDirectDrawSurface7=\x06DWORD\x0A\x0CS=\x08HRESULT\x03\x0A=\x14ID"
    "irectDrawSurface7=\x08LPDWORD\x0A\x0CS=\x08HRESULT\x03\x0A=\x14IDirect"
    "DrawSurface7=\x06DWORD\x0A\x0CS=\x08HRESULT\x03\x0A=\x14IDirectDrawSur"
    "face7=\x08LPDWORD";
  p_fields = "\x0FQueryInterface\x05This\x05riid\x07ppvObj\x07AddRef\x05This\x08Rele"
    "ase\x05This\x13AddAttachedSurface\x05This\x01\x14AddOverlayDirtyRect\x05"
    "This\x01\x04Blt\x05This\x01\x01\x01\x01\x01\x09BltBatch\x05This\x01\x01"
    "\x01\x08BltFast\x05This\x01\x01\x01\x01\x01\x16DeleteAttachedSurface\x05"
    "This\x01\x01\x15EnumAttachedSurfaces\x05This\x01\x01\x13EnumOverlayZOr"
    "ders\x05This\x01\x01\x01\x05Flip\x05This\x01\x01\x13GetAttachedSurface"
    "\x05This\x01\x01\x0DGetBltStatus\x05This\x01\x08GetCaps\x05This\x01\x0B"
    "GetClipper\x05This\x01\x0CGetColorKey\x05This\x01\x01\x06GetDC\x05This"
    "\x01\x0EGetFlipStatus\x05This\x01\x13GetOverlayPosition\x05This\x01\x01"
    "\x0BGetPalette\x05This\x01\x0FGetPixelFormat\x05This\x01\x0FGetSurface"
    "Desc\x05This\x01\x0BInitialize\x05This\x01\x01\x07IsLost\x05This\x05Lo"
    "ck\x05This\x01\x01\x01\x01\x0AReleaseDC\x05This\x01\x08Restore\x05This"
    "\x0BSetClipper\x05This\x01\x0CSetColorKey\x05This\x01\x01\x13SetOverla"
    "yPosition\x05This\x01\x01\x0BSetPalette\x05This\x01\x07Unlock\x05This\x01"
    "\x0EUpdateOverlay\x05This\x01\x01\x01\x01\x01\x15UpdateOverlayDisplay\x05"
    "This\x01\x14UpdateOverlayZOrder\x05This\x01\x01\x0FGetDDInterface\x05T"
    "his\x01\x09PageLock\x05This\x01\x0BPageUnlock\x05This\x01\x0FSetSurfac"
    "eDesc\x05This\x01\x01\x0FSetPrivateData\x05This\x01\x01\x01\x01\x0FGet"
    "PrivateData\x05This\x01\x01\x01\x10FreePrivateData\x05This\x01\x13GetU"
    "niquenessValue\x05This\x01\x16ChangeUniquenessValue\x05This\x0CSetPrio"
    "rity\x05This\x01\x0CGetPriority\x05This\x01\x07SetLOD\x05This\x01\x07G"
    "etLOD\x05This";
  load_type(ltf, 206, "IDirectDrawSurface7Vtbl", p_type, p_fields);
  p_type = "\x0A\x0CS=\x08HRESULT\x04=\x15LPDIRECTDRAWSURFACE7=\x11LPDDSURFACEDESC"
    "2=\x07LPVOID";
  load_type(ltf, 207, "LPDDENUMSURFACESCALLBACK7", p_type);
  p_type = "\x0A=\x09DDSCAPS2";
  load_type(ltf, 208, "LPDDSCAPS2", p_type);
  p_type = "\x0D\xA1\x01\x1B\x0D2\x0A=\x0Dddraw_window\x1B\xA5\x032=\x09uint32_t\x1B"
    "\x8D\x012\x0A=\x09uint32_t\x1B\xE9`2=\x09uint32_t=\x09uint32_t=\x09uin"
    "t32_t\x1B\xA1\xA02=\x09uint32_t=\x09uint32_t\x1B\x0D2=\x09uint32_t\x1B"
    "-2=\x09uint32_t\x1B\x8D\x012=\x09uint32_t\x1B52";
  p_fields = "\x05tmp8\x07window\x06tmpbb\x0CbFullscreen\x05tmp0\x06tmp13\x05tmp4\x05"
    "tmp9\x0DignoreInput2\x0CignoreInput\x05tmp5\x06tmp10\x06tmp11\x05tmp6\x06"
    "tmp12\x05tmp2\x05hwnd\x05tmp3\x10bEnableHighFreq\x04tmp";
  load_type(ltf, 209, "MainGame", p_type, p_fields);
  p_type = "\x0D!\x1B\x95[2\x08\x1B\x042\x1B\x91\x012";
  p_fields = "\x05tmp1\x0DbRunGameLoop\x05tmp2\x11rightClickButton";
  load_type(ltf, 210, "GameLoopEntity", p_type, p_fields);
  p_type = "\x07";
  load_type(ltf, 211, "int32_t", p_type);
  p_type = "%";
  load_type(ltf, 212, "QWORD", p_type);
  p_type = "-\x08\xC0\x9F\xFF\xFF\xFF|\x9F\xFF\xFF\xFF~\x9F\xFF\xFF\xFF~\x9F\xFF\xFF"
    "\xFFx\x9F\xFF\xFF\xFF|\x9F\xFF\xFF\xFF\x7FI";
  p_fields = "\x0CGWL_WNDPROC\x0EGWL_HINSTANCE\x0FGWL_HWNDPARENT\x0AGWL_STYLE\x0CGWL"
    "_EXSTYLE\x0DGWL_USERDATA\x07GWL_ID";
  load_type(ltf, 213, "MACRO_GWL", p_type, p_fields);
  p_type = "-x\xC0\x91\x87\xB0\x80EEJTO\x9E\xF8\xD1\xFFN\x81\x87\xAE\x81UEEJJJ\x9E"
    "\xF9\x87\xFFU\x81\x86\xF8\x80zEJJJAAWEBCEBCEJJEEJJJTFACJEEJJ\x9E\xF9\x87"
    "\xFAf\x81\x86\xF8\x85nBBCMJJJEEJJJJJ\x9E\xF8\xD1\xF8W\x81\x87\xAE\x87}"
    "JAPCBRAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAHAAAQJJTTJAAAAAAAB\x9E\xF8\xEF\xFC"
    "u";
  p_fields = "\x19DDERR_ALREADYINITIALIZED\x1ADDERR_CANNOTATTACHSURFACE\x1ADDERR_CAN"
    "NOTDETACHSURFACE\x18DDERR_CURRENTLYNOTAVAIL\x10DDERR_EXCEPTION\x0EDDER"
    "R_GENERIC\x12DDERR_HEIGHTALIGN\x1ADDERR_INCOMPATIBLEPRIMARY\x12DDERR_I"
    "NVALIDCAPS\x16DDERR_INVALIDCLIPLIST\x12DDERR_INVALIDMODE\x14DDERR_INVA"
    "LIDOBJECT\x14DDERR_INVALIDPARAMS\x19DDERR_INVALIDPIXELFORMAT\x12DDERR_"
    "INVALIDRECT\x15DDERR_LOCKEDSURFACES\x0BDDERR_NO3D\x10DDERR_NOALPHAHW\x17"
    "DDERR_NOSTEREOHARDWARE\x14DDERR_NOSURFACELEFT\x11DDERR_NOCLIPLIST\x14D"
    "DERR_NOCOLORCONVHW\x1CDDERR_NOCOOPERATIVELEVELSET\x11DDERR_NOCOLORKEY\x13"
    "DDERR_NOCOLORKEYHW\x1ADDERR_NODIRECTDRAWSUPPORT\x16DDERR_NOEXCLUSIVEMO"
    "DE\x0FDDERR_NOFLIPHW\x0CDDERR_NOGDI\x11DDERR_NOMIRRORHW\x0FDDERR_NOTFO"
    "UND\x12DDERR_NOOVERLAYHW\x17DDERR_OVERLAPPINGRECTS\x13DDERR_NORASTEROP"
    "HW\x13DDERR_NOROTATIONHW\x12DDERR_NOSTRETCHHW\x13DDERR_NOT4BITCOLOR\x18"
    "DDERR_NOT4BITCOLORINDEX\x13DDERR_NOT8BITCOLOR\x12DDERR_NOTEXTUREHW\x10"
    "DDERR_NOVSYNCHW\x12DDERR_NOZBUFFERHW\x13DDERR_NOZOVERLAYHW\x10DDERR_OU"
    "TOFCAPS\x12DDERR_OUTOFMEMORY\x17DDERR_OUTOFVIDEOMEMORY\x16DDERR_OVERLA"
    "YCANTCLIP#DDERR_OVERLAYCOLORKEYONLYONEACTIVE\x12DDERR_PALETTEBUSY\x15D"
    "DERR_COLORKEYNOTSET\x1DDDERR_SURFACEALREADYATTACHED\x1EDDERR_SURFACEAL"
    "READYDEPENDENT\x12DDERR_SURFACEBUSY\x16DDERR_CANTLOCKSURFACE\x18DDERR_"
    "SURFACEISOBSCURED\x12DDERR_SURFACELOST\x19DDERR_SURFACENOTATTACHED\x13"
    "DDERR_TOOBIGHEIGHT\x11DDERR_TOOBIGSIZE\x12DDERR_TOOBIGWIDTH\x12DDERR_U"
    "NSUPPORTED\x18DDERR_UNSUPPORTEDFORMAT\x16DDERR_UNSUPPORTEDMASK\x14DDER"
    "R_INVALIDSTREAM\x1EDDERR_VERTICALBLANKINPROGRESS\x16DDERR_WASSTILLDRAW"
    "ING\x1DDDERR_DDSCAPSCOMPLEXREQUIRED\x0DDDERR_XALIGN\x1CDDERR_INVALIDDI"
    "RECTDRAWGUID\x1FDDERR_DIRECTDRAWALREADYCREATED\x15DDERR_NODIRECTDRAWHW"
    "\"DDERR_PRIMARYSURFACEALREADYEXISTS\x12DDERR_NOEMULATION\x15DDERR_REGI"
    "ONTOOSMALL\x19DDERR_CLIPPERISUSINGHWND\x18DDERR_NOCLIPPERATTACHED\x0DD"
    "DERR_NOHWND\x15DDERR_HWNDSUBCLASSED\x15DDERR_HWNDALREADYSET\x18DDERR_N"
    "OPALETTEATTACHED\x12DDERR_NOPALETTEHW\x16DDERR_BLTFASTCANTCLIP\x0EDDER"
    "R_NOBLTHW\x11DDERR_NODDROPSHW\x18DDERR_OVERLAYNOTVISIBLE\x14DDERR_NOOV"
    "ERLAYDEST\x16DDERR_INVALIDPOSITION\x19DDERR_NOTAOVERLAYSURFACE\x1EDDER"
    "R_EXCLUSIVEMODEALREADYSET\x13DDERR_NOTFLIPPABLE\x14DDERR_CANTDUPLICATE"
    "\x10DDERR_NOTLOCKED\x13DDERR_CANTCREATEDC\x0BDDERR_NODC\x10DDERR_WRONG"
    "MODE\x18DDERR_IMPLICITLYCREATED\x14DDERR_NOTPALETTIZED\x16DDERR_UNSUPP"
    "ORTEDMODE\x11DDERR_NOMIPMAPHW\x19DDERR_INVALIDSURFACETYPE\x13DDERR_NOO"
    "PTIMIZEHW\x10DDERR_NOTLOADED\x14DDERR_NOFOCUSWINDOW\x1ADDERR_NOTONMIPM"
    "APSUBLEVEL\x17DDERR_DCALREADYCREATED\x17DDERR_NONONLOCALVIDMEM\x13DDER"
    "R_CANTPAGELOCK\x15DDERR_CANTPAGEUNLOCK\x14DDERR_NOTPAGELOCKED\x0FDDERR"
    "_MOREDATA\x0EDDERR_EXPIRED\x13DDERR_TESTFINISHED\x0EDDERR_NEWMODE\x18D"
    "DERR_D3DNOTINITIALIZED\x15DDERR_VIDEONOTACTIVE\x1BDDERR_NOMONITORINFOR"
    "MATION\x16DDERR_NODRIVERSUPPORT\x1DDDERR_DEVICEDOESNTOWNSURFACE\x15DDE"
    "RR_NOTINITIALIZED";
  load_type(ltf, 214, "MACRO_DDERR", p_type, p_fields);
}

static LocalTypes_1() 
{
  auto p_type, p_fields, p_cmt, p_fldcmts;

  p_type = "\x0D!\x0A\x01=\x09uint32_t=\x09uint32_t\x1B\x08=\x09uint32_t";
  p_fields = "\x19DestroyWindowFreeLibrary\x05tmp1\x05hwnd\x04tmp";
  load_type(ltf, 215, "Window", p_type, p_fields);
  p_type = "\x0D1\x0A\x01!!!!!";
  p_fields = "\x05func\x07dword4\x06maxMs\x06curMs\x08dword10\x08dword14";
  load_type(ltf, 217, "CallbackTimer", p_type, p_fields);
  p_type = "\x0D\x89\x01\x0A\x01=\x09uint16_t\x11\x11\x11\x10!=\x09uint32_t=\x08in"
    "t32_t=\x08uint8_t=\x08uint8_t=\x08uint8_t=\x08uint8_t=\x09uint32_t=\x09"
    "uint32_t=\x09uint32_t\x1B\x052";
  p_fields = "\x05Func\x05Year\x0BTotalHours\x06byte7\x06byte8\x06byte9\x07dwordC\x05"
    "Hour\x0EmsToTickClock\x07tmp2_1\x07tmp2_2\x07tmp2_3\x07tmp2_4\x0BbIsRu"
    "nning\x0AbQuitGame\x0DbStopTicking\x04tmp";
  load_type(ltf, 218, "WorldClock", p_type, p_fields);
  p_type = "#";
  load_type(ltf, 219, "uint16_t", p_type);
  p_type = "\"";
  load_type(ltf, 220, "byte", p_type);
  p_type = "\"";
  load_type(ltf, 221, "uint8_t", p_type);
}

//------------------------------------------------------------------------
// Information about local types

static LocalTypes()
{
  LocalTypes_0();
  LocalTypes_1();
}

// End of file.
