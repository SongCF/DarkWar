#pragma once
#include <string>
#include <map>

using namespace std;

enum ImageType
{
	IT_PNG = 0,
	IT_JPG,
};

class ImageTypeController
{
private:
	static ImageTypeController* mController;

	map<string,ImageType> configs;

	ImageTypeController(void);
public:
	static ImageTypeController* getControler();
	// 获取转换之后的文件名
	// 本来是kkkkk.png
	// 如果有指定某个图片使用jpg的图片，那么就会返回kkkkk.jpg
	string getImageFile(string fileName);

	~ImageTypeController(void);
};

