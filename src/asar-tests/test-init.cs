using System;
using System.IO;
using AsarCLR;

public static class Test
{
	public static int Main(string[] args){
		if(args.Length != 1)
		{
			Console.WriteLine("Usage: test-???.exe [path_to_tests_directory]");
			return 1;
		}

		if(!Asar.init())
		{
			Console.WriteLine("Unable to Initialize");
			return 1;
		}

		Asar.close();

		if(!Asar.init())
		{
			Console.WriteLine("Unable to Reinitialize");
			return 1;
		}

		Asar.close();

		return 0;
	}
}
