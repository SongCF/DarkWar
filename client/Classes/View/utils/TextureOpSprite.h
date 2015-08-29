#ifndef __TextureOpSprite_H__
#define __TextureOpSprite_H__

#include "cocos2d.h"
#include <vector>
#include <string>

using namespace std;
using namespace cocos2d;

// 纹理操作的一个Sprite
// 会把自己的纹理和其他的多个纹理(最多的就是GL支持的纹理卡槽的个数即GL_TEXTURE0~GL_TEXTURE*那么多)
// 这个类就是把自己的纹理坐标作为基准坐标，传值给外界的shader
// 然后把外界的纹理也传入shader
// 也就是这个类就是做个中介
// 最主要的就是使用不同的shader来处理这个最后的成色
class TextureOpSprite : public CCSprite
{
private:
	CCGLProgram* glProgram;
	float z_time;	// 每次都从0开始的时间
	void initShader(const char* vertexFile,const char* fragmentFile);
protected:
	// 所有的纹理的图片
	vector<CCTexture2D*> all_tex_arr;
	vector<string> all_tex_sampler_name_arr;
	vector<GLint> all_tex_location;
	GLint z_time_location;

	virtual void draw();
	virtual void tick_update_z_time(float t);
public:
	/**
	 *	构造一个TextureOpSprite，需要的参数：
	 *	opTexVec :	所有的纹理的图片，opTexVec[0]就是自己图片的Texture
	 *              后面的就全是其他纹理的图片
	 *
	 *	opTexSamlpeNameVec : 外界的这些纹理，在f_shader里面的采样器名字
	 *						  比如Uniform sampler2D oneText;那么名字就是oneText
	 *
	 *	v_shader_file : 顶点shader的文件名
	 *
	 *	f_shader_file : 片段shader的文件名
	 */
	TextureOpSprite(vector<CCTexture2D*> opTexVec,vector<string> opTexSamlpeNameVec,string v_shader_file,string f_shader_file);
	~TextureOpSprite();
};

#endif  //