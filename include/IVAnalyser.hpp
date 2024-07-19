#ifndef __IVAnalyser_HPP__
#define __IVAnalyser_HPP__

#include "TFile.h"
#include "TMultiGraph.h"
#include "TPaveText.h"
#include "TPaveLabel.h"
#include "TText.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TLine.h"
#include "TMath.h"
#include <string>
#include <new>
#include <memory>
#include <filesystem>
#include "TF1.h"
#include "Derivate.hpp"
#include "Plot.hpp"
#include "SavitzkyGolayFilter.hpp"

// #define NDEBUG

#ifdef NDEBUG
#define DBG_PRINT_PRETTY_FUNC std::cout << __PRETTY_FUNCTION__ << std::endl;
#else
#define DBG_PRINT_PRETTY_FUNC
#endif

/**
 *
 *  @class  IVAnalyser
 *  @author David Baranyai
 *	@date   2024.04
 *  @brief  Base class for IV analysation. Contains the shared parameters
 *          and provides an interface for currently used methods.
 *          (Relative Derivate and Third Derivate method)
 */

#ifdef __CINT__
typedef unsigned int uint8_t;
typedef unsigned int uint32_t;
typedef unsigned int uint64_t;
#endif

class IVAnalyser
{
private:
    /* data */
public:
    IVAnalyser();
    virtual ~IVAnalyser();

    double GetRawVbr();
    double GetCompVbr();
    double GetChi2();
    int Get_nderSmooths();
    int Get_derSmoothsWidth();
    int Get_nPreSmooths();
    int Get_preSmoothsWidth();

    void SetCompensationTemperature(double t);

    void SetSiPMTemperature(double temp);
    void SetSiPMTemperatureStdDev(double temp);
    void SetArrayPointers(double *voltages, double *currents, size_t nPoints);
    void SetSmoothingProperties(uint32_t nPreSmooths_, uint32_t preSmoothsWidth_, uint32_t nderSmooths_, uint32_t derSmoothsWidth_);

    //-------------------public virtual functions---------------------------------
    virtual void SaveAllPlot(std::string path, std::string prefix) = 0;
    virtual void SaveFitPlot(std::string path, std::string prefix) = 0;
    virtual bool RunAnalysis() = 0;

protected:
    //------------------------class protected functions---------------------------------
    double calcBreakdownTo25C_nearest(double BreakdownVoltage);
    double *runSGMultiple(int inArraySize, double *inArray, int filterWidth, int nruns);
    double GetGraphYmaxXpos(TGraph *gr);

    void OpenOutRootFile(std::string path, std::string prefix);
    void CloseOutRootFile();

    //------------------class protected variables---------------------------------
    // These variables are used in both currently used methods
    bool error = false;

    uint32_t nMeasurements = 0;
    double *voltageArray = NULL;
    double *currentArray = NULL;

    const double tempCoeff = 0.037; // temperature compensation coefficient V/Celsius
    double simpTemp = 30.0;
    double sipmTempStdDev = 30.0;

    double *fineCurr_sm = NULL;
    double *derivativeArray = NULL;
    double *derivativeArray_sm = NULL;

    double max_xpos = 0;

    // Drawings
    std::unique_ptr<Plot> plot = NULL;
    std::unique_ptr<TGraph> graph = NULL;
    std::unique_ptr<TF1> f1 = NULL;

    std::unique_ptr<TFile> outRootFile = NULL;

    // Temperature to compensate to
    double temperatureTo = 20.0;

    // Voltages
    double VBR_RAW = 0.0;
    double VBR_COMP = 0.0;
    double CHI2 = 0.0;

    int iv_plot_cut = 0;
    int first_der__plot_cut = 0;

    //----------------------------------------------------------------------------------
    // default settings, modifiable through constructor
    // number of runs of SG filter on original IV curve before ln(), if no filter is required set to 0
    int nPreSmooths = 0;
    // SG filter width 5,7 or 9
    int preSmoothsWidth = 9;

    // number of runs of SG filter after first derivative(), if no filter is required set to 0
    int nderSmooths = 0;
    // SG filter width 5,7 or 9
    int derSmoothsWidth = 9;
};

#endif /* End of __IVAnalyser_HPP__ */