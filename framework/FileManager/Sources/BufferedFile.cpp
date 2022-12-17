#include "PrecompiledHeaders.h"
#include "BufferedFile.h"
#include <stdio.h>

bool BufferedFile::searchNextWord(const std::string& toSearch)
{
	bool	found=false;
	bool	eofFound=false;
	size_t	searchLen=toSearch.length();
	int		foundlen=0;

	char*	read=&mFileContent[mFileCurrentOffset];

	while((!found) && (!eofFound))
	{
		if(mFileCurrentOffset<mFileSize)
		{
			if(*read == toSearch[foundlen])
			{
				foundlen++;
				if(foundlen==searchLen)
				{
					found=true;
				}
			}
			else
			{
				foundlen=0;
			}
			mFileCurrentOffset++;
			read++;
		}
		else
		{
			eofFound=true;
		}
	}	

	return found;
}

bool	BufferedFile::getNextFloat(float& flt)
{
	// go to next non white space
	bool	found=false;
	bool	eofFound=false;

	char*	read=&mFileContent[mFileCurrentOffset];

	while((!found) && (!eofFound))
	{
		if(mFileCurrentOffset<mFileSize)
		{
			if(*read != ' ')
			{
				found=true;
			}
			else
			{
				mFileCurrentOffset++;
				read++;
			}
		}
		else
		{
			eofFound=true;
		}
	}	
	if(found)
	{
		float readFloat=0.0f;
		if(sscanf(&mFileContent[mFileCurrentOffset],"%f",&readFloat) == 1)
		{
			flt=readFloat;

			// go to next white space
			found=false;
			while((!found) && (!eofFound))
			{
				if(mFileCurrentOffset<mFileSize)
				{
					if(*read == ' ')
					{
						found=true;
					}
					else
					{
						mFileCurrentOffset++;
						read++;
					}
				}
				else
				{
					eofFound=true;
				}
			}

			return true;
		}
	}
	return false;
}
