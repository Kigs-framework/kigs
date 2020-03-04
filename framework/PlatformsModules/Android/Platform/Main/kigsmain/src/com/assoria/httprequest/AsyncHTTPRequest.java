package com.assoria.httprequest;

import android.os.AsyncTask;

import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;

public class AsyncHTTPRequest
{
    public static native void  RequestEnded(int handle, boolean result);

    public static void initAsyncHTTPRequest() {    }

    public static AsyncHTTPRequest Query(String urlStr, boolean isASync, int handle)
    {
        AsyncHTTPRequest request = new AsyncHTTPRequest();
        request._Query(urlStr,isASync,handle);

        return request;
    }

    public String _content_encoding;
    public String _charset_encoding;
    public byte[] _result;
    public int _resultLen;
    public int _handle;
    private HTTPAsyncRequestTask asyncRequest;

    public void _Query(String urlStr, boolean isASync, int handle)
    {
        URL url = null;
        _result = null;
        _resultLen = 0;
        _content_encoding = "";
        _charset_encoding = "";
        _handle = handle;

        if(isASync)
        {
            asyncRequest = new HTTPAsyncRequestTask(this);
            asyncRequest.execute(urlStr);
        }
        else {
            try {
                url = new URL(urlStr);

                URLConnection conn = url.openConnection();

                InputStream is = new DataInputStream(conn.getInputStream());
                ByteArrayOutputStream buffer = new ByteArrayOutputStream();

                _content_encoding = conn.getContentEncoding();
                if (_content_encoding == null)
                    _content_encoding = "";

                String contentType = conn.getContentType();
                if (!contentType.isEmpty()) {
                    String[] strArray = contentType.split(";");
                    if (contentType.contains("charset"))
                        _charset_encoding = strArray[1].split("=")[1];
                }

                int nRead;
                byte[] data = new byte[1024*64];

                while ((nRead = is.read(data, 0, data.length)) != -1) {
                    buffer.write(data, 0, nRead);
                    _resultLen += nRead;
                }

                buffer.flush();

                _result = buffer.toByteArray();

            } catch (MalformedURLException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public int GetSize()
    {
        return _resultLen;
    }

    public String GetContentEncoding()
    {
        return _content_encoding;
    }

    public String GetCharsetEncoding()
    {
        return _charset_encoding;
    }

    public byte[] GetResult()
    {
        return _result;
    }
}


class HTTPAsyncRequestTask extends AsyncTask<String, Void, Boolean> {
    AsyncHTTPRequest master;

    public HTTPAsyncRequestTask(AsyncHTTPRequest r)
    {
        master=r;
    }

    /*@Override
    protected void onPreExecute() {
        super.onPreExecute();

    }*/

    @Override
    protected Boolean doInBackground(String... urls) {

        try {
            URL url = new URL(urls[0]);

            URLConnection conn = url.openConnection();

            InputStream is = new DataInputStream(conn.getInputStream());
            ByteArrayOutputStream buffer = new ByteArrayOutputStream();

            master._content_encoding = conn.getContentEncoding();
            if ( master._content_encoding == null)
                master._content_encoding = "";

            String contentType = conn.getContentType();
            if (contentType!=null && !contentType.isEmpty()) {
                String[] strArray = contentType.split(";");
                if (contentType.contains("charset"))
                    master._charset_encoding = strArray[1].split("=")[1];
            }

            int nRead;
            byte[] data = new byte[16384];

            while ((nRead = is.read(data, 0, data.length)) != -1) {
                buffer.write(data, 0, nRead);
                master._resultLen += nRead;
            }

            buffer.flush();

            master._result = buffer.toByteArray();
            return true;

        } catch (MalformedURLException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

        return false;
    }

    protected void onPostExecute(Boolean result) {
        AsyncHTTPRequest.RequestEnded(master._handle, result);
    }
}