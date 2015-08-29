#pragma once

#include "BaseLayer.h"

class RegisterLayer : public BaseLayer,CCEditBoxDelegate
{
private:
	BaseSprite* editGroup;

	void registClicked(CCObject* sender);
	void returnClicked(CCObject* sender);
	void registCallback(CCObject* msg_rsp);

	/**
     * This method is called when the edit box text was changed.
     * @param editBox The edit box object that generated the event.
     * @param text The new text.
     */
    virtual void editBoxTextChanged(CCEditBox* editBox, const std::string& text);
    
    /**
     * This method is called when the return button was pressed or the outside area of keyboard was touched.
     * @param editBox The edit box object that generated the event.
     */
    virtual void editBoxReturn(CCEditBox* editBox);
public:
	RegisterLayer(void);
	~RegisterLayer(void);
};

