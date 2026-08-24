using System;
using System.IO;
using System.Reflection;

namespace JumpSpaceBalance.Setup
{
    internal class Program
    {
        static int Main(string[] args)
        {
            int res = 0;

            try
            {
                res = Install(args);
            }
            catch (Exception ex)
            {
                Log($"Failed with exception:\n{ex}");
                res = 255;
            }

#if DEBUG
            Console.Write($"Done with exit code {res}. Press Enter to exit... ");
            Console.ReadLine();
#endif

            return res;
        }

        private static int Install(string[] args)
        {
            if (args.Length != 1)
            {
                Log($"Received {args.Length} args, expected 1.");
                return 1;
            }

            if (!Directory.Exists(args[0]))
            {
                Log($"Directory \"{args[0]}\" doesn't exist.");
                return 2;
            }

            string root = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
            if (string.IsNullOrEmpty(root))
            {
                return 3;
            }

            return Install(root, args[0]);
        }

        private static int Install(string from, string to)
        {
            Log($"Installing from \"{from}\" to \"{to}\"");

            if (HasMelonLoader(to))
            {
                Log($"Melon Loader detected at \"{to}\"");

                MoveFile(Path.Combine(from, "Release"), to, "JumpSpaceBalance_readme.txt");
                MoveFile(Path.Combine(from, "Release", "Mods"), Path.Combine(to, "Mods"), "JumpSpaceBalance.Loader.dll");
                MoveFile(Path.Combine(from, "Release", "UserLibs"), Path.Combine(to, "UserLibs"), "JumpSpaceBalance.dll");
            }
            else
            {
                Log($"Melon Loader NOT found at \"{to}\"");

                MoveFile(Path.Combine(from, "Release_Standalone"), to, "JumpSpaceBalance_readme.txt");
                MoveFile(Path.Combine(from, "Release_Standalone"), to, "JumpSpaceBalance.dll", "winhttp.dll");
            }

            return 0;
        }

        private static void MoveFile(string fromDir, string toDir, string fromName, string toName = null)
        {
            if (!Directory.Exists(toDir))
            {
                Directory.CreateDirectory(toDir);
            }

            string fromPath = Path.Combine(fromDir, fromName);
            string toPath = Path.Combine(toDir, toName ?? fromName);
            
            if (File.Exists(toPath))
            {
                File.Delete(toPath);
            }

            File.Move(fromPath, toPath);
        }

        private static bool HasMelonLoader(string root)
        {
            if (Directory.Exists(Path.Combine(root, "Mods")) &&
                Directory.Exists(Path.Combine(root, "UserLibs")) &&
                File.Exists(Path.Combine(root, "version.dll")))
            {
                return true;
            }

            return false;
        }

        private static void Log(string message)
        {
#if DEBUG
            Console.WriteLine(message);
#endif
        }
    }
}
