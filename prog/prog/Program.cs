/*
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
    class Program
    {
        
        static void Main(string[] args)
        {
            bool verify = true;
            string hexPath = "";
            string port = "";
            int baudRate = 115200;
            string speed = "";
            uint pageSize = 64;
            int lFuse = 98, hFuse = 223;
            foreach (string arg in args)
            {
                if (arg.Contains("--hex=")) hexPath = arg.Replace("--hex=", "").Replace("\"", "");
                if (arg.Contains("--port="))
                {
                    port = arg.Replace("--port=", "").Split(':')[0];
                    baudRate = Convert.ToInt32(arg.Replace("--port=", "").Split(':')[1]);
                }
                if (arg.Contains("--speed=")) speed = arg.Replace("--speed=", "");
                if (arg.Contains("--mcu="))
                {
                    if (arg.Replace("--mcu=", "") == "8") pageSize = 64;
                    else if (arg.Replace("--mcu=", "") == "168") pageSize = 128;
                    else pageSize = 256; //atmega328
                }
                if (arg.Contains("--hFuse=")) hFuse = Convert.ToInt32(arg.Replace("--hFuse=0x", ""), 16);
                if (arg.Contains("--lFuse=")) lFuse = Convert.ToInt32(arg.Replace("--lFuse=0x", ""), 16);
            }
            
            byte[] binImage = isp.Hex2Bin(hexPath);


            Console.WriteLine(isp.Connect(port, baudRate));
            isp.EraseFlash();
            Console.WriteLine(isp.SetProgSpeed(isp.ProgrammingSpeed.High));
            Console.WriteLine(isp.SetPageSize(pageSize));
            Console.WriteLine(isp.SetFuse(Convert.ToInt32("DF", 16), Convert.ToInt32("62", 16)));

            isp.WriteFlash(binImage);
            if (verify) isp.VerifyFlash(binImage);
            isp.Disconnect();
            Console.WriteLine("All Done");

            
            Console.ReadLine();
        }
    }
}
