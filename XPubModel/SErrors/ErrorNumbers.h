

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// general
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define ERR_XPUB_NO_ERROR                                             0
#define ERR_NEW_PROBLEM                                               1
#define ERR_ALLOC_PROBLEM                                             2
#define ERR_NULL_POINTER_AS_PARAMETER                                 3
#define ERR_ELEMENT_NOT_ADDED_IN_ARRAY                                4
#define ERR_REMOVE_ELEMENT_BUT_NOT_PRESENT_IN_ARRAY                   5

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// CExpression Exceptions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define ERR_EXPR_FIRST_ERROR                                          100
#define ERR_EXPR_NOERROR_SETRESULT_FALSE                              101
#define ERR_EXPR_NOERROR_SETRESULT_TRUE                               102
#define ERR_EXPR_NOERROR_SETRESULT_EMPTYSTRING                        103
#define ERR_EXPR_NOERROR_SETRESULT_LONG0                              104
#define ERR_EXPR_DIVIDEBYZERO																					105
#define ERR_EXPR_MALFORMEDXML																					106
#define ERR_EXPR_VARIABLEUNASSIGNED																		107
#define ERR_EXPR_SCRIPTING_ARGUMENT_FAILED                            108
#define ERR_EXPR_SCRIPTING_FUNCTIONCALL_FAILED                        109
#define ERR_EXPR_SCRIPTING_CANNOTFIND_FUNCTION                        110
#define ERR_EXPR_SCRIPTING_CANNOTFIND_MODULE                          111
#define ERR_EXPR_SCRIPTING_LIBRARY_NOT_INITIALIZED                    112
#define ERR_EXPR_SCRIPTING_UNDEFINED                                  113
#define ERR_EXPR_SCRIPTING_MODULE_NOTFOUND	                          114
#define ERR_EXPR_ERROR_WITHOUT_MESSAGE                                115
#define ERR_EXPR_VARIABLE_CONTENT_NOTFIELD														116		// the content of the variable is not a field

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// AddOnManager und AddOns
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// AddOn FormatWorker errors
#define ERR_ADDON_ADDWORKER_WITH_NULL_POINTER                             10000
#define ERR_ADDON_UNREGISTER_ADDON_WITH_NULL_POINTER                      10001
#define ERR_ADDON_CREATEWORKER_WITH_NULL_POINTER                          10002
#define ERR_ADDON_CREATEWORKERCLONE_WITH_NULL_POINTER                     10003
#define ERR_ADDON_RELEASEWORKER_WITH_NULL_POINTER                         10004
#define ERR_ADDON_CREATECONTROL_FAILED                                    10005
#define ERR_ADDON_NEWWORKER_NOT_CREATED                                   10006
#define ERR_ADDON_CREATEWORKERCLONE_FROM_NULL_POINTER                     10007
#define ERR_ADDON_RELEASEWORKER_FROM_NULL_POINTER                         10008
#define ERR_ADDON_INSERTWORKPAPER_NULL_POINTER_AS_OBJECT                  10009
#define ERR_ADDON_OPERATIONFORTHISSTONETEMPLATECOMBINATION_NOT_ALLOWED    10010
#define ERR_ADDON_FORMAT_MISMATCH                                         10011
#define ERR_ADDON_TEXTFIELD_PLACEHOLDER_NOT_FOUND                         10012
#define ERR_ADDON_AREAFIELD_PLACEHOLDER_NOT_FOUND                         10013
#define ERR_ADDON_LINKFIELD_PLACEHOLDER_NOT_FOUND                         10014
#define ERR_ADDON_LINKFIELD_LEFT_PARENTHESIS_FOUND_BUT_RIGHT_NOT          10015
#define ERR_ADDON_REPLACE_TEXT_FIELD_NOT_SUPPORTED                        10016
#define ERR_ADDON_REPLACE_AREA_FIELD_NOT_SUPPORTED                        10017
#define ERR_ADDON_REPLACE_LINK_FIELD_NOT_SUPPORTED                        10018
#define ERR_ADDON_REPLACE_GRAPHIC_FIELD_NOT_SUPPORTED                     10019
#define ERR_ADDON_FILE_NOT_CREATED_OR_OPENED_FOR_WRITE                    10020
#define ERR_ADDON_NEWWORKER_FOR_CLONE_NOT_CREATED                         10021
#define ERR_ADDON_CLONEWORKER_INIT_FAILED                                 10022
#define ERR_ADDON_REPLACE_CIRCULAR_ERROR                                  10023
#define ERR_ADDON_REPLACE_MAXCOUNT_REACHED                                10024

#define ERR_ADDON_CONVERTWORKER_FROMTAG_NOTFOUND                          10050
#define ERR_ADDON_CONVERTWORKER_TORTF_NOTFOUND                            10051
#define ERR_ADDON_CONVERTWORKER_TOPDF_NOTFOUND                            10052




