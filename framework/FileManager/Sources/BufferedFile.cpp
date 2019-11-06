#include "PrecompiledHeaders.h"
#include "BufferedFile.h"
#include <stdio.h>

bool BufferedFile::searchNextWord(const kstl::string& toSearch)
{
	bool	found=false;
	bool	eofFound=false;
	size_t	searchLen=toSearch.length();
	int		foundlen=0;

	char*	read=&myFileContent[myFileCurrentOffset];

	while((!found) && (!eofFound))
	{
		if(myFileCurrentOffset<myFileSize)
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
			myFileCurrentOffset++;
			read++;
		}
		else
		{
			eofFound=true;
		}
	}	

	return found;
}

bool	BufferedFile::getNextFloat(kfloat& flt)
{
	// go to next non white space
	bool	found=false;
	bool	eofFound=false;

	char*	read=&myFileContent[myFileCurrentOffset];

	while((!found) && (!eofFound))
	{
		if(myFileCurrentOffset<myFileSize)
		{
			if(*read != ' ')
			{
				found=true;
			}
			else
			{
				myFileCurrentOffset++;
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
		if(sscanf(&myFileContent[myFileCurrentOffset],"%f",&readFloat) == 1)
		{
			flt=readFloat;

			// go to next white space
			found=false;
			while((!found) && (!eofFound))
			{
				if(myFileCurrentOffset<myFileSize)
				{
					if(*read == ' ')
					{
						found=true;
					}
					else
					{
						myFileCurrentOffset++;
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
