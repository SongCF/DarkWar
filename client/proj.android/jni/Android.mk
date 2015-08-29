LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := cocos2dcpp_shared
LOCAL_MODULE_FILENAME := libcocos2dcpp


#宏定义
LOCAL_CPPFLAGS := -DJSON_IS_AMALGAMATION -fexceptions -D_CLIENT_

#traverse all the directory and subdirectory
define walk
  $(wildcard $(1)) $(foreach e, $(wildcard $(1)/*), $(call walk, $(e)))
endef

#traverse Classes Directory
ALLFILES = $(call walk, $(LOCAL_PATH)/../../Classes)

FILE_LIST := hellocpp/main.cpp
FILE_LIST += $(filter %.cpp, $(ALLFILES))

FILE_INCLUDES := $(LOCAL_PATH)/../../Classes
FILE_INCLUDES += $(LOCAL_PATH)/../../Classes/Model
FILE_INCLUDES += $(LOCAL_PATH)/../../Classes/Controller
FILE_INCLUDES += $(LOCAL_PATH)/../../Classes/Controller/NetWork
FILE_INCLUDES += $(LOCAL_PATH)/../../Classes/View
FILE_INCLUDES += $(LOCAL_PATH)/../../Classes/View/GongJiang
FILE_INCLUDES += $(LOCAL_PATH)/../../Classes/View/MessageBox
FILE_INCLUDES += $(LOCAL_PATH)/../../Classes/View/RichLabel
FILE_INCLUDES += $(LOCAL_PATH)/../../Classes/View/utils
FILE_INCLUDES += $(LOCAL_PATH)/../../Classes/View/Guide
FILE_INCLUDES += $(LOCAL_PATH)/../../Classes/fight/fightController
FILE_INCLUDES += $(LOCAL_PATH)/../../Classes/fight/fightModel
FILE_INCLUDES += $(LOCAL_PATH)/../../Classes/fight/fightView
FILE_INCLUDES += $(LOCAL_PATH)/../../Classes/Controller/jni_LF

#source file will be compiled
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)
LOCAL_C_INCLUDES := $(FILE_INCLUDES) \

#old
#LOCAL_SRC_FILES := hellocpp/main.cpp \
#                   ../../Classes/AppDelegate.cpp \
#                   ../../Classes/HelloWorldScene.cpp
#
#LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes

LOCAL_WHOLE_STATIC_LIBRARIES += cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static
#LOCAL_WHOLE_STATIC_LIBRARIES += box2d_static
#LOCAL_WHOLE_STATIC_LIBRARIES += chipmunk_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_extension_static

include $(BUILD_SHARED_LIBRARY)

$(call import-module,cocos2dx)
$(call import-module,cocos2dx/platform/third_party/android/prebuilt/libcurl)
$(call import-module,CocosDenshion/android)
$(call import-module,extensions)
#$(call import-module,external/Box2D)
#$(call import-module,external/chipmunk)
