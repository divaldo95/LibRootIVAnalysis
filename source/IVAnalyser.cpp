#include "IVAnalyser.hpp"

IVAnalyser::IVAnalyser()
{
}

IVAnalyser::~IVAnalyser()
{
    if (fineCurr_sm)
        delete fineCurr_sm;
    if (derivativeArray)
        delete derivativeArray;
    if (derivativeArray_sm)
        delete derivativeArray_sm;
}


double IVAnalyser::GetRawVbr()
{
    return VBR_RAW;
}

double IVAnalyser::GetCompVbr()
{
    return VBR_COMP;
}

double IVAnalyser::GetChi2()
{
    return CHI2;
}

int IVAnalyser::Get_nderSmooths()
{
    return nderSmooths;
}

int IVAnalyser::Get_derSmoothsWidth()
{
    return derSmoothsWidth;
}

int IVAnalyser::Get_nPreSmooths()
{
    return nPreSmooths;
}

int IVAnalyser::Get_preSmoothsWidth()
{
    return preSmoothsWidth;
}

void IVAnalyser::SetSiPMTemperature(double temp)
{
    simpTemp = temp;
}

void IVAnalyser::SetSiPMTemperatureStdDev(double temp)
{
    sipmTempStdDev = temp;
}

void IVAnalyser::SetArrayPointers(double *voltages, double *currents, size_t nPoints)
{
    voltageArray = voltages;
    currentArray = currents;
    nMeasurements = nPoints;
}

void IVAnalyser::SetSmoothingProperties(uint32_t nPreSmooths_, uint32_t preSmoothsWidth_, uint32_t nderSmooths_, uint32_t derSmoothsWidth_)
{
    nPreSmooths = nPreSmooths_;
    preSmoothsWidth = preSmoothsWidth_;
    nderSmooths = nderSmooths_;
    derSmoothsWidth = derSmoothsWidth_;
}

double IVAnalyser::calcBreakdownTo25C_nearest(double BreakdownVoltage)
{
    const double operatingTemp = 25.0; // Celsius
    return BreakdownVoltage + ((operatingTemp - simpTemp) * tempCoeff);
}

// run savitzky-Golay filter 'nruns' time on the inputArray
//  filterWidth it must be 5,7 or 9
//  if nruns 0 this function return with the original array
double* IVAnalyser::runSGMultiple(int inArraySize, double *inArray, int filterWidth, int nruns)
{
    double *ret = inArray;
    for (int i = 0; i < nruns; i++)
    {
        SavitzkyGolayFilter smooth(inArraySize, ret, filterWidth); // filter width 5,7, or 9
        if (i != 0)
        {
            delete ret;
        }
        ret = smooth.GetSmoothed();
    }
    return ret;
}

double IVAnalyser::GetGraphYmaxXpos(TGraph *gr)
{
    return gr->GetX()[TMath::LocMax(gr->GetN(), gr->GetY())];
}

void IVAnalyser::OpenOutRootFile(std::string path, std::string prefix)
{
    CloseOutRootFile();
    std::string extension = ".root";
    std::filesystem::path filePath = std::filesystem::path(path) / std::filesystem::path(prefix + extension);
    outRootFile = std::make_unique<TFile>(filePath.c_str(), "UPDATE");
}

void IVAnalyser::CloseOutRootFile()
{
    if (outRootFile != NULL)
    {
        outRootFile -> Close();
        outRootFile = NULL;
        
    }
}
