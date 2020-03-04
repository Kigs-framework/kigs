package com.assoria.filemanager;

import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.media.MediaScannerConnection;
import android.os.Environment;
import android.util.Log;

import com.assoria.kigsmain.kigsmainActivity;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.ByteBuffer;
import java.util.HashMap;
import java.io.RandomAccessFile;


class FileInPath {
	public HashMap<String, Boolean> mMap;
}

class FileExistResult {
	FileExistResult() {
		Exist = false;
		isDir = false;
	}

	public boolean Exist;
	public boolean isDir;
};

// manage file open / read / write / close ...
public class FileManager {

	public static void initFileManager(AssetManager manager, Context context) {
		mManager = manager;
		mContext = context;

		mFileMap = new HashMap<String, FileInPath>(100);
	}

	static public String GetFullPath(String dir, int location) {
		File file;

		switch (location) {
		case 0: // asset
			return dir;
		case 3: // APPLICATION_STORAGE
			file = new File(mContext.getFilesDir(), dir);
			return file.getAbsolutePath();
		case 2: // DEVICE_STORAGE
		case 5: // DOCUMENT_FOLDER
			file = new File(Environment.getExternalStorageDirectory() , dir);
			return file.getAbsolutePath();
		}

		return "";
	}

	static public FileExistResult FileExist(String path, int location) {
		FileExistResult result = new FileExistResult();

		InputStream is = null;

		try {
			switch (location) {
			case 0: // asset
				is = mManager.open(path);
				break;

			case 2: // DEVICE_STORAGE
			case 3: // APPLICATION_STORAGE
			case 5: // DOCUMENT_FOLDER
				File file = new File(path);

				if (file.exists()) {
					result.Exist = true;
					result.isDir = file.isDirectory();
					return result;
				}
				break;
			}

			if (is == null) {
				return result;
			}

			is.close();

		} catch (Exception e) {
			return result;
		}
		
		result.Exist = true;
		return result;
	}

	static public String CheckFile(String filePath, int location) {
		String fullPath = GetFullPath(filePath, location);
		String ret = filePath + "|";

		FileExistResult lResult = FileExist(fullPath, location);

		ret += (lResult.Exist == true) ? "1|" : "0|";
		ret += (lResult.isDir) ? "1" : "0";

		return ret;
	}

	static public void createFolder(String name, int location) {
		// use short name only
		String root = GetFullPath(name, location);

		int lastDirectoryPos = root.lastIndexOf("/");
		if (lastDirectoryPos != -1) {
			String L_Directory = root.substring(0, lastDirectoryPos);
			File L_Dir = new File(L_Directory);
			try{
						L_Dir.mkdirs();
			}
			catch(Exception e)
			{
			}
		}
	}

	static public FILE_DESCRIPTOR openFile(String name, int location, boolean read, boolean append) {
		FILE_DESCRIPTOR returnedstream = null;

		switch (location) {
		case 0: //assets
			try {
				InputStream stream = mManager.open(name, AssetManager.ACCESS_RANDOM);
				if (stream != null) {
					returnedstream = new FILE_DESCRIPTOR();
					returnedstream.mStream = stream;
					returnedstream.mReadFile = read;
					returnedstream.mCurrentPosition = 0;
					returnedstream.mPath = name;
				}
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
			break;

		case 2: // DEVICE_STORAGE
		case 3: //APPLICATION_STORAGE
		case 5: //DOCUMENT_FOLDER
			if (isExternalStorageWritable()) {
				// use short name only
				String root = GetFullPath(name, location);
				createFolder(name, location);
				
				File myDir = new File(root);
				returnedstream = new FILE_DESCRIPTOR();
				try
				{
					myDir.createNewFile();
					returnedstream.mRandomAccessFile = new RandomAccessFile(myDir, read ? "r" : "rw");
					returnedstream.mReadFile = read;
					returnedstream.mPath = myDir.getPath();
					if(append)
						returnedstream.mRandomAccessFile.seek(returnedstream.mRandomAccessFile.length());
				}
			 	catch (FileNotFoundException e) 
			 	{
					e.printStackTrace();
				}
				catch (IOException e)
				{
					e.printStackTrace();
				}
			}
			break;
		}
		return returnedstream;
	}

	static public int FileSeek(FILE_DESCRIPTOR FD, int pos, int origin) 
	{
		if (FD != null) 
		{
			if(FD.mStream != null)
			{
				// Resource
				if (FD.mData == null) {
					FD.InitFile();
				}
				switch (origin) {
				case 0:
					FD.mCurrentPosition = pos;
					if (FD.mCurrentPosition > FD.mSize) {
						FD.mCurrentPosition = FD.mSize;
						return -1;
					}
					break;
				case 1:
					FD.mCurrentPosition += pos;
					if (FD.mCurrentPosition > FD.mSize) {
						FD.mCurrentPosition = FD.mSize;
						return -1;
					} else if (FD.mCurrentPosition < 0) {
						FD.mCurrentPosition = 0;
						return -1;
					}
					break;
				case 2:
					FD.mCurrentPosition = FD.mSize;
					break;
				}
			}
			else
			{
				// External File
				try{
					switch(origin)
					{
						case 0:
							FD.mRandomAccessFile.seek(pos);
							break;
						case 1:
							FD.mRandomAccessFile.skipBytes(pos);
							break;
						case 2:
							FD.mRandomAccessFile.seek(FD.mRandomAccessFile.length());
							break;
						default:
							return -1;	
					}
				}
				catch (IOException e)
				{
					e.printStackTrace();
					return -1;
				}
			}
			return 0;
		}
		return -1;
	}

	static public int FileTell(FILE_DESCRIPTOR FD) {
		if (FD != null) 
		{
			try
			{
				return FD.mStream != null ? FD.mCurrentPosition : (int)FD.mRandomAccessFile.getFilePointer();
			}
			catch (IOException e)
			{
				e.printStackTrace();
				return -1;
			}
		}
		return -1;
	}

	static public void closeFile(FILE_DESCRIPTOR FD) {
		if (FD != null) {
			try {
				if(FD.mStream != null)
				{
					FD.mStream.close();
				}
				else
				{
					FD.mRandomAccessFile.close();
					MediaScannerConnection.scanFile(kigsmainActivity.getContext(), new String[] { FD.mPath }, null, null);
				}
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}

	static int cur_size = -1;
	static byte[] cur_buf = null;

	static public int writeFile(FILE_DESCRIPTOR FD, ByteBuffer towritebuffer, int size) {
		if (FD == null) {
			return 0;
		}
		if (FD.mReadFile) {
			return 0;
		}

		// Only available for external files
		try {
			
			if(cur_size < size)
			{
				cur_size = size < 1024 ? 1024 : size;
				cur_buf = new byte[cur_size];
			}
			towritebuffer.get(cur_buf, 0, size);
			FD.mRandomAccessFile.write(cur_buf, 0, size);


		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		return size;
	}

	static public int readFile(FILE_DESCRIPTOR FD, ByteBuffer returnedbytebuffer, int size) 
	{
		if (FD != null) 
		{
			if(size == 0) return 0;
			
			if(FD.mStream != null)
			{
				// Resource
				if (FD.mData == null) {
					FD.InitFile();
				}
				// TODO handle eof

				if(FD.mCurrentPosition == FD.mSize) return -1;

				if(FD.mCurrentPosition + size > FD.mSize)
					size = FD.mSize - FD.mCurrentPosition;

				returnedbytebuffer.put(FD.mData.toByteArray(), FD.mCurrentPosition, size);
				FD.mCurrentPosition += size;

				return size;
			}
			else
			{
				// External File
				try
				{
					//Log.d("READ", "reading " + FD.mPath);
					if(cur_size < size)
					{
						cur_size = size < 1024 ? 1024 : size;
						cur_buf = new byte[cur_size];
					}
					int read = FD.mRandomAccessFile.read(cur_buf, 0, size);
					if(read >= 0)
						returnedbytebuffer.put(cur_buf, 0, read);

					return read;
				}
				catch (IOException e) 
				{
					e.printStackTrace();
					return -1;
				}
			}
		}
		return -1;
	}

	static public void PrintDirectory(String fname) {

		try {
			String list[] = mManager.list(fname);
			if (list.length != 0) {
				int i;
				for (i = 0; i < list.length; i++) {
					PrintDirectory(fname + "/" + list[i]);
				}
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	/* Checks if external storage is available for read and write */
	static public boolean isExternalStorageWritable() {
		String state = Environment.getExternalStorageState();
		return Environment.MEDIA_MOUNTED.equals(state);
	}

	/* Checks if external storage is available to at least read */
	static public boolean isExternalStorageReadable() {
		String state = Environment.getExternalStorageState();
		return Environment.MEDIA_MOUNTED.equals(state) || Environment.MEDIA_MOUNTED_READ_ONLY.equals(state);
	}

	static private AssetManager mManager;
	static private Context mContext;
	static private HashMap<String, FileInPath> mFileMap;

}
