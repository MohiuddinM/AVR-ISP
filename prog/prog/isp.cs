<<<<<<< HEAD
﻿/*
    STM32 based AVR programmer
    Copyright (C) 2014  Muhammad Mohiuddin

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
=======
/*
STM32 based AVR programmer
Copyright (C) 2014 Muhammad Mohiuddin
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
>>>>>>> origin/master
*/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.IO.Ports;
using System.Threading;

namespace prog
{
    class isp
    {
        public enum ProgrammingSpeed
        {
            High = 0,
            Normal,
            Low,
            Lowest
        };
        static private SerialPort sPort;
        static public byte[] Hex2Bin(string hexPath)
        {
            string data;
            int byteCount, recordType, address, checksum;
            byte[] binImage = new byte[1024 * 1024];
            int index = 0;

            TextReader hexFile = File.OpenText(hexPath);
            string hex = hexFile.ReadToEnd();
            string[] hexLines = hex.Split(':');


            foreach (string hexLine in hexLines)
            {
                if (hexLine.Length > 1)
                {
                    byteCount = Convert.ToInt32(hexLine.Substring(0, 2), 16);
                    address = Convert.ToInt32(hexLine.Substring(2, 4), 16);
                    recordType = Convert.ToInt32(hexLine.Substring(6, 2), 16);
                    data = hexLine.Substring(8, byteCount * 2);
                    for (int i = 0; i < byteCount; i++, index++)
                    {
                        if (recordType == 0) binImage[index] = Convert.ToByte(data.Substring(2 * i, 2), 16);
                    }
                    checksum = Convert.ToInt32(hexLine.Substring(hexLine.Length - 4, 2), 16);
                }
            }
            Array.Resize(ref binImage, index);
            return binImage;
        }
        static public string SetFuse(int hFuse, int lFuse)
        {
            sPort.WriteLine("3");
            sPort.WriteLine(hFuse.ToString());           //high fuse
            sPort.WriteLine(lFuse.ToString());           //low fuse
            return sPort.ReadLine();
        }
        static public string SetPageSize(uint pageSize)
        {
            sPort.WriteLine("2");
            sPort.WriteLine(pageSize.ToString());
            return sPort.ReadLine();
        }
        static public string SetProgSpeed(ProgrammingSpeed speed)
        {
            sPort.WriteLine("4");
            sPort.WriteLine(((int)speed).ToString());
            return sPort.ReadLine();
        }
        static public string EraseFlash()
        {
            sPort.WriteLine("5");
            return sPort.ReadLine();
        }
        static public string Connect(string port, int baud)
        {
            sPort = new SerialPort(port, baud);
            sPort.Open();
            return sPort.ReadLine();
        }
        static public string WriteFlash(byte[] binImage)
        {
            sPort.WriteLine("1");
            //Thread.Sleep(10);
            float progress = 0;
            int byteCount = binImage.Length;
            for (int i = 0; i < byteCount; i++)
            {
                sPort.WriteLine(binImage[i].ToString());
                Console.Clear();
                Console.WriteLine(progress++ / byteCount * 100 + "%");
            }

            sPort.WriteLine("500");
            return sPort.ReadLine();
        }
        static public string VerifyFlash(byte[] memoryImage)
        {
            int memoryIndex = 0;
            sPort.WriteLine("6");
            string temp = "";
            while(true)
            {
                temp = sPort.ReadLine();
                if (temp == "END") return "";
                if (Convert.ToInt32(temp) != memoryImage[memoryIndex])
                    Console.WriteLine("NOT OK @ " + memoryIndex + "[sent = " + memoryImage[memoryIndex] + "|| recv = " + Convert.ToInt32(temp) + "]");
                memoryIndex++;
            }
            return "";
        }
        static public string Disconnect()
        {
            sPort.WriteLine("255");
            return sPort.ReadLine();
        }
    }
}
