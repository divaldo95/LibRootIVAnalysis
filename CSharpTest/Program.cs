namespace CSharpTest;

class Program
{
    static void Main(string[] args)
    {
        Console.WriteLine("Hello, World!");
        RootIVAnalyser.TestLibrary(FilePathHelper.GetCurrentDirectory() + "IV_0_0_0_0.json");
    }
}
