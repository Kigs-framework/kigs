package com.assoria.httprequest;

import com.assoria.kigsmain.kigsmainActivity;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;
import java.util.Map;
 

public class SyncHTTPRequest
{
    private static int _size;
    private static String _content_encoding;
    private static String _charset_encoding;
    private static String _result;

    public static void Query(String urlStr)
    {
        URL url = null;
        String result = "";

        try
        {
            url = new URL(urlStr);

            URLConnection urlConnection = null;

            urlConnection = url.openConnection();

            assert urlConnection != null;

            InputStream in = urlConnection.getInputStream();

            BufferedReader bufferreader = new BufferedReader(new InputStreamReader(in));

            String line;
            while ((line = bufferreader.readLine()) != null) {
                result += line;
            }
            in.close();
        }
        catch (MalformedURLException e)
        {
            e.printStackTrace();
        }
        catch(IOException e)
        {
            e.printStackTrace();
        }
    }

    public static int GetSize()
    {
        return _size;
    }

    public static String GetContentEncoding()
    {
        return _content_encoding;
    }

    public static String GetCharsetEncoding()
    {
        return _charset_encoding;
    }

    public static String GetResult()
    {
        return _result;
    }
}

