/*
    Open Manager, an open source file manager for the Android system
    Copyright (C) 2009, 2010, 2011  Joe Berria <nexesdevelopment@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
 this file written by dhm,for file catalog and filter
 */
package com.softwinner.explore;

import java.io.File;
import java.io.FileFilter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;

import android.content.Context;



public class CatalogList {
	public final static int TYPE_MUSIC = 1;
	public final static int TYPE_MOVIE = 2;
	public final static int TYPE_EBOOK = 3;
	public final static int TYPE_PICTURE = 4;
	public final static int TYPE_UNKNOWN = 0xff;
	
	public final static int STORAGE_USBHOST = 1;
	public final static int STORAGE_SDCARD = 2;
	public final static int STORAGE_FLASH = 3;
	public final static int STORAGE_UNKNOWN = 4;
	
	private ArrayList<String> mlist = null;
	private int m_filetype = TYPE_UNKNOWN;
	private FileFilter mfilter = null;
	
	private String flashpath;
	private String sdcardpath;
	private String usbhostpath;
	
	private static final Comparator alph = new Comparator<String>() {
		@Override
		public int compare(String arg0, String arg1) {
			String str0 = arg0.substring(arg0.lastIndexOf("/") + 1);
			String str1 = arg1.substring(arg1.lastIndexOf("/") + 1);;
			return str0.toLowerCase().compareTo(str1.toLowerCase());
		}
	};
	
	//init all filters may be used
	private static final FileFilter musicfliter = new FileFilter() {

		@Override
		public boolean accept(File pathname) {
			// TODO Auto-generated method stub
			//keep all directions and needed files
			if(pathname.isDirectory())
			{
				return true;
			}
			
			String name = pathname.getAbsolutePath();
			String item_ext = null;
			
			try {
	    		item_ext = name.substring(name.lastIndexOf(".") + 1, name.length());
	    		
	    	} catch(IndexOutOfBoundsException e) {	
	    		item_ext = ""; 
	    	}
			if(TypeFilter.getInstance().catalogMusicFile(item_ext))
			{
				return true;
			}
			
			return false;
		}
	};
	
	private static final FileFilter moviefliter = new FileFilter() {

		@Override
		public boolean accept(File pathname) {
			// TODO Auto-generated method stub
			//keep all directions and needed files
			if(pathname.isDirectory())
			{
				return true;
			}
			
			String name = pathname.getAbsolutePath();
			String item_ext = null;
			
			try {
	    		item_ext = name.substring(name.lastIndexOf(".") + 1, name.length());
	    		
	    	} catch(IndexOutOfBoundsException e) {	
	    		item_ext = ""; 
	    	}
			if(TypeFilter.getInstance().catalogMovieFile(item_ext))
			{
				return true;
			}
			
			return false;
		}
	};
	
	private static final FileFilter ebookfliter = new FileFilter() {

		@Override
		public boolean accept(File pathname) {
			// TODO Auto-generated method stub
			//keep all directions and needed files
			if(pathname.isDirectory())
			{
				return true;
			}
			
			String name = pathname.getAbsolutePath();
			String item_ext = null;
			
			try {
	    		item_ext = name.substring(name.lastIndexOf(".") + 1, name.length());
	    		
	    	} catch(IndexOutOfBoundsException e) {	
	    		item_ext = ""; 
	    	}
			if(TypeFilter.getInstance().catalogEbookFile(item_ext))
			{
				return true;
			}
			
			return false;
		}
	};
	
	private static final FileFilter picturefliter = new FileFilter() {

		@Override
		public boolean accept(File pathname) {
			// TODO Auto-generated method stub
			//keep all directions and needed files
			if(pathname.isDirectory())
			{
				return true;
			}
			
			String name = pathname.getAbsolutePath();
			String item_ext = null;
			
			try {
	    		item_ext = name.substring(name.lastIndexOf(".") + 1, name.length());
	    		
	    	} catch(IndexOutOfBoundsException e) {	
	    		item_ext = ""; 
	    	}
			if(TypeFilter.getInstance().catalogPictureFile(item_ext))
			{
				return true;
			}
			
			return false;
		}
	};
	
	public CatalogList(Context context){
		mlist = new ArrayList<String>();
		DevicePath devices = new DevicePath(context);
		flashpath = devices.getSdStoragePath();
		sdcardpath = devices.getInterStoragePath();
		usbhostpath = devices.getUsbStoragePath();
	}
	
	private void attachPathToList(File file){
		File[] filelist = file.listFiles(mfilter);
		
		/*
		 * add by chenjd,chenjd@allwinnertech 2011-09-14
		 * other application:mediaScanner will save its thumbnail data here, so 
		 * I must not scan this area. 
		 */
		String pathToIgnored = sdcardpath + "/" + "DCIM";
		
		int i = 0;
		
		if(filelist == null)
		{
			return;
		}
		
		for(i = 0;i < filelist.length;i ++)
		{
			if(filelist[i].isDirectory())
			{
				String mPath = filelist[i].getPath();
				//Log.d("CatalogList",mPath);
				if(!mPath.equalsIgnoreCase(pathToIgnored))
				{
					attachPathToList(filelist[i]);
				}
			}
			else
			{
				if(mlist.size() >= 500)
				{
					return;
				}
				mlist.add(filelist[i].getAbsolutePath());
			}
		}
	}
	
	private void attachPathToList(String path){
		File file = new File(path);
		attachPathToList(file);
	}
	
	public ArrayList<String> listSort()
	{
		Object[] tt = mlist.toArray();
		mlist.clear();
		
		Arrays.sort(tt, alph);
		
		for (Object a : tt){
			mlist.add((String)a);
		}
		return mlist;
	}
	
	public ArrayList<String> SetFileTyp(int filetype){
		m_filetype = filetype;
			
		mlist.clear();
		switch(filetype)
		{
			case TYPE_MUSIC:
				mfilter = musicfliter;
				break;
			case TYPE_MOVIE:
				mfilter = moviefliter;
				break;
			case TYPE_EBOOK:
				mfilter = ebookfliter;
				break;
			case TYPE_PICTURE:
				mfilter = picturefliter;
				break;
			default:
				return null;
		}
			
		//we only need these three storage
		attachPathToList(flashpath + "/");
		attachPathToList(sdcardpath + "/");
		attachPathToList(usbhostpath + "/");
		//maybe sort?
		Object[] tt = mlist.toArray();
		mlist.clear();
			
		Arrays.sort(tt, alph);
			
		for (Object a : tt){
			mlist.add((String)a);
		}
		return mlist;
	}
		
	public ArrayList<String> AttachMediaStorage(int media_typ){
		//maybe sort?
		switch(media_typ){
		case STORAGE_USBHOST:
			attachPathToList(usbhostpath + "/");
			break;
		case STORAGE_SDCARD:
			attachPathToList(sdcardpath + "/");
			break;
		case STORAGE_FLASH:
			attachPathToList(flashpath + "/");
			break;
		default:
			break;
		}
		return mlist;
	}
	
	public ArrayList<String> DisAttachMediaStorage(int media_typ){
		int i = mlist.size() - 1;
		
		switch(media_typ){
		case STORAGE_USBHOST:
			for( ;i >= 0;i --)
			{
				if(mlist.get(i).startsWith(usbhostpath))
				{
					mlist.remove(i);
				}
			}
			break;
		case STORAGE_SDCARD:
			for( ;i >= 0;i --)
			{
				if(mlist.get(i).startsWith(sdcardpath))
				{
					mlist.remove(i);
				}
			}
			break;
		case STORAGE_FLASH:
			for( ;i >= 0;i --)
			{
				if(mlist.get(i).startsWith(flashpath))
				{
					mlist.remove(i);
				}
			}
			break;
		default:
			break;
		}
		return mlist;
	}
}

