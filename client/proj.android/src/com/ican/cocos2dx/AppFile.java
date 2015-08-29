package com.ican.cocos2dx;

import java.io.File;
import java.io.IOException;

import android.app.Activity;

public class AppFile {

	/**
	 * 创建一个doc目录下面的文件
	 * @param acticity
	 * @param dir		比如/infor/
	 * @param fileName 	比如"infor.txt"
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
			//这里内部已经判断了文件是否存在
			//不用考虑重复创建的问题
			file.createNewFile();
		} catch (IOException e) {
			e.printStackTrace();
			System.out.println("创建文件异常");
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
