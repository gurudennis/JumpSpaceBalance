namespace JumpSpaceBalance.Loader
{
    public class ModLoader : MelonLoader.MelonMod
    {
        public override void OnInitializeMelon()
        {
            string path = Path.Combine(MelonLoader.Utils.MelonEnvironment.UserLibsDirectory, "JumpSpaceBalance.dll");
            dll_ = MelonLoader.NativeLibrary.LoadLib(path);
        }

        public override void OnDeinitializeMelon()
        {
            try
            {
                if (dll_ != IntPtr.Zero)
                {
                    IntPtr symbolAddress = MelonLoader.NativeLibrary.GetExport(dll_, "JumpSpaceBalance_Finalize");
                    if (symbolAddress != IntPtr.Zero)
                    {
                        Action finalize = System.Runtime.InteropServices.Marshal.GetDelegateForFunctionPointer<Action>(symbolAddress);
                        if (finalize != null)
                        {
                            finalize();
                        }
                    }
                }
            } catch { }
        }

        private IntPtr dll_;
    }
}
