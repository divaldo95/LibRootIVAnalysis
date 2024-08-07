#ifndef __ANALYSIS_PROPERTIES_HPP__
#define __ANALYSIS_PROPERTIES_HPP__

#include <stddef.h>

/**
 *
 *  @class  AnalysisProperties
 *  @author David Baranyai
 *	@date   2024.05
 *  @brief  This struct holds the properties of an analysis
 *          like fit width, smoothing parameters, etc
 */

struct AnalysisProperties
{
    int nPreSmooth = 0;
    int preSmoothWidth = 5;
    int nlnSmooth = 0;
    int lnSmoothWidth = 5;
    int nDerivativeSmooth = 0;
    int derivativeSmoothWidth = 5;
    int fitWidth = 100;

    // set default properties
    AnalysisProperties()
    {
        nPreSmooth = 0;
        preSmoothWidth = 5;
        nlnSmooth = 0;
        lnSmoothWidth = 5;
        nDerivativeSmooth = 0;
        derivativeSmoothWidth = 5;
        fitWidth = 100;
    }

    std::ostream &operator<<(std::ostream &out)
    {
        out << "nPreSmooth: " << this->nPreSmooth << std::endl
            << "preSmoothWidth: " << this->preSmoothWidth << std::endl
            << "nlnSmooth: " << this->nlnSmooth << std::endl
            << "lnSmoothWidth: " << this->lnSmoothWidth << std::endl
            << "nDerivativeSmooth: " << this->nDerivativeSmooth << std::endl
            << "derivativeSmoothWidth: " << this->derivativeSmoothWidth << std::endl
            << "fitWidth: " << this->fitWidth << std::endl;
        return out;
    }
};

#endif /* End of __ANALYSIS_PROPERTIES_HPP__ */