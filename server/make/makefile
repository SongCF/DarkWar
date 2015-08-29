##################
ROOT_DIR=../
SRC_DIR=$(ROOT_DIR)/src
COMM_DIR=$(SRC_DIR)/common
ARC_DIR=$(ROOT_DIR)/bin
UTILITY_DIR=$(SRC_DIR)/utility
PROTOCOL_DIR=$(SRC_DIR)/protocol
LIB_DIR=$(ROOT_DIR)/lib
OBJS_DIR=$(ROOT_DIR)/make

VPATH=$(SRC_DIR):$(COMM_DIR):$(UTILITY_DIR):$(SRC_DIR)/script_builder:$(SRC_DIR)/database_check:$(SRC_DIR)/zhuangbei_template_creater:$(SRC_DIR)/robot_creater:$(SRC_DIR)/tophero_creater

INCLUDE=\
		-I$(SRC_DIR) \
		-I$(UTILITY_DIR) \
		-I$(COMM_DIR) \
		-I$(PROTOCOL_DIR) \
		-I$(SRC_DIR)/script_builder \
		-I/usr/local/mysql/include/mysql \

		
######################################################################
#CXX=g++ -D_DEBUG  -Wpointer-arith -O3 -g -pipe -D_REENTRANT -D_GNU_SOURCE $(INCLUDE)
CXX=g++ -D_DEBUG  -Wall -Wno-sign-compare -O0 -g -pipe -D_REENTRANT -D_GNU_SOURCE $(INCLUDE)
######################################################################

COMM_OBJS=\
		SDConfiguration.o \
		SDThreadPool.o \
		SDUtility.o \
		DoubleLinkedList.o

UTILITY_OBJS=\
		AES.o \
		base64.o \
		bob_hash.o \
		MsgQueue.o \
		os.o \
		utils.o \
		logs.o \
		cmysql.o


BSLSVR_OBJS=\
		ConnServerThread.o \
		ProcServerThread.o \
		PacketQueue.o \
		main.o \
		ChessBoardHelper.o \
		CommonDefine.o \
		FightControler.o \
		FightScriptBuilder.o \
		ScriptDataProvider.o \
		PropertyCalculator.o \
		GameBuffProc.o \
		ChessBoardHelper_TeamFight.o \
		ZhuangbeiGongMingManager.o \
		CS_Common.o \
		SuitPool.o \
		SkillHelperBase.o \
		SkillHelperAdapterImpl.o \
		FightScriptDefine.o \
		PlayerNameFactory.o

DBCHECK_OBJS=\
		checker_main.o

ZB_TMP_UPDATE_OBJS=\
		zhuangbei_template_creater.o

ROBOT_CREATER_OBJS=\
		ConnServerThread.o \
		ProcServerThread.o \
		PacketQueue.o \
		robot_creater.o \
		ChessBoardHelper.o \
		CommonDefine.o \
		FightControler.o \
		FightScriptBuilder.o \
		ScriptDataProvider.o \
		PropertyCalculator.o \
		GameBuffProc.o \
		ChessBoardHelper_TeamFight.o \
		ZhuangbeiGongMingManager.o \
		CS_Common.o \
		SuitPool.o \
		SkillHelperBase.o \
		SkillHelperAdapterImpl.o \
		FightScriptDefine.o \
		PlayerNameFactory.o

TOPHERO_CREATER_OBJS=\
		ConnServerThread.o \
		ProcServerThread.o \
		PacketQueue.o \
		tophero_creater.o \
		ChessBoardHelper.o \
		CommonDefine.o \
		FightControler.o \
		FightScriptBuilder.o \
		ScriptDataProvider.o \
		PropertyCalculator.o \
		GameBuffProc.o \
		ChessBoardHelper_TeamFight.o \
		ZhuangbeiGongMingManager.o \
		CS_Common.o \
		SuitPool.o \
		SkillHelperBase.o \
		SkillHelperAdapterImpl.o \
		FightScriptDefine.o \
		PlayerNameFactory.o

###-------------------------------------------------------------------------				
ALL_OBJS= $(BSLSVR_OBJS) $(COMM_OBJS) $(UTILITY_OBJS) $(DBCHECK_OBJS) $(ZB_TMP_UPDATE_OBJS) $(ROBOT_CREATER_OBJS) $(TOPHERO_CREATER_OBJS)

.phony: all


all: $(ALL_OBJS) lib 
		$(CXX) -o $(ARC_DIR)/hellgate_server   $(BSLSVR_OBJS) $(COMM_OBJS) $(UTILITY_OBJS) -L/usr/local/mysql/lib/mysql -lmysqlclient  -lm -lz -lpthread -lssl
		$(CXX) -o $(ARC_DIR)/db_check   $(DBCHECK_OBJS) $(COMM_OBJS) $(UTILITY_OBJS) -L/usr/local/mysql/lib/mysql -lmysqlclient  -lm -lz -lpthread -lssl
		$(CXX) -o $(ARC_DIR)/zb_tmp_updater   $(ZB_TMP_UPDATE_OBJS) $(COMM_OBJS) $(UTILITY_OBJS) -L/usr/local/mysql/lib/mysql -lmysqlclient  -lm -lz -lpthread -lssl
		$(CXX) -o $(ARC_DIR)/robot_creater   $(ROBOT_CREATER_OBJS) $(COMM_OBJS) $(UTILITY_OBJS) -L/usr/local/mysql/lib/mysql -lmysqlclient  -lm -lz -lpthread -lssl
		$(CXX) -o $(ARC_DIR)/tophero_creater   $(TOPHERO_CREATER_OBJS) $(COMM_OBJS) $(UTILITY_OBJS) -L/usr/local/mysql/lib/mysql -lmysqlclient  -lm -lz -lpthread -lssl
#----------------------------------------------------------------------------------------------
		
lib: 
		
		
##########################################################################

#*.o: %.o: %.cpp 
%.o: %.cpp 
		$(CXX) -c $< -o $(OBJS_DIR)/$@

clean:
	rm -rf *.o
