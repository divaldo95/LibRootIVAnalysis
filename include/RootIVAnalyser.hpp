/*
Description:
    Define a shared library export for any platform.
 */
#ifdef _WIN32
#define DLL_EXPORT_TYP __declspec(dllexport)
#else
#define DLL_EXPORT_TYP
#endif

#pragma once

#include "RelativeDerivativeAnalysis.hpp"
#include "ThirdDerivativeAnalysis.hpp"
#include "SiPMData.hpp"
#include "AnalysisTypes.hpp"
#include "AnalysisProperties.hpp"
#include <sys/stat.h>

// MacOS compatibility
#ifndef S_IRWXU
#define S_IRWXU 0000700 // RWX mask for owner
#endif

#ifndef S_IRWXG
#define S_IRWXG 0000070 // RWX mask for group
#endif

#ifndef S_IROTH
#define S_IROTH 0000004 // R for other
#endif

#ifndef S_IXOTH
#define S_IXOTH 0000001 // X for other
#endif

class RootIVAnalyser
{
private:
    double rawVbr = 0.0;
    double compVbr = 0.0;
    double chisquare = 0.0;

    AnalysisProperties props;

public:
    RootIVAnalyser();
    ~RootIVAnalyser();

    bool AnalyseIV(SiPMData data, AnalysisTypes method, double temperatureTo, std::string outBasePath, std::string filePrefix, bool savePlots);

    void GetResults(double *rVbr, double *cVbr, double *cs);

    void SetProperties(AnalysisProperties props);
};