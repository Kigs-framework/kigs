#include "ResourceDownloader.h"

IMPLEMENT_CLASS_INFO(ResourceDownloader)

void ResourceDownloader::InitModifiable()
{
	if ((std::string)myURL != "")
	{
		std::string parseUrl = myURL;

		std::string protocol = "";
		unsigned int port = 0;
		if (parseUrl.substr(0, 8) == "https://")
		{
			protocol = "HTTPS";
			port = 443;
			parseUrl = parseUrl.substr(8, parseUrl.length() - 8);
		}
		else if (parseUrl.substr(0, 7) == "http://")
		{
			protocol = "HTTP";
			port = 80;
			parseUrl = parseUrl.substr(7, parseUrl.length() - 7);
		}

		if (protocol == "")
		{
			KIGS_ERROR("ResourceDownloader Bad URL", 1);
			return;
		}

		size_t hostpos=parseUrl.find('/');
		if (hostpos == std::string::npos)
		{
			KIGS_ERROR("ResourceDownloader Bad URL", 1);
			return;
		}

		std::string hostname = parseUrl.substr(0, hostpos);
		std::string url= parseUrl.substr(hostpos, parseUrl.length()- hostpos);

		mConnect = KigsCore::GetInstanceOf(getName()+"Connect", "HTTPConnect");
		mConnect->setValue("HostName", hostname);
		mConnect->setValue("Type", protocol);
		mConnect->setValue("Port", port);
		mConnect->Init();

		mAnswer = mConnect->retreiveGetAsyncRequest(url.c_str(), "DownloadDone", this);
		mAnswer->Init();

		ParentClassType::InitModifiable();
	}
}



DEFINE_METHOD(ResourceDownloader, DownloadDone)
{
	void* result = nullptr;
	sender->getValue("ReceivedBuffer", result);
	CoreRawBuffer* r = nullptr;
	if (result)
	{
		r = (CoreRawBuffer*)result;
	}
	EmitSignal(Signals::onDownloadDone, r,this);
	return true;
}