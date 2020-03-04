package com.assoria.input;

import com.assoria.input.KigsTouchEvent;

public class KigsTouchEventList 
{
	KigsTouchEvent eventlist[];
	int			currentLength;
	
	public KigsTouchEventList()
	{
		currentLength=0;
		eventlist=new KigsTouchEvent[50];
		int i;
		for(i=0;i<50;i++)
		{
			eventlist[i]=new KigsTouchEvent();
		}
	}
	
	public 	boolean isEmpty()
	{
		return (currentLength==0);
	}
	
	public void addEvent(KigsTouchEvent e)
	{
		if(currentLength<50)
		{
			eventlist[currentLength].mAction=e.mAction;
			eventlist[currentLength].mX=e.mX;
			eventlist[currentLength].mY=e.mY;
			
			currentLength++;
		}
	}
	
	public int getEventCount()
	{
		return currentLength;
	}
	
	public KigsTouchEvent getEvent(int index)
	{
		return eventlist[index];
	}
	
	public	void	clearEventList()
	{
		currentLength=0;
	}
	
}

