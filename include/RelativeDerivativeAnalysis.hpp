#ifndef __RelDerivativeAnalysis_H__
#define __RelDerivativeAnalysis_H__

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
#include "TF1.h"
#include "SavitzkyGolayFilter.hpp"
#include "Derivate.hpp"
#include "Plot.hpp"

/**
*
*  @class  RelativeDerivativeAnalysis
*  @author Balazs Gyongyosi && Tamas Majoros && David Baranyai
*	 @date 2019.03 - 2019.05 | 2024.04
*  @brief
*/

#ifdef __CINT__
typedef unsigned int uint8_t;
typedef unsigned int uint32_t;
typedef unsigned int uint64_t;
#endif

class RelativeDerivativeAnalysis 
{

public:

  //constructor with default parameters
  RelativeDerivativeAnalysis(double *voltages, double *currents, size_t nPoints)
  {
    volatgeArray = voltages;
    currentArray = currents;
    nmeasurements = nPoints;
    init_function();
  }

  /*
  //----------------------------------------------------------------------------------
  // nPreSmooths   number of runs of SG filter on original IV curve before ln(), if no filter is required set to 0

  //preSmoothsWidth  SG filter width 5,7 or 9

  //nlnSmooths  number of runs of SG filter after ln(), if no filter is required set to 0

  //lnSmoothsWidthSG filter width 5,7 or 9

  //nderSmooths   number of runs of SG filter after derivative(), if no filter is required set to 0

  //derSmoothsWidth   SG filter width 5,7 or 9

  //----------------------------------------------------------------------------------
  */
  //constructor
  RelativeDerivativeAnalysis(double *voltages, double *currents, size_t nPoints, uint32_t nPreSmooths_, uint32_t preSmoothsWidth_, uint32_t nlnSmooths_, uint32_t lnSmoothsWidth_, uint32_t nderSmooths_, uint32_t derSmoothsWidth_, double fit_width_   )
  {
    volatgeArray = voltages;
    currentArray = currents;
    nmeasurements = nPoints;

    nPreSmooths = nPreSmooths_;
    preSmoothsWidth = preSmoothsWidth_;
    nlnSmooths = nlnSmooths_;
    lnSmoothsWidth = lnSmoothsWidth_;
    nderSmooths = nderSmooths_;
    derSmoothsWidth = derSmoothsWidth_;
    fit_width = fit_width_;

    init_function();
  }



  void SaveFitPlot(const char* path)
  {
    if(!error && relDerPlot != NULL)
    {
      std::string RAW_Vbr_text = "RAW Vbr:" + std::to_string(VBR_RAW) +"V";
      std::string comp_Vbr_text = "Comp Vbr:" +  std::to_string(VBR_COMP) +"V";
      std::string tempAvgLabelText = "avg temp: 25C"; //std::to_string(IV_file->GetSIPMTemp()) +"C";
      std::string tempStdDevLabelText = "temp StdDev: 25C"; //std::to_string(IV_file->GetSIPMStdDev());

      relDerPlot->addLabel(RAW_Vbr_text.c_str(), 0.92,0.95);
      relDerPlot->addLabel(comp_Vbr_text.c_str(), 0.92,0.98);
      relDerPlot->addLabel(tempAvgLabelText.c_str(), 0.7,0.98);
      relDerPlot->addLabel(tempStdDevLabelText.c_str(), 0.7,0.95);

      std::string filename = std::to_string(position) +  "_rel_der_fit" + std::to_string(nPreSmooths) + std::to_string(preSmoothsWidth) + std::to_string(nlnSmooths) + std::to_string(lnSmoothsWidth) + std::to_string(nderSmooths) + std::to_string(derSmoothsWidth) + std::to_string((int)(fit_width*1000)) ;

      relDerPlot->save(path, filename.c_str());
    }
  }

  void SaveAllPlot(const char* path)
  {
    if(!error )
    {
      SaveFitPlot(path);

      Plot IV_plot(volatgeArray, currentArray, nmeasurements, iv_plot_cut, "IV curve");
      IV_plot.addSmoothedGraph(volatgeArray, fineCurr_sm);
      std::string filename = std::to_string(position) +  "_IV" + std::to_string(nPreSmooths) + std::to_string(preSmoothsWidth)  + std::to_string(nlnSmooths) + std::to_string(lnSmoothsWidth) + std::to_string(nderSmooths) + std::to_string(derSmoothsWidth) + std::to_string((int)(fit_width*1000)) ;
      IV_plot.save(path, filename.c_str());

      Plot ln_IV_plot(volatgeArray, lnArray, nmeasurements, ln_iv_plot_cut, "ln IV curve");
      ln_IV_plot.addSmoothedGraph(volatgeArray, lnArray_sm);
      filename = std::to_string(position) +  "_ln_IV" + std::to_string(nPreSmooths) + std::to_string(preSmoothsWidth)  + std::to_string(nlnSmooths) + std::to_string(lnSmoothsWidth) + std::to_string(nderSmooths) + std::to_string(derSmoothsWidth) + std::to_string((int)(fit_width*1000))  ;
      ln_IV_plot.save(path, filename.c_str());
    }
  }


  double GetRawVbr()
  {
    return VBR_RAW;
  }

  double GetCompVbr()
  {
    return VBR_COMP;
  }

  double GetChi2()
  {
    return chi2;
  }

  //get parameters functions---------------------------------
  int Get_nPreSmooths()
  {
    return nPreSmooths;
  }

  int Get_preSmoothsWidth()
  {
    return preSmoothsWidth;
  }

  int Get_nlnSmooths()
  {
    return nlnSmooths;
  }

  int Get_lnSmoothsWidth()
  {
    return lnSmoothsWidth;
  }

  int Get_nderSmooths()
  {
    return nderSmooths;
  }

  int Get_derSmoothsWidth()
  {
    return derSmoothsWidth;
  }

  double Get_fit_width()
  {
    return fit_width;
  }



  // destructor
  virtual ~RelativeDerivativeAnalysis()
  {
    //printf("~RelDerivativeAnalysis()\n");

    delete volatgeArray;
    delete currentArray;
    delete fineCurr_sm;
    delete lnArray_sm;
    delete derivativeArray;
    delete derivativeArray_sm;
    delete relDerPlot;
    delete f1;
  }



private:

  //------------------class private functions---------------------------------
  void init_function()
  {
      if(nmeasurements != 0 && volatgeArray != NULL && currentArray != NULL)
      {
        lnArray = new (std::nothrow) double[nmeasurements];
        if(lnArray != NULL)
        {
          if(RunCalculations())
          {
            error = true;
          }
        }
        else
        {
          error = true;
        }
      }
      else
      {
        error = true;
      }
  }

  double calcBreakdownTo25C_nearest(double BreakdownVoltage)
    {
        const double operatingTemp = 25.0; // Celsius
        return BreakdownVoltage + ((operatingTemp - simpTemp) * tempCoeff);
    }

  //run savitzky-Golay filter 'nruns' time on the inputArray
  // filterWidth it must be 5,7 or 9
  // if nruns 0 this function return with the original array
  double *runSGMultiple(int inArraySize, double* inArray,int filterWidth,  int nruns)
  {
    double *ret = inArray;
    for(int i=0;i<nruns;i++)
    {
      SavitzkyGolayFilter smooth(inArraySize, ret, filterWidth );  //filter width 5,7, or 9
      if(i!=0){
        delete ret;
      }
      ret = smooth.GetSmoothed();
    }
    return ret;
  }

  double GetGraphYmaxXpos(TGraph *gr){
    return gr->GetX()[ TMath::LocMax(gr->GetN(),gr->GetY()) ];
  }

  bool RunCalculations()
  {
    if(!error)
    {
      //calculate plot display cuts
      iv_plot_cut = nPreSmooths * ((preSmoothsWidth - 1)/2);
      ln_iv_plot_cut = iv_plot_cut + nlnSmooths * ((lnSmoothsWidth - 1)/2);
      der_plot_cut = ln_iv_plot_cut + nderSmooths * ((derSmoothsWidth - 1)/2)+5;

      //apply SavitzkyGolay filter
      fineCurr_sm =  runSGMultiple(nmeasurements, currentArray, preSmoothsWidth,  nPreSmooths);

      //calculate ln()
      for(int i=0;i<nmeasurements;i++)
      {
        lnArray[i] = TMath::Log(fineCurr_sm[i]);
      }

      //apply SavitzkyGolay filter after ln()
      lnArray_sm = runSGMultiple(nmeasurements, lnArray, lnSmoothsWidth,  nlnSmooths);

      //calculate derivative
      Derivate Derivate_(nmeasurements, lnArray_sm, volatgeArray);
      derivativeArray = Derivate_.GetDerivative();

      //apply SavitzkyGolay filter after derivative
      derivativeArray_sm = runSGMultiple(nmeasurements, derivativeArray, derSmoothsWidth,  nderSmooths);
      /*
      for(int i=0;i<nmeasurements;i++)
      {
      printf("%d %lf %lf\n",i,volatgeArray[i], currentArray[i]);
    }
    */
    relDerPlot = new Plot(volatgeArray, derivativeArray, nmeasurements, der_plot_cut , "relative derivative");
    if(relDerPlot == NULL)
    return true;

    relDerPlot->addSmoothedGraph(volatgeArray, derivativeArray_sm);

    TGraph *relDerGraphSm =  relDerPlot->GetSmoothedTgraph();
    if(relDerGraphSm == NULL)
    return true;

    max_xpos = GetGraphYmaxXpos(relDerGraphSm);

    printf("max XPos:%lf\n", max_xpos);

    f1 = new TF1("f1","gaus",max_xpos-fit_width,max_xpos+fit_width);

    relDerGraphSm->Fit("f1","R");

    VBR_RAW = relDerGraphSm->GetFunction("f1")->GetParameter(1);

    VBR_COMP = calcBreakdownTo25C_nearest(VBR_RAW);

    chi2 = f1->GetChisquare();


    return false;
  }
  else
  {
    return true;
  }

}


//------------------class private variables---------------------------------
bool error = false;

uint8_t position = 0;
uint32_t nmeasurements = 0;
double* volatgeArray = NULL;
double* currentArray = NULL;

const double tempCoeff  = 0.037;  //temperature compensation coefficient V/Celsius
double simpTemp = 30.0;

double *fineCurr_sm =  NULL;
double *lnArray = NULL;
double *lnArray_sm =  NULL;
double *derivativeArray =  NULL;
double *derivativeArray_sm =  NULL;

double max_xpos = 0;

Plot *relDerPlot = NULL;


TGraph *relDerGraphSm = NULL;

TF1 *f1 = NULL;

double VBR_RAW = 0;
double VBR_COMP = 0;

double chi2 = 0;


int iv_plot_cut = 0;
int ln_iv_plot_cut = 0;
int first_der__plot_cut = 0;
int der_plot_cut = 0;



//----------------------------------------------------------------------------------
//default settings, modifiable through constructor
//number of runs of SG filter on original IV curve before ln(), if no filter is required set to 0
int nPreSmooths = 0;
//SG filter width 5,7 or 9
int   preSmoothsWidth = 5;

//number of runs of SG filter after ln(), if no filter is required set to 0
int nlnSmooths = 0;
//SG filter width 5,7 or 9
int lnSmoothsWidth = 7;

//number of runs of SG filter after derivative(), if no filter is required set to 0
int nderSmooths = 0;
//SG filter width 5,7 or 9
int derSmoothsWidth = 5;

double fit_width = 0.15;
//----------------------------------------------------------------------------------

};

#endif /* __RelDerivativeAnalysis_H__ */
