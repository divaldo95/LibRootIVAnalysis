#ifndef __RelDerivativeAnalysis_H__
#define __RelDerivativeAnalysis_H__

#include "IVAnalyser.hpp"

/**
 *
 *  @class  RelativeDerivativeAnalysis
 *  @author Balazs Gyongyosi && Tamas Majoros && David Baranyai
 *	@date 2019.03 - 2019.05 | 2024.04
 *  @brief Running Relative Derivative method to calculate breakdown voltage
 *         for Silicon Photomultipliers
 */

class RelativeDerivativeAnalysis : public IVAnalyser
{
public:
	RelativeDerivativeAnalysis()
	{
		DBG_PRINT_PRETTY_FUNC;
		nPreSmooths = 0;
		// SG filter width 5,7 or 9
		preSmoothsWidth = 5;

		// number of runs of SG filter after derivative(), if no filter is required set to 0
		nderSmooths = 0;
		// SG filter width 5,7 or 9
		derSmoothsWidth = 5;
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

	void SetSmoothingProperties(uint32_t nPreSmooths_, uint32_t preSmoothsWidth_, uint32_t nlnSmooths_, uint32_t lnSmoothsWidth_,
								uint32_t nderSmooths_, uint32_t derSmoothsWidth_, double fit_width_)
	{
		DBG_PRINT_PRETTY_FUNC;
		// Set common properties using base function
		IVAnalyser::SetSmoothingProperties(nPreSmooths_, preSmoothsWidth_, nderSmooths_, derSmoothsWidth_);

		// Set custom properties
		nlnSmooths = nlnSmooths_;
		lnSmoothsWidth = lnSmoothsWidth_;
		fit_width = fit_width_;

		/*
		std::cout << "nPreSmooth: "           << nPreSmooths << std::endl
					<< "preSmoothWidth: "       << preSmoothsWidth << std::endl
					<< "nlnSmooth: "            << nlnSmooths << std::endl
					<< "lnSmoothWidth: "        << lnSmoothsWidth << std::endl
					<< "nDerivativeSmooth: "    << nderSmooths << std::endl
					<< "derivativeSmoothWidth: " << derSmoothsWidth << std::endl
					<< "fitWidth: "             << fit_width << std::endl;
		*/
	}

	void SaveFitPlot(std::string path, std::string prefix)
	{
		DBG_PRINT_PRETTY_FUNC;
		if (!error && plot != NULL)
		{
			std::string RAW_Vbr_text = "RAW Vbr:" + std::to_string(VBR_RAW) + "V";
			std::string comp_Vbr_text = "Comp Vbr:" + std::to_string(VBR_COMP) + "V";
			std::string tempAvgLabelText = "avg temp: " + std::to_string(simpTemp) + "C";
			std::string tempStdDevLabelText = "temp StdDev: " + std::to_string(sipmTempStdDev);

			plot->addLabel(RAW_Vbr_text.c_str(), 0.92, 0.95);
			plot->addLabel(comp_Vbr_text.c_str(), 0.92, 0.98);
			plot->addLabel(tempAvgLabelText.c_str(), 0.7, 0.98);
			plot->addLabel(tempStdDevLabelText.c_str(), 0.7, 0.95);

			std::filesystem::path dirPath(path);
			if (!std::filesystem::exists(dirPath))
			{
				std::filesystem::create_directories(dirPath);
			}

			std::string filename = prefix + "_rel_der_fit" + std::to_string(nPreSmooths) + std::to_string(preSmoothsWidth) + std::to_string(nlnSmooths) + std::to_string(lnSmoothsWidth) + std::to_string(nderSmooths) + std::to_string(derSmoothsWidth) + std::to_string((int)(fit_width * 1000));

			plot->save(path.c_str(), filename.c_str());

			OpenOutRootFile(path, prefix);
			TMultiGraph *ptr = plot -> GetResultMultiGraph();
			ptr -> Write("fitted_data");
			CloseOutRootFile();
		}
	}

	void SaveAllPlot(std::string path, std::string prefix)
	{
		DBG_PRINT_PRETTY_FUNC;
		if (!error)
		{
			SaveFitPlot(path, prefix);

			OpenOutRootFile(path, prefix);

			Plot IV_plot(voltageArray, currentArray, nMeasurements, iv_plot_cut, "IV curve");
			IV_plot.addSmoothedGraph(voltageArray, fineCurr_sm);
			std::string filename = prefix + "_IV" + std::to_string(nPreSmooths) + std::to_string(preSmoothsWidth) + std::to_string(nlnSmooths) + std::to_string(lnSmoothsWidth) + std::to_string(nderSmooths) + std::to_string(derSmoothsWidth) + std::to_string((int)(fit_width * 1000));
			IV_plot.save(path.c_str(), filename.c_str());

			TMultiGraph *ptr = IV_plot.GetResultMultiGraph();
			ptr -> Write("iv_plot");

			Plot ln_IV_plot(voltageArray, lnArray, nMeasurements, ln_iv_plot_cut, "ln IV curve");
			ln_IV_plot.addSmoothedGraph(voltageArray, lnArray_sm);
			filename = prefix + "_ln_IV" + std::to_string(nPreSmooths) + std::to_string(preSmoothsWidth) + std::to_string(nlnSmooths) + std::to_string(lnSmoothsWidth) + std::to_string(nderSmooths) + std::to_string(derSmoothsWidth) + std::to_string((int)(fit_width * 1000));
			ln_IV_plot.save(path.c_str(), filename.c_str());

			ptr = ln_IV_plot.GetResultMultiGraph();
			ptr -> Write("ln_iv_plot");

			CloseOutRootFile();
		}
	}

	// get parameters functions---------------------------------

	int Get_nlnSmooths()
	{
		DBG_PRINT_PRETTY_FUNC;
		return nlnSmooths;
	}

	int Get_lnSmoothsWidth()
	{
		DBG_PRINT_PRETTY_FUNC;
		return lnSmoothsWidth;
	}

	double Get_fit_width()
	{
		DBG_PRINT_PRETTY_FUNC;
		return fit_width;
	}

	// destructor
	virtual ~RelativeDerivativeAnalysis()
	{
		DBG_PRINT_PRETTY_FUNC;
#ifdef NDEBUG
		std::cout << "lnArray: " << lnArray << std::endl;
		std::cout << "lnSMArray: " << lnArray_sm << std::endl;
#endif
		/*
		 * if nSmooth* is set to zero one of the functions returns the same address
		 * must prevent double free
		 */
		if (lnArray != NULL && lnArray_sm != NULL && lnArray == lnArray_sm)
		{
			delete lnArray;
		}
		else if (lnArray)
			delete lnArray;
		else if (lnArray_sm)
			delete lnArray_sm;
	}

	bool RunAnalysis()
	{
		DBG_PRINT_PRETTY_FUNC;
		if (nMeasurements == 0 || voltageArray == NULL || currentArray == NULL)
		{
			error = true;
			return false;
		}
		lnArray = new (std::nothrow) double[nMeasurements];
		if (lnArray == NULL)
		{
			error = true;
			return false;
		}

		// calculate plot display cuts
		iv_plot_cut = nPreSmooths * ((preSmoothsWidth - 1) / 2);
		ln_iv_plot_cut = iv_plot_cut + nlnSmooths * ((lnSmoothsWidth - 1) / 2);
		der_plot_cut = ln_iv_plot_cut + nderSmooths * ((derSmoothsWidth - 1) / 2) + 5;

		// apply SavitzkyGolay filter
		fineCurr_sm = runSGMultiple(nMeasurements, currentArray, preSmoothsWidth, nPreSmooths);

		// calculate ln()
		for (uint32_t i = 0; i < nMeasurements; i++)
		{
			lnArray[i] = TMath::Log(fineCurr_sm[i]);
		}

		// apply SavitzkyGolay filter after ln()
		lnArray_sm = runSGMultiple(nMeasurements, lnArray, lnSmoothsWidth, nlnSmooths);

		// calculate derivative
		Derivate Derivate_(nMeasurements, lnArray_sm, voltageArray);
		derivativeArray = Derivate_.GetDerivative();

		// apply SavitzkyGolay filter after derivative
		derivativeArray_sm = runSGMultiple(nMeasurements, derivativeArray, derSmoothsWidth, nderSmooths);

		plot = std::make_unique<Plot>(voltageArray, derivativeArray, nMeasurements, der_plot_cut, "relative derivative");
		if (plot == NULL)
		{
			error = true;
			return false;
		}

		plot->addSmoothedGraph(voltageArray, derivativeArray_sm);

		TGraph *relDerGraphSm = plot->GetSmoothedTgraph();
		if (relDerGraphSm == NULL)
		{
			error = true;
			return false;
		}

		max_xpos = GetGraphYmaxXpos(relDerGraphSm);

		printf("max XPos:%lf\n", max_xpos);

		f1 = std::make_unique<TF1>("f1", "gaus", max_xpos - fit_width, max_xpos + fit_width);

		relDerGraphSm->Fit("f1", "R");

		VBR_RAW = relDerGraphSm->GetFunction("f1")->GetParameter(1);

		VBR_COMP = calcBreakdownTo25C_nearest(VBR_RAW);

		CHI2 = f1->GetChisquare();

		return true;
	}

private:
	//------------------class private functions---------------------------------

	//------------------class private variables---------------------------------
	double *lnArray = NULL;
	double *lnArray_sm = NULL;

	int ln_iv_plot_cut = 0;
	int der_plot_cut = 0;

	//----------------------------------------------------------------------------------
	// default settings, modifiable through functions

	// number of runs of SG filter after ln(), if no filter is required set to 0
	int nlnSmooths = 0;
	// SG filter width 5,7 or 9
	int lnSmoothsWidth = 7;

	double fit_width = 0.15;
	//----------------------------------------------------------------------------------
};

#endif /* __RelDerivativeAnalysis_H__ */
