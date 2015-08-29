#include "main.h"
#include "AppDelegate.h"
#include "NetWorkThreadProc.h"
#include "CCEGLView.h"
#include <string>
#include "atlconv.h"
#include <vector>
#include <sstream>
#include <dbghelp.h>
#pragma comment( lib, "dbghelp.lib" )


using namespace std;

USING_NS_CC;


//û��
void dump_callstack( CONTEXT *context )
{
	printf( "%s\n", __FUNCTION__ );

	STACKFRAME sf;
	memset( &sf, 0, sizeof( STACKFRAME ) );

	sf.AddrPC.Offset = context->Eip;
	sf.AddrPC.Mode = AddrModeFlat;
	sf.AddrStack.Offset = context->Esp;
	sf.AddrStack.Mode = AddrModeFlat;
	sf.AddrFrame.Offset = context->Ebp;
	sf.AddrFrame.Mode = AddrModeFlat;

	DWORD machineType = IMAGE_FILE_MACHINE_I386;

	HANDLE hProcess = GetCurrentProcess();
	HANDLE hThread = GetCurrentThread();

	for( ; ; )
	{
		if( !StackWalk(machineType, hProcess, hThread, &sf, context, 0, SymFunctionTableAccess, SymGetModuleBase, 0 ) )
		{
			break;
		}

		if( sf.AddrFrame.Offset == 0 )
		{
			break;
		}
		BYTE symbolBuffer[ sizeof( SYMBOL_INFO ) + 1024 ];
		PSYMBOL_INFO pSymbol = ( PSYMBOL_INFO ) symbolBuffer;

		pSymbol->SizeOfStruct = sizeof( symbolBuffer );
		pSymbol->MaxNameLen = 1024;

		DWORD64 symDisplacement = 0;
		if( SymFromAddr( hProcess, sf.AddrPC.Offset, 0, pSymbol ) )
		{
			printf( "Function : %s\n", pSymbol->Name );
		}
		else
		{
			printf( "SymFromAdd failed!\n" );
		}

		IMAGEHLP_LINE lineInfo = { sizeof(IMAGEHLP_LINE) };
		DWORD dwLineDisplacement;

		if( SymGetLineFromAddr( hProcess, sf.AddrPC.Offset, &dwLineDisplacement, &lineInfo ) )
		{
			printf( "[Source File : %s]\n", lineInfo.FileName ); 
			printf( "[Source Line : %u]\n", lineInfo.LineNumber ); 
		}
		else
		{
			printf( "SymGetLineFromAddr failed!\n" );
		}
	}
}
//û��
DWORD excep_filter( LPEXCEPTION_POINTERS lpEP )
{
	printf( "%s.\n",__FUNCTION__ );
	/**//// init dbghelp.dll
	if( SymInitialize( GetCurrentProcess(), NULL, TRUE ) )
	{
		printf( "Init dbghelp ok.\n" );
	}
	else
	{
		printf( "SymInitialize faild!\n");
	}

	dump_callstack( lpEP->ContextRecord );

	if( SymCleanup( GetCurrentProcess() ) )
	{
		printf( "Cleanup dbghelp ok.\n" );
	}else
	{
		printf( "SymCleanup faild!\n");
	}

	return EXCEPTION_EXECUTE_HANDLER;
}






LPTOP_LEVEL_EXCEPTION_FILTER filter;
long   __stdcall   exceptionCallback(_EXCEPTION_POINTERS*   excp);

// uncomment below line, open debug console
static int USE_WIN32_CONSOLE = 0;

// �ɴ����� -w 640 -h 960
int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);


	// �쳣����
	filter = SetUnhandledExceptionFilter(exceptionCallback);


	char timeString[128];
	char dayString[128];
	time_t timep = time(0); 
	tm* _tm = gmtime(&timep);
	{
		sprintf(timeString,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d",
			_tm->tm_year+1900,_tm->tm_mon+1,_tm->tm_mday,
			_tm->tm_hour+8,_tm->tm_min,_tm->tm_sec); // ��8��
		sprintf(dayString,"%.4d-%.2d-%.2d",
			_tm->tm_year+1900,_tm->tm_mon+1,_tm->tm_mday);
		//tm* _tm2 = localtime(&timep);
	}
	printf("\n");
	printf("\n");
	printf("\n");
	printf("========================================================================\n");
	printf("=====================LAUNCH [%s]=======================\n",timeString);
	printf("========================================================================\n");


	// Ĭ�ϵ�
	float scale = -1.0f; // ���������Զ�����
	float width = 640,height = 1152;
	int port = 0;
	string ip = "";
	string logPos = "txt"; //����ض���-log txt/window��



	// LPTSTR --> string
	USES_CONVERSION;
	string str = T2A(lpCmdLine);
	cout<<str<<endl;

	// ��������
	char* cmd = new char[str.length() + 1];
	memset(cmd,0,str.length() + 1);
	strncpy(cmd,str.c_str(),str.length());
	char *tokenPtr=strtok(cmd," ");
	while(tokenPtr!=NULL)
	{
		if (string(tokenPtr) == "-w")
		{
			char* value = strtok(NULL," ");
			// http://www.cplusplus.com/forum/general/8165/
			stringstream ss (stringstream::in | stringstream::out);
			string test =string(value);
			ss << test;
			float val; 
			ss >> val;  
			width = val;
		}
		else if (string(tokenPtr) == "-h")
		{
			char* value = strtok(NULL," ");
			// http://www.cplusplus.com/forum/general/8165/
			stringstream ss (stringstream::in | stringstream::out);
			string test =string(value);
			ss << test;
			float val; 
			ss >> val;  
			height = val;
		}
		else if (string(tokenPtr) == "-s")
		{
			char* value = strtok(NULL," ");
			stringstream ss (stringstream::in | stringstream::out);
			string test =string(value);
			ss << test;
			float val; 
			ss >> val;  
			scale = val;
		}
		else if (string(tokenPtr) == "-port")
		{
			char* _value = strtok(NULL," ");
			// http://www.cplusplus.com/forum/general/8165/
			stringstream ss (stringstream::in | stringstream::out);
			string test =string(_value);
			ss << test;
			float val; 
			ss >> val;  
			port = val;
		}
		else if (string(tokenPtr) == "-ip")
		{
			char* _value = strtok(NULL," ");
			ip =string(_value);
		}
		else if (string(tokenPtr) == "-log")
		{
			char* _value = strtok(NULL," ");
			logPos =string(_value);
		}

		tokenPtr=strtok(NULL," ");
	}

	//��־���
	{
		if (logPos == "window")
			USE_WIN32_CONSOLE = 1;

		if (USE_WIN32_CONSOLE == 1)
		{
			AllocConsole();
			freopen("CONIN$", "r", stdin);
			freopen("CONOUT$", "w", stdout);
			freopen("CONOUT$", "w", stderr);
		}
		else
		{
			CreateDirectory(TEXT("log"),NULL);
			string fileName = string("log//Netfight-")+dayString+string(".log");
			FILE* pFile = freopen(fileName.c_str(),"a+",stdout); // �ض���
			if (pFile == NULL)   
			{
				CCMessageBox("Can Not Create Log File","Waring");
			}
		}
	}



	NetWorkThreadProc::getProc()->initNetwork(ip.c_str(),port);

	// create the application instance
	AppDelegate app;
	CCEGLView* eglView = CCEGLView::sharedOpenGLView();    
	eglView->setViewName("NetFight");

	int winHeight = 0;
	int winWidth = 0;
	{
		LPRECT screenRect = new tagRECT();
		GetClientRect(GetDesktopWindow(),screenRect);
		winWidth = (screenRect->right-screenRect->left);
		winHeight = -(screenRect->top-screenRect->bottom);
		CCLOG("PC Screen Size = [%d,%d]",winWidth,winHeight);
		delete screenRect;
	}

	eglView->setFrameSize(width, height);
	if (scale < 0)
	{
		scale = ((float)winHeight)/eglView->getFrameSize().height;
		scale *= 0.9f;
	}

	eglView->setFrameZoomFactor(scale);
    int ret = CCApplication::sharedApplication()->run();

	if (USE_WIN32_CONSOLE == 1)
		FreeConsole();
	

	return ret;
}


long   __stdcall   exceptionCallback(_EXCEPTION_POINTERS*   excp)   
{   
	// һ��Ҫ��֤NetFight.win32.pdb��exe��һ���汾
	// һ�α�����ϵĽ���������ͬ�汾���޷�����
	CCMessageBox("Sorry: Unhandler exception crash.. \ndump file[NetFight.dmp].\nPlease send it to developer","Error");

	HANDLE hFile = ::CreateFile( _T("NetFight.dmp"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if( hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION einfo;
		einfo.ThreadId = ::GetCurrentThreadId();
		einfo.ExceptionPointers = excp;
		einfo.ClientPointers = FALSE;

		::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, MiniDumpNormal, &einfo, NULL, NULL);
		::CloseHandle(hFile);
	}
	// ע��
	SetUnhandledExceptionFilter(filter);
	return EXCEPTION_EXECUTE_HANDLER;






	//// ע��
	//SetUnhandledExceptionFilter(filter);
	//printf("Error   address   %x/n",excp->ExceptionRecord->ExceptionAddress);   
	//printf("CPU   register:/n");   
	//printf("eax   %x   ebx   %x   ecx   %x   edx   %x/n",excp->ContextRecord->Eax,   
	//	excp->ContextRecord->Ebx,excp->ContextRecord->Ecx,   
	//	excp->ContextRecord->Edx);   
	//return   EXCEPTION_EXECUTE_HANDLER;   
}
