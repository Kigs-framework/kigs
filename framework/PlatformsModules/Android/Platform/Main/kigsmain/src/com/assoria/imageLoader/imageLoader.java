package com.assoria.imageLoader;

import java.io.InputStream;

import com.assoria.filemanager.FILE_DESCRIPTOR;
import com.assoria.filemanager.FileManager;

import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.util.Log;

public class imageLoader {
	
	public static void initImageLoader(AssetManager manager,Context context )
	{
		mManager=manager;
		mContext=context;
	}
	
	static public Bitmap open(String path)
    {
		if (path.charAt(0)=='#')
		{		
			switch(path.charAt(1))
			{
				case 3 : //  = FilePathManager::APPLICATION_STORAGE
				{
					FILE_DESCRIPTOR FD = FileManager.openFile(path.substring(3,path.length()),3,true,false);
					return BitmapFactory.decodeStream(FD.getInputStream());
				}
				case 5 : // = FilePathManager::DOCUMENT_FOLDER
				{
					FILE_DESCRIPTOR FD = FileManager.openFile(path.substring(3,path.length()),5,true,false);
					try{
						return BitmapFactory.decodeStream(FD.getInputStream());
					}
					catch (Exception e)
					{
					}
				}
			}		
		}
        try
        {
        	Log.i("imageLoader","load image :" + path);
        	InputStream localstream=mManager.open(path);
        	Log.i("imageLoader","stream OK");
        	Bitmap bmp= BitmapFactory.decodeStream(localstream);
        	Log.i("imageLoader","bmp OK");
        	return bmp;
        }
        catch (Exception e) 
        {
        	Log.i("imageLoader","exception");
        }
       
        return null;
    }

    static public int getWidth(Bitmap bmp) { return bmp.getWidth(); }
    static public int getHeight(Bitmap bmp) { return bmp.getHeight(); }

    static public void getPixels(Bitmap bmp, int[] pixels)
    {
        int w = bmp.getWidth();
        int h = bmp.getHeight();
        bmp.getPixels(pixels, 0, w, 0, 0, w, h);
    }

    static public void close(Bitmap bmp)
    {
        bmp.recycle();
    }
	
	static private AssetManager  					mManager;
	static private Context							mContext;
	
}
