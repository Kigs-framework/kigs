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
	XMLAttributeBase* autoUpdate = currentNode->getAttribute("AutoUpdate");
	XMLAttributeBase* aggregate = currentNode->getAttribute("Aggregate");
#else
	XMLAttributeBase* autoUpdate = currentNode->getAndRemoveAttribute("AutoUpdate");
	XMLAttributeBase* aggregate = currentNode->getAndRemoveAttribute("Aggregate");
#endif
	bool needInit = true;
	bool do_not_add_as_son = false;
	bool reused_unique_instance = false;
	// add shorter syntax
	if (currentNode->nameOneOf("Instance", "Inst"))
	{
#ifdef KEEP_XML_DOCUMENT
		XMLAttributeBase* NameAttribute = currentNode->getAttribute("N", "Name");
		XMLAttributeBase* typeAttribute = currentNode->getAttribute("T", "Type");
		XMLAttributeBase* pathAttribute = currentNode->getAttribute("P", "Path");
		XMLAttributeBase* uniqueAttribute = currentNode->getAttribute("U", "Unique");
#else
		XMLAttributeBase* NameAttribute = currentNode->getAndRemoveAttribute("N", "Name");
		XMLAttributeBase* typeAttribute = currentNode->getAndRemoveAttribute("T", "Type");
		XMLAttributeBase* pathAttribute = currentNode->getAndRemoveAttribute("P", "Path");
		XMLAttributeBase* uniqueAttribute = currentNode->getAndRemoveAttribute("U", "Unique");
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
				if (instances.size())
				{
					KigsID	typeID(typeAttribute->getRefString());
					//std::vector<s32>	linklist=currentModifiable->getItemLinkTypes();
					std::vector<ModifiableItemStruct>::const_iterator itson;
					//std::vector<s32>::const_iterator	itsonlink=linklist.begin();
					for (itson = instances.begin(); itson != instances.end(); ++itson)
					{
						CMSP son = (*itson).mItem;
						if (son->getName() == name)
						{
							if (son->isSubType(typeID))
							{
								found = true;
								needInit = false;
								current = son;
								break;
							}
						}
					}
				}
			}

			if (!found)
			{
				if (is_unique)
				{
					current = GetFirstInstanceByName(typeAttribute->getRefString(), name, false);
					reused_unique_instance = (bool)current;
					//if (current) current->GetRef(); // already set by GetFirstInstanceByName
				}

				if (!current)
				{
					current = KigsCore::GetInstanceOf(name, typeAttribute->getRefString());
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

			current = Import(pathAttribute->getString(), importState.noInit, importState.keepImportFileName, &importState, name);
			
			importState.is_include_unique = false;

			if (current)
			{
				if (importState.keepImportFileName)
				{
					current->AddDynamicAttribute(ATTRIBUTE_TYPE::STRING, "SeparatedFile", pathAttribute->getString().c_str());
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
	else if (currentNode->XMLNodeBase::compareName("Include"))
	{
#ifdef KEEP_XML_DOCUMENT
		XMLAttributeBase* NameAttribute = currentNode->getAttribute("Name", "N");
		XMLAttributeBase* PathAttribute = currentNode->getAttribute("Path", "P");
#else
		XMLAttributeBase* NameAttribute = currentNode->getAndRemoveAttribute("Name", "N");
		XMLAttributeBase* PathAttribute = currentNode->getAndRemoveAttribute("Path", "P");
#endif
		bool rename = NameAttribute && PathAttribute;

		if (NameAttribute)
		{
			std::string name = NameAttribute->getString();
			std::string path = PathAttribute ? PathAttribute->getString() : name;

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
		XMLAttributeBase* DontAdd = currentNode->getAttribute("DontAddAsSon");
		XMLAttributeBase* pathAttribute = currentNode->getAttribute("P", "Path");
		if (pathAttribute)
		{
			current = SearchInstance(pathAttribute->getString(), currentModifiable);
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
		XMLAttributeBase* PathAttribute = currentNode->getAttribute("P", "Path");
		XMLAttributeBase* NameAttribute = currentNode->getAttribute("Name", "N");
		bool rename = NameAttribute && PathAttribute;
		if (PathAttribute)
		{

			current = SearchInstance(PathAttribute->getString(), currentModifiable);
			if (current)
			{
				if (rename)
				{
					current->setName(NameAttribute->getString());
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
	XMLAttributeBase* ref = currentNode->getAttribute("Ref", "Reference");

	if (ref)
	{
		// search modifiable by type, 1st among the already loaded items ...
		bool WasFoundInLoaded = false;

		std::vector<CMSP>::const_iterator Iter = loadedItems.begin();
		std::vector<CMSP>::const_iterator Iterend = loadedItems.end();
		KigsID type(ref->getRefString());
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
		XMLNodeBase* sonXML = node->getChildElement(i);
		if (sonXML->getType() == XML_NODE_ELEMENT)
		{
			if (sonXML->compareName("Upgrd"))
			{
				XMLAttributeBase* attr = sonXML->getAttribute("N", "Name");
				currentModifiable->Upgrade(attr->getString());
			}
		}
	}
}

template<typename StringType>
void 	CoreModifiable::ImportAttributes(XMLNodeBase* node, CoreModifiable* currentModifiable, ImportState& importState, std::vector<XMLNodeBase*>& sons)
{
	for (s32 i = 0; i < node->getChildCount(); i++)
	{
		XMLNodeBase* sonXML = node->getChildElement(i);
		if (sonXML->getType() == XML_NODE_ELEMENT)
		{

			if (sonXML->nameOneOf("Attr", "CoreModifiableAttribute"))
			{
				// init attribute
				InitAttribute(static_cast<XMLNodeTemplate<StringType>*>(sonXML), currentModifiable, importState);
			}
			else if (sonXML->nameOneOf("CoreDecorator", "Deco"))
			{
				XMLAttributeBase* attr =sonXML->getAttribute("N", "Name");
				KigsCore::DecorateInstance(currentModifiable, attr->getRefString());
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
				XMLAttributeBase * slot = sonXML->getAttribute("N","Name");
				XMLAttributeBase* signal = sonXML->getAttribute("T","Type");

				KigsCore::Connect(currentModifiable, signal->getRefString(), currentModifiable, slot->getRefString());
			}
			else if (sonXML->nameOneOf("OnEvent", "OnE"))
			{
				XMLAttributeBase* NameAttribute = sonXML->getAttribute("N", "Name");
				XMLAttributeBase* ActionAttribute = sonXML->getAttribute("A", "Action");

				if (NameAttribute && ActionAttribute)
				{
					KigsCore::GetNotificationCenter()->addObserver(currentModifiable, ActionAttribute->getString(), NameAttribute->getString());
				}
			}
			else if (sonXML->nameOneOf("Connection", "Connect"))
			{
				XMLAttributeBase* SiNameAttribute = sonXML->getAttribute("Si", "Signal");
				XMLAttributeBase* InstAAttribute = sonXML->getAttribute("E", "Emitter");
				XMLAttributeBase* SlNameAttribute = sonXML->getAttribute("Sl", "Slot");
				XMLAttributeBase* InstBAttribute = sonXML->getAttribute("R", "Receiver");

				XMLAttributeBase* SetValueAttribute = sonXML->getAttribute("V", "SetValue");

				if (SiNameAttribute && SlNameAttribute)
				{
					std::string signal = SiNameAttribute->getString();
					std::string slot = SlNameAttribute->getString();

					std::string ia = InstAAttribute ? InstAAttribute->getString() : "this";
					std::string ib = InstBAttribute ? InstBAttribute->getString() : "this";

					importState.toConnect.push_back(ImportState::ToConnect{ currentModifiable, ia, signal, ib, slot, SetValueAttribute ? SetValueAttribute->getString() : "", sonXML });
				}
			}
			else if (sonXML->compareName("Call"))
			{
				XMLAttributeBase* ParamList = sonXML->getAttribute("P","Params");
				XMLAttributeBase* MethodName = sonXML->getAttribute("N", "Name");
				if (MethodName)
				{
					std::string plist = "";
					if (ParamList) // if param attribute found
					{
						plist = ParamList->getString();
					}
					else // check if text son node exists
					{
						if (sonXML->getChildCount() == 1)
						{
							XMLNodeBase* textSon = sonXML->getChildElement(0);
							if ((textSon->getType() == XML_NODE_TEXT_NO_CHECK) || (textSon->getType() == XML_NODE_TEXT))
							{
								plist = textSon->getString();
							}
						}
					}
					
					importState.toCall.push_back(ImportState::ToCall{ currentModifiable, MethodName->getString() ,plist });
				}
			}
			else if (sonXML->nameOneOf("RegisterTouchEvent", "REvent"))
			{
				// don't want to include ModuleInput and dependencies, so use call method

				auto theInputModule = KigsCore::GetModule("ModuleInput");

				XMLAttributeBase * MethodNameAttribute = sonXML->getAttribute("M", "Method");
				XMLAttributeBase * EventNameAttribute = sonXML->getAttribute("E", "Event");
				XMLAttributeBase* FlagAttribute = sonXML->getAttribute("F", "Flag");

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
			currentModifiable->ImportFromCData(sonXML->getString());
		}
	}
}

template<typename StringType>
void CoreModifiable::ImportSons(const std::vector<XMLNodeBase*>& sons, CoreModifiable* currentModifiable, ImportState& importState)
{
	for (XMLNodeBase* sonXML : sons)
	{
		Import(static_cast<XMLNodeTemplate< StringType>*>(sonXML), currentModifiable, importState);
	}
}

template<typename StringType>
void	CoreModifiable::InitAttribute(XMLNodeTemplate<StringType>* currentNode, CoreModifiable* currentModifiable, ImportState& importState)
{

#ifdef KEEP_XML_DOCUMENT
	XMLAttributeBase* pathAttribute = currentNode->getAttribute("P", "Path");
	XMLAttributeBase* attrname = currentNode->getAttribute("N", "Name");
#else
	XMLAttributeBase* pathAttribute = currentNode->getAndRemoveAttribute("P", "Path");
	XMLAttributeBase* attrname = currentNode->getAndRemoveAttribute("N", "Name");
#endif

	if (pathAttribute)
	{
		currentModifiable = SearchInstance(pathAttribute->getString(), currentModifiable).get();
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

	XMLAttributeBase* attrtype =currentNode->getAttribute("T", "Type");
	if (attrtype && attrtype->compareValue("unknown"))
	{
		return;
	}

	KigsID id(attrname->getRefString());

	CoreModifiableAttribute* attr = currentModifiable->findAttributeOnThisOnly(id);
	bool attrfound = false;
	if (attr)
	{
		XMLAttributeBase* attrvalue = currentNode->getAttribute("V", "Value");

		if (attrvalue)
		{
			std::string tempvalue = attrvalue->getString();
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
		else 
		{
			attrvalue = currentNode->getAttribute("LUA", "L");
			if (attrvalue)
			{
				auto luamodule = KigsCore::GetModule("LuaKigsBindModule");
				if (luamodule)
				{
					luamodule->SimpleCall("SetValueLua", currentModifiable, attrname->getString(), attrvalue->getString());
				}
			}
			else // check if value is in text or CDATA
			{
				for (s32 i = 0; i < currentNode->getChildCount(); i++)
				{
					XMLNodeBase* sonXML = currentNode->getChildElement(i);
					if ((sonXML->getType() == XML_NODE_TEXT_NO_CHECK) || (sonXML->getType() == XML_NODE_TEXT))
					{
						std::string tempvalue = sonXML->getString();
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
		}
		attrfound = true;
	}

	if (!attrfound)
	{
		// if attribute is dynamic or currentModifiable is tagget as autocreateattributes
		XMLAttributeBase* attrdynamic = currentNode->getAttribute("Dyn", "Dynamic");

		if (attrdynamic || currentModifiable->isFlagAutoCreateAttributes())
		{
			if (attrdynamic->compareValue("true") || attrdynamic->compareValue("yes") || currentModifiable->isFlagAutoCreateAttributes())
			{
				XMLAttributeBase* attrvalue = currentNode->getAttribute("V", "Value");

				if (attrvalue)
				{
					if (attrtype)
					{
						ATTRIBUTE_TYPE atype = CoreModifiableAttribute::stringToType(attrtype->getRefString());
						if (atype != ATTRIBUTE_TYPE::UNKNOWN)
						{
							if ((atype != ATTRIBUTE_TYPE::ARRAY) && (atype != ATTRIBUTE_TYPE::WEAK_REFERENCE) && (atype != ATTRIBUTE_TYPE::STRONG_REFERENCE))
							{
								attr = currentModifiable->AddDynamicAttribute(atype, attrname->getRefString());
								std::string tempvalue = attrvalue->getString();
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
								attr = currentModifiable->AddDynamicAttribute(atype, attrname->getRefString(), attrvalue->getString().c_str());
							}
						}
					}
				}
				else // check if value is in text or CData
				{
					for (s32 i = 0; i < currentNode->getChildCount(); i++)
					{
						XMLNodeBase* sonXML = currentNode->getChildElement(i);
						if ((sonXML->getType() == XML_NODE_TEXT_NO_CHECK) || (sonXML->getType() == XML_NODE_TEXT))
						{
							ATTRIBUTE_TYPE atype = CoreModifiableAttribute::stringToType(attrtype->getRefString());
							if ((atype != ATTRIBUTE_TYPE::ARRAY) && (atype != ATTRIBUTE_TYPE::WEAK_REFERENCE) && (atype != ATTRIBUTE_TYPE::STRONG_REFERENCE))
							{
								attr = currentModifiable->AddDynamicAttribute(atype, attrname->getRefString());
								std::string tempvalue = sonXML->getString();
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
								attr = currentModifiable->AddDynamicAttribute(atype, attrname->getRefString(), sonXML->getString().c_str());
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
			XMLNodeBase* sonXML = currentNode->getChildElement(i);
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



#endif //_COREMODIFIABLETEMPLATEIMPORT_H
