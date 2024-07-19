using CSharpTest;
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
    public static extern IntPtr RIVA_Class_AnalyseIV(IntPtr ivAnalyser,
                                                    SiPMData data,
                                                    AnalysisTypes method,
                                                    double temperatureToCompensate,
                                                    bool savePlots,
                                                    string outBasePath,
                                                    string filePrefix);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void RIVA_Class_GetResults(IntPtr ivAnalyser,
                                                        out double rawVbr,
                                                        out double compVbr,
                                                        out double cs);


    private IntPtr ivAnalyser;
    private bool disposedValue;

    public RootIVAnalyser()
    {
        ivAnalyser = RIVA_Class_Create();
    }

    public void AnalyseIV(SiPMData data, AnalysisTypes type, double temperatureToCompensate, bool savePlots, string outBasePath, string filePrefix)
    {
        RIVA_Class_AnalyseIV(ivAnalyser, data, type, temperatureToCompensate, savePlots, outBasePath, filePrefix);
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

    public static CurrentMeasurementDataModel OpenFile(string file)
    {
        CurrentMeasurementDataModel c = JSONHelper.ReadJsonFile<CurrentMeasurementDataModel>(file);

        /*
        double[] temps;
        if (c.IVResult.Temperatures.Count > 0)
        {
            int index = (int)c.IVResult.Temperatures.Count / 2;
            temps = c.IVResult.Temperatures[index].Module1;
        }
        else
        {
            temps = new double[8];
            for (int i = 0; i < 8; i++)
            {
                temps[i] = 25.0;
            }
        }
        */
        return c;
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

        double[] voltagesArray = c.IVResult.DMMVoltage.ToArray();
        double[] currentsArray = c.IVResult.SMUCurrent.ToArray();

        // Pin the arrays to get their pointers
        GCHandle voltagesHandle = GCHandle.Alloc(voltagesArray, GCHandleType.Pinned);
        GCHandle currentsHandle = GCHandle.Alloc(currentsArray, GCHandleType.Pinned);

        SiPMData data = new SiPMData
        {
            voltages = voltagesHandle.AddrOfPinnedObject(),
            currents = currentsHandle.AddrOfPinnedObject(),
            dataPoints = (nuint)c.IVResult.SMUCurrent.Count,
            preTemp = 25.0,
            postTemp = 30.0,
            timestamp = (ulong)c.IVResult.StartTimestamp
        };

        string outPath = Path.Combine(FilePathHelper.GetCurrentDirectory(), "results");
        Directory.CreateDirectory(outPath);

        iv.AnalyseIV(data, AnalysisTypes.RelativeDerivativeMethod, 20.0, true, outPath, "TestIVResult");

        voltagesHandle.Free();
        currentsHandle.Free();

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

    public static int GetUsedTemperatureIndex(int arrayIndex, int sipmIndex)
    {
        int sipmIndexOffset = arrayIndex * 2 + (sipmIndex < 8 ? 0 : 1);
        return sipmIndexOffset;
    }

    public static List<double> GetTemperatures(CurrentMeasurementDataModel c)
    {
        List<double> temperatures = new List<double>();
        int index = GetUsedTemperatureIndex(c.SiPMLocation.Array, c.SiPMLocation.SiPM);
        for (int i = 0; i < c.IVResult.Temperatures.Count; i++)
        {
            if (c.SiPMLocation.Module == 0)
            {
                double temp = c.IVResult.Temperatures[i].Module1[index];
                temperatures.Add(temp);
            }
            else
            {
                double temp = c.IVResult.Temperatures[i].Module2[index];
                temperatures.Add(temp);
            }
        }
        return temperatures;
    }

    public static List<double> GetDarkCurrentTemperatures(CurrentMeasurementDataModel c)
    {
        List<double> temperatures = new List<double>();
        int index = GetUsedTemperatureIndex(c.SiPMLocation.Array, c.SiPMLocation.SiPM);
        if (c.DarkCurrentResult.DarkCurrentResult == null)
        {
            return temperatures;
        }
        for (int i = 0; i < c.DarkCurrentResult.DarkCurrentResult.Temperatures.Count; i++)
        {
            if (c.SiPMLocation.Module == 0)
            {
                double temp = c.DarkCurrentResult.DarkCurrentResult.Temperatures[i].Module1[index];
                temperatures.Add(temp);
            }
            else
            {
                double temp = c.DarkCurrentResult.DarkCurrentResult.Temperatures[i].Module2[index];
                temperatures.Add(temp);
            }
        }
        return temperatures;
    }

    public static void CalculateBreakdown(string file, out double Vbr, out double compVbr, out double chi2)
    {
        CurrentMeasurementDataModel c = JSONHelper.ReadJsonFile<CurrentMeasurementDataModel>(file);
        CalculateBreakdown(c, out Vbr, out compVbr, out chi2);
    }

    public static void CalculateBreakdown(CurrentMeasurementDataModel c, out double Vbr, out double compVbr, out double chi2)
    {
        Console.WriteLine("Starting analysis...");
        RootIVAnalyser iv = new RootIVAnalyser();

        double[] voltagesArray = c.IVResult.DMMVoltage.ToArray();
        double[] currentsArray = c.IVResult.SMUCurrent.ToArray();

        //compensate current
        if (c.DMMResistanceResult.Resistance > 0)
        {
            for (int i = 0; i < voltagesArray.Length; i++)
            {
                double current = voltagesArray[i] / c.DMMResistanceResult.Resistance;
                currentsArray[i] = currentsArray[i] - current;
                if (currentsArray[i] < 0)
                {

                    Console.WriteLine($"Negative current ({currentsArray[i].ToString("0.00")}). Consider increasing the DMM resistance compensation percentage");
                    currentsArray[i] = 0;
                }
            }
        }

        // Pin the arrays to get their pointers
        GCHandle voltagesHandle = GCHandle.Alloc(voltagesArray, GCHandleType.Pinned);
        GCHandle currentsHandle = GCHandle.Alloc(currentsArray, GCHandleType.Pinned);

        List<double> temperatures = GetTemperatures(c);

        double usedTemp = temperatures.Average();

        SiPMData data = new SiPMData
        {
            voltages = voltagesHandle.AddrOfPinnedObject(),
            currents = currentsHandle.AddrOfPinnedObject(),
            dataPoints = (nuint)c.IVResult.SMUCurrent.Count,
            preTemp = 25.0,
            postTemp = usedTemp,
            timestamp = (ulong)c.IVResult.StartTimestamp
        };

        string outPath = Path.Combine(FilePathHelper.GetCurrentDirectory(), "results");
        Directory.CreateDirectory(outPath);
        string outFilePrefix = $"{c.SiPMLocation.Block}_{c.SiPMLocation.Module}_{c.SiPMLocation.Array}_{c.SiPMLocation.SiPM}";

        iv.AnalyseIV(data, AnalysisTypes.RelativeDerivativeMethod, 20.0, false, outPath, outFilePrefix);

        voltagesHandle.Free();
        currentsHandle.Free();

        iv.GetResult(out Vbr, out compVbr, out chi2);

        Console.WriteLine($"Vbr: {Vbr.ToString()}, cVbr: {compVbr.ToString()}, ChiSquare: {chi2}");

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

public enum VoltageAndCurrentMeasurementTypes
{
    LeakageCurrent,
    DarkCurrent,
    ForwardResistance,
    Unknown
}

public class CurrentMeasurementDataModel
{
    public bool IsIVDone { get; set; } = false;
    public bool IsSPSDone { get; set; } = false;
    public CurrentSiPMModel SiPMLocation { get; set; }
    public string Barcode { get; set; } = "";
    public SiPM SiPMMeasurementDetails { get; set; }
    public DMMResistanceMeasurementResponseModel DMMResistanceResult { get; set; } = new DMMResistanceMeasurementResponseModel();
    public MeasurementIdentifier IVMeasurementID { get; set; } = new MeasurementIdentifier();
    public MeasurementIdentifier SPSMeasurementID { get; set; } = new MeasurementIdentifier();
    public IVMeasurementResponseModel IVResult { get; set; } = new IVMeasurementResponseModel();
    public ForwardResistanceMeasurementResponseModel ForwardResistanceResult { get; set; } = new ForwardResistanceMeasurementResponseModel();
    public DarkCurrentMeasurementResponseModel DarkCurrentResult { get; set; } = new DarkCurrentMeasurementResponseModel();
}

public class SMUVoltageModel
{
    public double Voltage { get; set; }
    public double CurrentLimit { get; set; }
    public double CurrentLimitRange { get; set; }
    public int Iterations { get; set; }
    public double VoltageRange { get; set; }
}

public class NIVoltageAndCurrentStartModel
{
    public MeasurementIdentifier Identifier { get; set; } = new MeasurementIdentifier();
    public VoltageAndCurrentMeasurementTypes MeasurementType { get; set; } = VoltageAndCurrentMeasurementTypes.Unknown;
    public SMUVoltageModel FirstIteration { get; set; } = new SMUVoltageModel();
    public SMUVoltageModel SecondIteration { get; set; } = new SMUVoltageModel();
}

public class VoltageAndCurrentMeasurementResponseModel
{
    public MeasurementIdentifier Identifier { get; set; }
    public NIVoltageAndCurrentStartModel StartModel { get; set; }
    public long StartTimestamp { get; set; }
    public long EndTimestamp { get; set; }
    public bool ErrorHappened { get; set; }
    public string ErrorMessage { get; set; }
    public List<double> FirstIterationVoltages { get; set; }
    public List<double> FirstIterationCurrents { get; set; }
    public List<double> SecondIterationVoltages { get; set; }
    public List<double> SecondIterationCurrents { get; set; }
    public double FirstIterationVoltageAverage { get; set; }
    public double SecondIterationVoltageAverage { get; set; }
    public double FirstIterationCurrentAverage { get; set; }
    public double SecondIterationCurrentAverage { get; set; }
    public List<TemperaturesArray> Temperatures { get; set; }
}

public class DarkCurrentMeasurementResponseModel : IEquatable<MeasurementIdentifier>
{
    public VoltageAndCurrentMeasurementResponseModel LeakageCurrentResult { get; set; } = new VoltageAndCurrentMeasurementResponseModel();
    public VoltageAndCurrentMeasurementResponseModel DarkCurrentResult { get; set; } = new VoltageAndCurrentMeasurementResponseModel();

    public double FirstLeakageCurrent
    {
        get
        {
            if (LeakageCurrentResult == null)
            {
                throw new NullReferenceException("LeakageCurrentResult is null");
            }
            return LeakageCurrentResult.FirstIterationCurrentAverage;
        }
    }

    public double SecondLeakageCurrent
    {
        get
        {
            if (LeakageCurrentResult == null)
            {
                throw new NullReferenceException("LeakageCurrentResult is null");
            }
            return LeakageCurrentResult.SecondIterationCurrentAverage;
        }
    }

    public double FirstDarkCurrent
    {
        get
        {
            if (DarkCurrentResult == null)
            {
                throw new NullReferenceException("DarkCurrentResult is null");
            }
            return DarkCurrentResult.FirstIterationCurrentAverage;
        }
    }

    public double SecondDarkCurrent
    {
        get
        {
            if (DarkCurrentResult == null)
            {
                throw new NullReferenceException("DarkCurrentResult is null");
            }
            return DarkCurrentResult.SecondIterationCurrentAverage;
        }
    }

    public double FirstDarkCurrentCompensated
    {
        get
        {
            if (DarkCurrentResult == null || LeakageCurrentResult == null)
            {
                throw new NullReferenceException("DarkCurrentResult or LeakageCurrentResult is null");
            }
            return DarkCurrentResult.FirstIterationCurrentAverage - LeakageCurrentResult.FirstIterationCurrentAverage;
        }
    }

    public double SecondDarkCurrentCompensated
    {
        get
        {
            if (DarkCurrentResult == null || LeakageCurrentResult == null)
            {
                throw new NullReferenceException("DarkCurrentResult or LeakageCurrentResult is null");
            }
            return DarkCurrentResult.SecondIterationCurrentAverage - LeakageCurrentResult.FirstIterationCurrentAverage;
        }
    }

    public DarkCurrentMeasurementResponseModel()
    {
    }

    //To get the right measurement by only one matching identifier
    public bool Equals(MeasurementIdentifier? other)
    {
        if (other == null)
        {
            return false;
        }

        if (LeakageCurrentResult != null && LeakageCurrentResult.Identifier != null)
        {
            if (LeakageCurrentResult.Identifier.Equals(other))
            {
                return true;
            }
        }
        if (DarkCurrentResult != null && DarkCurrentResult.Identifier != null)
        {
            if (DarkCurrentResult.Identifier.Equals(other))
            {
                return true;
            }
        }
        return false;
    }
}

public class ForwardResistanceMeasurementResponseModel : IEquatable<MeasurementIdentifier>
{
    public VoltageAndCurrentMeasurementResponseModel Result { get; set; } = new VoltageAndCurrentMeasurementResponseModel();

    public double ForwardResistance
    {
        get
        {
            if (Result == null)
            {
                throw new NullReferenceException("Forward Resistance Result is null");
            }
            return (Math.Abs(Result.SecondIterationVoltageAverage) - Math.Abs(Result.FirstIterationVoltageAverage)) / (Math.Abs(Result.SecondIterationCurrentAverage) - Math.Abs(Result.FirstIterationCurrentAverage)) - 49.9;
        }
    }

    public ForwardResistanceMeasurementResponseModel()
    {
    }

    //Get by identifier
    public bool Equals(MeasurementIdentifier? other)
    {
        if (other == null)
        {
            return false;
        }

        if (Result != null && Result.Identifier != null)
        {
            if (Result.Identifier.Equals(other))
            {
                return true;
            }
        }
        return false;
    }
}

public class DMMResistanceMeasurementResponseModel
{
    public MeasurementIdentifier Identifier { get; set; }
    public long StartTimestamp { get; set; }
    public long EndTimestamp { get; set; }
    public bool ErrorHappened { get; set; }
    public string ErrorMessage { get; set; }
    public int CorrectionPercentage { get; set; }
    public List<double> Voltages { get; set; }
    public List<double> Currents { get; set; }
    public double Resistance { get; set; } = 0.0;
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

public enum AnalysisTypes
{
    RelativeDerivativeMethod,
    ThirdDerivativeMethod
}

public struct AnalysisProperties
{
    public int nPreSmooth = 1;
    public int preSmoothWidth = 5;
    public int nlnSmooth = 1;
    public int lnSmoothWidth = 5;
    public int nDerivativeSmooth = 1;
    public int derivativeSmoothWidth = 5;
    public int fitWidth = 100;

    //set default properties
    public AnalysisProperties()
    {
        nPreSmooth = 1;
        preSmoothWidth = 5;
        nlnSmooth = 1;
        lnSmoothWidth = 5;
        nDerivativeSmooth = 1;
        derivativeSmoothWidth = 5;
        fitWidth = 100;
    }
}

[StructLayout(LayoutKind.Sequential)]
public struct SiPMData
{
    public IntPtr voltages;
    public IntPtr currents;
    public UIntPtr dataPoints;
    public double preTemp;
    public double postTemp;
    public ulong timestamp;
}