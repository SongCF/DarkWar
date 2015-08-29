#include "TextureOpSprite.h"
#include "shaders/CCGLProgram.h"

TextureOpSprite::TextureOpSprite
	(vector<CCTexture2D*> opTexVec,vector<string> opTexSamlpeNameVec,string v_shader_file,string f_shader_file)
{
	CCAssert(opTexVec.size() > 0,"");
	CCAssert(opTexSamlpeNameVec.size() == opTexVec.size(),"");
	
	this->all_tex_arr = opTexVec;
	this->all_tex_sampler_name_arr = opTexSamlpeNameVec;
	initWithTexture(opTexVec[0]);

	//这个顶点Shader就是直接用ccShader_PositionTextureColor_vert.h的shader
	initShader(v_shader_file.c_str(),f_shader_file.c_str());//

	// retain
	int num = all_tex_arr.size();
	for (int i=0;i<num;i++)
	{
		all_tex_arr[i]->retain();
	}
}

TextureOpSprite::~TextureOpSprite()
{
	// release
	int num = all_tex_arr.size();
	for (int i=0;i<num;i++)
	{
		all_tex_arr[i]->release();
	}
}

void TextureOpSprite::initShader(const char* vertexFile,const char* fragmentFile)
{
	//这个里面自己就封装了很多属性的管理
	glProgram = new CCGLProgram();

	// 这里先把shader载入内存，然后调用gl去编译，然后就在附加到我们程序里面去
	// 在编译的函数里面，还做了如下操作：
	// 给顶点着色器的代码添加了新的代码：有7个uniform，如下:
	//"precision highp float;\n"	// 电脑就用hight，手机设备就用mediump
	//"precision mediump float;\n",
	//"uniform mat4 CC_PMatrix;\n"
	//"uniform mat4 CC_MVMatrix;\n"
	//"uniform mat4 CC_MVPMatrix;\n"
	//"uniform vec4 CC_Time;\n"
	//"uniform vec4 CC_SinTime;\n"
	//"uniform vec4 CC_CosTime;\n"
	//"uniform vec4 CC_Random01;\n"
	// 注意没有图片采样器(uniform sampler2D *****;)
	//
	// 所以对于有图片的shader来说，我们要添加一个uniform sampler2D *****;在shader里面.
	// 由于下面updateUniform时cocos2dx会找名字为CC_Texture0的采样器，所以我们直接用这个名字就可以了，
	// 即添加一个uniform sampler2D CC_Texture0;在片段shader里面即可。
	// 对于片段着色器，把uniform也定义了一次。
	// 上面的各个字段的名字在kCCUniform****_s宏定义中可以找到
	// 
	// 可以根据glGetShaderSource方法得到运行时用的代码
	if (!glProgram->initWithVertexShaderFilename(vertexFile,fragmentFile))
		CCLog("glProgram->initWithVertexShaderFilename error!");
	CHECK_GL_ERROR_DEBUG();


	//这里的属性就是ccShader_PositionTextureColor_vert.h中的那些属性
	//绑定位置和名字，attribute需要调用enable方法启用了，才可以传值。
	//每次draw的时候都会启用。
	//addAttribute调用的是glBindAttribLocation(m_uProgram, index, attributeName);
	//就是把name绑定到某个位置，需要在link之前绑定
	//link会把没有指定绑定的元素，自动分配索引位置，
	//link之后可以通过glGetAttribLocation(prog,name)获得
	//但是目前还没有绑定Uniform的位置，只有获取glGetUniformLocation
	glProgram->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
	glProgram->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
	glProgram->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
	CHECK_GL_ERROR_DEBUG();

	// 链接程序，并删除载入的源码
	// glLinkProgram(m_uProgram);
	// glDeleteShader(m_uVertShader);
	// glDeleteShader(m_uFragShader);
	if (!glProgram->link())
		CCLog("glProgram->link error!");
	 CHECK_GL_ERROR_DEBUG();

	// 链接完毕之后，就可以获取各种Uniform的位置了，
	// 做法就是把shader里面默认添加的7个Uniform的loaction保存下来,
	// {{{****还会保存kCCUniformSampler的location，kCCUniformSampler就是"CCTexture0"的sample2D。****}}}
	// 保存到m_uUniforms数组里面，每一个数值在数组中的索引有kCCUniform****的枚举表示,如下
	// m_uUniforms[kCCUniformPMatrix] = glGetUniformLocation(m_uProgram, kCCUniformPMatrix_s);
	// kCCUniform****_s这个宏就是字段的名字。
	// 
	// 然后glUseProgram
	// 最后还给自己的纹理传了值：setUniformLocationWith1i(m_uUniforms[kCCUniformSampler], 0)-->glUniform1i( (GLint)location, i1)
	// 也就是shader的uniform sampler2D CC_Texture0赋值为0.
	// 那么这个CC_Texture0就代表在当前实例下的第0号纹理,也就和GL_TEXTURE0对应了,红宝书450页也是这么说的。
	// 在draw方法里面ccGLBindTexture2D就激活并启用这个纹理
	glProgram->updateUniforms();

	CHECK_GL_ERROR_DEBUG();

	// 查询所有纹理的位置，这里说所有是因为base的纹理不见的是CC_Texture0的名字,于是最好就是都查找一次
	// 然后重新设置下卡槽，又从0开始
	int texNum = all_tex_arr.size();
	all_tex_location.clear();
	for (int i=0;i<texNum;i++)
	{
		GLint loca = glGetUniformLocation(glProgram->getProgram(),all_tex_sampler_name_arr[i].c_str());
		if (loca == -1)
		{
			CCLog("%s%s can not found uniform sampler2D %s!",__FILE__,__FUNCTION__,all_tex_sampler_name_arr[i].c_str());
		}
		all_tex_location.push_back(loca);
		glUniform1i(loca,i);// 使用对应的GL_TEXTURE*纹理，i = 0，就是GL_TEXTURE0
	}

	z_time_location = glGetUniformLocation(glProgram->getProgram(),"z_time");

	CHECK_GL_ERROR_DEBUG();
	//是父类的方法
	setShaderProgram(glProgram);
	glProgram->release();

	z_time = 0;

	schedule(schedule_selector(TextureOpSprite::tick_update_z_time));
}

void TextureOpSprite::tick_update_z_time(float t)
{
	z_time += t;
}

void TextureOpSprite::draw()
{
	// 这里相当于：
	// glEnableVertexAttribArray( kCCVertexAttrib_Position );
	// glEnableVertexAttribArray( kCCVertexAttrib_Color );
	// glEnableVertexAttribArray( kCCVertexAttrib_TexCoords );
	// 就是启用顶点数组，包括颜色，纹理，坐标
	// 这3个位置的绑定是在初始化addAttribute函数调用的glBindAttribLocation绑定的
	//
	// 在draw方法的最后面就会给这3个数组传值
	ccGLEnableVertexAttribs(kCCVertexAttribFlag_PosColorTex );
	ccBlendFunc blend = getBlendFunc();
	ccGLBlendFunc(blend.src, blend.dst);

	// 转掉ccGLUseProgram()--->glUseProgram(program);
	// 每次都告诉gl使用当前这个program，因为gl是状态机。
	// 所有才得每渲染一个东西，就得把这个东西需要的都告诉他
	getShaderProgram()->use();

	// 最终就是调用glUniform4fv()给cocos2dx 自动添加的变量赋值。
	// 
	// 1,给矩阵赋值，这几个矩阵就是如下 cocos2dx 在编译之前给我们添加的矩阵
	// "uniform mat4 CC_PMatrix;\n"
	// "uniform mat4 CC_MVMatrix;\n"
	// "uniform mat4 CC_MVPMatrix"
	// 2,如果有用时间变量，就给时间赋值(glProgram->updateUniforms()就知道有没有使用时间变量了)：
	// "uniform vec4 CC_Time;\n"
	// "uniform vec4 CC_SinTime;\n"
	// "uniform vec4 CC_CosTime;\n"
	// 3,如果随机变量uniform vec4 CC_Random01;也有定义，那么就传入随机值
	// 4,注意，纹理是没有传的，因为cocos2dx默认没添加纹理字段，是需要的程序自己在shader里面添加CC_Texture0就可以了
	//   这样cocos2dx会自动在updateUniform的时候找到他的位置.
	getShaderProgram()->setUniformsForBuiltins();

	if (z_time_location != -1)
	{
		glUniform1f(z_time_location,z_time);
	}

	//
	// 使用GL_TEXTURE0
	// 这里转掉的是ccGLBindTexture2DN(GLuint textureUnit, GLuint textureId)
	// textureUnit = 0,textureId就是ccGLBindTexture2D的传入参数(即getTexture()->getName())
	// 内部主要的代码就是如下
	// glActiveTexture(GL_TEXTURE0 + textureUnit); 
	// glBindTexture(GL_TEXTURE_2D, textureId);
	//
	// glActiveTexture这一接口，用于指定使用哪个纹理单元(即GL_TEXTURE*)
	// 因为这里textureUnit = 0.就代表使用GL_TEXTURE0,也就是使用0号位置的纹理单元.
	//
	// glBindTexture就用于指定当前GL_TEXTURE* 真正绑定的纹理Id，这里就是指GL_TEXTURE0绑定一个2D纹理，其Id是getTexture()->getName()。
	// 那么shader里的sample2D用0号纹理，那么就是现在设置的Sprite的纹理了！！
	// 
	// glBindTexture实际上是改变了OpenGL的这个状态，它告诉OpenGL下面对纹理的任何操作都是对它所绑定的纹理对象的，
	// 比如glBindTexture(GL_TEXTURE_2D,7)告诉OpenGL下面代码中对2D纹理的任何设置都是针对索引为7的纹理的。
	// getTexture()->getName() 就是这个索引号.CCTexture2D就把纹理和分配的号保存好了的。
	//ccGLBindTexture2D( getTexture()->getName()); // 下面用循环全部传值了，这就不需要了

	// 挨个传入纹理到绑定到的卡槽
	int texNum = all_tex_arr.size();
	for (int i=0;i<texNum;i++)
	{
		ccGLBindTexture2DN(i,all_tex_arr[i]->getName());
	}

	//
	// Attributes
	//
#define kQuadSize sizeof(m_sQuad.bl)
	long offset = (long)&m_sQuad;

	// vertex
	// #define offsetof(s,m)   (size_t)&reinterpret_cast<const volatile char&>((((s *)0)->m))
	int diff = offsetof( ccV3F_C4B_T2F, vertices);
	glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, kQuadSize, (void*) (offset + diff));

	// texCoods
	diff = offsetof( ccV3F_C4B_T2F, texCoords);
	glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(offset + diff));

	// color (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer);
	diff = offsetof( ccV3F_C4B_T2F, colors);
	glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (void*)(offset + diff));
	

	// 调用绘制函数，执行绘制
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// cocos2dx的统计记录
	CC_INCREMENT_GL_DRAWS(1);
}
