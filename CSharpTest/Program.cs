using System.Security.AccessControl;
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
    static void Main(string[] args)
    {
        Console.WriteLine("Hello, World!");
        RootIVAnalyser.TestLibrary(FilePathHelper.GetCurrentDirectory() + "IV_0_0_0_0.json");
        BatchAnalysis();
    }
}
