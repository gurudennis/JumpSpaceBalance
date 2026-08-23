using System;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text.RegularExpressions;

namespace PrepRelease
{
    internal class Program
    {
        static int Main(string[] args)
        {
            // Parse the desired version
            if (args == null || args.Length != 1 || !Version.TryParse(args[0], out Version version))
            {
                Console.WriteLine("Expected exactly one argument in version format, e.g. \"1.2.34.5\".");
                return 1;
            }

            // Find the root of the project
            string root = Path.GetDirectoryName(Path.GetDirectoryName(Path.GetDirectoryName(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location))));
            Console.WriteLine($"Detected project root to be \"{root}\"");

            // Update the version in AssemblyInfo.cs
            ReplaceVersionInFiles(root, "AssemblyInfo.cs", "Version\\(\"[0-9]+?\\.[0-9]+?\\.[0-9]+?\\.[0-9]+?\"\\)", $"Version(\"{version.ToString()}\")");

            // Update the version in JumpSpaceBalance.rc
            ReplaceVersionInFiles(root, "JumpSpaceBalance.rc", "VERSION [0-9]+,[0-9]+,[0-9]+,[0-9]+", $"VERSION {version.ToString().Replace('.', ',')}");
            ReplaceVersionInFiles(root, "JumpSpaceBalance.rc", "VALUE \"FileVersion\", \"[0-9]+.[0-9]+.[0-9]+.[0-9]+\"", $"VALUE \"FileVersion\", \"{version.ToString()}\"");
            ReplaceVersionInFiles(root, "JumpSpaceBalance.rc", "VALUE \"ProductVersion\", \"[0-9]+.[0-9]+.[0-9]+.[0-9]+\"", $"VALUE \"ProductVersion\", \"{version.ToString()}\"");

            return 0;
        }

        private static void ReplaceVersionInFiles(string root, string fileNameMask, string regex, string version)
        {
            string[] files = Directory.GetFiles(root, fileNameMask, SearchOption.AllDirectories);
            foreach (string filePath in files)
            {
                string content = File.ReadAllText(filePath);
                content = Regex.Replace(content, regex, version);
                File.WriteAllText(filePath, content);
                Console.WriteLine($"Patched version to {version} in \"{filePath}\"");
            }
        }
    }
}
