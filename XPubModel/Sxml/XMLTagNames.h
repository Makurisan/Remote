#if !defined(_XMLTAGNAMES_H_)
#define _XMLTAGNAMES_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



///////////////////////////////////////////////////////////////////////////
// Values for attributes
// 
// information sight -> entity -> table
#define VAL_ISENTITY_TABLE_USEENTITY  _XT("__use_entity_not_table__")
// stone -> link -> target frame
#define TARGET_FRAME_SAMEFRAME        _XT("_self")
#define TARGET_FRAME_WHOLEPAGE        _XT("_top")
#define TARGET_FRAME_NEWWINDOW        _XT("_blank")
#define TARGET_FRAME_PARENTFRAME      _XT("_parent")




///////////////////////////////////////////////////////////////////////////
// FOR XML 4 #define EMPTY_XPUB_XML_DOCUMENT  _XT("<?xml version=\"1.0\" encoding=\"utf-8\" ?>\r\n<X-Publisher></X-Publisher>\r\n")
#define EMPTY_PROJ_XPUB_XML_DOCUMENT        _XT("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<X-Publisher-Project></X-Publisher-Project>\r\n")
#define MAIN_TAG_PROJ                       _XT("X-Publisher-Project")
#define EMPTY_GUIPROJ_XPUB_XML_DOCUMENT     _XT("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<X-Publisher-GUI></X-Publisher-GUI>\r\n")
#define MAIN_TAG_GUIPROJ                    _XT("X-Publisher-GUI")
#define EMPTY_ER_XPUB_XML_DOCUMENT          _XT("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<X-Publisher-ER></X-Publisher-ER>\r\n")
#define MAIN_TAG_ER                         _XT("X-Publisher-ER")
#define EMPTY_IFS_XPUB_XML_DOCUMENT         _XT("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<X-Publisher-IFS></X-Publisher-IFS>\r\n")
#define MAIN_TAG_IFS                        _XT("X-Publisher-IFS")
#define EMPTY_STONE_XPUB_XML_DOCUMENT       _XT("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<X-Publisher-Stone></X-Publisher-Stone>\r\n")
#define MAIN_TAG_STONE                      _XT("X-Publisher-Stone")
#define EMPTY_TEMPLATE_XPUB_XML_DOCUMENT    _XT("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<X-Publisher-Template></X-Publisher-Template>\r\n")
#define MAIN_TAG_TEMPLATE                   _XT("X-Publisher-Template")

///////////////////////////////////////////////////////////////////////////
// project items
#define ER_FILES_NAME               _XT("ER-Files")
#define ER_FILE_NAME                _XT("ER-File")
#define IFS_FILES_NAME              _XT("IFS-Files")
#define IFS_FILE_NAME               _XT("IFS-File")
#define STONE_FILES_NAME            _XT("Stone-Files")
#define STONE_FILE_NAME             _XT("Stone-File")
#define TEMPLATE_FILES_NAME         _XT("Template-Files")
#define TEMPLATE_FILE_NAME          _XT("Template-File")
#define PYTHON_FILES_NAME           _XT("Python-Files")
#define PYTHON_FILE_NAME            _XT("Python-File")

#define ER_FILE_FILENAME            _XT("FileName")
#define IFS_FILE_FILENAME           _XT("FileName")
#define STONE_FILE_FILENAME         _XT("FileName")
#define TEMPLATE_FILE_FILENAME      _XT("FileName")
#define PYTHON_FILE_FILENAME        _XT("FileName")

#define PROJ_PROJECTNAME                  _XT("ProjectName")
#define PROJ_PROJECTSUBJECT               _XT("ProjectSubject")
#define PROJ_PROJECTVERSION               _XT("ProjectVersion")
#define PROJ_TESTFOLDER                   _XT("TestFolder")
#define PROJ_USE_TEST_CONSTANTS_FOR_PRODUCTION  _XT("UseTestConstantsForProduction")
#define PROJ_PREVIEW_TYPE                 _XT("PreviewType")
#define PROJ_TRANSLATECODEFILE            _XT("TranslateCodeFile")

#define PROJ_DTN_LONGDATEFORMAT                 _XT("LongDateFormat")
#define PROJ_DTN_YEARWITHCENTURY                _XT("YearWithCentury")
#define PROJ_DTN_DATESEPARATOR                  _XT("DateSeparator")
#define PROJ_DTN_TIMESEPARATOR                  _XT("TimeSeparator")
#define PROJ_DTN_USESYMBOLSFROMSYSTEM           _XT("UseSymbolsFromSystem")
#define PROJ_DTN_DECIMALSYMBOL                  _XT("DecimalSymbol")
#define PROJ_DTN_DIGITGROUPINGSYMBOL            _XT("DigitGroupingSymbol")
#define PROJ_DTN_USECOUNTOFDIGITSAFTERDECIMAL   _XT("UseCountOfDigitsAfterDecimal")
#define PROJ_DTN_COUNTOFDIGITSAFTERDECIMAL      _XT("CountOfDigitsAfterDecimal")

///////////////////////////////////////////////////////////////////////////
#define ERMODEL_ELEM_NAME           _XT("ER-Model")
#define IFSENTITY_ELEM_NAME         _XT("IFS-Entity")
#define STONE_ELEM_NAME             _XT("Stone")
#define TEMPLATE_ELEM_NAME          _XT("Template")

#define PROPERTIES_ELEM_NAME              _XT("Properties")
#define PROPERTY_MP_ELEM_NAME             _XT("MainProperty")
#define PROPERTY_MP_IFSPROPERTYSTONES     _XT("IFSPropertyStones")
#define PROPERTY_MP_IFSDBSTONES           _XT("IFSDBStones")

#define EMPTY_STANDALONE_NODE_DOCUMENT    _XT("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<StandAloneNode></StandAloneNode>\r\n")
#define EMPTY_STANDALONE_NODE             _XT("StandAloneNode")
#define NODES_ELEM_NAME             _XT("Nodes")
#define NODE_ELEM_NAME              _XT("Node")
#define NODEUI_ELEM_NAME            _XT("NodeUI")
#define NODE_ELEM_TABLES_FOR_FREE_SELECT  _XT("NodeTablesForFreeSelect")
#define NODE_ELEM_TABLE_FOR_FREE_SELECT   _XT("TableForFreeSelect%ld")

#define FIELDS_ELEM_NAME            _XT("Fields")
#define FIELD_ELEM_NAME             _XT("Field")

#define PARAMS_ELEM_NAME            _XT("Params")
#define PARAM_ELEM_NAME             _XT("Param")

#define RELATIONS_ELEM_NAME         _XT("Relations")
#define RELATIONSCHILD_ELEM_NAME    _XT("RelationsToChild")
#define RELATION_ELEM_NAME          _XT("Relation")
#define HIERARCHIES_ELEM_NAME       _XT("Hierarchies")
#define HIERARCHIESCHILD_ELEM_NAME  _XT("HierarchiesToChild")

#define CONNECTIONS_ELEM_NAME       _XT("Connections")
#define CONNECTIONSCHILD_ELEM_NAME  _XT("ConnectionsToChild")
#define CONNECTION_ELEM_NAME        _XT("Connection")
#define VARIABLES_ELEM_NAME         _XT("Variables")
#define VARIABLE_ELEM_NAME          _XT("Variable")
#define ASSIGNMENTS_ELEM_NAME       _XT("Assignments")
#define ASSIGNMENT_ELEM_NAME        _XT("Assignment")

#define HITNAMES_ELEM_NAME          _XT("HitNames")
#define HITNAME_ELEM_NAME           _XT("HitName")

#define PREPROCESSES_ELEM_NAME      _XT("PreProcesses")
#define PREPROCESS_ELEM_NAME        _XT("PreProcess")
#define POSTPROCESSES_ELEM_NAME     _XT("PostProcesses")
#define POSTPROCESS_ELEM_NAME       _XT("PostProcess")

#define GLOBALERTABLES_ELEM_NAME    _XT("GlobalERTables")
#define GLOBALERTABLE_ELEM_NAME     _XT("GlobalERTable")

#define CONSTANTS_ELEM_NAME         _XT("Constants")
#define CONSTANT_ELEM_NAME          _XT("Constant")
#define KEYS_ELEM_NAME              _XT("Keys")
#define KEY_ELEM_NAME               _XT("Key")
#define TEXTS_ELEM_NAME             _XT("Texts")
#define TEXT_ELEM_NAME              _XT("Text")
#define AREAS_ELEM_NAME             _XT("Areas")
#define AREA_ELEM_NAME              _XT("Area")
#define LINKS_ELEM_NAME             _XT("Links")
#define LINK_ELEM_NAME              _XT("Link")
#define GRAPHICS_ELEM_NAME          _XT("Graphics")
#define GRAPHIC_ELEM_NAME           _XT("Graphic")


///////////////////////////////////////////////////////////////////////////
#define NODE_ATTRIB_OBJECTTYPE              _XT("ObjectType")
#define NODE_ATTRIB_NAME                    _XT("Name")
#define NODE_ATTRIB_COMMENT                 _XT("Comment")
#define NODE_ATTRIB_TYPEOFENTITY            _XT("TypeOfEntity")
#define NODE_ATTRIB_SQLCOMMAND              _XT("SQLCommand")
#define NODE_ATTRIB_FREESELECTIONDATA       _XT("FreeSelectionData")
#define NODE_ATTRIB_DLLNAME                 _XT("DLLName")
#define NODE_ATTRIB_DLLINTERFACE            _XT("DLLInterface")
#define NODE_ATTRIB_XMLFILE                 _XT("XMLFile")
#define NODE_ATTRIB_XMLPATH                 _XT("XMLPath")
#define NODE_ATTRIB_FLAGS                   _XT("Flags")
#define NODE_ATTRIB_PERSISTENTDATA          _XT("PersistentData")
#define NODE_ATTRIB_IMPORTTABLEDEFFILE      _XT("ImportTableDefFile")
#define NODE_ATTRIB_IMPORTTABLEDEFNAME      _XT("ImportTableDefName")
#define NODE_ATTRIB_SUBIFS                  _XT("SubIFS")
#define NODE_ATTRIB_PORTCONDITION             _XT("PortCondition")
#define NODE_ATTRIB_PORTCONDITION_EXPRVALUES  _XT("PortConditionExprValues")
#define NODE_ATTRIB_PORTCONDITION_EXPRSYMBOLS _XT("PortConditionExprSymbols")
#define NODE_ATTRIB_READMETHOD              _XT("ReadMethod")
#define NODE_ATTRIB_ERTABLEORGLOBALTABLEORPROPSTONETYPEORDBSTONENAME  _XT("ERTableOrGlobalTableOrPropStoneTypeOrDBStoneName")
#define NODE_ATTRIB_ERTABLEREALNAME             _XT("ERTableRealName")
#define NODE_ATTRIB_ERTABLEREALNAMEEXPRVALUES   _XT("ERTableRealNameExprValues")
#define NODE_ATTRIB_ERTABLEREALNAMEEXPRSYMBOLS  _XT("ERTableRealNameExprSymbols")
#define NODE_ATTRIB_IDENTITY                _XT("Identity")
#define NODE_ATTRIB_CONDITION               _XT("Condition")
#define NODE_ATTRIB_EXPRVALUES              _XT("ExpressionValues")
#define NODE_ATTRIB_EXPRSYMBOLS             _XT("ExpressionSymbols")
#define NODE_ATTRIB_EXPROFFSETS             _XT("ExpressionOffsets")
#define NODE_ATTRIB_DONTOPTIMIZECONDITION   _XT("DontOptimizeCondition")
#define NODE_ATTRIB_WHERECLAUSE             _XT("WhereClause")
#define NODE_ATTRIB_SORTFIELD               _XT("SortField")
#define NODE_ATTRIB_SORTDESC                _XT("SortDesc")
#define NODE_ATTRIB_HITTABLEFIELD           _XT("HitTableField")
#define NODE_ATTRIB_HITTABLEVALUE           _XT("HitTableValue")
#define NODE_ATTRIB_MAINNODE                _XT("Main")
#define NODE_ATTRIB_DEBUGMODE               _XT("DebugMode")
#define NODE_ATTRIB_OPENEDENTITY            _XT("OpenedEntity")
#define NODE_ATTRIB_OPENEDENTITYNAME        _XT("OpenedEntityName")
#define NODE_ATTRIB_SHOWEXPANDED            _XT("ShowExpanded")
#define NODE_ATTRIB_SHOWBEHAVIOUR           _XT("ShowBehaviour")
#define NODE_ATTRIB_SHOWADDCONDITION        _XT("ShowAddCondition")
#define NODE_ATTRIB_SHOWADDFREESELECTION    _XT("ShowAddFreeSelection")
#define NODE_ATTRIB_SHOWFREESELECTION       _XT("ShowFreeSelection")
#define NODE_ATTRIB_HIT_TYPE                _XT("HitType")
#define NODE_ATTRIB_HIT_NAME_TEXT               _XT("HitNameText")
#define NODE_ATTRIB_HIT_NAME_TEXT_EXPRVALUES    _XT("HitNameTextExprValues")
#define NODE_ATTRIB_HIT_NAME_TEXT_EXPRSYMBOLS   _XT("HitNameTextExprSymbols")
#define NODE_ATTRIB_HIT_CONDITION               _XT("HitCondition")
#define NODE_ATTRIB_HIT_CONDITION_EXPRVALUES    _XT("HitConditionExprValues")
#define NODE_ATTRIB_HIT_CONDITION_EXPRSYMBOLS   _XT("HitConditionExprSymbols")
#define NODE_ATTRIB_CHILD_EXECUTION         _XT("ChildExecution")
#define NODE_ATTRIB_UI_X                    _XT("X")
#define NODE_ATTRIB_UI_Y                    _XT("Y")
#define NODE_ATTRIB_UI_CX                   _XT("CX")
#define NODE_ATTRIB_UI_CY                   _XT("CY")
#define NODE_ATTRIB_UI_SYNC                 _XT("Synchronize")
#define NODE_ATTRIB_UI_COLOR                _XT("Color")
#define NODE_ATTRIB_UI_COLOR_BACK           _XT("ColorBack")
#define NODE_ATTRIB_UI_COLOR_HDR            _XT("ColorHeader")
#define NODE_ATTRIB_UI_COLOR_HDR_SEL        _XT("ColorHeaderSel")
#define NODE_ATTRIB_UI_COLOR_HDR_TEXT       _XT("ColorHeaderText")
#define NODE_ATTRIB_UI_COLOR_HDR_SEL_TEXT   _XT("ColorHeaderSelText")
#define NODE_ATTRIB_UI_COLOR_SEL_ITEM       _XT("ColorSelectedItem")
#define NODE_ATTRIB_UI_MOVEABLE             _XT("Moveable")
#define NODE_ATTRIB_UI_SELECTABLE           _XT("Selectable")
#define NODE_ATTRIB_UI_VISIBLE              _XT("Visible")
#define NODE_ATTRIB_UI_SIZEABLE             _XT("Sizeable")

#define PARAMTABLE_VALUES_NAME              _XT("ParamTableValues")
#define NODE_ATTRIB_PARAMTABLE_FIELD        _XT("FieldName%ld")
#define NODE_ATTRIB_PARAMTABLE_VALUE        _XT("FieldValue%ld")

///////////////////////////////////////////////////////////////////////////
#define FIELD_ATTRIB_NAME                             _XT("Name")
#define FIELD_ATTRIB_COMMENT                          _XT("Comment")
#define FIELD_ATTRIB_TYPE                             _XT("Type")
#define FIELD_ATTRIB_DEFINEDSIZE                      _XT("DefinedSize")
#define FIELD_ATTRIB_ATTRIBUTES                       _XT("Attributes")
#define FIELD_ATTRIB_IMPORTTABLEDEFSTRUCTNAME         _XT("ImportTableDefStructName")
#define FIELD_ATTRIB_IMPORTTABLEDEFSTRUCTFIELDINDEX   _XT("ImportTableDefStructFieldIndex")
#define FIELD_ATTRIB_FLAGS                            _XT("Flags")
#define FIELD_ATTRIB_PERSISTENTDATA                   _XT("PersistentData")

///////////////////////////////////////////////////////////////////////////
#define PARAM_ATTRIB_NAME             _XT("Name")
#define PARAM_ATTRIB_COMMENT          _XT("Comment")
#define PARAM_ATTRIB_TYPE             _XT("Type")
#define PARAM_ATTRIB_DEFINEDSIZE      _XT("DefinedSize")
#define PARAM_ATTRIB_DEFAULTVALUE     _XT("DefaultValue")
#define PARAM_ATTRIB_FLAGS            _XT("Flags")
#define PARAM_ATTRIB_PERSISTENTDATA   _XT("PersistentData")

///////////////////////////////////////////////////////////////////////////
#define RELATION_ATTRIB_NAME              _XT("Name")
#define RELATION_ATTRIB_COMMENT           _XT("Comment")
#define RELATION_ATTRIB_TYPE              _XT("Type")
#define RELATION_ATTRIB_PARENTTABLE       _XT("ParentTable")
#define RELATION_ATTRIB_CHILDTABLE        _XT("ChildTable")
#define RELATION_ATTRIB_PARENTUPDATERULE  _XT("ParentUpdateRule")
#define RELATION_ATTRIB_PARENTDELETERULE  _XT("ParentDeleteRule")
#define RELATION_ATTRIB_CHILDUPDATERULE   _XT("ChildUpdateRule")
#define RELATION_ATTRIB_CHILDDELETERULE   _XT("ChildDeleteRule")
#define RELATION_ATTRIB_PARENTFIELDS      _XT("ParentFields")
#define RELATION_ATTRIB_CHILDFIELDS       _XT("ChildFields")
#define RELATION_ATTRIB_FLAGS             _XT("Flags")
#define RELATION_ATTRIB_PERSISTENTDATA    _XT("PersistentData")

///////////////////////////////////////////////////////////////////////////
#define CONNECTION_ATTRIB_NAME            _XT("Name")
#define CONNECTION_ATTRIB_COMMENT         _XT("Comment")
#define CONNECTION_ATTRIB_TYPE            _XT("Type")
#define CONNECTION_ATTRIB_PARENTTABLE     _XT("ParentTable")
#define CONNECTION_ATTRIB_CHILDTABLE      _XT("ChildTable")
#define CONNECTION_ATTRIB_PARENTTYPE      _XT("ParentType")
#define CONNECTION_ATTRIB_CHILDTYPE       _XT("ChildType")
#define CONNECTION_ATTRIB_USERELATIONS    _XT("UseRelations")
#define CONNECTION_ATTRIB_RELATIONSTOUSE  _XT("RelationsToUse")

///////////////////////////////////////////////////////////////////////////
#define PROPERTY_ATTRIB_NAME                          _XT("Name")
#define PROPERTY_ATTRIB_COMMENT                       _XT("Comment")
#define PROPERTY_ATTRIB_HITTABLE                      _XT("HitTable")
#define PROPERTY_ATTRIB_PARAMTABLE                    _XT("ParamTable")
#define PROPERTY_ATTRIB_AUTOR                         _XT("Autor")
#define PROPERTY_ATTRIB_PASSWORD                      _XT("Password")
#define PROPERTY_ATTRIB_IFSTYPEFORTHIRDPARTY          _XT("IFSTypeForThirdParty")
#define PROPERTY_ATTRIB_IFSNAMEFORTHIRDPARTY          _XT("IFSNameForThirdParty")
#define PROPERTY_ATTRIB_SHOWOPENEDENTITIESHIERARCHICAL  _XT("ShowOpenedEntitiesHierarchical")
#define PROPERTY_ATTRIB_SHOWOPENEDNAMEALWAYS          _XT("ShowOpenedNameAlways")
#define PROPERTY_ATTRIB_SHOWEXPANDED                  _XT("ShowExpanded")
#define PROPERTY_ATTRIB_SHOWBEHAVIOUR                 _XT("ShowBehaviour")
#define PROPERTY_ATTRIB_DBNAMEPROD                    _XT("DBNameProduction")
#define PROPERTY_ATTRIB_DBNAMETEST                    _XT("DBNameTest")
#define PROPERTY_ATTRIB_ERMODELNAME                   _XT("ERModelName")
#define PROPERTY_ATTRIB_SOURCEVIEWACTIVE              _XT("SourceViewActive")
#define PROPERTY_ATTRIB_VISIBILITY_TEXT               _XT("VisibilityText")
#define PROPERTY_ATTRIB_VISIBILITY_TEXT_EXPRVALUES    _XT("VisibilityTextExprValues")
#define PROPERTY_ATTRIB_VISIBILITY_TEXT_EXPRSYMBOLS   _XT("VisibilityTextExprSymbols")
#define PROPERTY_ATTRIB_IFSVIEW                       _XT("IFSView")
#define PROPERTY_ATTRIB_USEIFSVIEW                    _XT("UseIFSView")
#define PROPERTY_ATTRIB_IFSTARGETENTITY               _XT("IFSTargetEntity")
#define PROPERTY_ATTRIB_USEIFSTARGETENTITY            _XT("UseIFSTargetEntity")
#define PROPERTY_ATTRIB_STONEORTEMPLATE               _XT("StoneOrTemplate")
#define PROPERTY_ATTRIB_USESTONEORTEMPLATE            _XT("UseStoneOrTemplate")
#define PROPERTY_ATTRIB_STONEORTEMPLATEREF            _XT("StoneOrTemplateRef")
#define PROPERTY_ATTRIB_OVERLOADPROPERTIES            _XT("OverloadProperties")
#define PROPERTY_ATTRIB_OVERLOADPROPERTIESCHANGED     _XT("OverloadPropertiesChanged")
#define PROPERTY_ATTRIB_WPCOPYTYPE                    _XT("WorkPaperCopyType")
#define PROPERTY_ATTRIB_WPPROCESSINGMODE              _XT("WorkPaperProcessingMode")
#define PROPERTY_ATTRIB_ADDONVALUE                    _XT("AddOnValue")
#define PROPERTY_ATTRIB_TAG_TEXT                      _XT("TagText")
#define PROPERTY_ATTRIB_TAG_TEXT_EXPRVALUES           _XT("TagTextExprValues")
#define PROPERTY_ATTRIB_TAG_TEXT_EXPRSYMBOLS          _XT("TagTextExprSymbols")
#define PROPERTY_ATTRIB_TARGETFORTEMPLATE             _XT("TargetForTemplate")
#define PROPERTY_ATTRIB_SAVETEMPLATEINFORMAT          _XT("SaveTemplateInFormat")
#define PROPERTY_ATTRIB_TEMPLATEFILE_ICON             _XT("TemplateFileIcon")
#define PROPERTY_ATTRIB_TEMPLATEFILESHOWNAME_TEXT             _XT("TemplateFileShowNameText")
#define PROPERTY_ATTRIB_TEMPLATEFILESHOWNAME_TEXT_EXPRVALUES  _XT("TemplateFileShowNameTextExprValues")
#define PROPERTY_ATTRIB_TEMPLATEFILESHOWNAME_TEXT_EXPRSYMBOLS _XT("TemplateFileShowNameTextExprSymbols")
#define PROPERTY_ATTRIB_TEMPLATEFILE_TEXT             _XT("TemplateFileText")
#define PROPERTY_ATTRIB_TEMPLATEFILE_TEXT_EXPRVALUES  _XT("TemplateFileTextExprValues")
#define PROPERTY_ATTRIB_TEMPLATEFILE_TEXT_EXPRSYMBOLS _XT("TemplateFileTextExprSymbols")
#define PROPERTY_ATTRIB_TEMPLATEFOLDER_TEXT             _XT("TemplateFolderText")
#define PROPERTY_ATTRIB_TEMPLATEFOLDER_TEXT_EXPRVALUES  _XT("TemplateFolderTextExprValues")
#define PROPERTY_ATTRIB_TEMPLATEFOLDER_TEXT_EXPRSYMBOLS _XT("TemplateFolderTextExprSymbols")
#define PROPERTY_ATTRIB_TEMPLATESYMBOLICDBNAME        _XT("SymbolicDBName")
#define PROPERTY_ATTRIB_TEMPLATEDBNAME_TEXT             _XT("TemplateDBNameText")
#define PROPERTY_ATTRIB_TEMPLATEDBNAME_TEXT_EXPRVALUES  _XT("TemplateDBNameTextExprValues")
#define PROPERTY_ATTRIB_TEMPLATEDBNAME_TEXT_EXPRSYMBOLS _XT("TemplateDBNameTextExprSymbols")
#define PROPERTY_ATTRIB_SHOW_REL_NAMES                _XT("ShowRelNames")
#define PROPERTY_ATTRIB_SHOW_HIT_LINES                _XT("ShowHitLines")
#define PROPERTY_ATTRIB_DATAFORMAT                    _XT("DataFormat")
#define PROPERTY_ATTRIB_VIEW1                         _XT("View1")
#define PROPERTY_ATTRIB_VIEW2                         _XT("View2")
#define PROPERTY_ATTRIB_PREPOSTPROCESSSTONE           _XT("PrePostProcessStone")
#define PROPERTY_ATTRIB_GLOBALERTABLENAME             _XT("GlobalERTableName")
#define PROPERTY_ATTRIB_TABLEISTEMPORARY              _XT("TableIsTemporary")
#define PROPERTY_ATTRIB_ASSIGNMENT_TEXT               _XT("AssignmentText")
#define PROPERTY_ATTRIB_ASSIGNMENT_TEXT_EXPRVALUES    _XT("AssignmentTextExprValues")
#define PROPERTY_ATTRIB_ASSIGNMENT_TEXT_EXPRSYMBOLS   _XT("AssignmentTextExprSymbols")
#define PROPERTY_ATTRIB_ASSIGNFIELD_TEXT              _XT("AssignFieldText")
#define PROPERTY_ATTRIB_ASSIGNFIELD_TEXT_EXPRVALUES   _XT("AssignFieldTextExprValues")
#define PROPERTY_ATTRIB_ASSIGNFIELD_TEXT_EXPRSYMBOLS  _XT("AssignFieldTextExprSymbols")
#define PROPERTY_ATTRIB_ASSIGNVALUE_TEXT              _XT("AssignValueText")
#define PROPERTY_ATTRIB_ASSIGNVALUE_TEXT_EXPRVALUES   _XT("AssignValueTextExprValues")
#define PROPERTY_ATTRIB_ASSIGNVALUE_TEXT_EXPRSYMBOLS  _XT("AssignValueTextExprSymbols")

#define VARIABLE_PROPERTY_ATTRIB_NAME                               _XT("Name")
#define VARIABLE_PROPERTY_ATTRIB_COMMENT                            _XT("Comment")
#define VARIABLE_PROPERTY_ATTRIB_TYPE                               _XT("Type")
#define VARIABLE_PROPERTY_ATTRIB_DEFINEDSIZE                        _XT("DefinedSize")

#define ASSIGNMENT_PROPERTY_ATTRIB_NAME                             _XT("Name")
#define ASSIGNMENT_PROPERTY_ATTRIB_TABLE                            _XT("Table")

///////////////////////////////////////////////////////////////////////////
#define STONETEMPLATE_PROPERTY_ATTRIB_NAME                              _XT("Name")
#define STONETEMPLATE_PROPERTY_ATTRIB_COMMENT                           _XT("Comment")
#define STONETEMPLATE_PROPERTY_ATTRIB_TEXTTABLE                         _XT("TextTable")
#define STONETEMPLATE_PROPERTY_ATTRIB_TEXTCOLUMN                        _XT("TextColumn")
#define STONETEMPLATE_PROPERTY_ATTRIB_CONTENT_TEXT                      _XT("ContentText")
#define STONETEMPLATE_PROPERTY_ATTRIB_CONTENT_TEXT_EXPRVALUES           _XT("ContentTextExprValues")
#define STONETEMPLATE_PROPERTY_ATTRIB_CONTENT_TEXT_EXPRSYMBOLS          _XT("ContentTextExprSymbols")
#define STONETEMPLATE_PROPERTY_ATTRIB_CONTENT_TEXT_MASK                 _XT("ContentTextMask")
#define STONETEMPLATE_PROPERTY_ATTRIB_ADDRESS_TEXT                      _XT("Address")
#define STONETEMPLATE_PROPERTY_ATTRIB_ADDRESS_TEXT_EXPRVALUES           _XT("AddressExprValues")
#define STONETEMPLATE_PROPERTY_ATTRIB_ADDRESS_TEXT_EXPRSYMBOLS          _XT("AddressExprSymbols")
#define STONETEMPLATE_PROPERTY_ATTRIB_QUICKINFO_TEXT                    _XT("QuickInfoText")
#define STONETEMPLATE_PROPERTY_ATTRIB_QUICKINFO_TEXT_EXPRVALUES         _XT("QuickInfoTextExprValues")
#define STONETEMPLATE_PROPERTY_ATTRIB_QUICKINFO_TEXT_EXPRSYMBOLS        _XT("QuickInfoTextExprSymbols")
#define STONETEMPLATE_PROPERTY_ATTRIB_TARGETFRAME                       _XT("TargetFrame")
#define STONETEMPLATE_PROPERTY_ATTRIB_PYTHONFUNCTION_TEXT               _XT("PythonFunctionText")
#define STONETEMPLATE_PROPERTY_ATTRIB_PYTHONFUNCTION_TEXT_EXPRVALUES    _XT("PythonFunctionTextExprValues")
#define STONETEMPLATE_PROPERTY_ATTRIB_PYTHONFUNCTION_TEXT_EXPRSYMBOLS   _XT("PythonFunctionTextExprSymbols")
#define STONETEMPLATE_PROPERTY_ATTRIB_PICFILENAMELOCAL_TEXT             _XT("PicFileNameLocalText")
#define STONETEMPLATE_PROPERTY_ATTRIB_PICFILENAMELOCAL_TEXT_EXPRVALUES  _XT("PicFileNameLocalTextExprValues")
#define STONETEMPLATE_PROPERTY_ATTRIB_PICFILENAMELOCAL_TEXT_EXPRSYMBOLS _XT("PicFileNameLocalTextExprSymbols")
#define STONETEMPLATE_PROPERTY_ATTRIB_PICFILENAME_TEXT                  _XT("PicFileNameText")
#define STONETEMPLATE_PROPERTY_ATTRIB_PICFILENAME_TEXT_EXPRVALUES       _XT("PicFileNameTextExprValues")
#define STONETEMPLATE_PROPERTY_ATTRIB_PICFILENAME_TEXT_EXPRSYMBOLS      _XT("PicFileNameTextExprSymbols")
#define STONETEMPLATE_PROPERTY_ATTRIB_PICSIZEINPERCENT                  _XT("PicSizeInPercent")
#define STONETEMPLATE_PROPERTY_ATTRIB_PICWIDTH                          _XT("PicWidth")
#define STONETEMPLATE_PROPERTY_ATTRIB_PICHEIGHT                         _XT("PicHeight")

///////////////////////////////////////////////////////////////////////////

#define PROJ_OPENED_MODULES         _XT("OpenedModules")
#define OPENED_MODULE_ELEMENT       _XT("OpenedModule")
#define OPENED_MODULE_TYPE          _XT("Type")
#define OPENED_MODULE_NAME          _XT("Name")
#define ACTIVE_MODULE_TYPE          _XT("ActiveModuleType")
#define ACTIVE_MODULE_NAME          _XT("ActiveModuleName")

#define PROJ_ZOOMFACTORS_ER         _XT("ZoomFactors-ER")
#define PROJ_ZOOMFACTORS_IFS        _XT("ZoomFactors-IFS")
#define ZOOMFACTOR_ELEMENT          _XT("ZoomFactor")
#define ZOOMFACTOR_MODULE_NAME      _XT("ZoomFactorModuleName")
#define ZOOMFACTOR_ZOOM_FACTOR      _XT("ZoomFactorZoomFactor")

#define PROJ_LASTFILTER             _XT("LastFilter")
#define PROJ_LASTFILTER_IFS         _XT("LastFilterIFS")
#define PROJ_LASTIFS                _XT("LastIFS")
#define PROJ_LASTIFSFOREXEC         _XT("LastIFSForExec")






///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// log file
#define EMPTY_LOG_FILE_XML_DOCUMENT      _XT("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<X-Publisher-LogFile></X-Publisher-LogFile>\r\n")
#define MAIN_TAG_LOG_FILE                _XT("X-Publisher-LogFile")

#define LOG_OUT_MESSAGES              _XT("OutMessages")
#define LOG_HIT_MESSAGES              _XT("HitMessages")
#define LOG_OUT_ENTRY                 _XT("OutEntry")
#define LOG_HIT_ENTRY_REALISED        _XT("HitEntryRealised")
#define LOG_HIT_ENTRY_NOTREALISED     _XT("HitEntryNotRealised")
#define LOG_OUT_HIT_X_POINT           _XT("OutHit_X_Point")
#define LOG_OUT_TYPE_OF_MESSAGE       _XT("TypeOfMsg")
#define LOG_OUT_SOURCE_OF_MESSAGE     _XT("SourceOfMsg")
#define LOG_OUT_MODULE                _XT("Module")
#define LOG_OUT_ENTITY                _XT("Entity")
#define LOG_OUT_WORKPAPER             _XT("WorkPaper")
#define LOG_OUT_MSG_KEYWORD           _XT("MsgKeyword")
#define LOG_OUT_MSG_TEXT              _XT("MsgText")
#define LOG_OUT_MSG_DESCRIPTION       _XT("MsgDescription")
#define LOG_HIT_ENTITY_NAME           _XT("EntityName")
#define LOG_HIT_WPNAME                _XT("WPName")
#define LOG_HIT_WPCOMMENT             _XT("WPComment")
#define LOG_HIT_TARGET_IFS_NAME       _XT("TargetInformationSight")
#define LOG_HIT_TARGET_ENTITY_NAME    _XT("TargetEntityName")
#define LOG_HIT_TARGET_WPNAME         _XT("TargetWPName")
#define LOG_HIT_PLACEHOLDER           _XT("PlaceHolder")
#define LOG_HIT_WPCOPYTYPE            _XT("WorkPaperCopyType")
#define LOG_HIT_WPPROCESSINGMODE      _XT("WorkPaperProcessingMode")
#define LOG_HIT_WPTYPE                _XT("WPType")
#define LOG_HIT_TEMPLATETARGET        _XT("TemplateTarget")
#define LOG_HIT_SAVETEMPLATEINFORMAT  _XT("SaveTemplateInFormat")









// constants tree
#define STANDALONE_CONSTANTS        _XT("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<StandAloneConstants></StandAloneConstants>\r\n")
#define STANDALONE_CONSTANTS_TAG    _XT("StandAloneConstants")

#define PROJ_CONSTANTSALL_NAME          _XT("ProjectConstants")
#define PROJ_CONSTANTS_TEST             _XT("ProjectConstantsTest")
#define PROJ_CONSTANTS_PRODUCTION       _XT("ProjectConstantsProduction")
#define PROJECTCONSTANT_ELEMENT         _XT("ProjectConstant")
#define PROJECTCONSTANT_ATTRIB_NAME               _XT("Name")
#define PROJECTCONSTANT_ATTRIB_EXPRESSION         _XT("Expression")
#define PROJECTCONSTANT_ATTRIB_EXPRESSIONSYMBOLS  _XT("ExprSymbols")
#define PROJECTCONSTANT_ATTRIB_EXPRESSIONVALUES   _XT("ExprValues")

// prop stone
#define EMPTY_PROPSTONE_XML                       _XT("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<PropStoneFile></PropStoneFile>\r\n")
#define MAIN_TAG_PROPSTONE                        _XT("PropStoneFile")
#define EMPTY_PROPSTONES_XML                      _XT("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<PropStones></PropStones>\r\n")
#define MAIN_TAG_PROPSTONES                       _XT("PropStones")

#define PROPSTONE_ELEM_NAME                       _XT("PropStone")
#define PROPSTONE_PROPERTIES_ELEM_NAME            _XT("Properties")
#define PROPSTONE_PROPERTIES_PROPGROUP            _XT("PropGroup")
#define PROPSTONE_PROPERTY                        _XT("Property")

#define PROPSTONE_PROPERTY_EXPRESSION             _XT("Expression")
#define PROPSTONE_PROPERTY_EXPRESSIONVALUES       _XT("ExpressionValues")
#define PROPSTONE_PROPERTY_EXPRESSIONSYMBOLS      _XT("ExpressionSymbols")
#define PROPSTONE_PROPERTY_INTERNALNAME           _XT("InternalName")
#define PROPSTONE_PROPERTY_PROPERTY               _XT("Property")
#define PROPSTONE_PROPERTY_PROPERTYTYPE           _XT("PropertyType")
#define PROPSTONE_PROPERTY_PROPERTYFUNCTION       _XT("PropertyFunction")
#define PROPSTONE_PROPERTY_VALUETYPE              _XT("ValueType")
#define PROPSTONE_PROPERTY_VALUEVALUE             _XT("ValueValue")


// db stone
#define EMPTY_DBSTONE_XML                       _XT("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<DBStoneFile></DBStoneFile>\r\n")
#define MAIN_TAG_DBSTONE                        _XT("DBStoneFile")
#define EMPTY_DBSTONES_XML                      _XT("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<DBStones></DBStones>\r\n")
#define MAIN_TAG_DBSTONES                       _XT("DBStones")

#define DBSTONE_CONTAINER_ELEM_NAME             _XT("DBStoneInContainer")
#define DBSTONE_CONTAINER_STONE_NAME            _XT("DBStoneNameInContainer")
#define DBSTONE_ELEM_NAME                       _XT("DBStone")
#define DBSTONE_ATTRIB_ERMODELNAME              _XT("ERModel")
#define DBSTONE_ATTRIB_TABLE                    _XT("Table")
#define DBSTONE_ATTRIB_PROCESSINGTYPE           _XT("ProcessingType")
#define DBSTONE_FIELDS_ELEM_NAME                _XT("Fields")
#define DBSTONEFIELD_ELEM_NAME                  _XT("Field")
#define DBSTONEFIELD_ATTRIB_FIELDNAME           _XT("FieldName")
#define DBSTONEFIELD_ATTRIB_FIELDISKEY          _XT("FieldIsKey")
#define DBSTONEFIELD_ATTRIB_VARIANTTYPE         _XT("VariantType")
#define DBSTONEFIELD_ATTRIB_VARIANTVALUE        _XT("VariantValue")
#define DBSTONEFIELD_ATTRIB_VALUE               _XT("Value")
#define DBSTONEFIELD_ATTRIB_VALUEEXPRVALUES     _XT("ValueExprValues")
#define DBSTONEFIELD_ATTRIB_VALUEEXPRSYMBOLS    _XT("ValueExprSymbols")


// db template
#define EMPTY_DBTEMPLATE_XML                          _XT("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<DBTemplateFile></DBTemplateFile>\r\n")
#define MAIN_TAG_DBTEMPLATE                           _XT("DBTemplateFile")
#define DBTEMPLATE_ELEM_NAME                          _XT("DBTemplate")
#define DBTEMPLATE_ATTRIB_ERMODELNAME                 _XT("ERModel")
#define DBTEMPLATE_ATTRIB_DROPTABLES                  _XT("DropTables")
#define DBTEMPLATE_ATTRIB_DELETEDBAFTERNOHIT          _XT("DeleteDBAfterNoHit")
#define DBTEMPLATE_ATTRIB_DELETEDBTABLESAFTERNOHIT    _XT("DeleteDBTablesAfterNoHit")
#define DBTEMPLATE_TABLES_ELEM_NAME                   _XT("Tables")
#define DBTEMPLATETABLE_ELEM_NAME                     _XT("Table")
#define DBTEMPLATETABLE_ATTRIB_TABLENAME              _XT("TableName")
#define DBTEMPLATETABLE_ATTRIB_CREATETABLE            _XT("CreateTable")
#define DBTEMPLATETABLE_ATTRIB_VALUE                  _XT("Value")
#define DBTEMPLATETABLE_ATTRIB_VALUEEXPRVALUES        _XT("ValueExprValues")
#define DBTEMPLATETABLE_ATTRIB_VALUEEXPRSYMBOLS       _XT("ValueExprSymbols")


// one condition variation
#define XMLTAG_ONECONDVARIATION_NAME                _XT("Name")
#define XMLTAG_ONECONDVARIATION_DESCRIPTION         _XT("Description")
#define XMLTAG_ONECONDVARIATION_EXPRESSION          _XT("Expression")
#define XMLTAG_ONECONDVARIATION_EXPRVALUES          _XT("ExprValues")
#define XMLTAG_ONECONDVARIATION_EXPRSYMBOLS         _XT("ExprSymbols")
#define XMLTAG_ONECONDVARIATION_EXPROFFSETS         _XT("ExprOffsets")
#define XMLTAG_ONECONDVARIATION_DONTOPTIMIZECONDITION _XT("DontOptimizeCondition")
#define XMLTAG_ONECONDVARIATION_DONTSHOWCONDITION   _XT("DontShowCondition")
#define XMLTAG_ONECONDVARIATION_CONDITIONSHOWMETHOD _XT("ConditionShowMethod")
#define XMLTAG_ONECONDVARIATION_TEXTFORMOFCONDITION _XT("TextFormOfCondition")
#define XMLTAG_ONECONDVARIATION_CONSTANTS           _XT("Constants")
#define XMLTAG_ONECONDVARIATION_CONSTANT            _XT("Constant")


// param for text form
#define XMLTAG_PARAMETERS_PARAMETER_NAME              _XT("Name")
#define XMLTAG_PARAMETERS_PARAMETER_SHOWPARAMETER     _XT("ShowParameter")
#define XMLTAG_PARAMETERS_PARAMETER_EXPRTEXT          _XT("ExprText")
#define XMLTAG_PARAMETERS_PARAMETER_EXPRVALUES        _XT("ExprValues")
#define XMLTAG_PARAMETERS_PARAMETER_EXPRSYMBOLS       _XT("ExprSymbols")
#define XMLTAG_PARAMETERS_PARAMETER_EXPROFFSETS       _XT("ExprOffsets")
#define XMLTAG_PARAMETERS_PARAMETER_FIELDTYPE         _XT("FieldType")
#define XMLTAG_PARAMETERS_PARAMETER_FORMAT            _XT("Format")
#define XMLTAG_PARAMETERS_PARAMETER_MAXCHARS          _XT("MaxChars")
#define XMLTAG_PARAMETERS_PARAMETER_BACKGROUND        _XT("Background")
#define XMLTAG_PARAMETERS_PARAMETER_FOREGROUND        _XT("Foreground")
#define XMLTAG_PARAMETERS_PARAMETER_ALLOWNULL         _XT("AllowNull")
#define XMLTAG_PARAMETERS_PARAMETER_BORDER            _XT("Border")
#define XMLTAG_PARAMETERS_PARAMETER_DEFAULTTEXT       _XT("DefaultText")
#define XMLTAG_PARAMETERS_PARAMETER_FONTNAME          _XT("FontName")
#define XMLTAG_PARAMETERS_PARAMETER_FONTHEIGHT        _XT("FontHeight")
#define XMLTAG_PARAMETERS_PARAMETER_VALUE             _XT("Value")


// key field
#define KEYVALUE_KEYVALUES                _XT("KeyValues")
#define KEYVALUE_KEYVALUE                 _XT("KeyValue")
#define KEYVALUE_KEYCOLUMN                _XT("KeyColumn")
#define KEYVALUE_KEYVALUE_TEXT            _XT("KeyValueText")
#define KEYVALUE_KEYVALUE_TEXTEXPRVALUES  _XT("KeyValueTextExprValues")
#define KEYVALUE_KEYVALUE_TEXTEXPRSYMBOLS _XT("KeyValueTextExprSymbols")


// hierarchy item
#define HIERARCHYITEM_OPENEDNAME            _XT("OpenedName%ld")
#define HIERARCHYITEM_NAME                  _XT("Name%ld")
#define HIERARCHYITEM_TYPE                  _XT("Type%ld")
#define HIERARCHYITEM_SHOWSUBIFS            _XT("ShowSubIFS%ld")
#define HIERARCHYITEM_SHOWHIERARCHICAL      _XT("ShowHierarchical%ld")
#define HIERARCHYITEM_SHOWEXPANDED          _XT("ShowExpanded%ld")
#define HIERARCHYITEM_SHOWBEHAVIOUR         _XT("ShowBehaviour%ld")
#define HIERARCHYITEM_SHOWADDCONDITION      _XT("ShowAddCondition%ld")
#define HIERARCHYITEM_SHOWADDFREESELECTION  _XT("ShowAddFreeSelection%ld")

#define HIERARCHYITEM_COND_TYPE             _XT("ConditionType")
#define HIERARCHYITEM_COND_INDEX            _XT("ConditionIndex")
#define HIERARCHYITEM_COND_NAME             _XT("ConditionName")
#define HIERARCHYITEM_COND_EXPR             _XT("ConditionExpression")
#define HIERARCHYITEM_COND_EXPRVALUES       _XT("ConditionExprValues")
#define HIERARCHYITEM_COND_EXPRSYMBOLS      _XT("ConditionExprSymbols")
#define HIERARCHYITEM_COND_DONTOPTIMIZECONDITION  _XT("DontOptimizeCondition")

#define HIERARCHYITEM_SB_SHOWBEHAVIOUR      _XT("ShowBehaviour")
#define HIERARCHYITEM_SHOWEXP_SHOWEXPANDED  _XT("ShowExpanded")



#endif // !defined(_XMLTAGNAMES_H_)
