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

package com.softwinner.explore;

import java.io.File;
import java.util.ArrayList;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

/**
 * This class sits between the Main activity and the FileManager class. 
 * To keep the FileManager class modular, this class exists to handle 
 * UI events and communicate that information to the FileManger class
 * 
 * This class is responsible for the buttons onClick method. If one needs
 * to change the functionality of the buttons found from the Main activity
 * or add button logic, this is the class that will need to be edited.
 * 
 * This class is responsible for handling the information that is displayed
 * from the list view (the files and folder) with a a nested class TableRow.
 * The TableRow class is responsible for displaying which icon is shown for each
 * entry. For example a folder will display the folder icon, a Word doc will 
 * display a word icon and so on. If more icons are to be added, the TableRow 
 * class must be updated to display those changes. 
 * 
 * @author Joe Berria
 */
public class EventHandler implements OnClickListener{
	/*
	 * Unique types to control which file operation gets
	 * performed in the background
	 */
    private static final int SEARCH_TYPE =		0x00;
	private static final int COPY_TYPE =		0x01;
	private static final int UNZIP_TYPE =		0x02;
	private static final int UNZIPTO_TYPE =		0x03;
	private static final int ZIP_TYPE =			0x04;
	private static final int DELETE_TYPE = 		0x05;
	private static final int MANAGE_DIALOG =	 0x06;
	
	public static final int TREEVIEW_MODE = 1;
	public static final int CATALOG_MODE = 2;
	
	private int	mlistmode = TREEVIEW_MODE;
	
	private final Context mContext;
	private final FileManager mFileMang;
	private final CatalogList mCataList;
	private TableRow mDelegate;
	private boolean multi_select_flag = false;
	private boolean delete_after_copy = false;
	private boolean thumbnail_flag = true;
	private int mColor = Color.WHITE;
	
	//the list used to feed info into the array adapter and when multi-select is on
	private ArrayList<String> mDataSource, mMultiSelectData;
	private TextView mPathLabel;
	private TextView mInfoLabel;
	
	private View preView;

	private void UpdateButtons(int mode)
	{
		ImageButton toolbox = (ImageButton)((Activity) mContext).findViewById(R.id.manage_button);
		ImageButton multi = (ImageButton)((Activity) mContext).findViewById(R.id.multiselect_button);

		switch(mode)
		{
		case TREEVIEW_MODE:
			toolbox.setEnabled(true);
			multi.setEnabled(true);
			break;
		case CATALOG_MODE:
			toolbox.setEnabled(false);
			multi.setEnabled(false);
			break;
		}
	}
	
	/**
	 * Creates an EventHandler object. This object is used to communicate
	 * most work from the Main activity to the FileManager class.
	 * 
	 * @param context	The context of the main activity e.g  Main
	 * @param manager	The FileManager object that was instantiated from Main
	 */
	public EventHandler(Context context, final FileManager manager,final CatalogList CataList) {
		mContext = context;
		mFileMang = manager;
		mCataList = CataList;
		
		mDataSource = new ArrayList<String>(mFileMang.getHomeDir(FileManager.ROOT_FLASH));
	}

	/**
	 * This method is called from the Main activity and this has the same
	 * reference to the same object so when changes are made here or there
	 * they will display in the same way.
	 * 
	 * @param adapter	The TableRow object
	 */
	public void setListAdapter(TableRow adapter) {
		mDelegate = adapter;
	}
	
	/**
	 * This method is called from the Main activity and this has the same
	 * reference to the same object so when changes are made here or there
	 * they will display in the same way.
	 * 
	 * @param adapter	The TableRow object
	 */
	public int getMode() {
		return mlistmode;
	}
	/**
	 * This method is called from the Main activity and is passed
	 * the TextView that should be updated as the directory changes
	 * so the user knows which folder they are in.
	 * 
	 * @param path	The label to update as the directory changes
	 * @param label	the label to update information
	 */
	public void setUpdateLabels(TextView path, TextView label) {
		mPathLabel = path;
		mInfoLabel = label;
	}
	
	/**
	 * 
	 * @param color
	 */
	public void setTextColor(int color) {
		mColor = color;
	}
	
	/**
	 * Set this true and thumbnails will be used as the icon for image files. False will
	 * show a default image. 
	 * 
	 * @param show
	 */
	public void setShowThumbnails(boolean show) {
		thumbnail_flag = show;
	}
	
	/**
	 * If you want to move a file (cut/paste) and not just copy/paste use this method to 
	 * tell the file manager to delete the old reference of the file.
	 * 
	 * @param delete true if you want to move a file, false to copy the file
	 */
	public void setDeleteAfterCopy(boolean delete) {
		delete_after_copy = delete;
	}
	
	/**
	 * Indicates whether the user wants to select 
	 * multiple files or folders at a time.
	 * <br><br>
	 * false by default
	 * 
	 * @return	true if the user has turned on multi selection
	 */
	public boolean isMultiSelected() {
		return multi_select_flag;
	}
	
	/**
	 * Use this method to determine if the user has selected multiple files/folders
	 * 
	 * @return	returns true if the user is holding multiple objects (multi-select)
	 */
	public boolean hasMultiSelectData() {
		return (mMultiSelectData != null && mMultiSelectData.size() > 0);
	}
	
	/**
	 * Will search for a file then display all files with the 
	 * search parameter in its name
	 * 
	 * @param name	the name to search for
	 */
	public void searchForFile(String name) {
		new BackgroundWork(SEARCH_TYPE).execute(name);
	}
	
	/**
	 * Will delete the file name that is passed on a background
	 * thread.
	 * 
	 * @param name
	 */
	public void deleteFile(String name) {
		new BackgroundWork(DELETE_TYPE).execute(name);
	}
	
	/**
	 * Will copy a file or folder to another location.
	 * 
	 * @param oldLocation	from location
	 * @param newLocation	to location
	 */
	public void copyFile(String oldLocation, String newLocation) {
		String[] data = {oldLocation, newLocation};
		
		new BackgroundWork(COPY_TYPE).execute(data);
	}
	
	/**
	 * 
	 * @param newLocation
	 */
	public void copyFileMultiSelect(String newLocation) {
		String[] data;
		int index = 1;
		
		if (mMultiSelectData.size() > 0) {
			data = new String[mMultiSelectData.size() + 1];
			data[0] = newLocation;
			
			for(String s : mMultiSelectData)
				data[index++] = s;
			
			new BackgroundWork(COPY_TYPE).execute(data);
		}
	}
	
	/**
	 * This will extract a zip file to the same directory.
	 * 
	 * @param file	the zip file name
	 * @param path	the path were the zip file will be extracted (the current directory)
	 */
	public void unZipFile(String file, String path) {
		new BackgroundWork(UNZIP_TYPE).execute(file, path);
	}
	
	/**
	 * This method will take a zip file and extract it to another
	 * location
	 *  
	 * @param name		the name of the of the new file (the dir name is used)
	 * @param newDir	the dir where to extract to
	 * @param oldDir	the dir where the zip file is
	 */
	public void unZipFileToDir(String name, String newDir, String oldDir) {
		new BackgroundWork(UNZIPTO_TYPE).execute(name, newDir, oldDir);
	}
	
	/**
	 * Creates a zip file
	 * 
	 * @param zipPath	the path to the directory you want to zip
	 */
	public void zipFile(String zipPath) {
		new BackgroundWork(ZIP_TYPE).execute(zipPath);
	}

	/**
	 *  This method, handles the button presses of the top buttons found
	 *  in the Main activity. 
	 */
	@Override
	public void onClick(View v) {
		
		switch(v.getId()) {
		
			case R.id.back_button:	
				if (mlistmode != TREEVIEW_MODE)
				{
					break;
				}
				
				if (!mFileMang.isRoot()) {
					if(multi_select_flag) {
						mDelegate.killMultiSelect(true);
						Toast.makeText(mContext, "Multi-select is now off", 
									   Toast.LENGTH_SHORT).show();
					}
					updateDirectory(mFileMang.getPreviousDir());
					if(mPathLabel != null)
						mPathLabel.setText(mFileMang.getCurrentDir());
				}
				break;
			
			case R.id.home_sdcard_button:
				refreshFocus(preView,v);
				if(mFileMang.whichRoot() == FileManager.ROOT_SDCARD &&
					mlistmode == TREEVIEW_MODE)
				{
					break;
				}
				mlistmode = TREEVIEW_MODE;
				if(multi_select_flag) {
					mDelegate.killMultiSelect(true);
					Toast.makeText(mContext, "Multi-select is now off", 
								   Toast.LENGTH_SHORT).show();
				}
				updateDirectory(mFileMang.getHomeDir(FileManager.ROOT_SDCARD));
				if(mPathLabel != null)
					mPathLabel.setText(mFileMang.getCurrentDir());
				break;
				
			case R.id.home_usbhost_button:
				refreshFocus(preView,v);
				if(mFileMang.whichRoot() == FileManager.ROOT_USBHOST &&
					mlistmode == TREEVIEW_MODE)
				{
					break;
				}
				mlistmode = TREEVIEW_MODE;
				if(multi_select_flag) {
					mDelegate.killMultiSelect(true);
					Toast.makeText(mContext, "Multi-select is now off", 
								   Toast.LENGTH_SHORT).show();
				}
				updateDirectory(mFileMang.getHomeDir(FileManager.ROOT_USBHOST));
				if(mPathLabel != null)
					mPathLabel.setText(mFileMang.getCurrentDir());
				break;
				
			case R.id.home_flash_button:
				Log.d("EventHandler", "------1------");
				refreshFocus(preView,v);
				if(mFileMang.whichRoot() == FileManager.ROOT_FLASH &&
					mlistmode == TREEVIEW_MODE)
				{
					break;
				}
				mlistmode = TREEVIEW_MODE;
				if(multi_select_flag) {
					mDelegate.killMultiSelect(true);
					Toast.makeText(mContext, "Multi-select is now off", 
								   Toast.LENGTH_SHORT).show();
				}
				updateDirectory(mFileMang.getHomeDir(FileManager.ROOT_FLASH));
				if(mPathLabel != null)
					mPathLabel.setText(mFileMang.getCurrentDir());	
				break;
				
			case R.id.manage_button:
				refreshFocus(preView,v);
				display_dialog(MANAGE_DIALOG);
				break;
				
			case R.id.multiselect_button:	
				refreshFocus(preView,v);
				if(getMode() != TREEVIEW_MODE)
				{
					break;
				}
				
				if(multi_select_flag) {
					mDelegate.killMultiSelect(true);				
					
				} else {
					LinearLayout hidden_lay = 
						(LinearLayout)((Activity) mContext).findViewById(R.id.hidden_buttons);
					
					multi_select_flag = true;
					hidden_lay.setVisibility(LinearLayout.VISIBLE);
				}
				break;
			
			/* 
			 * three hidden buttons for multiselect
			 */
			case R.id.hidden_attach:
				/* check if user selected objects before going further */
				if(mMultiSelectData == null || mMultiSelectData.isEmpty()) {
					mDelegate.killMultiSelect(true);
					break;
				}
				
				ArrayList<Uri> uris = new ArrayList<Uri>();
				int length = mMultiSelectData.size();
				Intent mail_int = new Intent();
    			
    			mail_int.setAction(android.content.Intent.ACTION_SEND_MULTIPLE);
    			mail_int.setType("application/mail");
    			mail_int.putExtra(Intent.EXTRA_BCC, "");
    			mail_int.putExtra(Intent.EXTRA_SUBJECT, " ");
    			
    			for(int i = 0; i < length; i++) {
    				File file = new File(mMultiSelectData.get(i));
    				uris.add(Uri.fromFile(file));
    			}
    			
    			mail_int.putParcelableArrayListExtra(Intent.EXTRA_STREAM, uris);
    			mContext.startActivity(Intent.createChooser(mail_int, 
    													   "Email using..."));
    			
    			mDelegate.killMultiSelect(true);
				break;
			case R.id.image_button:
				mlistmode = CATALOG_MODE;
				setFileList(mCataList.SetFileTyp(CatalogList.TYPE_PICTURE));
				if(mPathLabel != null)
					mPathLabel.setText("Picture");
				refreshFocus(preView,v);
				break;
				
			case R.id.movie_button:
				mlistmode = CATALOG_MODE;
				setFileList(mCataList.SetFileTyp(CatalogList.TYPE_MOVIE));
				if(mPathLabel != null)
					mPathLabel.setText("Movie");
				refreshFocus(preView,v);
				break;
				
			case R.id.hidden_move:
			case R.id.hidden_copy:
				/* check if user selected objects before going further */
				if(mMultiSelectData == null || mMultiSelectData.isEmpty()) {
					mDelegate.killMultiSelect(true);
					break;
				}
				
				if(v.getId() == R.id.hidden_move)
					delete_after_copy = true;
					
				mInfoLabel.setText("Holding " + mMultiSelectData.size() + 
								   " file(s)");
				
				mDelegate.killMultiSelect(false);
				break;
				
			case R.id.hidden_delete:	
				/* check if user selected objects before going further */
				if(mMultiSelectData == null || mMultiSelectData.isEmpty()) {
					mDelegate.killMultiSelect(true);
					break;
				}

				final String[] data = new String[mMultiSelectData.size()];
				int at = 0;
				
				for(String string : mMultiSelectData)
					data[at++] = string;
				
				AlertDialog.Builder builder = new AlertDialog.Builder(mContext);
				builder.setMessage("Are you sure you want to delete " +
								    data.length + " files? This cannot be " +
								    "undone.");
				builder.setCancelable(false);
				builder.setPositiveButton("Delete", new DialogInterface.OnClickListener() {
					@Override
					public void onClick(DialogInterface dialog, int which) {
						new BackgroundWork(DELETE_TYPE).execute(data);
						mDelegate.killMultiSelect(true);
					}
				});
				builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
					@Override
					public void onClick(DialogInterface dialog, int which) {
						mDelegate.killMultiSelect(true);
						dialog.cancel();
					}
				});
				
				builder.create().show();
				break;
		}
		UpdateButtons(mlistmode);
	}
	
	public void getInitView(View v){
		preView = v;
	}
	
	private void refreshFocus(View pre,View cur) {
		if( pre != cur)
		{
			cur.setSelected(true);
			pre.setSelected(false);
			preView = cur;
		}
	}
	
	/**
	 * will return the data in the ArrayList that holds the dir contents. 
	 * 
	 * @param position	the indext of the arraylist holding the dir content
	 * @return the data in the arraylist at position (position)
	 */
	public String getData(int position) {
		
		if(position > mDataSource.size() - 1 || position < 0)
			return null;
		
		return mDataSource.get(position);
	}

	/**
	 * called to update the file contents as the user navigates there
	 * phones file system. 
	 * 
	 * @param content	an ArrayList of the file/folders in the current directory.
	 */
	public void updateDirectory(ArrayList<String> content) {	
		if(!mDataSource.isEmpty())
			mDataSource.clear();
		
		for(String data : content)
			mDataSource.add(data);
		
		mDelegate.notifyDataSetChanged();
	}

	/**
	 * called to refresh the file list
	 * 
	 * @param content	an ArrayList of the file/folders in the current directory.
	 */
	public void setFileList(ArrayList<String> content) {
		if(mDataSource.equals(content))
		{
			return;
		}
		if(!mDataSource.isEmpty())
			mDataSource.clear();
		
		mDataSource.addAll(content);
		
		/*
		 * File list have been change,so clear the thumbnail
		 */
		mDelegate.clearThumbnail();
		mDelegate.notifyDataSetChanged();
	}
	
	/**
	 * This private method is used to display options the user can select when
	 * the tool box button is pressed. The WIFI option is commented out as it doesn't
	 * seem to fit with the overall idea of the application. However to display it, just 
	 * uncomment the below code and the code in the AndroidManifest.xml file.
	 */
	private void display_dialog(int type) {
		AlertDialog.Builder builder;
    	AlertDialog dialog;
    	
    	switch(type) {
    		case MANAGE_DIALOG:
    			//un-comment WIFI Info here and in the manifest file 
    	    	//to display WIFI info. Also uncomment and change case number below
    	    	CharSequence[] options = {"Process Info", /*"Wifi Info",*/ "Application backup"};
    	    	
    	    	builder = new AlertDialog.Builder(mContext);
    	    	builder.setTitle("Tool Box");
    	    	builder.setIcon(R.drawable.toolbox);
    	    	builder.setItems(options, new DialogInterface.OnClickListener() {
    	    		
    				public void onClick(DialogInterface dialog, int index) {
    					Intent i;
    					
    					switch(index) {
    						case 0:
    							i = new Intent(mContext, ProcessManager.class);
    							mContext.startActivity(i);
    							break;
    	/*
    						case 1:
    							i = new Intent(context, WirelessManager.class);
    							context.startActivity(i);
    							break;
    	*/
    						case 1:
    							i = new Intent(mContext, ApplicationBackup.class);
    							mContext.startActivity(i);
    							break;
    					}
    				}
    			});
    	    	dialog = builder.create();
    	    	dialog.show();
    			break;
    	}
	}
	
	private static class ViewHolder {
		TextView topView;
		TextView bottomView;
		ImageView icon;
		ImageView mSelect;	//multi-select check mark icon
	}

	
	/**
	 * A nested class to handle displaying a custom view in the ListView that
	 * is used in the Main activity. If any icons are to be added, they must
	 * be implemented in the getView method. This class is instantiated once in Main
	 * and has no reason to be instantiated again. 
	 * 
	 * @author Joe Berria
	 */
    public class TableRow extends ArrayAdapter<String> {
    	private final int KB = 1024;
    	private final int MG = KB * KB;
    	private final int GB = MG * KB;
    	private String display_size;
    	private String dir_name;
    	private ArrayList<Integer> positions;
    	private LinearLayout hidden_layout;
    	private ThumbnailCreator thumbnail;
    	
    	public TableRow() {
    		super(mContext, R.layout.tablerow, mDataSource);
    		
    		thumbnail = new ThumbnailCreator(32, 32);
    		dir_name = new DevicePath(mContext).getInterStoragePath();
    	}
    	
    	public void addMultiPosition(int index, String path) {
    		if(positions == null)
    			positions = new ArrayList<Integer>();
    		
    		if(mMultiSelectData == null) {
    			positions.add(index);
    			add_multiSelect_file(path);
    			
    		} else if(mMultiSelectData.contains(path)) {
    			if(positions.contains(index))
    				positions.remove(new Integer(index));
    			
    			mMultiSelectData.remove(path);
    			
    		} else {
    			positions.add(index);
    			add_multiSelect_file(path);
    		}
    		
    		notifyDataSetChanged();
    	}
   	
    	/**
    	 * This will turn off multi-select and hide the multi-select buttons at the
    	 * bottom of the view. 
    	 * 
    	 * @param clearData if this is true any files/folders the user selected for multi-select
    	 * 					will be cleared. If false, the data will be kept for later use. Note:
    	 * 					multi-select copy and move will usually be the only one to pass false, 
    	 * 					so we can later paste it to another folder.
    	 */
    	public void killMultiSelect(boolean clearData) {
    		hidden_layout = (LinearLayout)((Activity)mContext).findViewById(R.id.hidden_buttons);
    		hidden_layout.setVisibility(LinearLayout.GONE);
    		multi_select_flag = false;
    		
    		if(positions != null && !positions.isEmpty())
    			positions.clear();
    		
    		if(clearData)
    			if(mMultiSelectData != null && !mMultiSelectData.isEmpty())
    				mMultiSelectData.clear();
    		
    		notifyDataSetChanged();
    	}
    	
    	public String getFilePermissions(File file) {
    		String per = "-";
    	    		
    		if(file.isDirectory())
    			per += "d";
    		if(file.canRead())
    			per += "r";
    		if(file.canWrite())
    			per += "w";
    		
    		return per;
    	}
    	
    	public void clearThumbnail() {
    		if(thumbnail_flag) {
    			thumbnail.clearBitmapCache();
    		}
    	}
    	@Override
    	public View getView(int position, View convertView, ViewGroup parent){
    		if(mlistmode == CATALOG_MODE)
    		{
    			return getView_catalog(position,convertView,parent);
    		}
    		else if (mlistmode == TREEVIEW_MODE)
    		{
        		return getView_tree(position,convertView,parent);
    		}
    		
    		return getView_tree(position,convertView,parent);
    	}
    	
    	private View getView_catalog(int position, View convertView, ViewGroup parent){
    		ViewHolder holder;
    		File file = new File(mDataSource.get(position));
    		
    		if(convertView == null) {
    			LayoutInflater inflater = (LayoutInflater) mContext.
    						getSystemService(Context.LAYOUT_INFLATER_SERVICE);
    			convertView = inflater.inflate(R.layout.tablerow, parent, false);
    			
    			holder = new ViewHolder();
    			holder.topView = (TextView)convertView.findViewById(R.id.top_view);
    			holder.bottomView = (TextView)convertView.findViewById(R.id.bottom_view);
    			holder.icon = (ImageView)convertView.findViewById(R.id.row_image);
    			holder.mSelect = (ImageView)convertView.findViewById(R.id.multiselect_icon);
    			
    			convertView.setTag(holder);
    		} else {
    			holder = (ViewHolder)convertView.getTag();
    		}
    				
    		  		
    		if (positions != null && positions.contains(position))
    			holder.mSelect.setVisibility(ImageView.VISIBLE);
    		else
    			holder.mSelect.setVisibility(ImageView.GONE);
    		
    		holder.topView.setTextColor(mColor);
    		holder.bottomView.setTextColor(mColor);
    		
    		if(file != null && file.isFile()) {
    			String ext = file.toString();
    			String sub_ext = ext.substring(ext.lastIndexOf(".") + 1);
    			
    			/* This series of else if statements will determine which 
    			 * icon is displayed 
    			 */
    			if (TypeFilter.getInstance().isPdfFile(sub_ext)) {
    				holder.icon.setImageResource(R.drawable.pdf);
    			
    			} else if (TypeFilter.getInstance().isMusicFile(sub_ext)) {
    				
    				holder.icon.setImageResource(R.drawable.music);
    			
    			} else if (TypeFilter.getInstance().isPictureFile(sub_ext)) {
    				
    				if(thumbnail_flag && file.length() != 0) {
	    				Bitmap thumb = thumbnail.hasBitmapCached(position);
	    				
	    				if(thumb == null) {

	    					holder.icon.setImageResource(R.drawable.image);
	    					final Handler mHandler = new Handler();
	    					boolean isJPG = false;
	    					if(sub_ext.equalsIgnoreCase("jpeg") || sub_ext.equalsIgnoreCase("jpg")){
	    						isJPG = true;
	    					}
	   						thumbnail.setBitmapToImageView(file.getPath(), 
	   													   mHandler, 
	   													   holder.icon,
	   													   isJPG,
	   													   position);
	   						
	    				} else {
	    					holder.icon.setImageBitmap(thumb);
	    				}
	    				
    				} else {
    					holder.icon.setImageResource(R.drawable.image);
    				}
    				
    			} else if(TypeFilter.getInstance().isMovieFile(sub_ext)) {
    				
    				holder.icon.setImageResource(R.drawable.movies);
    			
    			} else {
    				holder.icon.setImageResource(R.drawable.text);
    			}	
    		}
    		    		
    		String permission = getFilePermissions(file);
    		
    		if(file.isFile()) {
    			double size = file.length();
        		if (size > GB)
    				display_size = String.format("%.2f Gb ", (double)size / GB);
    			else if (size < GB && size > MG)
    				display_size = String.format("%.2f Mb ", (double)size / MG);
    			else if (size < MG && size > KB)
    				display_size = String.format("%.2f Kb ", (double)size/ KB);
    			else
    				display_size = String.format("%.2f bytes ", (double)size);
        		
        		if(file.isHidden())
        			holder.bottomView.setText("(hidden) | " + display_size +" | "+ permission);
        		else
        			holder.bottomView.setText(display_size +" | "+ permission);
        		
    		}
    		
    		holder.topView.setText(file.getName());
    		
    		return convertView;
    	}
    	
    	private View getView_tree(int position, View convertView, ViewGroup parent) {
    		ViewHolder holder;
    		int num_items = 0;
    		String temp = mFileMang.getCurrentDir();
    		File file = new File(temp + "/" + mDataSource.get(position));
    		String[] list = file.list();
    		
    		if(list != null)
    			num_items = list.length;
   
    		if(convertView == null) {
    			LayoutInflater inflater = (LayoutInflater) mContext.
    						getSystemService(Context.LAYOUT_INFLATER_SERVICE);
    			convertView = inflater.inflate(R.layout.tablerow, parent, false);
    			
    			holder = new ViewHolder();
    			holder.topView = (TextView)convertView.findViewById(R.id.top_view);
    			holder.bottomView = (TextView)convertView.findViewById(R.id.bottom_view);
    			holder.icon = (ImageView)convertView.findViewById(R.id.row_image);
    			holder.mSelect = (ImageView)convertView.findViewById(R.id.multiselect_icon);
    			
    			convertView.setTag(holder);
    		} else {
    			holder = (ViewHolder)convertView.getTag();
    		}
    		
    		/* This will check if the thumbnail cache needs to be cleared by checking
    		 * if the user has changed directories. This way the cache wont show
    		 * a wrong thumbnail image for the new image file 
    		 */
    		if(!dir_name.equals(temp) && thumbnail_flag) {
    			thumbnail.clearBitmapCache();
    			dir_name = temp;
    		}    			
    		  		
    		if (positions != null && positions.contains(position))
    			holder.mSelect.setVisibility(ImageView.VISIBLE);
    		else
    			holder.mSelect.setVisibility(ImageView.GONE);

    		holder.topView.setTextColor(mColor);
    		holder.bottomView.setTextColor(mColor);
    		
    		if(file != null && file.isFile()) {
    			String ext = file.toString();
    			String sub_ext = ext.substring(ext.lastIndexOf(".") + 1);
    			
    			/* This series of else if statements will determine which 
    			 * icon is displayed 
    			 */
    			if (TypeFilter.getInstance().isPdfFile(sub_ext)) {
    				holder.icon.setImageResource(R.drawable.pdf);
    			
    			} else if (TypeFilter.getInstance().isMusicFile(sub_ext)) {
    				
    				holder.icon.setImageResource(R.drawable.music);
    			
    			} else if (TypeFilter.getInstance().isPictureFile(sub_ext)) {
    				
    				if(thumbnail_flag && file.length() != 0) {
	    				Bitmap thumb = thumbnail.hasBitmapCached(position);
	    				boolean isJPG = false;
						if(sub_ext.equalsIgnoreCase("jpeg") || sub_ext.equalsIgnoreCase("jpg")){
							isJPG = true;
						}
	    				if(thumb == null) {
	    					holder.icon.setImageResource(R.drawable.image);
	    					final Handler mHandler = new Handler();
	   						thumbnail.setBitmapToImageView(file.getPath(), 
	   													   mHandler, 
	   													   holder.icon,
	   													   isJPG,
	   													   position);
	   						
	    				} else {
	    					holder.icon.setImageBitmap(thumb);
	    				}
	    				
    				} else {
    					holder.icon.setImageResource(R.drawable.image);
    				}
    				
    			} else if (TypeFilter.getInstance().isZipFile(sub_ext) ||
    					TypeFilter.getInstance().isGZipFile(sub_ext)) {
    				
    				holder.icon.setImageResource(R.drawable.zip);
    			
    			} else if(TypeFilter.getInstance().isMovieFile(sub_ext)) {
    				
    				holder.icon.setImageResource(R.drawable.movies);
    			
    			} else if(TypeFilter.getInstance().isWordFile(sub_ext)) {
    				
    				holder.icon.setImageResource(R.drawable.word);
    			
    			} else if(TypeFilter.getInstance().isExcelFile(sub_ext)) {
    				
    				holder.icon.setImageResource(R.drawable.excel);
    				
    			} else if(TypeFilter.getInstance().isPptFile(sub_ext)) {
    				
    				holder.icon.setImageResource(R.drawable.ppt);   	
    				
    			} else if(TypeFilter.getInstance().isHtml32File(sub_ext)) {
    				holder.icon.setImageResource(R.drawable.html32);  
    				
    			} else if(TypeFilter.getInstance().isXml32File(sub_ext)) {
    				holder.icon.setImageResource(R.drawable.xml32);
    				
    			} else if(TypeFilter.getInstance().isConfig32File(sub_ext)) {
    				holder.icon.setImageResource(R.drawable.config32);
    				
    			} else if(TypeFilter.getInstance().isApkFile(sub_ext)) {
    				holder.icon.setImageResource(R.drawable.appicon);
    				
    			} else if(TypeFilter.getInstance().isJarFile(sub_ext)) {
    				holder.icon.setImageResource(R.drawable.jar32);
    				
    			} else {
    				holder.icon.setImageResource(R.drawable.text);
    			}
    			
    		} else if (file != null && file.isDirectory()) {
    			holder.icon.setImageResource(R.drawable.folder);
    		}
		else{
			holder.icon.setImageResource(R.drawable.folder);
		}
    		    		
    		String permission = getFilePermissions(file);
    		
    		if(file.isFile()) {
    			double size = file.length();
        		if (size > GB)
    				display_size = String.format("%.2f Gb ", (double)size / GB);
    			else if (size < GB && size > MG)
    				display_size = String.format("%.2f Mb ", (double)size / MG);
    			else if (size < MG && size > KB)
    				display_size = String.format("%.2f Kb ", (double)size/ KB);
    			else
    				display_size = String.format("%.2f bytes ", (double)size);
        		
        		if(file.isHidden())
        			holder.bottomView.setText("(hidden) | " + display_size +" | "+ permission);
        		else
        			holder.bottomView.setText(display_size +" | "+ permission);
        		
    		} else {
    			if(file.isHidden())
    				holder.bottomView.setText("(hidden) | " + num_items + " items | " + permission);
    			else
    				holder.bottomView.setText(num_items + " items | " + permission);
    		}
    		
    		holder.topView.setText(file.getName());
    		
    		return convertView;
    	}
    	
    	private void add_multiSelect_file(String src) {
    		if(mMultiSelectData == null)
    			mMultiSelectData = new ArrayList<String>();
    		
    		mMultiSelectData.add(src);
    	}
    }
    
    /**
     * A private inner class of EventHandler used to perform time extensive 
     * operations. So the user does not think the the application has hung, 
     * operations such as copy/past, search, unzip and zip will all be performed 
     * in the background. This class extends AsyncTask in order to give the user
     * a progress dialog to show that the app is working properly.
     * 
     * (note): this class will eventually be changed from using AsyncTask to using
     * Handlers and messages to perform background operations. 
     * 
     * @author Joe Berria
     */
    private class BackgroundWork extends AsyncTask<String, Void, ArrayList<String>> {
    	private String file_name;
    	private ProgressDialog pr_dialog;
    	private int type;
    	private int copy_rtn;
    	
    	private BackgroundWork(int type) {
    		this.type = type;
    	}
    	
    	/**
    	 * This is done on the EDT thread. this is called before 
    	 * doInBackground is called
    	 */
    	@Override
    	protected void onPreExecute() {
    		
    		switch(type) {
    			case SEARCH_TYPE:
    				pr_dialog = ProgressDialog.show(mContext, "Searching", 
    												"Searching current file system...",
    												true, true);
    				break;
    				
    			case COPY_TYPE:
    				pr_dialog = ProgressDialog.show(mContext, "Copying", 
    												"Copying file...", 
    												true, false);
    				break;
    				
    			case UNZIP_TYPE:
    				pr_dialog = ProgressDialog.show(mContext, "Unzipping", 
    												"Unpacking zip file please wait...",
    												true, false);
    				break;
    				
    			case UNZIPTO_TYPE:
    				pr_dialog = ProgressDialog.show(mContext, "Unzipping", 
    												"Unpacking zip file please wait...",
    												true, false);
    				break;
    			
    			case ZIP_TYPE:
    				pr_dialog = ProgressDialog.show(mContext, "Zipping", 
    												"Zipping folder...", 
    												true, false);
    				break;
    				
    			case DELETE_TYPE:
    				pr_dialog = ProgressDialog.show(mContext, "Deleting", 
    												"Deleting files...", 
    												true, false);
    				break;
    		}
    	}

    	/**
    	 * background thread here
    	 */
    	@Override
		protected ArrayList<String> doInBackground(String... params) {
			
			switch(type) {
				case SEARCH_TYPE:
					file_name = params[0];
					ArrayList<String> found = mFileMang.searchInDirectory(mFileMang.getCurrentDir(), 
																	    file_name);
					return found;
					
				case COPY_TYPE:
					int len = params.length;
					
					if(mMultiSelectData != null && !mMultiSelectData.isEmpty()) {
						for(int i = 1; i < len; i++) {
							copy_rtn = mFileMang.copyToDirectory(params[i], params[0]);
							
							if(delete_after_copy)
								mFileMang.deleteTarget(params[i]);
						}
					} else {
						copy_rtn = mFileMang.copyToDirectory(params[0], params[1]);
						
						if(delete_after_copy)
							mFileMang.deleteTarget(params[0]);
					}
					
					delete_after_copy = false;
					return null;
					
				case UNZIP_TYPE:
					mFileMang.extractZipFiles(params[0], params[1]);
					return null;
					
				case UNZIPTO_TYPE:
					mFileMang.extractZipFilesFromDir(params[0], params[1], params[2]);
					return null;
					
				case ZIP_TYPE:
					mFileMang.createZipFile(params[0]);
					return null;
					
				case DELETE_TYPE:
					int size = params.length;
					
					for(int i = 0; i < size; i++)
						mFileMang.deleteTarget(params[i]);
					
					return null;
			}
			return null;
		}
		
    	/**
    	 * This is called when the background thread is finished. Like onPreExecute, anything
    	 * here will be done on the EDT thread. 
    	 */
    	@Override
		protected void onPostExecute(final ArrayList<String> file) {			
			final CharSequence[] names;
			int len = file != null ? file.size() : 0;
			
			switch(type) {
				case SEARCH_TYPE:				
					if(len == 0) {
						Toast.makeText(mContext, "Couldn't find " + file_name, 
											Toast.LENGTH_SHORT).show();
					
					} else {
						names = new CharSequence[len];
						
						for (int i = 0; i < len; i++) {
							String entry = file.get(i);
							names[i] = entry.substring(entry.lastIndexOf("/") + 1, entry.length());
						}
						
						AlertDialog.Builder builder = new AlertDialog.Builder(mContext);
						builder.setTitle("Found " + len + " file(s)");
						builder.setItems(names, new DialogInterface.OnClickListener() {
							
							public void onClick(DialogInterface dialog, int position) {
								String path = file.get(position);
								updateDirectory(mFileMang.getNextDir(path.
													substring(0, path.lastIndexOf("/")), true));
							}
						});
						
						AlertDialog dialog = builder.create();
						dialog.show();
					}
					
					pr_dialog.dismiss();
					break;
					
				case COPY_TYPE:
					if(mMultiSelectData != null && !mMultiSelectData.isEmpty()) {
						multi_select_flag = false;
						mMultiSelectData.clear();
					}
					
					if(copy_rtn == 0)
						Toast.makeText(mContext, "File successfully copied and pasted", 
											Toast.LENGTH_SHORT).show();
					else
						Toast.makeText(mContext, "Copy pasted failed", Toast.LENGTH_SHORT).show();
					
					pr_dialog.dismiss();
					mInfoLabel.setText("");
					break;
					
				case UNZIP_TYPE:
					updateDirectory(mFileMang.getNextDir(mFileMang.getCurrentDir(), true));
					pr_dialog.dismiss();
					break;
					
				case UNZIPTO_TYPE:
					updateDirectory(mFileMang.getNextDir(mFileMang.getCurrentDir(), true));
					pr_dialog.dismiss();
					break;
					
				case ZIP_TYPE:
					updateDirectory(mFileMang.getNextDir(mFileMang.getCurrentDir(), true));
					pr_dialog.dismiss();
					break;
					
				case DELETE_TYPE:
					if(mMultiSelectData != null && !mMultiSelectData.isEmpty()) {
						mMultiSelectData.clear();
						multi_select_flag = false;
					}
					
					updateDirectory(mFileMang.getNextDir(mFileMang.getCurrentDir(), true));
					pr_dialog.dismiss();
					mInfoLabel.setText("");
					break;
			}
		}
    }
}
