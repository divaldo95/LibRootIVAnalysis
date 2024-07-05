namespace CSharpTest;

public static class FileWriter
{
    public static void WriteBinaryIVData(int module, int SiPMNum, string basePath, double[] temps, double[] dmmVoltage, double[] smuVoltage, double[] smuCurrent, double dmmResistance, string arrayID, long timestamp)
    {
        if (dmmVoltage == null || smuVoltage == null || smuCurrent == null) return;
        int sipmid = SiPMNum % 16;
        int arraypos = SiPMNum / 16;
        //string fullPath = Path.Combine(basePath, SiPMNum.ToString(), "IV");
        string fullPath = Path.Combine(basePath, arrayID, "IV");
        Directory.CreateDirectory(fullPath);
        Console.WriteLine($"Binary output: {fullPath}");
        using (FileStream fileStream = new FileStream(Path.Combine(fullPath, "IV_" + sipmid + ".bin"), FileMode.Create, FileAccess.Write, FileShare.None, (int)(sizeof(double) * 1024), FileOptions.WriteThrough))
        {
            BinaryWriter writer = new BinaryWriter(fileStream);
            byte version = 2;
            writer.Write(version);

            byte idLength = Convert.ToByte(arrayID.Length);

            //writer.Write(idLength);
            writer.Write(arrayID);

            writer.Write(Convert.ToByte(module)); //0 front 1 back
            writer.Write(Convert.ToByte(sipmid)); //sipm num on array (0-15)
            writer.Write(Convert.ToByte(arraypos)); //sipm array position

            UInt64 datetime = (ulong)timestamp; //ConvertToTimestamp(DateTime.Now);
            writer.Write(datetime);

            writer.Write(dmmResistance);

            for (int i = 0; i < 8; i++)
            {
                writer.Write(temps[i]);
            }
            for (int i = 0; i < 2; i++)
            {
                writer.Write((double)0.0);
            }
            for (int i = 0; i < 8; i++)
            {
                writer.Write(temps[i]);
            }
            for (int i = 0; i < 2; i++)
            {
                writer.Write((double)0.0);
            }

            writer.Write(Convert.ToUInt32(dmmVoltage.Length));

            for (int i = 0; i < dmmVoltage.Length; i++) //dmm voltages
            {
                writer.Write(dmmVoltage[i]);
            }
            for (int i = 0; i < smuCurrent.Length; i++) //smu current
            {
                writer.Write(smuCurrent[i]);
            }
            for (int i = 0; i < smuVoltage.Length; i++) //smu voltages
            {
                writer.Write(smuVoltage[i]);
            }

            /*
            if (darkCurrentData != null)
            {
                writer.Write(darkCurrentData.Id1);
                writer.Write(darkCurrentData.Id2);
                writer.Write(darkCurrentData.Ileak1);
                writer.Write(darkCurrentData.Ileak2);
                writer.Write(darkCurrentData.start_temp);
                writer.Write(darkCurrentData.end_temp);
            }
            else*/
            {
                double zero_Data = 0;
                for (int i = 0; i < 6; i++)
                    writer.Write(zero_Data);
            }

            /*
            if (forwardResistanceData != null)
            {
                writer.Write(forwardResistanceData.ForwardResistance);
                writer.Write(forwardResistanceData.V_1);
                writer.Write(forwardResistanceData.V_2);
                writer.Write(forwardResistanceData.I_1);
                writer.Write(forwardResistanceData.I_2);
                writer.Write(forwardResistanceData.start_temp);
                writer.Write(forwardResistanceData.end_temp);
            }
            else*/
            {
                double zero_Data = 0;
                for (int i = 0; i < 7; i++)
                    writer.Write(zero_Data);
            }

            writer.Close();
        }
    }
}
