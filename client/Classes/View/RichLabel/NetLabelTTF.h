#ifndef __NetLabelTTF_H__
#define __NetLabelTTF_H__

#include "textures/CCTexture2D.h"
#include "cocos2d.h"
#include "NetTextListShowUI.h"
NS_CC_BEGIN

class NetLabelTTF : public CCSprite, public CCLabelProtocol
{
public:
    NetLabelTTF();
    virtual ~NetLabelTTF();
    const char* description();    
    
    /** creates a NetLabelTTF with a font name and font size in points
     @since v2.0.1
     */
    static NetLabelTTF * create(const char *string, const char *fontName, float fontSize);
    
    /** creates a NetLabelTTF from a fontname, horizontal alignment, dimension in points,  and font size in points.
     @since v2.0.1
     */
    static NetLabelTTF * create(const char *string, const char *fontName, float fontSize,
                               const CCSize& dimensions, CCTextAlignment hAlignment);
  
    /** creates a CCLabel from a fontname, alignment, dimension in points and font size in points
     @since v2.0.1
     */
    static NetLabelTTF * create(const char *string, const char *fontName, float fontSize,
                               const CCSize& dimensions, CCTextAlignment hAlignment, 
                               CCVerticalTextAlignment vAlignment);
    
    /** initializes the NetLabelTTF with a font name and font size */
    bool initWithString(const char *string, const char *fontName, float fontSize);
    
    /** initializes the NetLabelTTF with a font name, alignment, dimension and font size */
    bool initWithString(const char *string, const char *fontName, float fontSize,
                        const CCSize& dimensions, CCTextAlignment hAlignment);

    /** initializes the NetLabelTTF with a font name, alignment, dimension and font size */
    bool initWithString(const char *string, const char *fontName, float fontSize,
                        const CCSize& dimensions, CCTextAlignment hAlignment, 
                        CCVerticalTextAlignment vAlignment);
    
    /** initializes the NetLabelTTF */
    bool init();

    /** Creates an label.
     */
    static NetLabelTTF * create();

    /** changes the string to render
    * @warning Changing the string is as expensive as creating a new NetLabelTTF. To obtain better performance use CCLabelAtlas
    */
    virtual void setString(const char *label);
    virtual const char* getString(void);
    
    CCTextAlignment getHorizontalAlignment();
    void setHorizontalAlignment(CCTextAlignment alignment);
    
    CCVerticalTextAlignment getVerticalAlignment();
    void setVerticalAlignment(CCVerticalTextAlignment verticalAlignment);
    
    CCSize getDimensions();
    void setDimensions(const CCSize &dim);
    
    float getFontSize();
    void setFontSize(float fontSize);
    
    const char* getFontName();
    void setFontName(const char *fontName);
	virtual void draw(void);

	inline void setMsgColorTexCoords(vector<MsgColorTexCoords> list)
	{
		m_lMsgColorTexCoords = list;
	}
	inline void setHeight(int height)
	{
		m_nHeight = height;
	}
	inline void setFontSize(int fontSize)
	{
		m_nFontSize = fontSize;
	}
private:
    bool updateTexture();

	ccV3F_C4B_T2F_Quad getQuad(CCTexture2D *tex,int count);
protected:
    /** Dimensions of the label in Points */
    CCSize m_tDimensions;
    /** The alignment of the label */
    CCTextAlignment         m_hAlignment;
    /** The vertical alignment of the label */
    CCVerticalTextAlignment m_vAlignment;
    /** Font name used in the label */
    std::string * m_pFontName;
    /** Font size of the label */
    float m_fFontSize;
    
    std::string m_string;

	int					m_nTextCount;
	vector<MsgColorTexCoords> m_lMsgColorTexCoords;

	int					m_nHeight;
	int					m_nFontSize;
};


// end of GUI group
/// @}
/// @}

NS_CC_END

#endif //__CCLABEL_H__

