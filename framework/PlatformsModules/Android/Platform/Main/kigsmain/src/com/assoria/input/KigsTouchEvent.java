package com.assoria.input;

public class KigsTouchEvent 
{
	public int 		mAction;
	public float 	mX;
	public float 	mY;
	
	public KigsTouchEvent()
	{
		mAction = 0;
		mX = 0;
		mY = 0;
	}
	
	public KigsTouchEvent(int action, float x, float y)
	{
		mAction = action;
		mX = x;
		mY = y;
	}
	
	public int	getAction()
	{
		return mAction;
	}
	
	public float	getX()
	{
		return mX;
	}
	
	public float	getY()
	{
		return mY;
	}
}

