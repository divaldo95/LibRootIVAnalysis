namespace CSharpTest;

public static class FileWriter
{
    public static void WriteBinaryIVData(CurrentMeasurementDataModel data, string basePath)
    {
        if (data.IVResult.DMMVoltage == null || data.IVResult.SMUVoltage == null || data.IVResult.SMUCurrent == null) return;
        //string fullPath = Path.Combine(basePath, SiPMNum.ToString(), "IV");
        string fullPath = Path.Combine(basePath, data.Barcode, "IV");
        Directory.CreateDirectory(fullPath);
        Console.WriteLine($"Binary output: {fullPath}");
        using (FileStream fileStream = new FileStream(Path.Combine(fullPath, "IV_" + data.SiPMLocation.SiPM + ".bin"), FileMode.Create, FileAccess.Write, FileShare.None, (int)(sizeof(double) * 1024), FileOptions.WriteThrough))
        {
            BinaryWriter writer = new BinaryWriter(fileStream);
            byte version = 2;
            writer.Write(version);

            byte idLength = Convert.ToByte(data.Barcode.Length);

            //writer.Write(idLength);
            writer.Write(data.Barcode);

            writer.Write(Convert.ToByte(data.SiPMLocation.Module)); //0 front 1 back
            writer.Write(Convert.ToByte(data.SiPMLocation.SiPM)); //sipm num on array (0-15)
            writer.Write(Convert.ToByte(data.SiPMLocation.Array)); //sipm array position

            UInt64 datetime = (ulong)data.IVResult.StartTimestamp; //ConvertToTimestamp(DateTime.Now);
            writer.Write(datetime);

            writer.Write(data.DMMResistanceResult.Resistance);

            double[] temps;
            if (data.IVResult.Temperatures.Count > 0)
            {
                int index = (int)data.IVResult.Temperatures.Count / 2;
                temps = data.SiPMLocation.Module == 0 ? data.IVResult.Temperatures[index].Module1 : data.IVResult.Temperatures[index].Module2;
            }
            else
            {
                temps = new double[8];
                for (int i = 0; i < 8; i++)
                {
                    temps[i] = 25.0;
                }
            }

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

            writer.Write(Convert.ToUInt32(data.IVResult.DMMVoltage.Count));

            for (int i = 0; i < data.IVResult.DMMVoltage.Count; i++) //dmm voltages
            {
                writer.Write(data.IVResult.DMMVoltage[i]);
            }
            for (int i = 0; i < data.IVResult.SMUCurrent.Count; i++) //smu current
            {
                writer.Write(data.IVResult.SMUCurrent[i]);
            }
            for (int i = 0; i < data.IVResult.SMUVoltage.Count; i++) //smu voltages
            {
                writer.Write(data.IVResult.SMUVoltage[i]);
            }


            if (data.DarkCurrentResult.DarkCurrentResult != null)
            {
                if (data.DarkCurrentResult.DarkCurrentResult.Temperatures.Count > 0)
                {
                    int index = (int)data.DarkCurrentResult.DarkCurrentResult.Temperatures.Count / 2;
                    temps = data.SiPMLocation.Module == 0 ? data.DarkCurrentResult.DarkCurrentResult.Temperatures[index].Module1 : data.DarkCurrentResult.DarkCurrentResult.Temperatures[index].Module2;
                }
                else
                {
                    temps = new double[8];
                    for (int i = 0; i < 8; i++)
                    {
                        temps[i] = 25.0;
                    }
                }

                writer.Write(data.DarkCurrentResult.FirstDarkCurrent);
                writer.Write(data.DarkCurrentResult.SecondDarkCurrent);
                writer.Write(data.DarkCurrentResult.FirstLeakageCurrent);
                writer.Write(data.DarkCurrentResult.SecondLeakageCurrent);
                writer.Write(temps.First());
                writer.Write(temps.Last());
            }
            else
            {
                double zero_Data = 0;
                for (int i = 0; i < 6; i++)
                    writer.Write(zero_Data);
            }


            if (data.ForwardResistanceResult != null)
            {
                if (data.ForwardResistanceResult.Result.Temperatures.Count > 0)
                {
                    int index = (int)data.ForwardResistanceResult.Result.Temperatures.Count / 2;
                    temps = data.SiPMLocation.Module == 0 ? data.ForwardResistanceResult.Result.Temperatures[index].Module1 : data.ForwardResistanceResult.Result.Temperatures[index].Module2;
                }
                else
                {
                    temps = new double[8];
                    for (int i = 0; i < 8; i++)
                    {
                        temps[i] = 25.0;
                    }
                }

                writer.Write(data.ForwardResistanceResult.ForwardResistance);
                writer.Write(data.ForwardResistanceResult.Result.FirstIterationVoltageAverage);
                writer.Write(data.ForwardResistanceResult.Result.SecondIterationVoltageAverage);
                writer.Write(data.ForwardResistanceResult.Result.FirstIterationCurrentAverage);
                writer.Write(data.ForwardResistanceResult.Result.SecondIterationCurrentAverage);
                writer.Write(temps.First());
                writer.Write(temps.Last());
            }
            else
            {
                double zero_Data = 0;
                for (int i = 0; i < 7; i++)
                    writer.Write(zero_Data);
            }

            writer.Close();
        }
    }
}
