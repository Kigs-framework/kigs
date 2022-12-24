#pragma once

#include <stdio.h>

namespace Kigs
{
	namespace Core
	{
		class StringTree
		{
		private:
			class Entry
			{
			public:
				Entry()
				{
					ref = 0;
					key = 0;
					father = 0;
					sons.clear();
					stop = false;
				}

				Entry(char k, Entry* f)
				{
					ref = 0;
					key = k;
					father = f;
					sons.clear();
					stop = false;
				}

				~Entry()
				{
					Clear();
				}

				// destroy all sons and set ref to 0
				void Clear()
				{
					std::map<const char, Entry*>::iterator it = sons.begin();
					std::map<const char, Entry*>::iterator end = sons.end();
					while (it != end)
					{
						delete it->second;
						++it;
					}
					sons.clear();
					ref = 0;
				}

				Entry* Son(char k)
				{
					std::map<const char, Entry*>::iterator it = sons.find(k);
					while (it != sons.end())
					{
						return it->second;
					}

					return NULL;
				}

				void AddSon(const char c, Entry* son)
				{
					sons.insert(std::pair<const char, Entry*>(c, son));
				}

				void RemoveSon(char c)
				{
					sons.erase(c);
				}

				void print(char* str, int size, bool printAll = false)
				{
					char* nStr;
					if (size >= 0)
					{
						nStr = new char[size + 2];
						strcpy(nStr, str);
						nStr[size] = key;
						nStr[size + 1] = 0;
						if (printAll)
							printf((stop) ? "%d %s stop\n" : "%d %s\n", ref, nStr);
						else if (stop)
							printf("%s\n", nStr);
					}
					else
					{
						nStr = new char[1];
						nStr[0] = 0;
					}

					std::map<const char, Entry*>::iterator it = sons.begin();
					std::map<const char, Entry*>::iterator end = sons.end();
					while (it != end)
					{
						it->second->print(nStr, size + 1, printAll);
						++it;
					}
					delete[] nStr;
				}

				char key;
				bool stop;
				int ref;
				Entry* father;
				std::map<const char, Entry*> sons;
			};

		public:
			StringTree()
			{
			}

			~StringTree()
			{
				Clear();
			}

			void Clear()
			{
				baseEntry.Clear();
			}

			void Insert(const char* filename)
			{
				char c = filename[0];
				if (c == 0)
					return;

				// list of letters used (for inc ref)
				std::vector<Entry*> letters;
				letters.clear();

				Entry* cLetter = &baseEntry;
				Entry* nLetter;

				int index = 0;
				c = filename[index];
				while (c != 0)
				{
					// treat first letter (create new mEntry if needed)
					nLetter = cLetter->Son(c);
					if (nLetter == NULL)
					{
						nLetter = new Entry(c, cLetter);
						cLetter->AddSon(c, nLetter);
					}

					// keep ref on letter
					letters.push_back(nLetter);

					cLetter = nLetter;
					c = filename[++index];
				}

				if (cLetter->stop == false)
				{
					// set the last character as a stop mEntry (a string has stopped here)
					cLetter->stop = true;

					// add ref for each letter
					std::vector<Entry*>::iterator it = letters.begin();
					std::vector<Entry*>::iterator end = letters.end();
					for (; it != end; ++it)
						(*it)->ref++;
					baseEntry.ref++;
				}
			}

			void Remove(const char* str)
			{
				if (str[0] == 0)
					return;

				// list of letters used (for inc ref)
				std::vector<Entry*> letters;
				letters.clear();

				Entry* cLetter = &baseEntry;

				int index = 0;
				while (str[index] != 0)
				{
					cLetter = cLetter->Son(str[index]);
					if (cLetter == NULL)
						return;

					letters.push_back(cLetter);
					index++;
				}

				// set the last character as a stop mEntry (a string has stopped here)
				if (cLetter->stop)
				{
					cLetter->stop = false;

					// add ref for each letter
					std::vector<Entry*>::iterator it = letters.begin();
					std::vector<Entry*>::iterator end = letters.end();
					for (; it != end; ++it)
					{
						if ((*it)->ref == 1)
						{
							(*it)->father->RemoveSon((*it)->key);
							delete (*it);
							return;
						}
						else
						{
							(*it)->ref--;
						}
					}
					baseEntry.ref--;
				}
			}

			bool HasString(const char* str)
			{
				if (str[0] == 0)
					return false;

				Entry* cLetter = &baseEntry;
				if (cLetter == NULL)
					return false;

				int index = 0;
				while (str[index] != 0)
				{
					cLetter = cLetter->Son(str[index]);
					if (cLetter == NULL)
						return false;

					index++;
				}

				return cLetter->stop;
				return false;
			}

			void print(bool printAll = false)
			{
				baseEntry.print("", -1, printAll);
			}

		private:
			Entry baseEntry;
		};
		// end file map struct

	}
}