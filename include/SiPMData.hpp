#ifndef __SIPMDATA_HPP__
#define __SIPMDATA_HPP__

#include <stddef.h>

/**
 *
 *  @class  SiPMData
 *  @author David Baranyai
 *	@date   2024.05
 *  @brief  This struct holds all the necessary information for an analysis
 */

struct SiPMData 
{
    double *voltages;
    double *currents;
    size_t dataPoints;
    double preTemp;
    double postTemp;
    unsigned long timestamp;
};

#endif /* End of __SIPMDATA_HPP__ */