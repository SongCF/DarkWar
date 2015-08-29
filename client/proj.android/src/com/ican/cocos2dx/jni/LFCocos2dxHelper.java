package com.ican.cocos2dx.jni;

import java.util.List;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.telephony.TelephonyManager;
import android.telephony.SmsManager;
import android.util.Log;

import com.ican.cocos2dx.AppFile;

/**
 * 一个Cocos2dx 的C++端和Java端的一个帮助类。
 * 封装了一些常用的公共方法。<br/><br/>
 * 
 * <uses-permission android:name="android.permission.INTERNET"/>
 * 
 * 这里完成了所有 自己的 C++需要个Java通讯的一切事情<br/>
 * 当这个类需要的一切数据都准备好之后，就会调用{@link LFCocos2dxHelper.lFCocos2dxHelperReady};
 * 
 * 生成头文件的：
 * cd /cygdrive/e/cocos2d-2.1rc0-x-2.1.3/NetFight/proj.android/bin/classes
 * 
 * javah -jni com.ican.cocos2dx.jni.LFCocos2dxHelper
 * 
 * javap -s com.ican.cocos2dx.jni.LFCocos2dxHelper
 * 
 * @author LiFeng
 *
 */
public class LFCocos2dxHelper{
	
	private static Activity gameActivity = null;
	private static int STATE_Thread = 0;
	private static int STATE_UI = 1;
	private static int STATE_GL = 2;
	private static int CALLBACK_STATE = STATE_Thread;
	
	private static void log(String text)
	{
		Log.d("LFCocos2dxHelper", text);
	}
	/**
	 * 初始化，在游戏的GameActivity启动就应该初始化。
	 * @param game
	 * @param adManager
	 */
	public static void init(Activity game)
	{
		gameActivity = game;
	}
	
 	/**
 	 * 传入数据，创建doc下面的文件
 	 * 比如传入的是createDocFile("infor/",infor.txt);
 	 * 那么就会在创建文件 /data/data/[包路径]/files/infor/infor.txt
 	 * @param dir		目录比如：infor 或者infor/都行
 	 * @param fileName 	文件名：infor.txt
 	 * @return
 	 */
 	public static boolean createDocFile(String dir,String fileName)
 	{
 		if (gameActivity == null) {
 			System.out.println("createDocFile ---> gameActivity is NULL");
			return false;
		}
 		return AppFile.createFile(gameActivity, dir, fileName) == null;
 	}
 	
 	/**
 	 * 设置屏幕常量与否
 	 * <uses-permission android:name="android.permission.WAKE_LOCK"></uses-permission>
 	 * 需要权限的配置。
 	 * 签名：(Z)V
 	 * @param awake
 	 */
 	public static void setScreenWake(boolean awake)
 	{
// 		if (awake) 
// 		{
// 			gameActivity.getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
// 			log("常亮-开");
//		}
// 		else
//		{
// 			gameActivity.getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
// 			log("常亮-关");
//		}
 	}
 	
	/**
 	 * 打开URl，C++传过来char* 即可
 	 * @param url
 	 */
 	public static void openUrl(byte[] url)
 	{
 		if (gameActivity == null) {
 			log("LFCocos2dxHelper,还没有初始化！");
 			return;
		}
 		openUrl(new String(url));
 	}
 	public static void  openUrl(String url) {
 		System.out.println("openUrl"+"["+url+"]");
		Uri uri =Uri.parse(url); 
		Intent it = new Intent(Intent.ACTION_VIEW,uri); 
		gameActivity.startActivity(it);
	}
 	
 	/**
 	 * <pre>
 	 * 得到设备的Id字符串。需要如下权限
 	 * <uses-permission android:name="android.permission.READ_PHONE_STATE"/>
 	 * 见如下网页
 	 * http://blog.csdn.net/cocodehouse/article/details/5925722
 	 * </pre>
 	 * @return
 	 */
 	public static String getDeviceId()
 	{
 		if (gameActivity == null) {
 			log("LFCocos2dxHelper：getDeviceId,还没有初始化！");
			return "";
		}
 		TelephonyManager tm = (TelephonyManager) gameActivity.getSystemService(Context.TELEPHONY_SERVICE); 
 		final String tmDevice; 
 		tmDevice = "" + tm.getDeviceId(); 
 		
 		return tmDevice;
 	}
 	
 	/**
 	 * 获取手机号码
 	 * 需要权限
 	 * <uses-permission android:name="android.permission.READ_PHONE_STATE"/>
 	 * @return
 	 */
 	public static String getPhoneNumber()
 	{
 		if (gameActivity == null) {
 			log("LFCocos2dxHelper：getPhoneNumber,还没有初始化！");
			return "";
		}
 		TelephonyManager tm = (TelephonyManager) gameActivity.getSystemService(Context.TELEPHONY_SERVICE); 
 		return tm.getLine1Number();
 	}
 	
 	/**
 	 * 发送短信，需要权限：
 	 * <uses-permission android:name="android.permission.SEND_SMS" />
 	 * 无异常发送就返回true，否则返回false
 	 * @param phoneNumber
 	 * @param text
 	 */
 	public static boolean sendSms(String phoneNumber,String text)
 	{
 		SmsManager smsManager = SmsManager.getDefault();   
 		List<String> divideContents = smsManager.divideMessage(text);     
 		for (String part : divideContents) {     
 		    smsManager.sendTextMessage(phoneNumber, null, part, null, null);     
 		}  

 		return true;
 	}
 	
 	/**
 	 * 游戏oPause时，不要调用mGLView.onPause();
 	 * 而是调用我们的LFCocos2dxHelper.oPause();
 	 * 这样就不会后台释放资源了
 	 * 
 	 * 	 @Override
		 protected void onPause() {
		     super.onPause();
		     LFCocos2dxHelper.oPause();
	//	     mGLView.onPause();
		 }
 	 */
 	public static void oPause()
 	{
 		gameActivityOnPause();
 	}
	
	/**
	 * 通知C++ 游戏已经暂停了，C++那边的传统做法就是调用：<br/>
	 * void AppDelegate::applicationDidEnterBackground()
	 * @return
	 */
	private static native void gameActivityOnPause();
 	
}
