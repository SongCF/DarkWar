#include <time.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include "SDUtility.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>

const char NULL_CHAR = '\0';
using namespace std;

bool SDUtility::isIPAddress(const char *s)
{
	const char *pChar;
	bool rv = true;
	int tmp1, tmp2, tmp3, tmp4, i;

	while( 1 )
	{
		i = sscanf(s, "%d.%d.%d.%d", &tmp1, &tmp2, &tmp3, &tmp4);

		if( i != 4 )
		{
			rv = false;
			break;
		}

		if( (tmp1 > 255) || (tmp2 > 255) || (tmp3 > 255) || (tmp4 > 255) )
		{
			rv = false;
			break;
		}

		for( pChar = s; *pChar != '\0' ; pChar++ )
		{
			if( (*pChar != '.') 
				&& ((*pChar < '0') || (*pChar > '9')) )
			{
				rv = false;
				break;
			}
		}
		break;
	}

	return rv;
}

char* SDUtility::iptostr(unsigned int ip)
{
	struct in_addr in;
	in.s_addr = ip;
	return inet_ntoa(in);
}

unsigned int SDUtility::iptoint(const char *s)
{
	if (!isIPAddress(s))
	{
	//	return 0;   // 0 IP地址表示该地址不存在
		return inet_addr("127.0.0.1");
	}

	return inet_addr(s);
}

char * SDUtility::strtoupper(char* str)
{
	char * original = str;
	while (*str)
	{
		*str = toupper(*str);
		str++;
	}

	return original;
}

char * SDUtility::strtolower(char* str)
{
	char * original = str;
	while (*str)
	{
		*str = tolower(*str);
		str++;
	}

	return original;
}

char * SDUtility::strrev (char * string)
{
	char *start = string;
	char *left = string;
	char ch;

	while (*string++)				 /* find end of string */
			;
	string -= 2;

	while (left < string)
	{
		ch = *left;
		*left++ = *string;
		*string-- = ch;
	}

	return(start);
}

char * SDUtility::safeCopyStr(char *dst, const char* src, int maxlen)
{
	if (maxlen < 0)
	{
		return NULL;
	}
	
	// 只有目标缓冲区指针不为空才能拷贝
	if (dst && dst != src)
	{
		int len = 0;

		if (src)
		{
			int temp = strlen(src);
			len = (temp <= maxlen-1) ? temp : (maxlen-1);
			//strncpy(dst, src, len); 
			memcpy(dst, src, len);
		}

		dst[len] = NULL_CHAR;
	}

	return dst;
}


//maxlen is the max size of dst, including '\0'
char * SDUtility::safeStrCat(char *dst, const char* src, int maxlen)
{
	if (dst == NULL || src == NULL || maxlen <= 0)
	{
		return NULL;
	}

	if (strlen(dst) >= (unsigned)(maxlen-1))
		return dst;

	if (strlen(dst) + strlen(src) < unsigned(maxlen))
	{
		strcat(dst, src);
	}
	else
	{
		int len = maxlen - strlen(dst) -1;
		strncat(dst, src, len);
		dst[maxlen-1] = NULL_CHAR;
	}
	
	return dst;
}


bool SDUtility::isEmptyStr(const char* str)
{
	return (bool)(NULL == str || 0 == strlen(str));
}


 //将字符串转变为整数
bool  SDUtility::strToInt(const char * str, int &out)
{
	int strLen = strlen(str);
	if (strLen == 0)
	{
		out = 0;
		return false;
	}
	int iResult = 0;
	int i = 0;
	int iHex;
	unsigned int iPower = 0;
	//前面的两个字节是"0x""，不要转换
	for(i = strLen - 1; i >= 2; i--)
	{
		unsigned char cCurChar = str[i];
		if (cCurChar >= '0' && cCurChar <= '9')
		{
			iHex = cCurChar - '0';
		}
		else if (cCurChar >= 'A' && cCurChar <= 'F')
		{
			iHex = cCurChar - 'A' + 10;
		}
		else if (cCurChar >= 'a' && cCurChar <= 'f')
		{
			iHex = cCurChar - 'a' + 10;
		}
		else
		{
			//转换失败
			out = 0;
			return false;
		}
		if (iPower == 0)
		{
			iResult += iHex;
		}
		else
		{
			iResult += iHex * g_power(16, iPower);
		}
		iPower++;
		
	}
	out = iResult;
	return true;
}

unsigned SDUtility::g_power(unsigned int mother, unsigned int power)
{
    unsigned int i = 0;
    unsigned int iResult = 1;
    
    for( i = 0; i < power; i++)
    {
        iResult = iResult * mother;
    }
    return iResult;
}
 
bool SDUtility::intToStr(int iInt, char *sStr)
{
	  char tempStr[200];
	  memset(tempStr,0,sizeof(tempStr));
  
	  int tempInt1 = iInt;
	  int tempInt2;
	  int i;
	  int iLen = 1;
	  //获取整形的长度
	  if (tempInt1 < 10 && tempInt1 >= 1)
	  {
		 iLen = 1;
	  }
	  else
	  {
		  for (i = 0; i < 200; i++)
		  {
			  tempInt1 =tempInt1/10;
			  if ( tempInt1 < 10 && tempInt1 >= 1)
			  {
				 iLen++;
				 break;
			  }
			  else
			  {
				 iLen++;
			  }
		  }
	  }
	  //开始转化整形为字符串类型
	  tempInt1 = iInt;
	  tempInt2 = iInt;
	  for (i = iLen-1; i >= 0; i--)
	  {
		  tempInt1 = tempInt1 / 10;
		  tempInt2 = tempInt2 % 10;
		  tempStr[i] = '0' + tempInt2;
		  tempInt2 = tempInt1;
	  }
	  strncpy(sStr, tempStr, iLen);
	  sStr[iLen] = '\0';
  
	  return true;
  }
  
string SDUtility::get_cur_datestring()
{
	time_t cur_time = time(NULL);
	string curdate = string(ctime(&cur_time), 0, 10);
	curdate = "_"+curdate.substr(4,3)+"_"+curdate.substr(8,2);
	if(curdate[5] == 0x20)
		curdate[5] = '0';
	return curdate;
}

unsigned SDUtility::get_curdate_int()
{	
	struct timeval  tp;
	struct tm tf;
	
	if (-1 == gettimeofday(&tp, NULL))
	{
		return 0;
	}
	
	localtime_r(&(tp.tv_sec), &tf);
	return ( ( tf.tm_year + 1900 ) * 10000 + ( tf.tm_mon + 1 ) * 100 + tf.tm_mday );
}

unsigned SDUtility::get_curtime_int()
{
	struct timeval  tp;
	struct tm tf;
	
	if (-1 == gettimeofday(&tp, NULL))
	{
		return 0;
	}
	
	localtime_r(&(tp.tv_sec), &tf);	
	
	localtime_r(&(tp.tv_sec), &tf);

	return (tf.tm_hour * 3600 + tf.tm_min * 60 +  tf.tm_sec);
}

unsigned SDUtility::get_time()
{
	return time(NULL);
}

/*
string SDUtility::get_curdate()
{
	string str;
	struct timeval  tp;
	struct tm tf;
	
	if (-1 == gettimeofday(&tp, NULL))
	{
		return str;
	}
	
	localtime_r(&(tp.tv_sec), &tf);
	
	char buffer[20] = {0};
	sprintf(buffer,"%04d%02d%02d",tf.tm_year + 1900,tf.tm_mon + 1,tf.tm_mday);
	str = buffer;
	
	return str;	
}
*/

unsigned SDUtility::get_peer_ip(int sock_fd)
{
	struct sockaddr_in peer_addr;
	int len = sizeof(peer_addr);
	if(!getpeername(sock_fd, (struct sockaddr*)&peer_addr, (socklen_t*)&len))
	{
	//	return string(inet_ntoa(peer_addr.sin_addr));
		return unsigned(peer_addr.sin_addr.s_addr);
	}
	else
	{
		return 0;
	}
}

unsigned SDUtility::get_peer_port(int sock_fd)
{
	struct sockaddr_in peer_addr;
	int len = sizeof(peer_addr);
	if(!getpeername(sock_fd, (struct sockaddr*)&peer_addr, (socklen_t*)&len))
	{
		return unsigned(peer_addr.sin_port);
	}
	else
	{
		return 0;
	}
}

void SDUtility::get_peer_ipport(int sock_fd,unsigned& ip,unsigned& port)
{
	ip = 0;
	port = 0;
	struct sockaddr_in peer_addr;
	int len = sizeof(peer_addr);
	if(!getpeername(sock_fd, (struct sockaddr*)&peer_addr, (socklen_t*)&len))
	{
		ip = unsigned(peer_addr.sin_addr.s_addr);
		port = unsigned(peer_addr.sin_port);
	}
}

string SDUtility::get_cur_4y2m2d()
{
	string str;
	struct timeval  tp;
	struct tm tf;
	
	if (-1 == gettimeofday(&tp, NULL))
	{
		return str;
	}
	
	localtime_r(&(tp.tv_sec), &tf);
	char buffer[20] = {0};
	sprintf(buffer,"%04d%02d%02d",tf.tm_year + 1900,tf.tm_mon + 1,	tf.tm_mday);
	str = buffer;
	return str;
}

string SDUtility::get_cur_4y2m2d2h2m()
{
	string str;
	struct timeval  tp;
	struct tm tf;
	
	if (-1 == gettimeofday(&tp, NULL))
	{
		return str;
	}
	char buffer[20] = {0};
	
	localtime_r(&(tp.tv_sec), &tf);
	sprintf(buffer,"%04d%02d%02d%02d%02d",tf.tm_year + 1900,tf.tm_mon + 1,
		tf.tm_mday,tf.tm_hour,tf.tm_min);
	str = buffer;
	return str;
}

string SDUtility::get_cur_4y2m2d2h2m2s()
{
	string str;
	struct timeval  tp;
	struct tm tf;
	
	if (-1 == gettimeofday(&tp, NULL))
	{
		return str;
	}
	
	localtime_r(&(tp.tv_sec), &tf);
	char buffer[20] = {0};
	sprintf(buffer,"%04d%02d%02d%02d%02d%02d",tf.tm_year + 1900,tf.tm_mon + 1,
		tf.tm_mday,tf.tm_hour,tf.tm_min,tf.tm_sec);
	str = buffer;
	return str;
}

/*string SDUtility::get_nextdate()
{
	string str;
	struct timeval  tp;
	struct tm tf;
	
	if (-1 == gettimeofday(&tp, NULL))
	{
		return str;
	}

	//next day
	tp.tv_sec += 86400;
	
	localtime_r(&(tp.tv_sec), &tf);
	char buffer[20] = {0};
	sprintf(buffer,"%04d%02d%02d",tf.tm_year + 1900,tf.tm_mon + 1,tf.tm_mday);
	str = buffer;
	return str;	
}*/

unsigned SDUtility::get_predate_int()
{
	struct timeval  tp;
	struct tm tf;
	
	if (-1 == gettimeofday(&tp, NULL))
	{
		return 0;
	}

	tp.tv_sec -= 86400;	//3600 * 24 表示24个小时
	
	localtime_r(&(tp.tv_sec), &tf);
	return ( ( tf.tm_year + 1900 ) * 10000 + ( tf.tm_mon + 1 ) * 100 + tf.tm_mday );
}

unsigned SDUtility::get_nextdate_int()
{
	struct timeval  tp;
	struct tm tf;
	
	if (-1 == gettimeofday(&tp, NULL))
	{
		return 0;
	}

	tp.tv_sec += 86400;	//3600 * 24 表示24个小时
	
	localtime_r(&(tp.tv_sec), &tf);
	return ( ( tf.tm_year + 1900 ) * 10000 + ( tf.tm_mon + 1 ) * 100 + tf.tm_mday );
}

unsigned long long SDUtility::get_cur_millseconds()
{
	struct timeval now;
	gettimeofday(&now, NULL);
	return (unsigned long long)now.tv_sec * 1000 + now.tv_usec / 1000;
}


bool SDUtility::get_this_date(unsigned& year,unsigned& month,unsigned& day,unsigned timerevise)
{
	struct timeval  tp;
	struct tm tf;
	
	if (-1 == gettimeofday(&tp, NULL))
	{
		return false;
	}

	tp.tv_sec += timerevise;
	
	localtime_r(&(tp.tv_sec), &tf);

	
	year =  tf.tm_year + 1900;
	month = tf.tm_mon + 1;
	day = tf.tm_mday;
	
	return true;
}

int SDUtility::get_this_date()
{
	struct timeval  tp;
	struct tm tf;
	
	if (-1 == gettimeofday(&tp, NULL))
	{
		return 0;
	}	
	localtime_r(&(tp.tv_sec), &tf);	
	return tf.tm_mday;
}

int SDUtility::get_curhour()
{
	struct timeval  tp;
	struct tm tf;
	
	if (-1 == gettimeofday(&tp, NULL))
	{
		return 0;
	}
	
	localtime_r(&(tp.tv_sec), &tf);
	
	return tf.tm_hour;
}

int SDUtility::get_curdate()
{
	struct timeval  tp;
	struct tm tf;
	
	if (-1 == gettimeofday(&tp, NULL))
	{
		return 0;
	}
	
	localtime_r(&(tp.tv_sec), &tf);
	
	return tf.tm_mday;
}

bool SDUtility::get_pre_date(unsigned& year,unsigned& month,unsigned& day,unsigned timerevise)
{
	struct timeval  tp;
	struct tm tf;
	
	if (-1 == gettimeofday(&tp, NULL))
	{
		return false;
	}

	tp.tv_sec += timerevise;
	tp.tv_sec -= 86400;
	
	localtime_r(&(tp.tv_sec), &tf);

	
	year =  tf.tm_year + 1900;
	month = tf.tm_mon + 1;
	day = tf.tm_mday;
	
	return true;
}

bool SDUtility::get_next_date(unsigned& year,unsigned& month,unsigned& day,unsigned timerevise)
{
	struct timeval  tp;
	struct tm tf;
	
	if (-1 == gettimeofday(&tp, NULL))
	{
		return false;
	}

	tp.tv_sec += timerevise;
	tp.tv_sec += 86400;
	
	localtime_r(&(tp.tv_sec), &tf);

	
	year =  tf.tm_year + 1900;
	month = tf.tm_mon + 1;
	day = tf.tm_mday;
	
	return true;
}


string SDUtility::buffer_to_str(const char* buffer, int buffer_size)
{
	string ret_str;
	char buf[40] = {0};
	sprintf(buf,"buffer_len=[%d],data= ",buffer_size);
	ret_str += buf;
	
	for(int i = 0 ;i < buffer_size; i++)
	{
		sprintf(buf,"%02X ",(unsigned char)buffer[i]);		
		ret_str += buf;
	}	
	return ret_str;
}

/*
int SDUtility::hex_2_int(char chr)
{
	if(chr >= '0' && chr <= '9')
	{
		return chr - '0';
	}
	if(chr >= 'a' && chr <= 'f')
	{
		return chr - 'a' + 10;
	}
	if(chr >= 'A' && chr <= 'F')
	{
		return chr - 'A' + 10;
	}
	return 0;
}

bool SDUtility::is_hex(char chr)
{
	if(chr >= '0' && chr <= '9')
	{
		return true;
	}
	if(chr >= 'a' && chr <= 'f')
	{
		return true;
	}
	if(chr >= 'A' && chr <= 'F')
	{
		return true;
	}
	return false;
}

_u64 SDUtility::string_to_uint64( const std::string& value )
{
	_u64 result=0;
	string::const_iterator  it = value.begin();
	while( it != value.end() )
	{
		if( (*it>='0') && (*it<='9') )
			result = result*10 + (*it-'0');
		else
		{
			result = 0;
			break;
		}

		it++;
	}
	return result;
}

string SDUtility::char_2_hex( unsigned char c )
{
	string result;
	int mask = 0X000000F0;
	int temp =  c;
	temp &= mask;
	temp >>= 4;
	if( temp > 9 )
	{
		result.append( 1, 'A'+temp-10 );
	}
	else
	{
		result.append( 1, '0' + temp );
	}

	temp = c;
	mask >>= 4;
	temp &= mask;

	if( temp > 9 )
	{
		result.append( 1, 'A'+temp-10 );
	}
	else
	{
		result.append( 1, '0' + temp );
	}
	return result;
}

*/

string SDUtility::format(int i)
{
	char buff[100];
	sprintf(buff, "%d", i);
	return string(buff);
}

string SDUtility::format(double db)
{
	char buff[100];
	sprintf(buff, "%f", db);
	return string(buff);
}

string SDUtility::format(unsigned int i)
{
	char buff[100];
	sprintf(buff, "%u", i);
	return string(buff);
}

string SDUtility::format(__u32 i)
{
	return format((unsigned)i);
}

unsigned SDUtility::make_version_nated(bool isnate,bool issupposeP2P)
{
	return (issupposeP2P << 1 | isnate);
}

string SDUtility::uint64_2_string(__u64 value )
{
	char buf[50] ={0};
	sprintf(buf,"%llu",value);
	return string(buf);
	/*
	if( value == 0 )
		return "0";
	string temp;
	while( value != 0 )
	{
		int remainder = value%10;
		char c = '0';
		c += remainder;
		string::size_type pos = 0;
		string::size_type count = 1;
		temp.insert( pos, count, c );
		value = value/10;
	}
	return temp;
	*/
}

string SDUtility::format(__u64 i)
{
	return uint64_2_string(i);
}

/*
string SDUtility::cid_to_str( unsigned char* cid )
{
	string content_id_str;
	for( int i=0; i<CID_SIZE; i++ )
		content_id_str.append(char_2_hex(cid[i]));
	return content_id_str;
}

string SDUtility::get_cur_datestring()
{
	time_t cur_time = time(NULL);
	string curdate = string(ctime(&cur_time), 0, 10);
	curdate = "_"+curdate.substr(4,3)+"_"+curdate.substr(8,2);
	if(curdate[5] == 0x20)
		curdate[5] = '0';
	return curdate;
}

string SDUtility::get_peer_name(int sock_fd)
{
	struct sockaddr_in peer_addr;
	int len = sizeof(peer_addr);
	if(!getpeername(sock_fd, (struct sockaddr*)&peer_addr, (socklen_t*)&len))
		return string(inet_ntoa(peer_addr.sin_addr));
	else
		return "";
}

*/

bool SDUtility::farse_dbpool(const string& str,string& host,string& user,string& pwd,string&dbname,int& port,int& pool_size)
{
	int n1 = str.find(":");
	int n2 = str.find(":",n1 + 1);	
	int n3 = str.find(":",n2  + 1);
	int n4 = str.find(":",n3 + 1);	
	int n5 = str.find(":",n4 + 1);
		
	if(n1 < 0 || n2 < 0 || n3 < 0 || n4 < 0 || n5 < 0)
	{
		return false;
	}
	
	
	
	host = str.substr(0,n1);
	user = str.substr(n1+1,n2-n1-1);
	pwd = str.substr(n2+1,n3-n2-1);
	dbname = str.substr(n3+1,n4-n3-1);
	port = atoi(str.substr(n4+1,n5-n4-1).c_str());
	pool_size = atoi(str.substr(n5+1).c_str());

	return true;
}

bool SDUtility::farse_dbdaemon(const string& str,string& host,int& port,int& que_size)
{
	int n1 = str.find(":");
	int n2 = str.find(":",n1 + 1);	
		
	if(n1 < 0 || n2 < 0)
	{
		return false;
	}
	
	
	
	host = str.substr(0,n1);
	port = atoi(str.substr(n1+1,n2-n1-1).c_str());
	que_size = atoi(str.substr(n2+1).c_str());

	return true;
}

bool SDUtility::farse_string(const string& str,int& para1,int& para2)
{
	int n1 = str.find(":");		
	if(n1 < 0)
	{
		return false;
	}
	
	para1 = atoi(str.substr(0,n1).c_str());
	para2 = atoi(str.substr(n1+1).c_str());
	return true;
}

bool SDUtility::farse_string(const string& str,string& par1,string& par2,string& par3)
{
	int n1 = str.find(":");
	int n2 = str.find(":",n1 + 1);	
		
	if(n1 < 0 || n2 < 0)
	{
		return false;
	}
	
	
	
	par1 = str.substr(0,n1);
	par2 = str.substr(n1+1,n2-n1-1).c_str();
	par3 = str.substr(n2+1).c_str();

	return true;
}

bool SDUtility::farse_string(const string& str,string& par1,string& par2,string& par3,string& par4)
{
	int n1 = str.find(":");
	int n2 = str.find(":",n1 + 1);	
	int n3 = str.find(":",n2  + 1);
		
	if(n1 < 0 || n2 < 0 || n3 < 0)
	{
		return false;
	}
	
	par1 = str.substr(0,n1);
	par2 = str.substr(n1+1,n2-n1-1);
	par3 = str.substr(n2+1,n3-n2-1);
	par4 = str.substr(n3+1).c_str();
	return true;
}

bool SDUtility::farse_string(const string& str,string& par1,string& par2,string& par3,string&par4,string& par5)
{
	int n1 = str.find(":");
	int n2 = str.find(":",n1 + 1);	
	int n3 = str.find(":",n2  + 1);
	int n4 = str.find(":",n3 + 1);			
	if(n1 < 0 || n2 < 0 || n3 < 0 || n4 < 0)
	{
		return false;
	}
	
	
	
	par1 = str.substr(0,n1);
	par2 = str.substr(n1+1,n2-n1-1);
	par3 = str.substr(n2+1,n3-n2-1);
	par4 = str.substr(n3+1,n4-n3-1);
	par5 = str.substr(n4+1).c_str();

	return true;
}


bool SDUtility::farse_string(const string& str,string& par1,string& par2,string& par3,string&par4,string& par5,string& par6)
{
	int n1 = str.find(":");
	int n2 = str.find(":",n1 + 1);	
	int n3 = str.find(":",n2  + 1);
	int n4 = str.find(":",n3 + 1);	
	int n5 = str.find(":",n4 + 1);
		
	if(n1 < 0 || n2 < 0 || n3 < 0 || n4 < 0 || n5 < 0)
	{
		return false;
	}
	
	
	
	par1 = str.substr(0,n1);
	par2 = str.substr(n1+1,n2-n1-1);
	par3 = str.substr(n2+1,n3-n2-1);
	par4 = str.substr(n3+1,n4-n3-1);
	par5 = str.substr(n4+1,n5-n4-1).c_str();
	par6 = str.substr(n5+1).c_str();

	return true;
}

int SDUtility::hex_2_int(char chr)
{
	if(chr >= '0' && chr <= '9')
	{
		return chr - '0';
	}
	if(chr >= 'a' && chr <= 'f')
	{
		return chr - 'a' + 10;
	}
	if(chr >= 'A' && chr <= 'F')
	{
		return chr - 'A' + 10;
	}
	return 0;
}

bool SDUtility::is_hex(char chr)
{
	if(chr >= '0' && chr <= '9')
	{
		return true;
	}
	if(chr >= 'a' && chr <= 'f')
	{
		return true;
	}
	if(chr >= 'A' && chr <= 'F')
	{
		return true;
	}
	return false;
}

