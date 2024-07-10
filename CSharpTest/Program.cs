using System.Security.AccessControl;
using System.Text;
using Newtonsoft.Json;

namespace CSharpTest;

class ResultData
{
    public double RawBreakdownVoltage { get; set; } = 0.0;
    public double CompensatedBreakdownVoltage { get; set; } = 0.0;
    public double ChiSquare { get; set; } = 0.0;
}

class Program
{
    /*
     * This function opens a folder recursively, do the analysis for each file
     * and groups the results by file name. The results are saved in a JSON file.
     */
    public static void BatchAnalysis()
    {
        double vbr = 0.0;
        double compvbr = 0.0;
        double chi2 = 0.0;

        string directoryToLoopThrough = Path.Combine(FilePathHelper.GetCurrentDirectory(), "IVResult");
        string fileNamePattern = "*.json"; // The common file name pattern to match

        // Get all directories in the parent directory
        string[] directories = Directory.GetDirectories(directoryToLoopThrough);

        // Dictionary to hold data grouped by file name
        Dictionary<string, List<ResultData>> groupedData = new Dictionary<string, List<ResultData>>();

        // Loop through each directory
        foreach (string directory in directories)
        {
            // Get all files with the specified file name pattern in the current directory
            string[] files = Directory.GetFiles(directory, fileNamePattern);

            // Process each file in the directory
            foreach (string file in files)
            {
                // Read JSON data from the file
                RootIVAnalyser.CalculateBreakdown(file, out vbr, out compvbr, out chi2);
                ResultData resultData = new ResultData
                {
                    RawBreakdownVoltage = vbr,
                    CompensatedBreakdownVoltage = compvbr,
                    ChiSquare = chi2
                };

                // Get the file name (without directory path)
                string fileName = Path.GetFileName(file);

                // Add the data to the dictionary, grouping by file name
                if (!groupedData.ContainsKey(fileName))
                {
                    groupedData[fileName] = new List<ResultData>();
                }
                groupedData[fileName].Add(resultData);
            }
        }

        // Now you can process the grouped data as needed
        foreach (var kvp in groupedData)
        {
            string fileName = kvp.Key;
            List<ResultData> dataList = kvp.Value;

            // Process the list of data for each file name
            Console.WriteLine($"File Name: {fileName}");
            foreach (var data in dataList)
            {
                // Process each data item
                Console.WriteLine($"Data: {data}");
            }

            // Serialize the groupedData dictionary to JSON
            string jsonResult = JsonConvert.SerializeObject(groupedData, Formatting.Indented);

            // Output the JSON result
            //Console.WriteLine(jsonResult);

            // Optionally, save the JSON result to a file
            File.WriteAllText("output.json", jsonResult);
        }
    }

    public static void BatchIVDataConverter(string directory, bool relativePath = true)
    {
        string directoryToLoopThrough;
        if (relativePath)
        {
            directoryToLoopThrough = Path.Combine(FilePathHelper.GetCurrentDirectory(), directory);
        }
        else
        {
            directoryToLoopThrough = directory;
        }

        string fileNamePattern = "*.json"; // The common file name pattern to match
        string[] files = Directory.GetFiles(directory, fileNamePattern);

        // Process each file in the directory
        foreach (string file in files)
        {
            Console.WriteLine($"File: {file}");
            // Read JSON data from the file

            CurrentMeasurementDataModel data = RootIVAnalyser.OpenFile(file);

            double[] temps;
            if (data.IVResult.Temperatures.Count > 0)
            {
                int index = (int)data.IVResult.Temperatures.Count / 2;
                temps = data.IVResult.Temperatures[index].Module1;
            }
            else
            {
                temps = new double[8];
                for (int i = 0; i < 8; i++)
                {
                    temps[i] = 25.0;
                }
            }

            FileWriter.WriteBinaryIVData(data.SiPMLocation.Module, data.SiPMLocation.SiPM, "IVConvert", temps, data.IVResult.DMMVoltage.ToArray(), data.IVResult.SMUVoltage.ToArray(), data.IVResult.SMUCurrent.ToArray(), data.DMMResistanceResult.Resistance, "0106", data.IVResult.StartTimestamp);
        }
    }

    public static void BatchIVDataAnalyser(string directory, bool relativePath = true)
    {
        string directoryToLoopThrough;
        if (relativePath)
        {
            directoryToLoopThrough = Path.Combine(FilePathHelper.GetCurrentDirectory(), directory);
        }
        else
        {
            directoryToLoopThrough = directory;
        }

        string fileNamePattern = "*.json"; // The common file name pattern to match
        string[] files = Directory.GetFiles(directory, fileNamePattern);

        // Process each file in the directory
        StringBuilder sb = new StringBuilder();
        string filePath = Path.Combine(directory, $"results.txt");
        using StreamWriter outputFile = new StreamWriter(filePath, true);
        foreach (string file in files)
        {
            try
            {
                sb.Clear();
                Console.WriteLine($"File: {file}");
                // Read JSON data from the file
                CurrentMeasurementDataModel data = RootIVAnalyser.OpenFile(file);

                var temps = RootIVAnalyser.GetTemperatures(data);
                var dcTemps = RootIVAnalyser.GetDarkCurrentTemperatures(data);
                double average = temps.Average();
                double sumOfSquaresOfDifferences = temps.Select(val => (val - average) * (val - average)).Sum();
                double stdDevTemp = Math.Sqrt(sumOfSquaresOfDifferences / temps.Count);

                double Vbr = 0.0;
                double cVbr = 0.0;
                double chi2 = 0.0;

                RootIVAnalyser.CalculateBreakdown(data, out Vbr, out cVbr, out chi2);
                sb.Append(data.Barcode);
                sb.Append(" ");
                sb.Append(data.SiPMLocation.SiPM);
                sb.Append(" ");
                sb.Append(average);
                sb.Append(" ");
                sb.Append(stdDevTemp);
                sb.Append(" ");
                sb.Append(Vbr);
                sb.Append(" ");
                sb.Append(cVbr);
                sb.Append(" ");

                sb.Append(data.DarkCurrentResult.FirstDarkCurrent);
                sb.Append(" ");
                sb.Append(data.DarkCurrentResult.SecondDarkCurrent);
                sb.Append(" ");
                sb.Append(dcTemps.Average());
                sb.Append(" ");
                sb.Append(data.ForwardResistanceResult.ForwardResistance);
                sb.Append(" ");

                outputFile.WriteLine(sb.ToString());
            }
            catch (System.Exception ex)
            {
                Console.WriteLine($"Error: {ex.Message}");
            }

        }
    }


    static void Main(string[] args)
    {
        Console.WriteLine("Hello, World!");
        try
        {
            string directory = "Measurements/61-64";
            //RootIVAnalyser.TestLibrary(FilePathHelper.GetCurrentDirectory() + "IV_0_0_0_0.json");
            //BatchIVDataConverter(directory);
            BatchIVDataAnalyser(directory);
        }
        catch (System.Exception e)
        {
            Console.WriteLine($"Error: {e.Message}{Environment.NewLine}{e.StackTrace}");
        }

    }
}
