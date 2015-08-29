package com.ican.cocos2dx;

import java.io.File;
import java.io.IOException;

import android.app.Activity;

public class AppFile {

	/**
	 * ����һ��docĿ¼������ļ�
	 * @param acticity
	 * @param dir		����/infor/
	 * @param fileName 	����"infor.txt"
	 * @return
	 */
	public static File createFile(Activity acticity, String dir, String fileName) {
		
		String dataRoot = "/data/data/";
		String t1 = acticity.getPackageName();	 //		com.ican.cocos2dx
		String t2 =acticity.getPackageCodePath();//     /data/app/com.ican.cocos2dx-1.apk
		String t3 =acticity.getPackageResourcePath();// /data/app/com.ican.cocos2dx-1.apk
		
		String packagePath = t1;
		File dirFile = new File(dataRoot+packagePath+"/files/"+dir);
		dirFile.mkdirs();
		File file = new File(dirFile.getAbsolutePath()+"/"+fileName);
		try {
			//�����ڲ��Ѿ��ж����ļ��Ƿ����
			//���ÿ����ظ�����������
			file.createNewFile();
		} catch (IOException e) {
			e.printStackTrace();
			System.out.println("�����ļ��쳣");
			return null;
		}
		System.out.println("AppFile --> createFile : "+"[" + file +"]");
		return file;
	}	
	
	public static Boolean isExist(String fileFullPath)
	{
		File f = new File(fileFullPath);
		
		return f.exists();
	}
}
