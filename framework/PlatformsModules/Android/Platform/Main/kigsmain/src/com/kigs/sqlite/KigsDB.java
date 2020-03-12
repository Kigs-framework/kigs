package com.kigs.sqlite;

import android.database.Cursor;
import android.database.SQLException;
import android.database.sqlite.SQLiteDatabase;
import android.util.Log;

import com.kigs.filemanager.FILE_DESCRIPTOR;
import com.kigs.filemanager.FileManager;
import com.kigs.kigsmain.kigsmainActivity;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.util.concurrent.Semaphore;


public class KigsDB
{
    private KigsSQLiteHelper DBHelper;
    private SQLiteDatabase DB;

    private Semaphore mutex = new Semaphore(1);

    public static void initKigsDB()
    {
        KigsSQLiteHelper.initKigsSQLiteHelper();
    }

    public void Init(String DB_name)
    {
        DBHelper = new KigsSQLiteHelper(kigsmainActivity.getContext(), DB_name);
        DB = DBHelper.getWritableDatabase();
        Open();
    }

    public void Open()
    {
        DBHelper.onOpen(DB);
    }

    public void Close()
    {
        DBHelper.close();
    }

    public boolean ExecCommand(String command)
    {
        try
        {
            DB.execSQL(command);
            return true;
        }
        catch(SQLException e)
        {
            Log.e("SQLException : ", "" + e);
            return false;
        }
    }

    public String[] Query(String command)
    {
        Cursor cursor = DB.rawQuery(command, null);

        int rows_number_last_query = cursor.getCount();
        int columns_number_last_query = cursor.getColumnCount();

        String[] results = new String[rows_number_last_query * columns_number_last_query];

        cursor.moveToFirst();
        int loop_cursor = 0;

        while(!cursor.isAfterLast())
        {
            for(int loop = 0; loop < columns_number_last_query; loop++)
            {
                //Log.i("Query Result", cursor.getString(loop) + "   (" + ((loop_cursor * columns_number_last_query) + loop) + ")");
                results[(loop_cursor * columns_number_last_query) + loop] = cursor.getString(loop);
            }
            cursor.moveToNext();
            loop_cursor++;
        }

        cursor.close();
        return results;
    }

    public void CopyFrom(KigsDB from)
    {
        Close();

       String fromPath = from.DB.getPath();
        String toPath = DB.getPath();

        try
        {
            mutex.acquire();
            try
            {
                try
                {
                    FileChannel src = new FileInputStream(fromPath).getChannel();
                    FileChannel dst = new FileOutputStream(toPath).getChannel();
                    dst.transferFrom(src, 0, src.size());
                    src.close();
                    dst.close();
                }
                catch (IOException e)
                {

                }
            }
            finally
            {
                DB = DBHelper.getWritableDatabase();
                Open();

                mutex.release();
            }
        }
        catch(InterruptedException ie)
        {

        }
    }

    public static boolean ExistDatabase( String dbName)
    {
        File dbFile = kigsmainActivity.getContext().getDatabasePath(dbName);
        return dbFile.exists();
    }

    private void copyInputStreamToFile( InputStream in, File file ) {
        try {
            OutputStream out = new FileOutputStream(file);
            byte[] buf = new byte[1024];
            int len;
            while((len=in.read(buf))>0){
                out.write(buf,0,len);
            }
            out.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void CopyFromPath(String from)
    {
        Close();

        String toPath = DB.getPath();

        try
        {
            mutex.acquire();
            try
            {
                InputStream in = kigsmainActivity.getContext().getAssets().open(from);
                File target = new File(toPath);

                copyInputStreamToFile(in,target);

                in.close();
                /*int size = 1024*16;
                ByteBuffer buf = ByteBuffer.allocate(size);
                int readCount = FileManager.readFile(fdin,buf,size);
                while(readCount!=0) {
                    fdout.write(buf.array());

                    buf.clear();
                    readCount = FileManager.readFile(fdin,buf,size);
                }

                FileManager.closeFile(fdin);
                fdout.close();*/
            }
            catch (Exception e)
            {
                e.printStackTrace();
            }
            finally
            {
                DB = DBHelper.getWritableDatabase();
                Open();

                mutex.release();
            }
        }
        catch(InterruptedException ie)
        {

        }
    }
}

