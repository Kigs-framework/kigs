package com.assoria.input;

import android.view.KeyEvent;
import android.view.inputmethod.InputMethodManager;
import android.view.WindowManager;

import com.assoria.kigsmain.kigsmainActivity;
import com.assoria.kigsmain.KigsGLSurfaceView;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.BitSet;
import android.os.Handler;
import android.util.Log;
import android.content.Context;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.view.View;
import android.R;

import java.lang.Runnable;


class KeyboardRunnable implements Runnable
{
    // ----------------------- Constants ----------------------- //
    private static final String TAG = "KEYBOARD_RUNNABLE";

    // Runnable Interval
    private static final int INTERVAL_MS = 100;

	
	private View myTextEdit = null;

    // ----------------------- Overrides ----------------------- //
    @Override
    public void run()
    {
        // Get Input Method Manager
		kigsmainActivity c = (kigsmainActivity)kigsmainActivity.getContext();
		myTextEdit = c.getSurfaceView();
		// show the keyboard so we can enter text
		InputMethodManager imm = (InputMethodManager) (c.getSystemService(Context.INPUT_METHOD_SERVICE));
		/*if(myTextEdit == null)
		{
			myTextEdit = new EditText(c);
			LinearLayout ll = new LinearLayout(c);
			ll.addView(myTextEdit);
			c.setContentView(ll);
			Post();
		}
        // Check view is focusable
        else*/ if (!(myTextEdit.isFocusable() && myTextEdit.isFocusableInTouchMode()))
        {
            Log.i(TAG, "Non focusable view");
			return;
        }
        // Try focusing
        else if (!myTextEdit.requestFocus())
        {
			Log.i(TAG, "Cannot focus on view");
			Post();
        }
        // Check if Imm is active with this view
        else if (!imm.isActive(myTextEdit))
        {
            Log.i(TAG, "IMM is not active");
            Post();
        }
        // Show Keyboard
		else if (!imm.showSoftInput(myTextEdit, InputMethodManager.SHOW_IMPLICIT))
        {
            Log.i(TAG, "Unable to show keyboard");
            Post();
        }
    }

    // ----------------------- Public APIs ----------------------- //
    public static void Hide()
    {
		kigsmainActivity c = (kigsmainActivity)kigsmainActivity.getContext();
		// show the keyboard so we can enter text
		InputMethodManager imm = (InputMethodManager) (c.getSystemService(Context.INPUT_METHOD_SERVICE)); 
	    imm.hideSoftInputFromWindow(c.getSurfaceView().getWindowToken(), InputMethodManager.HIDE_IMPLICIT_ONLY);
    }

    // ----------------------- Private APIs ----------------------- //
    protected void Post()
    {
        // Post this aftr 100 ms
        new Handler().postDelayed(this, INTERVAL_MS);
    }
}

public class KigsKeyboard {
	//public static native void  PushKeyEvent(ByteBuffer b);

	static byte NoEvent[] = { 0 };
	static ByteBuffer mBuffer = ByteBuffer.allocate(64 * KeyStruct.ClassSize + 4);

	static void ShowKeyboard(boolean b)
	{
		kigsmainActivity c = (kigsmainActivity)kigsmainActivity.getContext();


		// show the keyboard so we can enter text
		//mIMM = (InputMethodManager) (c.getSystemService(Context.INPUT_METHOD_SERVICE));
		if(b)
		{
			c.runOnUiThread(new KeyboardRunnable());
			//ok = new Handler(Looper.getMainLooper()).postDelayed(new KeyboardRunnable(), 100);
		}
		else 
			KeyboardRunnable.Hide();

		/*
		if(b)
		{
			c.getSurfaceView().requestFocus();
			mIMM.showSoftInput(c.getSurfaceView(), InputMethodManager.SHOW_IMPLICIT);
			//mIMM.toggleSoftInput(InputMethodManager.SHOW_FORCED, 0);
		}
		else
			mIMM.hideSoftInputFromWindow(c.getSurfaceView().getWindowToken(), InputMethodManager.HIDE_NOT_ALWAYS);
		*/
	}

	public static void dispatchKeyEvent(KeyEvent event) {
		//Log.d("NONO", "event : code("+event.getKeyCode()+") unicode("+Integer.toString(event.getUnicodeChar())+") = "+event.getAction());

		KeyStruct ks = new KeyStruct();
		ks.Action = event.getAction();
		ks.KeyCode = event.getKeyCode();
		ks.Unicode = event.getUnicodeChar();
		if (event.isPrintingKey())
			ks.Flag.set(0);

		mKeyEvent.add(ks);

		/*mBuffer.clear();
		mBuffer.order(ByteOrder.nativeOrder());
		ks.toByteArray(mBuffer);
		PushKeyEvent(mBuffer);*/
	}

	public static byte[] GetKeyActions() {
		int size = mKeyEvent.size();
		if (size == 0)
			return NoEvent;

		mBuffer.clear();
		mBuffer.order(ByteOrder.nativeOrder());

		mBuffer.putInt(size);

		for (KeyStruct ks : mKeyEvent) {
			ks.toByteArray(mBuffer);
		}
		byte[] arr = mBuffer.array();
		return arr;
	}

	public static void Clear() {
		mKeyEvent.clear();
	}

	static ArrayList<KeyStruct> mKeyEvent = new ArrayList();
}

class KeyStruct {
	static byte[] bytes = new byte[4];
	public static final int ClassSize = 16;

	int Action;
	int KeyCode;
	int Unicode;

	// 1 >> isPrintable
	BitSet Flag = new BitSet(32);

	public void toByteArray(ByteBuffer buff) {
		//ByteBuffer b = ByteBuffer.allocate(ClassSize);
		buff.putInt(Action);
		buff.putInt(KeyCode);
		buff.putInt(Unicode);

		bytes[0] = bytes[1] = bytes[2] = bytes[3] = 0;
		for (int i = Flag.nextSetBit(0); i >= 0; i = Flag.nextSetBit(i + 1)) {
			bytes[i / 8] |= 1 << (i % 8);
		}
		buff.put(bytes);
	}
}