#include "LFFileUtiles.h"
//#include "SpriteHelper.h"

LFFileUtiles* LFFileUtiles::fileUtiles;

// JNI
extern bool createJavaDocFile(string dir,string fileName);
extern bool createJavaDocFile(string filePath);

LFFileUtiles* LFFileUtiles::getLFFileUtiles()
{
	if (fileUtiles == NULL)
	{
		fileUtiles = new LFFileUtiles();
	}
	return fileUtiles;
}

LFFileUtiles::LFFileUtiles()
{
	resRoot = "";
    resRoot = getResourceRootPath();
}

void LFFileUtiles::test()
{
	CCLog("========================test===========================");

	//string p = CCFileUtils::sharedFileUtils()->getWriteablePath()+"infor/infor.txt";
	string p = CCFileUtils::sharedFileUtils()->getWritablePath()+"infor/infor.txt";
	CCLog("GameInforEditer : getWriteablePath->\n");
	CCLog(p.c_str());
	//string r = CCFileUtils::sharedFileUtils()->fullPathFromRelativePath("infor/infor.txt");
#pragma message("LFFileUtiles ----> fullPathForFilename not Test!") 
	string r = CCFileUtils::sharedFileUtils()->fullPathForFilename("infor/infor.txt");
	CCLog("GameInforEditer : fullPathFromRelativePath->\n");
	CCLog(r.c_str());

	CCLog("GameInforEditer : getFileData->\n");
	unsigned long size = 0;
	unsigned char* text = CCFileUtils::sharedFileUtils()->getFileData("infor/infor.txt","r",&size);
	if (size > 0)
	{
		CCLog((const char*)text);
	}else
	{
		CCLog("no data !");
	}

	const char* test = NULL;
	size = 0;
	//测试readResourceFile
	test = readResourceFile("infor/infor.txt",&size);
	if (size > 0)
	{
		CCLog("Sucess readResourceFile!");
		CCLog(test);
		delete [] test;
	}else
	{
		CCLog("Failed readResourceFile!");
	}
	//测试copyFileFromResToDocIfNotExist
	bool su = copyFileFromResToDocIfNotExist("infor/infor.txt");
	if (su)
	{
		CCLog("Sucess copyFileFromResToDocIfNotExist!");
	}else
	{
		CCLog("Failed copyFileFromResToDocIfNotExist!");
	}	

	size =  0;
	test = NULL;
	//测试readDocFile
	test = readDocFile("infor/infor.txt",&size);
	if (size > 0)
	{
		CCLog("Sucess readDocFile!");
		CCLog(test);
		delete [] test;
	}else
	{
		CCLog("Failed readDocFile!");
	}

	//测试writeToDocFile
	if (writeToDocFile("infor/infor.txt","测试测试！test test!"))
	{
		CCLog("Sucess writeToDocFile!");
	}else
	{
		CCLog("Failed writeToDocFile!");
	}

	size =  0;
	test = NULL;
	//测试readDocFile
	test = readDocFile("infor/infor.txt",&size);
	if (size > 0)
	{
		CCLog("Sucess readDocFile!");
		CCLog(test);
		delete [] test;
	}else
	{
		CCLog("Failed readDocFile!");
	}
}

const char* LFFileUtiles::getResourceRootPath()
{
    if (resRoot != "") {
        return resRoot.c_str();
    }
    
#if ( CC_TARGET_PLATFORM == CC_PLATFORM_IOS )
	string temp = CCFileUtils::sharedFileUtils()->fullPathForFilename("");
	string path = temp.substr(0,temp.find(".app/")+5);
	return path.c_str();
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	string temp = CCFileUtils::sharedFileUtils()->fullPathForFilename("");
	temp.insert(0, "assets/");
	return temp.c_str();
#else
	//return CCFileUtils::sharedFileUtils()->fullPathFromRelativePath("");
	return CCFileUtils::sharedFileUtils()->fullPathForFilename("").c_str();
#endif
}

bool LFFileUtiles::isResourceFileExist(string file)
{
	string resRoot = getResourceRootPath();
//	if (CC_CONTENT_SCALE_FACTOR() == 2)
//	{
//		file = SpriteHelper::replace_all_distinct(file,".png","-ipadhd.png");
//	}
	string path = resRoot + file;
	return isFileExist(path);
}

const char* LFFileUtiles::readResourceFile(string file,unsigned long *size)
{
	if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	{
		file = file.substr(file.find_last_of("/")+1);
		file = CCFileUtils::sharedFileUtils()->fullPathForFilename(file.c_str());
	}
	return (const char*)CCFileUtils::sharedFileUtils()->getFileData( file.c_str(),"r",size);
}
//对于win32，doc就是res的文件
const char* LFFileUtiles::readDocFile(string file,unsigned long *size)
{
	string path = "";
	if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
	{
		//path = CCFileUtils::sharedFileUtils()->getWriteablePath();
		path = CCFileUtils::sharedFileUtils()->getWritablePath();
		path += "/";
	}
	//ios的对于fileName只保留文件名
	if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	{
		file = file.substr(file.find_last_of("/")+1);
	}

	return (char*)readFileWithModeR( (path+file).c_str(),size);
}

// 读取文件
const char* LFFileUtiles::readFromFile(string fullPath,const char* modle,unsigned long& size)
{
	CCAssert(fullPath.length() > 0 && modle != NULL, "Invaild parameters.");
	CCLog("%s--->fullPath = [%s]",__FUNCTION__,fullPath.c_str());

	unsigned char* pBuffer = NULL;
	size = 0;

	
	FILE *fp = fopen(fullPath.c_str(), modle);
	if (fp == NULL)
	{
		CCLog("Error : %s--> open file faild ! [%s]",__FUNCTION__,fullPath.c_str());
		return NULL;
	}

	fseek(fp,0,SEEK_END);
	size = ftell(fp);
	fseek(fp,0,SEEK_SET);
	pBuffer = new unsigned char[size];
	if (pBuffer == NULL)
	{
		fclose(fp);
		CCLog("Error : %s --> new buffer[size = %ld] error!",__FUNCTION__,size);
		return NULL;
	}
	size = fread(pBuffer,sizeof(unsigned char), size,fp);
	fclose(fp);

	return (char*)pBuffer;
}
// 写文件
bool LFFileUtiles::writeToFile(string fullPath,const char* modle,const char* data,unsigned long size)
{
	CCAssert(fullPath.length() > 0 && modle != NULL && data != NULL, "Invaild parameters.");
	CCLog("%s---->path=%s",__FUNCTION__,fullPath.c_str());
	FILE *fp1;
	if( (fp1=fopen(fullPath.c_str(),modle))==NULL )
	{
		CCLog("%s---->open file %s failed ! Maybe the file have desn't exist !",__FUNCTION__,fullPath.c_str());
		return false;
	}

	unsigned long wirted = fwrite(data, sizeof(char), size, fp1);
	if (wirted != size)
	{
		CCLog("Error : %s --> write error, wirted = %ld",__FUNCTION__,wirted);
		fclose(fp1);
		return false;
	}

	if (fclose(fp1) == -1)
	{
		CCLog("Error : %s---->close file: %s failed !",__FUNCTION__,fullPath.c_str());
		return false;
	}
	return true;
}

bool LFFileUtiles::writeBinaryDataToDocFile(string relativeFileName,const char* data,unsigned long size)
{
	string path = "";
	if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
	{
		path = CCFileUtils::sharedFileUtils()->getWritablePath();
		path += "/";
	}
	//ios的对于fileName只保留文件名
	if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	{
		relativeFileName = relativeFileName.substr(relativeFileName.find_last_of("/")+1);
	}

	return writeToFile(path+relativeFileName,"wb",data,size);
}
const char* LFFileUtiles::readBinaryDataFromDocFile(string relativeFileName,unsigned long& size)
{
	string path = "";
	if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
	{
		path = CCFileUtils::sharedFileUtils()->getWritablePath();
		path += "/";
	}
	//ios的对于fileName只保留文件名
	if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	{
		relativeFileName = relativeFileName.substr(relativeFileName.find_last_of("/")+1);
	}

	return readBinaryDataFromFile(path+relativeFileName,size);
}
const char* LFFileUtiles::readBinaryDataFromResFile(string relativeFileName,unsigned long& size)
{
	string fullFile = relativeFileName;
	if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	{
		fullFile = fullFile.substr(fullFile.find_last_of("/")+1);
		fullFile = CCFileUtils::sharedFileUtils()->fullPathForFilename(fullFile.c_str());
	}
	return (const char*)CCFileUtils::sharedFileUtils()->getFileData( fullFile.c_str(),"rb",&size);
}
const char* LFFileUtiles::readBinaryDataFromFile(string fileFullPathName,unsigned long& size)
{
	return readFromFile(fileFullPathName,"rb",size);
}


string LFFileUtiles::getResourceFile(string fileName,bool& success)
{
	unsigned long size;
	success = true;
	const char* doc = readResourceFile(fileName,&size);
	if (doc == NULL)
	{
		CCLog("Error : LFFileUtiles::getResourceFile ---> read file Error[%s]",fileName.c_str());
		success = false;
		return "null";
	}
	if (size <= 0)
	{
		return "null";
	}
	string fileData = string(doc);
	delete [] doc;
	return fileData;
}

bool LFFileUtiles::copyFileFromResToDocIfNotExist(string fromFileName,string toFileName)
{
	if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	{
		CCLog("copyFileFromResToDocIfNotExist--->CC_PLATFORM_WIN32--> no need to copy !");
		return true;
	}
	//ios的对于fileName只保留文件名
	if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	{
		fromFileName = fromFileName.substr(fromFileName.find_last_of("/")+1);
	}
	if ( toFileName =="" )
	{
		toFileName = fromFileName;
	}

	string path = "";
	if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
	{
		//path = CCFileUtils::sharedFileUtils()->getWriteablePath();
		path = CCFileUtils::sharedFileUtils()->getWritablePath();
		path += "/";
	}

	{
		string patht = path + toFileName;
		CCLog("copyFileFromResToDocIfNotExist --> isFileExist : [%s]",patht.c_str()); 
	}
	if (isFileExist(path + toFileName))
	{
		CCLog("copyFileFromResToDocIfNotExist--->file [%s] is exist , so we will not copy it .",(path + toFileName).c_str());
		return true;
	}else
	{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
		createJavaDocFile(toFileName);
#endif
	}

	unsigned long fromSize = 0;
	const char* fromData = readResourceFile(fromFileName,&fromSize);
	if (fromData)
	{
		return writeToDocFile(toFileName,fromData);
	}

	return false;
	
	//string path = CCFileUtils::sharedFileUtils()->getWriteablePath();
	//ios的对于fileName只保留文件名
	//if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	//{
	//	fromFileName = fromFileName.substr(fromFileName.find_last_of("/")+1);
	//}
	//if ( toFileName =="" )
	//{
	//	toFileName = fromFileName;
	//}
	//return fileCopy( CCFileUtils::sharedFileUtils()->fullPathFromRelativePath(fromFileName.c_str()), (path+"/"+toFileName).c_str());
}

//保存到Doc文件：调用方法writeToDocFile("KK/KKK/test.t","axjkankjn");
bool LFFileUtiles::writeToDocFile(string fileName,string data,const char* modle /* = "w" */)
{
	string path = "";
	if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
	{
		//path = CCFileUtils::sharedFileUtils()->getWriteablePath();
		path = CCFileUtils::sharedFileUtils()->getWritablePath();
		path += "/";
	}
	//ios的对于fileName只保留文件名
	if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	{
		fileName = fileName.substr(fileName.find_last_of("/")+1);
	}
	return writeDataToFile(data,path+fileName,modle);
}

bool LFFileUtiles::writeDataToFile(string data,string fullFileFath,const char* modle /* = "w" */)
{
	CCLog("\n==LFFileUtiles--->writeDataToFile---->path=%s",fullFileFath.c_str());
	//CCLog("\n==LFFileUtiles--->writeDataToFile---->data=%s",data.c_str());
	FILE *fp1;
	if( (fp1=fopen(fullFileFath.c_str(),modle))==NULL )
	{
		CCLog("\n==LFFileUtiles--->writeDataToFile---->open file %s failed ! Maybe the file have desn't exist !",fullFileFath.c_str());
		return false;
	}
	int ret = -1;
	ret = fprintf(fp1, "%s", data.c_str()); /*向所建文件写一字符串*/ 
	if (ret == -1)
	{
		CCLog("\n==LFFileUtiles--->writeDataToFile---->write string to file: %s failed !",fullFileFath.c_str());
		return false;
	}
	ret = fclose(fp1);
	if (ret == -1)
	{
		CCLog("\n==LFFileUtiles--->writeDataToFile---->close file: %s failed !",fullFileFath.c_str());
		return false;
	}

	return true;
}

unsigned char* LFFileUtiles::readFileWithModeR(const char* fileFullPath,unsigned long * pSize)
{
	unsigned char* pBuffer = NULL;
	CCAssert(fileFullPath != NULL && pSize != NULL, "Invaild parameters.");
	*pSize = 0;

	CCLog("LFFileUtiles::readFile--->[%s]",fileFullPath);
	FILE *fp = fopen(fileFullPath, "r");
	if (fp == NULL)
	{
		CCLog("\n==LFFileUtiles-->readFile----> open file faild ! [%s]",fileFullPath);
		return NULL;
	}

	fseek(fp,0,SEEK_END);
	*pSize = ftell(fp);
	fseek(fp,0,SEEK_SET);
	pBuffer = new unsigned char[*pSize];
	*pSize = fread(pBuffer,sizeof(unsigned char), *pSize,fp);
	fclose(fp);

	return pBuffer;
}

bool LFFileUtiles::isFileExist(string fileFullPath)
{
	CCLog("isFileExist : [%s]",fileFullPath.c_str()); 

	return CCFileUtils::sharedFileUtils()->isFileExist(fileFullPath);

	//bool isToExist = false;
	//{//检测目标文件是否存在
	//	FILE* fp;
	//	// 这里只能用 读(rb) 的形式去打开，因为如果是iOS等其他的话，好多文件只能读，不能写
	//	// 所有如果是r+之类的mode，那么即使有文件，也会返回NULL的
	//	fp = fopen( fileFullPath.c_str(), "rb");
	//	if (fp == NULL)
	//	{
	//		isToExist = false;
	//	}else
	//	{
	//		isToExist = true;
	//		fclose(fp);
	//	}
	//}
	//return isToExist;
}

bool LFFileUtiles::fileCopy(const char* from,const char* to)
{
	CCLog("LFFileUtiles::fileCopy----->from[%s] to[%s]!",from,to);
	char c;  
	FILE *fin, *fout;  
	if((fin = fopen(from,"r")) == NULL) 
	{
		CCLog("LFFileUtiles::fileCopy----->open (from) file [%s] failed !",from);
		return false;
	}
	if((fout = fopen(to,"w")) == NULL) //以 w 方式，只能用于往文件中写，如果不存在，则以指定的文件名新建一个文件   
	{
		CCLog("LFFileUtiles::fileCopy----->open (to) file [%s] failed !",to);
		return false;
	}
	while((c = fgetc(fin)) != EOF)  
		fputc(c,fout);     

	fclose(fin);  
	fclose(fout);  

	return true;
}