/*==============================================================================
   TER_OLE.C
   TER Object Linking and Embedding functions

   TER Editor Developer's Kit    
   Sub Systems, Inc.
   Software License Agreement  (1994)              
   ----------------------------------
   This license agreement allows the purchaser the right to modify the 
   source code to incorporate in an application larger than the editor itself.
   The target application must not be a programmer's utility 'like' a text editor.
   Sub Systems, Inc. reserves the right to prosecute anybody found to be 
   making illegal copies of the executable software or compiling the source
   code for the purpose of selling there after.
===============================================================================*/

#define  PREFIX extern
#define  INITGUID
#define  NONAMELESSUNION

#include "ter_hdr.h"

#if !defined (NO_OLE)

/*==============================================================================
   TER_OLE.C
   TER Object Linking and Embedding functions

   TER Editor Developer's Kit
   Sub Systems, Inc.
   Software License Agreement  (1994)
   ----------------------------------
   This license agreement allows the purchaser the right to modify the
   source code to incorporate in an application larger than the editor itself.
   The target application must not be a programmer's utility 'like' a text editor.
   Sub Systems, Inc. reserves the right to prosecute anybody found to be
   making illegal copies of the executable software or compiling the source
   code for the purpose of selling there after.
===============================================================================*/

#if !defined(WIN32) || (defined(__TURBOC__) && __TURBOC__ < 0x460)
  #include "coguid.h"
  #include "oleguid.h"
#else
  #include "cguid.h"
  #include "olectl.h"
#endif

#if defined(__TURBOC__) && defined(WIN32)
  #if defined(__FLAT__)
    #include "olectl.h"
  #else
    #include "olectlid.h"
  #endif
#endif

#include "ole2ver.h"


/*******************************************************************************
    InitOle:
    Create the OLE client structure
*******************************************************************************/
BOOL InitOle(PTERWND w)
{
    DWORD OleVer;
    HRESULT result;
    int i,TaskIdx;
    HTASK CurTask;

    if (TerFlags&TFLAG_NO_OLE) return TRUE;

    // check if OLE already initialized for this task
    CurTask=GetWindowTask(hTerWnd);
    for (i=0;i<TotalWinTasks;i++) if (WinTask[i].count>0 && WinTask[i].task==CurTask) break;
    if (i==TotalWinTasks) {  // look for an unused slot
       for (i=0;i<TotalWinTasks;i++) if (WinTask[i].count==0) break;
    }
    if (i==TotalWinTasks) {
       if (TotalWinTasks<MAX_TASKS) {
          i=TotalWinTasks;
          WinTask[i].count=0;
          TotalWinTasks++;
       }
       else return PrintError(w,MSG_OUT_OF_TASK,"");
    }

    // update the task slot
    TaskIdx=i;
    WinTask[TaskIdx].task=CurTask;
    WinTask[TaskIdx].count++;

    // initialize OLE
    if (WinTask[TaskIdx].count==1) {      // first window for this application instance
       OleVer=OleBuildVersion();
       if (HIWORD(OleVer) !=rmm || LOWORD(OleVer) < rup) {
          return PrintError(w,MSG_ERR_OLE_VERSION,"");
       }

       result=OleInitialize(NULL);
       if (GetScode(result)!=S_OK && GetScode(result)!=S_FALSE) {
          PrintOleError(w,result);
          return PrintError(w,MSG_ERR_INIT_OLE,"");
       }

       // create and open a document storage - created one per DLL task
       result=StgCreateDocfile(NULL,STGM_TRANSACTED|STGM_READWRITE
                 |STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_DELETEONRELEASE,0,&(WinTask[TaskIdx].pDocStg));
       if (result!=NOERROR) {
          PrintOleError(w,result);
          return PrintError(w,MSG_ERR_STORAGE_CREATE,"");
       }
    }

    pDocStorage=WinTask[TaskIdx].pDocStg;  // use it from the table


    // create the OLE1 compatible streams
    OleStreamVtbl.Get=NULL;     // Ole stream get callback function
    OleStreamVtbl.Put=NULL;     // Ole stream put callback function

    // Create the client structure
    if ( NULL==(OleStreamVtbl.Get=(LPVOID) MakeProcInstance((FARPROC)TerOleGet,hTerInst))
      || NULL==(OleStreamVtbl.Put=(LPVOID) MakeProcInstance((FARPROC)TerOlePut,hTerInst)) ){
       return PrintError(w,MSG_ERR_INSTANCE,"InitOle");
    }

    OleStream.stream.lpstbl=&OleStreamVtbl;
    OleStream.TerData=w;

    // enable drag/drop
    DragAcceptFiles(hTerWnd,!TerArg.ReadOnly);

    if (!TerArg.ReadOnly && !(TerFlags5&TFLAG5_NO_EXT_DROP)) RegisterDragDrop(hTerWnd,(LPDROPTARGET)CreateDropTarget(w));

    return TRUE;
}

/*******************************************************************************
    ExitOle:
    Exit ole logic. Called by CloseTer
*******************************************************************************/
BOOL ExitOle(PTERWND w)
{
    int i;
    HTASK CurTask;

    if (TerFlags&TFLAG_NO_OLE) return TRUE;

    #if !defined(WIN32)
       if (OleStreamVtbl.Get)      FreeProcInstance((FARPROC)OleStreamVtbl.Get);
       if (OleStreamVtbl.Put)      FreeProcInstance((FARPROC)OleStreamVtbl.Put);
    #endif
    
    if (!TerArg.ReadOnly) RevokeDragDrop(hTerWnd);
    DragAcceptFiles(hTerWnd,FALSE);

    // check if the last window for the task
    CurTask=GetWindowTask(hTerWnd);
    for (i=0;i<TotalWinTasks;i++) if (WinTask[i].count>0 && WinTask[i].task==CurTask) break;

    if (i<TotalWinTasks) {
      WinTask[i].count--;

      if (WinTask[i].count==0) {
         if (WinTask[i].pDocStg) (WinTask[i].pDocStg)->lpVtbl->Release(WinTask[i].pDocStg);
         OleUninitialize();
      }
    }

    return TRUE;
}

/*******************************************************************************
    OlePostProcessing:
    This function gets the data from the application's persistence stroage to
    the IStorage interface residing in OLE2.DLL.
*******************************************************************************/
OlePostProcessing(PTERWND w)
{
    if (TerFlags&TFLAG_NO_OLE) return TRUE;
    return TRUE;
}

/*******************************************************************************
    TerPasteSpecial:
    Accept a string from the user along with other parameters and search for
    the first instances of the string.
*******************************************************************************/
TerPasteSpecial(PTERWND w)
{
    int index,len,i;
    int select;
    UINT ClipFormat,PasteFormat;
    long BuffSize;
    BYTE class[MAX_WIDTH+1],doc[MAX_WIDTH+1],item[MAX_WIDTH+1],PasteFmts[MAX_PASTE_FORMATS];
    BOOL PasteLink=FALSE;

    if (TerFlags&TFLAG_NO_OLE) return TRUE;

    if (FontsReleased) RecreateFonts(w,hTerDC);    // recreate fonts

    // Build the source description string
    ClipFormat=GetOleClipItem(w,class,doc,item);

    if (ClipFormat>0) {
       // get the class name
       BuffSize=sizeof(TempString);
       RegQueryValue(HKEY_CLASSES_ROOT,class,TempString,&BuffSize);

       // Build the item string
       len=lstrlen(doc);
       for (i=len;i>0;i--) if (doc[i-1]=='\\') break;
       lstrcpy(TempString1,&(doc[i]));
       if (lstrlen(TempString1)>0) lstrcat(TempString1," ");
       lstrcat(TempString1,item);
    }
    else {
       lstrcpy(TempString,MsgString[MSG_UNKNOWN]);
       lstrcpy(TempString1,"");
    }

    // build the datatype table
    for (i=0;i<MAX_PASTE_FORMATS;i++) PasteFmts[i]=FALSE;    // list of available formats
    if (IsClipboardFormatAvailable(OwnerLinkClipFormat)) PasteFmts[PASTE_OBJECT]=TRUE;
    if (IsClipboardFormatAvailable(CF_METAFILEPICT))     PasteFmts[PASTE_PICT]=TRUE;
    if (IsClipboardFormatAvailable(CF_BITMAP))           PasteFmts[PASTE_BM]=TRUE;
    if (IsClipboardFormatAvailable(CF_DIB))              PasteFmts[PASTE_DIB]=TRUE;
    if (IsClipboardFormatAvailable(RtfClipFormat))       PasteFmts[PASTE_RTF]=TRUE;
    if (IsClipboardFormatAvailable(CF_TEXT))             PasteFmts[PASTE_TEXT]=TRUE;

    // check if atleast one format available
    for (i=0;i<MAX_PASTE_FORMATS;i++) if (PasteFmts[i]) break;
    if (i==MAX_PASTE_FORMATS) return TRUE;             // no formats available

    index=CallDialogBox(w,"PasteSpecialParam",PasteSpecialParam,(long)(LPBYTE)PasteFmts);
    if (index<0) return TRUE;                          // operation cancelled

    // extract the selected clipboard format
    if (index>=MAX_PASTE_FORMATS) {                    // Is object linking selected
       index-=MAX_PASTE_FORMATS;
       PasteLink=TRUE;
    }
    for (i=0;i<MAX_PASTE_FORMATS;i++) {
       if (PasteFmts[i]) index--;
       if (index<0) break;
    }
    select=i;                                           // selected format

    // get the paste format
    if      (select==PASTE_TEXT)   PasteFormat=CF_TEXT;
    else if (select==PASTE_RTF)    PasteFormat=RtfClipFormat;
    else if (select==PASTE_PICT)   PasteFormat=CF_METAFILEPICT;
    else if (select==PASTE_DIB)    PasteFormat=CF_DIB;
    else if (select==PASTE_BM)     PasteFormat=CF_BITMAP;
    else                           PasteFormat=NativeClipFormat;

    // Retrieve the text formats from the clipboard
    if (select==PASTE_RTF || select==PASTE_TEXT)  return CopyFromClipboard(w,PasteFormat);

    // Process the ole objects
    if (select==PASTE_OBJECT)  return TerOleFromClipboard(w,PasteFormat,PasteLink);

    // Process the non-ole picture formats
    if (select==PASTE_PICT || select==PASTE_DIB || select==PASTE_BM)  return TerPastePicture(hTerWnd,PasteFormat,NULL,0,ALIGN_BOT,TRUE);

    return TRUE;
}

/*******************************************************************************
    TerOleFromClipboard:
    Embed an OLE object from clipboard.
*******************************************************************************/
TerOleFromClipboard(PTERWND w, UINT format,BOOL PasteLink)
{
    int obj;
    LPDATAOBJECT pDataObject;
    FORMATETC OleFmt;
    LPSTORAGE pObjStg;
    LPOLECLIENTSITE pObjSite;
    STGMEDIUM med;
    HRESULT result;

    if (TerFlags&TFLAG_NO_OLE) return TRUE;

    // Erase any previously highlighted text
    if (HilightType==HILIGHT_CHAR) TerDeleteBlock(hTerWnd,TRUE);
    UndoRef--;                    // restore the undo reference

    if ((obj=FindOpenSlot(w))==-1) return FALSE;  // find an empty slot

    // get the data object from the clipboard
    result=OleGetClipboard(&pDataObject);
    if (result!=NOERROR) return PrintError(w,MSG_CLIPBOARD_NO_DATA,"TerOleFromClipboard");

    // create IOleClientSite and IStorage interfaces for the object
    if (!TerOlePreCreate(w,obj,&pObjSite,&pObjStg,&OleFmt)) return FALSE;

    // Create the object
    if (PasteLink) result=OleCreateLinkFromData(pDataObject,&IID_IOleObject,OLERENDER_FORMAT,&OleFmt,pObjSite,pObjStg,(LPVOID far *)&(TerFont[obj].pObject));
    else           result=OleCreateFromData(pDataObject,&IID_IOleObject,OLERENDER_FORMAT,&OleFmt,pObjSite,pObjStg,(LPVOID far *)&(TerFont[obj].pObject));
    if (result!=NOERROR || !(TerFont[obj].pObject)) {
       pObjSite->lpVtbl->Release(pObjSite);    // release the client site
       PrintOleError(w,result);
       return PrintError(w,MSG_NO_SERVER,"");
    }

    // make advisory connection and display the object
    TerOlePostCreate(w,obj);

    // retrieve the presentation data
    if (NOERROR!=(pDataObject->lpVtbl->GetData(pDataObject,&OleFmt,&med)) || med.tymed!=TYMED_MFPICT) {
       return PrintError(w,MSG_ERR_PRESENT_DATA,"TerOleFromClipboard");
    }
    if (TerCreateMetafile(w,med.u.hGlobal,obj)<0) return FALSE;   // extract metafile
    ReleaseStgMedium(&med);                      // release data handle
    pDataObject->lpVtbl->Release(pDataObject);   // this data object needs to be released

    SetOlePictOffset(w,obj);                     // retrieve picture offset from the metafile
    
    if (PasteLink) TerFont[obj].ObjectType=OBJ_LINK;
    else           TerFont[obj].ObjectType=OBJ_EMBED;

    TerInsertObjectId(hTerWnd,obj,TRUE);   // insert this object into text
    FitPictureInFrame(w,CurLine,FALSE);

    return TRUE;
}

/******************************************************************************
     TerSetNoDropList:
     The list of extension for the file that should not be dropped into the editor.
     The list should included the extensions delimited by a comma:  ".exe,.c"
******************************************************************************/
BOOL WINAPI _export TerSetNoDropList(HWND hWnd,LPBYTE exts)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return false;  // get the pointer to window data

    lstrcpy(NoDropList,exts);

    return true;
}
 
/******************************************************************************
     TerInsertOleObject:
     Insert an ole object specified by the program name.  This function returns
     the picture id for the object.
******************************************************************************/
int WINAPI _export TerInsertOleObject(HWND hWnd,LPBYTE ProgName)
{
    int SubKey;
    PTERWND w;
    BOOL FullPath=FALSE;
    int i,len;
    BYTE string[MAX_WIDTH+1],clsid[100],key[100];
    BOOL found=FALSE;
    LPBYTE ptr;
    long  BufSize;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (lstrlen(ProgName)==0) return 0;

    // check if the full path specified
    len=lstrlen(ProgName);
    for (i=len-1;i>=0;i--) if (ProgName[i]=='\\' || ProgName[i]==':') break;
    if (i>=0) FullPath=TRUE;

    // find this program in the registration database
    for (SubKey=0;TRUE;SubKey++) {
       if (ERROR_SUCCESS!=RegEnumKey(HKEY_CLASSES_ROOT,SubKey,TempString,sizeof(TempString)-1)) break;
       
       // check if it is an OLE server
       lstrcpy(TempString1,TempString);
       lstrcat(TempString1,"\\protocol\\StdFileEditing\\server"); // look for this subkey now
       BufSize=sizeof(string)-1;
       if (ERROR_SUCCESS!=RegQueryValue(HKEY_CLASSES_ROOT,TempString1,string,&BufSize)) { // key not found, look for 'insertable' key
          lstrcpy(TempString1,TempString);
          lstrcat(TempString1,"\\Insertable"); // look for this key now
          BufSize=sizeof(string)-1;
          if (ERROR_SUCCESS!=RegQueryValue(HKEY_CLASSES_ROOT,TempString1,string,&BufSize)) continue;
       
          // now find the program path
          lstrcpy(key,TempString);
          lstrcat(key,"\\CLSID"); // look for this clsid subkey now
          BufSize=sizeof(clsid)-1;
          if (ERROR_SUCCESS!=RegQueryValue(HKEY_CLASSES_ROOT,key,clsid,&BufSize)) continue;  // CLSID subkey not found for this key
          lstrcpy(key,"CLSID\\");
          lstrcat(key,clsid);
          lstrcat(key,"\\LocalServer32");
          BufSize=sizeof(string)-1;
          if (ERROR_SUCCESS!=RegQueryValue(HKEY_CLASSES_ROOT,key,string,&BufSize)) continue;  // 'Control' subkey not found for this CLSID
       } 

       // check for the program name match
       if (FullPath) {
          if (lstrcmpi(string,ProgName)==0) found=TRUE;
       }
       else {
          if (lstrlen(string)>=lstrlen(ProgName)) {
             ptr=&(string[lstrlen(string)-lstrlen(ProgName)]);
             if (lstrcmpi(ptr,ProgName)==0) found=TRUE;
          }
       }

       // check if the class description exists
       if (found) {
          BufSize=sizeof(string)-1;
          if (ERROR_SUCCESS!=RegQueryValue(HKEY_CLASSES_ROOT,TempString,string,&BufSize)) continue;  // class description missing
          break;
       }
    }

    if (found) return TerInsertObject(w,SubKey);
    else       return 0;
}

/******************************************************************************
     TerInsertObject:
     Insert a new ole object. This function activates the OLE server.  the
     second part of this function (TerInsertObjectId) is called from the
     ole post processing function to actually insert the object into the
     text.
******************************************************************************/
int TerInsertObject(PTERWND w, int SubKey)
{
    int obj;
    CLSID ClassId;
    FORMATETC OleFmt;
    LPSTORAGE pObjStg;
    LPOLECLIENTSITE pObjSite;
    HRESULT result;
    LPOLEOBJECT pObject;

    if (TerFlags&TFLAG_NO_OLE) return 0;

    if (SubKey<0 && (SubKey=CallDialogBox(w,"InsertObjectParam",InsertObjectParam,0L))<0) return 0;

    // get the class id
    RegEnumKey(HKEY_CLASSES_ROOT,SubKey,TempString,sizeof(TempString)-1);
    result=CLSIDFromProgID(StrToOleStr(TempString,TempString1),&ClassId);
    if (NOERROR!=result)  return PrintError(w,MSG_NO_OLE_CLASS,"");     // convert to class id

    // create the object
    if ((obj=FindOpenSlot(w))==-1) return 0; // find an empty slot to make the object name
    TerFont[obj].InUse=TRUE;
    TerFont[obj].ObjectType=OBJ_EMBED_INSERT;    // object to be inserted
    TerFont[obj].style=PICT;
    TerFont[obj].PictType=PICT_METAFILE;
    TerFont[obj].hMeta=0;

    // create IOleClientSite and IStorage interfaces for the object
    if (!TerOlePreCreate(w,obj,&pObjSite,&pObjStg,&OleFmt)) return 0;

    // Create the object
    result=OleCreate(&ClassId,&IID_IOleObject,OLERENDER_FORMAT,&OleFmt,pObjSite,pObjStg,(LPVOID far *)&(TerFont[obj].pObject));
    if (result!=NOERROR || !(TerFont[obj].pObject)) {
       TerFont[obj].InUse=FALSE;
       pObjSite->lpVtbl->Release(pObjSite);    // release the client site
       return PrintError(w,MSG_NO_SERVER,"");
    }

    // make advisory connection and display the object
    TerOlePostCreate(w,obj);

    // Show the object window if not alredy shown
    pObject=TerFont[obj].pObject;
    result=pObject->lpVtbl->DoVerb(pObject,OLEIVERB_SHOW,NULL,pObjSite,0,hTerWnd,NULL);

    return obj;
}

/******************************************************************************
     TerOlePreCreate:
     This function creates IOleClientSite and IStorage interface needed
     to create an object.
******************************************************************************/
BOOL TerOlePreCreate(PTERWND w,int obj, LPOLECLIENTSITE far *ppObjSite, LPSTORAGE far *ppObjStg,FORMATETC far *OleFmt)
{
    BYTE ObjName[20];
    HRESULT result;

    // create a new storage for this object
    if (True(TerFlags5&TFLAG5_UNLIMITED_OLE_SPACE)) {
       DWORD MajVers =(DWORD)(LOBYTE(LOWORD(GetVersion())));

       /*if (IsWinNT2000 && MajVers >= 5)      // Windows 2000, Windows XP and higher
            result=StgCreateStorageEx(NULL,STGM_TRANSACTED|STGM_READWRITE|STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_DELETEONRELEASE,
                   STGFMT_STORAGE,0,NULL,0,&IID_IStorage,ppObjStg);   // faster process, but not supported in old WinNT, Win98
       else */  // StgCreateStorateEx commented out because the older OS might not have reference to this function, thus dll may be loaded 
        
        result=StgCreateDocfile(NULL,STGM_TRANSACTED|STGM_READWRITE     // slower process
                                   |STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_DELETEONRELEASE,0,ppObjStg);
    }
    else { 
       StorageId++;
       wsprintf(ObjName,"TerObj%dId%d",obj,StorageId);
       result=pDocStorage->lpVtbl->CreateStorage(pDocStorage,StrToOleStr(ObjName,TempString),
              STGM_READWRITE|STGM_TRANSACTED|STGM_SHARE_EXCLUSIVE,0,0,ppObjStg);

       TerFont[obj].OleStgName=MemAlloc(strlen(ObjName)+1);  // save the ole storage name
       lstrcpy(TerFont[obj].OleStgName,ObjName);
    }

    if (result!=NOERROR || !(*ppObjStg)) {
       TerFont[obj].InUse=FALSE;
       wsprintf(TempString,"Code: %lx",(ULONG)result);
       return PrintError(w,MSG_ERR_STORAGE_CREATE,TempString);
    }

    TerFont[obj].pObjStg=(*ppObjStg);

    // create a client site interface for this object
    if (NULL==((*ppObjSite)=CreateOleClientSite(w,obj,(*ppObjStg)))) {
       TerFont[obj].InUse=FALSE;
       return PrintError(w,MSG_ERR_SITE_CREATE,"");
    }
    TerFont[obj].pObjSite=(*ppObjSite);

    TerFont[obj].ObjectAspect=OASPECT_CONTENT;     // initialize to content

    // set ole format
    OleFmt->cfFormat=CF_METAFILEPICT;
    OleFmt->ptd=NULL;
    OleFmt->dwAspect=DVASPECT_CONTENT;
    OleFmt->lindex=-1;
    OleFmt->tymed=TYMED_MFPICT;

    return TRUE;
}

/******************************************************************************
     TerOlePostCreate:
     Make advisory connection and display object
******************************************************************************/
BOOL TerOlePostCreate(PTERWND w,int obj)
{
    BYTE ObjName[MAX_WIDTH+1];
    LPOLEOBJECT pObject;
    LPOLECLIENTSITE pObjSite;
    LPSTRCLIENTSITE pSite;
    LPVIEWOBJECT pViewObject;
    HRESULT result;
    CLSID ClsId;
    LPOLESTR pProgId;
    BYTE string[MAX_WIDTH+1];

    // extract object pointers
    pObject=TerFont[obj].pObject;
    pObjSite=TerFont[obj].pObjSite;
    pSite=(LPSTRCLIENTSITE )pObjSite;


    // check if it is an equation object
    pObject->lpVtbl->GetUserClassID(pObject,&ClsId);
    ProgIDFromCLSID(&ClsId,&pProgId);
    string[0]=0;
    OleStrToStr((LPSTR)pProgId,string,0);
    if (strstr(string,"Equation.")==string) TerFont[obj].flags|=FFLAG_EQ_OBJECT;
    if (TerFont[obj].flags&FFLAG_EQ_OBJECT  && TerFont[obj].hMeta) SetOlePictOffset(w,obj);

    // set the client site
    pObject->lpVtbl->SetClientSite(pObject,pObjSite);

    // set the object name
    if (lstrlen(DocName)>0) wsprintf(ObjName,"%s, obj#%d",(LPSTR)DocName,obj);
    else                    wsprintf(ObjName,"TerObject#%d",obj);
    pObject->lpVtbl->SetHostNames(pObject,
                                 StrToOleStr(DocName,TempString),
                                 StrToOleStr(ObjName,TempString1));

    // set the advise link to the pObject --  Increments use count on pObjAdvise
    pObject->lpVtbl->Advise(pObject,pSite->pObjAdvise,&(pSite->ConnectToken));

    // set the advise link to the pViewObject
    result=pObject->lpVtbl->QueryInterface(pObject,&IID_IViewObject,(LPVOID far *)&pViewObject);
    if (result!=NOERROR)   return PrintError(w,MSG_ERR_VIEW_RETRIEVE,"TerOlePostCreate");
    result=pViewObject->lpVtbl->SetAdvise(pViewObject,DVASPECT_CONTENT,0,pSite->pObjAdvise);
    if (result!=NOERROR)   return PrintError(w,MSG_ERR_ADVISE_CONNECT,"TerOlePostCreate");
    pViewObject->lpVtbl->Release(pViewObject);  // release after accessing it

    OleSetContainedObject((LPUNKNOWN)pObject,TRUE);      // soft lock maintenance function

    // set the ocx ambient properties
    if (TerFont[obj].ObjectType==OBJ_OCX) {
        UpdateOleObject(pSite->pObjAdvise);   // to create the initial hMeta for the object
       //SetPropBool(w,obj,DISPID_AMBIENT_USERMODE,TRUE);
       //SetPropBool(w,obj,DISPID_AMBIENT_SHOWGRABHANDLES,FALSE);
       //SetPropBool(w,obj,DISPID_AMBIENT_SHOWHATCHING,FALSE);
    }


    pObjSite->lpVtbl->SaveObject(pObjSite);  // save the initial status to the storage

    return TRUE;
}

/******************************************************************************
     TerInsertDragObject:
     Insert an ole object inserted using drag/drop.  Only the first file
     in the drag/drop buffer is processed.
******************************************************************************/
BOOL TerInsertDragObject(PTERWND w,HDROP hDrop,BOOL AtCurCursor)
{
    POINT pt;
    DWORD lParam;
    BYTE file[MAX_WIDTH+1];

    dm("TerInsertDragObject");         // debug message

    if ((TerFlags&TFLAG_NO_OLE) || (TerFlags4&TFLAG4_NO_OLE_DROP)) return TRUE;

    if (TerArg.ReadOnly) return TRUE;

    // Extract the dropped file
    if (!DragQueryPoint(hDrop,&pt)) return TRUE;
    DragQueryFile(hDrop,0,file,sizeof(file)-1);  // get the first file

    DragFinish(hDrop);             // release the drag buffer

    // get cursor location at the drag point
    lParam=pt.y;
    lParam=lParam<<16;             // high word
    lParam=lParam+pt.x;
    TerMousePos(w,lParam,FALSE);

    // translate the x/y for an bounding frame
    if (TerArg.PageMode && TerFlags&TFLAG_PICT_IN_FRAME && fid(CurLine)==0) {
       NewFrameX=ScrToTwipsX(pt.x-TerWinRect.left+TerWinOrgX);
       if (BorderShowing) NewFrameX-=PrtToTwipsX(GetBorderLeftSpace(w,CurPage));

       NewFrameY=pt.y-TerWinRect.top+TerWinOrgY;
       NewFrameY=FrameToPageY(w,ScrToTwipsY(NewFrameY));
    }


    return TerInsertOleFile(hTerWnd,file,TRUE,FALSE,AtCurCursor?CurLine:MouseLine,AtCurCursor?CurCol:MouseCol);
}     
    
/******************************************************************************
     TerInsertOleFile:
     Insert an file as an ole object - as an icon.  Set line to -1 to insert the file
     at the current cursor position
******************************************************************************/
int WINAPI _export TerInsertOleFile(HWND hWnd,LPBYTE file, BOOL embed, BOOL icon, long line, int col)
{
    PTERWND w;
    int obj;
    LPOLEOBJECT pObject;
    LPDATAOBJECT pDataObject;
    FORMATETC OleFmt;
    LPSTORAGE pObjStg;
    LPOLECLIENTSITE pObjSite;
    STGMEDIUM med;
    HRESULT result;
    long SaveLine;
    int  i,SaveCol,NameLen,ExtLen;
    BYTE ext[10];
    LPBYTE ExtList[]={"jpg","jpeg","png","gif","wmf","bmp","emf"};   //number of picture file formats supported
    int    ExtCount=7;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (TerFlags&TFLAG_NO_OLE) return 0;

    if (TerArg.ReadOnly) return 0;

    // check if the file included in the no drop list
    if (lstrlen(NoDropList)>0) {
       strlwr(NoDropList);
       for (i=strlen(file)-1;i>=0;i--) if (file[i]=='.') break;
       if (i>=0) {
          lstrcpy(TempString,&(file[i]));
          strlwr(TempString);
          if (strstr(NoDropList,TempString)!=null) return 0;
       }
    } 

    if (line<0) {
       line=CurLine;
       col=CurCol;
    } 

    SaveLine=CurLine;
    SaveCol=CurCol;
    CurLine=line;
    CurCol=col;
    if (!CanInsert(w,CurLine,CurCol)) {
        CurLine=SaveLine;
        CurCol=SaveCol;
        MessageBeep(0);
        return TRUE;
    }
    
    
    // check if it is an rtf or a text file
    if (DropFile(w,file)) return 0;   // an rtf or a text file

    // check if this is a picture file
    NameLen=strlen(file);
    
    for (i=0;i<ExtCount;i++) {
       lstrcpy(ext,ExtList[i]);
       ExtLen=lstrlen(ext);
       if (NameLen>ExtLen && strcmpi(&(file[NameLen-ExtLen]),ext)==0) 
          return TerInsertPictureFile(hTerWnd,file,False(TerFlags6&TFLAG6_INSERT_DROP_PICT_AS_LINK),0,true);
    }

    // create the object
    if ((obj=FindOpenSlot(w))==-1) return 0;  // find an empty slot to make the object name
    
    TerFont[obj].InUse=TRUE;
    TerFont[obj].style=PICT;
    TerFont[obj].PictType=PICT_METAFILE;
    TerFont[obj].hMeta=0;

    // create IOleClientSite and IStorage interfaces for the object
    if (!TerOlePreCreate(w,obj,&pObjSite,&pObjStg,&OleFmt)) return 0;
    if (icon) {
       OleFmt.dwAspect=DVASPECT_ICON;
       TerFont[obj].ObjectAspect=OASPECT_ICON;
    }

    // Create the object
    if (embed) result=OleCreateFromFile(&CLSID_NULL,StrToOleStr(file,TempString),&IID_IOleObject,OLERENDER_FORMAT,&OleFmt,pObjSite,pObjStg,(LPVOID far *)&(TerFont[obj].pObject));
    else       result=OleCreateLinkToFile(StrToOleStr(file,TempString),&IID_IOleObject,OLERENDER_FORMAT,&OleFmt,pObjSite,pObjStg,(LPVOID far *)&(TerFont[obj].pObject));
    if (result!=NOERROR || !(TerFont[obj].pObject)) {
       pObjSite->lpVtbl->Release(pObjSite);    // release the client site
       TerFont[obj].InUse=FALSE;
       return PrintError(w,MSG_NO_SERVER,"");
    }
    pObject=TerFont[obj].pObject;

    // make advisory connection and display the object
    TerOlePostCreate(w,obj);


    // get the IDataObject Interface
    result=pObject->lpVtbl->QueryInterface(pObject,&IID_IDataObject,(LPVOID far *)&pDataObject);
    if (result!=NOERROR)   return PrintError(w,MSG_ERR_DATA_RETRIEVE,"TerInsertDragObject");

    // retrieve the presentation data
    if (NOERROR!=(pDataObject->lpVtbl->GetData(pDataObject,&OleFmt,&med)) || med.tymed!=TYMED_MFPICT) {
       return PrintError(w,MSG_ERR_PRESENT_DATA,"TerInsertDragObject");
    }

    if (TerCreateMetafile(w,med.u.hGlobal,obj)<0) return FALSE;   // extract metafile
    TerFont[obj].ObjectType=OBJ_EMBED;

    ReleaseStgMedium(&med);                      // release data handle

    SetOlePictOffset(w,obj);                     // retrieve picture offset from the metafile
    
    TerInsertObjectId(hTerWnd,obj,TRUE);   // insert this object into text

    return obj;
}

/******************************************************************************
     DropFile:
     Insert an RTF or a text file at the current location.  This function returns
     TRUE if the file is inserted.
******************************************************************************/
BOOL DropFile(PTERWND w, LPBYTE file)
{
     BYTE ext[10];
     long BufLen;
     HGLOBAL hMem;
     BYTE huge *pMem;
     BOOL result;

     if (TerInsertRtfFile(hTerWnd,file,-1,0,TRUE)) return TRUE;   // rtf file inserted

     // check if it is a text file
     TerMbcsExt(hTerWnd,file,ext);
     lstrupr(w,ext);

     if ( lstrlen(ext)==0 || lstrcmp(ext,".")==0 || lstrcmp(ext,".TXT")==0
       || lstrcmp(ext,".BAT")==0 || lstrcmp(ext,".SYS")==0) {       // assume it to be a text file

       if (NULL==(hMem=TerFileToMem(file,(DWORD far *)&BufLen))) return FALSE;

       if (NULL==(pMem=GlobalLock(hMem))) return FALSE;

       result=InsertTerText(hTerWnd,pMem,TRUE);

       GlobalUnlock(hMem);
       GlobalFree(hMem);

       return result;
     }

     return FALSE;
}

/******************************************************************************
     TerEditOle:
     If the current item represents an OLE object, this functions calls the
     server to update the item.
******************************************************************************/
BOOL TerEditOle(PTERWND w, BOOL edit)
{
    int   obj;
    LPOLEOBJECT pObject;

    if (TerFlags&TFLAG_NO_OLE) return FALSE;

    // get current font id
    obj=GetCurCfmt(w,CurLine,CurCol);
    if (!(TerFont[obj].style&PICT)) return FALSE;
    if (TerFont[obj].ObjectType==OBJ_OCX || TerFont[obj].ObjectType==OBJ_OCX_EXT) return TRUE;
    if (!(TerFlags4&TFLAG4_ALWAYS_INVOKE_OLE)) {
       if (TerArg.ReadOnly || TerFont[obj].style&PROTECT || TerFlags&TFLAG_NO_EDIT_OLE) return FALSE;
    }

    // If necessary, convert picture objects to temporary OLE objects
    if (TerFont[obj].ObjectType==OBJ_NONE && !(TerFlags&TFLAG_NO_EDIT_PICT))
       TerCreateTempOle(w,obj);
    if (TerFont[obj].ObjectType==OBJ_NONE) return FALSE;

    // deactivate any previously active ole object
    if (ActiveOleObj>=0) DeactivateEditOle(w);

    // run the ole object
    pObject=TerFont[obj].pObject;
    if (edit) pObject->lpVtbl->DoVerb(pObject,OLEIVERB_SHOW,NULL,TerFont[obj].pObjSite,0,hTerWnd,NULL);
    else      pObject->lpVtbl->DoVerb(pObject,OLEIVERB_PRIMARY,NULL,TerFont[obj].pObjSite,0,hTerWnd,NULL);

    ActiveOleObj=obj;

    return TRUE;
}

/******************************************************************************
     DeactivateEditOle:
     Deactivate the editing of ole object
******************************************************************************/
BOOL DeactivateEditOle(PTERWND w)
{
    int   obj=ActiveOleObj;
    LPOLEOBJECT pObject;
    LPOLECLIENTSITE pObjSite;
    LPSTRCLIENTSITE pSite;

    if (False(TerFlags6&TFLAG6_IN_PLACE_ACTIVATE)) return true;  // in-place activation not used
     
    if (obj<0 || obj>=TotalFonts) return true;

    // run the ole object
    pObject=TerFont[obj].pObject;
    if (!pObject) return true;


    pObject->lpVtbl->DoVerb(pObject,OLEIVERB_HIDE,NULL,TerFont[obj].pObjSite,0,hTerWnd,NULL);
    
    // extract object pointers
    pObjSite=TerFont[obj].pObjSite;
    pSite=(LPSTRCLIENTSITE )pObjSite;
    
    if (pSite!=NULL && pSite->pObjAdvise!=NULL) UpdateOleObject(pSite->pObjAdvise);   // to create the initial hMeta for the object
    if (pObjSite!=NULL) pObjSite->lpVtbl->SaveObject(pObjSite);  // save the modified data to the storage

    ActiveOleObj=-1;

    return TRUE;
}

/******************************************************************************
     UpdateOleObj:
     Update ole object size.  This function is called when an in-place ole object
     is resized using mouse.
******************************************************************************/
BOOL UpdateOleObj(PTERWND w, int obj)
{
    LPOLEOBJECT pObject;
    LPOLECLIENTSITE pObjSite;
    LPSTRCLIENTSITE pSite;
    SIZEL size;

    if (False(TerFlags6&TFLAG6_IN_PLACE_ACTIVATE)) return true;  // in-place activation not used
     
    if (ActiveOleObj>=0) DeactivateEditOle(w);   // deactive any current ole object

    if (obj<0 || obj>=TotalFonts) return true;

    // run the ole object
    pObject=TerFont[obj].pObject;
    if (!pObject) return true;

    if (TerFont[obj].flags&FFLAG_RESIZED) {  // let the object know about the new size
       size.cx=TerFont[obj].PictWidth*2540/1440;
       size.cy=TerFont[obj].PictHeight*2540/1440;
       pObject->lpVtbl->SetExtent(pObject,DVASPECT_CONTENT,&size);
    }

    // extract object pointers
    pObjSite=TerFont[obj].pObjSite;
    pSite=(LPSTRCLIENTSITE )pObjSite;
    
    if (pSite!=NULL && pSite->pObjAdvise!=NULL) UpdateOleObject(pSite->pObjAdvise);   // to create the initial hMeta for the object
    if (pObjSite!=NULL) pObjSite->lpVtbl->SaveObject(pObjSite);  // save the initial status to the storage
    
    return TRUE;
}

/******************************************************************************
     TerCreateTempOle:
     This function creates a temporary ole object from a metafile picture.  The
     temporary metafile object is used to edit picture using MSDRAW.
******************************************************************************/
TerCreateTempOle(PTERWND w,int obj)
{
    BYTE huge *pMeta;
    METAHEADER far *hdr;
    LPWORD pNative;
    LPBYTE pStr;
    int len;
    long far *pLong;
    long   ObjSize,bmWidth,bmHeight,MetaSize;
    struct StrFont SaveDIBObj,SaveMetaObj;
    CLSID ClassId;
    LPOLECLIENTSITE pObjSite;
    LPSTORAGE pObjStg;
    FORMATETC OleFmt;
    BYTE ClassName[50];

    if (!(TerFont[obj].style&PICT)) return FALSE;
    if (TerFont[obj].ObjectType!=OBJ_NONE) return FALSE;

    // get the class id for MSDRAW program
    lstrcpy(ClassName,MSDRAW_CLASS);
    if (NOERROR!=CLSIDFromProgID(StrToOleStr(ClassName,TempString),&ClassId)) return FALSE;   // MS draw program not found

    // Convert DIB to metafile if necessary
    if (TerFont[obj].PictType==PICT_DIBITMAP) {
       SaveDIBObj=TerFont[obj];    // save DIB object
       if (DIB2Metafile(w,obj)) {
          SaveMetaObj=TerFont[obj];// save metafile object
          TerFont[obj]=SaveDIBObj; // restore to delete
          DeleteTerObject(w,obj);
          TerFont[obj]=SaveMetaObj;// restore metafile object
       }
    }
    if (TerFont[obj].PictType!=PICT_METAFILE) return FALSE;

    // Make a native object from the metafile
    pMeta=(LPVOID)TerGetMetaFileBits(TerFont[obj].hMeta,null);   // convert to global handle
    if (NULL==(hdr=(METAHEADER far *)pMeta) ) {
        PrintError(w,MSG_ERR_META_ACCESS,NULL);
        TerFont[obj].ObjectType=OBJ_NONE;           // restore to plain picture
        return FALSE;
    }
    MetaSize=hdr->mtSize*2;

    // calculate the size of the tempporary ole object
    ObjSize=2*sizeof(long);                        // ver and indentifier
    ObjSize+=sizeof(long)+lstrlen(ClassName)+1; // class id + null terminator
    ObjSize+=2*sizeof(long);                       // length of topic and item string
    ObjSize+=sizeof(long);                         // length of native data
    ObjSize+=3*sizeof(WORD);                       // native data header
    ObjSize+=MetaSize;                             // native data bytes

    ObjSize+=2*sizeof(long);                       // presentation data, ole ver and id
    ObjSize+=sizeof(long)+strlen("METAFILEPICT")+1;// format
    ObjSize+=2*sizeof(long);                       // width and height
    ObjSize+=sizeof(long);                         // size of presentation data
    ObjSize+=MetaSize;                             // presentation data bits

    if (  NULL==(hOleData=GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,ObjSize))  // allocate memory for the text data in our format
       || NULL==(pOleData=GlobalLock(hOleData)) ){
        PrintError(w,MSG_OUT_OF_MEM,"TerCreateTempOle");
        TerFont[obj].ObjectType=OBJ_NONE;           // restore to plain picture
        return FALSE;
    }

    // set the object header
    pLong=(LPLONG)pOleData;
    *pLong=0x501;pLong++;                           // ole version number
    *pLong=2;pLong++;                               // native data id

    len=lstrlen(ClassName);                      // length of class string
    *pLong=(len+1);pLong++;                         // length of class string plus NULL terminator
    pStr=(LPBYTE)pLong;
    lstrcpy(pStr,ClassName);
    pStr=&(pStr[len]);                              // NULL not included
    *pStr=0;pStr++;                                 // add the numm terminator
    pLong=(LPLONG)pStr;

    *pLong=0;pLong++;                               // length of topic string
    *pLong=0;pLong++;                               // length of item string

    *pLong=MetaSize+3*sizeof(WORD);                 // size of native bits
    pLong++;

    // set native data
    pNative=(LPWORD)pLong;
    bmHeight=ScrToPointsY(TerFont[obj].bmHeight);   // objure height in pointsize
    bmWidth=ScrToPointsX(TerFont[obj].bmWidth);     // objure width in pointsize
    pNative[0]=MM_ANISOTROPIC;                      // mapping mode
    pNative[1]=(WORD)((bmWidth*2500)/72);           // width in HIMETRIC
    pNative[2]=(WORD)((bmHeight*2500)/72);          // height in HIMETIC

    pStr=(LPBYTE)&(pNative[3]);
    HugeMove(hdr,pStr,MetaSize);                     // move the rtf data

    pLong=(LPLONG)&(((char huge *)pStr)[MetaSize]);  // advance the pointer

    // set the presentation data
    *pLong=0x501;pLong++;                           // ole version number
    *pLong=5;pLong++;                               // presentation data id

    len=lstrlen("METAFILEPICT");                    // length of class string
    *pLong=(len+1);pLong++;                         // length of class string plus NULL terminator
    pStr=(LPBYTE)pLong;
    lstrcpy(pStr,"METAFILEPICT");
    pStr=&(pStr[(int)len]);                         // NULL not included
    *pStr=0;pStr++;                                 // add the numm terminator
    pLong=(LPLONG)pStr;

    *pLong=pNative[1];pLong++;                      // width of presentation
    *pLong=pNative[2];pLong++;                      // height of presentation

    *pLong=MetaSize;pLong++;                       // size of the presentation bits
    HugeMove(hdr,pLong,MetaSize);              // move the rtf data

    TerFont[obj].hMeta=TerSetMetaFileBits(pMeta);   // restore the metafile handle

    // create an ole object
    if (!TerOlePreCreate(w,obj,&pObjSite,&pObjStg,&OleFmt)) return FALSE;

    // copy OLE1 stream data to OLE2 storage
    OleStream.MaxSize=ObjSize;                 // object size
    OleStream.offset=0;                         // beginning offset in the buffer
    if (NOERROR!=(OleConvertOLESTREAMToIStorage(&(OleStream.stream),pObjStg,NULL))) {
       return PrintError(w,MSG_ERR_OLE_DATA_CONV,"TerCreateTempOle");
    }

    TerFont[obj].ObjectType=OBJ_EMBED_TEMP;         // mark as temporary embedded object
    if (NOERROR!=(OleLoad(pObjStg,&(IID_IOleObject),pObjSite,(LPVOID far *)&(TerFont[obj].pObject)))) {
       return PrintError(w,MSG_ERR_OLE_LOAD,"TerCreateTempOle");
    }

    TerOlePostCreate(w,obj);                        // postcreation tasks

    GlobalUnlock(hOleData);                         // free the temporary data
    GlobalFree(hOleData);

    return TRUE;
}

/*******************************************************************************
    TerOleLoad:
    This function supplies the ole data in the temporary handle (TerFont.hObject).
    and creates the OLE object. This functions frees the temporary memory
    handle.
*******************************************************************************/
TerOleLoad(PTERWND w,int obj)
{
    FORMATETC OleFmt;
    LPSTORAGE pObjStg;
    LPOLECLIENTSITE pObjSite;
    HRESULT result;
    LPLONG pLong;
    CLSID  ClassId;
    int ObjectAspect;
    int SavePictHeight,SavePictWidth;
    LPOLECACHE pOleCache;
    DWORD connection;
    LPOLEOBJECT pObject;
    BYTE str[MAX_WIDTH+1];

    if (TerFlags&TFLAG_NO_OLE) return TRUE;

    if (!TerFont[obj].hObject) return FALSE;

    hOleData=TerFont[obj].hObject;       // initialize the buffer
    pOleData=NULL;

    SavePictHeight=TerFont[obj].PictHeight;
    SavePictWidth=TerFont[obj].PictWidth;

    if (NULL==(pOleData=GlobalLock(hOleData)) ){
        return PrintError(w,MSG_OUT_OF_MEM,"TerOleLoad");
    }

    // check if the class is registered on the current machine
    lstrcpy(TempString,(LPSTR)&(pOleData[3*sizeof(LONG)]));    // prog id
    result=CLSIDFromProgID(StrToOleStr(TempString,TempString1),&ClassId);
    if (NOERROR!=result)  {
        GlobalUnlock(hOleData);
        GlobalFree(hOleData);
        TerFont[obj].hObject=NULL;
        TerFont[obj].ObjectType=OBJ_NONE;
        return FALSE;
    }


    // create internal interfaces for the object
    ObjectAspect=TerFont[obj].ObjectAspect;   // save
    if (!TerOlePreCreate(w,obj,&pObjSite,&pObjStg,&OleFmt)) return FALSE;
    TerFont[obj].ObjectAspect=ObjectAspect;   // restore

    // Create the CompObj stream for a link object
    pLong=(LPLONG)pOleData;
    if (pLong[1]==1) {         // link type object
       lstrcpy(TempString,(LPSTR)&(pOleData[3*sizeof(LONG)]));    // prog id
       result=CLSIDFromProgID(StrToOleStr(TempString,TempString1),&ClassId);
       if (NOERROR!=result)  return PrintError(w,MSG_NO_OLE_CLASS,"TerOleLoad");     // convert to class id

       result=WriteClassStg(pObjStg,&ClassId);
       if (NOERROR!=result)  return PrintError(w,MSG_ERR_OLE_CLSID,"TerOleLoad");     // convert to class id

       result=WriteFmtUserTypeStg(pObjStg,0,StrToOleStr("",TempString));
       if (NOERROR!=result)  return PrintError(w,MSG_ERR_OLE_USER,"TerOleLoad");     // convert to class id
    }

    // copy OLE1 stream data to OLE2 storage
    OleStream.MaxSize=TerFont[obj].ObjectSize;  // object size
    OleStream.offset=0;                         // beginning offset in the buffer
    result=OleConvertOLESTREAMToIStorage(&(OleStream.stream),pObjStg,NULL);
    
    if (result==0x800401c2 && FixedPresentationData(w,obj)) {  // 800401c2 = ole stream data not in correct format
       OleStream.MaxSize=TerFont[obj].ObjectSize;  // object size
       OleStream.offset=0;                         // beginning offset in the buffer
       result=OleConvertOLESTREAMToIStorage(&(OleStream.stream),pObjStg,NULL);
    } 

    if (result!=NOERROR) {
       wsprintf(str,"TerOleLoad, Error Code: %x",result);
       return PrintError(w,MSG_ERR_OLE_DATA_CONV,str);
    }

    // load the object -- Increments use count on pObjSite
    result=OleLoad(pObjStg,&(IID_IOleObject),pObjSite,(LPVOID far *)&(TerFont[obj].pObject));
    if (result!=NOERROR || !TerFont[obj].pObject) return PrintError(w,MSG_ERR_OLE_LOAD,"");


    // make advisory connection and display the object
    TerOlePostCreate(w,obj);

    // Use OleCache interface to ensure that MFPICT picture representation is available.
    // This is not a problem for an OLE object create (and then subsequently saved to an RTF file)
    // within TE.  However, other application such as MSWord might create an OLE object using the
    // Draw method of paiting the screen.  This would be a problem for TE because it expect the
    // cache metafile image to be available.  Unfortunately the OleLoad function does not allow
    // you to load an ole object from steram with your own FORMATETC specification.  So here
    // we are ensuring that ole objeect does cache the metafile representation.  If this is 
    // not done, then the GetData method in IasOnViewChange might fail for such objects. The
    // absense of this code will also cause the IasOnViewChange to be not called atall for
    // such objects.
    pObject=TerFont[obj].pObject;
    result=pObject->lpVtbl->QueryInterface(pObject,&IID_IOleCache,(LPVOID far *)&pOleCache);
    if (result==NOERROR) {
       result=pOleCache->lpVtbl->Cache(pOleCache,&OleFmt,0,&connection);
       if (result==NOERROR) TerFont[obj].CacheConnectId=connection;
       pOleCache->lpVtbl->Release(pOleCache);  // release after accessing it
    }

    GlobalUnlock(hOleData);
    GlobalFree(hOleData);
    TerFont[obj].hObject=NULL;

    // update the link object
    if (TerFont[obj].ObjectType==OBJ_LINK) {
       LPOLELINK pOleLink;
       LPOLEOBJECT pObject=TerFont[obj].pObject;
       if (NOERROR==pObject->lpVtbl->QueryInterface(pObject,&IID_IOleLink,(LPVOID far *)&pOleLink)) {
          pOleLink->lpVtbl->Update(pOleLink,NULL);
          pOleLink->lpVtbl->Release(pOleLink);
       }
    } else if (TerFont[obj].ObjectUpdate) {
       LPOLEOBJECT pObject=TerFont[obj].pObject;
       pObject->lpVtbl->Update(pObject);
    }

    // maintain the original metafile display dimensions
    if (!TerFont[obj].ObjectUpdate) { 
       TerFont[obj].PictHeight=SavePictHeight;     // apply the previous display size
       TerFont[obj].PictWidth=SavePictWidth;
    }
    SetPictSize(w,obj,TwipsToScrY(TerFont[obj].PictHeight),TwipsToScrX(TerFont[obj].PictWidth),TRUE);
    XlateSizeForPrt(w,obj);              // convert size to printer resolution


    return TRUE;
}

/*******************************************************************************
    FixedPresentationData:
    When OleConvertOLESTREAMToIStorage function can not read the original ole
    data, this function resets the presentation data in the ole buffer to NULL
    presentation data.  
    Currently this fix is implemented for embedded object only (not yet for linked object).
    
    The data is assumed to be in the pOleData variable 
 
*******************************************************************************/
BOOL FixedPresentationData(PTERWND w,int obj)
{
 
    LPBYTE ptr;
    LPLONG lptr;
    int size;

    ptr=pOleData;
    lptr=(LPVOID)ptr;
    
    if (lptr[1]!=2) return false;    // second long identifies embdeeded (value 2) or linked (value 1)  objects

    ptr=&(pOleData[2*sizeof(long)]); // beyond the format long
    lptr=(LPLONG)ptr;
    size=(*lptr);  // size of class string
    ptr=(LPVOID)&(lptr[1]);
    ptr=&(ptr[size]);  // after class string

    lptr=(LPVOID)ptr;
    size=(*lptr);  // size of topic string
    ptr=(LPVOID)&(lptr[1]);
    ptr=&(ptr[size]);  // after topic string

    lptr=(LPVOID)ptr;
    size=(*lptr);  // size of item string
    ptr=(LPVOID)&(lptr[1]);
    ptr=&(ptr[size]);  // after item string

    lptr=(LPVOID)ptr;
    size=(*lptr);  // size of item string
    ptr=(LPVOID)&(lptr[1]);
    ptr=&(ptr[size]);  // after native string

    size=(int)((DWORD)ptr-(DWORD)pOleData);  // size upto native data
    
    if ((int)TerFont[obj].ObjectSize>(size+8)) {   // substitue current presentation object by NULL presentation object
       TerFont[obj].ObjectSize=size+8;   

       ptr=&(pOleData[size]);
       lptr=(LPVOID)ptr;
       lptr[0]=0x501;         // 0x501 - first long for NULL presentation data
       lptr[1]=0;             // 0     - second long for NULL presentation data
       
       return true;
    }

    return false;
} 

/*******************************************************************************
    TerOleUnload:
    This function delete the OLE object.
*******************************************************************************/
TerOleUnload(PTERWND w,int obj)
{
    LPOLEOBJECT pObject=TerFont[obj].pObject;
    LPOLECLIENTSITE pObjSite=TerFont[obj].pObjSite;
    LPSTRCLIENTSITE pSite=(LPSTRCLIENTSITE )pObjSite;
    LPVIEWOBJECT pViewObject;
    HRESULT result;

    if (TerFlags&TFLAG_NO_OLE) return TRUE;

    HideOcx(w,obj);           // hide if an ole object is visible

    if (pObject) {
       if (TerFont[obj].ObjectType!=OBJ_OCX_EXT) {
           if (TerFont[obj].CacheConnectId!=0) {   // remove image caching 
              LPOLECACHE pOleCache;
              result=pObject->lpVtbl->QueryInterface(pObject,&IID_IOleCache,(LPVOID far *)&pOleCache);
              if (result==NOERROR) {
                 pOleCache->lpVtbl->Uncache(pOleCache,TerFont[obj].CacheConnectId);
                 pOleCache->lpVtbl->Release(pOleCache);  // release after accessing it
              }
           } 
           
           // Reset the advisory connections, reduces ref count on IAdviseSink
           result=pObject->lpVtbl->QueryInterface(pObject,&IID_IViewObject,(LPVOID far *)&pViewObject);
           if (result==NOERROR)   {
               pViewObject->lpVtbl->SetAdvise(pViewObject,DVASPECT_CONTENT,0,NULL);
               pViewObject->lpVtbl->Release(pViewObject);  // release after accessing it
           }
           pObject->lpVtbl->Unadvise(pObject,pSite->ConnectToken);

           // reset the client site for the object, reduces ref count on IOleClientSite
           pObject->lpVtbl->SetClientSite(pObject,NULL);

           pObject->lpVtbl->Close(pObject,OLECLOSE_SAVEIFDIRTY);   // close any open server application
       }
       pObject->lpVtbl->Release(pObject);
    }

    // release the IAdviseSink object
    pObjSite=TerFont[obj].pObjSite;
    pSite=(LPSTRCLIENTSITE )pObjSite;
    if (pSite->pObjAdvise) {
       while (pSite->pObjAdvise->lpVtbl->Release(pSite->pObjAdvise)>0);
    }

    #if defined(WIN32)         // Relase ActiveX support interfaces
       // release the IOleControlSite object
       if (pSite->pOleControlSite) {
          while (pSite->pOleControlSite->lpVtbl->Release(pSite->pOleControlSite)>0);
       }

       // release the IOleInPlaceSite object
       if (pSite->pInPlaceSite) {
          while (pSite->pInPlaceSite->lpVtbl->Release(pSite->pInPlaceSite)>0);
       }

       // release the IOleInPlaceFrame object
       if (pSite->pInPlaceFrame) {
          while (pSite->pInPlaceFrame->lpVtbl->Release(pSite->pInPlaceFrame)>0);
       }

       // release the IDispatch object
       if (pSite->pDispatch) {
          while (pSite->pDispatch->lpVtbl->Release(pSite->pDispatch)>0);
       }
    #endif

    // release the IOleClientSite object
    pObjSite=TerFont[obj].pObjSite;
    if (pObjSite) {
       while (pObjSite->lpVtbl->Release(pObjSite)>0);
    }

    if (True(TerFont[obj].pObjStg)) {
       LPSTORAGE pObjStg=TerFont[obj].pObjStg;
       while (pObjStg->lpVtbl->Release(pObjStg)>0);
       TerFont[obj].pObjStg=0;

       // delete the ole storage from the pDocStorage.  This ole storage can not be deleted by the above release method
       if (TerFont[obj].OleStgName!=NULL) { 
          result=pDocStorage->lpVtbl->DestroyElement(pDocStorage,StrToOleStr(TerFont[obj].OleStgName,TempString));
          if ((StorageId % 500)==0) {   // commit every 500 elements to increase speed
             result=pDocStorage->lpVtbl->Commit(pDocStorage,STGC_DEFAULT);   // commit needed in transacted mode
          }
          MemFree(TerFont[obj].OleStgName);
          TerFont[obj].OleStgName=NULL;
       }
    } 

    return TRUE;
}

/*******************************************************************************
    TerOleQuerySize:
    This function returns the size of the specified object storage.
*******************************************************************************/
BOOL TerOleQuerySize(PTERWND w, int obj, DWORD far *size)
{
    LPSTRCLIENTSITE pSite=(LPSTRCLIENTSITE)TerFont[obj].pObjSite;
    LPSTORAGE pObjStg=pSite->pObjStg;
    HRESULT result;
    BYTE huge * pSaveData;
    HGLOBAL hSaveData;

    if (TerFont[obj].ObjectType==OBJ_OCX || TerFont[obj].ObjectType==OBJ_OCX_EXT) {      // Ocx may not have explicit 'update' function
       LPOLECLIENTSITE pObjSite=TerFont[obj].pObjSite;
       pObjSite->lpVtbl->SaveObject(pObjSite);   // save the object
    }

    (*size)=0;

    if (TerFlags&TFLAG_NO_OLE) return TRUE;

    hSaveData=hOleData;         // save these global variables
    pSaveData=pOleData;
    hOleData=NULL;
    pOleData=0;                 // set TerOlePut in query mode

    OleStream.MaxSize=0;        // initialize the size output variable
    OleStream.offset=0;         // beginning offset in the buffer
    result=OleConvertIStorageToOLESTREAM(pObjStg,&(OleStream.stream));
    if (result!=NOERROR) {
       PrintOleError(w,result);
       wsprintf(TempString,"Code: %lx (TerOleQuerySize)",(DWORD)result);
       return PrintError(w,MSG_ERR_OLE_DATA_CONV,TempString);
    }


    (*size)=OleStream.MaxSize;  // buffer size

    hOleData=hSaveData;         // restore these global variables
    pOleData=pSaveData;

    return TRUE;
}

/*******************************************************************************
    GetOleStorageData:
    This function extracts the OLE storage data in a global buffer.  This
    function allocates a new global buffer and leaves it locked.  The calling
    fuction must unlock and free the buffer when done.
*******************************************************************************/
GetOleStorageData(PTERWND w,int obj)
{
    LPSTRCLIENTSITE pSite=(LPSTRCLIENTSITE)TerFont[obj].pObjSite;
    LPSTORAGE pObjStg=pSite->pObjStg;
    HRESULT result;
    DWORD ObjectSize;

    if (TerFlags&TFLAG_NO_OLE) return TRUE;

    if (ActiveOleObj>=0) DeactivateEditOle(w);   // deactive any current ole object

    // get the data size requirement and allocate memory
    if (!TerOleQuerySize(w,obj,&ObjectSize) || ObjectSize==0) {
        return PrintError(w,MSG_ERR_OBJ_SIZE,"GetOleStorageData");
    }

    hOleData=NULL;       // initialize the buffer
    pOleData=NULL;

    if (  NULL==(hOleData=GlobalAlloc(GMEM_MOVEABLE,ObjectSize))  // allocate memory for the ole data
       || NULL==(pOleData=GlobalLock(hOleData)) ){
        return PrintError(w,MSG_OUT_OF_MEM,"GetOleStorageData");
    }

    // set to object to the global buffer
    OleStream.MaxSize=ObjectSize;      // size of the object to read
    OleStream.offset=0;                // beginning buffer offset
    result=OleConvertIStorageToOLESTREAM(pObjStg,&(OleStream.stream));
    if (result!=NOERROR) {
        PrintError(w,MSG_ERR_OBJ_DATA,"GetOleStorageData");
        GlobalUnlock(hOleData);
        GlobalFree(hOleData);
        return FALSE;
    }

    return TRUE;
}

/*******************************************************************************
    StrToOleStr:
    Wait for an ole async operation to be over
*******************************************************************************/
LPOLESTR StrToOleStr(LPSTR str,LPSTR OleStr)
{
    #if defined(OLE2ANSI)
       lstrcpy(OleStr,str);
       return OleStr;
    #else
       int i,len;
       LPOLESTR str1=(LPOLESTR)OleStr;

       len=lstrlen(str);
       for (i=0;i<len;i++) str1[i]=str[i];
       str1[i]=0;

       return str1;
    #endif
}

/*******************************************************************************
    OleStrToStr:
    Copy an ole string to a regular string.  If count is 0 all characters are copied
*******************************************************************************/
LPSTR OleStrToStr(LPSTR OleStr,LPSTR str, int count)
{
    #if defined(OLE2ANSI)
       if (!OleStr) str[0]=0;
       else {
          if (count==0) lstrcpy(str,OleStr);
          else          lstrncpy(str,OleStr,count);
       }
       return str;
    #else
       int i;
       LPOLESTR str1=(LPOLESTR)OleStr;

       if (!OleStr) str[0]=0;
       else {
          for (i=0;i<count || count==0;i++) {
            str[i]=(BYTE)str1[i];
            if (str[i]==0) break;
          }
          str[i]=0;
       }

       return str;
    #endif
}

/*******************************************************************************
    PrintOleError:
    Print Ole error messages.
*******************************************************************************/
PrintOleError(PTERWND w, HRESULT result)
{
    long code=(long)result;
    char  desc[100];

    desc[0]=0;

    if      (code==REGDB_E_CLASSNOTREG) lstrcpy(desc,MsgString[MSG_OLEERR_NO_CLASS]);
    else if (code==STG_E_SHAREREQUIRED) lstrcpy(desc,MsgString[MSG_OLEERR_NEED_SHARE]);
    else if (code==STG_E_TOOMANYOPENFILES) lstrcpy(desc,MsgString[MSG_ERR_TOO_MANY_FILES]);
    else if (code==STG_E_ACCESSDENIED)  lstrcpy(desc,MsgString[MSG_ERR_FILE_ACCESS]);

    if (desc[0]==0) wsprintf(desc,"Error Code: %lx",(DWORD)result);

    MessageBox(NULL,desc,"OLE Error",MB_OK);

    return TRUE;
}

/*****************************************************************************
    Implementation of IOleClientSite Interface
******************************************************************************/

/*****************************************************************************
    CreateOleClientSite:
    This function creates an instance of IOleClientSite interface.  The use
    count for the interface is set to 1.
******************************************************************************/
LPOLECLIENTSITE CreateOleClientSite(PTERWND w, int obj, LPSTORAGE pObjStg)
{
    LPSTRCLIENTSITE pTerSite;
    LPOLECLIENTSITE pSite;

    // allocate and inititialize pTerSite
    if (NULL==(pTerSite=(LPSTRCLIENTSITE )OurAlloc(sizeof(struct StrClientSite)))) return NULL;
    FarMemSet(pTerSite,0,sizeof(struct StrClientSite));
    pTerSite->count=1;                     // count for the interface
    pTerSite->w=w;                         // window pointer for the pSite
    pTerSite->obj=obj;                     // object id for the pSite
    pTerSite->pObjStg=pObjStg;             // object IStorage

    // create IAdviseSink
    pTerSite->pObjAdvise=CreateAdviseSink(w,obj,pObjStg,(LPOLECLIENTSITE)pTerSite);
    if (!(pTerSite->pObjAdvise)) return NULL;

    // Create ActiveX support interaces
    #if defined(WIN32)
       if (TerFont[obj].ObjectType==OBJ_OCX || TerFont[obj].ObjectType==OBJ_OCX_EXT 
           || True(TerFlags6&TFLAG6_IN_PLACE_ACTIVATE)) {
          pTerSite->pInPlaceSite=CreateInPlaceSite(w,obj,(LPOLECLIENTSITE)pTerSite);
          if (!(pTerSite->pInPlaceSite)) return NULL;
          pTerSite->pInPlaceFrame=CreateInPlaceFrame(w,obj,(LPOLECLIENTSITE)pTerSite);
          if (!(pTerSite->pInPlaceFrame)) return NULL;
          pTerSite->pOleControlSite=CreateOleControlSite(w,obj,(LPOLECLIENTSITE)pTerSite);
          if (!(pTerSite->pOleControlSite)) return NULL;
          pTerSite->pDispatch=CreateDispatch(w,obj,(LPOLECLIENTSITE)pTerSite);
          if (!(pTerSite->pDispatch)) return NULL;
       }
    #endif

    // allocate and intialize the CLIENTSITE virtual functions
    pSite=(LPOLECLIENTSITE)pTerSite;
    if (NULL==(pSite->lpVtbl=OurAlloc(sizeof(struct IOleClientSiteVtbl)))) return NULL;

    if ( NULL==(pSite->lpVtbl->QueryInterface=(LPVOID) MakeProcInstance((FARPROC)IocsQueryInterface,hTerInst))
      || NULL==(pSite->lpVtbl->AddRef=(LPVOID) MakeProcInstance((FARPROC)IocsAddRef,hTerInst))
      || NULL==(pSite->lpVtbl->Release=(LPVOID) MakeProcInstance((FARPROC)IocsRelease,hTerInst))
      || NULL==(pSite->lpVtbl->GetContainer=(LPVOID) MakeProcInstance((FARPROC)IocsGetContainer,hTerInst))
      || NULL==(pSite->lpVtbl->OnShowWindow=(LPVOID) MakeProcInstance((FARPROC)IocsOnShowWindow,hTerInst))
      || NULL==(pSite->lpVtbl->GetMoniker=(LPVOID) MakeProcInstance((FARPROC)IocsGetMoniker,hTerInst))
      || NULL==(pSite->lpVtbl->RequestNewObjectLayout=(LPVOID) MakeProcInstance((FARPROC)IocsRequestNewObjectLayout,hTerInst))
      || NULL==(pSite->lpVtbl->SaveObject=(LPVOID) MakeProcInstance((FARPROC)IocsSaveObject,hTerInst))
      || NULL==(pSite->lpVtbl->ShowObject=(LPVOID) MakeProcInstance((FARPROC)IocsShowObject,hTerInst)) ){
       PrintError(w,MSG_ERR_SITE_CREATE,"CreateOleClientSite");
       return NULL;
    }

    return pSite;
}

/*****************************************************************************
    IocsQueryInterface: Query interface function for IOleClientSite
******************************************************************************/
HRESULT STDMETHODCALLTYPE IocsQueryInterface(LPOLECLIENTSITE this,REFIID riid,LPVOID far *obj)
{
    LPSTRCLIENTSITE pSite=(LPSTRCLIENTSITE )this;

    (*obj)=NULL;    // set to NULL if interface not supported

    if (IsEqualIID(riid,&IID_IUnknown))        (*obj)=this;
    if (IsEqualIID(riid,&IID_IOleClientSite))  (*obj)=this;
    if (IsEqualIID(riid,&IID_IAdviseSink))     (*obj)=pSite->pObjAdvise;

    #if defined(WIN32)         // activeX interfaces
       if (IsEqualIID(riid,&IID_IOleInPlaceSite)) (*obj)=pSite->pInPlaceSite;
       if (IsEqualIID(riid,&IID_IOleInPlaceFrame))(*obj)=pSite->pInPlaceFrame;
       if (IsEqualIID(riid,&IID_IOleControlSite)) (*obj)=pSite->pOleControlSite;
       if (IsEqualIID(riid,&IID_IDispatch))       (*obj)=pSite->pDispatch;
    #endif

    if (*obj) {
       LPUNKNOWN far *ppUnk=(LPUNKNOWN far *)obj;
       (*ppUnk)->lpVtbl->AddRef(*ppUnk);
       return ResultFromScode(S_OK);
    }
    else return ResultFromScode(E_NOINTERFACE);
}

/*****************************************************************************
    IocsAddRef: AddRef function for IOleClientSite interface
******************************************************************************/
ULONG STDMETHODCALLTYPE IocsAddRef(LPOLECLIENTSITE this)
{
    LPSTRCLIENTSITE pSite=(LPSTRCLIENTSITE )this;

    pSite->count++;
    return pSite->count;
}
/*****************************************************************************
    IocsRelease: Release function for the IOleClientSite interface
******************************************************************************/
ULONG STDMETHODCALLTYPE IocsRelease(LPOLECLIENTSITE this)
{
    int obj;
    PTERWND w;

    LPSTRCLIENTSITE pSite=(LPSTRCLIENTSITE )this;

    w=pSite->w;
    obj=pSite->obj;

    pSite->count--;

    if (pSite->count==0) {      // unallocate pSite interface

       #if !defined(WIN32)
          FreeProcInstance((FARPROC)(this->lpVtbl->QueryInterface));
          FreeProcInstance((FARPROC)(this->lpVtbl->AddRef));
          FreeProcInstance((FARPROC)(this->lpVtbl->Release));
          FreeProcInstance((FARPROC)(this->lpVtbl->GetContainer));
          FreeProcInstance((FARPROC)(this->lpVtbl->OnShowWindow));
          FreeProcInstance((FARPROC)(this->lpVtbl->GetMoniker));
          FreeProcInstance((FARPROC)(this->lpVtbl->RequestNewObjectLayout));
          FreeProcInstance((FARPROC)(this->lpVtbl->SaveObject));
          FreeProcInstance((FARPROC)(this->lpVtbl->ShowObject));
       #endif

       OurFree(this->lpVtbl);
       OurFree(this);

       TerFont[obj].pObjSite=NULL;      // object deleted

       return 0;
    }

    return pSite->count;
}

/*****************************************************************************
    IocsSaveObject: SaveObject function for the IOleClientSite interface
    This function brings the data from the IPersistStorage to the IStorage
    interface.
******************************************************************************/
HRESULT STDMETHODCALLTYPE IocsSaveObject(LPOLECLIENTSITE this)
{
    PTERWND w;
    LPSTRCLIENTSITE pSite=(LPSTRCLIENTSITE )this;
    LPOLEOBJECT pObject;
    LPPERSISTSTORAGE lpPS;
    LPSTORAGE lpStg;
    HRESULT result;

    w=pSite->w;                  // window data pointer

    pObject=TerFont[pSite->obj].pObject; // object pointer
    lpStg=pSite->pObjStg;                 // object storage

    if (NOERROR!=(result=pObject->lpVtbl->QueryInterface(pObject,&IID_IPersistStorage,(LPVOID far *)&lpPS))) {
       PrintError(w,MSG_ERR_STORAGE_RETRIEVE,"IocsSaveObject");
       return result;
    }

    if (NOERROR!=(result=OleSave(lpPS,lpStg,TRUE))) {
       if ((DWORD)result!=(DWORD)OLE_E_BLANK) PrintError(w,MSG_ERR_OLE_SAVE,"IocsSaveObject");  // packager calls this interface with an uninitialized object
       return result;
    }

    lpPS->lpVtbl->SaveCompleted(lpPS,NULL);
    lpPS->lpVtbl->Release(lpPS);            // decrement for QueryInterface

    TerArg.modified++;                       // file modified

    return ResultFromScode(S_OK);
}

/*****************************************************************************
    Unimplemented functions for the IOleClientSite interface
******************************************************************************/
HRESULT STDMETHODCALLTYPE IocsGetContainer(LPOLECLIENTSITE this,LPOLECONTAINER far * ppContainer)
{
   (*ppContainer)=NULL;
   return ResultFromScode(E_NOTIMPL);
}
HRESULT STDMETHODCALLTYPE IocsOnShowWindow(LPOLECLIENTSITE this,BOOL show)
{
   return ResultFromScode(S_OK);
}
HRESULT STDMETHODCALLTYPE IocsGetMoniker(LPOLECLIENTSITE this,DWORD dwAssign,DWORD dwWhichMoniker,LPMONIKER far * ppmk)
{
   (*ppmk)=NULL;
   return ResultFromScode(E_NOTIMPL);
}
HRESULT STDMETHODCALLTYPE IocsRequestNewObjectLayout(LPOLECLIENTSITE this)
{
   return ResultFromScode(E_NOTIMPL);
}
HRESULT STDMETHODCALLTYPE IocsShowObject(LPOLECLIENTSITE this)
{
   return NOERROR;
}

/*****************************************************************************
    Implementation of IAdviseSink Interface
******************************************************************************/

/*****************************************************************************
    CreateAdviseSink:
    This function creates an instance of IAdviseSink interface.  The use
    count for the interface is set to 1.
******************************************************************************/
LPADVISESINK CreateAdviseSink(PTERWND w, int obj, LPSTORAGE pObjStg, LPOLECLIENTSITE pObjSite)
{
    LPSTRADVISESINK pTerAdvise;
    LPADVISESINK pAdvise;

    // allocate and inititialize pTerSite
    if (NULL==(pTerAdvise=(LPSTRADVISESINK )OurAlloc(sizeof(struct StrAdviseSink)))) return NULL;
    pTerAdvise->count=1;                     // count for the interface
    pTerAdvise->w=w;                         // window pointer for the pAdvise
    pTerAdvise->obj=obj;                   // object id for the pAdvise
    pTerAdvise->pObjStg=pObjStg;             // object IStorage
    pTerAdvise->pObjSite=pObjSite;           // client site for the object

    // allocate and intialize the CLIENTSITE virtual functions
    pAdvise=(LPADVISESINK)pTerAdvise;
    if (NULL==(pAdvise->lpVtbl=OurAlloc(sizeof(struct IAdviseSinkVtbl)))) return NULL;
    if ( NULL==(pAdvise->lpVtbl->QueryInterface=(LPVOID) MakeProcInstance((FARPROC)IasQueryInterface,hTerInst))
      || NULL==(pAdvise->lpVtbl->AddRef=(LPVOID) MakeProcInstance((FARPROC)IasAddRef,hTerInst))
      || NULL==(pAdvise->lpVtbl->Release=(LPVOID) MakeProcInstance((FARPROC)IasRelease,hTerInst))
      || NULL==(pAdvise->lpVtbl->OnDataChange=(LPVOID) MakeProcInstance((FARPROC)IasOnDataChange,hTerInst))
      || NULL==(pAdvise->lpVtbl->OnViewChange=(LPVOID) MakeProcInstance((FARPROC)IasOnViewChange,hTerInst))
      || NULL==(pAdvise->lpVtbl->OnRename=(LPVOID) MakeProcInstance((FARPROC)IasOnRename,hTerInst))
      || NULL==(pAdvise->lpVtbl->OnSave=(LPVOID) MakeProcInstance((FARPROC)IasOnSave,hTerInst))
      || NULL==(pAdvise->lpVtbl->OnClose=(LPVOID) MakeProcInstance((FARPROC)IasOnClose,hTerInst)) ){
       PrintError(w,MSG_ERR_ADV_CREATE,NULL);
       return NULL;
    }

    return pAdvise;
}

/*****************************************************************************
    IasQueryInterface: Query interface function for IAdviseSink.
    This function simply delegates to the IOleClientSite::QueryInterface
******************************************************************************/
HRESULT STDMETHODCALLTYPE IasQueryInterface(LPADVISESINK this,REFIID riid,LPVOID far *obj)
{
    LPSTRADVISESINK pAdvise=(LPSTRADVISESINK )this;
    LPOLECLIENTSITE pObjSite=pAdvise->pObjSite;

    return pObjSite->lpVtbl->QueryInterface(pObjSite,riid,obj);
}

/*****************************************************************************
    IasAddRef: AddRef function for IAdviseSink interface
******************************************************************************/
ULONG STDMETHODCALLTYPE IasAddRef(LPADVISESINK this)
{
    LPSTRADVISESINK pAdvise=(LPSTRADVISESINK )this;

    pAdvise->count++;
    return pAdvise->count;
}

/*****************************************************************************
    IasRelease: Release function for the IAdviseSink interface
******************************************************************************/
ULONG STDMETHODCALLTYPE IasRelease(LPADVISESINK this)
{
    LPSTRADVISESINK pAdvise=(LPSTRADVISESINK )this;
    LPOLECLIENTSITE pObjSite=pAdvise->pObjSite;
    LPSTRCLIENTSITE pSite=(LPSTRCLIENTSITE )pObjSite;

    pAdvise->count--;

    if (pAdvise->count==0) {      // unallocate pAdvise interface
       #if !defined(WIN32)
          FreeProcInstance((FARPROC)(this->lpVtbl->QueryInterface));
          FreeProcInstance((FARPROC)(this->lpVtbl->AddRef));
          FreeProcInstance((FARPROC)(this->lpVtbl->Release));
          FreeProcInstance((FARPROC)(this->lpVtbl->OnDataChange));
          FreeProcInstance((FARPROC)(this->lpVtbl->OnViewChange));
          FreeProcInstance((FARPROC)(this->lpVtbl->OnRename));
          FreeProcInstance((FARPROC)(this->lpVtbl->OnSave));
          FreeProcInstance((FARPROC)(this->lpVtbl->OnClose));
       #endif
       OurFree(this->lpVtbl);
       OurFree(this);

       // reset the pointer in the IOleClientSite interface
       if (pSite) pSite->pObjAdvise=NULL;

       return 0;
    }

    return pAdvise->count;
}

/*****************************************************************************
    IasOnViewChange: OnViewChange function for the IAdviseSink interface
******************************************************************************/
void STDMETHODCALLTYPE IasOnViewChange(LPADVISESINK this, DWORD dwAspect, LONG index)
{
     UpdateOleObject(this);
}

/*****************************************************************************
    IasOnSave: 
    This advise-sink is useful for the ole object that we read from an RTF
    file generated from other application.  Some application may not create the
    the ole object to provide FORMATETC set to provide OLE image to draw within
    TE.  This advise-sink then allows us to update the object when it is updated
    in the server. 
******************************************************************************/
void STDMETHODCALLTYPE IasOnSave(LPADVISESINK this) 
{
   UpdateOleObject(this);
}

/*****************************************************************************
    UpdateObject:
    Update object data and object image 
******************************************************************************/
void UpdateOleObject(LPADVISESINK this)
{
  
    LPSTRADVISESINK pAdvise=(LPSTRADVISESINK )this;
    LPOLECLIENTSITE pObjSite=pAdvise->pObjSite;
    LPSTRCLIENTSITE pSite=(LPSTRCLIENTSITE )pObjSite;
    int obj;
    LPOLEOBJECT pObject;
    LPDATAOBJECT pDataObject;
    PTERWND w;
    STGMEDIUM med;
    struct StrFont font;
    int    FontCharWidth[256];
    FORMATETC OleFmt;
    BOOL IsOcx;
    BOOL  ContentFound=FALSE,IconFound=FALSE;

    // retrieve the current ole item
    w=pSite->w;
    obj=pSite->obj;                         // item number in the font table
    
    if (/*TerFont[obj].ObjectType==OBJ_OCX  || */TerFont[obj].ObjectType==OBJ_OCX_EXT) return;  // OBJ_OCX needs this function to create the initial hMeta for the object
    
    pObject=TerFont[obj].pObject;

    // get the IDataObject Interface
    if (NOERROR!=(pObject->lpVtbl->QueryInterface(pObject,&IID_IDataObject,(LPVOID far *)&pDataObject))) {
       PrintError(w,MSG_ERR_DATA_RETRIEVE,"IasOnViewChange");
       return;
    }


    // get the metafile picture representation of the data
    OleFmt.cfFormat=CF_METAFILEPICT;
    OleFmt.ptd=NULL;
    OleFmt.tymed=TYMED_MFPICT;
    OleFmt.dwAspect=DVASPECT_CONTENT;
    OleFmt.lindex=-1;
    
    if (TerFont[obj].ObjectAspect==OASPECT_NONE) {      // try to guess if aspect is iconic
       SIZEL size;
       LPENUMFORMATETC pEnumFormatEtc;

       if (NOERROR==(pDataObject->lpVtbl->EnumFormatEtc(pDataObject,DATADIR_GET,&pEnumFormatEtc))) {
           FORMATETC EnumFmt;
           while (NOERROR==(pEnumFormatEtc->lpVtbl->Next(pEnumFormatEtc,1,&EnumFmt,NULL))) {
              if (OleFmt.dwAspect==OASPECT_CONTENT) ContentFound=TRUE;
              if (OleFmt.dwAspect==OASPECT_ICON)    IconFound=TRUE;
           }
           pEnumFormatEtc->lpVtbl->Release(pEnumFormatEtc);
           
           if (ContentFound) OleFmt.dwAspect=OASPECT_CONTENT;
           else if (IconFound) OleFmt.dwAspect=OASPECT_ICON;
       } 
       if (!ContentFound && !IconFound) {
          if (NOERROR==(pObject->lpVtbl->GetExtent(pObject,DVASPECT_CONTENT,&size))) {
             int width=(int)((size.cx*1440L)/2540);          // current width/height
             int height=(int)((size.cy*1440L)/2540);
             int tol=100;  // tolerance in twips
             if (abs(TerFont[obj].PictWidth-width)<=tol && abs(TerFont[obj].PictHeight-height)<=tol) 
                  TerFont[obj].ObjectAspect=OASPECT_CONTENT;
             else TerFont[obj].ObjectAspect=OASPECT_ICON;
          }
       }
    }

    if (TerFont[obj].ObjectAspect==OASPECT_CONTENT || ContentFound) {
       if (NOERROR!=(pDataObject->lpVtbl->GetData(pDataObject,&OleFmt,&med)) || med.tymed!=TYMED_MFPICT) return;
    }
    else return;

    pDataObject->lpVtbl->Release(pDataObject);   // no longer needed


    // Incorporate this data into our font table
    font=TerFont[obj];                       // make a copy of the exiting font structure
    FarMove(TerFont[obj].CharWidth,FontCharWidth,sizeof(FontCharWidth));

    InitTerObject(w,obj);                    // freeup this slot
    if (TerCreateMetafile(w,(HGLOBAL)(med.u.hGlobal),obj)<0) {
       TerFont[obj]=font;                    // restore the previous structure
       FarMove(FontCharWidth,TerFont[obj].CharWidth,sizeof(FontCharWidth)); // restore the width table
       ReleaseStgMedium(&med);                      // release data handle
       return;
    }

    ReleaseStgMedium(&med);                      // release data handle

    // Apply the previous values
    TerFont[obj].ObjectType=font.ObjectType;
    TerFont[obj].pObject=font.pObject;
    TerFont[obj].pObjSite=font.pObjSite;
    TerFont[obj].pObjStg=font.pObjStg;
    TerFont[obj].ObjectSize=font.ObjectSize;
    TerFont[obj].ObjectAspect=font.ObjectAspect;
    TerFont[obj].ObjectUpdate=font.ObjectUpdate;
    TerFont[obj].AuxId=font.AuxId;
    TerFont[obj].style=font.style;
    TerFont[obj].flags=font.flags;
    TerFont[obj].ParaFID=font.ParaFID;
    TerFont[obj].FrameType=font.FrameType;
    TerFont[obj].MapId=font.MapId;
    TerFont[obj].OleStgName=font.OleStgName;

    if (TerFont[obj].flags&FFLAG_RESIZED) {  // restore the previous object size
       TerFont[obj].PictHeight=font.PictHeight;
       TerFont[obj].PictWidth=font.PictWidth;
       SetPictSize(w,obj,TwipsToScrY(TerFont[obj].PictHeight),TwipsToScrX(TerFont[obj].PictWidth),TRUE);
       XlateSizeForPrt(w,obj);              // convert size to printer resolution
    } 

    SetOlePictOffset(w,obj);                     // retrieve picture offset from the metafile

    // Insert the object
    if (TerFont[obj].ObjectType==OBJ_EMBED_INSERT) {  // item being inserted
        TerFont[obj].ObjectType=OBJ_EMBED;
        TerInsertObjectId(hTerWnd,obj,TRUE);        // this is a new object to be inserted
    }
    else {
        if (font.hMeta) DeleteMetaFile(font.hMeta); // delete the previous metafile
    }

    //** reset the text map as the contents of the picture has changed
    IsOcx=TerFont[obj].ObjectType==OBJ_OCX || TerFont[obj].ObjectType==OBJ_OCX_EXT;
    if (!TerFont[obj].flags&FFLAG_CTL_VISIBLE || !IsOcx) {    // when not an visible ocx
      DeleteTextMap(w,TRUE);
      PaintTer(w);
    }

    TerArg.modified++;                             // file modified
}

/*****************************************************************************
    Unimplemented functions for the IAdviseSink interface
******************************************************************************/
void STDMETHODCALLTYPE IasOnClose(LPADVISESINK this) {}
void STDMETHODCALLTYPE IasOnDataChange(LPADVISESINK this, FORMATETC far * fmt, STGMEDIUM far * med) {}
void STDMETHODCALLTYPE IasOnRename(LPADVISESINK this, LPMONIKER pmk) {}



/******************************************************************************
     TerCreateOcxObject:
    Create an instance of the active X object. The prog name would be the name
    of an ActiveX control file such toc.ocx
******************************************************************************/
int WINAPI _export TerCreateOcxObject(HWND hWnd,LPBYTE OcxProgName, BOOL msg)
{
    int SubKey;
    PTERWND w;
    BOOL FullPath=FALSE;
    int i,len,obj;
    BYTE string[MAX_WIDTH+1];
    BOOL found=FALSE;
    LPBYTE ptr;
    long  BufSize;
    CLSID ClassId;
    LPSTORAGE pObjStg;
    LPOLECLIENTSITE pObjSite;
    HRESULT result;
    FORMATETC OleFmt;
    LPDISPATCH pDispatch;
    BYTE ProgId[50];

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (lstrlen(OcxProgName)==0) return 0;

    // set the prog id for known system classes
    ProgId[0]=0;
    if (lstrcmpi(OcxProgName,"msscript.ocx")==0) lstrcpy(ProgId,"MSScriptControl.ScriptControl");

    // check if the full path specified
    len=lstrlen(OcxProgName);
    for (i=len-1;i>=0;i--) if (OcxProgName[i]=='\\' || OcxProgName[i]==':') break;
    if (i>=0) FullPath=TRUE;

    // find this program in the registration database
    for (SubKey=0;TRUE;SubKey++) {
       if (lstrlen(ProgId)>0) {
          if (SubKey==0) lstrcpy(TempString,ProgId);
          else break;
       }
       else if (ERROR_SUCCESS!=RegEnumKey(HKEY_CLASSES_ROOT,SubKey,TempString,sizeof(TempString)-1)) break;

       // check if it is an OLE server
       lstrcpy(TempString1,TempString);
       lstrcat(TempString1,"\\CLSID");         // look for HKEY_CLASSES_ROOT\progid\CLSID
       BufSize=sizeof(string)-1;
       if (ERROR_SUCCESS!=RegQueryValue(HKEY_CLASSES_ROOT,TempString1,string,&BufSize)) continue;  // not an ole server


       // ensure that HKEY_CLASSES_ROOT\CLSID\clsid\InprocServer32 key exists
       // The value for this key has the program pathname
       lstrcpy(TempString1,"CLSID\\");
       lstrcat(TempString1,string);
       lstrcat(TempString1,"\\InprocServer32");
       if (ERROR_SUCCESS!=RegQueryValue(HKEY_CLASSES_ROOT,TempString1,string,&BufSize)) continue;  // not an ole server

       // check for the program name match
       if (FullPath) {
          if (lstrcmpi(string,OcxProgName)==0) found=TRUE;
       }
       else {
          if (lstrlen(string)>=lstrlen(OcxProgName)) {
             ptr=&(string[lstrlen(string)-lstrlen(OcxProgName)]);
             if (lstrcmpi(ptr,OcxProgName)==0) found=TRUE;
          }
       }

       // check if the class description exists
       if (found) {
          BufSize=sizeof(string)-1;
          if (ERROR_SUCCESS!=RegQueryValue(HKEY_CLASSES_ROOT,TempString,string,&BufSize)) continue;  // class description missing
          break;
       }
    }

    if (!found) return 0;

    // convert program-id to class id
    result=CLSIDFromProgID(StrToOleStr(TempString,TempString1),&ClassId);
    if (NOERROR!=result)  {
        if (msg) return PrintError(w,MSG_NO_OLE_CLASS,"");     // convert to class id
        else     return 0;
    }

    // create the object
    if ((obj=FindOpenSlot(w))==-1) return 0; // find an empty slot to make the object name
    TerFont[obj].InUse=TRUE;
    TerFont[obj].ObjectType=OBJ_OCX_EXT;    // external ocx object that is not inserted in the text
    TerFont[obj].style=PICT;
    TerFont[obj].PictType=PICT_METAFILE;
    TerFont[obj].hMeta=0;

    // create IOleClientSite and IStorage interfaces for the object
    if (!TerOlePreCreate(w,obj,&pObjSite,&pObjStg,&OleFmt)) return 0;

    pDispatch=NULL;
    result=CoCreateInstance(&ClassId,NULL,CLSCTX_ALL,&IID_IDispatch,(LPVOID far *)&pDispatch);

    if (NOERROR!=result)  {
       TerFont[obj].pObject=NULL; 
       DeleteTerObject(w,obj); 
       if (msg) return PrintError(w,MSG_NO_SERVER,"TerCreateOcxObject");
       else     return 0;
    }

    TerFont[obj].pObject=(LPVOID)pDispatch;

    return obj;
}

/******************************************************************************
     TerCreateOcxObject2:
    Create an instance of the active X object.
******************************************************************************/
int WINAPI _export TerCreateOcxObject2(HWND hWnd,LPBYTE ProgId, int width, int height)
{
    PTERWND w;
    int  obj;
    BYTE string[MAX_WIDTH+1];
    long  BufSize;
    CLSID ClassId;
    LPSTORAGE pObjStg;
    LPOLECLIENTSITE pObjSite;
    HRESULT result;
    FORMATETC OleFmt;
    BYTE ClassIdString[100];
    LPSTREAM pStream;
    WORD wname[100];
    
    //LPDISPATCH pDispatch;
    //LPOLECONTROL pControl;

    if (NULL==(w=GetWindowPointer(hWnd))) return 0;  // get the pointer to window data

    if (lstrlen(ProgId)==0) return 0;

    // check if it is an OLE server
    lstrcpy(TempString1,ProgId);
    lstrcat(TempString1,"\\CLSID");         // look for HKEY_CLASSES_ROOT\progid\CLSID
    BufSize=sizeof(ClassIdString)-1;
    if (ERROR_SUCCESS!=RegQueryValue(HKEY_CLASSES_ROOT,TempString1,ClassIdString,&BufSize)) return 0;  // Get the class id

    // ensure that HKEY_CLASSES_ROOT\CLSID\clsid\InprocServer32 key exists
    // The value for this key has the program pathname
    lstrcpy(TempString1,"CLSID\\");
    lstrcat(TempString1,ClassIdString);
    lstrcat(TempString1,"\\InprocServer32");
    BufSize=sizeof(string)-1;
    if (ERROR_SUCCESS!=RegQueryValue(HKEY_CLASSES_ROOT,TempString1,string,&BufSize)) return 0;  // not an ole server

    // convert program-id to class id
    result=CLSIDFromProgID(StrToOleStr(ProgId,TempString1),&ClassId);
    if (NOERROR!=result)  return PrintError(w,MSG_NO_OLE_CLASS,"");     // convert to class id


    // create the object
    if ((obj=FindOpenSlot(w))==-1) return 0; // find an empty slot to make the object name
    TerFont[obj].InUse=TRUE;
    TerFont[obj].ObjectType=OBJ_OCX;    // external ocx object that is not inserted in the text
    TerFont[obj].ObjectAspect=OASPECT_CONTENT;
    TerFont[obj].style=PICT;
    TerFont[obj].PictType=PICT_METAFILE;
    TerFont[obj].hMeta=0;

    // create IOleClientSite and IStorage interfaces for the object
    if (!TerOlePreCreate(w,obj,&pObjSite,&pObjStg,&OleFmt)) return 0;

    //pControl=NULL;
    //result=CoCreateInstance(&ClassId,NULL,CLSCTX_ALL,&IID_IOleControl,(LPVOID far *)&pControl);
    //result=pControl->lpVtbl->QueryInterface(pControl,&IID_IOleObject,(LPVOID far *)&(TerFont[obj].pObject));

    // Create the object
    result=OleCreate(&ClassId,&IID_IOleObject,OLERENDER_DRAW/*OLERENDER_FORMAT*/,NULL/*&OleFmt*/,pObjSite,pObjStg,(LPVOID far *)&(TerFont[obj].pObject));
    if (result!=NOERROR || !(TerFont[obj].pObject)) {
       TerFont[obj].InUse=FALSE;
       pObjSite->lpVtbl->Release(pObjSite);    // release the client site
       PrintOleError(w,result);
       return PrintError(w,MSG_NO_SERVER,"TerCreateOcxObject2");
    }

    // make advisory connection and display the object
    TerOlePostCreate(w,obj);

    // Write the ObjInfo stream so MSWord accepts this object as an OCX
    StrToOleStr("ObjInfo",(LPSTR)&(wname[1]));
    wname[0]=3;    // first character of the stream name
    if (NOERROR!=(result=pObjStg->lpVtbl->CreateStream(pObjStg,wname,
                                                STGM_READWRITE|STGM_SHARE_EXCLUSIVE ,0,0,&pStream))) {
       PrintOleError(w,result);
       return PrintError(w,MSG_ERR_STORAGE_CREATE,"TerCreateOcxObject2(a)");
    }
    wname[0]=0x1200;      // 0x1200  magic numbers
    wname[1]=0x3;         // 0x3
    wname[2]=0x4;         // 0x4
    pStream->lpVtbl->Write(pStream,wname,6,NULL);
    pStream->lpVtbl->Release(pStream);  // close stream

    // maintain the original metafile display dimensions
    TerFont[obj].PictHeight=height;
    TerFont[obj].PictWidth=width;
    SetPictSize(w,obj,TwipsToScrY(TerFont[obj].PictHeight),TwipsToScrX(TerFont[obj].PictWidth),TRUE);
    XlateSizeForPrt(w,obj);              // convert size to printer resolution

    TerInsertObjectId(hTerWnd,obj,TRUE);   // insert this object into text

    return obj;
}

/******************************************************************************
     ShowOcx:
     Show an ocx control.
******************************************************************************/
BOOL ShowOcx(PTERWND w, int obj)
{
    HRESULT result;

    if (TerFlags&TFLAG_NO_OLE) return TRUE;

    if (TerFont[obj].ObjectType==OBJ_OCX && !(TerFont[obj].flags&(FFLAG_CTL_VISIBLE|FFLAG_OCX_SRV_ERROR))) {
       LPOLEOBJECT pObject=TerFont[obj].pObject;

       TerFont[obj].flags|=FFLAG_CTL_VISIBLE;
       result=pObject->lpVtbl->DoVerb(pObject,OLEIVERB_INPLACEACTIVATE,NULL,TerFont[obj].pObjSite,0,hTerWnd,NULL);

       if (result==NO_ERROR) {
          TerFont[obj].flags|=FFLAG_CTL_VISIBLE;
          PumpCtlMessages(w,obj);
       }
       else {
          TerFont[obj].flags=ResetUintFlag(TerFont[obj].flags,FFLAG_CTL_VISIBLE);
          TerFont[obj].flags|=FFLAG_OCX_SRV_ERROR;
       }
    }

    return TRUE;
}

/******************************************************************************
     HideOcx:
     Hide an ocx control.
******************************************************************************/
BOOL HideOcx(PTERWND w, int obj)
{
    HRESULT result;
    int first=0,last=TotalFonts-1,x,y,width,height;
    LPOLEOBJECT pObject;
    LPOLEINPLACEOBJECT pInPlaceObject;
    HBITMAP hBM,hOldBM;

    if (TerFlags&TFLAG_NO_OLE) return TRUE;

    if (obj>0) first=last=obj;

    for (obj=first;obj<=last;obj++) {
      if (!(TerFont[obj].InUse) || !(TerFont[obj].style&PICT)) continue;
      if (TerFont[obj].ObjectType!=OBJ_OCX || !(TerFont[obj].flags&FFLAG_CTL_VISIBLE)) continue;

      // save the pixels underneath the picture
      x=TerFont[obj].PictX+TerWinOrgX;
      y=TerFont[obj].PictY+TerWinOrgY;
      width=TerFont[obj].CharWidth[PICT_CHAR];
      height=TerFont[obj].height;

      if (NULL!=(hBM=CreateCompatibleBitmap(hTerDC,width,height))) {
         hOldBM=SelectObject(hMemDC,hBM);
         BitBlt(hMemDC,0,0,width,height,hTerDC,x,y,SRCCOPY);
      }

      pObject=TerFont[obj].pObject;

      result=pObject->lpVtbl->QueryInterface(pObject,&IID_IOleInPlaceObject,(LPVOID far *)&pInPlaceObject);
      pInPlaceObject->lpVtbl->InPlaceDeactivate(pInPlaceObject);
      pInPlaceObject->lpVtbl->Release(pInPlaceObject);

      result=pObject->lpVtbl->DoVerb(pObject,OLEIVERB_HIDE,NULL,TerFont[obj].pObjSite,0,hTerWnd,NULL);
      if (result==NOERROR) TerFont[obj].flags=ResetUintFlag(TerFont[obj].flags,FFLAG_CTL_VISIBLE);

      // restore the window area
      if (hBM) {
         BitBlt(hTerDC,x,y,width,height,hMemDC,0,0,SRCCOPY);
         SelectObject(hMemDC,hOldBM);
         DeleteObject(hBM);
      }
    }

    return TRUE;
}

/******************************************************************************
    PosOcx:
    Position an ocx control at the current location in the editor
******************************************************************************/
BOOL PosOcx(PTERWND w, int obj,HDC hDC)
{
    RECT PosRect,ClpRect;
    LPOLEOBJECT pObject;
    LPOLEINPLACEOBJECT pInPlaceObject;
    HRESULT result;
    RECT PictRect;

    if (TerFlags&TFLAG_NO_OLE) return TRUE;

    PosRect.left=TerFont[obj].PictX+TerWinRect.left;
    PosRect.top=TerFont[obj].PictY+TerWinRect.top;
    PosRect.right=PosRect.left+TerFont[obj].CharWidth[PICT_CHAR];
    PosRect.bottom=PosRect.top+TerFont[obj].height;

    PictRect.left=TerFont[obj].PictX+TerWinOrgX;
    PictRect.top=TerFont[obj].PictY+TerWinOrgY;
    PictRect.right=PictRect.left+TerFont[obj].CharWidth[PICT_CHAR];
    PictRect.bottom=PictRect.top+TerFont[obj].height;

    ClpRect=TerWinRect;


    pObject=TerFont[obj].pObject;
    result=pObject->lpVtbl->QueryInterface(pObject,&IID_IOleInPlaceObject,(LPVOID far *)&pInPlaceObject);
    if (result==NOERROR) {
       pInPlaceObject->lpVtbl->SetObjectRects(pInPlaceObject,&PosRect,&ClpRect);
       OleDraw((LPUNKNOWN)pObject,DVASPECT_CONTENT,hDC,&PictRect);

       pInPlaceObject->lpVtbl->Release(pInPlaceObject);
    }

    return TRUE;
}


/******************************************************************************
     PumpCtlMessage:
     Pump the pending control messages.
******************************************************************************/
BOOL PumpCtlMessages(PTERWND w,int obj)
{
    HRESULT result;
    LPOLEOBJECT pObject=TerFont[obj].pObject;
    LPOLEINPLACEOBJECT pInPlaceObject;
    HWND hCtlWnd=NULL;
    MSG msg;

    result=pObject->lpVtbl->DoVerb(pObject,OLEIVERB_INPLACEACTIVATE,NULL,TerFont[obj].pObjSite,0,hTerWnd,NULL);
    if (result!=NOERROR) return FALSE;

    result=pObject->lpVtbl->QueryInterface(pObject,&IID_IOleInPlaceObject,(LPVOID far *)&pInPlaceObject);
    if (result==NOERROR) {
       pInPlaceObject->lpVtbl->GetWindow(pInPlaceObject,&hCtlWnd);
       pInPlaceObject->lpVtbl->Release(pInPlaceObject);
    }

    if (hCtlWnd) {
       while (PeekMessage(&msg,hCtlWnd,0,0,PM_REMOVE)) {
          if (msg.message==WM_QUIT) break;
          TranslateMessage(&msg);
          DispatchMessage(&msg);
       }
    }

    return TRUE;
}

/******************************************************************************
     TerGetOlePtr:
     Get various object pointers
******************************************************************************/
LPVOID WINAPI _export TerGetOlePtr(HWND hWnd,int obj, int type)
{
    PTERWND w;

    if (NULL==(w=GetWindowPointer(hWnd))) return NULL;  // get the pointer to window data

    if (obj<0 || obj>=TotalFonts || !TerFont[obj].InUse || !(TerFont[obj].style&PICT) || TerFont[obj].ObjectType==OBJ_NONE) return 0;

    if (type==OLEPTR_OBJECT) return (LPVOID)TerFont[obj].pObject;
    if (type==OLEPTR_STORAGE) return (LPVOID) TerFont[obj].pObjStg;

    return NULL;
}
 
/******************************************************************************
     TerGetIDispatch:
     Get the IDispatch pointer for an object
******************************************************************************/
LPVOID WINAPI _export TerGetIDispatch(HWND hWnd,int obj, BOOL ReleaseId)
{
    PTERWND w;
    LPDISPATCH pDispatch;
    LPOLEOBJECT pObject=NULL;
    HRESULT result;
    BOOL IsExtOcx;

    if (NULL==(w=GetWindowPointer(hWnd))) return NULL;  // get the pointer to window data

    IsExtOcx=(TerFont[obj].ObjectType==OBJ_OCX_EXT);

    if (IsExtOcx) {
        pDispatch=(LPVOID)TerFont[obj].pObject;
        if (ReleaseId) {
           TerFont[obj].pObject=NULL;
           DeleteTerObject(w,obj);
        }    
    }
    else {
        pObject=TerFont[obj].pObject;
        result=pObject->lpVtbl->QueryInterface(pObject,&IID_IDispatch,(LPVOID far *)&pDispatch);
        if (result!=NOERROR) return NULL;
    }
    
    return (LPVOID)pDispatch;
}

/******************************************************************************
     TerDeleteIDispatch:
     Delete a IDispatch pointer
******************************************************************************/
BOOL  WINAPI _export TerDeleteIDispatch(LPVOID pDisp)
{
    LPDISPATCH pDispatch=pDisp;

    pDispatch->lpVtbl->Release(pDispatch);

    return TRUE;
}

/******************************************************************************
     TerSetOcxIntProp:
     Set the integer property for an ocx control
******************************************************************************/
BOOL WINAPI _export TerSetOcxIntProp(HWND hWnd,int obj,LPBYTE prop, int value)
{
    int ResultType;
    long ResultValue;
    
    return TerInvokeOcx(hWnd,obj,prop,OCX_SET_PROP,OCX_LONG,(DWORD)value,&ResultType,&ResultValue,0);
}
     
/******************************************************************************
     TerSetOcxTextProp:
     Set the text property for an ocx control
******************************************************************************/
BOOL WINAPI _export TerSetOcxTextProp(HWND hWnd,int obj,LPBYTE prop, LPBYTE text)
{
    int ResultType;
    long ResultValue;
    
    return TerInvokeOcx(hWnd,obj,prop,OCX_SET_PROP,OCX_STRING,(DWORD)text,&ResultType,&ResultValue,0);
}
     
/******************************************************************************
     TerInvokeOcx:
     Set/get the ActiveX property or method
******************************************************************************/
BOOL WINAPI _export TerInvokeOcx(HWND hWnd,int obj,LPBYTE name, UINT flags, int ArgType, DWORD pArg, LPINT pResultType, LPLONG pResult, int ResultLen)
{
    PTERWND w;
    LPDISPATCH pDispatch;
    LPOLEOBJECT pObject=NULL;
    HRESULT result;
    BOOL rtn,IsExtOcx;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    IsExtOcx=(TerFont[obj].ObjectType==OBJ_OCX_EXT);

    if (IsExtOcx) pDispatch=(LPVOID)TerFont[obj].pObject;
    else {
        pObject=TerFont[obj].pObject;
        result=pObject->lpVtbl->QueryInterface(pObject,&IID_IDispatch,(LPVOID far *)&pDispatch);
        if (result!=NOERROR) return FALSE;
    }
    
    rtn=TerInvokeOcx2(hWnd,(DWORD)pDispatch,name,flags,ArgType,pArg,pResultType,pResult,ResultLen);

    if (!IsExtOcx) pDispatch->lpVtbl->Release(pDispatch);    // release the displatch pointer

    return rtn;
}

/******************************************************************************
     TerInvokeOcx2:
     Set/get the ActiveX property or method given the dispatch pointer for the ocx.
******************************************************************************/
BOOL WINAPI _export TerInvokeOcx2(HWND hWnd,DWORD dwDispatch,LPBYTE name, UINT flags, int ArgType, DWORD pArg, LPINT pResultType, LPLONG pResult, int ResultLen)
{
    PTERWND w;
    LPDISPATCH pDispatch=(LPDISPATCH)dwDispatch;
    VARIANTARG arg,VarResult;
    DISPID ArgDispId=DISPID_PROPERTYPUT;
    DISPID dispid;
    DISPPARAMS params;
    HRESULT result;
    WORD InvokeFlags;

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    dispid=DispIdFromName2(w,pDispatch,name);                   // translate name to disp id
    if (dispid==0) return FALSE;

    // set flags
    if (flags==OCX_METHOD)       InvokeFlags=DISPATCH_METHOD;
    if (flags==OCX_GET_PROP)     InvokeFlags=DISPATCH_PROPERTYGET;
    if (flags==OCX_SET_PROP)     InvokeFlags=DISPATCH_PROPERTYPUT;
    if (flags==OCX_SET_PROP_REF) InvokeFlags=DISPATCH_PROPERTYPUTREF;


    // set parameters for passing
    params.rgvarg=NULL;
    params.cArgs=0;
    params.rgdispidNamedArgs=NULL;
    params.cNamedArgs=0;

    if (ArgType!=OCX_NONE) {    // proptery-get or parameter no used
        if (ArgType==OCX_BOOL) TerSetBool(&arg,(BOOL)pArg);
        else if (ArgType==OCX_LONG) {
           V_VT(&arg)=VT_I4;
           V_UNION(&arg,lVal)=pArg;
        }
        else if (ArgType==OCX_STRING) {
           LPBYTE pOleStr;
           V_VT(&arg)=VT_BSTR;
           pOleStr=OurAlloc(lstrlen((LPSTR)pArg)*2+1);
           V_UNION(&arg,bstrVal)=SysAllocString(StrToOleStr((LPSTR)pArg,pOleStr));
           OurFree(pOleStr);
        }

        params.rgvarg=&arg;
        params.cArgs=1;

        if (flags==OCX_SET_PROP) {             // property sets treats the first parameters as a named parameter
           params.rgdispidNamedArgs=&ArgDispId;
           params.cNamedArgs=1;
        }
    }


    result=pDispatch->lpVtbl->Invoke(pDispatch,dispid,&IID_NULL,LOCALE_USER_DEFAULT,InvokeFlags,&params,&VarResult,NULL,NULL);


    // Access the result
    if (pResult && pResultType && (flags==OCX_METHOD || flags==OCX_GET_PROP)) {
        (*pResultType)=OCX_NONE;
        (*pResult)=0;
        if (V_VT(&VarResult)==VT_BOOL) {
            (*pResultType)=OCX_BOOL;
            (*pResult)=TerGetBool(&VarResult);
        }
        else if (V_VT(&VarResult)==VT_I4) {
            (*pResultType)=OCX_LONG;
            (*pResult)=V_UNION(&VarResult,lVal);
        }
        else if (V_VT(&VarResult)==VT_BSTR) {
            (*pResultType)=OCX_STRING;
            OleStrToStr((LPSTR)V_UNION(&VarResult,bstrVal),(LPSTR)pResult,ResultLen);
            SysFreeString(V_UNION(&VarResult,bstrVal));
        }
        else if (V_VT(&VarResult)==VT_DISPATCH) {
            (*pResultType)=OCX_DISPATCH;
            (*pResult)=(DWORD)(V_UNION(&VarResult,pdispVal));  // force LPDISPATCH into a DWORD
        }    
    }


    // free the alloced bstr strings
    if (ArgType!=OCX_NONE && ArgType==OCX_STRING) SysFreeString(V_UNION(&arg,bstrVal));

    return TRUE;
}

/******************************************************************************
     DispIdFromName:
     Get DispId for a name.
******************************************************************************/
DISPID DispIdFromName(PTERWND w,int obj,LPBYTE name)
{
    LPDISPATCH pDispatch;
    LPOLEOBJECT pObject=NULL;
    HRESULT result;
    DISPID DispId;
    BOOL IsExtOcx=(TerFont[obj].ObjectType==OBJ_OCX_EXT);

    if (IsExtOcx) pDispatch=(LPVOID)TerFont[obj].pObject;
    else {
        pObject=TerFont[obj].pObject;
        result=pObject->lpVtbl->QueryInterface(pObject,&IID_IDispatch,(LPVOID far *)&pDispatch);
        if (result!=NOERROR) return (DISPID)0;
    }

    DispId=DispIdFromName2(w,pDispatch,name);

    if (!IsExtOcx) pDispatch->lpVtbl->Release(pDispatch);    // release the displatch pointer

    return DispId;
}

/******************************************************************************
     DispIdFromName2:
     Get DispId for a name given the dispatch id
******************************************************************************/
DISPID DispIdFromName2(PTERWND w,LPDISPATCH pDispatch,LPBYTE name)
{
    HRESULT result;
    OLECHAR OleName[MAX_WIDTH+1];
    OLECHAR far *pOleName=OleName;
    DISPID DispId;

    StrToOleStr((LPSTR)name,(LPSTR)OleName);

    result=pDispatch->lpVtbl->GetIDsOfNames(pDispatch,&IID_NULL,&pOleName,1,LOCALE_USER_DEFAULT,&DispId);

    if (result!=NOERROR) return (DISPID)0;

    return DispId;
}

/******************************************************************************
     SetPropBool:
     Set a boolean property.
******************************************************************************/
BOOL SetPropBool(PTERWND w,int obj,DISPID dispid,BOOL val)
{
    LPDISPATCH pDispatch;
    LPOLEOBJECT pObject=NULL;
    VARIANTARG arg;
    DISPID ArgDispId=DISPID_PROPERTYPUT;
    DISPPARAMS params;
    HRESULT result;
    BOOL IsExtOcx=(TerFont[obj].ObjectType==OBJ_OCX_EXT);

    if (IsExtOcx) pDispatch=(LPVOID)TerFont[obj].pObject;
    else {
        pObject=TerFont[obj].pObject;
        result=pObject->lpVtbl->QueryInterface(pObject,&IID_IDispatch,(LPVOID far *)&pDispatch);
        if (result!=NOERROR) return FALSE;
    }


    // set the argument
    //V_UNION(&arg,boolVal)=(VARIANT_BOOL)val;
    TerSetBool(&arg,val);

    params.rgvarg=&arg;
    params.cArgs=1;
    params.rgdispidNamedArgs=&ArgDispId;
    params.cNamedArgs=1;

    result=pDispatch->lpVtbl->Invoke(pDispatch,dispid,&IID_NULL,LOCALE_USER_DEFAULT,DISPATCH_PROPERTYPUT,&params,NULL,NULL,NULL);

    if (!IsExtOcx) pDispatch->lpVtbl->Release(pDispatch);    // release the displatch pointer

    return TRUE;
}

/******************************************************************************
     TerGetBool:
     Set a bool value into the given VARIANTARG pointer
******************************************************************************/
BOOL TerGetBool(VARIANTARG far *pArg)
{
     #if defined(__TURBOC__)
       return (BOOL) V_UNION(pArg,boolVal);
     #else
       return (BOOL) V_BOOL(pArg);
     #endif
}

/******************************************************************************
     TerSetBool:
     Set a bool value into the given VARIANTARG pointer
******************************************************************************/
TerSetBool(VARIANTARG far *pArg,BOOL val)
{
     #if defined(__TURBOC__)
       V_UNION(pArg,boolVal)=(VARIANT_BOOL)val;
     #else
       V_BOOL(pArg)=val;
     #endif

     return TRUE;
}

/******************************************************************************
     TerGetOleClass:
     Get property information for an embedded ActiveX control.
******************************************************************************/
BOOL WINAPI _export TerGetOleClass(HWND hWnd, int obj, LPBYTE ClassText)
{
    PTERWND w;
    LPOLEOBJECT pObject;
    CLSID ClsId;
    LPOLESTR pOleStr=NULL;
    HRESULT result;

    ClassText[0]=0;                  // initialize the out variable

    if (NULL==(w=GetWindowPointer(hWnd))) return FALSE;  // get the pointer to window data

    if (!(TerFont[obj].style&PICT)) return FALSE;
    if (TerFont[obj].ObjectType!=OBJ_OCX) return FALSE;

    pObject=TerFont[obj].pObject;
 
    result=pObject->lpVtbl->GetUserClassID(pObject, &ClsId);

    if (result!=NOERROR) return FALSE;

    result=StringFromCLSID(&ClsId,&pOleStr);

    if (result!=NOERROR || pOleStr==NULL) return FALSE;

    OleStrToStr((LPSTR)pOleStr,ClassText,0);

    CoTaskMemFree(pOleStr);   // free the space allocated by OLE system

    return TRUE;
}
 
/******************************************************************************
     TerOcxPropInfo:
     Get property information for an embedded ActiveX control.
******************************************************************************/
int WINAPI _export TerOcxPropInfo(HWND hWnd, int obj, int PropNo, LPINT pPropType, LPBYTE pPropName, LPLONG pPropInt, LPBYTE pPropText)
{
    PTERWND w;
    LPOLEOBJECT pObject;
    LPPERSISTPROPERTYBAG pPersistProp;
    //VARIANTARG arg,VarResult;
    //LPDISPATCH pDispatch;
    //DISPID ArgDispId=DISPID_PROPERTYPUT;
    //DISPID dispid;
    //DISPPARAMS params;
    LPPROPERTYBAG pPropBag;
    LPSTRPROPERTYBAG pProperty;
    HRESULT result;
    int ReturnVal=1,TextLen;

    if (NULL==(w=GetWindowPointer(hWnd))) return -1;  // get the pointer to window data

    if (!(TerFont[obj].style&PICT)) return -1;
    if (TerFont[obj].ObjectType!=OBJ_OCX) return -1;

    pObject=TerFont[obj].pObject;

    result=pObject->lpVtbl->QueryInterface(pObject,&IID_IPersistPropertyBag,(LPVOID far *)&pPersistProp);
    if (result!=NOERROR) return -1;

    pPropBag=CreatePropertyBag(w);

    // initialize the structure with count variables
    pProperty=(LPSTRPROPERTYBAG)pPropBag;
    pProperty->PropCount=0;
    pProperty->id=PropNo;
    pProperty->type=OCX_NONE;
    
    result=pPersistProp->lpVtbl->Save(pPersistProp,pPropBag,TRUE,FALSE);  // 3rd arg= true for clear dirty, 4th arg=true for save all, false for save properties with non-default values 
    if (result!=NOERROR) return -1;

    if (PropNo<0) {
       ReturnVal=pProperty->PropCount;  // getting the prop count;
       goto END;
    }

    // pass the property value
    if (pPropType) (*pPropType)=pProperty->type;
    if (pPropName) lstrcpy(pPropName,pProperty->name);
    if (pPropInt)  (*pPropInt)=0;   // initialize
    if (pPropText) pPropText[0]=0;

    if (pProperty->type==OCX_BOOL || pProperty->type==OCX_LONG) (*pPropInt)=pProperty->val;
    else {
       if (pProperty->pText==NULL) TextLen=0;
       else                        TextLen=lstrlen(pProperty->pText);
       ReturnVal=TextLen;

       if (pPropText!=NULL) lstrcpy(pPropText,pProperty->pText);  // copy to out variable

       MemFree(pProperty->pText);
       pProperty->pText=NULL; 
    } 

    END:
    pPropBag->lpVtbl->Release(pPropBag);
    pPersistProp->lpVtbl->Release(pPersistProp);

    return ReturnVal;
}

/******************************************************************************
    Implementation of PropertyBag Interface
******************************************************************************/

/*****************************************************************************
    CreatePropertyBag:
    This function creates an instance of PropertyBag interface.  The use
    count for the interface is set to 1.
******************************************************************************/
LPPROPERTYBAG CreatePropertyBag(PTERWND w)
{
    LPSTRPROPERTYBAG pPropertyBag;
    LPPROPERTYBAG pProperty;

    // allocate and inititialize pPropertyBag
    if (NULL==(pPropertyBag=(LPSTRPROPERTYBAG )OurAlloc(sizeof(struct StrPropertyBag)))) return NULL;
    FarMemSet(pPropertyBag,0,sizeof(struct StrPropertyBag));

    pPropertyBag->count=0;                     // RegisterDragDrop will increment this counter
    pPropertyBag->w=w;                         // window pointer for the pSite

    // allocate and intialize the PROPERTYBAG virtual functions
    pProperty=(LPPROPERTYBAG)pPropertyBag;
    if (NULL==(pProperty->lpVtbl=OurAlloc(sizeof(struct IPropertyBagVtbl)))) return NULL;

    if ( NULL==(pProperty->lpVtbl->QueryInterface=(LPVOID) MakeProcInstance((FARPROC)IpbQueryInterface,hTerInst))
      || NULL==(pProperty->lpVtbl->AddRef=(LPVOID) MakeProcInstance((FARPROC)IpbAddRef,hTerInst))
      || NULL==(pProperty->lpVtbl->Release=(LPVOID) MakeProcInstance((FARPROC)IpbRelease,hTerInst))
      || NULL==(pProperty->lpVtbl->Read=(LPVOID) MakeProcInstance((FARPROC)IpbRead,hTerInst))
      || NULL==(pProperty->lpVtbl->Write=(LPVOID) MakeProcInstance((FARPROC)IpbWrite,hTerInst)) ) {
       PrintError(w,MSG_ERR_SITE_CREATE,"CreateOlePropertyBag");
       return NULL;
    }

    return pProperty;
}

/*****************************************************************************
    IpbQueryInterface: Query interface function for PropertyBag
******************************************************************************/
HRESULT STDMETHODCALLTYPE IpbQueryInterface(LPPROPERTYBAG this,REFIID riid,LPVOID far *obj)
{
    LPSTRPROPERTYBAG pPropertyBag=(LPSTRPROPERTYBAG)this;

    if (*obj) {
       LPUNKNOWN far *ppUnk=(LPUNKNOWN far *)obj;
       (*ppUnk)->lpVtbl->AddRef(*ppUnk);
       return ResultFromScode(S_OK);
    }
    else return ResultFromScode(E_NOINTERFACE);
}

/*****************************************************************************
    IpbAddRef: AddRef function for PropertyBag interface
******************************************************************************/
ULONG STDMETHODCALLTYPE IpbAddRef(LPPROPERTYBAG this)
{
    LPSTRPROPERTYBAG pProperty=(LPSTRPROPERTYBAG )this;

    pProperty->count++;
    return pProperty->count;
}
/*****************************************************************************
    IpbRelease: Release function for the PropertyBag interface
******************************************************************************/
ULONG STDMETHODCALLTYPE IpbRelease(LPPROPERTYBAG this)
{

    LPSTRPROPERTYBAG pPropertyBag=(LPSTRPROPERTYBAG )this;

    pPropertyBag->count--;

    if (pPropertyBag->count==0) {      // unallocate pPropertyBag interface
       OurFree(this->lpVtbl);
       OurFree(this);
       return 0;
    }

    return pPropertyBag->count;
}

/*****************************************************************************
    IpbRead: Read from property bag
******************************************************************************/
HRESULT STDMETHODCALLTYPE IpbRead(LPPROPERTYBAG this, LPCOLESTR PropName, VARIANT *pVar, LPERRORLOG *pErrorLog)
{
   LPSTRPROPERTYBAG pProperty=(LPSTRPROPERTYBAG)this;
   PTERWND w=pProperty->w;
   //HRESULT result;
   
   return ResultFromScode(S_OK);
}

/*****************************************************************************
    IpbWrite: Write to property bag
******************************************************************************/
HRESULT STDMETHODCALLTYPE IpbWrite(LPPROPERTYBAG this, LPCOLESTR PropName, VARIANT *pVar)
{
   LPSTRPROPERTYBAG pProperty=(LPSTRPROPERTYBAG)this;
   PTERWND w=pProperty->w;
   LPOLESTR pOleStr;
   int i;

   pProperty->PropCount++;

   if (pProperty->id<0)  return ResultFromScode(S_OK);  // just get the prop count
   
   // check if the request property found 
   if (pProperty->id>=0 && (pProperty->id!=pProperty->PropCount-1)) return ResultFromScode(S_OK);

   OleStrToStr((LPSTR)PropName,pProperty->name,99);   // max 99 characters

   if (V_VT(pVar)==VT_BOOL) {
       pProperty->type=OCX_BOOL;
       if (TerGetBool(pVar)) pProperty->val=1;
       else                  pProperty->val=0;
   }
   else if (V_VT(pVar)==VT_I4) {
       pProperty->type=OCX_LONG;
       pProperty->val=V_UNION(pVar,lVal);
   }
   else if (V_VT(pVar)==VT_BSTR) {
       pProperty->type=OCX_STRING;
       pOleStr=V_UNION(pVar,bstrVal);
       if (pOleStr!=NULL) {
          for (i=0;pOleStr[i]!=0;i++) ;
          pProperty->pText=MemAlloc(i+1);
          OleStrToStr((LPSTR)pOleStr,pProperty->pText,0);
       }
       else pProperty->pText=NULL;
   }
   else pProperty->type=OCX_NONE;

   return ResultFromScode(S_OK);
}

/*****************************************************************************
    Implementation of IOleControlSite Interface
******************************************************************************/

/*****************************************************************************
    CreateOleControlSite:
    This function creates an instance of IOleControlSite interface.  The use
    count for the interface is set to 1.
******************************************************************************/
LPOLECONTROLSITE CreateOleControlSite(PTERWND w, int obj,LPOLECLIENTSITE pObjSite)
{
    LPSTRCONTROLSITE pOleControlSite;
    LPOLECONTROLSITE pSite;

    // allocate and inititialize pControlSite
    if (NULL==(pOleControlSite=(LPSTRCONTROLSITE )OurAlloc(sizeof(struct StrOleControlSite)))) return NULL;
    pOleControlSite->count=1;                     // count for the interface
    pOleControlSite->w=w;                         // window pointer for the pSite
    pOleControlSite->obj=obj;                     // object id for the pSite
    pOleControlSite->pObjSite=pObjSite;           // client site for the object

    // allocate and intialize the CONTROLSITE virtual functions
    pSite=(LPOLECONTROLSITE)pOleControlSite;
    if (NULL==(pSite->lpVtbl=OurAlloc(sizeof(struct IOleControlSiteVtbl)))) return NULL;

    if ( NULL==(pSite->lpVtbl->QueryInterface=(LPVOID) MakeProcInstance((FARPROC)IcsQueryInterface,hTerInst))
      || NULL==(pSite->lpVtbl->AddRef=(LPVOID) MakeProcInstance((FARPROC)IcsAddRef,hTerInst))
      || NULL==(pSite->lpVtbl->Release=(LPVOID) MakeProcInstance((FARPROC)IcsRelease,hTerInst))
      || NULL==(pSite->lpVtbl->OnControlInfoChanged=(LPVOID) MakeProcInstance((FARPROC)IcsOnControlInfoChanged,hTerInst))
      || NULL==(pSite->lpVtbl->LockInPlaceActive=(LPVOID) MakeProcInstance((FARPROC)IcsLockInPlaceActive,hTerInst))
      || NULL==(pSite->lpVtbl->GetExtendedControl=(LPVOID) MakeProcInstance((FARPROC)IcsGetExtendedControl,hTerInst))
      || NULL==(pSite->lpVtbl->TransformCoords=(LPVOID) MakeProcInstance((FARPROC)IcsTransformCoords,hTerInst))
      || NULL==(pSite->lpVtbl->TranslateAccelerator=(LPVOID) MakeProcInstance((FARPROC)IcsTranslateAccelerator,hTerInst))
      || NULL==(pSite->lpVtbl->OnFocus=(LPVOID) MakeProcInstance((FARPROC)IcsOnFocus,hTerInst))
      || NULL==(pSite->lpVtbl->ShowPropertyFrame=(LPVOID) MakeProcInstance((FARPROC)IcsShowPropertyFrame,hTerInst)) ){
       PrintError(w,MSG_ERR_SITE_CREATE,"CreateOleControlSite");
       return NULL;
    }

    return pSite;
}

/*****************************************************************************
    IcsQueryInterface: Query interface function for IOleControlSite
******************************************************************************/
HRESULT STDMETHODCALLTYPE IcsQueryInterface(LPOLECONTROLSITE this,REFIID riid,LPVOID far *obj)
{
    LPSTRCONTROLSITE pOleControlSite=(LPSTRCONTROLSITE)this;
    LPOLECLIENTSITE pObjSite=pOleControlSite->pObjSite;

    return pObjSite->lpVtbl->QueryInterface(pObjSite,riid,obj);
}

/*****************************************************************************
    IcsAddRef: AddRef function for IOleControlSite interface
******************************************************************************/
ULONG STDMETHODCALLTYPE IcsAddRef(LPOLECONTROLSITE this)
{
    LPSTRCONTROLSITE pSite=(LPSTRCONTROLSITE )this;

    pSite->count++;
    return pSite->count;
}

/*****************************************************************************
    IcsRelease: Release function for the IOleControlSite interface
******************************************************************************/
ULONG STDMETHODCALLTYPE IcsRelease(LPOLECONTROLSITE this)
{
    int obj;
    PTERWND w;


    LPSTRCONTROLSITE pOleControlSite=(LPSTRCONTROLSITE)this;
    LPOLECLIENTSITE pObjSite=pOleControlSite->pObjSite;
    LPSTRCLIENTSITE  pSite=(LPSTRCLIENTSITE )pObjSite;

    w=pSite->w;
    obj=pSite->obj;

    pOleControlSite->count--;

    if (pOleControlSite->count==0) {      // unallocate pOleControlSite interface
       // reset the pointer in the IOleClientSite interface
       if (pSite) pSite->pOleControlSite=NULL;

       OurFree(this->lpVtbl);
       OurFree(this);

       return 0;
    }

    return pOleControlSite->count;
}

/*****************************************************************************
    IcsOnControlInfoChanged:
    Informs the container that the control's CONTROLINFO structure has changed and
    that the container should call the conrol's IOleControl::GetControlInfo for an update
******************************************************************************/
HRESULT STDMETHODCALLTYPE IcsOnControlInfoChanged(LPOLECONTROLSITE this)
{
    return ResultFromScode(S_OK);
}

HRESULT STDMETHODCALLTYPE IcsLockInPlaceActive(LPOLECONTROLSITE this,BOOL fLock)
{
    return ResultFromScode(S_OK);
}

HRESULT STDMETHODCALLTYPE IcsGetExtendedControl(LPOLECONTROLSITE this,IDispatch ** ppDisp)
{
    (*ppDisp)=NULL;
    return ResultFromScode(E_POINTER);   // pointer not set
}

HRESULT STDMETHODCALLTYPE IcsTransformCoords(LPOLECONTROLSITE this,POINTL *pPtHimet, POINTF *pPtContainer, DWORD flags)
{
    return ResultFromScode(S_OK);
}

HRESULT STDMETHODCALLTYPE IcsTranslateAccelerator(LPOLECONTROLSITE this,LPMSG pMsg, DWORD flags)
{
    return ResultFromScode(S_OK);
}

HRESULT STDMETHODCALLTYPE IcsOnFocus(LPOLECONTROLSITE this,BOOL GotFocus)
{
    return ResultFromScode(S_OK);
}

HRESULT STDMETHODCALLTYPE IcsShowPropertyFrame(LPOLECONTROLSITE this)
{
    return ResultFromScode(S_OK);
}


/******************************************************************************
    Implementation of DropTarget Interface
******************************************************************************/

/*****************************************************************************
    CreateDropTarget:
    This function creates an instance of DropTarget interface.  The use
    count for the interface is set to 1.
******************************************************************************/
LPDROPTARGET CreateDropTarget(PTERWND w)
{
    LPSTRDROPTARGET pDropTarget;
    LPDROPTARGET pDrop;

    // allocate and inititialize pDropTarget
    if (NULL==(pDropTarget=(LPSTRDROPTARGET )OurAlloc(sizeof(struct StrDropTarget)))) return NULL;
    pDropTarget->count=0;                     // RegisterDragDrop will increment this counter
    pDropTarget->w=w;                         // window pointer for the pSite

    // allocate and intialize the DROPTARGET virtual functions
    pDrop=(LPDROPTARGET)pDropTarget;
    if (NULL==(pDrop->lpVtbl=OurAlloc(sizeof(struct IDropTargetVtbl)))) return NULL;

    if ( NULL==(pDrop->lpVtbl->QueryInterface=(LPVOID) MakeProcInstance((FARPROC)IdtQueryInterface,hTerInst))
      || NULL==(pDrop->lpVtbl->AddRef=(LPVOID) MakeProcInstance((FARPROC)IdtAddRef,hTerInst))
      || NULL==(pDrop->lpVtbl->Release=(LPVOID) MakeProcInstance((FARPROC)IdtRelease,hTerInst))
      || NULL==(pDrop->lpVtbl->DragEnter=(LPVOID) MakeProcInstance((FARPROC)IdtDragEnter,hTerInst))
      || NULL==(pDrop->lpVtbl->DragOver=(LPVOID) MakeProcInstance((FARPROC)IdtDragOver,hTerInst))
      || NULL==(pDrop->lpVtbl->DragLeave=(LPVOID) MakeProcInstance((FARPROC)IdtDragLeave,hTerInst))
      || NULL==(pDrop->lpVtbl->Drop=(LPVOID) MakeProcInstance((FARPROC)IdtDrop,hTerInst)) ){
       PrintError(w,MSG_ERR_SITE_CREATE,"CreateOleDropTarget");
       return NULL;
    }

    return pDrop;
}

/*****************************************************************************
    IdtQueryInterface: Query interface function for DropTarget
******************************************************************************/
HRESULT STDMETHODCALLTYPE IdtQueryInterface(LPDROPTARGET this,REFIID riid,LPVOID far *obj)
{
    LPSTRDROPTARGET pDropTarget=(LPSTRDROPTARGET)this;

    if (*obj) {
       LPUNKNOWN far *ppUnk=(LPUNKNOWN far *)obj;
       (*ppUnk)->lpVtbl->AddRef(*ppUnk);
       return ResultFromScode(S_OK);
    }
    else return ResultFromScode(E_NOINTERFACE);
}

/*****************************************************************************
    IdtAddRef: AddRef function for DropTarget interface
******************************************************************************/
ULONG STDMETHODCALLTYPE IdtAddRef(LPDROPTARGET this)
{
    LPSTRDROPTARGET pDrop=(LPSTRDROPTARGET )this;

    pDrop->count++;
    return pDrop->count;
}
/*****************************************************************************
    IdtRelease: Release function for the DropTarget interface
******************************************************************************/
ULONG STDMETHODCALLTYPE IdtRelease(LPDROPTARGET this)
{

    LPSTRDROPTARGET pDropTarget=(LPSTRDROPTARGET )this;

    pDropTarget->count--;

    if (pDropTarget->count==0) {      // unallocate pDropTarget interface
       OurFree(this->lpVtbl);
       OurFree(this);
       return 0;
    }

    return pDropTarget->count;
}

/*****************************************************************************
    IdtDrop: The object being dropped
******************************************************************************/
HRESULT STDMETHODCALLTYPE IdtDrop(LPDROPTARGET this, LPDATAOBJECT pDataObject, DWORD KeyState, POINTL ScrPt, DWORD *pEffect)
{
   FORMATETC OleFmt;
   LPSTRDROPTARGET pDrop=(LPSTRDROPTARGET)this;
   PTERWND w=pDrop->w;
   UINT ClipFormat=0;
   HRESULT result;
   DWORD lParam;
   POINT pt;
   STGMEDIUM medium;
   LPBYTE pMem;
   long  HilightBeg,HilightEnd,MousePos,CurPos,NewPos,SaveHilightType;
   BOOL  FixHilight=false;
   DWORD allowed;

   if (TerArg.ReadOnly) goto END;


   FarMemSet(&medium,0,sizeof(medium));

   // Get the insertion point
   pt.x=(int)ScrPt.x;
   pt.y=(int)ScrPt.y;
   ScreenToClient(hTerWnd,&pt);
   
   lParam=pt.y;
   lParam=lParam<<16;             // high word
   lParam=lParam+pt.x;
   TerMousePos(w,lParam,FALSE);

   // check if the cursor is on the protected text
   if (!CanInsert(w,MouseLine,MouseCol)) {
      MessageBeep(0);
      HilightType=HILIGHT_OFF;       // prevent selection deletion by the OleDragText function
      goto END;
   }

   // if doing ole-drag from the same TE window - check if mouse is on a highlighted text
   if (InOleDrag && HilightType!=HILIGHT_OFF) {   // rtf text being dropped to the same window
      NormalizeBlock(w);

      HilightBeg=RowColToAbs(w,HilightBegRow,HilightBegCol);
      HilightEnd=RowColToAbs(w,HilightEndRow,HilightEndCol);
      MousePos=RowColToAbs(w,MouseLine,MouseCol);
      if (HilightBeg>HilightEnd) SwapLongs(&HilightBeg,&HilightEnd);

      if (MousePos>=HilightBeg && MousePos<HilightEnd) {  // nowhere to drag
         HilightType=HILIGHT_OFF;       // prevent selection deletion by the OleDragText function
         goto END;
      }
   } 
   
   // position the cursor at the new location and insert the buffer
   CurLine=MouseLine;
   CurCol=MouseCol;
   CurPos=RowColToAbs(w,CurLine,CurCol);
   FixHilight=(InOleDrag && HilightType!=HILIGHT_OFF && CurPos<HilightBeg); // hilighting needs to be adjust because text being inserted before the hilight begin pos

   // Check available formats
   OleFmt.ptd=NULL;
   OleFmt.dwAspect=DVASPECT_CONTENT;
   OleFmt.lindex=-1;
   OleFmt.tymed=TYMED_HGLOBAL;

   OleFmt.cfFormat=RtfClipFormat;
   result=pDataObject->lpVtbl->QueryGetData(pDataObject,&OleFmt);
   if (GetScode(result)==S_OK) {
      result=pDataObject->lpVtbl->GetData(pDataObject,&OleFmt,&medium);
      if (GetScode(result)!=S_OK) goto END;
      if (medium.tymed==TYMED_HGLOBAL) {
         SendMessageToParent(w,TER_BEFORE_DROP,(WPARAM)hTerWnd,0L,FALSE);
         
         pMem=GlobalLock(medium.u.hGlobal);
         if (!(TerFlags3&TFLAG3_PASTE_RTF_CODE)) {
            SaveHilightType=HilightType;
            HilightType=HILIGHT_OFF;
            InsertRtfBuf(hTerWnd,pMem,GlobalSize(medium.u.hGlobal),CurLine,CurCol,TRUE);
            HilightType=SaveHilightType;

            if (FixHilight) { // text inserted before the selection, move the selection range forward
               NewPos=RowColToAbs(w,CurLine,CurCol);
               HilightBeg+=(NewPos-CurPos);
               HilightEnd+=(NewPos-CurPos);

               AbsToRowCol(w,HilightBeg,&HilightBegRow,&HilightBegCol);
               AbsToRowCol(w,HilightEnd,&HilightEndRow,&HilightEndCol);
            } 
         }
         else {
            InputFontId=GetEffectiveCfmt(w);

            InsertBuffer(w,pMem,NULL,NULL,NULL,NULL,FALSE);      // insert the clipboard data at current position
         }
         SendMessageToParent(w,TER_DROP,(WPARAM)hTerWnd,0L,FALSE);
         GlobalUnlock(medium.u.hGlobal);
         goto END; 
      }
   } 

   OleFmt.cfFormat=CF_TEXT;
   result=pDataObject->lpVtbl->QueryGetData(pDataObject,&OleFmt);
   if (GetScode(result)==S_OK) {
      result=pDataObject->lpVtbl->GetData(pDataObject,&OleFmt,&medium);
      if (GetScode(result)!=S_OK) goto END;
      if (medium.tymed==TYMED_HGLOBAL) {
         SendMessageToParent(w,TER_BEFORE_DROP,(WPARAM)hTerWnd,0L,FALSE);
         
         pMem=GlobalLock(medium.u.hGlobal);
         InputFontId=GetEffectiveCfmt(w);
         InsertBuffer(w,pMem,NULL,NULL,NULL,NULL,FALSE);      // insert the clipboard data at current position
         GlobalUnlock(medium.u.hGlobal);
         PaintTer(w);
         SendMessageToParent(w,TER_DROP,(WPARAM)hTerWnd,0L,FALSE);
         goto END;
      }
   } 
 
   OleFmt.tymed=TYMED_MFPICT;
   OleFmt.cfFormat=CF_METAFILEPICT;
   result=pDataObject->lpVtbl->QueryGetData(pDataObject,&OleFmt);
   if (GetScode(result)==S_OK) {
      result=pDataObject->lpVtbl->GetData(pDataObject,&OleFmt,&medium);
      if (GetScode(result)!=S_OK) goto END;
      if (medium.tymed==TYMED_MFPICT) {
         SendMessageToParent(w,TER_BEFORE_DROP,(WPARAM)hTerWnd,0L,FALSE);
         TerPastePicture(hTerWnd,OleFmt.cfFormat,medium.u.hMetaFilePict,0,0,TRUE);
         SendMessageToParent(w,TER_DROP,(WPARAM)hTerWnd,0L,FALSE);
         goto END; 
      }
   } 
 

   OleFmt.cfFormat=CfEnhMetafile;
   result=pDataObject->lpVtbl->QueryGetData(pDataObject,&OleFmt);
   if (GetScode(result)==S_OK) {
      if (medium.tymed==TYMED_ENHMF) {
         SendMessageToParent(w,TER_BEFORE_DROP,(WPARAM)hTerWnd,0L,FALSE);
         TerPastePicture(hTerWnd,OleFmt.cfFormat,medium.u.hEnhMetaFile,0,0,TRUE);
         SendMessageToParent(w,TER_DROP,(WPARAM)hTerWnd,0L,FALSE);
         goto END; 
      }
   } 

   OleFmt.tymed=TYMED_HGLOBAL;
   OleFmt.cfFormat=CF_HDROP;
   result=pDataObject->lpVtbl->QueryGetData(pDataObject,&OleFmt);
   if (GetScode(result)==S_OK) {
      result=pDataObject->lpVtbl->GetData(pDataObject,&OleFmt,&medium);
      if (GetScode(result)!=S_OK) goto END;
      if (medium.tymed==TYMED_HGLOBAL) {
         SendMessageToParent(w,TER_BEFORE_DROP,(WPARAM)hTerWnd,0L,FALSE);
         
         TerInsertDragObject(w,medium.u.hGlobal,true);
         SendMessageToParent(w,TER_DROP,(WPARAM)hTerWnd,0L,FALSE);
         goto END;
      }
   } 
 
   END:
   if (medium.pUnkForRelease) medium.pUnkForRelease->lpVtbl->Release(medium.pUnkForRelease);  // release the medimum

   //pDataObject->lpVtbl->Release(pDataObject);

    //set the effect flag to tell the source to delete or retain the source data
   allowed=(*pEffect);
   ResetLongFlag((*pEffect),(DROPEFFECT_COPY|DROPEFFECT_MOVE|DROPEFFECT_LINK));
   
   if (True(KeyState&MK_CONTROL)) {   
      if (True(allowed&DROPEFFECT_COPY))    (*pEffect)|=DROPEFFECT_COPY;
   }
   else {
      if (True(allowed&DROPEFFECT_MOVE))      (*pEffect)|=DROPEFFECT_MOVE;    // move implies deletion of the original text, see if it is allowed
      else if (True(allowed&DROPEFFECT_COPY)) (*pEffect)|=DROPEFFECT_COPY;    // if can't move, check for copy
   }
   if (True(allowed&DROPEFFECT_LINK))    (*pEffect)|=DROPEFFECT_LINK;
      

   InDragDrop=FALSE;

   InitCaret(w);              // to erase the caret again

   return ResultFromScode(S_OK);
}


/*****************************************************************************
    IdtDropEnter: The drag cursor enters the window
******************************************************************************/
HRESULT STDMETHODCALLTYPE IdtDragEnter(LPDROPTARGET this, LPDATAOBJECT pDataObject, DWORD KeyState, POINTL pt, DWORD *pEffect)
{
   LPSTRDROPTARGET pDrop=(LPSTRDROPTARGET)this;
   PTERWND w=pDrop->w;
   
   if (TerArg.ReadOnly) goto END;

   InDragDrop=TRUE;
   if (!CaretEnabled && UseCaret(w)) InitCaret(w);
   SendMessageToParent(w,TER_DRAG,(WPARAM)hTerWnd,0L,FALSE);

   END:
   return ResultFromScode(S_OK);
}


/*****************************************************************************
    IdtDragOver: The object being dropped
******************************************************************************/
HRESULT STDMETHODCALLTYPE IdtDragOver(LPDROPTARGET this, DWORD KeyState, POINTL ScrPt, DWORD *pEffect)
{
   LPSTRDROPTARGET pDrop=(LPSTRDROPTARGET)this;
   PTERWND w=pDrop->w;
   POINT pt;
   DWORD lParam,allowed;
   
   if (TerArg.ReadOnly) goto END;

   // Get the insertion point
   pt.x=(int)ScrPt.x;
   pt.y=(int)ScrPt.y;
   ScreenToClient(hTerWnd,&pt);
   
   lParam=pt.y;
   lParam=lParam<<16;             // high word
   lParam=lParam+pt.x;
   
   InDragDrop=TRUE;
   SetDragCaret(w,lParam);

   // check if text need to scroll
   if      (pt.y>(TerWinRect.bottom-20)) TerWinDown(w);
   else if (pt.y>(TerWinRect.bottom-10)) TerPageDn(w,false);
   else if (pt.y<(TerWinRect.top+20))    TerWinUp(w);
   else if (pt.y<(TerWinRect.top+10))    TerPageUp(w,false);

   END:
   allowed=(*pEffect);
   ResetLongFlag((*pEffect),(DROPEFFECT_COPY|DROPEFFECT_MOVE|DROPEFFECT_LINK));
   
   if (True(KeyState&MK_CONTROL)) {   
      if (True(allowed&DROPEFFECT_COPY))    (*pEffect)|=DROPEFFECT_COPY;
   }
   else {
      if (True(allowed&DROPEFFECT_MOVE))      (*pEffect)|=DROPEFFECT_MOVE;    // move implies deletion of the original text, see if it is allowed
      else if (True(allowed&DROPEFFECT_COPY)) (*pEffect)|=DROPEFFECT_COPY;    // if can't move, check for copy
   }

   if (True(allowed&DROPEFFECT_LINK))    (*pEffect)|=DROPEFFECT_LINK;

   if (TerArg.ReadOnly) (*pEffect)=0;

   return ResultFromScode(S_OK);
}

/*****************************************************************************
    IdtDragLeave: The object being dropped
******************************************************************************/
HRESULT STDMETHODCALLTYPE IdtDragLeave(LPDROPTARGET this)
{
   LPSTRDROPTARGET pDrop=(LPSTRDROPTARGET)this;
   PTERWND w=pDrop->w;
   
   InDragDrop=FALSE;

   return ResultFromScode(S_OK);
}



/*****************************************************************************
    Implementation of IOleInPlaceSite Interface
******************************************************************************/

/*****************************************************************************
    CreateInPlaceSite:
    This function creates an instance of IOleInPlaceSite interface.  The use
    count for the interface is set to 1.
******************************************************************************/
LPOLEINPLACESITE CreateInPlaceSite(PTERWND w, int obj,LPOLECLIENTSITE pObjSite)
{
    LPSTRINPLACESITE pInPlaceSite;
    LPOLEINPLACESITE pSite;

    // allocate and inititialize pInPlaceSite
    if (NULL==(pInPlaceSite=(LPSTRINPLACESITE )OurAlloc(sizeof(struct StrInPlaceSite)))) return NULL;
    pInPlaceSite->count=1;                     // count for the interface
    pInPlaceSite->w=w;                         // window pointer for the pSite
    pInPlaceSite->obj=obj;                     // object id for the pSite
    pInPlaceSite->pObjSite=pObjSite;           // client site for the object

    // allocate and intialize the INPLACESITE virtual functions
    pSite=(LPOLEINPLACESITE)pInPlaceSite;
    if (NULL==(pSite->lpVtbl=OurAlloc(sizeof(struct IOleInPlaceSiteVtbl)))) return NULL;

    if ( NULL==(pSite->lpVtbl->QueryInterface=(LPVOID) MakeProcInstance((FARPROC)IipsQueryInterface,hTerInst))
      || NULL==(pSite->lpVtbl->AddRef=(LPVOID) MakeProcInstance((FARPROC)IipsAddRef,hTerInst))
      || NULL==(pSite->lpVtbl->Release=(LPVOID) MakeProcInstance((FARPROC)IipsRelease,hTerInst))
      || NULL==(pSite->lpVtbl->GetWindow=(LPVOID) MakeProcInstance((FARPROC)IipsGetWindow,hTerInst))
      || NULL==(pSite->lpVtbl->ContextSensitiveHelp=(LPVOID) MakeProcInstance((FARPROC)IipsContextSensitiveHelp,hTerInst))
      || NULL==(pSite->lpVtbl->CanInPlaceActivate=(LPVOID) MakeProcInstance((FARPROC)IipsCanInPlaceActivate,hTerInst))
      || NULL==(pSite->lpVtbl->OnInPlaceActivate=(LPVOID) MakeProcInstance((FARPROC)IipsOnInPlaceActivate,hTerInst))
      || NULL==(pSite->lpVtbl->OnUIActivate=(LPVOID) MakeProcInstance((FARPROC)IipsOnUIActivate,hTerInst))
      || NULL==(pSite->lpVtbl->GetWindowContext=(LPVOID) MakeProcInstance((FARPROC)IipsGetWindowContext,hTerInst))
      || NULL==(pSite->lpVtbl->Scroll=(LPVOID) MakeProcInstance((FARPROC)IipsScroll,hTerInst))
      || NULL==(pSite->lpVtbl->OnUIDeactivate=(LPVOID) MakeProcInstance((FARPROC)IipsOnUIDeactivate,hTerInst))
      || NULL==(pSite->lpVtbl->OnInPlaceDeactivate=(LPVOID) MakeProcInstance((FARPROC)IipsOnInPlaceDeactivate,hTerInst))
      || NULL==(pSite->lpVtbl->DiscardUndoState=(LPVOID) MakeProcInstance((FARPROC)IipsDiscardUndoState,hTerInst))
      || NULL==(pSite->lpVtbl->DeactivateAndUndo=(LPVOID) MakeProcInstance((FARPROC)IipsDeactivateAndUndo,hTerInst))
      || NULL==(pSite->lpVtbl->OnPosRectChange=(LPVOID) MakeProcInstance((FARPROC)IipsOnPosRectChange,hTerInst)) ){
       PrintError(w,MSG_ERR_SITE_CREATE,"CreateOleInPlaceSite");
       return NULL;
    }

    return pSite;
}

/*****************************************************************************
    IipsQueryInterface: Query interface function for IOleInPlaceSite
******************************************************************************/
HRESULT STDMETHODCALLTYPE IipsQueryInterface(LPOLEINPLACESITE this,REFIID riid,LPVOID far *obj)
{
    LPSTRINPLACESITE pInPlaceSite=(LPSTRINPLACESITE)this;
    LPOLECLIENTSITE pObjSite=pInPlaceSite->pObjSite;

    return pObjSite->lpVtbl->QueryInterface(pObjSite,riid,obj);
}

/*****************************************************************************
    IipsAddRef: AddRef function for IOleInPlaceSite interface
******************************************************************************/
ULONG STDMETHODCALLTYPE IipsAddRef(LPOLEINPLACESITE this)
{
    LPSTRINPLACESITE pSite=(LPSTRINPLACESITE )this;

    pSite->count++;
    return pSite->count;
}
/*****************************************************************************
    IipsRelease: Release function for the IOleInPlaceSite interface
******************************************************************************/
ULONG STDMETHODCALLTYPE IipsRelease(LPOLEINPLACESITE this)
{
    int obj;
    PTERWND w;


    LPSTRINPLACESITE pInPlaceSite=(LPSTRINPLACESITE )this;
    LPOLECLIENTSITE  pObjSite=pInPlaceSite->pObjSite;
    LPSTRCLIENTSITE  pSite=(LPSTRCLIENTSITE )pObjSite;


    w=pInPlaceSite->w;
    obj=pInPlaceSite->obj;

    pInPlaceSite->count--;

    if (pInPlaceSite->count==0) {      // unallocate pInPlaceSite interface
       // reset the pointer in the IOleClientSite interface
       if (pSite) pSite->pInPlaceSite=NULL;

       OurFree(this->lpVtbl);
       OurFree(this);

       return 0;
    }

    return pInPlaceSite->count;
}

/*****************************************************************************
    IipsGetWindow:
******************************************************************************/
HRESULT STDMETHODCALLTYPE IipsGetWindow(LPOLEINPLACESITE this,HWND far *lphWnd)
{
    PTERWND w;

    LPSTRINPLACESITE pSite=(LPSTRINPLACESITE )this;

    w=pSite->w;
    if (lphWnd) (*lphWnd)=hTerWnd;
    return ResultFromScode(S_OK);
}

HRESULT STDMETHODCALLTYPE IipsContextSensitiveHelp(LPOLEINPLACESITE this,BOOL enter)
{
    return ResultFromScode(S_OK);
}

HRESULT STDMETHODCALLTYPE IipsCanInPlaceActivate(LPOLEINPLACESITE this)
{
    return ResultFromScode(S_OK);
}

HRESULT STDMETHODCALLTYPE IipsOnInPlaceActivate(LPOLEINPLACESITE this)
{
    return ResultFromScode(S_OK);
}

HRESULT STDMETHODCALLTYPE IipsOnUIActivate(LPOLEINPLACESITE this)
{
    return ResultFromScode(S_OK);
}

HRESULT STDMETHODCALLTYPE IipsGetWindowContext(LPOLEINPLACESITE this,
    LPOLEINPLACEFRAME far *lplpFrame, LPOLEINPLACEUIWINDOW FAR *lplpDoc,
    LPRECT pPosRect, LPRECT pClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
    int obj;
    PTERWND w;


    LPSTRINPLACESITE pSite=(LPSTRINPLACESITE )this;
    LPSTRCLIENTSITE  pClientSite=(LPSTRCLIENTSITE)(pSite->pObjSite);
    LPSTRINPLACEFRAME pInPlaceFrame=(LPSTRINPLACEFRAME)(pClientSite->pInPlaceFrame);

    w=pSite->w;
    obj=pSite->obj;

    (*lplpFrame)=pClientSite->pInPlaceFrame;
    pInPlaceFrame->count++;

    (*lplpDoc)=NULL;

    pPosRect->left=TerFont[obj].PictX+TerWinRect.left;
    pPosRect->top=TerFont[obj].PictY+TerWinRect.top;
    pPosRect->right=pPosRect->left+TerFont[obj].CharWidth[PICT_CHAR];
    pPosRect->bottom=pPosRect->top+TerFont[obj].height;

    (*pClipRect)=TerWinRect;

    // fill in the frame info structure
    lpFrameInfo->hwndFrame=hTerWnd;
    lpFrameInfo->haccel=hTerAccTable;
    lpFrameInfo->cAccelEntries=0;

    return ResultFromScode(S_OK);
}

HRESULT STDMETHODCALLTYPE IipsScroll(LPOLEINPLACESITE this, SIZE extent)
{
    return ResultFromScode(S_OK);
}

HRESULT STDMETHODCALLTYPE IipsOnUIDeactivate(LPOLEINPLACESITE this, BOOL undoable)
{
    return ResultFromScode(S_OK);
}

HRESULT STDMETHODCALLTYPE IipsOnInPlaceDeactivate(LPOLEINPLACESITE this)
{
    PTERWND w;

    LPSTRINPLACESITE pSite=(LPSTRINPLACESITE )this;
    LPSTRCLIENTSITE  pClientSite=(LPSTRCLIENTSITE)(pSite->pObjSite);

    w=pSite->w;

    pClientSite->pActiveObject=NULL;

    return ResultFromScode(S_OK);
}

HRESULT STDMETHODCALLTYPE IipsDiscardUndoState(LPOLEINPLACESITE this)
{
    return ResultFromScode(S_OK);
}

HRESULT STDMETHODCALLTYPE IipsDeactivateAndUndo(LPOLEINPLACESITE this)
{
    return ResultFromScode(S_OK);
}

HRESULT STDMETHODCALLTYPE IipsOnPosRectChange(LPOLEINPLACESITE this, LPCRECT lprcPosRect)
{
    return ResultFromScode(S_OK);
}

/******************************************************************************
    Implementation of IOleInPlaceFrame Interface
******************************************************************************/

/*****************************************************************************
    CreateInPlaceFrame:
    This function creates an instance of IOleInPlaceFrame interface.  The use
    count for the interface is set to 1.
******************************************************************************/
LPOLEINPLACEFRAME CreateInPlaceFrame(PTERWND w, int obj,LPOLECLIENTSITE pObjSite)
{
    LPSTRINPLACEFRAME pInPlaceFrame;
    LPOLEINPLACEFRAME pFrame;

    // allocate and inititialize pInPlaceFrame
    if (NULL==(pInPlaceFrame=(LPSTRINPLACEFRAME )OurAlloc(sizeof(struct StrInPlaceFrame)))) return NULL;
    pInPlaceFrame->count=1;                     // count for the interface
    pInPlaceFrame->w=w;                         // window pointer for the pSite
    pInPlaceFrame->obj=obj;                     // object id for the pSite
    pInPlaceFrame->pObjSite=pObjSite;           // client site for the object

    // allocate and intialize the INPLACEFRAME virtual functions
    pFrame=(LPOLEINPLACEFRAME)pInPlaceFrame;
    if (NULL==(pFrame->lpVtbl=OurAlloc(sizeof(struct IOleInPlaceFrameVtbl)))) return NULL;

    if ( NULL==(pFrame->lpVtbl->QueryInterface=(LPVOID) MakeProcInstance((FARPROC)IipfQueryInterface,hTerInst))
      || NULL==(pFrame->lpVtbl->AddRef=(LPVOID) MakeProcInstance((FARPROC)IipfAddRef,hTerInst))
      || NULL==(pFrame->lpVtbl->Release=(LPVOID) MakeProcInstance((FARPROC)IipfRelease,hTerInst))
      || NULL==(pFrame->lpVtbl->GetWindow=(LPVOID) MakeProcInstance((FARPROC)IipfGetWindow,hTerInst))
      || NULL==(pFrame->lpVtbl->ContextSensitiveHelp=(LPVOID) MakeProcInstance((FARPROC)IipfContextSensitiveHelp,hTerInst))
      || NULL==(pFrame->lpVtbl->GetBorder=(LPVOID) MakeProcInstance((FARPROC)IipfGetBorder,hTerInst))
      || NULL==(pFrame->lpVtbl->RequestBorderSpace=(LPVOID) MakeProcInstance((FARPROC)IipfRequestBorderSpace,hTerInst))
      || NULL==(pFrame->lpVtbl->SetBorderSpace=(LPVOID) MakeProcInstance((FARPROC)IipfSetBorderSpace,hTerInst))
      || NULL==(pFrame->lpVtbl->SetActiveObject=(LPVOID) MakeProcInstance((FARPROC)IipfSetActiveObject,hTerInst))
      || NULL==(pFrame->lpVtbl->InsertMenus=(LPVOID) MakeProcInstance((FARPROC)IipfInsertMenus,hTerInst))
      || NULL==(pFrame->lpVtbl->SetMenu=(LPVOID) MakeProcInstance((FARPROC)IipfSetMenu,hTerInst))
      || NULL==(pFrame->lpVtbl->RemoveMenus=(LPVOID) MakeProcInstance((FARPROC)IipfRemoveMenus,hTerInst))
      || NULL==(pFrame->lpVtbl->SetStatusText=(LPVOID) MakeProcInstance((FARPROC)IipfSetStatusText,hTerInst))
      || NULL==(pFrame->lpVtbl->EnableModeless=(LPVOID) MakeProcInstance((FARPROC)IipfEnableModeless,hTerInst))
      || NULL==(pFrame->lpVtbl->TranslateAccelerator=(LPVOID) MakeProcInstance((FARPROC)IipfTranslateAccelerator,hTerInst)) ){
       PrintError(w,MSG_ERR_SITE_CREATE,"CreateOleInPlaceFrame");
       return NULL;
    }

    return pFrame;
}

/*****************************************************************************
    IipfQueryInterface: Query interface function for IOleInPlaceFrame
******************************************************************************/
HRESULT STDMETHODCALLTYPE IipfQueryInterface(LPOLEINPLACEFRAME this,REFIID riid,LPVOID far *obj)
{
    LPSTRINPLACEFRAME pInPlaceFrame=(LPSTRINPLACEFRAME)this;
    LPOLECLIENTSITE pObjSite=pInPlaceFrame->pObjSite;

    return pObjSite->lpVtbl->QueryInterface(pObjSite,riid,obj);
}

/*****************************************************************************
    IipfAddRef: AddRef function for IOleInPlaceFrame interface
******************************************************************************/
ULONG STDMETHODCALLTYPE IipfAddRef(LPOLEINPLACEFRAME this)
{
    LPSTRINPLACEFRAME pFrame=(LPSTRINPLACEFRAME )this;

    pFrame->count++;
    return pFrame->count;
}
/*****************************************************************************
    IipfRelease: Release function for the IOleInPlaceFrame interface
******************************************************************************/
ULONG STDMETHODCALLTYPE IipfRelease(LPOLEINPLACEFRAME this)
{
    int obj;
    PTERWND w;


    LPSTRINPLACEFRAME pInPlaceFrame=(LPSTRINPLACEFRAME )this;
    LPOLECLIENTSITE  pObjSite=pInPlaceFrame->pObjSite;
    LPSTRCLIENTSITE  pSite=(LPSTRCLIENTSITE )pObjSite;

    w=pInPlaceFrame->w;
    obj=pInPlaceFrame->obj;

    pInPlaceFrame->count--;

    if (pInPlaceFrame->count==0) {      // unallocate pInPlaceFrame interface

       // reset the pointer in the IOleClientSite interface
       if (pSite) pSite->pInPlaceFrame=NULL;

       OurFree(this->lpVtbl);
       OurFree(this);

       return 0;
    }

    return pInPlaceFrame->count;
}

/*****************************************************************************
    IipfGetWindow:
******************************************************************************/
HRESULT STDMETHODCALLTYPE IipfGetWindow(LPOLEINPLACEFRAME this,HWND far *lphWnd)
{
    PTERWND w;

    LPSTRINPLACEFRAME pSite=(LPSTRINPLACEFRAME )this;

    w=pSite->w;
    if (*lphWnd) (*lphWnd)=hTerWnd;
    return ResultFromScode(S_OK);
}

HRESULT STDMETHODCALLTYPE IipfContextSensitiveHelp(LPOLEINPLACEFRAME this,BOOL enter)
{
    return ResultFromScode(S_OK);
}

HRESULT STDMETHODCALLTYPE IipfGetBorder(LPOLEINPLACEFRAME this,LPRECT rect)
{
    return ResultFromScode(INPLACE_E_NOTOOLSPACE); // toolbar space not allowed
}

HRESULT STDMETHODCALLTYPE IipfRequestBorderSpace(LPOLEINPLACEFRAME this,LPCBORDERWIDTHS widths)
{
    return ResultFromScode(INPLACE_E_NOTOOLSPACE); // toolbar space not allowed
}

HRESULT STDMETHODCALLTYPE IipfSetBorderSpace(LPOLEINPLACEFRAME this,LPCBORDERWIDTHS widths)
{
    return ResultFromScode(S_OK);
}

HRESULT STDMETHODCALLTYPE IipfSetActiveObject(LPOLEINPLACEFRAME this,LPOLEINPLACEACTIVEOBJECT pActiveObject, LPCSTR ObjName)
{
    LPSTRINPLACEFRAME pInPlaceFrame=(LPSTRINPLACEFRAME )this;
    LPOLECLIENTSITE  pObjSite=pInPlaceFrame->pObjSite;
    LPSTRCLIENTSITE  pSite=(LPSTRCLIENTSITE )pObjSite;
    pSite->pActiveObject=pActiveObject;      // record the active object

    return ResultFromScode(S_OK);
}

HRESULT STDMETHODCALLTYPE IipfInsertMenus(LPOLEINPLACEFRAME this,HMENU hMenu, LPOLEMENUGROUPWIDTHS widths)
{
    return ResultFromScode(S_OK);
}

HRESULT STDMETHODCALLTYPE IipfSetMenu(LPOLEINPLACEFRAME this,HMENU hMenu, HOLEMENU hOleMenu, HWND hWnd)
{
    return ResultFromScode(S_OK);
}

HRESULT STDMETHODCALLTYPE IipfRemoveMenus(LPOLEINPLACEFRAME this,HMENU hMenu)
{
    return ResultFromScode(S_OK);
}

HRESULT STDMETHODCALLTYPE IipfSetStatusText(LPOLEINPLACEFRAME this,LPCSTR StatusText)
{
    return ResultFromScode(S_OK);
}

HRESULT STDMETHODCALLTYPE IipfEnableModeless(LPOLEINPLACEFRAME this,BOOL enable)
{
    return ResultFromScode(S_OK);
}

HRESULT STDMETHODCALLTYPE IipfTranslateAccelerator(LPOLEINPLACEFRAME this,LPMSG pMsg, WORD wID)
{
    return ResultFromScode(S_FALSE);   // keystroke not used by TER
}


/*****************************************************************************
    Implementation of IDispatch Interface
******************************************************************************/

/*****************************************************************************
    CreateDispatch:
    This function creates an instance of IDispatch interface.  The use
    count for the interface is set to 1.
******************************************************************************/
LPDISPATCH CreateDispatch(PTERWND w, int obj, LPOLECLIENTSITE pObjSite)
{
    LPSTRDISPATCH pTerDispatch;
    LPDISPATCH pDispatch;

    // allocate and inititialize pTerSite
    if (NULL==(pTerDispatch=(LPSTRDISPATCH )OurAlloc(sizeof(struct StrDispatch)))) return NULL;
    pTerDispatch->count=1;                     // count for the interface
    pTerDispatch->w=w;                         // window pointer for the pDispatch
    pTerDispatch->obj=obj;                     // object id for the pDispatch
    pTerDispatch->pObjSite=pObjSite;           // client site for the object

    // allocate and intialize the CLIENTSITE virtual functions
    pDispatch=(LPDISPATCH)pTerDispatch;
    if (NULL==(pDispatch->lpVtbl=OurAlloc(sizeof(struct IDispatchVtbl)))) return NULL;
    if ( NULL==(pDispatch->lpVtbl->QueryInterface=(LPVOID) MakeProcInstance((FARPROC)IdQueryInterface,hTerInst))
      || NULL==(pDispatch->lpVtbl->AddRef=(LPVOID) MakeProcInstance((FARPROC)IdAddRef,hTerInst))
      || NULL==(pDispatch->lpVtbl->Release=(LPVOID) MakeProcInstance((FARPROC)IdRelease,hTerInst))
      || NULL==(pDispatch->lpVtbl->Invoke=(LPVOID) MakeProcInstance((FARPROC)IdInvoke,hTerInst))
      || NULL==(pDispatch->lpVtbl->GetIDsOfNames=(LPVOID) MakeProcInstance((FARPROC)IdGetIDsOfNames,hTerInst))
      || NULL==(pDispatch->lpVtbl->GetTypeInfo=(LPVOID) MakeProcInstance((FARPROC)IdGetTypeInfo,hTerInst))
      || NULL==(pDispatch->lpVtbl->GetTypeInfoCount=(LPVOID) MakeProcInstance((FARPROC)IdGetTypeInfoCount,hTerInst)) ){
       PrintError(w,MSG_ERR_ADV_CREATE,NULL);
       return NULL;
    }

    return pDispatch;
}

/*****************************************************************************
    IdQueryInterface: Query interface function for IDispatch.
    This function simply delegates to the IOleClientSite::QueryInterface
******************************************************************************/
HRESULT STDMETHODCALLTYPE IdQueryInterface(LPDISPATCH this,REFIID riid,LPVOID far *obj)
{
    LPSTRDISPATCH pDispatch=(LPSTRDISPATCH )this;
    LPOLECLIENTSITE pObjSite=pDispatch->pObjSite;

    return pObjSite->lpVtbl->QueryInterface(pObjSite,riid,obj);
}

/*****************************************************************************
    IdAddRef: AddRef function for IDispatch interface
******************************************************************************/
ULONG STDMETHODCALLTYPE IdAddRef(LPDISPATCH this)
{
    LPSTRDISPATCH pDispatch=(LPSTRDISPATCH )this;

    pDispatch->count++;
    return pDispatch->count;
}

/*****************************************************************************
    IdRelease: Release function for the IDispatch interface
******************************************************************************/
ULONG STDMETHODCALLTYPE IdRelease(LPDISPATCH this)
{
    LPSTRDISPATCH pDispatch=(LPSTRDISPATCH )this;
    LPOLECLIENTSITE pObjSite=pDispatch->pObjSite;
    LPSTRCLIENTSITE pSite=(LPSTRCLIENTSITE )pObjSite;

    pDispatch->count--;

    if (pDispatch->count==0) {      // unallocate pDispatch interface
       OurFree(this->lpVtbl);
       OurFree(this);

       // reset the pointer in the IOleClientSite interface
       if (pSite) pSite->pDispatch=NULL;

       return 0;
    }

    return pDispatch->count;
}

HRESULT STDMETHODCALLTYPE IdInvoke(LPDISPATCH this, DISPID dispid, REFIID riid, LCID lcid,
                                   WORD flags, DISPPARAMS FAR* pDispParams,
                                   VARIANT FAR* pVarResult, EXCEPINFO FAR * pExcepInfo,
                                   unsigned int FAR* puArgErr)
{
   VARIANT VarResult;
   BOOL DesignMode=FALSE;
   PTERWND w;
   LPSTRDISPATCH pDispatch=(LPSTRDISPATCH)this;

   w=pDispatch->w;

   if (!(flags&DISPATCH_PROPERTYGET)) return ResultFromScode(DISP_E_MEMBERNOTFOUND);

   if (pVarResult==NULL) pVarResult=&VarResult;

   V_VT(pVarResult)=VT_BOOL;      // initialize the result type

   if (dispid==DISPID_AMBIENT_BACKCOLOR) {
      V_VT(pVarResult)=VT_I4;
      V_I4(pVarResult)=TextDefBkColor;
   }
   else if (dispid==DISPID_AMBIENT_USERMODE) TerSetBool(pVarResult,!DesignMode);
   else if (dispid==DISPID_AMBIENT_SUPPORTSMNEMONICS) TerSetBool(pVarResult,TRUE);
   else if (dispid==DISPID_AMBIENT_SHOWGRABHANDLES) TerSetBool(pVarResult,DesignMode);
   else if (dispid==DISPID_AMBIENT_SHOWHATCHING) TerSetBool(pVarResult,DesignMode);
   else return ResultFromScode(DISP_E_MEMBERNOTFOUND);

   return NOERROR;
}

HRESULT STDMETHODCALLTYPE IdGetIDsOfNames(LPDISPATCH this, REFIID riid, OLECHAR FAR* FAR* names,
                                   unsigned int cNames, LCID lcid, DISPID far *dispid)
{
    return ResultFromScode(DISP_E_UNKNOWNNAME);
}

HRESULT STDMETHODCALLTYPE IdGetTypeInfo(LPDISPATCH this, UINT info,LCID lcid, ITypeInfo FAR* FAR* ppTypeInfo)
{
    (*ppTypeInfo)=NULL;
    return ResultFromScode(S_OK);
}

HRESULT STDMETHODCALLTYPE IdGetTypeInfoCount(LPDISPATCH this, unsigned int FAR* NumTypes)
{
    (*NumTypes)=0;                  // typeinfo not supported
    return ResultFromScode(S_OK);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///   IDataObject Interface
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct StrDataObject {              // advise sink interface data
   IDataObject DataObject;          // client site
   ULONG     count;                 // use count
   PTERWND   w;                     // window data pointer
};
typedef struct StrDataObject far *LPSTRDATAOBJECT;

HRESULT STDMETHODCALLTYPE IdoQueryInterface(LPDATAOBJECT,REFIID,LPVOID far *);
ULONG STDMETHODCALLTYPE IdoAddRef(LPDATAOBJECT);
ULONG STDMETHODCALLTYPE IdoRelease(LPDATAOBJECT);
HRESULT STDMETHODCALLTYPE IdoGetData(LPDATAOBJECT,/* [unique][in] */ FORMATETC *pformatetcIn,/* [out] */ STGMEDIUM *pmedium);
HRESULT STDMETHODCALLTYPE IdoGetDataHere(LPDATAOBJECT,/* [unique][in] */ FORMATETC *pformatetc,/* [out][in] */ STGMEDIUM *pmedium);
HRESULT STDMETHODCALLTYPE IdoQueryGetData(LPDATAOBJECT,/* [unique][in] */ FORMATETC *pformatetc);
HRESULT STDMETHODCALLTYPE IdoGetCanonicalFormatEtc(LPDATAOBJECT,/* [unique][in] */ FORMATETC *pformatectIn,/* [out] */ FORMATETC *pformatetcOut);
HRESULT STDMETHODCALLTYPE IdoSetData(LPDATAOBJECT,/* [unique][in] */ FORMATETC *pformatetc,/* [unique][in] */ STGMEDIUM *pmedium,/* [in] */ BOOL fRelease);
HRESULT STDMETHODCALLTYPE IdoEnumFormatEtc(LPDATAOBJECT,/* [in] */ DWORD dwDirection,/* [out] */ IEnumFORMATETC **ppenumFormatEtc);
HRESULT STDMETHODCALLTYPE IdoDAdvise(LPDATAOBJECT,/* [in] */ FORMATETC *pformatetc,/* [in] */ DWORD advf,/* [unique][in] */ IAdviseSink *pAdvSink,/* [out] */ DWORD *pdwConnection);
HRESULT STDMETHODCALLTYPE IdoDUnadvise(LPDATAOBJECT,/* [in] */ DWORD dwConnection);
HRESULT STDMETHODCALLTYPE IdoEnumDAdvise(LPDATAOBJECT,/* [out] */ IEnumSTATDATA **ppenumAdvise);

/*****************************************************************************
    CreateDataObject:
    This function creates an instance of IDataObject interface.  The use
    count for the interface is set to 1.
******************************************************************************/
LPDATAOBJECT CreateDataObject(PTERWND w)
{
    LPSTRDATAOBJECT pTerDataObject;
    LPDATAOBJECT pDataObject;

    // allocate and inititialize pTerSite
    if (NULL==(pTerDataObject=(LPSTRDATAOBJECT )OurAlloc(sizeof(struct StrDataObject)))) return NULL;
    pTerDataObject->count=1;                     // count for the interface
    pTerDataObject->w=w;                         // window pointer for the pDataObject

    // allocate and intialize the CLIENTSITE virtual functions
    pDataObject=(LPDATAOBJECT)pTerDataObject;
    if (NULL==(pDataObject->lpVtbl=OurAlloc(sizeof(struct IDataObjectVtbl)))) return NULL;
    if ( NULL==(pDataObject->lpVtbl->QueryInterface=(LPVOID) MakeProcInstance((FARPROC)IdoQueryInterface,hTerInst))
      || NULL==(pDataObject->lpVtbl->AddRef=(LPVOID) MakeProcInstance((FARPROC)IdoAddRef,hTerInst))
      || NULL==(pDataObject->lpVtbl->Release=(LPVOID) MakeProcInstance((FARPROC)IdoRelease,hTerInst))
      || NULL==(pDataObject->lpVtbl->GetData=(LPVOID) MakeProcInstance((FARPROC)IdoGetData,hTerInst))
      || NULL==(pDataObject->lpVtbl->GetDataHere=(LPVOID) MakeProcInstance((FARPROC)IdoGetDataHere,hTerInst))
      || NULL==(pDataObject->lpVtbl->QueryGetData=(LPVOID) MakeProcInstance((FARPROC)IdoQueryGetData,hTerInst))
      || NULL==(pDataObject->lpVtbl->GetCanonicalFormatEtc=(LPVOID) MakeProcInstance((FARPROC)IdoGetCanonicalFormatEtc,hTerInst))
      || NULL==(pDataObject->lpVtbl->SetData=(LPVOID) MakeProcInstance((FARPROC)IdoSetData,hTerInst))
      || NULL==(pDataObject->lpVtbl->EnumFormatEtc=(LPVOID) MakeProcInstance((FARPROC)IdoEnumFormatEtc,hTerInst))
      || NULL==(pDataObject->lpVtbl->DAdvise=(LPVOID) MakeProcInstance((FARPROC)IdoDAdvise,hTerInst))
      || NULL==(pDataObject->lpVtbl->DUnadvise=(LPVOID) MakeProcInstance((FARPROC)IdoDUnadvise,hTerInst))
      || NULL==(pDataObject->lpVtbl->EnumDAdvise=(LPVOID) MakeProcInstance((FARPROC)IdoEnumDAdvise,hTerInst)) ) {
       PrintError(w,MSG_ERR_ADV_CREATE,"CreateDataObject");
       return NULL;
    }

    return pDataObject;
}

/*****************************************************************************
    IdoQueryInterface: Query interface function for IDataObject.
    This function simply delegates to the IOleClientSite::QueryInterface
******************************************************************************/
HRESULT STDMETHODCALLTYPE IdoQueryInterface(LPDATAOBJECT this,REFIID riid,LPVOID far *obj)
{
    (*obj)=NULL;    // set to NULL if interface not supported

    if (IsEqualIID(riid,&IID_IUnknown))        (*obj)=this;
    if (IsEqualIID(riid,&IID_IDataObject))     (*obj)=this;

    if (*obj) {
       LPUNKNOWN far *ppUnk=(LPUNKNOWN far *)obj;
       (*ppUnk)->lpVtbl->AddRef(*ppUnk);
       return ResultFromScode(S_OK);
    }
    
    return ResultFromScode(E_NOINTERFACE);
}

/*****************************************************************************
    IdoAddRef: AddRef function for IDataObject interface
******************************************************************************/
ULONG STDMETHODCALLTYPE IdoAddRef(LPDATAOBJECT this)
{
    LPSTRDATAOBJECT pDataObject=(LPSTRDATAOBJECT )this;

    pDataObject->count++;
    return pDataObject->count;
}

/*****************************************************************************
    IdoRelease: Release function for the IDataObject interface
******************************************************************************/
ULONG STDMETHODCALLTYPE IdoRelease(LPDATAOBJECT this)
{
    LPSTRDATAOBJECT pDataObject=(LPSTRDATAOBJECT )this;

    pDataObject->count--;

    if (pDataObject->count==0) {      // unallocate pDataObject interface
       OurFree(this->lpVtbl);
       OurFree(this);

       return 0;
    }

    return pDataObject->count;
}



HRESULT STDMETHODCALLTYPE IdoGetData(LPDATAOBJECT this,/* [unique][in] */ FORMATETC *pformatetcIn,/* [out] */ STGMEDIUM *pmedium)
{
    LPSTRDATAOBJECT pDataObject=(LPSTRDATAOBJECT )this;
    PTERWND w=pDataObject->w;
    DWORD size;

    if (False(pformatetcIn->tymed&TYMED_HGLOBAL)) return DV_E_FORMATETC;
    
        
    if (pformatetcIn->cfFormat==RtfClipFormat)  {
      pmedium->tymed=TYMED_HGLOBAL;
      pmedium->pUnkForRelease=NULL;
      
      RtfWrite(w,RTF_CB_BUF,NULL);             // write in rich  text format
      pmedium->u.hGlobal=(HGLOBAL)DlgLong;
    }
    else if (pformatetcIn->cfFormat==CF_TEXT)  {
      pmedium->tymed=TYMED_HGLOBAL;
      pmedium->pUnkForRelease=NULL;
      
      RtfWrite(w,RTF_CB_BUF,NULL);             // write in rich  text format
      pmedium->u.hGlobal=(HGLOBAL)TerGetTextSel(hTerWnd,&size);
    }
    else if (pformatetcIn->cfFormat==CF_UNICODETEXT)  {
      pmedium->tymed=TYMED_HGLOBAL;
      pmedium->pUnkForRelease=NULL;
      
      RtfWrite(w,RTF_CB_BUF,NULL);             // write in rich  text format
      pmedium->u.hGlobal=(HGLOBAL)TerGetTextSelU(hTerWnd,&size);
    }
    else return DV_E_FORMATETC;  

    return ResultFromScode(S_OK);
} 
        
HRESULT STDMETHODCALLTYPE IdoGetDataHere(LPDATAOBJECT this,/* [unique][in] */ FORMATETC *pformatetc,/* [out][in] */ STGMEDIUM *pmedium)
{
    return STG_E_MEDIUMFULL;  // don't offer to copy to callers hglobal, because caller can't possibly know the size requirement of rtf data
} 
  
        
HRESULT STDMETHODCALLTYPE IdoQueryGetData(LPDATAOBJECT this,/* [unique][in] */ FORMATETC *pformatetc)
{
    LPSTRDATAOBJECT pDataObject=(LPSTRDATAOBJECT )this;
    PTERWND w=pDataObject->w;

    if (False(pformatetc->tymed&TYMED_HGLOBAL)) return DV_E_FORMATETC;
    if (pformatetc->cfFormat!=RtfClipFormat 
        && pformatetc->cfFormat!=CF_TEXT
        && pformatetc->cfFormat!=CF_UNICODETEXT) return DV_E_FORMATETC;  // only rtf format supported
        
    return ResultFromScode(S_OK);
} 
        
HRESULT STDMETHODCALLTYPE IdoGetCanonicalFormatEtc(LPDATAOBJECT this,/* [unique][in] */ FORMATETC *pformatectIn,/* [out] */ FORMATETC *pformatetcOut)
{
    FarMove(pformatectIn,pformatetcOut,sizeof(FORMATETC));
    pformatetcOut->ptd=NULL;         // data is always device independent

    return DATA_S_SAMEFORMATETC;
} 
        
HRESULT STDMETHODCALLTYPE IdoSetData(LPDATAOBJECT this,/* [unique][in] */ FORMATETC *pformatetc,/* [unique][in] */ STGMEDIUM *pmedium,/* [in] */ BOOL fRelease)
{
    return ResultFromScode(E_NOTIMPL);   // don't let the caller set the data
} 
        
HRESULT STDMETHODCALLTYPE IdoEnumFormatEtc(LPDATAOBJECT this,/* [in] */ DWORD dwDirection,/* [out] */ IEnumFORMATETC **ppenumFormatEtc)
{
    return ResultFromScode(E_NOTIMPL);
} 
        
HRESULT STDMETHODCALLTYPE IdoDAdvise(LPDATAOBJECT this,/* [in] */ FORMATETC *pformatetc,/* [in] */ DWORD advf,/* [unique][in] */ IAdviseSink *pAdvSink,/* [out] */ DWORD *pdwConnection)
{
    return ResultFromScode(E_NOTIMPL);
} 
        
HRESULT STDMETHODCALLTYPE IdoDUnadvise(LPDATAOBJECT this,/* [in] */ DWORD dwConnection)
{
    return ResultFromScode(E_NOTIMPL);
} 
        
HRESULT STDMETHODCALLTYPE IdoEnumDAdvise(LPDATAOBJECT this,/* [out] */ IEnumSTATDATA **ppenumAdvise)
{
    return ResultFromScode(E_NOTIMPL);
} 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///   IDropSource Interface
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct StrDropSource {              // advise sink interface data
   IDropSource DropSource;          // client site
   ULONG     count;                 // use count
   PTERWND   w;                     // window data pointer
};
typedef struct StrDropSource far *LPSTRDROPSOURCE;

HRESULT STDMETHODCALLTYPE IdsQueryInterface(LPDROPSOURCE,REFIID,LPVOID far *);
ULONG STDMETHODCALLTYPE IdsAddRef(LPDROPSOURCE);
ULONG STDMETHODCALLTYPE IdsRelease(LPDROPSOURCE);
HRESULT STDMETHODCALLTYPE IdsQueryContinueDrag(LPDROPSOURCE this,/* [in] */ BOOL fEscapePressed,/* [in] */ DWORD grfKeyState);
HRESULT STDMETHODCALLTYPE IdsGiveFeedback(LPDROPSOURCE this,/* [in] */ DWORD dwEffect);


/*****************************************************************************
    CreateDropSource:
    This function creates an instance of IDropSource interface.  The use
    count for the interface is set to 1.
******************************************************************************/
LPDROPSOURCE CreateDropSource(PTERWND w)
{
    LPSTRDROPSOURCE pTerDropSource;
    LPDROPSOURCE pDropSource;

    // allocate and inititialize pTerSite
    if (NULL==(pTerDropSource=(LPSTRDROPSOURCE )OurAlloc(sizeof(struct StrDropSource)))) return NULL;
    pTerDropSource->count=1;                     // count for the interface
    pTerDropSource->w=w;                         // window pointer for the pDropSource

    // allocate and intialize the CLIENTSITE virtual functions
    pDropSource=(LPDROPSOURCE)pTerDropSource;
    if (NULL==(pDropSource->lpVtbl=OurAlloc(sizeof(struct IDropSourceVtbl)))) return NULL;
    if ( NULL==(pDropSource->lpVtbl->QueryInterface=(LPVOID) MakeProcInstance((FARPROC)IdsQueryInterface,hTerInst))
      || NULL==(pDropSource->lpVtbl->AddRef=(LPVOID) MakeProcInstance((FARPROC)IdsAddRef,hTerInst))
      || NULL==(pDropSource->lpVtbl->Release=(LPVOID) MakeProcInstance((FARPROC)IdsRelease,hTerInst))
      || NULL==(pDropSource->lpVtbl->QueryContinueDrag=(LPVOID) MakeProcInstance((FARPROC)IdsQueryContinueDrag,hTerInst))
      || NULL==(pDropSource->lpVtbl->GiveFeedback=(LPVOID) MakeProcInstance((FARPROC)IdsGiveFeedback,hTerInst)) ){
       PrintError(w,MSG_ERR_ADV_CREATE,"CreateDropSource");
       return NULL;
    }

    return pDropSource;
}

/*****************************************************************************
    IdsQueryInterface: Query interface function for IDropSource.
    This function simply delegates to the IOleClientSite::QueryInterface
******************************************************************************/
HRESULT STDMETHODCALLTYPE IdsQueryInterface(LPDROPSOURCE this,REFIID riid,LPVOID far *obj)
{
    return ResultFromScode(E_NOINTERFACE);
}

/*****************************************************************************
    IdsAddRef: AddRef function for IDropSource interface
******************************************************************************/
ULONG STDMETHODCALLTYPE IdsAddRef(LPDROPSOURCE this)
{
    LPSTRDROPSOURCE pDropSource=(LPSTRDROPSOURCE )this;

    pDropSource->count++;
    return pDropSource->count;
}

/*****************************************************************************
    IdsRelease: Release function for the IDropSource interface
******************************************************************************/
ULONG STDMETHODCALLTYPE IdsRelease(LPDROPSOURCE this)
{
    LPSTRDROPSOURCE pDropSource=(LPSTRDROPSOURCE )this;

    pDropSource->count--;

    if (pDropSource->count==0) {      // unallocate pDropSource interface
       OurFree(this->lpVtbl);
       OurFree(this);

       return 0;
    }

    return pDropSource->count;
}

HRESULT STDMETHODCALLTYPE IdsQueryContinueDrag(LPDROPSOURCE this,/* [in] */ BOOL fEscapePressed,/* [in] */ DWORD grfKeyState)
{
    if(fEscapePressed) return DRAGDROP_S_CANCEL; 

    if(False(grfKeyState & MK_LBUTTON)) return DRAGDROP_S_DROP;  // left button released, drop now

    return S_OK;   // continue dragging
} 
        
HRESULT STDMETHODCALLTYPE IdsGiveFeedback(LPDROPSOURCE this,/* [in] */ DWORD dwEffect)
{
    return DRAGDROP_S_USEDEFAULTCURSORS;
} 

/*******************************************************************************
    TerOleGet:
    Function that the OLE client DLL calls to read the ole data and create an object
*******************************************************************************/
DWORD CALLBACK _export TerOleGet(LPOLESTREAM pStream,void FAR *pBuf, DWORD BufSize)
{
    PTERWND w;
    DWORD offset;
    struct StrOleStream far *OurStream;

    OurStream=(struct StrOleStream far *)pStream;
    w=OurStream->TerData;                                    // retrieve the TER window data pointer


    offset=OurStream->offset;          // offset to read from

    if ((offset+BufSize)>OurStream->MaxSize) {
       PrintError(w,MSG_BAD_OBJ_SIZE,"TerOleGet");
       return 0;                       // error
    }

    HugeMove(&(pOleData[offset]),pBuf,BufSize);
    OurStream->offset=offset+BufSize;  // offset to read from

    return BufSize;
}

/*******************************************************************************
    TerOlePut:
    Function that the OLE client DLL calls to save the OLE data to buffer.
*******************************************************************************/
DWORD CALLBACK _export TerOlePut(LPOLESTREAM pStream, void FAR *pBuf, DWORD BufSize)
{
    PTERWND w;
    DWORD offset;
    struct StrOleStream far *OurStream;

    OurStream=(struct StrOleStream far *)pStream;
    w=OurStream->TerData;              // retrieve the TER window data pointer

    offset=OurStream->offset;          // offset to read from

    if (pOleData) {                    // write mode
      if ((offset+BufSize)>OurStream->MaxSize) {
         PrintError(w,MSG_BAD_OBJ_SIZE,"TerOlePut");
         return 0;                     // error
      }
      HugeMove((void huge *)pBuf,&(pOleData[offset]),BufSize);
    }
    else {                             // query size mode
      OurStream->MaxSize=offset+BufSize;
    }

    OurStream->offset=offset+BufSize;  // offset to read from

    return BufSize;
}

/*******************************************************************************
    GetOleClipItem:
    This function reads the ObjectLink and OwnerLink format data from the
    clipboard.  It returns the class,document and item using the argument
    pointers.  Also the function returns ObjectLinkClipFormat or OwnerLinkClipFormat
    if successful.  Otherwise it returns 0.
*******************************************************************************/
UINT GetOleClipItem(PTERWND w, LPBYTE class, LPBYTE doc, LPBYTE item)
{
    HGLOBAL hMem;
    LPBYTE pClip;
    UINT ClipFormat=0;
    int  len;

    if (!OpenClipboard(hTerWnd)) return 0;

    if (NULL!=(hMem=GetClipboardData(ObjectLinkClipFormat))) ClipFormat=ObjectLinkClipFormat;
    if (hMem==NULL && NULL!=(hMem=GetClipboardData(OwnerLinkClipFormat))) ClipFormat=OwnerLinkClipFormat;
    if (!ClipFormat) goto END;          // clipboard format not found

    if (NULL==(pClip=GlobalLock(hMem))) {
        PrintError(w,MSG_OUT_OF_MEM,"GetOleClipItem");
        CloseClipboard();
        goto END;
    }
   
    // extract the strings
    if (class!=NULL) lstrcpy(class,pClip);

    len=lstrlen(pClip);              // length of the class name
    pClip=&(pClip[len+1]);           // pointer to the document name
    if (doc!=NULL) lstrcpy(doc,pClip);

    len=lstrlen(pClip);              // length of the document name
    pClip=&(pClip[len+1]);           // pointer to the item name
    if (item!=NULL) lstrcpy(item,pClip);

    GlobalUnlock(hMem);

    END:
    CloseClipboard();

    return ClipFormat;
}

/*******************************************************************************
    SetOlePictOffset:
    Set baseline offset for the picture 
*******************************************************************************/
SetOlePictOffset(PTERWND w, int obj)
{
    MFENUMPROC lpProc;

    if (!(TerFont[obj].flags&FFLAG_EQ_OBJECT)) return TRUE;  // only equation object have embedded offset information
    if (!(TerFont[obj].hMeta)) return TRUE;


    // scan the metafile to locate the MFCOMMENT printer escape record
    if (NULL!=(lpProc=(MFENUMPROC)MakeProcInstance((FARPROC)OleEnumMetafile, hTerInst))) {
        DlgInt1=obj;            // pass the current object id
        DlgInt2=FALSE;          // will be set to TRUE in OleEnumMetafile when an MFCOMMENT record is found
        TerFont[obj].offset=0;
        EnumMetaFile(hTerDC,TerFont[obj].hMeta,lpProc,(LPARAM)w);
        FreeProcInstance((FARPROC)lpProc);        // free the process instances
    }


    return TRUE;
}

/******************************************************************************
    OleEnumMetafile:
    Enumeration of metafile records to use background palette.  All metafile
    'SelectPalette' function by default use the foreground palette.
*******************************************************************************/
BOOL CALLBACK _export OleEnumMetafile(HDC hDC, HANDLETABLE FAR *pTable, METARECORD FAR *pMetaRec, int ObjCount, LPARAM lParam)
{
    PTERWND w=(PTERWND)lParam;
    int obj=DlgInt1;
    BYTE string[20];

    if (pMetaRec->rdFunction==META_ESCAPE) {
       int   EscapeCode,count;
       EscapeCode=pMetaRec->rdParm[0];
       count=pMetaRec->rdParm[1];

       if (EscapeCode==MFCOMMENT && count==12) {   // a MathType equation record
          struct StrEq {
             BYTE sign[8];    // "MathType" - No terminating NULL character
             WORD type;       // MathType's comment type is 0
             WORD BaseLineDelta; // 1/16 of a point
          } far *pEq;

          pEq=(LPVOID)&(pMetaRec->rdParm[2]);
          lstrncpy(string,pEq->sign,8);
          string[8]=0;

          if (strcmpi(string,"MathType")==0) {
             TerFont[obj].offset=MulDiv(pEq->BaseLineDelta,20,16);
             SetPictSize(w,obj,TwipsToScrY(TerFont[obj].PictHeight),TwipsToScrX(TerFont[obj].PictWidth),TRUE);
             XlateSizeForPrt(w,obj);              // convert size to printer resolution

             DlgInt2=TRUE;                                // indicates the MFCOMMENT record found
             return FALSE;                                // stop enumeration
          }
       }
    }
    
    return TRUE;                               // keep enumerating
}



#else
    /* OLE not used -- Include the function stubs only */

    BOOL InitOle(PTERWND w) {return TRUE;}
    BOOL ExitOle(PTERWND w) {return TRUE;}
    int  TerPasteSpecial(PTERWND w) {return TRUE;}
    int  TerOleFromClipboard(PTERWND w, UINT format,BOOL PasteLink) {return TRUE;} 
    BOOL TerInsertObject(PTERWND w, int SubKey) {return TRUE;}
    BOOL TerInsertOleFile(HWND,LPBYTE,long,int);
    BOOL TerEditOle(PTERWND w, BOOL edit) {return FALSE;}
    int  TerOleLoad(PTERWND w,int obj) {return TRUE;}
    int  TerOleUnload(PTERWND w,int obj) {return TRUE;}
    BOOL TerOleQuerySize(PTERWND w, int obj, DWORD far *size) {return TRUE;}
    int  GetOleStorageData(PTERWND w,int obj) {return TRUE;}
    BOOL TerInsertDragObject(PTERWND w,HDROP hDrop, BOOL) {return TRUE;}
    int  OlePostProcessing(PTERWND w) {return TRUE;}
    BOOL ShowOcx(PTERWND w, int obj) {return TRUE;}
    BOOL HideOcx(PTERWND w, int obj) {return TRUE;}
    BOOL PosOcx(PTERWND w, int obj, HDC hDC) {return TRUE;}
    int  WINAPI _export TerInsertOleObject(HWND hWnd,LPBYTE ProgName) {return 0;}
    BOOL WINAPI _export TerInvokeOcx(HWND hWnd,int obj,LPBYTE name, UINT flags, int ArgType, DWORD pArg, LPINT ResultType, LPLONG pResult, int ResultLen) {return FALSE;}
    BOOL WINAPI _export TerInvokeOcx2(HWND hWnd,DWORD dwDispatch,LPBYTE name, UINT flags, int ArgType, DWORD pArg, LPINT ResultType, LPLONG pResult, int ResultLen) {return FALSE;}
    int  WINAPI _export TerCreateOcxObject(HWND hWnd,LPBYTE OcxProgName, BOOL msg) {return 0;}
    LPVOID WINAPI _export TerGetIDispatch(HWND hWnd,int obj,BOOL ReleaseId) {return NULL;}
    BOOL  WINAPI _export TerDeleteIDispatch(LPVOID pDisp) {return TRUE;}
    int WINAPI _export TerCreateOcxObject2(HWND hWnd,LPBYTE ProgId, int width, int height) {return 0;}
    LPVOID WINAPI _export TerGetOlePtr(HWND hWnd,int obj,int type) {return NULL;}

#endif
