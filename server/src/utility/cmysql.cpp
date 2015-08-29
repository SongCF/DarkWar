#include <time.h>
#include "cmysql.h"

//#define RECONNECT_PING_TIME 3600

CMysql:: CMysql()
{
	m_bConnected = false;
	m_iField = 0;
	m_result = NULL;
	bzero (m_szHost, sizeof(m_szHost));
	bzero (m_szUser, sizeof(m_szUser));
	bzero (m_szPass, sizeof(m_szPass));
	m_bFieldIndexInitialized = false;
	m_bIfConfig=false;

    m_ErrMsg = new char[1024];
}


CMysql:: CMysql(const char* szHost, const char* szUser, const char* szPass, bool setUTF8)
{
	m_bConnected = false;
	m_iField = 0;
	m_result = NULL;
	m_bFieldIndexInitialized = false;
	m_bIfConfig=false;
	strncpy(m_szHost, szHost, 64);
    m_szHost[64] = 0;
    strncpy(m_szUser, szUser, 64);
    m_szUser[64] = 0;
    strncpy(m_szPass, szPass, 64);
    m_szPass[64] = 0;

    m_ErrMsg = new char[1024];

    m_setUTF8 = setUTF8;
}

CMysql:: ~CMysql()
{
    delete[] m_ErrMsg;
	Close();
}

int CMysql:: Close()
{
	if (m_bConnected)
	{
		FreeResult();
		mysql_close(&m_connection);
		m_bConnected= false;
	}
	return 0;
}

int CMysql:: EscapeString (string &str)
{
	if (str.size()==0) return 0;

	char *buff= new char[str.size()*2+1];
	mysql_real_escape_string (&m_connection, buff, str.c_str(), str.size());
	str= buff;
	delete[] buff;
	return 0;
}

void CMysql::Escape(string & str_out, char * buf_in, int buf_in_len)
{
    str_out = "";
    if(buf_in_len <= 0)
    {
        return;
    }

	char *buff= new char[buf_in_len*2+1];
	mysql_real_escape_string (&m_connection, buff, buf_in, buf_in_len);
	str_out = buff;
	delete[] buff;
    
	return;
}

int CMysql:: Connect(const char* szHost, const char* szUser, const char* szPass)
{
    strncpy(m_szHost, szHost, 64);
    m_szHost[64] = 0;
    strncpy(m_szUser, szUser, 64);
    m_szUser[64] = 0;
    strncpy(m_szPass, szPass, 64);
    m_szPass[64] = 0;
    
	Close();
	return Connect();
}

int CMysql:: Connect()
{
    /*
    static time_t reconnect_time = time(NULL);
    if((time(NULL) - reconnect_time) > RECONNECT_PING_TIME) { //定时ping数据库
        if(m_bConnected && mysql_ping(&m_connection) != 0) {
            //超时重连机制1
            Close();
        }
        reconnect_time = time(NULL);
    }
    */
    
	if (!m_bConnected)
	{
		mysql_init (&m_connection);
        
        ////超时重连机制2
        char value = 1;
        mysql_options(&m_connection, MYSQL_OPT_RECONNECT, (char *)&value);
        if(m_setUTF8) {
            const char *charset = "utf8";
            mysql_options(&m_connection, MYSQL_SET_CHARSET_NAME, charset);
        }
        ////////////
        
        
		if (mysql_real_connect(&m_connection, m_szHost, m_szUser, m_szPass, NULL, 0, NULL, 0) == NULL)
		{
			sprintf(m_ErrMsg, "connect[-h%s -u%s -p%s] fail.\nError %u (%s)\n",
					m_szHost, m_szUser, m_szPass,
					mysql_errno(&m_connection), mysql_error(&m_connection));
			throw CMysqlException(m_ErrMsg);
		}
		m_bConnected = true;

        /*if(m_setUTF8)
        {
            char sql_charset[64];
            sprintf(sql_charset, "set names utf8;");
            Query(sql_charset);
            FreeResult();
        }*/
	}
	return 0;
}

bool CMysql:: IfConnected(const char* szHost)
{
	if (m_bConnected)
		if (!strcmp(szHost, m_szHost))
			return true;
	return false;
}
		
int CMysql:: Query(char* szSqlString)
{
	Connect();
   
    FreeResult();
    
	if (mysql_real_query(&m_connection, szSqlString, strlen(szSqlString)) != 0)
	{
		sprintf(m_ErrMsg, "mysql query fail [%s].\nError=[%s]\nSQL=%s", m_szHost,  mysql_error(&m_connection), szSqlString);
        Close();
		throw CMysqlException(m_ErrMsg);
	}
	return 0;
}

int CMysql:: FreeResult()
{
	if (m_result != NULL)
		mysql_free_result (m_result);
	m_iField = 0;
	m_result = NULL;
	if (m_bFieldIndexInitialized)
	{
		m_FieldIndex.erase(m_FieldIndex.begin(), m_FieldIndex.end());
		m_bFieldIndexInitialized = false;
	}
	return 0;
}

int CMysql:: StoreResult()
{
	FreeResult();
	m_result = mysql_store_result (&m_connection);
	if (m_result == NULL)
	{
		sprintf(m_ErrMsg, "mysql_store_result :%s!", mysql_error(&m_connection));
		throw CMysqlException(m_ErrMsg);
	}
	m_iField = mysql_num_fields (m_result);
	m_iRows = mysql_num_rows (m_result);
	return 0;
}

char** CMysql:: FetchRow()
{
	if (m_result == NULL)
		StoreResult();
	m_row = mysql_fetch_row (m_result);
	return m_row;
}

int CMysql:: InitFieldName()
{
	if ((!m_bFieldIndexInitialized) && (m_result!=NULL))
	{
		unsigned int i;
		MYSQL_FIELD *fields;

		fields = mysql_fetch_fields(m_result);
		for(i = 0; i < m_iField; i++)
		{
			m_FieldIndex[fields[i].name] = i;
		}
		m_bFieldIndexInitialized = true;
	}
	return 0;
}

const char* CMysql:: GetFieldName(int iField)
{
	if (m_result==NULL) {
		return NULL;
	}
	MYSQL_FIELD *fields;
	fields = mysql_fetch_fields(m_result);
	if ((unsigned int)iField> m_iField) {
		return NULL;
	}
	return fields[iField].name;
}

/*
	返回结果的行数
*/
int CMysql:: GetAffectedRows()
{
	my_ulonglong iNumRows;

	if (!m_bConnected) return 0;
	iNumRows = mysql_affected_rows(&m_connection);

	return (int)iNumRows;
}

/* 返回最近一次insert的ID
*/
unsigned int CMysql:: GetLastInsertId ()
{
	return (unsigned int)mysql_insert_id(&m_connection);
}

/*
	按照字段名取回当前行的结果
*/
char* CMysql:: GetField(const char* szFieldName)
{
	InitFieldName();
    STRING2INT::iterator pos = m_FieldIndex.find(szFieldName);
    if(pos == m_FieldIndex.end())
    {
        std::string msg(szFieldName);
        msg = msg + " field not in sql";
        throw CMysqlException(msg.c_str());
    }
	return GetField(pos->second);
}

/*
	按照字段索引取回当前行的结果
*/
char* CMysql:: GetField(unsigned int iField)
{
	if (iField > m_iField)
		return NULL;
	return m_row[iField];
}

char* CMysql::GetFieldBinary(const char * szFieldName, int & iFieldLen)
{
    InitFieldName();

    STRING2INT::iterator pos = m_FieldIndex.find(szFieldName);
    if(pos == m_FieldIndex.end())
    {
        std::string msg(szFieldName);
        msg = msg + " field not in sql";
        throw CMysqlException(msg.c_str());
    }
    
	return GetFieldBinary(pos->second, iFieldLen);
}

char* CMysql::GetFieldBinary(unsigned int iField, int & iFieldLen)
{
    if (iField > m_iField)
		return NULL;

    //
    iFieldLen = mysql_fetch_lengths(m_result)[iField];
    
	return m_row[iField];
}


