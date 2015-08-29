#include "NetTextListShowUI.h"
#include "NetLabelTTF.h"
#include "font.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#include "iosExt.h"
#endif
typedef unsigned char	UTF8;	/* typically 8 bits */

NetTextListShowUI::NetTextListShowUI()
	:CCSprite()
	,m_bIsFirstText(false)
	,m_bThisText(false)
	,m_nCount(0)
	,m_bIsFirstLine(true)
	,m_bIsAddLine(false)
{
}
NetTextListShowUI::~NetTextListShowUI() {
}
bool NetTextListShowUI::init() {
	if (!CCSprite::init()) {
		return false;
	}
	m_mMsgString = "";
	m_nFontSize = 0;
	m_bIsMultiple = true;
	m_bIsFirstLine = true;
	m_textColor = ccWHITE;
	m_nMaxSizeNumber = 0;
	m_mMenu = CCMenu::create();
	m_mMenu->setContentSize(CCDirector::sharedDirector()->getWinSize());
	m_mMenu->setAnchorPoint(CCPointZero);
	m_mMenu->setPosition(CCPointZero);
	this->addChild(m_mMenu,3);

	m_uTalkColor = NULL;

	m_tMessageTTF = CCLabelTTF::create("",fontStr_kaiti,20.0f);
	m_tMessageTTF->setPosition(CCPointZero);
	m_tMessageTTF->setAnchorPoint(CCPointZero);
	addChild(m_tMessageTTF);
	return true;
}
void NetTextListShowUI::onEnter() {
	CCSprite::onEnter();

}

void NetTextListShowUI::onExit() {
	CCSprite::onExit();
}
string NetTextListShowUI::conversion(string str,int *count)
{
	if(!m_bIsFirstLine)
	{
		//return "";
	}
	UTF8 *utf8 = new UTF8[str.size() * 3];
	UTF8 *utf8_fp = utf8;
	memcpy(utf8,str.c_str(),str.size());
	long lenght = 0;
	string bString;
	//CCRect rect = mRect;
	NetLabelTTF *ttf = NetLabelTTF::create("","SimHei",m_nFontSize);
	CCRect bRect = ttf->getTextureRect();
	int size = str.size();
	string uStr = "";
	*count = 0;
	for(int i = 0 ; i < size ; i ++)
	{
		uStr = "";
		if (*utf8 >= 0xE0 && *utf8 <= 0xEF)// «3∏ˆ◊÷Ω⁄µƒ∏Ò Ω
		{
			uStr += *utf8++;
			i++;
			uStr += *utf8++;
			i++;
			uStr += *utf8++;
			lenght = 3;
			*count += 2;
			m_bIsFirstText = false;
		}
		else if (*utf8 >= 0xC0 && *utf8 <= 0xDF)// «2∏ˆ◊÷Ω⁄µƒ∏Ò Ω
		{
			uStr += *utf8++;
			i++;
			uStr += *utf8++;
			lenght = 2;
			*count += 2;
			m_bIsFirstText = false;
		}
		else if(*utf8 >= 0 && *utf8 <= 0x7F)// «1∏ˆ◊÷Ω⁄µƒ∏Ò Ω
		{
			uStr += *utf8++;
			lenght = 1;
			*count += 1;

			if (uStr.at(0) == '\n')
			{
				m_bIsFirstText = true;
				*count = 0;
			}
			else
			{
				m_bIsFirstText = false;
			}
		}
		string new_str = ttf->getString();
		new_str += uStr;
		ttf->setString(new_str.c_str());
		bRect = ttf->getTextureRect();
		if(bRect.size.width > m_nTextWidth)
		{
			if(lenght != 0)
			{
				if (uStr.at(0) == '\n')
				{
					bString += ttf->getString();
					ttf->setString("");
					*count = 0;
				}
				else
				{
					string new_str2 = new_str.substr(0,new_str.size() - lenght);
					if(!m_bIsMultiple)
					{
						ttf->setString(new_str2.c_str());
						break;
					}
					
					//new_str2 += "\n";
					new_str2 += "\n";
					bString += new_str2;
					new_str2 = new_str.substr(new_str.size() - lenght,new_str.size());
					ttf->setString(new_str2.c_str());
					if (lenght >= 2)
					{
						*count = 2;
					}
					else
					{
						*count = 1;
					}
					
				}
			}
			else
			{
				string new_str2 = new_str.substr(0,new_str.size() - 1);
				if(!m_bIsMultiple)
				{
					ttf->setString(new_str2.c_str());
					break;
				}
				new_str2 += "\n";
				bString += new_str2;
				new_str2 = new_str.substr(new_str.size() - 1,new_str.size());
				ttf->setString(new_str2.c_str());
				*count = 1;
			}
		}
		else if(bRect.size.width == m_nTextWidth)
		{
			if(lenght != 0)
			{
				if (uStr.at(0) == '\n')
				{
					bString += ttf->getString();
					ttf->setString("");
					*count = 0;
				}
				else
				{
					bString += ttf->getString();
					bString += '\n';
					ttf->setString("");
					*count = 0;
				}
			}
			else
			{
				string new_str2 = new_str.substr(0,new_str.size() - 1);
				if(!m_bIsMultiple)
				{
					ttf->setString(new_str2.c_str());
					break;
				}
				new_str2 += "\n";
				bString += new_str2;
				new_str2 = new_str.substr(new_str.size() - 1,new_str.size());
				ttf->setString(new_str2.c_str());
				*count = 1;
			}
		}
		lenght = 0;
	}
	bString += ttf->getString();
	delete[] utf8_fp;
	return bString;
}
void NetTextListShowUI::addMessage(string str)
{
	//str = formatting(str);
	while(str.size() > 0)
	{
		int pos = str.find('\n');
		str.erase(0,pos + 1);//»•µÙ[\n
		pos = str.find('\n');
		int type = atoi(str.substr(0,pos).c_str());
		str.erase(0,pos + 1);//»•µÙŒƒ◊÷¿‡–Õ
		pos = str.find("|\n");
		string text = str.substr(0,pos).c_str();//Œƒ±æƒ⁄»›
		str.erase(0,pos + 1);//»•µÙŒƒ◊÷ƒ⁄»›
		ccColor3B color;
		pos = str.find(',');
		color.r = atoi(str.substr(0,pos).c_str());
		str.erase(0,pos + 1);//»•µÙ—’…´
		pos = str.find(',');
		color.g = atoi(str.substr(0,pos).c_str());
		str.erase(0,pos + 1);//»•µÙ—’…´
		pos = str.find('\n');
		color.b = atoi(str.substr(0,pos).c_str());
		str.erase(0,pos + 1);//»•µÙ—’…´
		unsigned int callBackType;
		pos = str.find('\n');
		callBackType = atoi(str.substr(0,pos).c_str());
		str.erase(0,pos + 1);//»•µÙ ¬º˛¿‡–Õ
		unsigned int callBackId;
		pos = str.find('\n');
		callBackId = atoi(str.substr(0,pos).c_str());
		str.erase(0,pos + 1);//»•µÙ ¬º˛¿‡–Õ
		pos = str.find('\n');
		str.erase(0,pos + 1);//»•µÙ]\n
		
		addString(text,color);
	}	
}
void NetTextListShowUI::addString(string str,ccColor3B color)
{
	if (str.size() <= 0)
	{
		return;
	}
	m_bThisText = m_bIsFirstText;
	m_bIsFirstText = false;
	if (m_bIsFirstLine)
	{
		m_bIsFirstLine = false;
	}
	else
	{
        CCLabelTTF *ttf2 = CCLabelTTF::create("123",fontStr_kaiti,m_nFontSize);
        m_pCurPos.y += ttf2->getContentSize().height;

		//m_pCurPos.y += m_nFontSize;
	}
	
	int count = 0;

	if (m_nCount == 0)
	{
		string text = conversion(str,&count);

		int point_x = m_pCurPos.x;
		m_pCurPos.x += m_nFontSize / 2 * count;
		if (!m_bIsFirstText)
		{
			int count_old = (m_nTextWidth - m_pCurPos.x) * 2 / m_nFontSize;
			if ( count_old == 0)
			{
				m_bIsFirstText = true;
				text += "\n";
			}
		}
		
		
		CCLabelTTF *ttf = CCLabelTTF::create(text.c_str(),fontStr_kaiti,m_nFontSize);
		if (m_bThisText)
		{
            CCLabelTTF *ttf2 = CCLabelTTF::create("123",fontStr_kaiti,m_nFontSize);
            //m_pCurPos.y += ttf2->getContentSize().height;
            //CCLabelTTF *ttf2 = CCLabelTTF::create("123",fontStr_kaiti,m_nFontSize);
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
            if (iosExt::sharediosExt()->getSystemVersion() >= 6.9f) {
                m_pCurPos.y -= ttf2->getContentSize().height / 2;
            }
            else
            {
                m_pCurPos.y -= ttf2->getContentSize().height;
            }
#endif
            m_pCurPos.y -= ttf2->getContentSize().height;
		}
		
		MsgColorTexCoords msg;
		msg.color = ccc4(color.r,color.g,color.b,255);
		msg.mRect.origin.x = point_x;
		msg.mRect.origin.y = -m_pCurPos.y;
		msg.mRect.size = ttf->getContentSize();
		//msg.mRect.size.height = m_nFontSize;
		m_mMsgString += text;
		
		m_nCount = count;
		if (m_bIsFirstText)
		{
            //CCLabelTTF *ttf2 = CCLabelTTF::create("123",fontStr_kaiti,m_nFontSize);
			//msg.mRect.size.height -= ttf2->getContentSize().height;
			m_pCurPos.x = 0;
			m_nCount = 0;
		}
		m_pCurPos.y -= ttf->getTextureRect().size.height;
		/*if (count == 0 && (!m_bIsFirstText))
		{
			m_pCurPos.y -= m_nFontSize;
		}*/
		
		m_lMsgColorTexCoords.push_back(msg);
	}
	else
	{
		count = (m_nTextWidth - m_pCurPos.x) / m_nFontSize * 2;
		if(count == 0)
		{

		}
		else
		{
			{
				int new_count = 0;
				string bString = subString(str,count,new_count);
				CCLabelTTF *ttf = CCLabelTTF::create(bString.c_str(),fontStr_kaiti,m_nFontSize);
				MsgColorTexCoords msg;
				msg.color = ccc4(color.r,color.g,color.b,255);
				msg.mRect.origin.x = m_pCurPos.x;
				msg.mRect.origin.y = -m_pCurPos.y;
				msg.mRect.size = ttf->getContentSize();

				m_mMsgString += bString;
				if(new_count >= count)
				{
					m_pCurPos.x = 0;
                    //CCLabelTTF *ttf2 = CCLabelTTF::create("\n",fontStr_kaiti,m_nFontSize);
					//msg.mRect.size.height -= ttf2->getContentSize().height;
				}
				else
				{
					if (m_bIsFirstText)
					{
                        
                        //CCLabelTTF *ttf2 = CCLabelTTF::create("123",fontStr_kaiti,m_nFontSize);
                        //msg.mRect.size.height -= ttf2->getContentSize().height;
						m_pCurPos.x = 0;
						m_nCount = 0;
					}
					else
					{
						m_pCurPos.x += ttf->getTextureRect().size.width;
					}
					
				}
				m_lMsgColorTexCoords.push_back(msg);
				if (m_bIsAddLine)
				{
					str.erase(0,bString.size() - 1);
				}
				else
				{
					str.erase(0,bString.size());
				}
				
				if(str.size() > 0)
				{
                    CCLabelTTF *ttf2 = CCLabelTTF::create("123",fontStr_kaiti,m_nFontSize);
                    m_pCurPos.y -= ttf2->getContentSize().height;
					//m_pCurPos.y -= m_nFontSize;
				}
				else
				{
					m_pCurPos.y -= ttf->getTextureRect().size.height;
					return;
				}
			}
			{
				count = 0;
				m_bThisText = m_bIsFirstText;
				string text = conversion(str,&count);
                
                int point_x = m_pCurPos.x;
                m_pCurPos.x += m_nFontSize / 2 * count;
                if (!m_bIsFirstText)
                {
                    int count_old = (m_nTextWidth - m_pCurPos.x) * 2 / m_nFontSize;
                    if ( count_old == 0)
                    {
                        m_bIsFirstText = true;
                        text += "\n";
                    }
                }
                
                
                CCLabelTTF *ttf = CCLabelTTF::create(text.c_str(),fontStr_kaiti,m_nFontSize);
                if (m_bThisText)
                {
                    CCLabelTTF *ttf2 = CCLabelTTF::create("123",fontStr_kaiti,m_nFontSize);
                    //m_pCurPos.y += ttf2->getContentSize().height;
                    //CCLabelTTF *ttf2 = CCLabelTTF::create("123",fontStr_kaiti,m_nFontSize);
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
                    if (iosExt::sharediosExt()->getSystemVersion() >= 6.9f) {
                        m_pCurPos.y -= ttf2->getContentSize().height / 2;
                    }
                    else
                    {
                        m_pCurPos.y -= ttf2->getContentSize().height;
                    }
#endif
                    m_pCurPos.y -= ttf2->getContentSize().height;
                }
                
                MsgColorTexCoords msg;
                msg.color = ccc4(color.r,color.g,color.b,255);
                msg.mRect.origin.x = point_x;
                msg.mRect.origin.y = -m_pCurPos.y;
                msg.mRect.size = ttf->getContentSize();
                //msg.mRect.size.height = m_nFontSize;
                m_mMsgString += text;
                
                m_nCount = count;
                if (m_bIsFirstText)
                {
                   // CCLabelTTF *ttf2 = CCLabelTTF::create("123",fontStr_kaiti,m_nFontSize);
                    //msg.mRect.size.height -= ttf2->getContentSize().height;
                    m_pCurPos.x = 0;
                    m_nCount = 0;
                }
                m_pCurPos.y -= ttf->getTextureRect().size.height;
                /*if (count == 0 && (!m_bIsFirstText))
                 {
                 m_pCurPos.y -= m_nFontSize;
                 }*/
                
                m_lMsgColorTexCoords.push_back(msg);
			}
			
		
		}
	}
}

string NetTextListShowUI::subString(string str,int count,int &new_count)
{
	new_count = 0;
	UTF8 *utf8 = new UTF8[str.size()];
	UTF8 *utf8_fp = utf8;
	memcpy(utf8,str.c_str(),str.size());
	int lenght = 0;
	string bString = "";
	int size = str.size();
	string uStr = "";
	for(int i = 0 ; i < size && lenght < count ; i ++)
	{
		uStr = "";
		if (*utf8 >= 0xE0 && *utf8 <= 0xEF)// «3∏ˆ◊÷Ω⁄µƒ∏Ò Ω
		{
			uStr += *utf8++;
			i++;
			uStr += *utf8++;
			i++;
			uStr += *utf8++;
			new_count += 2;
			lenght += 2;
		}
		else if (*utf8 >= 0xC0 && *utf8 <= 0xDF)// «2∏ˆ◊÷Ω⁄µƒ∏Ò Ω
		{
			uStr += *utf8++;
			i++;
			uStr += *utf8++;
			new_count += 2;
			lenght += 2;
		}
		else if(*utf8 >= 0 && *utf8 <= 0x7F)// «1∏ˆ◊÷Ω⁄µƒ∏Ò Ω
		{
			uStr += *utf8++;
			new_count += 1;
			lenght += 1;
			if (uStr.at(0) == '\n')
			{
				m_bIsFirstText = true;
				bString += "\n";
				break;
			}
		}
		if(lenght > count)
		{
			//bString += "\n";
			break;
		}
		else
		{
			bString += uStr;
		}
		m_bIsFirstText = false;
	}
	if(lenght >= count)
	{
		m_bIsAddLine = true;
		bString += "\n";
	}
	else
	{
		m_bIsAddLine = false;
	}
	delete[] utf8_fp;
	return bString;
}
string NetTextListShowUI::formatting(string str)
{
	string bString = "";
	int size = str.size();
	for(int i = 0 ; i < size ; i ++)
	{
		if(str.at(i) != 13)
		{
			bString += str.at(i);
		}
	}
	return bString;
}
int NetTextListShowUI::getJianGePos(string str)
{
	return 0;
}