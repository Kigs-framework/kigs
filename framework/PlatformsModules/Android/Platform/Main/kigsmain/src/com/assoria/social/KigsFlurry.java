package com.assoria.social;

import com.assoria.kigsmain.kigsmainActivity;
import com.flurry.android.FlurryAgent;

import java.util.Map;
 

public class KigsFlurry
{
    public static void Init(String key)
    {
        FlurryAgent.setLogEvents(true);
        FlurryAgent.setLogEnabled(true);
        FlurryAgent.init(kigsmainActivity.getContext(), key);
        FlurryAgent.onStartSession(kigsmainActivity.getContext());
        FlurryAgent.setReportLocation(true);
    }

    public static void Destroy()
    {
        FlurryAgent.onEndSession(kigsmainActivity.getContext());
    }

    public static void pageView()
    {
        FlurryAgent.onPageView();
    }

    public static void timedEvent(String event)
    {
        FlurryAgent.logEvent(event, true);
    }

    public static void timedEventParameter(String event, Map<String, String> parameters)
    {
        FlurryAgent.logEvent(event, parameters, true);
    }

    public static void endTimedEvent(String event)
    {
        FlurryAgent.endTimedEvent(event);
    }
}