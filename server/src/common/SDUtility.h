/********************************************************************
	created:		2005/08/27
	created:		27:8:2005   15:31
	filename: 		d:\work\newcvs\chub\impl\src\common\sdutility.h
	file path:		d:\work\newcvs\chub\impl\src\common
	file base:		sdutility
	file ext:		h
	author:			lifeng
	description:	SDUtility
*********************************************************************/

#ifndef SANDAI_C_SDUTILITY_H_200508271531
#define SANDAI_C_SDUTILITY_H_200508271531

#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "SDTypes.h"
using namespace std;

class SDUtility  
{
public:
	static unsigned g_power(unsigned int mother, unsigned int power);

	static bool isIPAddress(const char *s);

	static char* iptostr(unsigned int ip);

	static unsigned int iptoint(const char *s);

	static char * strtoupper(char* str);

	static char * strtolower(char* str);

	static char * strrev (char * string);

	static char * safeCopyStr(char *dst, const char* src, int maxlen);

	static char * safeStrCat(char *dst, const char* src, int maxlen);

	static bool isEmptyStr(const char* str);

	static bool  strToInt(const char * str, int &out);
	
	static bool intToStr(int iInt, char *sStr);

	static bool get_pre_date(unsigned& year,unsigned& month,unsigned& day,unsigned timerevise = 0);

	static bool get_this_date(unsigned& year,unsigned& month,unsigned& day,unsigned timerevise = 0);
	static int get_this_date();

	static int get_curhour();
	static int get_curdate();

	static bool get_next_date(unsigned& year,unsigned& month,unsigned& day,unsigned timerevise = 0);

	static unsigned get_nextdate_int();

	static unsigned get_predate_int();
	
	static unsigned get_curtime_int();

	static unsigned get_time();

	static string get_cur_4y2m2d();
	static string get_cur_4y2m2d2h2m();
	static string get_cur_4y2m2d2h2m2s();
	
	static unsigned get_curdate_int();

	static string get_cur_datestring();

	static unsigned get_peer_ip(int sock_fd);
	static unsigned get_peer_port(int sock_fd);
	static void get_peer_ipport(int sock_fd,unsigned& ip,unsigned& port);

//	static string get_nextdate();

//	static string get_curdate();

	static unsigned long long get_cur_millseconds();

	static string buffer_to_str(const char* buffer, int buffer_size);

	/*static const int CID_SIZE = 20;
	static int hex_2_int(char chr);
	static std::string char_2_hex( unsigned char c );
	static std::string get_localhost();
	static std::string get_netmask(const std::string &localip);	
	static _u64 string_to_uint64( const std::string& value );

	*/
	//for transformation
	static std::string format(int i);
	static std::string format(unsigned int i);
	static std::string format(__u32 i);
	static std::string format(__u64 i);
	static std::string format(double db);


	static std::string uint64_2_string(__u64 value );
	/*
	static bool is_hex( char c );
	static std::string cid_to_str( unsigned char* cid );
	static std::string buffer_to_str(char* buffer, int buffer_size);

	static std::string get_cur_datestring();

	static std::string get_peer_name(int sock_fd);
	*/

	static unsigned make_version_nated(bool isnate,bool issupposeP2P);

	static bool farse_dbpool(const string& str,string& host,string& user,string& pwd,string&dbname,int& port,int& pool_size);
	static bool farse_dbdaemon(const string& str,string& host,int& port,int& que_size);
	static bool farse_string(const string& str,int& para1,int& para2);


	static bool farse_string(const string& str,string& par1,string& par2);
	static bool farse_string(const string& str,string& par1,string& par2,string& par3);
	static bool farse_string(const string& str,string& par1,string& par2,string& par3,string& par4);
	static bool farse_string(const string& str,string& par1,string& par2,string& par3,string&par4,string& par5);
	static bool farse_string(const string& str,string& par1,string& par2,string& par3,string&par4,string& par5,string& par6);

    static int hex_2_int(char chr);

    static bool is_hex(char chr);
protected:
	//bool is_special_address( const std::string& ip_address )
private:
};

#endif

