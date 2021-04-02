#include "XLSXDocument.h"
#include "FilePathManager.h"
#include "ModuleFileManager.h"
#include "miniz.h"

// default data
const char* DefaultContentTypeXML= R"===(
<?xml version="1.0" encoding="UTF-8"?>
<Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types">
<Default Extension="xml" ContentType="application/xml"/>
<Default Extension="rels" ContentType="application/vnd.openxmlformats-package.relationships+xml"/>
<Default Extension="png" ContentType="image/png"/>
<Default Extension="jpeg" ContentType="image/jpeg"/>
<Override PartName="/_rels/.rels" ContentType="application/vnd.openxmlformats-package.relationships+xml"/>
<Override PartName="/xl/workbook.xml" ContentType="application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.main+xml"/>
<Override PartName="/xl/styles.xml" ContentType="application/vnd.openxmlformats-officedocument.spreadsheetml.styles+xml"/>
<Override PartName="/xl/_rels/workbook.xml.rels" ContentType="application/vnd.openxmlformats-package.relationships+xml"/>
<Override PartName="/docProps/core.xml" ContentType="application/vnd.openxmlformats-package.core-properties+xml"/>
<Override PartName="/docProps/app.xml" ContentType="application/vnd.openxmlformats-officedocument.extended-properties+xml"/>
</Types>
)===";

const char* DefaultContentTypeSheet = R"===(application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml)===";


const char* DefaultRels = R"===(
<?xml version="1.0" encoding="UTF-8"?>
<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">
<Relationship Id="rId1" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument" Target="xl/workbook.xml"/>
<Relationship Id="rId2" Type="http://schemas.openxmlformats.org/officedocument/2006/relationships/metadata/core-properties" Target="docProps/core.xml"/>
<Relationship Id="rId3" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/extended-properties" Target="docProps/app.xml"/>
</Relationships>
)===";

const char* DefaultDocPropsApp = R"===(
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Properties xmlns="http://schemas.openxmlformats.org/officeDocument/2006/extended-properties" xmlns:vt="http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes">
<Template>
</Template>
<TotalTime>0</TotalTime>
<Application>KigsFramework</Application>
</Properties>
)===";

const char* DefaultDocPropsCore = R"===(
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<cp:coreProperties xmlns:cp="http://schemas.openxmlformats.org/package/2006/metadata/core-properties" xmlns:dc="http://purl.org/dc/elements/1.1/" xmlns:dcterms="http://purl.org/dc/terms/" xmlns:dcmitype="http://purl.org/dc/dcmitype/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
<dcterms:created xsi:type="dcterms:W3CDTF">2021-03-12T12:11:44Z</dcterms:created>
<dc:creator>
</dc:creator>
<dc:description></dc:description>
<dc:language>fr-FR</dc:language>
<cp:lastModifiedBy></cp:lastModifiedBy>
<cp:revision>0</cp:revision>
<dc:subject></dc:subject>
<dc:title></dc:title>
</cp:coreProperties>
)===";


const char* DefaultStyleXml = R"===(
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<styleSheet xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main"><numFmts count="1"><numFmt numFmtId="164" formatCode="General"/></numFmts><fonts count="4"><font><sz val="10"/><name val="Arial"/><family val="2"/></font><font><sz val="10"/><name val="Arial"/><family val="0"/></font><font><sz val="10"/><name val="Arial"/><family val="0"/></font><font><sz val="10"/><name val="Arial"/><family val="0"/></font></fonts><fills count="2"><fill><patternFill patternType="none"/></fill><fill><patternFill patternType="gray125"/></fill></fills><borders count="1"><border diagonalUp="false" diagonalDown="false"><left/><right/><top/><bottom/><diagonal/></border></borders><cellStyleXfs count="20"><xf numFmtId="164" fontId="0" fillId="0" borderId="0" applyFont="true" applyBorder="true" applyAlignment="true" applyProtection="true"><alignment horizontal="general" vertical="bottom" textRotation="0" wrapText="false" indent="0" shrinkToFit="false"/><protection locked="true" hidden="false"/></xf><xf numFmtId="0" fontId="1" fillId="0" borderId="0" applyFont="true" applyBorder="false" applyAlignment="false" applyProtection="false"></xf><xf numFmtId="0" fontId="1" fillId="0" borderId="0" applyFont="true" applyBorder="false" applyAlignment="false" applyProtection="false"></xf><xf numFmtId="0" fontId="2" fillId="0" borderId="0" applyFont="true" applyBorder="false" applyAlignment="false" applyProtection="false"></xf><xf numFmtId="0" fontId="2" fillId="0" borderId="0" applyFont="true" applyBorder="false" applyAlignment="false" applyProtection="false"></xf><xf numFmtId="0" fontId="0" fillId="0" borderId="0" applyFont="true" applyBorder="false" applyAlignment="false" applyProtection="false"></xf><xf numFmtId="0" fontId="0" fillId="0" borderId="0" applyFont="true" applyBorder="false" applyAlignment="false" applyProtection="false"></xf><xf numFmtId="0" fontId="0" fillId="0" borderId="0" applyFont="true" applyBorder="false" applyAlignment="false" applyProtection="false"></xf><xf numFmtId="0" fontId="0" fillId="0" borderId="0" applyFont="true" applyBorder="false" applyAlignment="false" applyProtection="false"></xf><xf numFmtId="0" fontId="0" fillId="0" borderId="0" applyFont="true" applyBorder="false" applyAlignment="false" applyProtection="false"></xf><xf numFmtId="0" fontId="0" fillId="0" borderId="0" applyFont="true" applyBorder="false" applyAlignment="false" applyProtection="false"></xf><xf numFmtId="0" fontId="0" fillId="0" borderId="0" applyFont="true" applyBorder="false" applyAlignment="false" applyProtection="false"></xf><xf numFmtId="0" fontId="0" fillId="0" borderId="0" applyFont="true" applyBorder="false" applyAlignment="false" applyProtection="false"></xf><xf numFmtId="0" fontId="0" fillId="0" borderId="0" applyFont="true" applyBorder="false" applyAlignment="false" applyProtection="false"></xf><xf numFmtId="0" fontId="0" fillId="0" borderId="0" applyFont="true" applyBorder="false" applyAlignment="false" applyProtection="false"></xf><xf numFmtId="43" fontId="1" fillId="0" borderId="0" applyFont="true" applyBorder="false" applyAlignment="false" applyProtection="false"></xf><xf numFmtId="41" fontId="1" fillId="0" borderId="0" applyFont="true" applyBorder="false" applyAlignment="false" applyProtection="false"></xf><xf numFmtId="44" fontId="1" fillId="0" borderId="0" applyFont="true" applyBorder="false" applyAlignment="false" applyProtection="false"></xf><xf numFmtId="42" fontId="1" fillId="0" borderId="0" applyFont="true" applyBorder="false" applyAlignment="false" applyProtection="false"></xf><xf numFmtId="9" fontId="1" fillId="0" borderId="0" applyFont="true" applyBorder="false" applyAlignment="false" applyProtection="false"></xf></cellStyleXfs><cellXfs count="1"><xf numFmtId="164" fontId="0" fillId="0" borderId="0" xfId="0" applyFont="false" applyBorder="false" applyAlignment="false" applyProtection="false"><alignment horizontal="general" vertical="bottom" textRotation="0" wrapText="false" indent="0" shrinkToFit="false"/><protection locked="true" hidden="false"/></xf></cellXfs><cellStyles count="6"><cellStyle name="Normal" xfId="0" builtinId="0"/><cellStyle name="Comma" xfId="15" builtinId="3"/><cellStyle name="Comma [0]" xfId="16" builtinId="6"/><cellStyle name="Currency" xfId="17" builtinId="4"/><cellStyle name="Currency [0]" xfId="18" builtinId="7"/><cellStyle name="Percent" xfId="19" builtinId="5"/></cellStyles></styleSheet>
)===";

const char* DefaultWorkbookXml = R"===(
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<workbook xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main" xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships">
<fileVersion appName="Calc"/>
<workbookPr backupFile="false" showObjects="all" dateCompatibility="false"/>
<workbookProtection/>
<bookViews>
<workbookView showHorizontalScroll="true" showVerticalScroll="true" showSheetTabs="true" xWindow="0" yWindow="0" windowWidth="16384" windowHeight="8192" tabRatio="500" firstSheet="0" activeTab="0"/>
</bookViews>
<sheets>
</sheets>
<calcPr iterateCount="100" refMode="A1" iterate="false" iterateDelta="0.001"/>
<extLst>
<ext xmlns:loext="http://schemas.libreoffice.org/" uri="{7626C862-2A13-11E5-B345-FEFF819CDC9F}">
<loext:extCalcPr stringRefSyntax="CalcA1"/>
</ext>
</extLst>
</workbook>
)===";

const char* DefaultWorkbookRelsXml = R"===(
<?xml version="1.0" encoding="UTF-8"?>
<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">
<Relationship Id="rId1" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles" Target="styles.xml"/>
</Relationships>
)===";

const char* DefaultEmptySheetXml = R"===(
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<worksheet xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main" xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships" xmlns:xdr="http://schemas.openxmlformats.org/drawingml/2006/spreadsheetDrawing" xmlns:x14="http://schemas.microsoft.com/office/spreadsheetml/2009/9/main" xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006">
<sheetPr filterMode="false">
<pageSetUpPr fitToPage="false"/>
</sheetPr>
<dimension ref="A1"/>
<sheetViews>
<sheetView showFormulas="false" showGridLines="true" showRowColHeaders="true" showZeros="true" rightToLeft="false" tabSelected="true" showOutlineSymbols="true" defaultGridColor="true" view="normal" topLeftCell="A1" colorId="64" zoomScale="100" zoomScaleNormal="100" zoomScalePageLayoutView="100" workbookViewId="0">
<selection pane="topLeft" activeCell="A1" activeCellId="0" sqref="A1"/>
</sheetView>
</sheetViews>
<sheetFormatPr defaultColWidth="11.53515625" defaultRowHeight="12.8" zeroHeight="false" outlineLevelRow="0" outlineLevelCol="0"></sheetFormatPr>
<sheetData/>
<printOptions headings="false" gridLines="false" gridLinesSet="true" horizontalCentered="false" verticalCentered="false"/>
<pageMargins left="0.7875" right="0.7875" top="1.05277777777778" bottom="1.05277777777778" header="0.7875" footer="0.7875"/>
<pageSetup paperSize="9" scale="100" firstPageNumber="1" fitToWidth="1" fitToHeight="1" pageOrder="downThenOver" orientation="portrait" blackAndWhite="false" draft="false" cellComments="none" useFirstPageNumber="true" horizontalDpi="300" verticalDpi="300" copies="1"/>
<headerFooter differentFirst="false" differentOddEven="false">
<oddHeader>&amp;C&amp;&quot;Times New Roman,Regular&quot;&amp;12&amp;A</oddHeader>
<oddFooter>&amp;C&amp;&quot;Times New Roman,Regular&quot;&amp;12Page &amp;P</oddFooter>
</headerFooter>
</worksheet>
)===";

const char* DefaultSharedString = R"===(
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<sst xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main" count="0" uniqueCount="0">
</sst>
)===";

XLSXDocument::XLSXDocument() : XMLArchiveManager()
{

}

XLSXDocument::~XLSXDocument()
{
	for (auto s : mSheets)
	{
		delete(s);
	}
	mSheets.clear();
}

XLSXSheet* XLSXDocument::initSheet(const std::string& file, std::string name, int id)
{
	XLSXSheet* sheet = nullptr;
	XMLArchiveFile* xlsxfile = static_cast<XMLArchiveFile*>(mRoot.getFile(file));
	if (xlsxfile && xlsxfile->getXML())
	{
		sheet = new XLSXSheet(name, id,&mSharedStrings);
		sheet->initFromXML(xlsxfile->getXML());
	}
	return sheet;
}

void		XLSXDocument::initSharedStrings(const std::string& name)
{
	XMLArchiveFile* xlsxfile = static_cast<XMLArchiveFile*>(mRoot.getFile(name));
	if (xlsxfile && xlsxfile->getXML())
	{
		mSharedStringsXML = xlsxfile->getXML();
		mSharedStrings.initFromXML(xlsxfile->getXML());
	}
}

void	XLSXDocument::initWorkbook(const std::string& name)
{
	
	XMLArchiveFile* xlsxfile = static_cast<XMLArchiveFile*>(mRoot.getFile(name));
	if (xlsxfile)
	{

		auto frels = mRels.find(name);
		if (frels != mRels.end())
		{
			XLSXRelationships& wb_rels = (*frels).second;

			// init shared strings first
			std::string sharedStrings = wb_rels.getTargetFromType("sharedStrings");
			if (sharedStrings.length())
			{
				sharedStrings = xlsxfile->getPath() + "/" + sharedStrings;
				initSharedStrings(sharedStrings);
			}

			XMLBase* wb_xml = xlsxfile->getXML();
			XMLNodeBase* root = wb_xml->getRoot();

			XMLNodeBase* sheets = root->getChildElement("sheets");

			for (u32 i = 0; i < sheets->getChildCount(); i++)
			{
				XMLNodeBase* e = sheets->getChildElement(i);
				if (e->getName() == "sheet")
				{
					auto sheetname = e->getAttribute("name");
					auto sheetid = e->getAttribute("sheetId");
					auto sheetrid = e->getAttribute("r:id");
					if (sheetname && sheetid && sheetrid)
					{
						std::string target=wb_rels.getTarget(sheetrid->getString());
						if (target.length())
						{
							// add current path

							target = xlsxfile->getPath() + "/" + target;

							XLSXSheet* sheet=initSheet(target, sheetname->getString(), sheetid->getInt());
							if (sheet)
							{
								mSheets.push_back(sheet);
							}
						}
					}
				}
			}

			std::sort(mSheets.begin(), mSheets.end(), [](const XLSXSheet* a, const XLSXSheet* b)->bool {
				if (a->mSheedID == b->mSheedID)
				{
					return a < b;
				}
				return a->mSheedID < b->mSheedID;

				});

		}
	}
}

void		XLSXDocument::createFolderHierarchy()
{
	
	// create folder & files hierarchy
	XMLArchiveFile* f=new XMLArchiveFile("[Content_Types].xml", DefaultContentTypeXML, &mRoot);
	f->interpretAsXML();
	mContentType.initFromXML(f->getXML());
	XMLArchiveFolder* _rels = new XMLArchiveFolder("_rels", &mRoot);
	f = new XMLArchiveFile(".rels", DefaultRels, _rels);
	f->interpretAsXML();
	mRels[""].initFromXML(f->getXML());

	XMLArchiveFolder* docProps = new XMLArchiveFolder("docProps", &mRoot);
	f = new XMLArchiveFile("app.xml", DefaultDocPropsApp, docProps);
	f = new XMLArchiveFile("core.xml", DefaultDocPropsCore, docProps);

	XMLArchiveFolder* xl = new XMLArchiveFolder("xl", &mRoot);
	f = new XMLArchiveFile("workbook.xml", DefaultWorkbookXml, xl);


	f = new XMLArchiveFile("styles.xml", DefaultStyleXml, xl);

	XMLArchiveFolder* xl_rels = new XMLArchiveFolder("_rels", xl);
	f = new XMLArchiveFile("workbook.xml.rels", DefaultWorkbookRelsXml, xl_rels);
	f->interpretAsXML();
	mRels["xl/workbook.xml"].initFromXML(f->getXML());

	XMLArchiveFolder* xl_worksheets = new XMLArchiveFolder("worksheets", xl);
}

void	XLSXDocument::initEmpty()
{
	// clear current hierarchy if any
	clear();
	createFolderHierarchy();
}

bool	XLSXDocument::open(const std::string& filename)
{
	if (XMLArchiveManager::open(filename))
	{
		XMLArchiveFile* xlsxfile=static_cast<XMLArchiveFile*>(mRoot.getFile("[Content_Types].xml"));
		if ((xlsxfile) && (xlsxfile->getXML()))
		{
			XMLBase* type_xml= xlsxfile->getXML();
			mContentType.initFromXML(type_xml);
		}
		else
		{
			return false;
		}

		// check all contents that can be converted to xml
		for (auto f : (*this))
		{
			if (!f->isFolder())
			{
				std::string path = f->getName(true);
				std::string ctype=mContentType.getContentType(path);

				if (ctype.length()>=3)
				{
					if (ctype.substr(ctype.length() - 3, 3) == "xml")
					{
						static_cast<XMLArchiveFile*>(f)->interpretAsXML();
					}
				}

				// manage rels
				if (f->getExtension() == "rels")
				{
					xlsxfile = static_cast<XMLArchiveFile*>(f);
					XMLBase* rels_xml = xlsxfile->getXML();

					std::string concernedFile = f->getName(true);

					// remove .rels extension
					concernedFile.erase(concernedFile.length() - 5);

					// remove _rels/

					size_t p = concernedFile.find("_rels/");
					if (p != std::string::npos)
					{
						concernedFile.erase(p,6);
					}


					mRels[concernedFile].initFromXML(rels_xml);
				}
			}
		}

		// search document
		std::string workbook = mRels[""].getTargetFromType("officeDocument");
		if (workbook.length())
		{
			initWorkbook(workbook);
		}

		return true;
	}
	return false;
}

std::vector<XLSXElementRef>	XLSXDocument::find(const std::string& content, bool exactmatch)
{
	std::vector<XLSXElementRef> result;

	// search all sheets
	for (size_t i = 0; i < mSheets.size(); i++)
	{
		auto s = (*this)[i];
		auto resultSheet = s.find(content, exactmatch);
		result.insert(result.end(), resultSheet.begin(), resultSheet.end());
	}

	return result;
}

std::vector<XLSXElementRef>	XLSXDocument::find(int content)
{
	std::vector<XLSXElementRef> result;

	// search all sheets
	for (size_t i = 0; i < mSheets.size(); i++)
	{
		auto s = (*this)[i];
		auto resultSheet = s.find(content);
		result.insert(result.end(), resultSheet.begin(), resultSheet.end());
	}

	return result;
}

XLSXSheet* XLSXDocument::addSheet(std::string name)
{
	int id = mSheets.size()+1;
	char cid[12];
	char crid[12];
	itoa(id, cid, 10);

	if (name == "")
	{
		name = "Sheet";
		name +=cid;
	}

	// create sheet with given name
	XLSXSheet* sheet = new XLSXSheet(name, id, &mSharedStrings);
	
	// then construct xml file
	std::string xmlname = "sheet";
	xmlname += cid;
	xmlname += ".xml";

	// add <Override PartName="/xl/worksheets/sheet#.xml" ContentType="application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml"/> to content_type
	XML* _xml = static_cast<XML*>( (static_cast<XMLArchiveFile*>(mRoot.getFile("[Content_Types].xml")))->getXML());
	XMLNodeBase* _xmlroot= _xml->getRoot();
	XMLNode* newNode = new XMLNode(XML_NODE_ELEMENT,"Override");
	XMLAttribute* newAttr = new XMLAttribute("PartName", std::string("/xl/worksheets/")+ xmlname);
	newNode->addAttribute(newAttr);
	newAttr = new XMLAttribute("ContentType", DefaultContentTypeSheet);
	newNode->addAttribute(newAttr);
	_xmlroot->addChild(newNode);

	//add relationship <Relationship Id = "rId2" Type = "http://schemas.openxmlformats.org/officeDocument/2006/relationships/worksheet" Target = "worksheets/sheet1.xml" / >
	_xml = static_cast<XML*>((static_cast<XMLArchiveFile*>(mRoot.getFile("xl/_rels/workbook.xml.rels")))->getXML());
	_xmlroot = _xml->getRoot();
	int rid=_xmlroot->getChildCount()+1;
	itoa(rid, crid, 10);
	newNode = new XMLNode(XML_NODE_ELEMENT, "Relationship");
	newAttr = new XMLAttribute("Id", std::string("rId")+crid);
	newNode->addAttribute(newAttr);
	newAttr = new XMLAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/worksheet");
	newNode->addAttribute(newAttr);
	newAttr = new XMLAttribute("Target", std::string("worksheets/") + xmlname);
	newNode->addAttribute(newAttr);
	_xmlroot->addChild(newNode);

	// add sheet to workbook <sheet name = "Sheet1" sheetId = "1" state = "visible" r:id = "rId2" / >
	_xml = static_cast<XML*>((static_cast<XMLArchiveFile*>(mRoot.getFile("xl/workbook.xml")))->getXML());
	_xmlroot = _xml->getRoot();
	_xmlroot = _xmlroot->getChildElement("sheets");
	newNode = new XMLNode(XML_NODE_ELEMENT, "sheet");
	newAttr = new XMLAttribute("name", name);
	newNode->addAttribute(newAttr);
	newAttr = new XMLAttribute("sheetId", cid);
	newNode->addAttribute(newAttr);
	newAttr = new XMLAttribute("state", "visible");
	newNode->addAttribute(newAttr);
	newAttr = new XMLAttribute("r:id", std::string("rId") + crid);
	newNode->addAttribute(newAttr);
	_xmlroot->addChild(newNode);

	XMLArchiveFolder* xl_worksheets = static_cast<XMLArchiveFolder*>(mRoot.getFile("xl/worksheets"));
	XMLArchiveFile* sheetfile=new XMLArchiveFile(xmlname.c_str(), DefaultEmptySheetXml, xl_worksheets);
	sheetfile->interpretAsXML();
	sheet->initFromXML(sheetfile->getXML());

	mSheets.push_back(sheet);
	return sheet;
}


SP<CoreRawBuffer> XLSXDocument::save()
{
	if (!mSheets.size()) // need at least one sheet
	{
		addSheet("sheet1");
	}
	
	// update sheets and sharedstrings
	mSharedStrings.reset();

	for (size_t i = 0; i < mSheets.size(); i++)
	{
		auto s = (*this)[i];
		XLSXSheet* sheet = s;

		XMLBase* sheetXML = sheet->mBaseXML;

		sheet->updateXML(sheetXML);
	}

	if (mSharedStrings.mSharedStrings.size())
	{
		if (!mSharedStringsXML)
		{
			XMLArchiveFolder* xl = static_cast<XMLArchiveFolder*>(mRoot.getFile("xl"));
			XMLArchiveFile* sharedfile = new XMLArchiveFile("sharedStrings.xml", DefaultSharedString, xl);
			sharedfile->interpretAsXML();
			mSharedStringsXML = sharedfile->getXML();

			// add shared strings to workbook relations

			XML* _xml = static_cast<XML*>((static_cast<XMLArchiveFile*>(mRoot.getFile("xl/_rels/workbook.xml.rels")))->getXML());
			XMLNodeBase* _xmlroot = _xml->getRoot();
			int rid = _xmlroot->getChildCount() + 1;
			char crid[12];
			itoa(rid, crid, 10);
			XMLNode* newNode = new XMLNode(XML_NODE_ELEMENT, "Relationship");
			XMLAttribute* newAttr = new XMLAttribute("Id", std::string("rId") + crid);
			newNode->addAttribute(newAttr);
			newAttr = new XMLAttribute("Type", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/sharedStrings");
			newNode->addAttribute(newAttr);
			newAttr = new XMLAttribute("Target", "sharedStrings.xml");
			newNode->addAttribute(newAttr);
			_xmlroot->addChild(newNode);

		}

		mSharedStrings.updateXML(mSharedStringsXML);

		// update content type for sharedstrings
		mContentType.addOverride("xl/sharedStrings.xml", "application/vnd.openxmlformats-officedocument.spreadsheetml.sharedStrings+xml");
	}
	else
	{
		mContentType.removeOverride("xl/sharedStrings.xml");
	}

	// update content type
	XMLArchiveFile* xlsxfile = static_cast<XMLArchiveFile*>(mRoot.getFile("[Content_Types].xml"));
	if ((xlsxfile) && (xlsxfile->getXML()))
	{
		XMLBase* type_xml = xlsxfile->getXML();
		mContentType.updateXML(type_xml);
	}

	// then call XMLArchiveManager save
	return XMLArchiveManager::save();
}

// content type

void XLSXContentType::initFromXML(XMLBase* xml)
{
	mExtensions.clear();
	mOverride.clear();

	XMLNodeBase* root=xml->getRoot();

	for (u32 i = 0; i < root->getChildCount(); i++)
	{
		XMLNodeBase* e = root->getChildElement(i);
		if (e->getName() == "Default")
		{
			auto ext = e->getAttribute("Extension");
			auto c = e->getAttribute("ContentType");
			if (ext && c)
			{
				mExtensions[ext->getString()] = c->getString();
			}
		}
		else if (e->getName() == "Override")
		{
			auto pn = e->getAttribute("PartName");
			auto c = e->getAttribute("ContentType");
			if (pn && c)
			{
				mOverride[pn->getString()] = c->getString();
			}
		}
	}
}

std::string XLSXContentType::getPartName(const std::string contenttype)
{
	for (auto f : mOverride)
	{
		if (f.second.find(contenttype) != std::string::npos)
		{
			return f.first;
		}
	}
	return "";
}

std::string XLSXContentType::getOverride(const std::string& name)
{
	auto f = mOverride.find(name);
	if (f != mOverride.end())
	{
		return (*f).second;
	}
	return "";
}

// ext parameter must be an extension without '.'
std::string XLSXContentType::getExtension(const std::string& ext)
{
	auto f = mExtensions.find(ext);
	if (f != mExtensions.end())
	{
		return (*f).second;
	}
	return "";
}

std::string XLSXContentType::getContentType(const std::string name)
{
	auto f = getOverride(name);
	if (f.length())
	{
		return f;
	}

	size_t foundext = name.rfind(".");
	if (foundext != std::string::npos)
	{
		std::string ext = name.substr(foundext + 1);

		f = getExtension(ext);
		if (f.length())
		{
			return f;
		}
	}
	return "";
}

void	XLSXContentType::addOverride(const std::string& name, const std::string& contenttype)
{
	// test if already exists
	std::string tst = getOverride(name);
	if (!tst.length())
	{
		mOverride[name] = contenttype;
	}

}
void	XLSXContentType::removeOverride(const std::string& name)
{
	auto found = mOverride.find(name);
	if (found != mOverride.end())
	{
		mOverride.erase(found);
	}
}
void	XLSXContentType::addDefault(const std::string& extension, const std::string& contenttype)
{
	std::string tst = getExtension(extension);
	if (!tst.length())
	{
		mExtensions[extension] = contenttype;
	}
}
void	XLSXContentType::removeDefault(const std::string& extension)
{
	auto found = mExtensions.find(extension);
	if (found != mExtensions.end())
	{
		mExtensions.erase(found);
	}
}

void	XLSXContentType::updateXML(XMLBase* toUpdate)
{
	XMLNodeBase* root = toUpdate->getRoot();
	root->clearAllChildren();

	// add extensions
	for (auto& ext : mExtensions)
	{
		XMLNode* def = new XMLNode(XML_NODE_ELEMENT, "Default");
		def->addAttribute(new XMLAttribute("Extension", ext.first));
		def->addAttribute(new XMLAttribute("ContentType", ext.second));
		root->addChild(def);
	}

	// then override
	for (auto& ov : mOverride)
	{
		XMLNode* over = new XMLNode(XML_NODE_ELEMENT, "Override");
		over->addAttribute(new XMLAttribute("PartName", ov.first));
		over->addAttribute(new XMLAttribute("ContentType", ov.second));
		root->addChild(over);
	}
}

// relations

void XLSXRelationships::initFromXML(XMLBase* xml)
{
	mRelations.clear();

	XMLNodeBase* root = xml->getRoot();

	for (u32 i = 0; i < root->getChildCount(); i++)
	{
		XMLNodeBase* e = root->getChildElement(i);
		if (e->getName() == "Relationship")
		{
			auto aid = e->getAttribute("Id");
			auto t = e->getAttribute("Type");
			auto target = e->getAttribute("Target");
			if (aid && t && target)
			{
				u32 id = getID(aid->getString());
				addRelation(id, t->getString(), target->getString());
			}
		}
		
	}
}

void XLSXRelationships::updateXML(XMLBase* xml)
{
	XMLNodeBase* root = xml->getRoot();



}


// shared strings

void  XLSXSharedStrings::initFromXML(XMLBase* xml)
{
	mSharedStrings.clear();
	mStringIndex.clear();

	XMLNodeBase* root = xml->getRoot();
	auto c=root->getAttribute("uniqueCount");
	mSharedStrings.resize(c->getInt());

	for (u32 i = 0; i < root->getChildCount(); i++)
	{
		XMLNodeBase* si = root->getChildElement(i);

		XMLNodeBase* t = si->getChildElement(0);
		if (t->getChildCount())
		{
			XMLNodeBase* txt = t->getChildElement(0);
			mSharedStrings[i]= MinimalXML::decodeEscaped(txt->getString());
			mStringIndex[txt->getString()] = i;
		}
		else
		{
			mSharedStrings[i]="";
			mStringIndex[""] = i;
		}
	}
}

void XLSXSharedStrings::updateXML(XMLBase* toUpdate)
{
	XMLNodeBase* root = toUpdate->getRoot();
	XMLAttribute* c = static_cast<XMLAttribute*>(root->getAttribute("uniqueCount"));
	c->setInt(mSharedStrings.size());

	c = static_cast<XMLAttribute*>(root->getAttribute("count"));
	c->setInt(mCountAccess);
	
	root->clearAllChildren();

	for (auto s : mSharedStrings)
	{
		XMLNode* si = new XMLNode(XML_NODE_ELEMENT,"si");
		root->addChild(si);
		XMLNode* t = new XMLNode(XML_NODE_ELEMENT, "t");
		si->addChild(t);
		XMLNode* txt = new XMLNode(XML_NODE_TEXT, "");
		txt->setString(s);
		t->addChild(txt);
	}

}