#include "RootIVAnalyser.hpp"

RootIVAnalyser::RootIVAnalyser()
{
    DBG_PRINT_PRETTY_FUNC;
}

RootIVAnalyser::~RootIVAnalyser()
{
    DBG_PRINT_PRETTY_FUNC;
}

bool RootIVAnalyser::AnalyseIV(SiPMData data, AnalysisTypes method, double temperatureTo, std::string outBasePath, std::string filePrefix, bool savePlots)
{
    // std::string output_dir_plots = outBasePath + "IV/plots";
    // mkdir(output_dir_plots.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    DBG_PRINT_PRETTY_FUNC;
    std::unique_ptr<IVAnalyser> analysis;
    switch (method)
    {
    case AnalysisTypes::RelativeDerivativeMethod:
        analysis = std::make_unique<RelativeDerivativeAnalysis>();
        dynamic_cast<RelativeDerivativeAnalysis *>(analysis.get())->SetSmoothingProperties(props.nPreSmooth, props.preSmoothWidth, props.nlnSmooth, props.lnSmoothWidth, props.nDerivativeSmooth, props.derivativeSmoothWidth, props.fitWidth / 1000.0);
        break;
    case AnalysisTypes::ThirdDerivativeMethod:
        analysis = std::make_unique<ThirdDerivativeAnalysis>();
        break;
    default:
        std::cout << "Unknown analysis type" << std::endl;
        return false;
    }
    if (analysis == NULL)
    {
        std::cout << "Can not create analysis" << std::endl;
        return false;
    }

    analysis->SetCompensationTemperature(temperatureTo);
    analysis->SetArrayPointers(data.voltages, data.currents, data.dataPoints);
    analysis->SetSiPMTemperature(data.postTemp); // might change it later

    if (!analysis->RunAnalysis())
    {
        std::cout << "Error while running analysis" << std::endl;
        return false;
    }
    rawVbr = analysis->GetRawVbr();
    compVbr = analysis->GetCompVbr();
    chisquare = analysis->GetChi2();
    if (savePlots)
    {
        analysis->SaveAllPlot(outBasePath, filePrefix);
    }
    return true;
}

void RootIVAnalyser::GetResults(double *rVbr, double *cVbr, double *cs)
{
    DBG_PRINT_PRETTY_FUNC;
    *rVbr = rawVbr;
    *cVbr = compVbr;
    *cs = chisquare;
}

void RootIVAnalyser::SetProperties(AnalysisProperties props)
{
    DBG_PRINT_PRETTY_FUNC;
    this->props = props;
}

extern "C"
{
    DLL_EXPORT_TYP RootIVAnalyser *RIVA_Class_Create()
    {
        return new RootIVAnalyser();
    }

    DLL_EXPORT_TYP void RIVA_Class_Delete(RootIVAnalyser *ivAnalyser)
    {
        if (ivAnalyser)
        {
            delete ivAnalyser;
            ivAnalyser = nullptr;
        }
    }

    DLL_EXPORT_TYP bool RIVA_Class_AnalyseIV(RootIVAnalyser *ivAnalyser, SiPMData data, AnalysisTypes method, double temperatureToCompensate, bool savePlots, char *outBasePath, char *filePrefix)
    {
        return ivAnalyser->AnalyseIV(data, method, temperatureToCompensate, outBasePath, filePrefix, savePlots);
    }

    DLL_EXPORT_TYP void RIVA_Class_SetProperties(RootIVAnalyser *ivAnalyser, AnalysisProperties props)
    {
        ivAnalyser->SetProperties(props);
    }

    DLL_EXPORT_TYP void RIVA_Class_GetResults(RootIVAnalyser *ivAnalyser, double *rawVbr, double *compVbr, double *cs)
    {
        ivAnalyser->GetResults(rawVbr, compVbr, cs);
    }
}
