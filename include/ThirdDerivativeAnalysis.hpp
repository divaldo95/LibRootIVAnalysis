#ifndef __ThirdDerivativeAnalysis_H__
#define __ThirdDerivativeAnalysis_H__

#include "IVAnalyser.hpp"

/**
 *
 *  @class  __ThirdDerivativeAnalysis
 *  @author Balazs Gyongyosi && David Baranyai && Tamas Majoros
 *	@date 2019.03 - 2019.05 | 2024.05
 *  @brief Running Third Derivative method to calculate breakdown voltage
 *         for Silicon Photomultipliers
 */

class ThirdDerivativeAnalysis : public IVAnalyser
{
public:
    // constructor with default parameters
    ThirdDerivativeAnalysis()
    {
        // Keep default values
        nPreSmooths = 1;
        preSmoothsWidth = 9;

        nderSmooths = 0;
        derSmoothsWidth = 9;
    }

    /*
    //----------------------------------------------------------------------------------
    // nPreSmooths   number of runs of SG filter on original IV curve before ln(), if no filter is required set to 0

    //preSmoothsWidth  SG filter width 5,7 or 9

    //nlnSmooths  number of runs of SG filter after ln(), if no filter is required set to 0

    //lnSmoothsWidth SG filter width 5,7 or 9

    //nderSmooths   number of runs of SG filter after first  derivative(), if no filter is required set to 0

    //derSmoothsWidth   SG filter width 5,7 or 9

    //nsecDerSmooths  number of runs of SG filter after sec derivative(), if no filter is required set to 0

    //secDerSmoothsWidth  SG filter width 5,7 or 9

    //nthirdDerSmooths  number of runs of SG filter after sec derivative(), if no filter is required set to 0

    //thirdDerSmoothsWidth  SG filter width 5,7 or 9


    //----------------------------------------------------------------------------------
    */

    void SetSmoothingProperties(uint32_t nPreSmooths_, uint32_t preSmoothsWidth_, uint32_t nderSmooths_, uint32_t derSmoothsWidth_,
                                uint32_t nsecDerSmooths_, uint32_t secDerSmoothsWidth_, uint32_t nthirdDerSmooths_, uint32_t thirdDerSmoothsWidth_)
    {
        // Set common properties using base function
        IVAnalyser::SetSmoothingProperties(nPreSmooths_, preSmoothsWidth_, nderSmooths_, derSmoothsWidth_);

        // Set custom properties
        nsecDerSmooths = nsecDerSmooths_;
        secDerSmoothsWidth = secDerSmoothsWidth_;
        nthirdDerSmooths = nthirdDerSmooths_;
        thirdDerSmoothsWidth = thirdDerSmoothsWidth_;
    }

    void SaveFitPlot(std::string path, std::string prefix)
    {
        if (!error && plot != NULL)
        {
            std::string RAW_Vbr_text = "RAW Vbr:" + std::to_string(VBR_RAW) + "V";
            std::string comp_Vbr_text = "Comp Vbr:" + std::to_string(VBR_COMP) + "V";
            std::string tempAvgLabelText = "avg temp: " + std::to_string(simpTemp) + "C";
			std::string tempStdDevLabelText = "temp StdDev: " + std::to_string(sipmTempStdDev);

            std::string fitV01LabelText = "fit V01: " + std::to_string(fit_V01);
            std::string fithLabelText = "fit h: " + std::to_string(fit_h);

            plot->addLabel(RAW_Vbr_text.c_str(), 0.92, 0.95);
            plot->addLabel(comp_Vbr_text.c_str(), 0.92, 0.98);
            plot->addLabel(tempAvgLabelText.c_str(), 0.7,0.98);
            plot->addLabel(tempStdDevLabelText.c_str(), 0.7,0.95);

            plot->addLabel(fitV01LabelText.c_str(), 0.2, 0.95);
            plot->addLabel(fithLabelText.c_str(), 0.2, 0.98);

            std::string filename = prefix + "_third_der";

            plot->save(path.c_str(), filename.c_str());
        }
    }

    void SaveAllPlot(std::string path, std::string prefix)
    {
        if (!error)
        {
            SaveFitPlot(path, prefix);

            Plot IV_plot(voltageArray, currentArray, nMeasurements, iv_plot_cut, "IV curve");
            IV_plot.addSmoothedGraph(voltageArray, fineCurr_sm);
            std::string filename = prefix + "_IV";
            IV_plot.save(path.c_str(), filename.c_str());

            Plot first_der_plot(voltageArray, derivativeArray, nMeasurements, first_der__plot_cut, "first der curve");
            first_der_plot.addSmoothedGraph(voltageArray, derivativeArray_sm);
            filename = prefix + "_first_der";
            first_der_plot.save(path.c_str(), filename.c_str());

            Plot sec_der_plot(voltageArray, secDerivativeArray, nMeasurements, sec_der__plot_cut, "sec der curve");
            sec_der_plot.addSmoothedGraph(voltageArray, secDerivativeArray_sm);
            filename = prefix + "_sec_der";
            sec_der_plot.save(path.c_str(), filename.c_str());
        }
    }

    double GetFith()
    {
        return fit_h;
    }

    double GetFitV01()
    {
        return fit_V01;
    }

    // destructor
    virtual ~ThirdDerivativeAnalysis()
    {
        if (secDerivativeArray)
            delete secDerivativeArray;
        if (secDerivativeArray_sm)
            delete secDerivativeArray_sm;
        if (thirdDerivativeArray)
            delete thirdDerivativeArray;
        if (thirdDerivativeArray_sm)
            delete thirdDerivativeArray_sm;
    }

    bool RunAnalysis()
    {
        if (nMeasurements == 0 || voltageArray == NULL || currentArray == NULL)
        {
            error = true;
            return false;
        }
        // calculate plot display cuts
        iv_plot_cut = nPreSmooths * ((preSmoothsWidth - 1) / 2);
        first_der__plot_cut = iv_plot_cut + nderSmooths * ((derSmoothsWidth - 1) / 2);
        sec_der__plot_cut = first_der__plot_cut + nsecDerSmooths * ((secDerSmoothsWidth - 1) / 2);
        third_der__plot_cut = sec_der__plot_cut + nthirdDerSmooths * ((thirdDerSmoothsWidth - 1) / 2);

        // apply SavitzkyGolay filter
        fineCurr_sm = runSGMultiple(nMeasurements, currentArray, preSmoothsWidth, nPreSmooths);

        // calculate first derivative
        Derivate Derivate_(nMeasurements, fineCurr_sm, voltageArray);
        derivativeArray = Derivate_.GetDerivative();

        // apply SavitzkyGolay filter after first derivative
        derivativeArray_sm = runSGMultiple(nMeasurements, derivativeArray, derSmoothsWidth, nderSmooths);

        // calculate sec derivative
        Derivate secDerivate_(nMeasurements, derivativeArray, voltageArray);
        secDerivativeArray = secDerivate_.GetDerivative();

        // apply SavitzkyGolay filter after sec derivative
        secDerivativeArray_sm = runSGMultiple(nMeasurements, secDerivativeArray, secDerSmoothsWidth, nsecDerSmooths);

        // calculate third derivative
        Derivate thirdDerivate_(nMeasurements, secDerivativeArray_sm, voltageArray);
        thirdDerivativeArray = thirdDerivate_.GetDerivative();

        // apply SavitzkyGolay filter after third derivative
        thirdDerivativeArray_sm = runSGMultiple(nMeasurements, thirdDerivativeArray, secDerSmoothsWidth, nsecDerSmooths);

        plot = std::make_unique<Plot>(voltageArray, thirdDerivativeArray, nMeasurements, third_der__plot_cut, "third derivative");
        if (plot == NULL)
        {
            error = true;
            return false;
        }

        plot->addSmoothedGraph(voltageArray, thirdDerivativeArray_sm);

        TGraph *thirdDerGraphSm = plot->GetSmoothedTgraph();
        if (thirdDerGraphSm == NULL)
        {
            error = true;
            return false;
        }

        max_xpos = GetGraphYmaxXpos(thirdDerGraphSm);

        // printf("max XPos:%lf\n", max_xpos);

        double fit_start = voltageArray[third_der__plot_cut + 1];
        double fit_end = voltageArray[nMeasurements - third_der__plot_cut - 2];

        TF1 *f1 = new TF1("f1", "[0] * (2-([1]*(x-[3]))/([2]^2)) * exp(-1*(((x-[3])*(x-[3]))/(2*([2]^2))))", fit_start, fit_end); //  |0 A| |1 h| |2 sigma| |3 V01|  65-68,3 fit width
        f1->SetParameters(0.0001, 0.8, 0.17, max_xpos);                                                                           // initial parameters
        thirdDerGraphSm->Fit("f1", "RM");
        fit_h = thirdDerGraphSm->GetFunction("f1")->GetParameter(1);
        fit_V01 = thirdDerGraphSm->GetFunction("f1")->GetParameter(3);

        VBR_RAW = fit_V01 - fit_h;

        VBR_COMP = calcBreakdownTo25C_nearest(VBR_RAW);

        return true;
    }

private:
    //------------------class private functions---------------------------------


    //------------------class private variables---------------------------------
    double *secDerivativeArray = NULL;
    double *secDerivativeArray_sm = NULL;
    double *thirdDerivativeArray = NULL;
    double *thirdDerivativeArray_sm = NULL;

    double fit_h = 0;
    double fit_V01 = 0;

    int sec_der__plot_cut = 0;
    int third_der__plot_cut = 0;

    //----------------------------------------------------------------------------------

    // number of runs of SG filter after sec derivative(), if no filter is required set to 0
    int nsecDerSmooths = 2;
    // SG filter width 5,7 or 9
    int secDerSmoothsWidth = 9;

    // number of runs of SG filter after third der if no filter is required set to 0
    int nthirdDerSmooths = 3;
    // SG filter width 5,7 or 9
    int thirdDerSmoothsWidth = 9;
    //----------------------------------------------------------------------------------
};

#endif /* __ThirdDerivativeAnalysis_H__ */
