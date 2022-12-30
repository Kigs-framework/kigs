#pragma once

// base class
#include "TinyImage.h"

namespace Kigs
{

	namespace Pict
	{
		// ****************************************
		// * ETCClass class
		// * --------------------------------------
		/**
		* \file	ETCClass.h
		* \class	ETCClass
		* \ingroup TinyImageModule
		* \brief TinyImage specialized for ETC management.
		*
		*/
		// ****************************************
		class ETCClass : public TinyImage
		{
		public:
			/**
			* constructor
			* \param filename  the name of the file to load
			*/
			ETCClass(File::FileHandle* fileName);

			/// destructor
			virtual ~ETCClass();

		protected:
			virtual void	Export(const char* filename)
			{
				printf("can not export to ETC\n");
			}


			/**
			* load image from file
			* \param filename the name of the file to load
			*/
			virtual bool	Load(File::FileHandle* fileName);

			struct ETC_Header
			{
				unsigned int	sizex;	// if sizex&0xFFFF0000	=> content size
				unsigned int	sizey;  // if sizey&0xFFFF0000	=> content size
				unsigned int	format;
				unsigned int	datasize;
			};

		};
	}
}

