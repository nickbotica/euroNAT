// euroNatData.h

#pragma once

using namespace System;
using namespace System::Text;
using namespace System::Configuration;
using namespace System::Net;
using namespace System::Net::Security;
using namespace System::IO;
using namespace System::Security::Cryptography::X509Certificates;
using namespace System::Runtime::InteropServices;


namespace euroNatData {

	public ref class euroNatData
	{
	private:
		static bool ValidateRemoteCertificate( Object^ sender,  X509Certificate^ certificate, X509Chain^ chain, SslPolicyErrors policyErrors ) {
            return true; // Allow bad SSL certs
        }
	public:
		euroNatData(){}

		static char * GetNatData(){
            HttpWebRequest^ http = dynamic_cast< HttpWebRequest^ >( WebRequest::Create( "https://www.notams.jcs.mil/common/nat.html" ) );
            http->AllowAutoRedirect = true;
            http->Method = "GET";

            ServicePointManager::ServerCertificateValidationCallback = gcnew RemoteCertificateValidationCallback( ValidateRemoteCertificate );

            HttpWebResponse^ response = dynamic_cast< HttpWebResponse^ >(http->GetResponse());
            Stream^ stream = response->GetResponseStream();

            StreamReader^ reader = gcnew StreamReader( stream );

            String^ ot = reader->ReadToEnd();

            stream->Close();
            response->Close();

			return (char*)(void*)Marshal::StringToHGlobalAnsi(ot);
		}

	};
}
