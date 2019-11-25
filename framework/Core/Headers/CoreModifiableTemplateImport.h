#ifndef _COREMODIFIABLETEMPLATEIMPORT_H
#define _COREMODIFIABLETEMPLATEIMPORT_H


template<typename StringType>
CoreModifiable* CoreModifiable::Import(std::shared_ptr<XMLTemplate<StringType> > xmlfile, const std::string& filename, bool noInit, bool keepImportFileName, ImportState* state, const std::string& override_name)
{
	CoreModifiable* root = 0;

	if (xmlfile)
	{


		XMLNodeTemplate< StringType >* rootnode = (XMLNodeTemplate< StringType > * )xmlfile->getRoot();
		if (rootnode == nullptr)
		{
			kigsprintf("Unnable to load %s\n", filename.c_str());
			return nullptr;
		}

#ifdef KEEP_XML_DOCUMENT
		keepImportFileName = true;
#endif

		bool UTF8Enc = false;

		if (xmlfile->getEncoding() == "UTF-8" || xmlfile->getEncoding() == "utf-8")
		{
			UTF8Enc = true;
		}

		ImportState importState;
		importState.keepImportFileName = keepImportFileName;
		importState.noInit = noInit;
		importState.UTF8Enc = UTF8Enc;
		importState.override_name = override_name;
		importState.current_xml_file = xmlfile.get();
#ifdef KEEP_XML_DOCUMENT
		XMLBase* old_xml = nullptr;
		if (state)
		{
			old_xml = state->current_xml_file;
			state->current_xml_file = xmlfile.get();
		}
#endif

		if (state && override_name.size()) state->override_name = override_name;

		root = Import<StringType>(rootnode, 0, state ? *state : importState);

#ifdef KEEP_XML_DOCUMENT
		if (state) state->current_xml_file = old_xml;
#endif

#ifdef KEEP_XML_DOCUMENT
		root->mXMLFiles.push_back(xmlfile);
		root->mXMLNodes[xmlfile.get()] = rootnode;
#endif



		if (keepImportFileName && filename.size())
		{
			root->AddDynamicAttribute(STRING, "SeparatedFile", filename.c_str());
		}

		if (!state)
		{
			for (auto& toconnect : importState.toConnect)
			{
				CoreModifiable* pia = toconnect.currentNode;
				CoreModifiable* pib = toconnect.currentNode;

				if ((toconnect.sender != "this") && (toconnect.sender != "self")) // connection is son of a coremodifiable, one of the instance can be set at this or self
				{
					// else search instance in hierachy
					pia = SearchInstance(toconnect.sender, toconnect.currentNode);
				}

				if ((toconnect.receiver != "this") && (toconnect.receiver != "self")) // connection is son of a coremodifiable, one of the instance can be set at this or self
				{
					// else search instance in hierachy
					pib = SearchInstance(toconnect.receiver, toconnect.currentNode);
				}

				if (pia && pib)
				{
					if (toconnect.setValue.size())
					{
						KigsCore::Connect(pia, toconnect.signal, pib, toconnect.slot + std::to_string(KigsID(toconnect.setValue).toUInt()), [pib, id = toconnect.slot, value = toconnect.setValue]()
						{
							pib->setValue(id, value);
						});
					}
					else
						KigsCore::Connect(pia, toconnect.signal, pib, toconnect.slot CONNECT_PASS_MANAGED(toconnect));

				}
				else
				{
					STACK_STRING(str, 1024, "Cannot connect %s to %s", toconnect.sender.c_str(), toconnect.receiver.c_str());
					KIGS_WARNING(str, 3);
				}
			}
			ReleaseLoadedItems(importState.loadedItems);
		}
	}

	return root;
}



//! recusrive method to add sons and attributes found in the XML file
template<typename StringType>
CoreModifiable* CoreModifiable::Import(XMLNodeTemplate< StringType >* currentNode, CoreModifiable* currentModifiable, ImportState& importState)
{
	CoreModifiable* current = 0;

#ifdef KEEP_XML_DOCUMENT
	XMLAttributeTemplate< StringType >* autoUpdate = currentNode->getAttribute("AutoUpdate");
	XMLAttributeTemplate< StringType >* aggregate = currentNode->getAttribute("Aggregate");
#else
	XMLAttributeTemplate< StringType >* autoUpdate = currentNode->getAndRemoveAttribute("AutoUpdate");
	XMLAttributeTemplate< StringType >* aggregate = currentNode->getAndRemoveAttribute("Aggregate");
#endif
	bool needInit = true;
	bool do_not_add_as_son = false;
	bool reused_unique_instance = false;
	// add shorter syntax
	if (currentNode->nameOneOf("Instance", "Inst"))
	{
#ifdef KEEP_XML_DOCUMENT
		XMLAttributeTemplate< StringType >* NameAttribute = currentNode->getAttribute("N", "Name");
		XMLAttributeTemplate< StringType >* typeAttribute = currentNode->getAttribute("T", "Type");
		XMLAttributeTemplate< StringType >* pathAttribute = currentNode->getAttribute("P", "Path");
		XMLAttributeTemplate< StringType >* uniqueAttribute = currentNode->getAttribute("U", "Unique");
#else
		XMLAttributeTemplate< StringType >* NameAttribute = currentNode->getAndRemoveAttribute("N", "Name");
		XMLAttributeTemplate< StringType >* typeAttribute = currentNode->getAndRemoveAttribute("T", "Type");
		XMLAttributeTemplate< StringType >* pathAttribute = currentNode->getAndRemoveAttribute("P", "Path");
		XMLAttributeTemplate< StringType >* uniqueAttribute = currentNode->getAndRemoveAttribute("U", "Unique");
#endif

		bool is_unique = importState.is_include_unique || uniqueAttribute;
		importState.is_include_unique = false;

		KIGS_ASSERT(!(typeAttribute && pathAttribute));

		std::string name;
		if (NameAttribute)
		{
			name = NameAttribute->getString();
		}

		if (!importState.override_name.empty())
		{
			name = importState.override_name;
			importState.override_name = "";
		}

		if (typeAttribute)
		{
			// check that parent doesn't already have this node
			bool found = false;
			if (currentModifiable)
			{
				const std::vector<ModifiableItemStruct>& instances = currentModifiable->getItems();
				//std::vector<s32>	linklist=currentModifiable->getItemLinkTypes();
				std::vector<ModifiableItemStruct>::const_iterator itson;
				//std::vector<s32>::const_iterator	itsonlink=linklist.begin();
				for (itson = instances.begin(); itson != instances.end(); ++itson)
				{
					CoreModifiable* son = (*itson).myItem;
					if (son->getName() == name)
					{
						if (son->isSubType(typeAttribute->getString()))
						{
							found = true;
							needInit = false;
							current = son;
							break;
						}
					}
				}
			}

			if (!found)
			{
				if (is_unique)
				{
					current = GetFirstInstanceByName(typeAttribute->getString(), name, false, true);
					reused_unique_instance = current;
					//if (current) current->GetRef(); // already set by GetFirstInstanceByName
				}

				if (!current)
				{
					current = (CoreModifiable*)KigsCore::GetInstanceOf(name, typeAttribute->getString());
#ifdef KEEP_XML_DOCUMENT
					current->mXMLNodes[importState.current_xml_file] = currentNode;
#endif
				}
				//				KIGS_ASSERT(loadedItems.find(ID)==loadedItems.end()); // two object with same ID
				importState.loadedItems.push_back(current);
				// add a ref on all loaded items to avoid they are destructed during the loading
				current->GetRef();
			}
		}
		else if (pathAttribute)
		{
			importState.is_include_unique = uniqueAttribute;

			if constexpr (std::is_same_v<StringType, std::string_view>)
			{
				current = Import(std::string(pathAttribute->getString()), importState.noInit, importState.keepImportFileName, &importState, name);
			}
			else
			{
				current = Import(pathAttribute->getString(), importState.noInit, importState.keepImportFileName, &importState, name);
			}
			importState.is_include_unique = false;

			if (current)
			{
				if (importState.keepImportFileName)
				{
					if constexpr (std::is_same_v<StringType, std::string_view>)
					{
						current->AddDynamicAttribute(STRING, "SeparatedFile", std::string(pathAttribute->getString()).c_str());
					}
					else
					{
						current->AddDynamicAttribute(STRING, "SeparatedFile", pathAttribute->getString().c_str());
					}
				}

				needInit = false;
				importState.loadedItems.push_back(current);
				current->GetRef();

#ifdef KEEP_XML_DOCUMENT
				current->mXMLNodes[importState.current_xml_file] = currentNode;
#endif

			}
			else
			{
				kigsprintf("Import error Include %s failed\n", name.c_str());
			}
		}
		else
		{
			current = InitReference(currentNode, importState.loadedItems, name);
			needInit = false;
#ifdef KEEP_XML_DOCUMENT
			if (current) current->mXMLNodes[importState.current_xml_file] = currentNode;
#endif
		}
#ifndef KEEP_XML_DOCUMENT
		if (NameAttribute) delete NameAttribute;
		if (typeAttribute) delete typeAttribute;
		if (pathAttribute) delete pathAttribute;
		if (uniqueAttribute) delete uniqueAttribute;
#endif

	}
	//permet de rajouter un xml avec la syntaxe <Include Name="fichier.xml"/>
	else if (currentNode->getName() == "Include")
	{
#ifdef KEEP_XML_DOCUMENT
		XMLAttributeTemplate< StringType >* NameAttribute = currentNode->getAttribute("Name", "N");
		XMLAttributeTemplate< StringType >* PathAttribute = currentNode->getAttribute("Path", "P");
#else
		XMLAttributeTemplate< StringType >* NameAttribute = currentNode->getAndRemoveAttribute("Name", "N");
		XMLAttributeTemplate< StringType >* PathAttribute = currentNode->getAndRemoveAttribute("Path", "P");
#endif
		bool rename = NameAttribute && PathAttribute;

		if (NameAttribute)
		{
			std::string name = (std::string)NameAttribute->getString();
			std::string path = PathAttribute ? (std::string)PathAttribute->getString() : name;

			if (rename)
				importState.override_name = name;

			current = Import(path, importState.noInit, importState.keepImportFileName, &importState);

			if (current == 0)
			{
				kigsprintf("Import error Include %s failed\n", name.c_str());
			}
			else
			{
				/*if (rename)
				{
					current->setName(name);
				}*/
				if (importState.keepImportFileName)
				{
					current->AddDynamicAttribute(STRING, "SeparatedFile", path.c_str());
				}
				needInit = false;
				importState.loadedItems.push_back(current);
				current->GetRef();
#ifdef KEEP_XML_DOCUMENT
				current->mXMLNodes[importState.current_xml_file] = currentNode;
#endif
			}
		}
#ifndef KEEP_XML_DOCUMENT
		if (NameAttribute) delete NameAttribute;
		if (PathAttribute) delete PathAttribute;
#endif
	}
	else if (currentNode->nameOneOf("Ref", "Reference"))
	{
		XMLAttributeTemplate< StringType >* pathAttribute = currentNode->getAttribute("P", "Path");
		if (pathAttribute)
		{
			current = SearchInstance((std::string)pathAttribute->getString(), currentModifiable,true);
			if (current)
			{
				//current->GetRef(); ref already get by searchinstance
				needInit = false;
				importState.loadedItems.push_back(current);
#ifdef KEEP_XML_DOCUMENT
				current->mXMLNodes[importState.current_xml_file] = currentNode;
#endif
			}
		}
	}
	else if (currentNode->nameOneOf("Rel", "RelativePath"))
	{
		XMLAttributeTemplate< StringType >* pathAttribute = currentNode->getAttribute("P", "Path");
		if (pathAttribute)
		{
			current = SearchInstance((std::string)pathAttribute->getString(), currentModifiable,true);
			if (current)
			{
				//current->GetRef(); getref already done in searchinstance
				needInit = false;
				importState.loadedItems.push_back(current);
				do_not_add_as_son = true;
#ifdef KEEP_XML_DOCUMENT
				current->mXMLNodes[importState.current_xml_file] = currentNode;
#endif
			}
		}
	}

	if (current && !reused_unique_instance)
	{
		std::vector<XMLNodeBase*>	sons;
		sons.clear();
		ImportAttributes(currentNode, current, importState, sons);

		// manage shared instances
		CoreModifiable* shared = current->getSharedInstance();
		if (shared != current)
		{
			current->Destroy();
			current = shared;
		}

		ImportSons<StringType>(sons, current, importState);

		if (needInit && !importState.noInit)
		{
			current->Init();
		}
	}

	if (current && autoUpdate)
	{
		KigsCore::GetCoreApplication()->AddAutoUpdate(current);
	}

	if (current && currentModifiable)
	{
		// check if son need add
		bool sonNeedAdd = !do_not_add_as_son;

		const std::vector<ModifiableItemStruct>& instances = currentModifiable->getItems();
		std::vector<ModifiableItemStruct>::const_iterator itson;
		for (itson = instances.begin(); itson != instances.end(); ++itson)
		{
			CoreModifiable* son = (*itson).myItem;
			if (son == current)
			{
				sonNeedAdd = false;
				break;
			}
		}
		if (sonNeedAdd)
		{
			if (aggregate)
			{
				currentModifiable->aggregateWith(current);
			}
			else
			{
				currentModifiable->addItem(current);
			}
			if (!current->IsInit() && !importState.noInit)
			{
				current->Init();
			}
			current->Destroy();
		}
	}

#ifndef KEEP_XML_DOCUMENT
	if (autoUpdate)
		delete autoUpdate;

	if (aggregate)
		delete aggregate;
#endif

	return current;
}

template<typename StringType>
CoreModifiable* CoreModifiable::InitReference(XMLNodeTemplate< StringType >* currentNode, std::vector<CoreModifiable*>& loadedItems, const std::string& name)
{
	CoreModifiable* current = 0;
	XMLAttributeTemplate< StringType >* ref = currentNode->getAttribute("Ref", "Reference");

	if (ref)
	{
		// search modifiable by type, 1st among the already loaded items ...
		bool WasFoundInLoaded = false;

		std::vector<CoreModifiable*>::const_iterator Iter = loadedItems.begin();
		std::vector<CoreModifiable*>::const_iterator Iterend = loadedItems.end();
		KigsID type = ref->getString();
		while (Iter != Iterend)
		{
			CoreModifiable* pLoaded = *Iter;
			if (pLoaded->getName() == name && pLoaded->isSubType(type))
			{
				WasFoundInLoaded = true;
				current = pLoaded;
				current->GetRef();
				break;
			}
			++Iter;
		}


		if (!WasFoundInLoaded)
		{
			//... then into ALL the objects
			std::set<CoreModifiable*> instancelist;

			GetInstances(type, instancelist);

			std::set<CoreModifiable*>::iterator it;

			for (it = instancelist.begin(); it != instancelist.end(); ++it)
			{
				if ((*it)->getName() == name)
				{
					current = (CoreModifiable*)(*it);
					current->GetRef();
					break;
				}
			}
		}
	}
	return current;
}

template<typename StringType>
void 	CoreModifiable::ImportAttributes(XMLNodeTemplate<StringType>* node, CoreModifiable* currentModifiable, ImportState& importState, std::vector<XMLNodeBase*>& sons)
{
	for (s32 i = 0; i < node->getChildCount(); i++)
	{
		XMLNodeTemplate<StringType>* sonXML = node->getChildElement(i);
		if (sonXML->getType() == XML_NODE_ELEMENT)
		{

			if (sonXML->nameOneOf("Attr", "CoreModifiableAttribute"))
			{
				// init attribute
				InitAttribute(sonXML, currentModifiable, importState);
			}
			else if (sonXML->nameOneOf("CoreDecorator", "Deco"))
			{
				XMLAttributeTemplate<StringType>* attr = sonXML->getAttribute("N", "Name");
				KigsCore::DecorateInstance(currentModifiable, attr->getString());
			}
			else if (sonXML->nameOneOf("Lua", "LuaScript") || sonXML->nameOneOf("LUAScript", "LUA"))
			{
				// init attribute
				InitLuaScript(sonXML, currentModifiable, importState);
			}
			else if (sonXML->nameOneOf("Callback", "CB"))
			{
				// init attribute
				KIGS_WARNING("Use Connect instead of callback in the XML file !", 0);
				XMLAttributeTemplate<StringType> * slot = sonXML->getAttribute("Name");
				if (!slot) slot = sonXML->getAttribute("N");

				XMLAttributeTemplate<StringType>* signal = sonXML->getAttribute("Type");
				if (!signal) signal = sonXML->getAttribute("T");

				KigsCore::Connect(currentModifiable, signal->getString(), currentModifiable, slot->getString());
			}
			else if (sonXML->nameOneOf("OnEvent", "OnE"))
			{
				XMLAttributeTemplate<StringType>* NameAttribute = sonXML->getAttribute("N", "Name");
				XMLAttributeTemplate<StringType>* ActionAttribute = sonXML->getAttribute("A", "Action");

				if (NameAttribute && ActionAttribute)
				{

					std::string name;
					std::string action;

					name = NameAttribute->getString();
					action = ActionAttribute->getString();

					KigsCore::GetNotificationCenter()->addObserver(currentModifiable, action, name);

				}
			}
			else if (sonXML->nameOneOf("Connection", "Connect"))
			{
				XMLAttributeTemplate<StringType>* SiNameAttribute = sonXML->getAttribute("Si", "Signal");
				XMLAttributeTemplate<StringType>* InstAAttribute = sonXML->getAttribute("E", "Emitter");
				XMLAttributeTemplate<StringType>* SlNameAttribute = sonXML->getAttribute("Sl", "Slot");
				XMLAttributeTemplate<StringType>* InstBAttribute = sonXML->getAttribute("R", "Receiver");

				XMLAttributeTemplate<StringType>* SetValueAttribute = sonXML->getAttribute("V", "SetValue");

				if (SiNameAttribute && SlNameAttribute)
				{
					std::string signal = (std::string)SiNameAttribute->getString();
					std::string slot = (std::string)SlNameAttribute->getString();

					std::string ia = InstAAttribute ? (std::string)InstAAttribute->getString() : "this";
					std::string ib = InstBAttribute ? (std::string) InstBAttribute->getString() : "this";

					importState.toConnect.push_back(ImportState::ToConnect{ currentModifiable, ia, signal, ib, slot, SetValueAttribute ? (std::string)SetValueAttribute->getString() : "", sonXML });
				}
			}
			else if (sonXML->nameOneOf("RegisterTouchEvent", "REvent"))
			{
				// don't want to include ModuleInput and dependencies, so use call method

				CoreModifiable* theInputModule = KigsCore::GetModule("ModuleInput");

				XMLAttributeTemplate<StringType>* MethodNameAttribute = sonXML->getAttribute("M", "Method");
				XMLAttributeTemplate<StringType>* EventNameAttribute = sonXML->getAttribute("E", "Event");
				XMLAttributeTemplate<StringType>* FlagAttribute = sonXML->getAttribute("F", "Flag");

				if (MethodNameAttribute && EventNameAttribute)
				{
					u32 flag = 0;
					if (FlagAttribute)
					{
						flag = FlagAttribute->getInt();
					}

					theInputModule->SimpleCall("registerTouchEvent", currentModifiable, MethodNameAttribute->getString(), EventNameAttribute->getString(), flag);
				}
			}
			else
			{
				sons.push_back(sonXML);
			}
		}
		else if ((sonXML->getType() == XML_NODE_TEXT_NO_CHECK) || (sonXML->getType() == XML_NODE_TEXT))
		{
			currentModifiable->ImportFromCData((std::string)sonXML->getString());
		}
	}
}

template<typename StringType>
void CoreModifiable::ImportSons(const std::vector<XMLNodeBase*>& sons, CoreModifiable* currentModifiable, ImportState& importState)
{
	for (XMLNodeBase* sonXML : sons)
	{
		Import((XMLNodeTemplate< StringType>*)sonXML, currentModifiable, importState);
	}
}

template<typename StringType>
void	CoreModifiable::InitAttribute(XMLNodeTemplate<StringType>* currentNode, CoreModifiable* currentModifiable, ImportState& importState)
{

#ifdef KEEP_XML_DOCUMENT
	XMLAttributeTemplate<StringType>* pathAttribute = currentNode->getAttribute("P", "Path");
	XMLAttributeTemplate<StringType>* attrname = currentNode->getAttribute("N", "Name");
#else
	XMLAttributeTemplate<StringType>* pathAttribute = currentNode->getAndRemoveAttribute("P", "Path");
	XMLAttributeTemplate<StringType>* attrname = currentNode->getAndRemoveAttribute("N", "Name");
#endif

	if (pathAttribute)
	{
		currentModifiable = SearchInstance((std::string)pathAttribute->getString(), currentModifiable);
		if (!currentModifiable)
		{
#ifndef KEEP_XML_DOCUMENT
			if (pathAttribute) delete pathAttribute;
			if (attrname) delete attrname;
#endif

			KIGS_WARNING("Incorrect Path for attribute tag", 0);
			return;
		}
	}



	KigsID id = attrname->getString();

	CoreModifiableAttribute* attr = 0;
	bool attrfound = false;

	auto AttrByID = currentModifiable->_attributes.find(id);

	if (AttrByID != currentModifiable->_attributes.end())
	{
		attr = (*AttrByID).second;
		if (attr)
		{
			XMLAttributeTemplate<StringType>* attrvalue = currentNode->getAttribute("V", "Value");

			if (attrvalue)
			{
				std::string tempvalue = (std::string)attrvalue->getString();
				if (AttributeNeedEval(tempvalue))
				{
					EvalAttribute(tempvalue, currentModifiable, attr);
				}

				// for usstring, manage UTF-8
				if ((attr->getType() == USSTRING) && (importState.UTF8Enc))
				{
					attr->setValue((const UTF8Char*)tempvalue.c_str());
				}
				else
				{
					attr->setValue(tempvalue);
				}
			}
			else if ((attrvalue = currentNode->getAttribute("LUA", "L"), attrvalue))
			{
				std::string code = (std::string)attrvalue->getString();
				CoreModifiable* luamodule = KigsCore::GetModule("LuaKigsBindModule");
				if (luamodule)
				{
					luamodule->SimpleCall("SetValueLua", currentModifiable, attrname->getString(), code);
				}
			}
			else // check if value is in text or CDATA
			{
				for (s32 i = 0; i < currentNode->getChildCount(); i++)
				{
					XMLNodeTemplate< StringType>* sonXML = currentNode->getChildElement(i);
					if ((sonXML->getType() == XML_NODE_TEXT_NO_CHECK) || (sonXML->getType() == XML_NODE_TEXT))
					{
						std::string tempvalue = (std::string)sonXML->getString();
						if (AttributeNeedEval(tempvalue))
						{
							EvalAttribute(tempvalue, currentModifiable, attr);
						}

						if ((attr->getType() == USSTRING) && (importState.UTF8Enc))
						{
							attr->setValue((const UTF8Char*)tempvalue.c_str());
						}
						else
						{
							attr->setValue(tempvalue);
						}
						break;
					}
				}
			}
			attrfound = true;
		}
	}

	if (!attrfound)
	{
		XMLAttributeTemplate<StringType>* attrdynamic = currentNode->getAttribute("Dyn", "Dynamic");

		if (attrdynamic)
		{
			if ((attrdynamic->getString() == "true") || (attrdynamic->getString() == "yes"))
			{
				XMLAttributeTemplate<StringType>* attrtype = currentNode->getAttribute("T", "Type");
				XMLAttributeTemplate<StringType>* attrvalue = currentNode->getAttribute("V", "Value");

				if (attrvalue)
				{
					if (attrtype)
					{
						ATTRIBUTE_TYPE atype = CoreModifiableAttribute::stringToType((std::string)attrtype->getString());
						if (atype != UNKNOWN)
						{
							if ((atype != ARRAY) && (atype != REFERENCE))
							{
								attr = currentModifiable->AddDynamicAttribute(atype, (std::string)attrname->getString());
								std::string tempvalue = (std::string)attrvalue->getString();
								if (AttributeNeedEval(tempvalue))
								{
									EvalAttribute(tempvalue, currentModifiable, attr);
								}

								if ((attr->getType() == USSTRING) && (importState.UTF8Enc))
								{
									attr->setValue((const UTF8Char*)tempvalue.c_str());
								}
								else
								{
									attr->setValue(tempvalue);
								}

							}
							else
							{
								attr = currentModifiable->AddDynamicAttribute(atype, (std::string)attrname->getString(), ((std::string)attrvalue->getString()).c_str());
							}
						}
					}
				}
				else // check if value is in text or CData
				{
					for (s32 i = 0; i < currentNode->getChildCount(); i++)
					{
						XMLNodeTemplate< StringType>* sonXML = currentNode->getChildElement(i);
						if ((sonXML->getType() == XML_NODE_TEXT_NO_CHECK) || (sonXML->getType() == XML_NODE_TEXT))
						{
							ATTRIBUTE_TYPE atype = CoreModifiableAttribute::stringToType((std::string)attrtype->getString());
							if ((atype != ARRAY) && (atype != REFERENCE))
							{
								attr = currentModifiable->AddDynamicAttribute(atype, (std::string)attrname->getString());
								std::string tempvalue = (std::string)sonXML->getString();
								if (AttributeNeedEval(tempvalue))
								{
									EvalAttribute(tempvalue, currentModifiable, attr);
								}
								if ((attr->getType() == USSTRING) && (importState.UTF8Enc))
								{
									attr->setValue((const UTF8Char*)tempvalue.c_str());
								}
								else
								{
									attr->setValue(tempvalue);
								}
							}
							else
							{
								attr = currentModifiable->AddDynamicAttribute(atype, (std::string)attrname->getString(), ((std::string)sonXML->getString()).c_str());
							}
							break;
						}
					}
				}
			}
		}
	}

	// add attribute modifier if any
	if (attr)
	{
		for (s32 i = 0; i < currentNode->getChildCount(); i++)
		{
			XMLNodeTemplate< StringType>* sonXML = currentNode->getChildElement(i);
			if (sonXML->getType() == XML_NODE_ELEMENT)
			{
				if (sonXML->nameOneOf("Modifier", "Mod"))
				{
					InitAttributeModifier(sonXML, attr);
				}
			}
		}
	}

#ifndef KEEP_XML_DOCUMENT
	if (pathAttribute) delete pathAttribute;
	if (attrname) delete attrname;
#endif
}

template<typename StringType>
void	CoreModifiable::InitLuaScript(XMLNodeTemplate< StringType>* currentNode, CoreModifiable* currentModifiable, ImportState& importState)
{
	CoreModifiable* luamodule = KigsCore::GetModule("LuaKigsBindModule");
	if (!luamodule)
		return;

	XMLAttributeTemplate<StringType>* attrname = currentNode->getAttribute("N", "Name");

	if (!attrname)
		return;

	XMLAttributeTemplate<StringType>* attrtype = currentNode->getAttribute("T", "Type");
	XMLAttributeTemplate<StringType>* attrvalue = currentNode->getAttribute("V", "Value");

	std::string code = "";
	if (attrvalue)
	{
		code = (std::string)attrvalue->getString();
		if (code.size() && code[0] == '#')
		{
			code.erase(code.begin());

			u64 size;
			CoreRawBuffer* rawbuffer = ModuleFileManager::LoadFileAsCharString(code.c_str(), size);
			if (rawbuffer)
			{
				code = rawbuffer->buffer();
				rawbuffer->Destroy();
			}
			else
			{
				STACK_STRING(errstr, 1024, "Cannot load LUA script : %s", code.c_str());
				KIGS_ERROR(errstr, 3);
			}
		}
	}
	else
	{
		if (currentNode->getChildCount())
		{
			for (s32 i = 0; i < currentNode->getChildCount(); i++)
			{
				XMLNodeTemplate< StringType>* sonXML = currentNode->getChildElement(i);
				if ((sonXML->getType() == XML_NODE_TEXT_NO_CHECK) || (sonXML->getType() == XML_NODE_TEXT))
				{
					code = (std::string)sonXML->getString();
					break;
				}
			}
		}
	}


	std::vector<CoreModifiableAttribute*> params;
	maString pName("pName", (std::string)attrname->getString());
	maString pCode("pCode", code);
	maRawPtr pXML("pXML", currentNode);

	params.push_back(&pName);
	params.push_back(&pCode);
	params.push_back(&pXML);

	maReference localthis("pThis", { "" });
	localthis = currentModifiable;
	params.push_back(&localthis);

	maString cbType("cbType", attrtype ? (std::string)attrtype->getString() : "");
	if (attrtype)
		params.push_back(&cbType);

	luamodule->CallMethod("RegisterLuaMethod", params);
}

template<typename StringType>
AttachedModifierBase* CoreModifiable::InitAttributeModifier(XMLNodeTemplate< StringType>* currentNode, CoreModifiableAttribute* attr)
{
	XMLAttributeTemplate<StringType>* attrtype = currentNode->getAttribute("T", "Type");

	AttachedModifierBase* toAdd = 0;
	if (attrtype)
	{
		std::string modifiertype = (std::string)attrtype->getString();
		if (modifiertype != "")
		{
			auto& instanceMap = KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap();
			auto itfound = instanceMap.find(modifiertype);
			if (itfound != instanceMap.end())
			{
				toAdd = (AttachedModifierBase*)(*itfound).second();
			}
		}

		if (toAdd != 0)
		{
			// is setter ?
			bool isSetter = false;
			XMLAttributeTemplate<StringType>* attrsetter = currentNode->getAttribute("Setter", "isSetter");

			if (attrsetter)
			{
				if ((attrsetter->getString() == "true") || (attrsetter->getString() == "yes"))
				{
					isSetter = true;
				}
			}

			// search value
			std::string value = "";
			XMLAttributeTemplate<StringType>* attrvalue = currentNode->getAttribute("V", "Value");

			if (attrvalue)
			{
				value = (std::string)attrvalue->getString();
			}

			// check for direct string
			if (value == "")
			{
				for (s32 i = 0; i < currentNode->getChildCount(); i++)
				{
					XMLNodeTemplate< StringType>* sonXML = currentNode->getChildElement(i);
					if ((sonXML->getType() == XML_NODE_TEXT_NO_CHECK) || (sonXML->getType() == XML_NODE_TEXT))
					{
						value = sonXML->getString();
						break;
					}
				}
			}

			toAdd->Init(attr, !isSetter, value);
			attr->attachModifier(toAdd);
		}
	}

	return toAdd;
}


#endif //_COREMODIFIABLETEMPLATEIMPORT_H
