#pragma once

enum ClientErrorCode
{
	CEC_START = 100,	// 避免protocol重复，这里就取100开始
	CEC_NullPointerException,
	CEC_RuntimeException,

	CEC_GoldNotEnough,	
	CEC_SilverNotEnough,	
	CEC_BaoShiNotEnough,	
	CEC_CaiLiaoNotEnough,	
	CEC_ExpNotEnough,	
	CEC_HeroUpdateLevelItemNotEnough,				// 升级道具不足
	CEC_HeroUpdateStageItemNotEnough,			// 升阶道具不足
	CEC_Shenbing_JiefengyinItemNotEnough,		// 道具不足
	CEC_Shenbing_JiandingItemNotEnough,					// 道具不足
	CEC_CanNotJingLian,						
	CEC_CanNotZhuanke,						
	CEC_JingwenZhuankeItemNotEnough,						
	CEC_NoArenaClgTime,						
	CEC_ArenaMarketBuyJiFenNotEnough,						
	CEC_NoArenaBuyClgTime,						
	CEC_ZahuopuGetOneItem_NoItem,		// 已经领完了					
	CEC_HeroLevelNotEnough,						
	CEC_NoHellBattleTime,						
	CEC_HeroIsMaxStage,					// 已经是最大阶				
	CEC_NeedUpLevelJiangTang,			// 			
	CEC_HeroIsMaxLevel,					// 已经是最大等级			
	CEC_NeedUpStageFirst,				// 需要先升阶才能下一步		
	CEC_YuanshenUpstageNeed5Mode,		// 元神升阶需要等级是5的mode	
	CEC_JiaoTangIsMaxLevel,	
	CEC_SkillIsMaxLevel,
	CEC_CDTimeIsNotOver,
	CEC_WorldBossIsNotOpen,
	CEC_NoReward,
	CEC_NoByTiLiTime,
	CEC_NoSaoDangTime,
	CEC_SaoDangTimeNotEnough,
	CEC_SaoDangTimeMustBigger0,
	CEC_RewardIsAlreadyGetted,
	CEC_NotShengBing,
	CEC_ShengBingIsMaxLevel,
	CEC_ShengBingNextLevelIsTooHight,
	CEC_CanNotUseThisEquipToLevelUpShenBing,
	CEC_EquipIsOnHero,
	CEC_TiLiIsFull,
	CEC_ActorIsAlreadyUnlocked,
	CEC_ActorIsDisableToUnlock,
	CEC_RongYuDianJiFenBuGou,
	CEC_CanNotCaiJieChuanQi,
	CEC_TiLiIsGetted,
	CEC_GettedTiLiTimeIsOver,		// 时间已过了
	CEC_GettedTiLiTimeIsNotOpen,	// 时间还没到
	CEC_KilledPutongNotEnough,
	CEC_KilledJingYinNotEnough,
	CEC_NoCastSilverTime,
	CEC_CanNotJingCai_NotInRound,// 还没开始
	CEC_CanNotJingCai_AlreadyGuessed,
	CEC_DFDJ_NotInited,
	CEC_DFDJ_HERO_IS_MAX_VALUE,
	CEC_CanNotJingCai_HaveNotMatch,
	CEC_ShenBinLuTimeOver,
	CEC_ShenBinLu_CanNotChlg_HeroIsLocked,
	CEC_HeroLevelIsNotEoungh,
	CEC_CanNotXueZhan,
	CEC_XueZhanNotOpen,
	CEC_XueZhanChlgTimeOver,
	CEC_PPT_NotEnough,
	CEC_XueZhanNoBox,
	CEC_SHZL_CanNotTiaoZhan,
	CEC_SHZL_LevelNotMatch,
	CEC_SHZL_SAFE,
	CEC_SHZL_CANNOT_CHLG_AGAIN_AFTER_LOSE,
	CEC_SHZL_CanNotWarshipSelf,
	CEC_SHZL_CanNotWarshipGW,
	CEC_SHZL_AreadyWarshiped,
	CEC_SHZL_CanNotWarshp,
	CEC_SHZL_CanNotTiaoZhanSelf,
	CEC_SHZL_CAN_NOT_GET_REWARD_NOT_THIS_SEASON,
	CEC_SHZL_CanNotGetReward,
	CEC_SHZL_NO_REWARD,
	CEC_SHZL_MUST_PERFECT_PASS,
	CEC_NO_TANSUO_TIME,
	CEC_CAN_NOT_JIEFENG,
	CEC_TOPHeroISCDTime,

	CEC_NoMoreFreeSpaceToStoreEquip,

	CEC_NoPurifyTimes,
	CEC_NoPurifyTools,
	CEC_ActorShangZhen_CannotSub, //上阵助手不能减少
	CEC_EmailContentOutSize,


	// 这期间的都是道具不够的错误CEC_ItemNotEnough_Begin+ItemsType_Gold就是金币不足
	CEC_ItemNotEnough_Begin = 300,
	CEC_ItemNotEnough_End = 1300,

	// 装备只能装备给【***】
	CEC_EquipOnlyCanUseForProfesion_Begin = 1400,
	CEC_EquipOnlyCanUseForProfesion_End = 1500,


};
