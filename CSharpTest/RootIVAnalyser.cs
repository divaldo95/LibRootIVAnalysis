using Newtonsoft.Json;
using System.Runtime.InteropServices;

public class RootIVAnalyser : IDisposable
{
    const string LibraryName = "RootIVAnalyser";

    [DllImport(LibraryName, EntryPoint = "RIVA_Class_Create", ExactSpelling = false, CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr RIVA_Class_Create();
    [DllImport(LibraryName, EntryPoint = "RIVA_Class_Delete", ExactSpelling = false, CallingConvention = CallingConvention.Cdecl)]
    public static extern void RIVA_Class_Delete(IntPtr ivAnalyser);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void RIVA_Class_AnalyseIV(IntPtr ivAnalyser,
                                                double[] voltages,
                                                double[] currents,
                                                UIntPtr dataPoints,
                                                double preTemp,
                                                double postTemp,
                                                int arrayID,
                                                int sipmID,
                                                ulong timestamp,
                                                string outBasePath);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void RIVA_Class_GetResults(IntPtr ivAnalyser,
                                                        out double rawVbr,
                                                        out double compVbr,
                                                        out double cs);

    /*
    public static void MyMethod()
    {
        try
        {
            TestRootFunction();
        }
        catch (DllNotFoundException ex)
        {
            // Handle the case where the library is not found
            Console.WriteLine("The required library is not available.");
            // Log or take any other appropriate action
        }
    }
    */

    private IntPtr ivAnalyser;
    private bool disposedValue;

    public RootIVAnalyser()
    {
        ivAnalyser = RIVA_Class_Create();
    }

    public void AnalyseIV(double[] voltages, double[] currents, UIntPtr dataPoints, double preTemp, double postTemp,
                            int arrayID, int sipmID, ulong timestamp, string outBasePath)
    {
        RIVA_Class_AnalyseIV(ivAnalyser, voltages, currents, dataPoints, preTemp, postTemp, arrayID, sipmID, timestamp, outBasePath);
    }

    public void GetResult(out double RawBreakdownVoltage, out double CompensatedBreakdownVoltage, out double ChiSquare)
    {
        RIVA_Class_GetResults(ivAnalyser, out RawBreakdownVoltage, out CompensatedBreakdownVoltage, out ChiSquare);
    }

    protected virtual void Dispose(bool disposing)
    {
        if (!disposedValue)
        {
            if (disposing)
            {
                // TODO: dispose managed state (managed objects)
                RIVA_Class_Delete(ivAnalyser);
            }

            // TODO: free unmanaged resources (unmanaged objects) and override finalizer
            // TODO: set large fields to null
            disposedValue = true;
        }
    }

    // // TODO: override finalizer only if 'Dispose(bool disposing)' has code to free unmanaged resources
    ~RootIVAnalyser()
    {
        // Do not change this code. Put cleanup code in 'Dispose(bool disposing)' method
        Dispose(disposing: false);
    }

    public void Dispose()
    {
        // Do not change this code. Put cleanup code in 'Dispose(bool disposing)' method
        Dispose(disposing: true);
        GC.SuppressFinalize(this);
    }

    public static void TestLibrary(string file)
    {
        Console.WriteLine("Starting analysis...");
        RootIVAnalyser iv = new RootIVAnalyser();
        double vbr;
        double cvbr;
        double cs;
        iv.GetResult(out vbr, out cvbr, out cs);

        Console.WriteLine($"Vbr: {vbr.ToString()}, cVbr: {cvbr.ToString()}, ChiSquare: {cs}");

        CurrentMeasurementDataModel c = JSONHelper.ReadJsonFile<CurrentMeasurementDataModel>(file);

        iv.AnalyseIV(c.IVResult.DMMVoltage.ToArray(), c.IVResult.SMUCurrent.ToArray(), (nuint)c.IVResult.SMUCurrent.Count, 25.0, 26.0, 0, 0, (ulong)c.IVResult.StartTimestamp, FilePathHelper.GetCurrentDirectory());


        iv.GetResult(out vbr, out cvbr, out cs);

        Console.WriteLine($"Vbr: {vbr.ToString()}, cVbr: {cvbr.ToString()}, ChiSquare: {cs}");

        Console.WriteLine("Analysis end");
        /*
        foreach (var item in c.IVResult.DMMVoltage)
        {
            Console.WriteLine(item.ToString());
        }
        */
    }
}

internal static class FilePathHelper
{
    public static string GetCurrentDirectory()
    {
        return AppDomain.CurrentDomain.BaseDirectory;
    }
}

internal static class JSONHelper
{
    public static T ReadJsonFile<T>(string filePath)
    {
        // Check if the file exists
        if (!File.Exists(filePath))
        {
            throw new FileNotFoundException("JSON file not found.", filePath);
        }

        // Read the JSON file
        string jsonText = File.ReadAllText(filePath);

        // Deserialize the JSON into the specified type
        T jsonObject = JsonConvert.DeserializeObject<T>(jsonText);

        return jsonObject;
    }
}

internal class CurrentMeasurementDataModel
{
    public bool IsIVDone { get; set; } = false;
    public bool IsSPSDone { get; set; } = false;
    public CurrentSiPMModel SiPMLocation { get; set; }
    public SiPM SiPMMeasurementDetails { get; set; }
    public MeasurementIdentifier IVMeasurementID { get; set; } = new MeasurementIdentifier();
    public MeasurementIdentifier SPSMeasurementID { get; set; } = new MeasurementIdentifier();
    public IVMeasurementResponseModel IVResult { get; set; } = new IVMeasurementResponseModel();
}

public class IVMeasurementResponseModel
{
    public MeasurementIdentifier Identifier { get; set; }
    public long StartTimestamp { get; set; }
    public long EndTimestamp { get; set; }
    public bool ErrorHappened { get; set; }
    public string ErrorMessage { get; set; }
    public List<TemperaturesArray> Temperatures { get; set; }
    public List<double> SMUVoltage { get; set; }
    public List<double> SMUCurrent { get; set; }
    public List<double> DMMVoltage { get; set; }
}

public class TemperaturesArray
{
    public double[] Module1 { get; set; } = new double[8];
    public double[] Module2 { get; set; } = new double[8];
    public double Pulser { get; set; }
    public double ControlTemperature { get; set; }
    public long Timestamp { get; set; }

    public TemperaturesArray()
    {

    }
}

public class SiPM
{
    public int DMMResistance { get; set; }
    public int IV { get; set; }
    public int SPS { get; set; }
    public int SPSVoltagesIsOffsets { get; set; }
    public List<double> IVVoltages { get; set; } = new List<double>();
    public List<double> SPSVoltages { get; set; } = new List<double>();
}

public enum MeasurementType
{
    //[EnumMember(Value = "IVMeasurement")]
    IVMeasurement = 0,
    //[EnumMember(Value = "SPSMeasurement")]
    SPSMeasurement = 1,
    //[EnumMember(Value = "DMMMeasurement")]
    DMMResistanceMeasurement = 2,
    //[EnumMember(Value = "Unknown")]
    Unknown = 999
}

public class MeasurementIdentifier
{
    public MeasurementType Type { get; set; }
    public string ID { get; set; }
}

public class CurrentSiPMModel
{
    public int Block { get; set; }
    public int Module { get; set; }
    public int Array { get; set; }
    public int SiPM { get; set; }

    public CurrentSiPMModel()
    {
        Block = -1;
        Module = -1;
        Array = -1;
        SiPM = -1;
    }
}