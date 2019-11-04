// ****************************************************************************
// * NAME: AIndexInterval.h
// * GROUP: Animation module
// * --------------------------------------------------------------------------
// * PURPOSE: 
// * 
// * COMMENT: 
// * --------------------------------------------------------------------------
// * RELEASE: 
// ****************************************************************************

#ifndef __AINDEXINTERVAL_H__
#define __AINDEXINTERVAL_H__

#include "AMDefines.h"


// ----------------------------------------------------------------------------

// ****************************************
// * AIndexInterval class
// * --------------------------------------
/*! Class used to store a min and a max index
    \ingroup Animation
*/ 
// ****************************************

class   AIndexInterval
{
public:

    // ******************************
    // * Structors
    // *-----------------------------
    /*! Constructor from a min and a max index
    */ 
    // ******************************

    AIndexInterval(IntU32    min,IntU32    max)
    {
        m_Min=min;
        m_Max=max;
    };

    // ******************************
    // * Structors
    // *-----------------------------
    /*! Destructor
    */ 
    // ******************************

    ~AIndexInterval();

    // ******************************
    // * Get/Set methods
    // *-----------------------------
    // * - 
    // * - 
    // ******************************

    // ******************************
    // * GetMin
    // *-----------------------------
    /*! return the min index
    */ 
    // ******************************

    IntU32    GetMin() const
    {
        return m_Min;
    };

    // ******************************
    // * SetMin
    // *-----------------------------
    /*! set the min index
    */ 
    // ******************************

    void    SetMin(IntU32 min)
    {
        m_Min=min;
    };

    // ******************************
    // * GetMax
    // *-----------------------------
    /*! return the max index
    */ 
    // ******************************

    IntU32    GetMax() const
    {
        return m_Max;
    };

    // ******************************
    // * SetMax
    // *-----------------------------
    /*! set the max index
    */ 
    // ******************************

    void    SetMax(IntU32 max)
    {
        m_Max=max;
    };

    // ******************************
    // * GetInterval
    // *-----------------------------
    /*!  set min and max values in the parameters
    */ 
    // ******************************

    void    GetInterval(IntU32& min,IntU32& max) const
    {
        min=m_Min;
        max=m_Max;
    };

    // ******************************
    // * SetInterval
    // *-----------------------------
    /*!  set min and max values from the parameters
    */ 
    // ******************************

    void    SetInterval(IntU32 min,IntU32 max)
    {
        m_Min=min;
        m_Max=max;
    };


protected:

    IntU32    m_Min;
    IntU32    m_Max;
};

#endif //__AINDEXINTERVAL_H__


