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
 * һ��Cocos2dx ��C++�˺�Java�˵�һ�������ࡣ
 * ��װ��һЩ���õĹ���������<br/><br/>
 * 
 * <uses-permission android:name="android.permission.INTERNET"/>
 * 
 * ������������� �Լ��� C++��Ҫ��JavaͨѶ��һ������<br/>
 * ���������Ҫ��һ�����ݶ�׼����֮�󣬾ͻ����{@link LFCocos2dxHelper.lFCocos2dxHelperReady};
 * 
 * ����ͷ�ļ��ģ�
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
	 * ��ʼ��������Ϸ��GameActivity������Ӧ�ó�ʼ����
	 * @param game
	 * @param adManager
	 */
	public static void init(Activity game)
	{
		gameActivity = game;
	}
	
 	/**
 	 * �������ݣ�����doc������ļ�
 	 * ���紫�����createDocFile("infor/",infor.txt);
 	 * ��ô�ͻ��ڴ����ļ� /data/data/[��·��]/files/infor/infor.txt
 	 * @param dir		Ŀ¼���磺infor ����infor/����
 	 * @param fileName 	�ļ�����infor.txt
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
 	 * ������Ļ�������
 	 * <uses-permission android:name="android.permission.WAKE_LOCK"></uses-permission>
 	 * ��ҪȨ�޵����á�
 	 * ǩ����(Z)V
 	 * @param awake
 	 */
 	public static void setScreenWake(boolean awake)
 	{
// 		if (awake) 
// 		{
// 			gameActivity.getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
// 			log("����-��");
//		}
// 		else
//		{
// 			gameActivity.getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
// 			log("����-��");
//		}
 	}
 	
	/**
 	 * ��URl��C++������char* ����
 	 * @param url
 	 */
 	public static void openUrl(byte[] url)
 	{
 		if (gameActivity == null) {
 			log("LFCocos2dxHelper,��û�г�ʼ����");
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
 	 * �õ��豸��Id�ַ�������Ҫ����Ȩ��
 	 * <uses-permission android:name="android.permission.READ_PHONE_STATE"/>
 	 * ��������ҳ
 	 * http://blog.csdn.net/cocodehouse/article/details/5925722
 	 * </pre>
 	 * @return
 	 */
 	public static String getDeviceId()
 	{
 		if (gameActivity == null) {
 			log("LFCocos2dxHelper��getDeviceId,��û�г�ʼ����");
			return "";
		}
 		TelephonyManager tm = (TelephonyManager) gameActivity.getSystemService(Context.TELEPHONY_SERVICE); 
 		final String tmDevice; 
 		tmDevice = "" + tm.getDeviceId(); 
 		
 		return tmDevice;
 	}
 	
 	/**
 	 * ��ȡ�ֻ�����
 	 * ��ҪȨ��
 	 * <uses-permission android:name="android.permission.READ_PHONE_STATE"/>
 	 * @return
 	 */
 	public static String getPhoneNumber()
 	{
 		if (gameActivity == null) {
 			log("LFCocos2dxHelper��getPhoneNumber,��û�г�ʼ����");
			return "";
		}
 		TelephonyManager tm = (TelephonyManager) gameActivity.getSystemService(Context.TELEPHONY_SERVICE); 
 		return tm.getLine1Number();
 	}
 	
 	/**
 	 * ���Ͷ��ţ���ҪȨ�ޣ�
 	 * <uses-permission android:name="android.permission.SEND_SMS" />
 	 * ���쳣���;ͷ���true�����򷵻�false
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
 	 * ��ϷoPauseʱ����Ҫ����mGLView.onPause();
 	 * ���ǵ������ǵ�LFCocos2dxHelper.oPause();
 	 * �����Ͳ����̨�ͷ���Դ��
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
	 * ֪ͨC++ ��Ϸ�Ѿ���ͣ�ˣ�C++�ǱߵĴ�ͳ�������ǵ��ã�<br/>
	 * void AppDelegate::applicationDidEnterBackground()
	 * @return
	 */
	private static native void gameActivityOnPause();
 	
}
