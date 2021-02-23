#include "BodyTracking.h"
#include "UI\UIImage.h"

IMPLEMENT_CLASS_INFO(BodyTracking)


//cette render size est fixe (on a pas le droit de la retailler c'est la kinect qui décide)
#define RENDERSIZE  1920*1080
//! constructor
BodyTracking::BodyTracking(const kstl::string& name, CLASS_NAME_TREE_ARG) : CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
{
	
}

//! destructor
BodyTracking::~BodyTracking()
{

	if (m_pKinectSensor)
	{
		m_pKinectSensor->Close();
	}
	
	m_pKinectSensor->Release();
	m_pKinectSensor = NULL;

	if (m_pColorRGBX)
	{
		delete[] m_pColorRGBX;
		m_pColorRGBX = NULL;
	}
	if (m_pDepthRGBX)
	{
		delete[] m_pDepthRGBX;
		m_pDepthRGBX = NULL;
	}

	


}

void BodyTracking::InitKinect()
{
	HRESULT hr;
	hr = GetDefaultKinectSensor(&m_pKinectSensor);

	if (FAILED(hr))
	{
		printf("failed to get default sensor\n");
	}
	if (m_pKinectSensor)
	{
		hr = m_pKinectSensor->Open();


		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
		}
	}

	if (!m_pKinectSensor || FAILED(hr))
	{
		printf("No ready Kinect found!");
		return;
	}


	m_pKinectSensor->OpenMultiSourceFrameReader(
		FrameSourceTypes::FrameSourceTypes_Depth | FrameSourceTypes::FrameSourceTypes_Color | FrameSourceTypes::FrameSourceTypes_BodyIndex | FrameSourceTypes::FrameSourceTypes_Body,
		&m_pMultiSourceFrameReader);

	m_pColorRGBX = new RGBQUAD[RENDERSIZE];
	m_pDepthRGBX = new RGBQUAD[512*424];
}


BYTE* BodyTracking::getStream()
{	
	//accès à l'image 
	m_pMultiSourceFrame = NULL;
	IColorFrame* pColorFrame = NULL;
	HRESULT hr = m_pMultiSourceFrameReader->AcquireLatestFrame(&m_pMultiSourceFrame);
	if (SUCCEEDED(hr))
	{
		IColorFrameReference* pColorFrameReference = NULL;

		hr = m_pMultiSourceFrame->get_ColorFrameReference(&pColorFrameReference);
		if (SUCCEEDED(hr))
		{
			hr = pColorFrameReference->AcquireFrame(&pColorFrame);
		}

		pColorFrameReference->Release();
		pColorFrameReference=NULL;
	}

	if (SUCCEEDED(hr))
	{
		INT64 nTime = 0;
		IFrameDescription* pFrameDescription = NULL;
		int nWidth = 0;
		int nHeight = 0;
		ColorImageFormat imageFormat = ColorImageFormat_None;
		unsigned int nBufferSize = 0;
		RGBQUAD *pBuffer = NULL;

		hr = pColorFrame->get_RelativeTime(&nTime);

		if (SUCCEEDED(hr))
		{
			hr = pColorFrame->get_FrameDescription(&pFrameDescription);
		}

		if (SUCCEEDED(hr))
		{
			hr = pFrameDescription->get_Width(&nWidth);
		}

		if (SUCCEEDED(hr))
		{
			hr = pFrameDescription->get_Height(&nHeight);
		}

		if (SUCCEEDED(hr))
		{
			hr = pColorFrame->get_RawColorImageFormat(&imageFormat);
		}

		if (SUCCEEDED(hr))
		{
			if (imageFormat == ColorImageFormat_Bgra)
			{
				hr = pColorFrame->AccessRawUnderlyingBuffer(&nBufferSize, reinterpret_cast<BYTE**>(&pBuffer));
			}
			else if (m_pColorRGBX)
			{
				pBuffer = m_pColorRGBX;
				nBufferSize = RENDERSIZE * sizeof(RGBQUAD);
				hr = pColorFrame->CopyConvertedFrameDataToArray(nBufferSize, reinterpret_cast<BYTE*>(pBuffer), ColorImageFormat_Rgba);
			}
			else
			{
				hr = E_FAIL;
			}
		}

		if (SUCCEEDED(hr))
		{
			
			// Make sure we've received valid data
			if (pBuffer)
			{
				if (pColorFrame)
				{
					pColorFrame->Release();
					pColorFrame = NULL;
				}
				// Draw the data with Direct2D
				return reinterpret_cast<BYTE*>(pBuffer);
				
			}
		}

		pFrameDescription->Release();
		pFrameDescription = NULL;
	}

	if (pColorFrame)
	{
		pColorFrame->Release();
		pColorFrame = NULL;
	}
	return NULL;
}

BYTE* BodyTracking::getDepthStream()
{
	//accès à l'image 
	m_pMultiSourceFrame = NULL;
	IDepthFrame* pDepthFrame = NULL;
	HRESULT hr = m_pMultiSourceFrameReader->AcquireLatestFrame(&m_pMultiSourceFrame);
	if (SUCCEEDED(hr))
	{
		IDepthFrameReference* pDepthFrameReference = NULL;

		hr = m_pMultiSourceFrame->get_DepthFrameReference(&pDepthFrameReference);
		if (SUCCEEDED(hr))
		{
			hr = pDepthFrameReference->AcquireFrame(&pDepthFrame);
		}

		pDepthFrameReference->Release();
		pDepthFrameReference = NULL;
	}

	if (SUCCEEDED(hr))
	{
		INT64 nTime = 0;
		IFrameDescription* pFrameDescription = NULL;
		int nWidth = 0;
		int nHeight = 0;
		unsigned int nBufferSize = 0;
		UINT16 *pBuffer = NULL;
		unsigned short nDepthMinReliableDistance = 0;
		unsigned short nDepthMaxDistance = 0;

		hr = pDepthFrame->get_RelativeTime(&nTime);

		if (SUCCEEDED(hr))
		{
			hr = pDepthFrame->get_FrameDescription(&pFrameDescription);
		}

		if (SUCCEEDED(hr))
		{
			hr = pFrameDescription->get_Width(&nWidth);
		}

		if (SUCCEEDED(hr))
		{
			hr = pFrameDescription->get_Height(&nHeight);
		}

		if (SUCCEEDED(hr))
		{
			hr = pDepthFrame->get_DepthMinReliableDistance(&nDepthMinReliableDistance);
		}

		if (SUCCEEDED(hr))
		{
			//nDepthMaxDistance = 2048;
			hr = pDepthFrame->get_DepthMaxReliableDistance(&nDepthMaxDistance);
		}
		if (SUCCEEDED(hr))
		{
			hr = pDepthFrame->AccessUnderlyingBuffer(&nBufferSize, &pBuffer);
		}

		if (SUCCEEDED(hr))
		{

			// Make sure we've received valid data
			if (pBuffer)
			{
				
				RGBQUAD* pRGBX = m_pDepthRGBX;
				// end pixel is start + width*height - 1
				const UINT16* pBufferEnd = pBuffer + (nWidth * nHeight);

				while (pBuffer < pBufferEnd)
				{
					unsigned short depth = *pBuffer;

					// To convert to a byte, we're discarding the most-significant
					// rather than least-significant bits.
					// We're preserving detail, although the intensity will "wrap."
					// Values outside the reliable depth range are mapped to 0 (black).

					// Note: Using conditionals in this loop could degrade performance.
					// Consider using a lookup table instead when writing production code.
					float deltaDepth = 255.0f / ((float)(nDepthMaxDistance - nDepthMinReliableDistance));
					BYTE intensity = static_cast<BYTE>((depth >= nDepthMinReliableDistance) && (depth <= nDepthMaxDistance) ? ((float)(depth - nDepthMinReliableDistance))*deltaDepth : 0);

					pRGBX->rgbRed = intensity;
					pRGBX->rgbGreen = intensity;
					pRGBX->rgbBlue = intensity;
					pRGBX->rgbReserved = 255;

					++pRGBX;
					++pBuffer;
				}
				if (pDepthFrame)
				{
					pDepthFrame->Release();
					pDepthFrame = NULL;
				}
				if (pFrameDescription)
				{
					pFrameDescription->Release();
					pFrameDescription = NULL;
				}
				//Return the data 
				return reinterpret_cast<BYTE*>(m_pDepthRGBX);

			}
		}

		pFrameDescription->Release();
		pFrameDescription = NULL;
	}

	if (pDepthFrame)
	{
		pDepthFrame->Release();
		pDepthFrame = NULL;
	}
	return NULL;
}
//appelé dans un update
//jointscoord doit avoir une taille de 25 sinon ça marche pas
void BodyTracking::DetectBodies(int windowWidth, int windowHeight)
{
	
	IBodyFrame* pBodyFrame = NULL;

	if (m_pMultiSourceFrame==NULL)
		HRESULT hr = m_pMultiSourceFrameReader->AcquireLatestFrame(&m_pMultiSourceFrame);
	else{
		IBodyFrameReference* pBodyFrameReference = NULL;

		HRESULT hr = m_pMultiSourceFrame->get_BodyFrameReference(&pBodyFrameReference);
		if (SUCCEEDED(hr))
		{
			hr = pBodyFrameReference->AcquireFrame(&pBodyFrame);
		}

		pBodyFrameReference->Release();
		pBodyFrameReference = NULL;


		if (SUCCEEDED(hr))
		{
			INT64 nTime = 0;

			hr = pBodyFrame->get_RelativeTime(&nTime);

			IBody* ppBodies[BODY_COUNT] = { 0 };

			if (SUCCEEDED(hr))
			{
				hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
			}

			if (SUCCEEDED(hr))
			{
				for (int i = 0; i < BODY_COUNT; ++i)
				{
					//de base on a les six, on regarde s'il est tracked
					//le body se fait enregistrer dans un des six (au pif)
					IBody* pBody = ppBodies[i];
					if (pBody)
					{
						BOOLEAN bTracked = false;
						hr = pBody->get_IsTracked(&bTracked);

						if (SUCCEEDED(hr) && bTracked)
						{
							Joint joints[JointType_Count];
						//	Point2D jointsCoords[JointType_Count];

							hr = pBody->GetJoints(_countof(joints), joints);
							if (SUCCEEDED(hr))
							{
								for (int j = 0; j < _countof(joints); ++j)
								{
									// Calculate the body's position on the screen
									DepthSpacePoint depthPoint = { 0 };
									m_pCoordinateMapper->MapCameraPointToDepthSpace(joints[j].Position, &depthPoint);

									kfloat screenPointX = static_cast<kfloat>(depthPoint.X * windowWidth) / 512;
									kfloat screenPointY = static_cast<kfloat>(depthPoint.Y * windowHeight) / 424;

									//printf("coordonnées trouvées :   %f,   %f\n", screenPointX, screenPointY);
								
									jointsCoords[i][j] = Point2D(screenPointX, screenPointY);

								}
							}
								
						}
						else{
							jointsCoords[i][0] = Point2D(100000, 100000);
						}
						
					}
				}
			}

			for (int i = 0; i < _countof(ppBodies); ++i)
			{
				if (ppBodies[i])
				{
					ppBodies[i]->Release();
					ppBodies[i] = NULL;
				}
			}
		}

		if (pBodyFrame)
		{
			pBodyFrame->Release();
			pBodyFrame = NULL;
		}
	}
}
