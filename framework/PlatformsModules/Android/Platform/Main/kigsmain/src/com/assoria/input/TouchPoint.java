package com.assoria.input;

public class TouchPoint
{
	int		id;
	Integer mState;
	float oldX;
	float oldY;
	float X;
	float Y;
	
	
	public TouchPoint()
	{
		mState = 0;
		X = 0;
		Y = 0;
		id = -1;
	}
	
	public int getId()
	{
		return id;
	}
	
	public int getState()
	{
		return mState;
	}
	
	public float getX()
	{
		return X;
	}
	
	public float getY()
	{
		return Y;
	}
	
	public void down(float x, float y, int id)
	{
		mState = 1;
		oldX = x;
		oldY = y;
		X = x;
		Y = y;
		this.id = id;
	}
	
   	public void up()
	{
		mState = 0;
		id = -1;
	}
	
	
	public boolean update(float x, float y)
	{
		if (oldX!=x || oldY!=y)
		{
			oldX = X;
			oldY = y;
			X = x;
			Y = y;
			return true;
		}
		else
		{
			return false;
		}
	}
	
}
