LOCAL_PATH := $(call my-dir)  
include $(CLEAR_VARS)  
LOCAL_MODULE := json_shared  
LOCAL_CPPFLAGS := -DJSON_IS_AMALGAMATION -fexceptions  
  
LOCAL_SRC_FILES := json_reader.cpp \
									json_value.cpp \
									json_writer.cpp
									
									
  
LOCAL_C_INCLUDES := $(LOCAL_PATH) 
  
include $(BUILD_SHARED_LIBRARY)  