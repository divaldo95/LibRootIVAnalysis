#include "IVAnalyser.hpp"

IVAnalyser::IVAnalyser()
{
}

IVAnalyser::~IVAnalyser()
{
    DBG_PRINT_PRETTY_FUNC;
#ifdef NDEBUG
    std::cout << "currentArray: " << currentArray << std::endl;
    std::cout << "fineCurr_sm: " << fineCurr_sm << std::endl;
    std::cout << "derivativeArray: " << derivativeArray << std::endl;
    std::cout << "derivativeArray_sm: " << derivativeArray_sm << std::endl;
#endif
    // if smooths are turned of, then these will be point to the same memory
    if (derivativeArray != NULL && derivativeArray_sm != NULL && derivativeArray == derivativeArray_sm)
        delete derivativeArray;
    else if (derivativeArray)
        delete derivativeArray;
    else if (derivativeArray_sm)
        delete derivativeArray_sm;

    // currentArray will be freed by caller, if these are matches, do nothing
    if (currentArray != NULL && fineCurr_sm != NULL && currentArray != fineCurr_sm)
        delete fineCurr_sm;
}

double IVAnalyser::GetRawVbr()
{
    DBG_PRINT_PRETTY_FUNC;
    return VBR_RAW;
}

double IVAnalyser::GetCompVbr()
{
    DBG_PRINT_PRETTY_FUNC;
    return VBR_COMP;
}

double IVAnalyser::GetChi2()
{
    DBG_PRINT_PRETTY_FUNC;
    return CHI2;
}

int IVAnalyser::Get_nderSmooths()
{
    DBG_PRINT_PRETTY_FUNC;
    return nderSmooths;
}

int IVAnalyser::Get_derSmoothsWidth()
{
    DBG_PRINT_PRETTY_FUNC;
    return derSmoothsWidth;
}

int IVAnalyser::Get_nPreSmooths()
{
    DBG_PRINT_PRETTY_FUNC;
    return nPreSmooths;
}

int IVAnalyser::Get_preSmoothsWidth()
{
    DBG_PRINT_PRETTY_FUNC;
    return preSmoothsWidth;
}

void IVAnalyser::SetCompensationTemperature(double t)
{
    DBG_PRINT_PRETTY_FUNC;
    temperatureTo = t;
}

void IVAnalyser::SetSiPMTemperature(double temp)
{
    DBG_PRINT_PRETTY_FUNC;
    simpTemp = temp;
}

void IVAnalyser::SetSiPMTemperatureStdDev(double temp)
{
    DBG_PRINT_PRETTY_FUNC;
    sipmTempStdDev = temp;
}

void IVAnalyser::SetArrayPointers(double *voltages, double *currents, size_t nPoints)
{
    DBG_PRINT_PRETTY_FUNC;
    voltageArray = voltages;
    currentArray = currents;
    nMeasurements = nPoints;
}

void IVAnalyser::SetSmoothingProperties(uint32_t nPreSmooths_, uint32_t preSmoothsWidth_, uint32_t nderSmooths_, uint32_t derSmoothsWidth_)
{
    DBG_PRINT_PRETTY_FUNC;
    nPreSmooths = nPreSmooths_;
    preSmoothsWidth = preSmoothsWidth_;
    nderSmooths = nderSmooths_;
    derSmoothsWidth = derSmoothsWidth_;
}

double IVAnalyser::calcBreakdownTo25C_nearest(double BreakdownVoltage)
{
    DBG_PRINT_PRETTY_FUNC;
    return BreakdownVoltage + ((temperatureTo - simpTemp) * tempCoeff);
}

// run savitzky-Golay filter 'nruns' time on the inputArray
//  filterWidth it must be 5,7 or 9
//  if nruns 0 this function return with the original array
double *IVAnalyser::runSGMultiple(int inArraySize, double *inArray, int filterWidth, int nruns)
{
    DBG_PRINT_PRETTY_FUNC;
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
    DBG_PRINT_PRETTY_FUNC;
    return gr->GetX()[TMath::LocMax(gr->GetN(), gr->GetY())];
}

void IVAnalyser::OpenOutRootFile(std::string path, std::string prefix)
{
    DBG_PRINT_PRETTY_FUNC;
    CloseOutRootFile();
    std::string extension = ".root";
    std::filesystem::path dirPath(path);
    if (!std::filesystem::exists(dirPath))
    {
        std::filesystem::create_directories(dirPath);
    }
    std::filesystem::path filePath = std::filesystem::path(path) / std::filesystem::path(prefix + extension);
    outRootFile = std::make_unique<TFile>(filePath.string().c_str(), "UPDATE");
}

void IVAnalyser::CloseOutRootFile()
{
    DBG_PRINT_PRETTY_FUNC;
    if (outRootFile != NULL)
    {
        outRootFile->Close();
        outRootFile = NULL;
    }
}
