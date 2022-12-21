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
    ALinearInterp(Float start,Float end,ATimeValue start_t,ATimeValue l_t, const KigsID& data) :
		  m_StartingValue(start)
		, m_EndingMinusStartingValue(end - start)
		, m_TimeCoef(1.0f / (Float)l_t)
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
        // get coef between 0.0f and 1.0f

        finished=false;
        Float coef=(Float)(t-m_StartingTime)*m_TimeCoef;

        if(coef < 0.0f)
        {
            coef = 0.0f;
        }
        if(coef >= 1.0f)
        {
            coef = 1.0f;
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

	const KigsID&   GetData() const
    {
        return m_Data;
    };


protected:

    Float						m_StartingValue;
    Float						m_EndingMinusStartingValue;
    Float						m_TimeCoef;
    ATimeValue					m_StartingTime;
	KigsID                      m_Data;
};


#endif //__ALINEARINTERP_H__


