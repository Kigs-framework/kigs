package com.kigs.sqlite;

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

public class KigsSQLiteHelper extends SQLiteOpenHelper
{
    public KigsSQLiteHelper(Context context, String dbName)
    {
        super(context, dbName, null, 1);
    }

    static void initKigsSQLiteHelper()
    {

    }

    @Override
    public void onCreate(SQLiteDatabase database)
    {

    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion)
    {
    }
}
