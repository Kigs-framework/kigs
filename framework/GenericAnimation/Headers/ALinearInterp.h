// ****************************************************************************
// * NAME: ALinearInterp.h
// * GROUP: Animation module
// * --------------------------------------------------------------------------
// * PURPOSE: manage linear interpolation on data
// * 
// * COMMENT: 
// * --------------------------------------------------------------------------
// * RELEASE: 
// ****************************************************************************

#ifndef __ALINEARINTERP_H__
#define __ALINEARINTERP_H__


// ----------------------------------------------------------------------------

// ****************************************
// * ALinearInterp class
// * --------------------------------------
/*!  Class used to manage linear interpolation on data (used to fade between animation by changing animation weight)
     \ingroup Animation
*/
// ****************************************

class   ALinearInterp
{
public:

    // ----------------------------------------------------------------------------

    // *******************
    // * Structors
    // * -----------------
    /*! Constructor from a start value, an end value, a start time,
        a duration, and the data to interpolate
    */
    // *******************
    ALinearInterp(Float start,Float end,ATimeValue start_t,ATimeValue l_t, AnimationResourceInfo* data) :
		  m_StartingValue(start)
		, m_EndingMinusStartingValue(end - start)
		, m_TimeCoef(KFLOAT_CONST(1.0f) / (Float)l_t)
		, m_StartingTime(start_t)
		, m_Data(data)
    {
       
    };

    // *******************
    // * Structors
    // * -----------------
    /*! Destructor
    */
    // *******************

    ~ALinearInterp()
    {
    };

    // ----------------------------------------------------------------------------

    // *******************
    // * GetFade
    // * -----------------
    /*! return the value of the fade ( in [start,end] ) for the given time
    */ 
    // *******************

    Float  GetFade(ATimeValue t,bool& finished)
    {
        // get coef between KFLOAT_CONST(0.0) and KFLOAT_CONST(1.0)

        finished=false;
        Float coef=(Float)(t-m_StartingTime)*m_TimeCoef;

        if(coef < KFLOAT_CONST(0.0f))
        {
            coef = KFLOAT_CONST(0.0f);
        }
        if(coef >= KFLOAT_CONST(1.0f))
        {
            coef = KFLOAT_CONST(1.0f);
            finished=true;
        }

        return (m_StartingValue + ( m_EndingMinusStartingValue ) * coef);

    };

    // ----------------------------------------------------------------------------

    // *******************
    // * GetData
    // * -----------------
    /*! return a pointer on the starting data for the fading
    */ 
    // *******************

	AnimationResourceInfo*   GetData()
    {
        return m_Data;
    };


protected:

    Float						m_StartingValue;
    Float						m_EndingMinusStartingValue;
    Float						m_TimeCoef;
    ATimeValue					m_StartingTime;
	AnimationResourceInfo*      m_Data;
};


#endif //__ALINEARINTERP_H__


