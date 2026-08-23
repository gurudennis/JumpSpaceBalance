using System;
using System.IO;
using System.Reflection;

namespace JumpSpaceBalance.Setup
{
    internal class Program
    {
        static int Main(string[] args)
        {
            int res = Install(args);

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

            // ...

            return 0;
        }

        private static void Log(string message)
        {
#if DEBUG
            Console.WriteLine(message);
#endif
        }
    }
}
