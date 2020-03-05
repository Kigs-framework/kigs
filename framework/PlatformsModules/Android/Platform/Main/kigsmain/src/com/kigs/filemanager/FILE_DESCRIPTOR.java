package com.kigs.filemanager;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.RandomAccessFile;

public class FILE_DESCRIPTOR {

	InputStream					mStream;
	boolean						mReadFile;
	boolean						mAppend;
	int							mCurrentPosition;
	int							mSize;
	
	ByteArrayOutputStream		mData;
	byte[]						mByteArray;

	String						mPath;
	RandomAccessFile			mRandomAccessFile;
	
	public FILE_DESCRIPTOR()
	{
		mStream=null;
		mReadFile=true;
		mAppend=false;
		mCurrentPosition=0;
		mSize=-1;
		mData=null;
		mRandomAccessFile=null;
		mByteArray=null;
	}
	
	public InputStream getInputStream() {return mStream;}

	
	// read the stream and set size 
	public void		InitFile()
	{
		mCurrentPosition=0;
		mSize =0;

		if(mStream != null)
		{
			if(!mReadFile)
			{
				return;
			}
			int len;
			try {
				int size;
				
				size = mStream.available();
				
				mData = new ByteArrayOutputStream();

				if(size==0)
				{
					size=2048;
				}
				
				// Read the entire asset into a local byte buffer.
				byte[] buf = new byte[size];
				while ((len = mStream.read(buf)) != -1)
				{
					mData.write(buf, 0, len);
				}
				mData.close();
				mSize=mData.size();
				mByteArray = mData.toByteArray();
			} 
			catch (IOException e) 
			{
			}	
		}
		else if(mRandomAccessFile != null)
		{
			try{
				mSize=(int)mRandomAccessFile.length();
				}
				catch (IOException e) 
			{
			}	
		}
	}
	
	public int	getSize()
	{
		if(mStream != null)
		{
			// Resource
			if (mData == null) {
				InitFile();
			}
		}
		else if(mRandomAccessFile!=null)
		{
			if(mSize == -1)
			{
				InitFile();
			}
		}
		return mSize;
	}
}
