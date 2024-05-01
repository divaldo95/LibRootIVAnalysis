#include "RootIVAnalyser.hpp"

RootIVAnalyser::RootIVAnalyser()
{
}

RootIVAnalyser::~RootIVAnalyser()
{
}

void RootIVAnalyser::AnalyseIV(double *voltages, double *currents, size_t dataPoints, double preTemp, double postTemp, int arrayID, int sipmID, unsigned long timestamp, std::string outBasePath)
{
    std::string output_dir_result = outBasePath + "IV/result";
    mkdir(output_dir_result.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    std::string output_dir_plots = outBasePath + "IV/plots";
    mkdir(output_dir_plots.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    //---------------------------------------------------------------------------

    //--------------------------create result file-------------------------------------------
    std::string res_file_path = output_dir_result + "/IV_result.txt";

    FILE *resultFile = fopen(res_file_path.c_str(), "a");

    if (resultFile == NULL)
    {
        std::cout << "Can not open result file: " << res_file_path << std::endl;
    }

    for (int pS = 1; pS < 2; pS++)
    {
        for (int pSW = 5; pSW < 6; pSW = pSW + 2)
        {
            for (int lnS = 1; lnS < 2; lnS++)
            {
                for (int lnSW = 5; lnSW < 6; lnSW = lnSW + 2)
                {
                    for (int dS = 1; dS < 2; dS++)
                    {
                        for (int dSW = 5; dSW < 6; dSW = dSW + 2)
                        {
                            for (int fW = 100; fW < 105; fW = fW + 5)
                            {

                                //  std::cout<<"ABCDEF "<<SIPM_ID<<" "<<pS<<" "<<pSW<<" "<<lnS<<" "<<lnSW<<" "<<dS<<" "<<dSW<<" "<<fW/100.0<<std::endl;

                                RelativeDerivativeAnalysis *relDerivativeAnalysis = new RelativeDerivativeAnalysis(voltages, currents, dataPoints, pS, pSW, lnS, lnSW, dS, dSW, fW / 1000.0);
                                ThirdDerivativeAnalysis *thirdDerivativeAnalysis = new ThirdDerivativeAnalysis(voltages, currents, dataPoints);

                                //!!!!!!! SWITCH OFF SAVE !!!!!!!!!!!!!!!!!!!!!!
                                // relDerivativeAnalysis->SaveAllPlot(output_dir_plots.c_str());

                                if (pS == 0 && lnS == 0 && dS == 0 && pSW == 5 && lnSW == 5 && dSW == 5)
                                    relDerivativeAnalysis->SaveAllPlot(output_dir_plots.c_str());

                                thirdDerivativeAnalysis->SaveAllPlot(output_dir_plots.c_str());
                                if(resultFile != NULL)
                                {
                                    //Relative derivative
                                    fprintf(resultFile, "%d ", arrayID);
                                    fprintf(resultFile, "%lu ", timestamp); //" PRIu64 "
                                    fprintf(resultFile, "%zu ", dataPoints);

                                    fprintf(resultFile, "%.3lf ", 1.0); // IV analysis method 1 relative derivative

                                    // IV analysis parameters
                                    fprintf(resultFile, "%.3lf ", (double)relDerivativeAnalysis->Get_nPreSmooths());
                                    fprintf(resultFile, "%.3lf ", (double)relDerivativeAnalysis->Get_preSmoothsWidth());
                                    fprintf(resultFile, "%.3lf ", (double)relDerivativeAnalysis->Get_nlnSmooths());
                                    fprintf(resultFile, "%.3lf ", (double)relDerivativeAnalysis->Get_lnSmoothsWidth());
                                    fprintf(resultFile, "%.3lf ", (double)relDerivativeAnalysis->Get_nderSmooths());
                                    fprintf(resultFile, "%.3lf ", (double)relDerivativeAnalysis->Get_derSmoothsWidth());

                                    fprintf(resultFile, "%d ", fW);

                                    fprintf(resultFile, "%d ", sipmID);

                                    fprintf(resultFile, "%2.3lf ", preTemp);

                                    fprintf(resultFile, "%2.3lf ", preTemp); // temp std dev, fix it later

                                    fprintf(resultFile, "%2.4lf ", relDerivativeAnalysis->GetRawVbr());  // relDerivative RAW Vbr:
                                    fprintf(resultFile, "%2.4lf ", relDerivativeAnalysis->GetCompVbr()); // relDerivative compensated Vbr:

                                    fprintf(resultFile, "%2.4lf ", relDerivativeAnalysis->GetChi2()); // Chi2

                                    fprintf(resultFile, "\n");


                                    //Third derivative
                                    fprintf(resultFile, "%d ", arrayID);
                                    fprintf(resultFile, "%lu ", timestamp); //" PRIu64 "
                                    fprintf(resultFile, "%zu ", dataPoints);

                                    fprintf(resultFile, "%.3lf ", 2.0); // IV analysis method 1 relative derivative

                                    // IV analysis parameters
                                    /*fprintf(resultFile, "%.3lf ", (double)thirdDerivativeAnalysis->Get_nPreSmooths());
                                    fprintf(resultFile, "%.3lf ", (double)thirdDerivativeAnalysis->Get_preSmoothsWidth());
                                    fprintf(resultFile, "%.3lf ", (double)thirdDerivativeAnalysis->Get_nlnSmooths());
                                    fprintf(resultFile, "%.3lf ", (double)thirdDerivativeAnalysis->Get_lnSmoothsWidth());
                                    fprintf(resultFile, "%.3lf ", (double)thirdDerivativeAnalysis->Get_nderSmooths());
                                    fprintf(resultFile, "%.3lf ", (double)thirdDerivativeAnalysis->Get_derSmoothsWidth());

                                    fprintf(resultFile, "%d ", fW);*/

                                    fprintf(resultFile, "%d ", sipmID);

                                    fprintf(resultFile, "%2.3lf ", preTemp);

                                    fprintf(resultFile, "%2.3lf ", preTemp); // temp std dev, fix it later

                                    fprintf(resultFile, "%2.4lf ", thirdDerivativeAnalysis->GetRawVbr());  // relDerivative RAW Vbr:
                                    fprintf(resultFile, "%2.4lf ", thirdDerivativeAnalysis->GetCompVbr()); // relDerivative compensated Vbr:

                                    //fprintf(resultFile, "%2.4lf ", relDerivativeAnalysis->GetChi2()); // Chi2

                                    fprintf(resultFile, "\n");
                                }

                                rawVbr = relDerivativeAnalysis->GetRawVbr();
                                compVbr = relDerivativeAnalysis->GetCompVbr();
                                chisquare = relDerivativeAnalysis->GetChi2();

                                delete relDerivativeAnalysis;
                                delete thirdDerivativeAnalysis;
                            }
                        }
                    }
                }
            }
        }
    }
    if (resultFile != NULL)
        fclose(resultFile);
}

void RootIVAnalyser::GetResults(double *rVbr, double *cVbr, double *cs)
{
    *rVbr = rawVbr;
    *cVbr = compVbr;
    *cs = chisquare;
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

    DLL_EXPORT_TYP void RIVA_Class_AnalyseIV(RootIVAnalyser *ivAnalyser, double *voltages, double *currents, size_t dataPoints, double preTemp, double postTemp, int arrayID, int sipmID, unsigned long timestamp, char *outBasePath)
    {
        ivAnalyser->AnalyseIV(voltages, currents, dataPoints, preTemp, postTemp, arrayID, sipmID, timestamp, outBasePath);
    }

    DLL_EXPORT_TYP void RIVA_Class_GetResults(RootIVAnalyser *ivAnalyser, double *rawVbr, double *compVbr, double *cs)
    {
        ivAnalyser->GetResults(rawVbr, compVbr, cs);
    }
}
