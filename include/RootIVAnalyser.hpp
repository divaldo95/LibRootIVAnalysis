/*
Description:
    Define a shared library export for any platform.
 */
#ifdef _WIN32
# define DLL_EXPORT_TYP __declspec(dllexport)
#else
# define DLL_EXPORT_TYP
#endif

#pragma once

#include "RelativeDerivativeAnalysis.hpp"
#include <sys/stat.h>

//MacOS compatibility
#ifndef S_IRWXU
#define S_IRWXU 0000700  // RWX mask for owner
#endif

#ifndef S_IRWXG
#define S_IRWXG 0000070  // RWX mask for group
#endif

#ifndef S_IROTH
#define S_IROTH 0000004  // R for other
#endif

#ifndef S_IXOTH
#define S_IXOTH 0000001  // X for other
#endif

class RootIVAnalyser
{
private:
    double rawVbr = 1.35464;
    double compVbr = 876.24524;
    double chisquare = 3785.78612;

public:
    RootIVAnalyser();
    ~RootIVAnalyser();

    void AnaylseIV(double *voltages, double *currents, size_t dataPoints, double preTemp, double postTemp, int arrayID, int sipmID, unsigned long timestamp, std::string outBasePath);

    void GetResults(double *rVbr, double *cVbr, double *cs);
};