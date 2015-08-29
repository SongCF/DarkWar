#include "PreLoadPareser.h"

PreLoadPareser* PreLoadPareser::parser = new PreLoadPareser();

PreLoadPareser::PreLoadPareser(void)
{
}
PreLoadPareser::~PreLoadPareser(void)
{
}

PreLoadPareser* PreLoadPareser::getParser()
{
	return parser;
}

PreLoad PreLoadPareser::getPreloadByFile(string file)
{
	map<string, PreLoad>::iterator iter;
	//如果map中没有要查找的数据，它返回的迭代器等于end函数返回的迭代器;
	iter = data.find(file);
	if (iter != data.end())
	{
		return iter->second;
	}

	PreLoad ret = loadPreloadByFile(PreLoadPareser_ROOT + file);
	data.insert(pair<string, PreLoad>(file,ret));

	return ret;
}

PreLoad PreLoadPareser::loadPreloadByFile(string file)
{
	PreLoad ret;

	Json::Reader reader;
	Json::Value root;
	bool suc;
	string doc = LFFileUtiles::getLFFileUtiles()->getResourceFile(file,suc);
	if (!suc)
	{
		CCLog("PreLoadPareser::getPreloadByFile ----> file [%s] have no content.",file.c_str());
		CCAssert(false,"");
		return ret;
	}

	if (!reader.parse(doc,root))
	{
		CCLog("PreLoadPareser::getPreloadByFile,reader.parse failed! [%s]",file.c_str());
		CCLog(doc.c_str());
		CCAssert(false,"");
		return ret;
	}

	//CCLog(doc.c_str());
	{
		Json::Value _link= root["link_pre_load"];
		if (!_link.isNull() && _link.isArray())
		{
			int _fileNum = _link.size();
			for (int i=0;i<_fileNum;i++)
			{
				Json::Value oneFile = _link[i];
				string _file = oneFile["file"].asString();

				ret.linkedFile.push_back(_file);	// 加入
			}
		}
	}
	
	{
		char picTemp[1024];

		Json::Value _imageFile= root["image_files"];
		if (!_imageFile.isNull() && _imageFile.isArray())
		{
			int _fileNum = _imageFile.size();
			for (int i=0;i<_fileNum;i++)
			{
				Json::Value oneFile = _imageFile[i];

				// 每次都是单独的图片
				if (!oneFile["pic"].isNull())
				{
					string _pic = oneFile["pic"].asString();
					ret.imageFile.push_back(_pic);	// 加入

					if (!oneFile["pic_list"].isNull())
					{
						CCLog("Waring --> %s%s:this key 'pic' is exist ,we will ignore 'pic_list'!",__FILE__,__FUNCTION__);
					}
				}
				// 是图片列表，带有%d格式化的
				else if (!oneFile["pic_list"].isNull())
				{
					Json::Value picList = oneFile["pic_list"];
					string _format = picList["format_d"].asString();
					int picIdFrom = picList["d_from"].asInt();
					int picNum = picList["pic_num"].asInt();

					for (int picIndex = 0;picIndex<picNum;picIndex++)
					{
						sprintf(picTemp,_format.c_str(),picIdFrom+picIndex);
						ret.imageFile.push_back(string(picTemp));	// 加入
					}
				}
				else
				{
					CCLog("Waring --> %s%s:Not found key 'pic' or 'pic_list'!",__FILE__,__FUNCTION__);
				}
			}
		}
	}
	return ret;
}

bool PreLoadPareser::isFileLoaded(string key)
{
	map<string, PreLoad>::iterator iter;
	//如果map中没有要查找的数据，它返回的迭代器等于end函数返回的迭代器;
	iter = data.find(key);
	return (iter != data.end());
}

void PreLoadPareser::init()
{
	data.clear();
	PreLoad initLoad = getPreloadByFile(PreLoadPareser_init_file);

	// 只支持一层初始化文件的链接
	int fileNum = initLoad.linkedFile.size();
	for (int i=0;i<fileNum;i++)
	{
		PreLoad oneData = getPreloadByFile(initLoad.linkedFile[i]);
		if (!oneData.linkedFile.empty())
		{
			CCAssert(false,"不支持多层链接！");
		}
	}
}

vector<string> PreLoadPareser::getAllPicFile()
{
	vector<string> ret;

	map<string,PreLoad>::iterator it;
	for(it=data.begin();it!=data.end();++it)
	{
		ret.insert(ret.end(),it->second.imageFile.begin(),it->second.imageFile.end());
	}
	return ret;
}
vector<string> PreLoadPareser::getPicFileByKey(string fileKey)
{
	return getPreloadByFile(fileKey).imageFile;
}
