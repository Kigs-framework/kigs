#ifndef _COREMODIFIABLETEMPLATEIMPORT_H
#define _COREMODIFIABLETEMPLATEIMPORT_H


template<typename StringType>
CMSP CoreModifiable::Import(std::shared_ptr<XMLTemplate<StringType> > xmlfile, const std::string& filename, bool noInit, bool keepImportFileName, ImportState* state, const std::string& override_name)
{
	CMSP root = nullptr;

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
			root->AddDynamicAttribute(ATTRIBUTE_TYPE::STRING, "SeparatedFile", filename.c_str());
		}

		if (!state)
		{
			// connect everything
			for (auto& toconnect : importState.toConnect)
			{
				CoreModifiable* pia = toconnect.currentNode;
				CoreModifiable* pib = toconnect.currentNode;

				if ((toconnect.sender != "this") && (toconnect.sender != "self")) // connection is son of a coremodifiable, one of the instance can be set at this or self
				{
					// else search instance in hierachy
					pia = SearchInstance(toconnect.sender, toconnect.currentNode).get();
				}

				if ((toconnect.receiver != "this") && (toconnect.receiver != "self")) // connection is son of a coremodifiable, one of the instance can be set at this or self
				{
					// else search instance in hierachy
					pib = SearchInstance(toconnect.receiver, toconnect.currentNode).get();
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
			// call methods
			if (importState.toCall.size())
			{
				for (auto& tocall : importState.toCall)
				{
					ManageToCall(tocall);
				}
			}
			ReleaseLoadedItems(importState.loadedItems);
		}
	}

	return root;
}



//! recusrive method to add sons and attributes found in the XML file
template<typename StringType>
CMSP CoreModifiable::Import(XMLNodeTemplate< StringType >* currentNode, CoreModifiable* currentModifiable, ImportState& importState)
{
	CMSP current = nullptr;

#ifdef KEEP_XML_DOCUMENT
	XMLAttributeTemplate< StringType >* autoUpdate = static_cast<XMLAttributeTemplate< StringType > * >(currentNode->getAttribute("AutoUpdate"));
	XMLAttributeTemplate< StringType >* aggregate = static_cast<XMLAttributeTemplate< StringType >*>(currentNode->getAttribute("Aggregate"));
#else
	XMLAttributeTemplate< StringType >* autoUpdate = static_cast<XMLAttributeTemplate< StringType >*>(currentNode->getAndRemoveAttribute("AutoUpdate"));
	XMLAttributeTemplate< StringType >* aggregate = static_cast<XMLAttributeTemplate< StringType >*>(currentNode->getAndRemoveAttribute("Aggregate"));
#endif
	bool needInit = true;
	bool do_not_add_as_son = false;
	bool reused_unique_instance = false;
	// add shorter syntax
	if (currentNode->nameOneOf("Instance", "Inst"))
	{
#ifdef KEEP_XML_DOCUMENT
		XMLAttributeTemplate< StringType >* NameAttribute = static_cast<XMLAttributeTemplate< StringType >*>(currentNode->getAttribute("N", "Name"));
		XMLAttributeTemplate< StringType >* typeAttribute = static_cast<XMLAttributeTemplate< StringType >*>(currentNode->getAttribute("T", "Type"));
		XMLAttributeTemplate< StringType >* pathAttribute = static_cast<XMLAttributeTemplate< StringType >*>(currentNode->getAttribute("P", "Path"));
		XMLAttributeTemplate< StringType >* uniqueAttribute = static_cast<XMLAttributeTemplate< StringType >*>(currentNode->getAttribute("U", "Unique"));
#else
		XMLAttributeTemplate< StringType >* NameAttribute = static_cast<XMLAttributeTemplate< StringType >*>(currentNode->getAndRemoveAttribute("N", "Name"));
		XMLAttributeTemplate< StringType >* typeAttribute = static_cast<XMLAttributeTemplate< StringType >*>(currentNode->getAndRemoveAttribute("T", "Type"));
		XMLAttributeTemplate< StringType >* pathAttribute = static_cast<XMLAttributeTemplate< StringType >*>(currentNode->getAndRemoveAttribute("P", "Path"));
		XMLAttributeTemplate< StringType >* uniqueAttribute = static_cast<XMLAttributeTemplate< StringType >*>(currentNode->getAndRemoveAttribute("U", "Unique"));
#endif

		bool is_unique = importState.is_include_unique || uniqueAttribute;
		importState.is_include_unique = false;

		KIGS_ASSERT(!(typeAttribute && pathAttribute));

		std::string name;
		if (NameAttribute)
		{
			name = NameAttribute->XMLAttributeBase::getString();
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
					CMSP son = (*itson).mItem;
					if (son->getName() == name)
					{
						if (son->isSubType(typeAttribute->XMLAttributeBase::getString()))
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
					current = GetFirstInstanceByName(typeAttribute->XMLAttributeBase::getString(), name, false);
					reused_unique_instance = current;
					//if (current) current->GetRef(); // already set by GetFirstInstanceByName
				}

				if (!current)
				{
					current = KigsCore::GetInstanceOf(name, typeAttribute->XMLAttributeBase::getString());
#ifdef KEEP_XML_DOCUMENT
					current->mXMLNodes[importState.current_xml_file] = currentNode;
#endif
				}
				//				KIGS_ASSERT(loadedItems.find(ID)==loadedItems.end()); // two object with same ID
				importState.loadedItems.push_back(current);
			}
		}
		else if (pathAttribute)
		{
			importState.is_include_unique = uniqueAttribute;

			if constexpr (std::is_same_v<StringType, std::string_view>)
			{
				current = Import(std::string(pathAttribute->XMLAttributeBase::getString()), importState.noInit, importState.keepImportFileName, &importState, name);
			}
			else
			{
				current = Import(pathAttribute->XMLAttributeBase::getString(), importState.noInit, importState.keepImportFileName, &importState, name);
			}
			importState.is_include_unique = false;

			if (current)
			{
				if (importState.keepImportFileName)
				{
					if constexpr (std::is_same_v<StringType, std::string_view>)
					{
						current->AddDynamicAttribute(ATTRIBUTE_TYPE::STRING, "SeparatedFile", std::string(pathAttribute->XMLAttributeBase::getString()).c_str());
					}
					else
					{
						current->AddDynamicAttribute(ATTRIBUTE_TYPE::STRING, "SeparatedFile", pathAttribute->XMLAttributeBase::getString().c_str());
					}
				}

				needInit = false;
				importState.loadedItems.push_back(current);
				
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
	else if (currentNode->compareName("Include"))
	{
#ifdef KEEP_XML_DOCUMENT
		XMLAttributeTemplate< StringType >* NameAttribute = static_cast<XMLAttributeTemplate< StringType >*>(currentNode->getAttribute("Name", "N"));
		XMLAttributeTemplate< StringType >* PathAttribute = static_cast<XMLAttributeTemplate< StringType >*>(currentNode->getAttribute("Path", "P"));
#else
		XMLAttributeTemplate< StringType >* NameAttribute = static_cast<XMLAttributeTemplate< StringType >*>(currentNode->getAndRemoveAttribute("Name", "N"));
		XMLAttributeTemplate< StringType >* PathAttribute = static_cast<XMLAttributeTemplate< StringType >*>(currentNode->getAndRemoveAttribute("Path", "P"));
#endif
		bool rename = NameAttribute && PathAttribute;

		if (NameAttribute)
		{
			std::string name = (std::string)NameAttribute->XMLAttributeBase::getString();
			std::string path = PathAttribute ? (std::string)PathAttribute->XMLAttributeBase::getString() : name;

			if (rename)
				importState.override_name = name;

			current = Import(path, importState.noInit, importState.keepImportFileName, &importState);

			if (current == nullptr)
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
					current->AddDynamicAttribute(ATTRIBUTE_TYPE::STRING, "SeparatedFile", path.c_str());
				}
				needInit = false;
				importState.loadedItems.push_back(current);
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
		XMLAttributeTemplate< StringType >* DontAdd = static_cast<XMLAttributeTemplate< StringType >*>(currentNode->getAttribute("DontAddAsSon"));
		XMLAttributeTemplate< StringType >* pathAttribute = static_cast<XMLAttributeTemplate< StringType >*>(currentNode->getAttribute("P", "Path"));
		if (pathAttribute)
		{
			current = SearchInstance((std::string)pathAttribute->XMLAttributeBase::getString(), currentModifiable);
			if (current)
			{
				//current->GetRef(); ref already get by searchinstance
				needInit = false;
				importState.loadedItems.push_back(current);

				if (DontAdd) // just here to be changed during loading, not to be added
				{
					do_not_add_as_son = true;
				}
#ifdef KEEP_XML_DOCUMENT
				current->mXMLNodes[importState.current_xml_file] = currentNode;
#endif
			}
		}
	}
	else if (currentNode->nameOneOf("Rel", "RelativePath"))
	{
		XMLAttributeTemplate< StringType >* PathAttribute = static_cast<XMLAttributeTemplate< StringType >*>(currentNode->getAttribute("P", "Path"));
		XMLAttributeTemplate< StringType >* NameAttribute = static_cast<XMLAttributeTemplate< StringType >*>(currentNode->getAttribute("Name", "N"));
		bool rename = NameAttribute && PathAttribute;
		if (PathAttribute)
		{

			current = SearchInstance((std::string)PathAttribute->XMLAttributeBase::getString(), currentModifiable);
			if (current)
			{
				if (rename)
				{
					std::string name = (std::string)NameAttribute->XMLAttributeBase::getString();
					current->setName(name);
				}

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
		// import upgradors first
		ImportUpgradors<StringType>(currentNode, current.get(), importState);
		std::vector<XMLNodeBase*>	sons;
		sons.clear();


		ImportAttributes<StringType>(currentNode, current.get(), importState, sons);

		// manage shared instances
		CMSP shared = current->getSharedInstance();
		if (shared != current)
		{
			current = shared;
		}

		ImportSons<StringType>(sons, current.get(), importState);

		if (needInit && !importState.noInit)
		{
			current->Init();
		}
	}

	if (current && autoUpdate)
	{
		KigsCore::GetCoreApplication()->AddAutoUpdate(current.get());
	}

	if (current && currentModifiable)
	{
		// check if son need add
		bool sonNeedAdd = !do_not_add_as_son;

		const std::vector<ModifiableItemStruct>& instances = currentModifiable->getItems();
		std::vector<ModifiableItemStruct>::const_iterator itson;
		for (itson = instances.begin(); itson != instances.end(); ++itson)
		{
			CMSP son = (*itson).mItem;
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
CMSP CoreModifiable::InitReference(XMLNodeTemplate< StringType >* currentNode, std::vector<CMSP>& loadedItems, const std::string& name)
{
	CMSP current = nullptr;
	XMLAttributeTemplate< StringType >* ref = static_cast<XMLAttributeTemplate< StringType > * >(currentNode->getAttribute("Ref", "Reference"));

	if (ref)
	{
		// search modifiable by type, 1st among the already loaded items ...
		bool WasFoundInLoaded = false;

		std::vector<CMSP>::const_iterator Iter = loadedItems.begin();
		std::vector<CMSP>::const_iterator Iterend = loadedItems.end();
		KigsID type = ref->XMLAttributeBase::getString();
		while (Iter != Iterend)
		{
			const CMSP& pLoaded = *Iter;
			if (pLoaded->getName() == name && pLoaded->isSubType(type))
			{
				WasFoundInLoaded = true;
				current = pLoaded;
				break;
			}
			++Iter;
		}


		if (!WasFoundInLoaded)
		{
			//... then into ALL the objects
			std::vector<CMSP> instancelist=		GetInstances(type);

			std::vector<CMSP>::const_iterator it;

			for (it = instancelist.begin(); it != instancelist.end(); ++it)
			{
				if ((*it)->getName() == name)
				{
					current = (*it);
					break;
				}
			}
		}
	}
	return current;
}

template<typename StringType>
void 	CoreModifiable::ImportUpgradors(XMLNodeTemplate<StringType>* node, CoreModifiable* currentModifiable, ImportState& importState)
{
	for (s32 i = 0; i < node->getChildCount(); i++)
	{
		XMLNodeTemplate<StringType>* sonXML = static_cast<XMLNodeTemplate<StringType> *>(node->getChildElement(i));
		if (sonXML->getType() == XML_NODE_ELEMENT)
		{
			if (sonXML->compareName("Upgrd"))
			{
				XMLAttributeBase* attr = sonXML->getAttribute("N", "Name");
				std::string name(attr->getString());
				currentModifiable->Upgrade(name);
			}
		}
	}
}

template<typename StringType>
void 	CoreModifiable::ImportAttributes(XMLNodeBase* node, CoreModifiable* currentModifiable, ImportState& importState, std::vector<XMLNodeBase*>& sons)
{
	for (s32 i = 0; i < node->getChildCount(); i++)
	{
		XMLNodeTemplate<StringType>* sonXML = static_cast<XMLNodeTemplate<StringType>*>(node->getChildElement(i));
		if (sonXML->getType() == XML_NODE_ELEMENT)
		{

			if (sonXML->nameOneOf("Attr", "CoreModifiableAttribute"))
			{
				// init attribute
				InitAttribute(sonXML, currentModifiable, importState);
			}
			else if (sonXML->nameOneOf("CoreDecorator", "Deco"))
			{
				XMLAttributeTemplate<StringType>* attr = static_cast<XMLAttributeTemplate<StringType> *>(sonXML->getAttribute("N", "Name"));
				KigsCore::DecorateInstance(currentModifiable, attr->XMLAttributeBase::getString());
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
				XMLAttributeTemplate<StringType> * slot = static_cast<XMLAttributeTemplate<StringType>*>(sonXML->getAttribute("Name"));
				if (!slot) slot = static_cast<XMLAttributeTemplate<StringType>*>(sonXML->getAttribute("N"));

				XMLAttributeTemplate<StringType>* signal = static_cast<XMLAttributeTemplate<StringType>*>(sonXML->getAttribute("Type"));
				if (!signal) signal = static_cast<XMLAttributeTemplate<StringType>*>(sonXML->getAttribute("T"));

				KigsCore::Connect(currentModifiable, signal->XMLAttributeBase::getString(), currentModifiable, slot->XMLAttributeBase::getString());
			}
			else if (sonXML->nameOneOf("OnEvent", "OnE"))
			{
				XMLAttributeTemplate<StringType>* NameAttribute = static_cast<XMLAttributeTemplate<StringType>*>(sonXML->getAttribute("N", "Name"));
				XMLAttributeTemplate<StringType>* ActionAttribute = static_cast<XMLAttributeTemplate<StringType>*>(sonXML->getAttribute("A", "Action"));

				if (NameAttribute && ActionAttribute)
				{

					std::string name;
					std::string action;

					name = NameAttribute->XMLAttributeBase::getString();
					action = ActionAttribute->XMLAttributeBase::getString();

					KigsCore::GetNotificationCenter()->addObserver(currentModifiable, action, name);

				}
			}
			else if (sonXML->nameOneOf("Connection", "Connect"))
			{
				XMLAttributeTemplate<StringType>* SiNameAttribute = static_cast<XMLAttributeTemplate<StringType>*>(sonXML->getAttribute("Si", "Signal"));
				XMLAttributeTemplate<StringType>* InstAAttribute = static_cast<XMLAttributeTemplate<StringType>*>(sonXML->getAttribute("E", "Emitter"));
				XMLAttributeTemplate<StringType>* SlNameAttribute = static_cast<XMLAttributeTemplate<StringType>*>(sonXML->getAttribute("Sl", "Slot"));
				XMLAttributeTemplate<StringType>* InstBAttribute = static_cast<XMLAttributeTemplate<StringType>*>(sonXML->getAttribute("R", "Receiver"));

				XMLAttributeTemplate<StringType>* SetValueAttribute = static_cast<XMLAttributeTemplate<StringType>*>(sonXML->getAttribute("V", "SetValue"));

				if (SiNameAttribute && SlNameAttribute)
				{
					std::string signal = (std::string)SiNameAttribute->XMLAttributeBase::getString();
					std::string slot = (std::string)SlNameAttribute->XMLAttributeBase::getString();

					std::string ia = InstAAttribute ? (std::string)InstAAttribute->XMLAttributeBase::getString() : "this";
					std::string ib = InstBAttribute ? (std::string) InstBAttribute->XMLAttributeBase::getString() : "this";

					importState.toConnect.push_back(ImportState::ToConnect{ currentModifiable, ia, signal, ib, slot, SetValueAttribute ? (std::string)SetValueAttribute->XMLAttributeBase::getString() : "", sonXML });
				}
			}
			else if (sonXML->XMLNodeBase::getName()=="Call")
			{
				XMLAttributeTemplate<StringType>* ParamList = static_cast<XMLAttributeTemplate<StringType>*>(sonXML->getAttribute("P","Params"));
				XMLAttributeTemplate<StringType>* MethodName = static_cast<XMLAttributeTemplate<StringType>*>(sonXML->getAttribute("N", "Name"));
				if (MethodName)
				{
					std::string plist = "";
					if (ParamList) // if param attribute found
					{
						plist = (std::string)ParamList->XMLAttributeBase::getString();
					}
					else // check if text son node exists
					{
						if (sonXML->getChildCount() == 1)
						{
							XMLNodeTemplate<StringType>* textSon = static_cast<XMLNodeTemplate<StringType>*>(sonXML->getChildElement(0));
							if ((textSon->getType() == XML_NODE_TEXT_NO_CHECK) || (textSon->getType() == XML_NODE_TEXT))
							{
								plist = (std::string)textSon->XMLNodeBase::getString();
							}
						}
					}
					
					importState.toCall.push_back(ImportState::ToCall{ currentModifiable, (std::string)MethodName->XMLAttributeBase::getString() ,plist });
				}
			}
			else if (sonXML->nameOneOf("RegisterTouchEvent", "REvent"))
			{
				// don't want to include ModuleInput and dependencies, so use call method

				CoreModifiable* theInputModule = KigsCore::GetModule("ModuleInput");

				XMLAttributeTemplate<StringType>* MethodNameAttribute = static_cast<XMLAttributeTemplate<StringType>*>(sonXML->getAttribute("M", "Method"));
				XMLAttributeTemplate<StringType>* EventNameAttribute = static_cast<XMLAttributeTemplate<StringType>*>(sonXML->getAttribute("E", "Event"));
				XMLAttributeTemplate<StringType>* FlagAttribute = static_cast<XMLAttributeTemplate<StringType>*>(sonXML->getAttribute("F", "Flag"));

				if (MethodNameAttribute && EventNameAttribute)
				{
					u32 flag = 0;
					if (FlagAttribute)
					{
						flag = FlagAttribute->getInt();
					}

					theInputModule->SimpleCall("registerTouchEvent", currentModifiable, MethodNameAttribute->XMLAttributeBase::getString(), EventNameAttribute->XMLAttributeBase::getString(), flag);
				}
			}
			else if (sonXML->compareName("Upgrd"))
			{
				// already done
			}
			else
			{
				sons.push_back(sonXML);
			}
		}
		else if ((sonXML->getType() == XML_NODE_TEXT_NO_CHECK) || (sonXML->getType() == XML_NODE_TEXT))
		{
			currentModifiable->ImportFromCData((std::string)sonXML->XMLNodeBase::getString());
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
	XMLAttributeTemplate<StringType>* pathAttribute = static_cast<XMLAttributeTemplate<StringType>*>(currentNode->getAttribute("P", "Path"));
	XMLAttributeTemplate<StringType>* attrname = static_cast<XMLAttributeTemplate<StringType>*>(currentNode->getAttribute("N", "Name"));
#else
	XMLAttributeTemplate<StringType>* pathAttribute = static_cast<XMLAttributeTemplate<StringType>*>(currentNode->getAndRemoveAttribute("P", "Path"));
	XMLAttributeTemplate<StringType>* attrname = static_cast<XMLAttributeTemplate<StringType>*>(currentNode->getAndRemoveAttribute("N", "Name"));
#endif

	if (pathAttribute)
	{
		currentModifiable = SearchInstance((std::string)pathAttribute->XMLAttributeBase::getString(), currentModifiable).get();
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

	XMLAttributeTemplate<StringType>* attrtype = static_cast<XMLAttributeTemplate<StringType>*>(currentNode->getAttribute("T", "Type"));
	if (attrtype && attrtype->XMLAttributeBase::getString() == "unknown")
	{
		return;
	}

	KigsID id = attrname->XMLAttributeBase::getString();

	CoreModifiableAttribute* attr = 0;
	bool attrfound = false;

	auto AttrByID = currentModifiable->mAttributes.find(id);

	if (AttrByID != currentModifiable->mAttributes.end())
	{
		attr = (*AttrByID).second;
		if (attr)
		{
			XMLAttributeTemplate<StringType>* attrvalue = static_cast<XMLAttributeTemplate<StringType>*>(currentNode->getAttribute("V", "Value"));

			if (attrvalue)
			{
				std::string tempvalue = (std::string)attrvalue->XMLAttributeBase::getString();
				if (AttributeNeedEval(tempvalue))
				{
					EvalAttribute(tempvalue, currentModifiable, attr);
				}

				// for usstring, manage UTF-8
				if ((attr->getType() == ATTRIBUTE_TYPE::USSTRING) && (importState.UTF8Enc))
				{
					attr->setValue((const UTF8Char*)tempvalue.c_str());
				}
				else
				{
					attr->setValue(tempvalue);
				}
			}
			else if ((attrvalue = static_cast<XMLAttributeTemplate<StringType>*>(currentNode->getAttribute("LUA", "L"), attrvalue)))
			{
				std::string code = (std::string)attrvalue->XMLAttributeBase::getString();
				CoreModifiable* luamodule = KigsCore::GetModule("LuaKigsBindModule");
				if (luamodule)
				{
					luamodule->SimpleCall("SetValueLua", currentModifiable, attrname->XMLAttributeBase::getString(), code);
				}
			}
			else // check if value is in text or CDATA
			{
				for (s32 i = 0; i < currentNode->getChildCount(); i++)
				{
					XMLNodeTemplate< StringType>* sonXML = static_cast<XMLNodeTemplate<StringType>*>(currentNode->getChildElement(i));
					if ((sonXML->getType() == XML_NODE_TEXT_NO_CHECK) || (sonXML->getType() == XML_NODE_TEXT))
					{
						std::string tempvalue = (std::string)sonXML->XMLNodeBase::getString();
						if (AttributeNeedEval(tempvalue))
						{
							EvalAttribute(tempvalue, currentModifiable, attr);
						}

						if ((attr->getType() == ATTRIBUTE_TYPE::USSTRING) && (importState.UTF8Enc))
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
		XMLAttributeTemplate<StringType>* attrdynamic = static_cast<XMLAttributeTemplate<StringType>*>(currentNode->getAttribute("Dyn", "Dynamic"));

		if (attrdynamic)
		{
			if ((attrdynamic->XMLAttributeBase::getString() == "true") || (attrdynamic->XMLAttributeBase::getString() == "yes"))
			{
				XMLAttributeTemplate<StringType>* attrvalue = static_cast<XMLAttributeTemplate<StringType>*>(currentNode->getAttribute("V", "Value"));

				if (attrvalue)
				{
					if (attrtype)
					{
						ATTRIBUTE_TYPE atype = CoreModifiableAttribute::stringToType((std::string)attrtype->XMLAttributeBase::getString());
						if (atype != ATTRIBUTE_TYPE::UNKNOWN)
						{
							if ((atype != ATTRIBUTE_TYPE::ARRAY) && (atype != ATTRIBUTE_TYPE::REFERENCE))
							{
								attr = currentModifiable->AddDynamicAttribute(atype, (std::string)attrname->XMLAttributeBase::getString());
								std::string tempvalue = (std::string)attrvalue->XMLAttributeBase::getString();
								if (AttributeNeedEval(tempvalue))
								{
									EvalAttribute(tempvalue, currentModifiable, attr);
								}

								if ((attr->getType() == ATTRIBUTE_TYPE::USSTRING) && (importState.UTF8Enc))
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
								attr = currentModifiable->AddDynamicAttribute(atype, (std::string)attrname->XMLAttributeBase::getString(), ((std::string)attrvalue->XMLAttributeBase::getString()).c_str());
							}
						}
					}
				}
				else // check if value is in text or CData
				{
					for (s32 i = 0; i < currentNode->getChildCount(); i++)
					{
						XMLNodeTemplate< StringType>* sonXML = static_cast<XMLNodeTemplate<StringType>*>(currentNode->getChildElement(i));
						if ((sonXML->getType() == XML_NODE_TEXT_NO_CHECK) || (sonXML->getType() == XML_NODE_TEXT))
						{
							ATTRIBUTE_TYPE atype = CoreModifiableAttribute::stringToType((std::string)attrtype->XMLAttributeBase::getString());
							if ((atype != ATTRIBUTE_TYPE::ARRAY) && (atype != ATTRIBUTE_TYPE::REFERENCE))
							{
								attr = currentModifiable->AddDynamicAttribute(atype, (std::string)attrname->XMLAttributeBase::getString());
								std::string tempvalue = (std::string)sonXML->XMLNodeBase::getString();
								if (AttributeNeedEval(tempvalue))
								{
									EvalAttribute(tempvalue, currentModifiable, attr);
								}
								if ((attr->getType() == ATTRIBUTE_TYPE::USSTRING) && (importState.UTF8Enc))
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
								attr = currentModifiable->AddDynamicAttribute(atype, (std::string)attrname->XMLAttributeBase::getString(), ((std::string)sonXML->XMLNodeBase::getString()).c_str());
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
			XMLNodeTemplate< StringType>* sonXML = static_cast<XMLNodeTemplate<StringType>*>(currentNode->getChildElement(i));
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

	XMLAttributeTemplate<StringType>* attrname = static_cast<XMLAttributeTemplate<StringType>*>(currentNode->getAttribute("N", "Name"));

	if (!attrname)
		return;

	XMLAttributeTemplate<StringType>* attrtype = static_cast<XMLAttributeTemplate<StringType>*>(currentNode->getAttribute("T", "Type"));
	XMLAttributeTemplate<StringType>* attrvalue = static_cast<XMLAttributeTemplate<StringType>*>(currentNode->getAttribute("V", "Value"));

	std::string code = "";
	if (attrvalue)
	{
		code = (std::string)attrvalue->XMLAttributeBase::getString();
		if (code.size() && code[0] == '#')
		{
			code.erase(code.begin());

			u64 size;
			CoreRawBuffer* rawbuffer = ModuleFileManager::LoadFileAsCharString(code.c_str(), size,1);
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
				XMLNodeTemplate< StringType>* sonXML = static_cast<XMLNodeTemplate<StringType>*>(currentNode->getChildElement(i));
				if ((sonXML->getType() == XML_NODE_TEXT_NO_CHECK) || (sonXML->getType() == XML_NODE_TEXT))
				{
					code = (std::string)sonXML->XMLNodeBase::getString();
					break;
				}
			}
		}
	}


	std::vector<CoreModifiableAttribute*> params;
	maString pName("pName", (std::string)attrname->XMLAttributeBase::getString());
	maString pCode("pCode", code);
	maRawPtr pXML("pXML", currentNode);

	params.push_back(&pName);
	params.push_back(&pCode);
	params.push_back(&pXML);

	maReference localthis("pThis", { "" });
	localthis = currentModifiable;
	params.push_back(&localthis);

	maString cbType("cbType", attrtype ? (std::string)attrtype->XMLAttributeBase::getString() : "");
	if (attrtype)
		params.push_back(&cbType);

	luamodule->CallMethod("RegisterLuaMethod", params);
}

template<typename StringType>
AttachedModifierBase* CoreModifiable::InitAttributeModifier(XMLNodeTemplate< StringType>* currentNode, CoreModifiableAttribute* attr)
{
	XMLAttributeTemplate<StringType>* attrtype = static_cast<XMLAttributeTemplate<StringType>*>(currentNode->getAttribute("T", "Type"));

	AttachedModifierBase* toAdd = 0;
	if (attrtype)
	{
		std::string modifiertype = (std::string)attrtype->XMLAttributeBase::getString();
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
			XMLAttributeTemplate<StringType>* attrsetter = static_cast<XMLAttributeTemplate<StringType>*>(currentNode->getAttribute("Setter", "isSetter"));

			if (attrsetter)
			{
				if ((attrsetter->XMLAttributeBase::getString() == "true") || (attrsetter->XMLAttributeBase::getString() == "yes"))
				{
					isSetter = true;
				}
			}

			// search value
			std::string value = "";
			XMLAttributeTemplate<StringType>* attrvalue = static_cast<XMLAttributeTemplate<StringType>*>(currentNode->getAttribute("V", "Value"));

			if (attrvalue)
			{
				value = (std::string)attrvalue->XMLAttributeBase::getString();
			}

			// check for direct string
			if (value == "")
			{
				for (s32 i = 0; i < currentNode->getChildCount(); i++)
				{
					XMLNodeTemplate< StringType>* sonXML = static_cast<XMLNodeTemplate<StringType>*>(currentNode->getChildElement(i));
					if ((sonXML->getType() == XML_NODE_TEXT_NO_CHECK) || (sonXML->getType() == XML_NODE_TEXT))
					{
						value = sonXML->XMLNodeBase::getString();
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
