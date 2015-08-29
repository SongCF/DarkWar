#include "NetLabelTTF.h"
#include "CCDirector.h"
#include "shaders/CCGLProgram.h"
#include "shaders/CCShaderCache.h"
#include "CCApplication.h"

NS_CC_BEGIN
#if CC_USE_LA88_LABELS
#define SHADER_PROGRAM kCCShader_PositionTextureColor
#else
#define SHADER_PROGRAM kCCShader_PositionTextureA8Color
#endif


	//
	//NetLabelTTF
	//
	NetLabelTTF::NetLabelTTF()
	: m_hAlignment(kCCTextAlignmentCenter)
	, m_vAlignment(kCCVerticalTextAlignmentTop)
	, m_pFontName(NULL)
	, m_fFontSize(0.0)
	, m_string("")
	,m_nTextCount(0)
	,m_nHeight(0)
{
}

NetLabelTTF::~NetLabelTTF()
{
	CC_SAFE_DELETE(m_pFontName);
}

NetLabelTTF * NetLabelTTF::create()
{
	NetLabelTTF * pRet = new NetLabelTTF();
	if (pRet && pRet->init())
	{
		pRet->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(pRet);
	}
	return pRet;
}

NetLabelTTF * NetLabelTTF::create(const char *string, const char *fontName, float fontSize)
{
	return NetLabelTTF::create(string, fontName, fontSize,
		CCSizeZero, kCCTextAlignmentCenter, kCCVerticalTextAlignmentTop);
}

NetLabelTTF * NetLabelTTF::create(const char *string, const char *fontName, float fontSize,
								const CCSize& dimensions, CCTextAlignment hAlignment)
{
	return NetLabelTTF::create(string, fontName, fontSize, dimensions, hAlignment, kCCVerticalTextAlignmentTop);
}

NetLabelTTF* NetLabelTTF::create(const char *string, const char *fontName, float fontSize,
							   const CCSize &dimensions, CCTextAlignment hAlignment, 
							   CCVerticalTextAlignment vAlignment)
{
	NetLabelTTF *pRet = new NetLabelTTF();
	if(pRet && pRet->initWithString(string, fontName, fontSize, dimensions, hAlignment, vAlignment))
	{
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return NULL;
}

bool NetLabelTTF::init()
{
	return this->initWithString("", "Helvetica", 12);
}

bool NetLabelTTF::initWithString(const char *label, const char *fontName, float fontSize, 
								const CCSize& dimensions, CCTextAlignment alignment)
{
	return this->initWithString(label, fontName, fontSize, dimensions, alignment, kCCVerticalTextAlignmentTop);
}

bool NetLabelTTF::initWithString(const char *label, const char *fontName, float fontSize)
{
	return this->initWithString(label, fontName, fontSize, 
		CCSizeZero, kCCTextAlignmentLeft, kCCVerticalTextAlignmentTop);
}

bool NetLabelTTF::initWithString(const char *string, const char *fontName, float fontSize,
								const cocos2d::CCSize &dimensions, CCTextAlignment hAlignment,
								CCVerticalTextAlignment vAlignment)
{
	if (CCSprite::init())
	{
		// shader program
		this->setShaderProgram(CCShaderCache::sharedShaderCache()->programForKey(SHADER_PROGRAM));

		m_tDimensions = CCSizeMake(dimensions.width, dimensions.height);
		m_hAlignment = hAlignment;
		m_vAlignment = vAlignment;
		m_pFontName = new std::string(fontName);
		m_fFontSize = fontSize;

		this->setString(string);

		return true;
	}

	return false;
}

void NetLabelTTF::setString(const char *string)
{
	CCAssert(string != NULL, "Invalid string");

	if (m_string.compare(string))
	{
		m_string = string;

		this->updateTexture();
	}
}

const char* NetLabelTTF::getString(void)
{
	return m_string.c_str();
}

const char* NetLabelTTF::description()
{
	return CCString::createWithFormat("<NetLabelTTF | FontName = %s, FontSize = %.1f>", m_pFontName->c_str(), m_fFontSize)->getCString();
}

CCTextAlignment NetLabelTTF::getHorizontalAlignment()
{
	return m_hAlignment;
}

void NetLabelTTF::setHorizontalAlignment(CCTextAlignment alignment)
{
	if (alignment != m_hAlignment)
	{
		m_hAlignment = alignment;

		// Force update
		if (m_string.size() > 0)
		{
			this->updateTexture();
		}
	}
}

CCVerticalTextAlignment NetLabelTTF::getVerticalAlignment()
{
	return m_vAlignment;
}

void NetLabelTTF::setVerticalAlignment(CCVerticalTextAlignment verticalAlignment)
{
	if (verticalAlignment != m_vAlignment)
	{
		m_vAlignment = verticalAlignment;

		// Force update
		if (m_string.size() > 0)
		{
			this->updateTexture();
		}
	}
}

CCSize NetLabelTTF::getDimensions()
{
	return m_tDimensions;
}

void NetLabelTTF::setDimensions(const CCSize &dim)
{
	if (dim.width != m_tDimensions.width || dim.height != m_tDimensions.height)
	{
		m_tDimensions = dim;

		// Force update
		if (m_string.size() > 0)
		{
			this->updateTexture();
		}
	}
}

float NetLabelTTF::getFontSize()
{
	return m_fFontSize;
}

void NetLabelTTF::setFontSize(float fontSize)
{
	if (m_fFontSize != fontSize)
	{
		m_fFontSize = fontSize;

		// Force update
		if (m_string.size() > 0)
		{
			this->updateTexture();
		}
	}
}

const char* NetLabelTTF::getFontName()
{
	return m_pFontName->c_str();
}

void NetLabelTTF::setFontName(const char *fontName)
{
	if (m_pFontName->compare(fontName))
	{
		delete m_pFontName;
		m_pFontName = new std::string(fontName);

		// Force update
		if (m_string.size() > 0)
		{
			this->updateTexture();
		}
	}
}

// Helper
bool NetLabelTTF::updateTexture()
{
	CCTexture2D *tex;

	// let system compute label's width or height when its value is 0
	// refer to cocos2d-x issue #1430
	tex = new CCTexture2D();
	if (!tex)
	{
		return false;
	}

	tex->initWithString( m_string.c_str(),
		m_pFontName->c_str(),
		m_fFontSize * CC_CONTENT_SCALE_FACTOR(),
		CC_SIZE_POINTS_TO_PIXELS(m_tDimensions), 
		m_hAlignment,
		m_vAlignment);


	this->setTexture(tex);
	tex->release();

	CCRect rect = CCRectZero;
	rect.size = m_pobTexture->getContentSize();
	this->setTextureRect(rect);

	return true;
}
void NetLabelTTF::draw(void)
{
	//CCSprite::draw();/*
	CC_PROFILER_START_CATEGORY(kCCProfilerCategorySprite, "CCSprite - draw");

	CCAssert(!m_pobBatchNode, "If CCSprite is being rendered by CCSpriteBatchNode, CCSprite#draw SHOULD NOT be called");

	CC_NODE_DRAW_SETUP();

	ccGLBlendFunc( m_sBlendFunc.src, m_sBlendFunc.dst );

	if (m_pobTexture != NULL)
	{
		ccGLBindTexture2D( m_pobTexture->getName() );
	}
	else
	{
		ccGLBindTexture2D(0);
	}

	//
	// Attributes
	//

	ccGLEnableVertexAttribs( kCCVertexAttribFlag_PosColorTex );
	int count = 0;

	for (vector<MsgColorTexCoords>::iterator iter = m_lMsgColorTexCoords.begin() ; iter != m_lMsgColorTexCoords.end();iter++)
	{
		ccV3F_C4B_T2F_Quad quad1 = getQuad(m_pobTexture,count);

#define  kQuadSize  sizeof(quad1.bl)
		long offset = (long)&quad1;

		// vertex
		int diff = offsetof( ccV3F_C4B_T2F, vertices);
		glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, kQuadSize, (void*) (offset + diff));

		// texCoods
		diff = offsetof( ccV3F_C4B_T2F, texCoords);
		glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(offset + diff));

		// color
		diff = offsetof( ccV3F_C4B_T2F, colors);
		glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (void*)(offset + diff));


		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		count ++;
	}


	CHECK_GL_ERROR_DEBUG();

	CC_INCREMENT_GL_DRAWS(1);

	CC_PROFILER_STOP_CATEGORY(kCCProfilerCategorySprite, "CCSprite - draw");
}
ccV3F_C4B_T2F_Quad NetLabelTTF::getQuad(CCTexture2D *tex,int count)
{
	ccV3F_C4B_T2F_Quad quad;

	float atlasWidth = (float)tex->getPixelsWide();
	float atlasHeight = (float)tex->getPixelsHigh();

	float left, right, top, bottom;

	ccColor4B tmpColor = m_lMsgColorTexCoords[count].color;

	quad.bl.colors = tmpColor;
	quad.br.colors = tmpColor;
	quad.tl.colors = tmpColor;
	quad.tr.colors = tmpColor;

	CCRect bRect = m_lMsgColorTexCoords[count].mRect;
	// Atlas: Vertex
	float x1 = bRect.origin.x + m_obOffsetPosition.x;
	float y1 = m_nHeight - bRect.origin.y - m_obOffsetPosition.y - bRect.size.height;
	float x2 = x1 + bRect.size.width;
	float y2 = y1 + bRect.size.height;

	// Don't update Z.
	quad.bl.vertices = vertex3(x1, y1, 0);
	quad.br.vertices = vertex3(x2, y1, 0);
	quad.tl.vertices = vertex3(x1, y2, 0);
	quad.tr.vertices = vertex3(x2, y2, 0);

	CCRect rect ;
	rect.origin.x = bRect.origin.x;
	rect.origin.y =  bRect.origin.y;
	rect.size.width = bRect.size.width;
	rect.size.height = bRect.size.height;

#if CC_FIX_ARTIFACTS_BY_STRECHING_TEXEL
	left    = (2*rect.origin.x+1)/(2*atlasWidth);
	right    = left + (rect.size.width*2-2)/(2*atlasWidth);
	top        = (2*rect.origin.y+1)/(2*atlasHeight);
	bottom    = top + (rect.size.height*2-2)/(2*atlasHeight);
#else
	left    = rect.origin.x/atlasWidth;
	right    = (rect.origin.x + rect.size.width) / atlasWidth;
	top        = rect.origin.y/atlasHeight;
	bottom    = (rect.origin.y + rect.size.height) / atlasHeight;
#endif // ! CC_FIX_ARTIFACTS_BY_STRECHING_TEXEL

	if(m_bFlipX)
	{
		CC_SWAP(left,right,float);
	}

	if(m_bFlipY)
	{
		CC_SWAP(top,bottom,float);
	}




	quad.bl.texCoords.u = left;
	quad.bl.texCoords.v = bottom;
	quad.br.texCoords.u = right;
	quad.br.texCoords.v = bottom;
	quad.tl.texCoords.u = left;
	quad.tl.texCoords.v = top;
	quad.tr.texCoords.u = right;
	quad.tr.texCoords.v = top;
	
	
	return quad;
/*
	*/
}
NS_CC_END
