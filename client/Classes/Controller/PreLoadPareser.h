#pragma once

#include <string>
#include "include_json.h"
#include "LFFileUtiles.h"
#include <map>
#include <vector>

using namespace std;

#define PreLoadPareser_ROOT			"config/"
#define PreLoadPareser_init_file	"pre_load.txt"

struct PreLoad					// 对应一个preload文件
{
	vector<string> linkedFile;	// 链接的文件--->link_pre_load
	vector<string> imageFile;	// 指定的图片--->image_files
};

/**
 *	纹理预加载的配置解析器,解析配置文件里面的数据，然后可以根据文件名，返回文件里面关联的所有的图片
 */
class PreLoadPareser
{
private:
	static PreLoadPareser* parser;
	map<string,PreLoad> data;		// 文件名和文件内容的map

	PreLoadPareser(void);

	PreLoad loadPreloadByFile(string file);	// 从文件载入
	PreLoad getPreloadByFile(string file);	// 先看缓存，没有就从文件载入
	// 是否某文件已经载入过了,true即载入过
	bool isFileLoaded(string key);

	//// 得到root链接的文件，deepLink即深层链接，就是如果链接的文件里面还有链接，那么就会继续链接
	//// 如果不深层链接，那么就只链接root指定的这一层的文件
	//vector<PreLoad> getLinkedPreLoad(PreLoad root,bool deepLink = true);
public:
	static PreLoadPareser* getParser();
	// 读取默认的配置文件[pre_load.txt]，把所有的配置数据初始化到内存
	// 所以应该在pre_load.txt里面把所有的自定义的配置文件链接进去
	// 到时也根据链接的文件名作为Key查找关联的图片列表
	void init();
	// 根据默认的配置文件，读取出所有的图片文件列表
	vector<string> getAllPicFile();
	// fileKey 就是在默认文件里面链接的文件名
	vector<string> getPicFileByKey(string fileKey);

	~PreLoadPareser(void);
};

