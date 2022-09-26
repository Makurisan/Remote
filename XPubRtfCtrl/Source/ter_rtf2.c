/*==============================================================================
   TER_RTF2.C
   RTF read for TER files/buffers

   TER Editor Developer's Kit
   Sub Systems, Inc.
   Software License Agreement  (1992)
   ----------------------------------
   This license agreement allows the purchaser the right to modify the
   source code to incorporate in an application larger than the editor itself.
   The target application must not be a programmer's utility 'like' a text editor.
   Sub Systems, Inc. reserves the right to prosecute anybody found to be
   making illegal copies of the executable software or compiling the source
   code for the purpose of selling there after.
===============================================================================*/

#define  PREFIX extern
#include "ter_hdr.h"


/*****************************************************************************
    ReadRtfShpGrp:
    Read the shape group instruction group.This function return 0 when sucessful, otherwise
    it returns an error code (RTF_FILE_INCOMPLETE or RTF_SYNTAX_ERROR)
******************************************************************************/
ReadRtfShpGrp(PTERWND w, struct StrRtf far * rtf)
{
    int    value,CurGroup,InstGroup,result,ControlGroupLevel=0;
    int    LineWdth=20;
    struct StrRtfGroup far* group;
    BYTE   PropName[MAX_WIDTH+1];
    BYTE   str[MAX_WIDTH];
    BOOL HasBorder=true;
    COLORREF color;
    BYTE  string[MAX_WIDTH+1];
    int   val,relLeft=0,relTop=0;
    int   width,height,GroupWidth,LogWidth,GroupHeight,LogHeight;
    struct StrShpGrp ParentShpGrp;

    ControlGroupLevel=rtf->GroupLevel;
    CurGroup=InstGroup=rtf->GroupLevel;
    group=rtf->group;

    group[CurGroup].shape.LineWdth=0;
    group[CurGroup].shape.LineColor=CLR_BLACK;
    group[CurGroup].shape.BackColor=CLR_WHITE;
    group[CurGroup].shape.FrmFlags|=PARA_FRAME_SHPGRP;

    FarMove(&(group[CurGroup].ShpGrp),&(ParentShpGrp),sizeof(ParentShpGrp));  // in case this is a shpgrp within shpgrp (nested)

    while (true) {
       // get next word
       if (!GetRtfWord(w,rtf)) return RTF_FILE_INCOMPLETE;

       if (rtf->GroupBegin) continue;
       if (rtf->GroupEnd) {
          if (rtf->GroupLevel<ControlGroupLevel) break;    // font group ended
          else                                   continue; // look for next font
       }

       CurGroup=rtf->GroupLevel;

       // process the control word
       if (rtf->IsControlWord) {
          if      (strcmpi(rtf->CurWord,"shpleft")==0)    group[InstGroup].ShpGrp.left=(int)rtf->IntParam;
          else if (strcmpi(rtf->CurWord,"shpright")==0)   group[InstGroup].ShpGrp.width=(int)rtf->IntParam-group[InstGroup].ShpGrp.left;
          else if (strcmpi(rtf->CurWord,"shptop")==0)     group[InstGroup].ShpGrp.top=(int)rtf->IntParam;
          else if (strcmpi(rtf->CurWord,"shpbottom")==0)  group[InstGroup].ShpGrp.height=(int)rtf->IntParam-group[InstGroup].ShpGrp.top;
          else if (strcmpi(rtf->CurWord,"shpbxpage")==0)  group[InstGroup].shape.FrmFlags|=PARA_FRAME_HPAGE;
          else if (strcmpi(rtf->CurWord,"shpbypage")==0)  group[InstGroup].shape.FrmFlags|=PARA_FRAME_VPAGE;
          else if (strcmpi(rtf->CurWord,"shpbymargin")==0)group[InstGroup].shape.FrmFlags|=PARA_FRAME_VMARG;
          else if (strcmpi(rtf->CurWord,"shpbxignore")==0)group[InstGroup].shape.FrmFlags|=PARA_FRAME_IGNORE_X;
          else if (strcmpi(rtf->CurWord,"shpbyignore")==0)group[InstGroup].shape.FrmFlags|=PARA_FRAME_IGNORE_Y;
          else if (strcmpi(rtf->CurWord,"shpwr")==0)      group[InstGroup].shape.WrapType=(int)rtf->IntParam;
          else if (strcmpi(rtf->CurWord,"shpwrk")==0)     group[InstGroup].shape.WrapSide=(int)rtf->IntParam;
          else if (strcmpi(rtf->CurWord,"shpz")==0)       group[InstGroup].shape.ZOrder=(rtf->IntParam==0?-1:(int)rtf->IntParam); // for Word/TE compatibliyt, change 0 to -1
          else if (strcmpi(rtf->CurWord,"ssshpalignleft")==0)  group[InstGroup].shape.align=LEFT;
          else if (strcmpi(rtf->CurWord,"ssshpalignright")==0) group[InstGroup].shape.align=RIGHT_JUSTIFY;

          else if (strcmpi(rtf->CurWord,"sp")==0)        PropName[0]=0;   // reset the shape name
          else if (strcmpi(rtf->CurWord,"sn")==0)        ReadRtfShapeParam(w,rtf,PropName); // get the current shape name
          else if (strcmpi(rtf->CurWord,"sv")==0) {      // get certain shape values
             if (strcmpi(PropName,"groupLeft")==0) {  // line width
                if ((result=ReadRtfShapeParam(w,rtf,str))>0) return result;
                group[InstGroup].ShpGrp.GroupLeft=atoi(str);
             }
             else if (strcmpi(PropName,"groupRight")==0) {  // line width
                if ((result=ReadRtfShapeParam(w,rtf,str))>0) return result;
                group[InstGroup].ShpGrp.GroupWidth=atoi(str)-group[InstGroup].ShpGrp.GroupLeft;
             }
             else if (strcmpi(PropName,"groupTop")==0) {  // line width
                if ((result=ReadRtfShapeParam(w,rtf,str))>0) return result;
                group[InstGroup].ShpGrp.GroupTop=atoi(str);
             }
             else if (strcmpi(PropName,"groupBottom")==0) {  // line width
                if ((result=ReadRtfShapeParam(w,rtf,str))>0) return result;
                group[InstGroup].ShpGrp.GroupHeight=atoi(str)-group[InstGroup].ShpGrp.GroupTop;
             }
             
             else if (strcmpi(PropName,"relLeft")==0) {    // group within group - x position relative to the group position
                if ((result=ReadRtfShapeParam(w, rtf, string))>0) return result;
                val=relLeft=atoi(string);
                val=ParentShpGrp.left+val;
                group[InstGroup].ShpGrp.left=val;        // temporary value, will be updated in relRight
             } 
             else if (strcmpi(PropName,"relRight")==0) {    // x position relative to the group position
                if ((result=ReadRtfShapeParam(w, rtf, string))>0) return result;
                val=atoi(string);
                width=val-relLeft;     // width in Group scale
                LogWidth=ParentShpGrp.GroupWidth;  // logical unit in which relLeft and relRight are specified - LogWidth maps to GroupWidth
                GroupWidth=ParentShpGrp.width;     // actual shape-group width in twips units
                group[InstGroup].ShpGrp.left=ParentShpGrp.left+MulDiv(relLeft-ParentShpGrp.GroupLeft,GroupWidth,LogWidth);
                group[InstGroup].ShpGrp.width=MulDiv(width,GroupWidth,LogWidth);
             } 
             else if (strcmpi(PropName,"relTop")==0) {
                if ((result=ReadRtfShapeParam(w, rtf, string))>0) return result;
                val=relTop=atoi(string);
                val=ParentShpGrp.top+val;
                group[InstGroup].ShpGrp.top=val;      // temporary value, will be updated in relBottom
             } 
             else if (strcmpi(PropName,"relBottom")==0) {
                if ((result=ReadRtfShapeParam(w, rtf, string))>0) return result;
                val=atoi(string);
                height=val-relTop;
                LogHeight=ParentShpGrp.GroupHeight;  // logical unit in which relTop and relBottom are specified - LogHeight maps to GroupHeight
                GroupHeight=ParentShpGrp.height;     // actual shape-group height in twips units
                group[InstGroup].ShpGrp.top=ParentShpGrp.top+MulDiv(relTop-ParentShpGrp.GroupTop,GroupHeight,LogHeight);
                group[InstGroup].ShpGrp.height=MulDiv(height,GroupHeight,LogHeight);
             } 
             
             else if (strcmpi(PropName,"lineWidth")==0) {  // line width
                if ((result=ReadRtfShapeParam(w,rtf,str))>0) return result;
                LineWdth=atoi(str);
                LineWdth=EmuToTwips(LineWdth);
                group[InstGroup].shape.LineWdth=LineWdth;
                group[InstGroup].shape.FrmFlags|=PARA_FRAME_BOXED;
             } 
             else if (strcmpi(PropName,"fLine")==0) {  // has border
                if ((result=ReadRtfShapeParam(w,rtf,str))>0) return result;
                HasBorder=True(atoi(str));
                if (HasBorder) group[InstGroup].shape.FrmFlags=PARA_FRAME_BOXED;
             } 
             else if (strcmpi(PropName,"lineDashing")==0) {  // line dashing
                int dashing;
                if ((result=ReadRtfShapeParam(w,rtf,str))>0) return result;
                dashing=atoi(str);
                if (dashing==6) {              // dotted line
                  group[InstGroup].shape.FrmFlags|=PARA_FRAME_DOTTED;
                }   
             } 
             else if (strcmpi(PropName,"fillColor")==0) {  // fill color
                if ((result=ReadRtfShapeParam(w,rtf, str))>0) return result;
                color=(COLORREF)atoi(str);  // extract the backgroud color
                group[InstGroup].shape.BackColor=color;
                group[InstGroup].shape.FillPattern=1;
             }   
             else if (strcmpi(PropName,"fFilled")==0) {  // fill color
                if ((result=ReadRtfShapeParam(w,rtf, str))>0) return result;
                value=atoi(str);
                if (value==0) group[InstGroup].shape.FillPattern=0;
             }   
             else if (strcmpi(PropName,"lineColor")==0) {  // fill color
                if ((result=ReadRtfShapeParam(w,rtf, str))>0) return result;
                color=((int)atoi(str));  // extract the backgroud color
                group[InstGroup].shape.LineColor=color;
             }   
             else if (strcmpi(PropName,"fBehindDocument")==0) {  // fill color
                int BehindText;
                if ((result=ReadRtfShapeParam(w,rtf, str))>0) return result;
                BehindText=atoi(str);  // extract the text rotation
                group[InstGroup].shape.ZOrder=(BehindText==1?-1:1);
             } 
          }
          else if (false && lstrcmpi(rtf->CurWord,"shpgrp")==0) {       // nested shape group not supported
             SkipRtfGroup(w,rtf);
          } 
          else {
             int ReturnCode=ProcessRtfControl(w,rtf);
             if (ReturnCode!=0) return ReturnCode;
             else               continue;
          } 
       }
    }

    return 0;

}



/*****************************************************************************
    ReadRtfShape:
    Read the shape instruction group.This function return 0 when sucessful, otherwise
    it returns an error code (RTF_FILE_INCOMPLETE or RTF_SYNTAX_ERROR)
******************************************************************************/
ReadRtfShape(PTERWND w,struct StrRtf far *rtf)
{
    int    CurGroup,InstGroup,result,ControlGroupLevel=0,ObjectType=OBJ_NONE;
    int    ShapeType=0,LineWdth=20;
    struct StrRtfGroup far *group;
    struct StrRtfGroup far *ShpGroup;
    BYTE huge *pMem=NULL;
    long  ObjectSize;
    int   ObjectAspect=OASPECT_NONE;
    int   ObjectUpdate=FALSE;
    BYTE  PropName[MAX_WIDTH+1];
    BYTE  string[MAX_WIDTH+1],string1[MAX_WIDTH+1];
    BOOL  BkndInfo,FlipH=FALSE,FlipV=FALSE,IsLine=FALSE;
    int   i,val,ParentFID=RtfParaFID,FillPattern=1,value,brightness=0,contrast=0x10000,relLeft=0,relTop=0;
    int   width,height,GroupWidth,LogWidth,GroupHeight,LogHeight;
    BOOL  InTable;
    BOOL HasBorder,IsShapePict=false;
    int   ShapeBelowText=-1;    // -1 = no value, 0=Text below shape, 1=Shape below text
    BOOL  PendingFillBlip=false;


    ControlGroupLevel=rtf->GroupLevel;
    CurGroup=InstGroup=rtf->GroupLevel;
    group=rtf->group;

    BkndInfo=(group[CurGroup].gflags&GFLAG_BACKGROUND)?TRUE:FALSE;
    InTable=group[CurGroup].InTable;

    if (!BkndInfo) rtf->flags|=(RFLAG_READ_SHAPE_PICT/*|RFLAG_INSERT_BEF_PARA*/);   // shprslt will be read if picture was not read from the shpinst
    ResetUintFlag(rtf->flags1,RFLAG1_PNG_FOUND);
    ResetUintFlag(rtf->flags2,RFLAG2_PICT_FRAME_INSERTED);

    ShpGroup=&group[rtf->ShpGroup];

    // initialize the shape structure
    for (i=rtf->ShpGroup;i<=rtf->GroupLevel;i++) {
       if (True(group[CurGroup].gflags&GFLAG_IN_SHPGRP)) {
          if (false) {
             if (True(group[i].shape.FrmFlags&PARA_FRAME_IGNORE_X)) group[i].shape.x=0;
             if (True(group[i].shape.FrmFlags&PARA_FRAME_IGNORE_X)) group[i].ShpGrp.left=0;

             if (True(group[i].shape.FrmFlags&PARA_FRAME_IGNORE_Y)) group[i].shape.y=0;
             if (True(group[i].shape.FrmFlags&PARA_FRAME_IGNORE_Y)) group[i].ShpGrp.top=0;
          }
       }
       else {
          group[i].shape.LineWdth=0;
          group[i].shape.LineColor=0;
          group[i].shape.BackColor=0xFFFFFF;
          group[i].shape.DistFromText=FRAME_DIST_FROM_TEXT;
       }
    }


    CurObject=-1;                  // will be set to 'pict' when a picture is read

    while (TRUE) {
       // get next word
       if (!GetRtfWord(w,rtf)) return RTF_FILE_INCOMPLETE;

       if (rtf->GroupBegin) continue;
       if (rtf->GroupEnd) {
          if (rtf->GroupLevel<ControlGroupLevel) break;    // font group ended
          else                                   continue; // look for next font
       }

       CurGroup=rtf->GroupLevel;

       // process the control word
       if (rtf->IsControlWord) {
          if      (lstrcmpi(rtf->CurWord,"shpleft")==0)  group[InstGroup].shape.x=(int)rtf->IntParam;
          else if (lstrcmpi(rtf->CurWord,"shpright")==0) group[InstGroup].shape.width=(int)rtf->IntParam-group[InstGroup].shape.x;
          else if (lstrcmpi(rtf->CurWord,"shptop")==0)   group[InstGroup].shape.y=(int)rtf->IntParam;
          else if (lstrcmpi(rtf->CurWord,"shpbottom")==0) group[InstGroup].shape.height=(int)rtf->IntParam-group[InstGroup].shape.y;
          else if (lstrcmpi(rtf->CurWord,"shpbxpage")==0) group[InstGroup].shape.FrmFlags|=PARA_FRAME_HPAGE;
          else if (lstrcmpi(rtf->CurWord,"shpbypage")==0) group[InstGroup].shape.FrmFlags|=PARA_FRAME_VPAGE;
          else if (lstrcmpi(rtf->CurWord,"shpbymargin")==0) group[InstGroup].shape.FrmFlags|=PARA_FRAME_VMARG;
          else if (strcmpi(rtf->CurWord,"shpbxignore")==0)group[InstGroup].shape.FrmFlags|=PARA_FRAME_IGNORE_X;
          else if (strcmpi(rtf->CurWord,"shpbyignore")==0)group[InstGroup].shape.FrmFlags|=PARA_FRAME_IGNORE_Y;
          else if (lstrcmpi(rtf->CurWord,"shpwr")==0)     group[InstGroup].shape.WrapType=ShpGroup->shape.WrapType=(int)rtf->IntParam;
          else if (lstrcmpi(rtf->CurWord,"shpwrk")==0)    group[InstGroup].shape.WrapSide=ShpGroup->shape.WrapSide=(int)rtf->IntParam;
          else if (lstrcmpi(rtf->CurWord,"shpz")==0 && ShapeBelowText==-1) {  // ShapeBelowText=-1 means shpfblwtxt tag not found
              group[InstGroup].shape.ZOrder=ShpGroup->shape.ZOrder=(rtf->IntParam==0?-1:(int)rtf->IntParam); // for Word/TE compatibliyt, change 0 to -1
          }
          else if (lstrcmpi(rtf->CurWord,"ssshpalignleft")==0)  group[InstGroup].shape.align=ShpGroup->shape.align=LEFT;
          else if (lstrcmpi(rtf->CurWord,"ssshpalignright")==0) group[InstGroup].shape.align=ShpGroup->shape.align=RIGHT_JUSTIFY;
          else if (lstrcmpi(rtf->CurWord,"shpfblwtxt")==0) {
              ShapeBelowText=(int)rtf->IntParam;
              if      (ShapeBelowText==0) group[InstGroup].shape.ZOrder=ShpGroup->shape.ZOrder=1;  // text below shape
              else if (ShapeBelowText==1) group[InstGroup].shape.ZOrder=ShpGroup->shape.ZOrder=-1; // shape below text
          } 

          else if (lstrcmpi(rtf->CurWord,"sp")==0)        lstrcpy(PropName,"");   // reset the shape name
          else if (lstrcmpi(rtf->CurWord,"sn")==0)        ReadRtfShapeParam(w,rtf,PropName); // get the current shape name
          else if (lstrcmpi(rtf->CurWord,"sv")==0) {      // get certain shape values
             if ((lstrcmpi(PropName,"shapeType")==0 || lstrcmpi(PropName,"pVerticies")==0) && !BkndInfo) {
                if ((result=ReadRtfShapeParam(w,rtf,string))) return result;
                
                // get the shape type from pVerticies
                ShapeType=0;
                if (lstrcmpi(PropName,"pVerticies")==0) {
                   int EleSize,NumPoints;
                   if (sscanf(string,"%d;%d;",&EleSize,&NumPoints)==2) { // if both fields read
                      if      (NumPoints==2) ShapeType=SHPTYPE_LINE;
                      else if (NumPoints==4) ShapeType=SHPTYPE_RECT;
                      if (ShapeType==SHPTYPE_LINE && EleSize==8) {    // read the line vertices, ElSize =8 imples each point in (%d,%d) format
                         int x1,y1,x2,y2,temp;
                         if (sscanf(string,"%d;%d;(%d,%d);(%d,%d);",&temp,&temp,&x1,&y1,&x2,&y2)==6) { // if all 6 fields found
                            group[InstGroup].shape.x+=x1;
                            group[InstGroup].shape.y+=y1;
                            group[InstGroup].shape.width=x2-x1;
                            group[InstGroup].shape.height=y2-y1;
                         } 
                      }
                   }
                } 
                else {
                   ShapeType=atoi(string);  // save to the parent shpinst group
                   if (ShapeType==32) ShapeType=SHPTYPE_LINE;  // special line type or callout type
                }
                
                group[InstGroup].shape.type=ShapeType;  // save to the parent shpinst group

                if (ShapeType==SHPTYPE_PICT_FRAME || ShapeType==SHPTYPE_OCX) // treat it as a picture frame
                   ResetUintFlag(rtf->flags,RFLAG_INSERT_BEF_PARA);

                if (ShapeType==SHPTYPE_LINE || ShapeType==SHPTYPE_RECT || ShapeType==SHPTYPE_ELLIPSE) {   // not yet supported
                   int SaveGroupLevel=rtf->GroupLevel;

                   IsLine=(ShapeType==SHPTYPE_LINE);
                   group[InstGroup].shape.FrmFlags|=(IsLine?PARA_FRAME_LINE:PARA_FRAME_RECT);
                   group[InstGroup].FrmFlags|=(group[InstGroup].shape.FrmFlags);
                   group[InstGroup].gflags|=GFLAG_USE_SHP_INFO;
                   group[InstGroup].HPageGroup=InstGroup;   // group where PARA_FRAME_HPAGE was set
                   group[InstGroup].shape.LineWdth=15;                // rtf spec default
                   group[InstGroup].shape.FrmFlags|=PARA_FRAME_BOXED; // rtf spec default

                   if (True(group[InstGroup].gflags&(GFLAG_IN_SHPGRP|GFLAG_IN_FRAME)) || InTable 
                      || ShapeType==SHPTYPE_ELLIPSE || ShapeType==SHPTYPE_LINE) {  // create as picture frame
                      struct StrRtfPict pic;

                      FarMemSet(&pic,0,sizeof(pic));

                      pic.type=PICT_SHAPE;
                      IsShapePict=true;

                      // pass the picture to the application
                      group[InstGroup].gflags2|=GFLAG2_PICT_SHAPE;
                      rtf->GroupLevel=InstGroup;                     // all parameters are in the InstGroup level
                      ImportRtfData(w,RTF_PICT,&(rtf->group[rtf->GroupLevel]),null,(LPBYTE)&pic);
                      rtf->GroupLevel=SaveGroupLevel;
                   } 
                   else {
                      group[InstGroup].FrmFlags|=PARA_FRAME_USED;
                      rtf->flags|=RFLAG_INSERT_BEF_PARA;      // drawing object should be in its own paragraph
                      rtf->flags|=RFLAG_NEW_FRAME;               // new frame 
                      rtf->OutBuf[rtf->OutBufLen]=ParaChar;         // insert a para marker
                      rtf->OutBufLen++;
                      rtf->OutBuf[rtf->OutBufLen]=0;                // NULL termination
                  
                      rtf->GroupLevel=InstGroup;                     // all parameters are in the InstGroup level
                      if (!SendRtfText(w,rtf)) return RTF_END_FILE;       // send this line
                      rtf->GroupLevel=SaveGroupLevel;
                   }

                   ParaFrame[RtfParaFID].LineColor=group[InstGroup].shape.LineColor;
                   ParaFrame[RtfParaFID].BackColor=group[InstGroup].shape.BackColor;

                   ParaFrame[RtfParaFID].LineWdth=LineWdth;
                   ParaFrame[RtfParaFID].flags|=PARA_FRAME_BOXED;
                   
                   if (IsLine) SetRtfLineOrient(w,rtf,FlipH,FlipV);

                   ResetUintFlag(rtf->flags,(RFLAG_READ_SHAPE_PICT|RFLAG_INSERT_BEF_PARA));
                }
                else if (ShapeType==SHPTYPE_TEXT_BOX && !InTable && !(group[InstGroup].gflags&GFLAG_IN_FRAME)) {
                   group[InstGroup].FrmFlags|=(PARA_FRAME_USED|PARA_FRAME_TEXT_BOX);
                   group[InstGroup].FrmFlags|=(group[InstGroup].shape.FrmFlags);
                   group[InstGroup].gflags|=GFLAG_USE_SHP_INFO;
                   group[InstGroup].HPageGroup=InstGroup;  // group where PARA_FRAME_HPAGE was set
                   group[InstGroup].shape.FrmFlags=PARA_FRAME_BOXED; // rtf spec default
                   group[InstGroup].shape.LineWdth=15;               // rtf spec default width in twips
                   group[InstGroup].TextBoxMargin=2*MIN_FRAME_MARGIN;
                   rtf->flags|=RFLAG_INSERT_BEF_PARA;      // drawing object should be in its own paragraph
                   rtf->flags|=RFLAG_NEW_FRAME;               // new frame 
                }
                else ShapeType=0;

                HasBorder=true;                           // initial value
             }
             else if (strcmpi(PropName,"relLeft")==0) {    // x position relative to the group position
                if ((result=ReadRtfShapeParam(w, rtf, string))>0) return result;
                val=relLeft=atoi(string);
                val=group[InstGroup].ShpGrp.left+val;
                group[InstGroup].shape.x=val;        // temporary value, will be updated in relRight
             } 
             else if (strcmpi(PropName,"relRight")==0) {    // x position relative to the group position
                if ((result=ReadRtfShapeParam(w, rtf, string))>0) return result;
                val=atoi(string);
                width=val-relLeft;     // width in Group scale
                LogWidth=group[InstGroup].ShpGrp.GroupWidth;  // logical unit in which relLeft and relRight are specified - LogWidth maps to GroupWidth
                GroupWidth=group[InstGroup].ShpGrp.width;     // actual shape-group width in twips units
                group[InstGroup].shape.x=group[InstGroup].ShpGrp.left+MulDiv(relLeft-group[InstGroup].ShpGrp.GroupLeft,GroupWidth,LogWidth);
                group[InstGroup].shape.width=MulDiv(width,GroupWidth,LogWidth);
             } 
             else if (strcmpi(PropName,"relTop")==0) {
                if ((result=ReadRtfShapeParam(w, rtf, string))>0) return result;
                val=relTop=atoi(string);
                val=group[InstGroup].ShpGrp.top+val;
                group[InstGroup].shape.y=val;      // temporary value, will be updated in relBottom
             } 
             else if (strcmpi(PropName,"relBottom")==0) {
                if ((result=ReadRtfShapeParam(w, rtf, string))>0) return result;
                val=atoi(string);
                height=val-relTop;
                LogHeight=group[InstGroup].ShpGrp.GroupHeight;  // logical unit in which relTop and relBottom are specified - LogHeight maps to GroupHeight
                GroupHeight=group[InstGroup].ShpGrp.height;     // actual shape-group height in twips units
                group[InstGroup].shape.y=group[InstGroup].ShpGrp.top+MulDiv(relTop-group[InstGroup].ShpGrp.GroupTop,GroupHeight,LogHeight);
                group[InstGroup].shape.height=MulDiv(height,GroupHeight,LogHeight);
             } 
             else if (strcmpi(PropName,"posh")==0) {  // overrides shpleft value
                int ParaFID;
                ParaFID=(True(rtf->flags2&RFLAG2_PICT_FRAME_INSERTED) && rtf->PictFID>0)?rtf->PictFID:RtfParaFID;
                
                if ((result=ReadRtfShapeParam(w,rtf,string))>0) return result;
                val=atoi(string);
                if (ParaFID>0) {
                   if (val==1)      ParaFrame[ParaFID].x=ParaFrame[ParaFID].OrgX=0;  // left aligned
                   else if (val==2) ParaFrame[ParaFID].flags|=PARA_FRAME_CENTER;
                   else if (val==3) ParaFrame[ParaFID].flags|=PARA_FRAME_RIGHT_JUST;
                }
                if      (val==1)  group[InstGroup].shape.x=0;
                else if (val==2)  group[InstGroup].shape.FrmFlags|=PARA_FRAME_CENTER;
                else if (val==3)  group[InstGroup].shape.FrmFlags|=PARA_FRAME_RIGHT_JUST;
             }
             else if (_strcmpi(PropName,"posrelv")==0) {  // vertical positioning
                int ParaFID;
                if ((result=ReadRtfShapeParam(w,rtf,string))>0) return result;
                val=atoi(string);
                ParaFID=(True(rtf->flags2&RFLAG2_PICT_FRAME_INSERTED) && rtf->PictFID>0)?rtf->PictFID:RtfParaFID;
                if (ParaFID>0) {
                   ResetUintFlag(ParaFrame[ParaFID].flags,(PARA_FRAME_VPAGE|PARA_FRAME_VMARG|PARA_FRAME_IGNORE_Y));
                   if (val==0)      ParaFrame[ParaFID].flags|=PARA_FRAME_VMARG;  // val==0 : margin relative
                   else if (val==1) ParaFrame[ParaFID].flags|=PARA_FRAME_VPAGE;
                }
                ResetUintFlag(group[InstGroup].shape.FrmFlags,(PARA_FRAME_VPAGE|PARA_FRAME_VMARG|PARA_FRAME_IGNORE_Y));
                if (val==0) group[InstGroup].shape.FrmFlags|=PARA_FRAME_VMARG;
                else        group[InstGroup].shape.FrmFlags|=PARA_FRAME_VPAGE;
             }
             else if (_strcmpi(PropName,"posrelh")==0) {  // horizontal positioning
                int ParaFID;
                if ((result=ReadRtfShapeParam(w,rtf,string))>0) return result;
                val=atoi(string);
                ParaFID=(True(rtf->flags2&RFLAG2_PICT_FRAME_INSERTED) && rtf->PictFID>0)?rtf->PictFID:RtfParaFID;
                if (ParaFID>0) {
                   ResetUintFlag(ParaFrame[ParaFID].flags,(PARA_FRAME_HPAGE|PARA_FRAME_IGNORE_X));
                   if (val==1) {                     // page relative, convert to margin relative
                      ParaFrame[ParaFID].x=ParaFrame[ParaFID].OrgX-((int)InchesToTwips(rtf->sect.LeftMargin));
                      ParaFrame[ParaFID].flags|=PARA_FRAME_HPAGE;
                   }
                }
                ResetUintFlag(group[InstGroup].shape.FrmFlags,(PARA_FRAME_HPAGE|PARA_FRAME_IGNORE_X));
                if (val==1) group[InstGroup].shape.FrmFlags|=PARA_FRAME_HPAGE;  // this value will be convert to margin relative value SetRtfParaFID function
             }
             else if (_strcmpi(PropName,"fLayoutInCell")==0) {  // cell valitioning relative to cell (instead of page)
                int ParaFID;
                if ((result=ReadRtfShapeParam(w,rtf,string))>0) return result;
                val=atoi(string);
                ParaFID=(True(rtf->flags2&RFLAG2_PICT_FRAME_INSERTED) && rtf->PictFID>0)?rtf->PictFID:RtfParaFID;
                if (ParaFID>0) {
                   ResetUintFlag(ParaFrame[ParaFID].flags,PARA_FRAME_IN_CELL);
                   if (val==1)  ParaFrame[ParaFID].flags|=PARA_FRAME_IN_CELL;
                }
                ResetUintFlag(group[InstGroup].shape.FrmFlags,PARA_FRAME_IN_CELL);
                if (val==1) group[InstGroup].shape.FrmFlags|=PARA_FRAME_IN_CELL;
             }
             else if (lstrcmpi(PropName,"lineWidth")==0) {  // line width
                if (HasBorder) {
                   if ((result=ReadRtfShapeParam(w,rtf,string))) return result;
                   LineWdth=atoi(string);
                   LineWdth=EmuToTwips(LineWdth);
                   if ((ShapeType==SHPTYPE_LINE || ShapeType==SHPTYPE_RECT)&& RtfParaFID>0) {
                      ParaFrame[RtfParaFID].LineWdth=LineWdth;
                      ParaFrame[RtfParaFID].flags|=PARA_FRAME_BOXED;
                   }
                   if (ShapeType==SHPTYPE_TEXT_BOX || ShapeType==SHPTYPE_RECT) {
                      group[InstGroup].shape.LineWdth=LineWdth;
                      group[InstGroup].shape.FrmFlags|=PARA_FRAME_BOXED;
                   }
                }    
             } 
             else if (lstrcmpi(PropName,"fLine")==0) {  // has border
                if ((result=ReadRtfShapeParam(w,rtf,string))) return result;
                HasBorder=atoi(string);
                if (ShapeType==SHPTYPE_TEXT_BOX || ShapeType==SHPTYPE_RECT) {
                   if (HasBorder) {
                     if (group[InstGroup].shape.LineWdth==0) group[InstGroup].shape.LineWdth=20;   // default width in twips
                     group[InstGroup].shape.FrmFlags|=PARA_FRAME_BOXED;
                   }
                   else {
                      ResetLongFlag(group[InstGroup].shape.FrmFlags,PARA_FRAME_BOXED);
                      if (RtfParaFID>0) ResetLongFlag(ParaFrame[RtfParaFID].flags,PARA_FRAME_BOXED);
                   }
                }
                else if (ShapeType==SHPTYPE_LINE) {
                   if (!HasBorder)  {
                      ParaFrame[RtfParaFID].flags|=PARA_FRAME_NO_LINE;
                      if (RtfParaFID>0) ResetLongFlag(ParaFrame[RtfParaFID].flags,PARA_FRAME_NO_LINE);
                   }
                } 
             } 
             else if (lstrcmpi(PropName,"lineDashing")==0) {  // line dashing
                int dashing;
                if ((result=ReadRtfShapeParam(w,rtf,string))) return result;
                dashing=atoi(string);
                if (dashing==6) {              // dotted line
                  if ((ShapeType==SHPTYPE_LINE || ShapeType==SHPTYPE_RECT)&& RtfParaFID>0) {
                     ParaFrame[RtfParaFID].flags|=(PARA_FRAME_DOTTED|PARA_FRAME_BOXED);
                  }
                  if (ShapeType==SHPTYPE_TEXT_BOX || ShapeType==SHPTYPE_RECT) {
                     group[InstGroup].shape.FrmFlags=(PARA_FRAME_DOTTED|PARA_FRAME_BOXED);
                  } 
                }   
             } 
             else if (lstrcmpi(PropName,"fillBlip")==0) { // shape filled with picture
                for (i=InstGroup;i<=rtf->GroupLevel;i++) group[i].shape.FrmFlags|=PARA_FRAME_FILL_PICT;
                if (!BkndInfo) rtf->flags|=RFLAG_READ_SHAPE_PICT;  // mark picture as not read yet again
                PendingFillBlip=false;
             } 
             else if (lstrcmpi(PropName,"fillType")==0) { // shape filled with picture
                int FillType;
                if ((result=ReadRtfShapeParam(w,rtf,string))) return result;
                FillType=atoi(string);
                if (FillType==3) {         // fill with picture
                   for (i=InstGroup;i<=rtf->GroupLevel;i++) group[i].shape.FrmFlags|=PARA_FRAME_FILL_PICT;
                   if (!BkndInfo) rtf->flags|=RFLAG_READ_SHAPE_PICT;  // mark picture as not read yet again
                   PendingFillBlip=true;     // now wait for fillblip to fill the shape
                }
             } 
             else if (lstrcmpi(PropName,"pibName")==0) {  // linked file name
                rtf->flags|=RFLAG_IGNORE_SLASH;  // don't treat backslash as control word or special character
                if ((result=ReadRtfShapeParam(w,rtf,string1))) return result;
                ResetUintFlag(rtf->flags,RFLAG_IGNORE_SLASH);

                StripSlashes(w,string1,string);  // convert double slash to single slash in the string
                XlateRtfHex(w,string);
                if (CurObject<0) CurObject=ReadRtfLinkedPicture(w,rtf,string);
                else             TerPictLinkName(hTerWnd,CurObject,FALSE,string);  // set the link name
                if (CurObject>=0) ResetUintFlag(rtf->flags,RFLAG_READ_SHAPE_PICT);  // picture read
             }
             else if (lstrcmpi(PropName,"pibFlags")==0) {  // picture link type
                if ((result=ReadRtfShapeParam(w,rtf,string))) return result;
                if (CurObject>=0 && lstrcmp(string,"10")!=0 && lstrcmp(string,"14")!=0)
                   TerPictLinkName(hTerWnd,CurObject,FALSE,NULL);  // remove the picture link name
             }
             else if (lstrcmpi(PropName,"fillColor")==0) {  // fill color
                COLORREF color;
                if ((result=ReadRtfShapeParam(w,rtf,string))) return result;
                color=(COLORREF)atol(string);  // extract the backgroud color
                if (BkndInfo && RtfInput<RTF_CB && !(TerFlags3&TFLAG3_NO_RTF_BKND_COLOR)) {
                    if (True(TerFlags6&TFLAG6_SAVE_TEXT_BK_COLOR)) TextDefBkColor=color;  // extract the backgroud color
                    else                                           PageBkColor=color;  // extract the backgroud color
                }        
                if ((ShapeType==SHPTYPE_LINE || ShapeType==SHPTYPE_RECT)&& RtfParaFID>0) {
                   ParaFrame[RtfParaFID].BackColor=color;
                   ParaFrame[RtfParaFID].FillPattern=FillPattern;
                } 
                if (ShapeType==SHPTYPE_TEXT_BOX || ShapeType==SHPTYPE_RECT) {
                   group[InstGroup].shape.BackColor=color;
                   group[InstGroup].shape.FillPattern=FillPattern;
                } 
             }   
             else if (strcmpi(PropName,"fFilled")==0) {  // fill color
                if ((result=ReadRtfShapeParam(w,rtf, string))>0) return result;
                value=atoi(string);
                if (value==0) FillPattern=group[InstGroup].shape.FillPattern=0;
             }   
             else if (lstrcmpi(PropName,"lineColor")==0) {  // fill color
                COLORREF color;
                if ((result=ReadRtfShapeParam(w,rtf,string))) return result;
                color=(COLORREF)atol(string);  // extract the backgroud color
                if ((ShapeType==SHPTYPE_LINE || ShapeType==SHPTYPE_RECT)&& RtfParaFID>0) {
                   ParaFrame[RtfParaFID].LineColor=color;
                }
                if (ShapeType==SHPTYPE_TEXT_BOX || ShapeType==SHPTYPE_RECT) {
                   group[InstGroup].shape.LineColor=color;
                } 
             }   
             else if (lstrcmpi(PropName,"txflTextFlow")==0) {  // fill color
                int TextFlow,angle=0,i;
                if ((result=ReadRtfShapeParam(w,rtf,string))) return result;
                TextFlow=atol(string);  // extract the text rotation
                if   (TextFlow==TEXTFLOW_T2B || TextFlow==TEXTFLOW_T2B_NON_ANSII) angle=270;
                else if (TextFlow==TEXTFLOW_B2T) angle=90;
                for (i=rtf->ShpGroup;i<=rtf->GroupLevel;i++) group[i].TextAngle=angle;  // save text angle
             }
             else if (lstrcmpi(PropName,"fBehindDocument")==0) {  // fill color
                int i,BehindText;
                if ((result=ReadRtfShapeParam(w,rtf,string))) return result;
                BehindText=atol(string);  // extract the text rotation
                group[InstGroup].shape.ZOrder=ShpGroup->shape.ZOrder=(BehindText==1?-1:1);
                for (i=rtf->ShpGroup;i<=rtf->GroupLevel;i++) group[i].shape.ZOrder=ShpGroup->shape.ZOrder;
                if ((ShapeType==SHPTYPE_LINE || ShapeType==SHPTYPE_RECT)&& RtfParaFID>0) {
                   ParaFrame[RtfParaFID].ZOrder=group[InstGroup].shape.ZOrder;
                }
             } 
             else if (strcmpi(PropName,"dxTextLeft")==0) {  // left distance from the text
                if ((result=ReadRtfShapeParam(w,rtf, string))>0) return result;
                group[InstGroup].TextBoxMargin=EmuToTwips(atoi(string));  // extract the text rotation
             } 
             else if (lstrcmpi(PropName,"fFlipH")==0 || lstrcmpi(PropName,"fRelFlipH")==0) {  // Horizontal flip
                if ((result=ReadRtfShapeParam(w,rtf,string))) return result;
                FlipH=atol(string);
                if (IsLine && FlipH) SetRtfLineOrient(w,rtf,FlipH,FlipV);
             } 
             else if (lstrcmpi(PropName,"fFlipV")==0 || lstrcmpi(PropName,"fRelFlipV")==0) {  // vertical flip
                if ((result=ReadRtfShapeParam(w,rtf,string))) return result;
                FlipV=atol(string);
                if (IsLine && FlipV) SetRtfLineOrient(w,rtf,FlipH,FlipV);
             } 
             else if (lstrcmpi(PropName,"wzName")==0) {  // watermark
                if ((result=ReadRtfShapeParam(w,rtf,string))) return result;
                if (strcmpi(string,"WordPictureWatermark3")==0) {   // picture water mark
                   if (True(rtf->flags2&RFLAG2_PICT_FRAME_INSERTED) && rtf->PictFID>0) {
                      int pict;
                      if (RtfInput<RTF_CB) {
                         WmParaFID=rtf->PictFID;                       // record the paragraph frame id for watermark
                         ParaFrame[rtf->PictFID].flags|=PARA_FRAME_WATERMARK;
                      
                         pict=ParaFrame[WmParaFID].pict;
                         if (brightness!=0 || contrast!=0x10000) ApplyPictureBrightnessContrast(w,pict,brightness,contrast);
                         SetPictSize(w,pict,TwipsToScrY(TerFont[pict].PictHeight),TwipsToScrX(TerFont[pict].PictWidth),TRUE);
                         XlateSizeForPrt(w,pict);              // convert size to printer resolution
                      }
                      
                      MoveLineData(w,rtf->PictFrameLine,rtf->PictFrameCol,1,'D');  // delete this picture from the text
                   } 
                } 
             } 
             else if (lstrcmpi(PropName,"pictureBrightness")==0) {  
                if ((result=ReadRtfShapeParam(w,rtf,string))) return result;
                brightness=atoi(string);
             }
             else if (lstrcmpi(PropName,"pictureContrast")==0) {  
                if ((result=ReadRtfShapeParam(w,rtf,string))) return result;
                contrast=atoi(string);
             }
          }

          else if (lstrcmpi(rtf->CurWord,"pict")==0){
             if (CurObject>=0) SkipRtfGroup(w,rtf);   // a picture alread read
             else {
                result=ReadRtfPicture(w,rtf);
                if (result) return result;       // error condition

                if (True(group[rtf->GroupLevel].shape.FrmFlags&PARA_FRAME_FILL_PICT) && group[rtf->GroupLevel].shape.FillPict>0) {
                    for (i=InstGroup;i<rtf->GroupLevel;i++) group[i].shape.FillPict=group[rtf->GroupLevel].shape.FillPict;
                    if ((ShapeType==SHPTYPE_LINE || ShapeType==SHPTYPE_RECT)&& RtfParaFID>0) {
                       ParaFrame[RtfParaFID].flags|=PARA_FRAME_FILL_PICT;
                       ParaFrame[RtfParaFID].FillPict=group[InstGroup].shape.FillPict;
                    }
                }
                else if (!(rtf->flags&RFLAG_READ_SHAPE_PICT) && rtf->flags&RFLAG_INSERT_BEF_PARA && !BkndInfo) { // picture read - append a para marker
                   rtf->OutBuf[rtf->OutBufLen]=ParaChar;   // insert a para marker
                   rtf->OutBufLen++;
                   rtf->OutBuf[rtf->OutBufLen]=0;          // NULL termination
                   SendRtfText(w,rtf);
                }
              }
          }
          else if (lstrcmpi(rtf->CurWord,"objemb")==0) ObjectType=OBJ_EMBED;       // embedded object
          else if (lstrcmpi(rtf->CurWord,"objocx")==0) ObjectType=OBJ_OCX;       // embedded object
          else if (lstrcmpi(rtf->CurWord,"objautlink")==0) ObjectType=OBJ_LINK;    // linked object
          else if (lstrcmpi(rtf->CurWord,"objupdate")==0) ObjectUpdate=TRUE;    // display content
          else if (lstrcmpi(rtf->CurWord,"sscontent")==0) ObjectAspect=OASPECT_CONTENT;    // display content
          else if (lstrcmpi(rtf->CurWord,"ssicon")==0) ObjectAspect=OASPECT_ICON;    // display icon
          else if (lstrcmpi(rtf->CurWord,"objdata")==0) {
             pMem=ReadRtfObjBytes(w,rtf,&ObjectSize);
          }
          else if (lstrcmpi(rtf->CurWord,"shp")==0) {         // nested shape not supported
             SkipRtfGroup(w,rtf);
          } 
          else if (lstrcmpi(rtf->CurWord,"shpgrp")==0) {       // nested shape group not supported
             SkipRtfGroup(w,rtf);
          } 
          else if (lstrcmpi(rtf->CurWord,"shptxt")==0) {      // not yet supported - do not skip the group, because it might include objects
             //SkipRtfGroup(w,rtf);
             if (RtfParaFID>0 && ShapeType==SHPTYPE_LINE) {  // shptext found for line or rectangle
                rtf->flags|=RFLAG_READ_SHAPE_PICT;    // disregard line/rect and read shprslt
             }
             else {
               if (ShapeType==SHPTYPE_RECT) {         // convert RECT to TEXT_BOX
                  if (CurLine>0) {
                    int i,line=CurLine-1;
                    if (IsShapePict) {               // delete the rectangle object entered as a picture shape
                       for (line=CurLine-1;line>=0;line--) {
                          if (LineLen(line)==1) {
                              LPWORD fmt=OpenCfmt(w,line);
                              int CurFont=fmt[0];                // picture index serves as the formatting attribute
                              CloseCfmt(w,line);
                              if (TerFont[CurFont].style&PICT && TerFont[CurFont].ParaFID==RtfParaFID) {
                                  MoveLineData(w,line,0,1,'D');     // delete the paramarker character
                                  DeleteTerObject(w,CurFont);       // this picture font object no longer needed
                                  ParaFrame[RtfParaFID].InUse=FALSE;
                                  RtfParaFID=0;
                                  break;
                              } 
                          } 
                       }
                    
                    }
                    else {                           // delete the rectangle object entered as a drawing object
                       for (line=CurLine-1;line>=0;line--) if (fid(line)==RtfParaFID) break;
                       if (line>=0 && fid(line)>0) {
                           MoveLineData(w,line,0,1,'D');     // delete the paramarker character
                           ParaFrame[RtfParaFID].InUse=FALSE;
                           fid(line)=RtfParaFID=0;
                       }
                    }

                    group[InstGroup].shape.type=ShapeType=SHPTYPE_TEXT_BOX;     
                    ResetLongFlag(group[InstGroup].FrmFlags,PARA_FRAME_RECT);
                    ResetLongFlag(group[InstGroup].shape.FrmFlags,PARA_FRAME_RECT);
                    ResetUintFlag(group[InstGroup].gflags2,GFLAG2_PICT_SHAPE);
                    group[InstGroup].FrmFlags|=(PARA_FRAME_USED|PARA_FRAME_TEXT_BOX);
                    group[InstGroup].FrmFlags|=(group[InstGroup].shape.FrmFlags);
                    group[InstGroup].gflags|=GFLAG_USE_SHP_INFO;
                    group[InstGroup].HPageGroup=InstGroup;  // group where PARA_FRAME_HPAGE was set
                    for (i=(InstGroup+1);i<=(rtf->GroupLevel);i++) {
                       group[i].shape=group[InstGroup].shape;
                       group[i].FrmFlags=group[InstGroup].FrmFlags;
                       group[i].gflags=group[InstGroup].gflags;
                       group[i].gflags2=group[InstGroup].gflags2;
                       group[i].HPageGroup=group[InstGroup].HPageGroup;
                    } 
                    rtf->flags|=RFLAG_INSERT_BEF_PARA;      // drawing object should be in its own paragraph
                    rtf->flags|=RFLAG_NEW_FRAME;               // new frame 
                    rtf->flags|=RFLAG_READ_SHAPE_PICT;
                  } 
               } 
               if (ShapeType==SHPTYPE_TEXT_BOX) {
                  if (group[InstGroup].shape.width<40 && group[InstGroup].shape.height<40) SkipRtfGroup(w,rtf);  // too small to show
                  else {
                     int ReturnCode;
                     ReturnCode=ReadRtfShapeText(w,rtf);
                     if (ReturnCode!=0) return ReturnCode;
                  }
                  ResetUintFlag(rtf->flags,RFLAG_READ_SHAPE_PICT);  // so that shprslt is not read
               }
             }
          } 
       }
    }

    if (pMem && CurObject>0 && ObjectSize>0 && ObjectType!=OBJ_NONE) 
       MakeRtfObject(w,rtf,pMem,ObjectType,ObjectSize,ObjectAspect,ObjectUpdate);

    ResetUintFlag(rtf->flags,RFLAG_INSERT_BEF_PARA);

    // initialize the shp group if a png picture was found
    if (rtf->flags&RFLAG_READ_SHAPE_PICT && rtf->flags1&RFLAG1_PNG_FOUND) {
       FarMove(&(group[InstGroup].shape),&(ShpGroup->shape),sizeof(struct StrShape));
    }

    if (!PendingFillBlip) RtfParaFID=ParentFID;   // restored any containing id

    return 0;
}

/*****************************************************************************
    GetRtfInsertionLine:
    Get the line at which to insert the new text.  This function returns -1
    if an error occurs.
******************************************************************************/
long GetRtfInsertionLine(PTERWND w, struct StrRtf far *rtf)
{
    long InsLine,l;
    struct StrRtfGroup far *group=&(rtf->group[rtf->GroupLevel]);

    // Set the text array
    if (!CheckLineLimit(w,TotalLines+1)) {
        PrintError(w,MSG_LINE_TRUNCATED,MsgString[MSG_MAX_LINES_EXCEEDED]);
        return -1;
    }

    // get the insertion line
    InsLine=CurLine;
    if (rtf->flags&RFLAG_INSERT_BEF_PARA || group->gflags&GFLAG_INSERT_BEF_PARA) {   // insert before current paragraph
       while (InsLine>0) {
          if (LineFlags(InsLine-1)&(LFLAG_PARA|LFLAG_BREAK) || fid(InsLine-1)>0) break;
          InsLine--;
       }
    }
    else if (rtf->InsertBefCell>0) {          // insert before a specified cell
       int level=cell[rtf->InsertAftCell].level;
       for (l=0;l<TotalLines;l++) if (LevelCell(w,level,-cid(l))==rtf->InsertBefCell) break;
       if (l<TotalLines) InsLine=l;
       CurLine=InsLine;
    }
    else if (rtf->InsertAftCell>0) {          // insert after the specified cell
       int level=cell[rtf->InsertAftCell].level;
       for (l=0;l<TotalLines;l++) if (LevelCell(w,level,-cid(l))==rtf->InsertAftCell) break;
       if (l<TotalLines) {
          for (;l<TotalLines;l++) if (LevelCell(w,level,-cid(l))!=rtf->InsertAftCell) break;
          if (l<TotalLines) InsLine=l;
       }
       CurLine=InsLine;
    }

    rtf->InsertBefCell=rtf->InsertAftCell=0;


    if ((RtfInput>=RTF_CB && RtfInput!=RTF_CB_APPEND) || InsLine!=CurLine) {// insert text line
       MoveLineArrays(w,InsLine,1,'B');// insert a line before the current line
    }
    else {                             // append text line
       TotalLines++;
       InsLine=CurLine=TotalLines-1;
       InitLine(w,InsLine);
    }

    rtf->InsLine=InsLine;

    return InsLine;
}

/*****************************************************************************
    InitRtfGroup:
    Setup the group that begins an RTF group.
******************************************************************************/
InitRtfGroup(PTERWND w,struct StrRtf far *rtf, int RtfGroup)
{
    struct StrRtfGroup far *pRtfGroup=&(rtf->group[RtfGroup]);
    int i;

    pRtfGroup->RtfGroup=RtfGroup;

    pRtfGroup->MaxRtfFonts=200;       // allocate for 200 fonts to start
    if (NULL==(pRtfGroup->font=(struct StrRtfFont huge *)OurAlloc(sizeof(struct StrRtfFont)*(long)pRtfGroup->MaxRtfFonts))) {
       return PrintError(w,MSG_OUT_OF_MEM,"InitRtfGroup(font)");
    }
    for (i=0;i<pRtfGroup->MaxRtfFonts;i++) FarMemSet(&(pRtfGroup->font[i]),0,sizeof(struct StrRtfFont));  // initialize with zeros

    // allocate color table
    if (NULL==(pRtfGroup->color=(struct StrRtfColor far *)OurAlloc(sizeof(struct StrRtfColor)*MaxRtfColors))) {
       return PrintError(w,MSG_OUT_OF_MEM,"InitRtfGroup(color)");
    }
    FarMemSet(pRtfGroup->color,0,sizeof(struct StrRtfColor)*MaxRtfColors);  // initialize with zeros

    // allocate the style item translation table
    pRtfGroup->MaxRtfSID=200;    // initially allocate the table for 200 entires
    if (NULL==(pRtfGroup->RtfSID=OurAlloc(sizeof(int)*pRtfGroup->MaxRtfSID))) {
       return PrintError(w,MSG_OUT_OF_MEM,"InitRtfGroup(color)");
    }

    for (i=0;i<pRtfGroup->MaxRtfSID;i++) pRtfGroup->RtfSID[i]=-1;   // holds rtf to doc style id table

    // set stylesheet index
    for (i=0;i<TotalSID;i++) StyleId[i].RtfIndex=-1;

    return TRUE;
}

/*****************************************************************************
    ExitRtfGroup:
    exit logic for the group that begins an RTF group.
******************************************************************************/
ExitRtfGroup(PTERWND w,struct StrRtf far *rtf, int CurGroup)
{
    struct StrRtfGroup far *pRtfGroup;
    int i,RtfIdx;

    if (rtf->group[CurGroup].RtfGroup!=CurGroup) return TRUE;   // not an RTF segement group

    pRtfGroup=&(rtf->group[CurGroup]);

    // Do rtf group ending tasks

    // check if  NextStyId needs to be adjusted
    for (i=0;i<TotalSID;i++) {
       if (StyleId[i].RtfIndex==-1) continue;   // this style was not created in this rtf group
       if (StyleId[i].next==0) continue; // does not need updating

       RtfIdx=StyleId[i].next;           // this was assigned as rtf index
       if (RtfIdx>=pRtfGroup->MaxRtfSID) StyleId[i].next=0;
       else                              StyleId[i].next=pRtfGroup->RtfSID[RtfIdx];
    } 



    OurFree(pRtfGroup->font);
    OurFree(pRtfGroup->color);
    OurFree(pRtfGroup->RtfSID);

    return TRUE;
}

/*****************************************************************************
    GetRtfGroup:
    Get the pointer to the \rtf group
******************************************************************************/
struct StrRtfGroup far *GetRtfGroup(PTERWND w,struct StrRtf far *rtf)
{
    int RtfGroup,CurGroup;

    CurGroup=rtf->GroupLevel;
    RtfGroup=rtf->group[CurGroup].RtfGroup;
    return &(rtf->group[RtfGroup]);
}

/*****************************************************************************
    SetRtfTableInfo:
    Set table info beforee inserting the text
******************************************************************************/
SetRtfTableInfo(PTERWND w, struct StrRtfGroup far *CurGroup)
{
    struct StrRtf far *rtf;

    rtf=CurGroup->rtf;        // pointer to the rtf data structure
       
    // check if the table level changed
    if (rtf->CurTblLevel!=CurGroup->level && !(rtf->flags1&RFLAG1_SSSUBTABLE_USED)) SetRtfTblLevel(w,rtf,rtf->GroupLevel,CurGroup->level,rtf->CurTblLevel);


    // check if processing a table
    if (CurGroup->InTable && RtfCurRowId<=0 && rtf->OpenRowId>0) { // begin a table row
       RtfCurRowId=rtf->OpenRowId;       // make use of the previously opened row
       RtfCurCellId=rtf->OpenCellId;
       RtfLastCellX=rtf->OpenLastCellX;
    }
       
    if (!CurGroup->InTable) {   // not in table
       // delete any empty row
       int CurRowId=RtfCurRowId;
       if (CurRowId>0) {                      // keep track of this open row
          rtf->OpenRowId=RtfCurRowId;
          rtf->OpenCellId=RtfCurCellId;
          rtf->OpenLastCellX=RtfLastCellX;
       }

       // reset row and cell ids
       RtfCurRowId=0;
       RtfCurCellId=0;
       RtfLastCellX=0;
    }

    if (CurGroup->InTable && RtfCurCellId>0 && CellAux[RtfCurCellId].flags&CAUX_CLOSED && rtf->OutBuf[0]!=ROW_CHAR) {  // cell closed
       // define the next cell, if not already defined
       if (cell[RtfCurCellId].NextCell<=0 && !CreateRtfCell(w,rtf,rtf->group,rtf->GroupLevel)) return FALSE;
       RtfCurCellId=cell[RtfCurCellId].NextCell;
    } 

    if (rtf->CurTblLevel==1 && CurGroup->InTable && RtfCurCellId==0 && rtf->OutBuf[0]!=ROW_CHAR) {  // cell data found without table definition
       if (RtfCurRowId==0) SetRtfRowDefault(w,rtf,rtf->group,rtf->GroupLevel);  // row not yet defined
       if (!CreateRtfCell(w,rtf,rtf->group,rtf->GroupLevel)) return FALSE;      // cell not yet defined
    } 


    if (RtfCurRowId>0) {
       ResetUintFlag(TableAux[RtfCurRowId].flags,TAUX_RTF_DEL);  // this row has some text
       TableAux[RtfCurRowId].flags|=TAUX_HAS_NEW_TEXT;
    }
       
    if (!(rtf->TableRead)) rtf->InsertBefCell=0;   // not inserting a table column
    if (rtf->InsertBefCell>0 && RtfCurCellId>0 && cell[rtf->InsertBefCell].level<cell[RtfCurCellId].level) rtf->InsertBefCell=0;  // not inserting a table column

    return TRUE;
}

/*****************************************************************************
    HideHiddenParaMarkers:
    Remove hidden paramarkers when paragraph has visible text as well.
******************************************************************************/
HideHiddenParaMarkers(PTERWND w)
{
    int font,len,ParaId;
    long l;
    BOOL AllHiddenText,ConnectPara;
    LPBYTE ptr,ptr1;
    BYTE LastChar;
      
    AllHiddenText=TRUE;

    for (l=0;l<(TotalLines-1);l++) {
       len=LineLen(l);
       if (len==0) continue;

       if (AllHiddenText && !IsHiddenLine(w,l)) AllHiddenText=FALSE;
     
       ptr=pText(l);
       if (ptr[len-1]!=ParaChar) {
           if (IsBreakChar(w,ptr[len-1])) AllHiddenText=TRUE;  // reset
           continue;
       }

       // check if this paragrah marker needs to be hidden
       font=GetCurCfmt(w,l,len-1);
       if (!AllHiddenText && TerFont[font].style&HIDDEN 
          && cid(l)==cid(l+1) && fid(l)==fid(l+1)) {
          
          ConnectPara=TRUE;

          if (LineLen(l+1)>=0) {
            ptr1=pText(l+1);
            if (IsHdrFtrChar(ptr1[0])) ConnectPara=FALSE;
            if (ptr1[LineLen(l+1)-1]==SECT_CHAR) ConnectPara=FALSE;
          }

          if (ConnectPara) {   
             pText(l)[len-1]=HPARA_CHAR; // replace ParaChar by hidden para char
             SetTag(w,l,len-1,TERTAG_HPARA,"HPARA",NULL,pfmt(l+1));   // save the next para id;
             
             ParaId=pfmt(l);             // apply the current para id to the new paragraph being appended
             ResetLongFlag(LineFlags(l),LFLAG_PARA);
             ResetLongFlag(LineFlags(l+1),LFLAG_PARA_FIRST);

             // set the para id for the next paragraph
             for (l++;l<TotalLines;l++) {
                len=LineLen(l);
                if (len<=0) continue;

                pfmt(l)=ParaId;
                ChangeLineTextStyle(w,l,PfmtId[ParaId].StyId);

                LastChar=pText(l)[len-1];
                if (IsBreakChar(w,LastChar)) break;
             } 
          }
       }   
       AllHiddenText=TRUE; 
    }
      
    return TRUE;
}
 
/*****************************************************************************
    ReadRtfPicture:
    Read the RTF picture data. This function return 0 when sucessful, otherwise
    it returns an error code (RTF_FILE_INCOMPLETE or RTF_SYNTAX_ERROR)
******************************************************************************/
ReadRtfPicture(PTERWND w,struct StrRtf far *rtf)
{
    int    i,k,pict,len,ControlGroupLevel=0,HdrSize;
    long   l,BinaryDataSize=0,ImageSize;
    LPBYTE  ptr;
    METAHEADER hdr;           // metafile header
    BOOL   PictureDataRead=FALSE,SkipPicture=FALSE,binary=FALSE;
    BYTE huge *pMem;
    struct StrRtfPict pic;
    BITMAPINFOHEADER BmHdr;
    BITMAP bm;
    HBITMAP hBM;

    ControlGroupLevel=rtf->GroupLevel;
    FarMemSet(&pic,0,sizeof(pic));  // initialize
    pic.type=-1;                    // set to no type

    while (TRUE) {
       // get next word
       if (!GetRtfWord(w,rtf)) return RTF_FILE_INCOMPLETE;

       if (rtf->GroupBegin) continue;
       if (rtf->GroupEnd) {
          if (rtf->GroupLevel<ControlGroupLevel) break;    // font group ended
          else                                   continue; // look for next font
       }

       if (SkipPicture || PictureDataRead) continue;

       // process the control word
       if (rtf->IsControlWord) {
          if (lstrcmpi(rtf->CurWord,"wmetafile")==0) {
             if ((int)rtf->IntParam==7) pic.type=PICT_META7;    // isotropic
             else                       pic.type=PICT_METAFILE; // anisotropic
          }
          else if (lstrcmpi(rtf->CurWord,"emfblip")==0 && Win32) pic.type=PICT_ENHMETAFILE;
          else if (lstrcmpi(rtf->CurWord,"dibitmap")==0) pic.type=PICT_DIBITMAP;    // DI bitmap picture
          else if (lstrcmpi(rtf->CurWord,"wbitmap")==0)  pic.type=PICT_BITMAP;    // bitmap picture
          else if (lstrcmpi(rtf->CurWord,"jpegblip")==0) pic.type=PICT_JPEG;    // bitmap picture
          else if (lstrcmpi(rtf->CurWord,"pngblip")==0)  {
             if (TerFlags3&TFLAG3_READ_PNG) pic.type=PICT_PNG;
             else                           rtf->flags1|=RFLAG1_PNG_FOUND;    // png found, but not supported
          }
          else if (lstrcmpi(rtf->CurWord,"ssctl")==0)    pic.type=PICT_CONTROL;    // embedded control
          else if (lstrcmpi(rtf->CurWord,"picw")==0)     pic.bmWidth=(rtf->IntParam);   // picture pic.width in twips
          else if (lstrcmpi(rtf->CurWord,"pich")==0)     pic.bmHeight=(rtf->IntParam);  // picture pic.height in twips
          else if (lstrcmpi(rtf->CurWord,"picwgoal")==0) pic.width=(rtf->IntParam);   // picture pic.width in twips
          else if (lstrcmpi(rtf->CurWord,"piccropl")==0) pic.CropLeft=(rtf->IntParam);   // cropping in twips
          else if (lstrcmpi(rtf->CurWord,"piccropr")==0) pic.CropRight=(rtf->IntParam);   // cropping in twips
          else if (lstrcmpi(rtf->CurWord,"pichgoal")==0) pic.height=(rtf->IntParam);  // picture pic.height in twips
          else if (lstrcmpi(rtf->CurWord,"piccropt")==0) pic.CropTop=(rtf->IntParam);  // cropping in twips
          else if (lstrcmpi(rtf->CurWord,"piccropb")==0) pic.CropBot=(rtf->IntParam);  // cropping in twips
          else if (lstrcmpi(rtf->CurWord,"picscalex")==0) pic.ScaleX=(rtf->IntParam); // scaling for display in x direction
          else if (lstrcmpi(rtf->CurWord,"picscaley")==0) pic.ScaleY=(rtf->IntParam); // scaling for display in y direction
          else if (lstrcmpi(rtf->CurWord,"wbmbitspixel")==0) pic.BitsPerPixel=(int)(rtf->IntParam);
          else if (lstrcmpi(rtf->CurWord,"wbmplanes")==0) pic.planes=(int)(rtf->IntParam);
          else if (lstrcmpi(rtf->CurWord,"wbmwidthbytes")==0) pic.WidthBytes=(int)(rtf->IntParam);
          else if (lstrcmpi(rtf->CurWord,"sspicalign")==0) pic.align=(int)(rtf->IntParam); // picture alignment, Sub Systems extension
          else if (lstrcmpi(rtf->CurWord,"bin")==0) {  // picture data in binary
              BinaryDataSize=(long)rtf->DoubleParam;
              binary=TRUE;
              rtf->WordLen=0;     // discard current control character
              goto READ_DATA;
          }
          else if (rtf->IgnoreText) SkipRtfGroup(w,rtf);

          continue;
       }

       READ_DATA:

       // push the word back to the input stream
       for (k=rtf->WordLen-1;k>=0;k--) {
          rtf->CurChar=rtf->CurWord[k];
          PushRtfChar(w,rtf);
       }

       // check for supported picture type
       if (pic.type==PICT_METAFILE || pic.type==PICT_META7) {  // retrieve metafile picture data
           struct StrMetaFileHdr FileHdr;

           // check if metafile header included
           DWORD sign;
           len=sizeof(sign);
           ptr=(LPBYTE)&sign;
           for (i=0;i<len;i++) {
               if      (binary  && !GetRtfChar(w,rtf)) return RTF_FILE_INCOMPLETE;
               else if (!binary && !GetRtfHexChar(w,rtf)) return RTF_FILE_INCOMPLETE; // read in hex format
               ptr[i]=rtf->CurChar;
           }
           if (sign==0x9ac6cdd7) {  // read the file header
              FileHdr.sign=sign;
              ptr=(LPBYTE)&FileHdr;
              ptr=&(ptr[4]);        // go past the sign;
              len=sizeof(FileHdr)-4;   // sign already read
              for (i=0;i<len;i++) {
                  if      (binary  && !GetRtfChar(w,rtf)) return RTF_FILE_INCOMPLETE;
                  else if (!binary && !GetRtfHexChar(w,rtf)) return RTF_FILE_INCOMPLETE; // read in hex format
                  ptr[i]=rtf->CurChar;
              }
              // prepare to read meta header
              len=sizeof(METAHEADER);
              ptr=(LPBYTE)&hdr;
           }
           else {                 
              hdr.mtType=LOWORD(sign);
              hdr.mtHeaderSize=HIWORD(sign);
              ptr=(LPBYTE)&hdr;
              ptr=&(ptr[4]);    // DWORD already read
              len=sizeof(METAHEADER)-4;
           }  

           // retrieve the meta header
           for (i=0;i<len;i++) {
               if      (binary  && !GetRtfChar(w,rtf)) return RTF_FILE_INCOMPLETE;
               else if (!binary && !GetRtfHexChar(w,rtf)) return RTF_FILE_INCOMPLETE; // read in hex format
               ptr[i]=rtf->CurChar;
           }
           HdrSize=2*hdr.mtHeaderSize; // convert words to bytes
           pic.ImageSize=2*hdr.mtSize; // convert words to bytes

           // Allocate global memory for the metafile
           if ( (NULL==(pMem=OurAlloc(pic.ImageSize)))) {
              PrintError(w,MSG_OUT_OF_MEM,"ReadRtfPicture");
              SkipPicture=TRUE;
              continue;
           }

           // copy the header
           HugeMove(&hdr,pMem,HdrSize);  // copy the header

           // read the remaining metafile data
           for (l=HdrSize;l<pic.ImageSize;l++) {
               if      (binary  && !GetRtfChar(w,rtf)) return RTF_FILE_INCOMPLETE;
               else if (!binary && !GetRtfHexChar(w,rtf)) {
                  if (rtf->CurChar=='}') {
                     PushRtfChar(w,rtf);
                     break;
                  }
                  else return RTF_FILE_INCOMPLETE;   // read in hex format
               }
               pMem[l]=rtf->CurChar;
           }
           if (l<pic.ImageSize) pic.ImageSize=l;     // adjust the image size

           // create metafile
           if (NULL==(pic.hMeta=TerSetMetaFileBits(pMem))) {
              PrintError(w,MSG_ERR_META_CREATE,"ReadRtfPicture");
              break;
           }
       }
       #if defined(WIN32)
       else if (pic.type==PICT_ENHMETAFILE) {  // retrieve enh metafile picture data
           ENHMETAHEADER emh;

           // retrieve the metafile header
           len=sizeof(emh);
           ptr=(LPBYTE)&emh;
           for (i=0;i<len;i++) {
               if      (binary  && !GetRtfChar(w,rtf)) return RTF_FILE_INCOMPLETE;
               else if (!binary && !GetRtfHexChar(w,rtf)) return RTF_FILE_INCOMPLETE; // read in hex format
               ptr[i]=rtf->CurChar;
           }
           HdrSize=emh.nSize; // convert words to bytes
           pic.ImageSize=emh.nBytes;

           // Allocate global memory for the metafile
           if ( (NULL==(pMem=(LPVOID)OurAlloc(pic.ImageSize)))) {
              PrintError(w,MSG_OUT_OF_MEM,"ReadRtfPicture");
              SkipPicture=TRUE;
              continue;
           }

           // copy the header
           HugeMove(&emh,pMem,i);  // copy the header

           // read the remaining metafile data
           for (l=i;l<pic.ImageSize;l++) {
               if      (binary  && !GetRtfChar(w,rtf)) return RTF_FILE_INCOMPLETE;
               else if (!binary && !GetRtfHexChar(w,rtf)) {
                  if (rtf->CurChar=='}') {
                     PushRtfChar(w,rtf);
                     break;
                  }
                  else return RTF_FILE_INCOMPLETE;   // read in hex format
               }
               pMem[l]=rtf->CurChar;
           }
           if (l<pic.ImageSize) {       // update the header
               // adjust the image size
               emh.nSize = l;
               pic.ImageSize=l;
               // recopy the header
               HugeMove(&emh,pMem,HdrSize);  // copy the header
           }

           // create metafile
           if (NULL==(pic.hEnhMeta=TerSetEnhMetaFileBits(pMem))) {
              PrintError(w,MSG_ERR_META_CREATE,"ReadRtfPicture a");
              break;
           }
       }
       #endif
       else if (pic.type==PICT_DIBITMAP) {// retrieve DI bitmap picture data
           // retrieve the bitmap info header
           len=sizeof(BITMAPINFOHEADER);
           ptr=(LPBYTE)&BmHdr;
           for (i=0;i<len;i++) {
               if      (binary  && !GetRtfChar(w,rtf)) return RTF_FILE_INCOMPLETE;
               else if (!binary && !GetRtfHexChar(w,rtf)) return RTF_FILE_INCOMPLETE; // read in hex format
               ptr[i]=rtf->CurChar;
           }

           //************ calculate image and info structure sizes **
           if (BmHdr.biClrUsed>0 && BmHdr.biBitCount<24) {
              pic.InfoSize=len+(BmHdr.biClrUsed)*sizeof(RGBQUAD);
           }
           else {
              if      (BmHdr.biBitCount==1) pic.InfoSize=len+2*sizeof(RGBQUAD);
              else if (BmHdr.biBitCount==4) pic.InfoSize=len+16*sizeof(RGBQUAD);
              else if (BmHdr.biBitCount==8) pic.InfoSize=len+256*sizeof(RGBQUAD);
              else                          pic.InfoSize=len;  // 24 bit colors
           }

           pic.ImageSize=BmHdr.biSizeImage;
           if (pic.ImageSize==0 && BmHdr.biCompression==BI_RGB) {
              pic.ImageSize=(BmHdr.biWidth*BmHdr.biBitCount+31)/32; // scan line width in number of DWORDs
              pic.ImageSize*=BmHdr.biHeight*4; // image width in number of bytes
           }

           // read picture info block
           if ( (NULL==(pic.pInfo=OurAlloc(pic.InfoSize)))) {
              PrintError(w,MSG_OUT_OF_MEM,"ReadRtfPicture(hInfo)");
              SkipPicture=TRUE;
              continue;
           }
           pMem=(BYTE huge *)pic.pInfo;
           HugeMove(&BmHdr,pMem,len);  // copy the header

           for (l=len;l<pic.InfoSize;l++) {
               if      (binary  && !GetRtfChar(w,rtf)) return RTF_FILE_INCOMPLETE;
               else if (!binary && !GetRtfHexChar(w,rtf)) return RTF_FILE_INCOMPLETE;  // read in hex format
               pMem[l]=rtf->CurChar;
           }

           // read picture data block
           if ( (NULL==(pic.pImage=OurAlloc(pic.ImageSize)))) {
              OurPrintf("%ld",pic.ImageSize);
              PrintError(w,MSG_OUT_OF_MEM,"ReadRtfPicture(pImage)");
              SkipPicture=TRUE;
              continue;
           }

           pMem=(BYTE huge *)pic.pImage;

           for (l=0;l<pic.ImageSize;l++) {
               if      (binary  && !GetRtfChar(w,rtf)) return RTF_FILE_INCOMPLETE;
               else if (!binary && !GetRtfHexChar(w,rtf)) return RTF_FILE_INCOMPLETE;  // read in hex format
               pMem[l]=rtf->CurChar;
           }

           if (pic.width==0) pic.width=ScrToTwipsX(pic.bmWidth);
           if (pic.height==0) pic.height=ScrToTwipsX(pic.bmHeight);
       }
       else if (pic.type==PICT_BITMAP) {// retrieve bitmap picture data

           // read picture data block
           ImageSize=pic.WidthBytes*pic.bmHeight;
           if ( (NULL==(pMem=(LPVOID)OurAlloc(ImageSize)))) {
              OurPrintf("%ld",ImageSize);
              PrintError(w,MSG_OUT_OF_MEM,"ReadRtfPicture(hImage)");
              SkipPicture=TRUE;
              continue;
           }
           for (l=0;l<ImageSize;l++) {
               if      (binary  && !GetRtfChar(w,rtf)) return RTF_FILE_INCOMPLETE;
               else if (!binary && !GetRtfHexChar(w,rtf)) return RTF_FILE_INCOMPLETE;  // read in hex format
               pMem[l]=rtf->CurChar;
           }

           // create a bitmap handle
           FarMemSet(&bm,0,sizeof(bm));
           bm.bmWidth=(int)pic.bmWidth;
           bm.bmHeight=(int)pic.bmHeight;
           bm.bmWidthBytes=pic.WidthBytes;
           bm.bmPlanes=pic.planes;
           bm.bmBitsPixel=pic.BitsPerPixel;
           bm.bmBits=pMem;

           if (NULL==(hBM=CreateBitmapIndirect(&bm))) {
              PrintError(w,MSG_ERR_BMP_CREATE,"ReadRtfPicture(hBM)");
              SkipPicture=TRUE;
              continue;
           }
           OurFree(pMem);

           if ((pict=Bitmap2DIB(w,hBM))==-1) {  // create a device independent bitmap
              PrintError(w,MSG_ERR_BMP_CREATE,"ReadRtfPicture(pict)");
              SkipPicture=TRUE;
              continue;
           }

           // move the values into the pic structure and treat it as DIB now
           pic.type=PICT_DIBITMAP;
           pic.InfoSize=TerFont[pict].InfoSize;
           pic.pInfo=TerFont[pict].pInfo;
           TerFont[pict].pInfo=NULL;              // pointer transferred
           pic.ImageSize=TerFont[pict].ImageSize;
           pic.pImage=TerFont[pict].pImage;
           TerFont[pict].pImage=NULL;             // pointer transfereed

           if (pic.width==0) pic.width=ScrToTwipsX((int)(pic.bmWidth));
           if (pic.height==0) pic.height=ScrToTwipsY((int)(pic.bmHeight));

           // free the resources
           InitTerObject(w,pict);                 // temporary object
           DeleteObject(hBM);
       }
       else if (pic.type==PICT_JPEG || pic.type==PICT_PNG) {           // retrieve the jpeg image
           long BufLen=0;                        // initial buffer size
           long increment=8192;
           HGLOBAL hDIB,hImage;

           ImageSize=0;                          // number of bytes read
           hImage=0;

           // read picture data block
           while (TRUE) {
               if      (binary  && !GetRtfChar(w,rtf)) break;
               else if (!binary && !GetRtfHexChar(w,rtf)) break; // end of picture data

               if (ImageSize>=BufLen) {         // enlarge the buffer
                  if (hImage) GlobalUnlock(hImage);
                  if (hImage) hImage=GlobalReAlloc(hImage,BufLen+increment,GMEM_MOVEABLE);
                  else        hImage=GlobalAlloc(GMEM_MOVEABLE,increment);
                  if (!hImage || (NULL==(pMem=(BYTE huge *)GlobalLock(hImage))) ){
                     OurPrintf("%ld",ImageSize);
                     PrintError(w,MSG_OUT_OF_MEM,"ReadRtfPicture(hImage)");
                     SkipPicture=TRUE;
                     break;;
                  }
                  BufLen+=increment;
               }
               pMem[ImageSize]=rtf->CurChar;
               ImageSize++;
           }
           if (SkipPicture) continue;

           PushRtfChar(w,rtf);   // push the last unused character

           // convert jpeg to DIB
           GlobalUnlock(hImage);
           
           ImgDenX=ScrResX;     // pixel densities - these variables are set by the image import functions
           ImgDenY=ScrResY;

           if (pic.type==PICT_JPEG) {
              if (!Jpg2Bmp(w,TER_BUF,"",hImage,ImageSize,&hDIB,TRUE) || !hDIB) {
                 SkipPicture=TRUE;
                 continue;
              }
           }
           else {            // translate png
              if (!Png2Bmp(w,TER_BUF,"",hImage,ImageSize,&hDIB) || !hDIB) {
                 SkipPicture=TRUE;
                 continue;
              }
           } 

           if ((pict=ExtractDIB(w,hDIB))==-1) {// create a device independent bitmap
               SkipPicture=TRUE;
               continue;
           }
           GlobalFree(hDIB);                   // not needed any more

           // move the values into the pic structure and treat it as DIB now
           pic.OrigPictType=pic.type;          // save the original picture type

           pic.type=PICT_DIBITMAP;
           pic.InfoSize=TerFont[pict].InfoSize;
           pic.pInfo=TerFont[pict].pInfo;
           TerFont[pict].pInfo=NULL;              // pointer transferred
           pic.ImageSize=TerFont[pict].ImageSize;
           pic.pImage=TerFont[pict].pImage;
           TerFont[pict].pImage=NULL;             // pointer transfereed
           pic.bmWidth=TerFont[pict].bmWidth;
           pic.bmHeight=TerFont[pict].bmHeight;

           if (pic.width==0) {
              pic.width=ScrToTwipsX((int)(pic.bmWidth));
              if (ImgDenX!=ScrResX) pic.width=MulDiv(pic.width,ScrResX,ImgDenX);
           }
           if (pic.height==0) {
              pic.height=ScrToTwipsY((int)(pic.bmHeight));
              if (ImgDenY!=ScrResY) pic.height=MulDiv(pic.height,ScrResY,ImgDenY);
           }

           // copy the original picture data
           pic.pOrigImage=OurAlloc(ImageSize+1);
           if (pic.pOrigImage) {
              pic.OrigImageSize=ImageSize;
              pMem=GlobalLock(hImage);
              FarMove(pMem,pic.pOrigImage,ImageSize);
              GlobalUnlock(hImage);
           }
           GlobalFree(hImage);

           // free the resources
           InitTerObject(w,pict);                 // temporary object
       }
       else if (pic.type==PICT_CONTROL) { // embedded control
           // read picture info block
           pic.InfoSize=sizeof(struct StrControl);
           if ( (NULL==(pic.pInfo=OurAlloc(pic.InfoSize)))) {
              PrintError(w,MSG_OUT_OF_MEM,"ReadRtfPicture(hInfo)");
              SkipPicture=TRUE;
              continue;
           }
           pMem=(BYTE huge *)pic.pInfo;
           for (l=0;l<pic.InfoSize;l++) {
               if      (binary  && !GetRtfChar(w,rtf)) return RTF_FILE_INCOMPLETE;
               else if (!binary && !GetRtfHexChar(w,rtf)) return RTF_FILE_INCOMPLETE;  // read in hex format
               pMem[l]=rtf->CurChar;
           }
       }
       else {
          SkipPicture=TRUE;               // picture format not supported
          if (binary) for (i=0;i<BinaryDataSize;i++) GetRtfChar(w,rtf); // discard binary characters
          continue;
       }

       PictureDataRead=TRUE;
    }

    // check the picture dimensions
    if (pic.width==0 || pic.height==0) {
       struct StrRtfGroup far *pGroup=&(rtf->group[ControlGroupLevel]);
       if (pGroup->gflags&GFLAG_IN_SHP) {
          pic.width=pGroup->shape.width;
          pic.height=pGroup->shape.height;
       } 
       if (pic.width==0 || pic.height==0) return 0;     // invalid picture
    }
    
    if (!PictureDataRead) return 0;          // picture data missing
    if (pic.type!=PICT_METAFILE && pic.type!=PICT_META7 && pic.type!=PICT_ENHMETAFILE
       && pic.type!=PICT_DIBITMAP && pic.type!=PICT_CONTROL) return 0;  // unsupported format

    // pass the picture to the application
    if (True(rtf->group[rtf->GroupLevel].shape.FrmFlags&PARA_FRAME_FILL_PICT)) {  // picture to fill the shape
       int pict;
       if ((pict=FindOpenSlot(w))==-1) return 0;  // ran out of picture table
       if (!BuildRtfPicture(w,&(rtf->group[rtf->GroupLevel]),&pic,pict)) return FALSE;
       rtf->group[rtf->GroupLevel].shape.FillPict=pict;
    } 
    else ImportRtfData(w,RTF_PICT,&(rtf->group[rtf->GroupLevel]),NULL,(LPBYTE)&pic);

    rtf->flags=ResetUintFlag(rtf->flags,RFLAG_READ_SHAPE_PICT);  // read read

    return 0;                 // successful
}

/*****************************************************************************
    InitGroupForStyle:
    Initialize the current group when starting a new stylesheet item.
******************************************************************************/
InitGroupForStyle(PTERWND w,struct StrRtf far *rtf)
{
    struct StrRtfGroup far *group;

    // get the group pointer
    group=&(rtf->group[rtf->GroupLevel]);

    group->ParaStyId=0;
    group->CharStyId=1;

    // assign other style member variables
    lstrcpy(group->TypeFace,"");         // font type face
    group->PointSize2=0;                 // point size
    group->style=0;                      // character style bits
    group->TextColor=TerFont[0].TextColor; // text color
    group->TextBkColor=CLR_WHITE;        // text background color
    group->UlineColor=CLR_AUTO;          // underline color
    group->LeftIndent=0;                 // left indent in twips
    group->RightIndent=0;                // Right indent in twips
    group->FirstIndent=0;                // First line indent in twips
    group->flags=0;                      // includes such flags as double spacing,centering, right justify etc
    group->ParShading=0;                 // shading (0 to 10000)
    group->SpaceBefore=0;                // space before the paragraph in twips
    group->SpaceAfter=0;                 // space after the paragraph in twips
    group->SpaceBetween=0;               // space between the paragraph lines in twips
    group->LineSpacing=0;                // space between the paragraph lines in twips
    group->ParaBkColor=CLR_WHITE;        // paragraph background color
    group->ParaBorderColor=CLR_AUTO;     // paragraph border color

    // initialize the tab id
    group->tab.count=0;

    return TRUE;
}

/****************************************************************************
    TerInsertRtfFile:
    Insert an rtf file
****************************************************************************/
BOOL WINAPI _export TerInsertRtfFile(HWND hWnd,LPBYTE FileName, long line, int col, BOOL repaint)
{
    long BufLen;
    HGLOBAL hMem;
    BYTE huge *pMem;
    BOOL result;

    if (NULL==(hMem=TerFileToMem(FileName,(DWORD far *)&BufLen))) return FALSE;

    if (NULL==(pMem=GlobalLock(hMem))) return FALSE;

    result=InsertRtfBuf(hWnd,pMem,BufLen,line,col,repaint);

    GlobalUnlock(hMem);
    GlobalFree(hMem);

    return result;
}

/****************************************************************************
    InsertRtfBuf:
    Use this function to insert a specified RTF buffer at the specified
    location.  The location can be specified as following:
       1.  line/col -> line and col have 0 base
       2.  Absolute location -> Set col to -1 and specify the absolute
                                location in the line variable.
    The 'repaint' argument should be set to TRUE to refresh the screen.

    This function is valid in 'word wrap' mode only.
****************************************************************************/
BOOL WINAPI _export InsertRtfBuf(HWND hWnd,BYTE huge *buf, long BufLen, long line, int col, BOOL repaint)
{
    PTERWND w;
    BOOL SavePaintEnabled,FirstLineSplit=FALSE;
    BOOL result=FALSE;
    long FirstLine;
    long UndoBegRow,UndoBegPos,UndoEndRow,UndoEndPos;
    int  UndoBegCol,UndoEndCol;
    int  RtfReadMode=RTF_CB;
    BOOL PasteInTable=FALSE;

    w=GetWindowPointer(hWnd);           // get the pointer to window data
    
    // get line and column number
    if (line<0) {                       // use the current position
       if (line==-2) RtfReadMode=RTF_CB_APPEND;
       line=CurLine;
       col=CurCol;
    }
    else {
       if (col<0) AbsToRowCol(w,line,&line,&col);  // calculate row/col from absolute location
       if (line>=TotalLines) {
          line=TotalLines-1;
          col=LineLen(line);
          if (TerArg.WordWrap) col--;
          if (col<0) col=0;
       }
       if (line<0) line=0;
       if (col>LineLen(line)) col=LineLen(line);
       if (col<0) col=0;
    }
    
    // adjust insertion col
    if (TerOpFlags2&TOFLAG2_FROM_CB && ClipTblLevel>0) {  // paste data contain table
       int CurFont=GetCurCfmt(w,line,col);
       if (TerFont[CurFont].FieldId==FIELD_DATA) {       // position before the field name
          if (TerLocateFieldChar(hTerWnd,FIELD_DATA,NULL,FALSE,&line,&col,FALSE)) {
             if (TerLocateFieldChar(hTerWnd,FIELD_NAME,NULL,FALSE,&line,&col,FALSE)) {
                NextTextPos(w,&line,&col);      // position at the first character of the field name
             }
          } 
       
       } 
    } 

    CurLine=line;
    CurCol=col;


    if (RtfReadMode==RTF_CB && cid(CurLine)) {
       PasteInTable=TRUE;    // pasting in table
       //ClipTblLevel=cell[cid(CurLine)].level;  // ClipTblLevel is determined by the StrClipInfo data from clipboard
    }

    // save undo begin position
    if (RtfReadMode==RTF_CB_APPEND) UndoBegPos=RowColToAbs(w,TotalLines-1,LineLen(TotalLines-1)-1);
    else                            UndoBegPos=RowColToAbs(w,CurLine,CurCol);

    // split the current line at the current column
    if (CurCol>0) {
       SavePaintEnabled=PaintEnabled;
       PaintEnabled=FALSE;
       result=TerSplitLine(w,0,TabAlign,TRUE); // split the current line
       PaintEnabled=SavePaintEnabled;
       FirstLineSplit=TRUE;
       if (!result) return FALSE;
    }

    // set the table embedding options
    if (!(TerOpFlags2&TOFLAG2_FROM_CB) && !InUndo) {
       ClipEmbTable=(TerFlags3&TFLAG3_EMBEDDED_TABLES);
       ClipTblLevel=0;
    }

    // insert rtf buffer at the current line/col
    FirstLine=CurLine;
    result=RtfRead(w,RtfReadMode,NULL,buf,BufLen);        // treat the buffer as a clipboard input

    // merge line pieces together in non-wordwrap mode
    if (FirstLine>0 && FirstLineSplit) FirstLine--;
    if (TerArg.WordWrap) WordWrap(w,FirstLine,CurLine-FirstLine+1); // wrap the rtf text
    if (!TerArg.WordWrap && CurLine>FirstLine) {
       MergeRtfLinePieces(w,FirstLine,CurLine); // construct lines
    }

    // record for undo
    AbsToRowCol(w,UndoBegPos,&UndoBegRow,&UndoBegCol);
    UndoEndPos=RowColToAbs(w,CurLine,CurCol);      // absolute ending position
    if (PasteInTable && CurCol==0 && CurLine>0 && cid(CurLine)!=cid(CurLine-1)
       && cid(CurLine-1)>0 && LineInfo(w,CurLine-1,(INFO_CELL|INFO_ROW))) {
       UndoEndPos--;
    }

    AbsToRowCol(w,UndoEndPos-1,&UndoEndRow,&UndoEndCol);

    if (result) SaveUndo(w,UndoBegRow,UndoBegCol,UndoEndRow,UndoEndCol,'I');

    ClipEmbTable=TRUE;                            // reset to initialized value
    ClipTblLevel=1;

    if (repaint) PaintTer(w);

    return result;
}

/*****************************************************************************
    RtfSetLineOrient:
    set the line orientation of dpline.
******************************************************************************/
SetRtfLineOrient(PTERWND w, struct StrRtf far *rtf,BOOL FlipH, BOOL FlipV)
{
    int x1,y1,x2,y2,LineType;

    if (abs(ParaFrame[RtfParaFID].width)<=1) ParaFrame[RtfParaFID].width=0;  // provide tolerance
    if (abs(ParaFrame[RtfParaFID].height)<=1) ParaFrame[RtfParaFID].height=0;  // provide tolerance

    x1=ParaFrame[RtfParaFID].x;
    x2=x1+ParaFrame[RtfParaFID].width;
    y1=ParaFrame[RtfParaFID].ParaY;
    y2=y1+ParaFrame[RtfParaFID].height;

    LinePointsToRect(w,RtfParaFID,x1,y1,x2,y2);

    if (ParaFrame[RtfParaFID].flags&PARA_FRAME_LINE && (FlipH || FlipV)) {
      LineType=ParaFrame[RtfParaFID].LineType;
      if (FlipH) {
        if (LineType==DOL_DIAG)       LineType=DOL_BDIAG;
        else if (LineType==DOL_BDIAG) LineType=DOL_DIAG;
      } 
      if (FlipV) {
        if (LineType==DOL_DIAG)       LineType=DOL_BDIAG;
        else if (LineType==DOL_BDIAG) LineType=DOL_DIAG;
      } 
      ParaFrame[RtfParaFID].LineType=LineType;
    }

    return TRUE;
} 

/*****************************************************************************
    FixNegativeIndents:
    Fix negative left indents on the paragraph. 
    Also fixes the cell margins.
******************************************************************************/
FixNegativeIndents(PTERWND w,struct StrRtf far *rtf)
{
    int i,cl,CurIndent,indent=0,MinMargin;
    long l;
    float MinLeftMarg;

    if (TerFlags4&TFLAG4_DONT_FIX_NEG_INDENT) return TRUE;

    if (TerArg.PageMode && !TerArg.FittedView) {
       for (i=0;i<TotalTableRows;i++) {           // check if any row uses negative indentation greater than the standard Word indentation
          if (!TableRow[i].InUse) continue;
          if (TableRow[i].AddedIndent<108) break;
       }
       
       if (i==TotalTableRows) return TRUE;
    }

    // fix the cell margin 
    for (i=0;i<TotalCells;i++) {
       if (!cell[i].InUse ) continue;

       MinMargin=0;
       if (cell[i].PrevCell>0) {
          if (cell[i].border&BORDER_LEFT && cell[i].margin<cell[i].BorderWidth[BORDER_INDEX_LEFT]) MinMargin=cell[i].BorderWidth[BORDER_INDEX_LEFT]/2; 
          cl=cell[i].PrevCell;
          if (cell[cl].border&BORDER_RIGHT && MinMargin<cell[cl].BorderWidth[BORDER_INDEX_RIGHT]/2) MinMargin=cell[cl].BorderWidth[BORDER_INDEX_RIGHT]/2;
       }
       else if (cell[i].border&BORDER_LEFT) MinMargin=cell[i].BorderWidth[BORDER_INDEX_LEFT];  // this calculation is now done in CreateRtfCell
       
       if (cell[i].NextCell>0) {
          if (cell[i].border&BORDER_RIGHT && cell[i].margin<cell[i].BorderWidth[BORDER_INDEX_RIGHT]) MinMargin=cell[i].BorderWidth[BORDER_INDEX_RIGHT]/2; 
          cl=cell[i].NextCell;
          if (cell[cl].border&BORDER_LEFT && MinMargin<cell[cl].BorderWidth[BORDER_INDEX_LEFT]/2) MinMargin=cell[cl].BorderWidth[BORDER_INDEX_LEFT]/2;
       }
       else if (cell[i].border&BORDER_RIGHT) MinMargin=cell[i].BorderWidth[BORDER_INDEX_RIGHT];  // this calculation is now done in CreateRtfCell

       if (cell[i].margin<MinMargin) cell[i].margin=MinMargin;
    }    

    // find the most negative indent
    for (i=0;i<TotalPfmts;i++) {
       if (PfmtId[i].pflags&(PFLAG_FRAME|PFLAG_CELL)) continue;  // this id used for the frames

       CurIndent=PfmtId[i].LeftIndentTwips;
       if (PfmtId[i].FirstIndentTwips<0) CurIndent+=PfmtId[i].FirstIndentTwips;
       if (CurIndent<indent) indent=CurIndent;
    }

    // check the table indentations
    for (i=0;i<TotalTableRows;i++) {
       int cl;
       if (!TableRow[i].InUse) continue;
       if (TableRow[i].AddedIndent==0) continue;

       cl=TableRow[i].FirstCell;
       if (cell[cl].level>0) continue;  // consider only the top level tables

       CurIndent=-TableRow[i].AddedIndent;
       if (CurIndent<indent) indent=CurIndent;
    } 

    // check the para frame negative margin - when doc left margin not specified explicitly
    if (!(rtf->flags2&RFLAG2_LEFT_MARG_FOUND)) {
       for (i=0;i<TotalParaFrames;i++) {
          if (ParaFrame[i].flags&(PARA_FRAME_CENTER|PARA_FRAME_RIGHT_JUST))  continue;
                
          if (ParaFrame[i].x<indent) indent=ParaFrame[i].x;
       }
    } 

    if (indent>=0) return TRUE;     // no fix needed


    // find min margin
    indent=-indent;                 // make it positive
    MinLeftMarg=TerSect[0].LeftMargin;
    for (i=1;i<TotalSects;i++) {
       if (TerSect[i].LeftMargin<MinLeftMarg) MinLeftMarg=TerSect[i].LeftMargin;
    }
    MinLeftMarg-=(float).25;
    if (MinLeftMarg<(float)0) MinLeftMarg=(float)0;
    if (indent>(int)(MinLeftMarg*1440)) indent=(int)(MinLeftMarg*1440);


    // fix para indent
    for (i=0;i<TotalPfmts;i++) {
       if (PfmtId[i].pflags&(PFLAG_FRAME|PFLAG_CELL)) continue;   // this id used for the frames
       PfmtId[i].LeftIndentTwips+=indent;
       PfmtId[i].LeftIndent=TwipsToScrX(PfmtId[i].LeftIndentTwips);
    }

    // fix the left margin
    for (i=0;i<TotalSects;i++) {
       TerSect[i].LeftMargin=TerSect[i].LeftMargin-TwipsToInches(indent);
       if (TerSect[i].LeftMargin<0) TerSect[i].LeftMargin=(float)0;
    }

    // fix para frame x position
    for (i=0;i<TotalParaFrames;i++) {
       if (ParaFrame[i].flags&PARA_FRAME_CENTER)
             ParaFrame[i].x+=(indent/2);
       else  ParaFrame[i].x+=indent;
    }

    // fix the table indentation
    for (i=0;i<TotalTableRows;i++) {
       int FirstCell=TableRow[i].FirstCell;
       if (TableRow[i].InUse && cell[FirstCell].InUse && cell[FirstCell].level==0) {
          TableRow[i].indent+=(indent-TableRow[i].AddedIndent);
          TableRow[i].AddedIndent=0;
       }
    }

    // fix tab stops
    if (RtfInput<RTF_CB) {
        int CurPara,TabId;
        struct StrTab NewTab;
        struct StrPfmt NewPfmt;

        for (l=0;l<TotalLines;l++) {
           if (cid(l) || fid(l)) continue;
           CurPara=pfmt(l);
           TabId=PfmtId[CurPara].TabId;
           if (TabId==0) continue;

           NewTab=TerTab[TabId];
           for (i=0;i<NewTab.count;i++) NewTab.pos[i]+=indent;
           TabId=NewTabId(w,TabId,&NewTab);    // get new tab id
           
           FarMove(&(PfmtId[pfmt(l)]),&NewPfmt,sizeof(NewPfmt));
           NewPfmt.TabId=TabId;
           pfmt(l)=NewParaId2(w,pfmt(l),&NewPfmt);
        }
    } 

    return TRUE;
}

/*****************************************************************************
    SetRtfTblLevel:
    Set rtf table level
******************************************************************************/
SetRtfTblLevel(PTERWND w,struct StrRtf far *rtf,int CurGroup, int NewLevel,int PrevLevel)
{
    int level,MinLevel;
    struct StrRtfGroup far *group=rtf->group;
 
    if (NewLevel==PrevLevel) return TRUE;

    
    if (NewLevel<PrevLevel) {               // goto the parent cell
       if (rtf->OpenRowId>0) TableAux[rtf->OpenRowId].flags|=TAUX_RTF_DEL;   // delete this row if not data encountered
       if (RtfCurRowId>0) TableAux[RtfCurRowId].flags|=TAUX_RTF_DEL;

       SaveRtfLevelInfo(w,rtf,PrevLevel);

       GetRtfLevelInfo(w,rtf,NewLevel);
       group[CurGroup].level=rtf->CurTblLevel=NewLevel;        
       return TRUE;
    }

    // create nested level
    SaveRtfLevelInfo(w,rtf,rtf->CurTblLevel);      // save the current level information

    MinLevel=RtfInitLevel;
    if (rtf->EmbedTable) MinLevel++;          // lowest level of the text being inserted
    if (PrevLevel<MinLevel) PrevLevel=MinLevel;

    GetRtfLevelInfo(w,rtf,PrevLevel);              // retrieve the parent level information


    for (level=PrevLevel+1;level<=NewLevel;level++) {
       // check if processing a table
       if (group[CurGroup].InTable && RtfCurRowId<=0 && rtf->OpenRowId>0) { // begin a table row
          RtfCurRowId=rtf->OpenRowId;       // make use of the previously opened row
          RtfCurCellId=rtf->OpenCellId;
          RtfLastCellX=rtf->OpenLastCellX;
       }
       
       // create the parent row if not already created
       if (RtfCurRowId<=0 || !(TableAux[RtfCurRowId].flags&(TAUX_TROWD_DONE|TAUX_COPIED))) {   // when pasting column RtfCurRowId might exist but trowd not yet encountered
          rtf->CurTblLevel=level-1;
          SetRtfRowDefault(w,rtf,group,CurGroup);
          CreateRtfCell(w,rtf,group,CurGroup);
       }

       // create new parent cell if the previous cell already closed.
       if (RtfCurCellId>0 && CellAux[RtfCurCellId].flags&CAUX_CLOSED) {  // cell closed
          // define the next cell, if not already defined since this is going to be a parent cell
          if (cell[RtfCurCellId].NextCell<=0 && !CreateRtfCell(w,rtf,rtf->group,rtf->GroupLevel)) return FALSE;
          RtfCurCellId=cell[RtfCurCellId].NextCell;
       } 
       
       SaveRtfLevelInfo(w,rtf,level-1);  // save the parent table information

       // set new level
       //rtf->TblLevel[level].ParentCell=RtfCurCellId;  // parent cell for  this subtable
       group[CurGroup].level=rtf->CurTblLevel=level;        // increase the table level for the group
    
       RtfCurRowId=RtfCurCellId=RtfLastCellX=0;

       rtf->OpenRowId=rtf->OpenCellId=rtf->OpenLastCellX=0;
       rtf->InitTblCol=-1;
       rtf->PastingColumn=FALSE;

       // create first row and first cell for this level
       if (!SetRtfRowDefault(w,rtf,group,CurGroup)) return FALSE;
       if (!CreateRtfCell(w,rtf,group,CurGroup)) return RTF_SYNTAX_ERROR;
       rtf->group[CurGroup].InTable=true;
    }

    return TRUE;
}

/*****************************************************************************
    SaveRtfLevelInfo:
    Save the table level information to the level table
******************************************************************************/
SaveRtfLevelInfo(PTERWND w,struct StrRtf far *rtf,int level)
{
    rtf->TblLevel[level].CurRowId=RtfCurRowId;     // save the parent table information
    rtf->TblLevel[level].CurCellId=RtfCurCellId;
    rtf->TblLevel[level].LastCellX=RtfLastCellX;
    rtf->TblLevel[level].OpenRowId=rtf->OpenRowId;
    rtf->TblLevel[level].OpenCellId=rtf->OpenCellId;
    rtf->TblLevel[level].OpenLastCellX=rtf->OpenLastCellX;
    rtf->TblLevel[level].InitTblCol=rtf->InitTblCol;
    rtf->TblLevel[level].PastingColumn=rtf->PastingColumn;
 
    return TRUE;
} 

/*****************************************************************************
    GetRtfLevelInfo:
    Retrieve the table level information to the level table
******************************************************************************/
GetRtfLevelInfo(PTERWND w,struct StrRtf far *rtf,int level)
{
    RtfCurRowId=rtf->TblLevel[level].CurRowId; 
    RtfCurCellId=rtf->TblLevel[level].CurCellId;
    RtfLastCellX=rtf->TblLevel[level].LastCellX;
    rtf->OpenRowId=rtf->TblLevel[level].OpenRowId;
    rtf->OpenCellId=rtf->TblLevel[level].OpenCellId;
    rtf->OpenLastCellX=rtf->TblLevel[level].OpenLastCellX;
    rtf->InitTblCol=rtf->TblLevel[level].InitTblCol;
    rtf->PastingColumn=rtf->TblLevel[level].PastingColumn;
 
    rtf->CurTblLevel=level;

    return TRUE;
} 

/*****************************************************************************
    ReadRtfBullet:
    Read the paragraph bullet control.
******************************************************************************/
ReadRtfBullet(PTERWND w,struct StrRtf far *rtf)
{
    int    ControlGroupLevel=0,FontId,CurFont,CurGroup;
    struct StrRtfGroup far *grp;
    struct StrRtfGroup far *group;
    BOOL   pntxtbFound=FALSE,pntxtaFound=FALSE;

    ControlGroupLevel=rtf->GroupLevel;
    grp=&(rtf->group[ControlGroupLevel-1]);
    group=rtf->group;

    FarMemSet(&(grp->blt),0,sizeof(struct StrBlt));
    grp->blt.NumberType=NBR_DEFAULT;     // not yet assigned

    while (TRUE) {
       // get color number
       if (!GetRtfWord(w,rtf)) return RTF_FILE_INCOMPLETE;

       if (rtf->GroupBegin) continue;
       if (rtf->GroupEnd) {
          if (rtf->GroupLevel<ControlGroupLevel) break;    // color group ended
          else                                   continue; // look for next color
       }

       if (rtf->IsControlWord) {
         grp->flags|=BULLET;
         pntxtbFound=pntxtaFound=FALSE;

         if (lstrcmpi(rtf->CurWord,"pnlvlblt")==0 || (lstrcmpi(rtf->CurWord,"pnlvl")==0 && (int)rtf->IntParam==11))  grp->blt.IsBullet=TRUE;  // bullet found
         else if (lstrcmpi(rtf->CurWord,"pnlvl")==0) grp->blt.level=(int)rtf->IntParam;
         else if (lstrcmpi(rtf->CurWord,"pnlvlcont")==0) grp->blt.flags|=BLTFLAG_HIDDEN;
         else if (lstrcmpi(rtf->CurWord,"pnstart")==0) grp->blt.start=(int)rtf->IntParam;

         else if (lstrcmpi(rtf->CurWord,"pnindent")==0 && FALSE ) {  // not used since we now support lists
            if (grp->LeftIndent<(int)rtf->IntParam) {
               grp->LeftIndent=(int)rtf->IntParam;
               grp->FirstIndent=-(int)rtf->IntParam;
            }
            if (grp->FirstIndent==0) {
               grp->LeftIndent+=(int)rtf->IntParam;
               grp->FirstIndent=-(int)rtf->IntParam;
            }
         }

         else if (lstrcmpi(rtf->CurWord,"pnf")==0) {
            struct StrRtfGroup far *pRtfGroup=GetRtfGroup(w,rtf);

            FontId=(int)rtf->IntParam;
            if (FontId>=0 && FontId<MAX_RTF_SEQ_FONTS) CurFont=FontId;  // lower font ids are stored sequentially
            else for (CurFont=MAX_RTF_SEQ_FONTS;CurFont<pRtfGroup->MaxRtfFonts;CurFont++) if (pRtfGroup->font[CurFont].InUse && pRtfGroup->font[CurFont].FontId==FontId) break;

            if (grp->blt.IsBullet) {
               if      (lstrcmpi(pRtfGroup->font[CurFont].name,"symbol")==0)    grp->blt.font=BFONT_SYMBOL;
               else if (lstrcmpi(pRtfGroup->font[CurFont].name,"wingdings")==0) grp->blt.font=BFONT_WINGDINGS;
               else                                                             grp->blt.font=BFONT_DEF;
            }

            CurGroup=rtf->GroupLevel;
            lstrcpy(group[CurGroup].TypeFace,pRtfGroup->font[CurFont].name); // font name
            lstrcpy(group[CurGroup].FontFamily,pRtfGroup->font[CurFont].family); // font name
            group[CurGroup].CharSet=pRtfGroup->font[CurFont].CharSet;

         }
         else if (lstrcmpi(rtf->CurWord,"pnfs")==0) {
            CurGroup=rtf->GroupLevel;
            group[CurGroup].PointSize2=(int)(rtf->IntParam);// point size
         }
         else if (lstrcmpi(rtf->CurWord,"pnb")==0) {
            CurGroup=rtf->GroupLevel;
            if (rtf->IntParam) group[CurGroup].style=group[CurGroup].style|BOLD;
            else               group[CurGroup].style=group[CurGroup].style&(~(UINT)BOLD);
         }
         else if (lstrcmpi(rtf->CurWord,"pni")==0) {
            CurGroup=rtf->GroupLevel;
            if (rtf->IntParam) group[CurGroup].style=group[CurGroup].style|ITALIC;
            else               group[CurGroup].style=group[CurGroup].style&(~(UINT)ITALIC);
         }

         else if (lstrcmpi(rtf->CurWord,"pndec")==0 && !grp->blt.IsBullet) grp->blt.NumberType=NBR_DEC;
         else if (lstrcmpi(rtf->CurWord,"pnucltr")==0 && !grp->blt.IsBullet) grp->blt.NumberType=NBR_UPR_ALPHA;
         else if (lstrcmpi(rtf->CurWord,"pnlcltr")==0 && !grp->blt.IsBullet) grp->blt.NumberType=NBR_LWR_ALPHA;
         else if (lstrcmpi(rtf->CurWord,"pnucrm")==0 && !grp->blt.IsBullet) grp->blt.NumberType=NBR_UPR_ROMAN;
         else if (lstrcmpi(rtf->CurWord,"pnlcrm")==0 && !grp->blt.IsBullet) grp->blt.NumberType=NBR_LWR_ROMAN;

         else if (lstrcmpi(rtf->CurWord,"pntxtb")==0)  pntxtbFound=TRUE;
         else if (lstrcmpi(rtf->CurWord,"pntxta")==0)  pntxtaFound=TRUE;
       }
       else {                    // text
         if (pntxtbFound) {
            if (grp->blt.IsBullet) grp->blt.BulletChar=rtf->CurWord[0];
            else  {
                rtf->CurWord[MAX_NUM_TEXT_LEN]=0;
                lstrcpy(grp->blt.BefText,rtf->CurWord);
            }
         }
         else if (pntxtaFound) {
            if (!grp->blt.IsBullet) grp->blt.AftChar=rtf->CurWord[0];
         }
       }
    }

    return 0;                 // successful
}

/*****************************************************************************
    RtfHdrFtrExists:
    Returns TRUE if header/footer text exists for the section.
******************************************************************************/
BOOL RtfHdrFtrExists(PTERWND w, int sect)
{
    long FirstLine,LastLine;

    FirstLine=TerSect1[sect].hdr.FirstLine;
    LastLine=TerSect1[sect].hdr.LastLine;
    if (FirstLine>=0 && FirstLine<TotalLines && LastLine>=0 && LastLine<TotalLines) {
      if (LastLine>(FirstLine+2)) return TRUE;  // more than one line found
      if (LastLine==(FirstLine+2) && LineLen(FirstLine+1)>1) return TRUE;
    }

    FirstLine=TerSect1[sect].ftr.FirstLine;
    LastLine=TerSect1[sect].ftr.LastLine;
    if (FirstLine>=0 && FirstLine<TotalLines && LastLine>=0 && LastLine<TotalLines) {
      if (LastLine>(FirstLine+2)) return TRUE;  // more than one line found
      if (LastLine==(FirstLine+2) && LineLen(FirstLine+1)>1) return TRUE;
    }

    FirstLine=TerSect1[sect].fhdr.FirstLine;
    LastLine=TerSect1[sect].fhdr.LastLine;
    if (FirstLine>=0 && FirstLine<TotalLines && LastLine>=0 && LastLine<TotalLines) {
      if (LastLine>(FirstLine+2)) return TRUE;  // more than one line found
      if (LastLine==(FirstLine+2) && LineLen(FirstLine+1)>1) return TRUE;
    }

    FirstLine=TerSect1[sect].fftr.FirstLine;
    LastLine=TerSect1[sect].fftr.LastLine;
    if (FirstLine>=0 && FirstLine<TotalLines && LastLine>=0 && LastLine<TotalLines) {
      if (LastLine>(FirstLine+2)) return TRUE;  // more than one line found
      if (LastLine==(FirstLine+2) && LineLen(FirstLine+1)>1) return TRUE;
    }


    return FALSE;
}

/*****************************************************************************
    InitGroupFromStyle:
    Initialize the current group information from the specified stylesheet item.
******************************************************************************/
InitGroupFromStyle(PTERWND w,struct StrRtf far *rtf, int id, int type, BOOL initialize)
{
    int  TabId;
    struct StrRtfGroup far *group;
    struct StrRtfGroup far *pRtfGroup=GetRtfGroup(w,rtf);

    group=&(rtf->group[rtf->GroupLevel]);

    if (group->style&(FNOTEALL)) return TRUE;  // can't change style in the middle of the footnote

    // translate rtf style id to internal style id
    if (id>=pRtfGroup->MaxRtfSID) return true;
    
    id=pRtfGroup->RtfSID[id];
    if (id<0) return TRUE;     // style did not exist in the stylesheet group
    if (id>=TotalSID) return TRUE;   // style not found

    // set the style id
    if (type==SSTYPE_PARA) group->ParaStyId=id;
    else                   group->CharStyId=id;

    if (!initialize) return TRUE;   // don't initialize the group data

    // assign other style member variables
    if (lstrlen(StyleId[id].TypeFace)>0) lstrcpy(group->TypeFace,StyleId[id].TypeFace);         // font type face
    if (StyleId[id].TwipsSize>0) group->PointSize2=StyleId[id].TwipsSize/10;    // comvert twips to pointsize 2
    //group->style|=StyleId[id].style;            // character style bits
    group->style=StyleId[id].style|(group->style&(FNOTEALL));            // character style bits
    group->TextColor=StyleId[id].TextColor;     // text color
    group->TextBkColor=StyleId[id].TextBkColor; // text background color
    group->UlineColor=StyleId[id].UlineColor;     // text color
    group->expand=StyleId[id].expand;           // text character space
    group->offset=StyleId[id].offset;           // text character space

    // assign the paragraph properties
    if (type==SSTYPE_PARA) {
       group->LeftIndent=StyleId[id].LeftIndentTwips;  // left indent in twips
       group->RightIndent=StyleId[id].RightIndentTwips;// Right indent in twips
       group->FirstIndent=StyleId[id].FirstIndentTwips;// First line indent in twips
       group->flags|=StyleId[id].ParaFlags;            // includes such flags as double spacing,centering, right justify etc
       group->pflags|=StyleId[id].pflags;              // includes such flags as double spacing,centering, right justify etc
       group->ParShading=StyleId[id].shading;          // shading (0 to 10000)
       group->SpaceBefore=StyleId[id].SpaceBefore;     // space before the paragraph in twips
       group->SpaceAfter=StyleId[id].SpaceAfter;       // space after the paragraph in twips
       group->SpaceBetween=StyleId[id].SpaceBetween;   // space between the paragraph lines in twips
       group->LineSpacing=StyleId[id].LineSpacing;     // variable line spacing
       group->ParaBkColor=StyleId[id].ParaBkColor;     // space between the paragraph lines in twips
       group->ParaBorderColor=StyleId[id].ParaBorderColor;     // space between the paragraph lines in twips
       group->OutlineLevel=StyleId[id].OutlineLevel;   // outline level

       // initialize the tab id
       TabId=StyleId[id].TabId;
       FarMove(&(TerTab[TabId]),&(group->tab),sizeof(struct StrTab));;
    }

    return TRUE;
}

/*****************************************************************************
    BuildRtfAnimSeq:
    Build a animation sequence picture.
******************************************************************************/
BOOL BuildRtfAnimSeq(PTERWND w,struct StrRtfGroup far *group, struct StrRtfPict far *pic, int pict)
{
    struct StrRtf far *rtf;
    int FirstPict;

    rtf=group->rtf;                           // pointer to the rtf data structure

    FirstPict=rtf->FirstAnimPict;

    if (!(TerFont[FirstPict].InUse) || !(TerFont[FirstPict].style&PICT)) return TRUE;

    if (!BuildRtfPicture(w,group,pic,pict)) return FALSE;

    TerAddAnimPict(hTerWnd,FirstPict,pict,group->AnimDelay);

    return TRUE;
}


/*****************************************************************************
    BuildRtfPictFrame:
    Build a frame for the picture.
******************************************************************************/
BOOL BuildRtfPictFrame(PTERWND w,struct StrRtfGroup far *group, int pict)
{
    int ParaFID;
    UINT FrmFlags;
    struct StrRtf far *rtf=group->rtf;

    if ((ParaFID=GetParaFrameSlot(w))<0) return FALSE;

    FarMemSet(&(ParaFrame[ParaFID]),0,sizeof(struct StrParaFrame));
    ParaFrame[ParaFID].InUse=TRUE;
    ParaFrame[ParaFID].DistFromText=FRAME_DIST_FROM_TEXT;
    ParaFrame[ParaFID].margin=0;

    TerFont[pict].ParaFID=ParaFID;

    if      (group->shape.align==LEFT)          TerFont[pict].FrameType=PFRAME_LEFT;
    else if (group->shape.align==RIGHT_JUSTIFY) TerFont[pict].FrameType=PFRAME_RIGHT;
    else                                        TerFont[pict].FrameType=PFRAME_FLOAT;
    if (group->shape.FrmFlags&(PARA_FRAME_VPAGE|PARA_FRAME_VMARG|PARA_FRAME_HPAGE)) TerFont[pict].FrameType=PFRAME_FLOAT;

    ParaFID=TerFont[pict].ParaFID;
    ParaFrame[ParaFID].pict=pict;

    if (TerFont[pict].PictType==PICT_SHAPE) {
        TerFont[pict].PictWidth=group->shape.width;
        TerFont[pict].PictHeight=group->shape.height;
        ParaFrame[ParaFID].margin=0;

        ParaFrame[ParaFID].ShapeType=group->shape.type;
    }
    if (TerFont[pict].FrameType!=PFRAME_NONE) ParaFrame[ParaFID].ShapeType=group->shape.type;

    ParaFrame[ParaFID].width=TerFont[pict].PictWidth+2*ParaFrame[ParaFID].margin;
    ParaFrame[ParaFID].height=TerFont[pict].PictHeight+2*ParaFrame[ParaFID].margin;
    ParaFrame[ParaFID].MinHeight=ParaFrame[ParaFID].height;

    // set frame flags
    FrmFlags=group->shape.FrmFlags;
    if (group->shape.WrapType==SWRAP_NO_WRAP) FrmFlags|=PARA_FRAME_NO_WRAP;
    else if (group->shape.WrapType==SWRAP_IGNORE || group->shape.WrapType==SWRAP_THRU) FrmFlags|=(PARA_FRAME_WRAP_THRU|PARA_FRAME_TEXT_BOX);

    // set x/y
    ParaFrame[ParaFID].ParaY=group->shape.y;
    ParaFrame[ParaFID].x=group->shape.x;

    ParaFrame[ParaFID].GroupX=group->shape.x;  // used when PARA_FRAME_SHPGRP is set
    ParaFrame[ParaFID].GroupY=group->shape.y;  // used when PARA_FRAME_SHPGRP is set
    
    ParaFrame[ParaFID].OrgX=group->shape.x;    // used when PARA_FRAME_IGNORE_X is used
    ParaFrame[ParaFID].OrgY=group->shape.y;    // used when PARA_FRAME_IGNORE_X is used

    if (FrmFlags&PARA_FRAME_HPAGE) {
       ParaFrame[ParaFID].x-=((int)InchesToTwips(rtf->sect.LeftMargin));
       //ResetUintFlag(FrmFlags,PARA_FRAME_HPAGE);  // PosPictFrames sets again using the OrgX value and PARA_FRAME_HPAGE
    }

    ParaFrame[ParaFID].ZOrder=group->shape.ZOrder; // z order of the frame

    ParaFrame[ParaFID].BackColor=CLR_WHITE;
    ParaFrame[ParaFID].flags=FrmFlags;

    SetPictSize(w,pict,TwipsToScrY(TerFont[pict].PictHeight),TwipsToScrX(TerFont[pict].PictWidth),TRUE);
    XlateSizeForPrt(w,pict);              // convert size to printer resolution

    rtf->flags|=RFLAG_PARA_FRAME_FOUND;

    if (TerFont[pict].PictType==PICT_SHAPE) RtfParaFID=ParaFID;  // so that additional frame attributes can be applied in ReadRtfShape function

    rtf->flags2|=RFLAG2_PICT_FRAME_INSERTED;
    rtf->PictFID=ParaFID;

    return TRUE;
}

/*****************************************************************************
    BuildRtfPicture:
    Build a picture id.
******************************************************************************/
BOOL BuildRtfPicture(PTERWND w,struct StrRtfGroup far *group, struct StrRtfPict far *pic, int pict)
{
    int    type;
    struct StrRtf far *rtf=group->rtf;

    TerFont[pict].InUse=TRUE;
    TerFont[pict].PictType=pic->type;       // picture type
    TerFont[pict].ObjectType=OBJ_NONE;      // Not an OLE object
    TerFont[pict].hMeta=pic->hMeta;         // meta file handle
    #if defined(WIN32)
       TerFont[pict].hEnhMeta=pic->hEnhMeta;         // meta file handle
    #endif

    TerFont[pict].ImageSize=pic->ImageSize; // size of the picture image
    TerFont[pict].InfoSize=pic->InfoSize;   // size of the picture info (not used by metafile)
    TerFont[pict].pImage=pic->pImage;       // image block (not used by metafile)
    TerFont[pict].pInfo=pic->pInfo;         // picture info (not used by metafile
    TerFont[pict].style=PICT|group->style;
    TerFont[pict].AuxId=group->AuxId;
    TerFont[pict].PictAlign=pic->align;     // picture alignment
    TerFont[pict].OrigPictType=pic->OrigPictType;
    TerFont[pict].OrigImageSize=pic->OrigImageSize;
    TerFont[pict].pOrigImage=pic->pOrigImage;     // original image data - OurAlloc pointer
    TerFont[pict].offset=group->offset;
    TerFont[pict].CropLeft=pic->CropLeft;
    TerFont[pict].CropRight=pic->CropRight;
    TerFont[pict].CropTop=pic->CropTop;
    TerFont[pict].CropBot=pic->CropBot;
    TerFont[pict].OrigPictWidth=pic->width;    // original picture width in twips
    TerFont[pict].OrigPictHeight=pic->height;

    if (TerFont[pict].PictAlign==0 && group->style&SUBSCR) TerFont[pict].PictAlign=ALIGN_MIDDLE;

    if (group->FieldId) {
       TerFont[pict].FieldId=group->FieldId;
       if (rtf->FieldCode) {
           TerFont[pict].FieldCode=OurAlloc(lstrlen(rtf->FieldCode)+1);
           lstrcpy(TerFont[pict].FieldCode,rtf->FieldCode);
       } 
    } 
    
    type=TerFont[pict].PictType;

    if (pic->ScaleY<0) {  // fit picture to shape
       if (group->shape.height>0) pic->ScaleY=MulDiv(group->shape.height,100,pic->height);
       else                       pic->ScaleY=0;
    } 
    if (pic->ScaleY) {
       pic->height=lMulDiv(pic->height,pic->ScaleY,100);
       pic->CropTop=lMulDiv(pic->CropTop,pic->ScaleY,100);
       pic->CropBot=lMulDiv(pic->CropBot,pic->ScaleY,100);
    }
    
    if (pic->ScaleX<0) {  // fit picture to shape
       if (group->shape.width>0) pic->ScaleX=MulDiv(group->shape.width,100,pic->width);
       else                      pic->ScaleX=0;
    } 
    if (pic->ScaleX>0) {
       pic->width=lMulDiv(pic->width,pic->ScaleX,100);
       pic->CropLeft=lMulDiv(pic->CropLeft,pic->ScaleX,100);
       pic->CropRight=lMulDiv(pic->CropRight,pic->ScaleX,100);
    }
    
    if (pic->CropTop!=0) pic->height-=pic->CropTop;
    if (pic->CropBot!=0) pic->height-=pic->CropBot;
    if (pic->CropLeft!=0) pic->width-=pic->CropLeft;
    if (pic->CropRight!=0) pic->width-=pic->CropRight;

    TerFont[pict].PictHeight=(int)(pic->height);// height in twips
    TerFont[pict].PictWidth=(int)(pic->width);  // width in twips

    if (type==PICT_DIBITMAP) {
        DIB2Bitmap(w,hTerDC,pict);
        XlateSizeForPrt(w,pict);                       // convert to printer dimensions
    }
    else if (type==PICT_CONTROL) {   // embedded control
        RealizeControl(w,pict);
        XlateSizeForPrt(w,pict);                       // convert to printer dimensions
    }
    else if (type==PICT_FORM)    {   // form elements
        TerFont[pict].FieldId=pic->FormId;
        RealizeControl(w,pict);
        XlateSizeForPrt(w,pict);                       // convert to printer dimensions
    }
    else {
        // calculate height/width in pixels
        if (TRUE || type==PICT_META7 || type==PICT_ENHMETAFILE) {
           TerFont[pict].bmWidth=(int)pic->bmWidth;
           TerFont[pict].bmHeight=(int)pic->bmHeight;
        }
        else {
           TerFont[pict].bmWidth=(int)lMulDiv(pic->bmWidth,1440,2540);  // convert HIMETRIC to twips           TerFont[pict].bmWidth=PointsToScrX(TerFont[pict].bmWidth);  // convert to pixel units
           TerFont[pict].bmHeight=(int)lMulDiv(pic->bmHeight,1440,2540);// convert HIMETRIC to twips
        }

        SetPictSize(w,pict,TwipsToScrY(TerFont[pict].PictHeight),TwipsToScrX(TerFont[pict].PictWidth),TRUE);

        XlateSizeForPrt(w,pict);                       // convert to printer dimensions
    }

    return TRUE;
}

/*****************************************************************************
    UpdateRtfStylesheet:
    Update the stylesheet.
******************************************************************************/
UpdateRtfStylesheet(PTERWND w,struct StrRtf far *rtf)
{
    int id,i,type=SSTYPE_PARA,len,RtfId;
    struct StrRtfGroup far *group;
    BYTE name[MAX_WIDTH+1];
    struct StrRtfGroup far *pRtfGroup=GetRtfGroup(w,rtf);

    // extract the style name
    rtf->OutBuf[rtf->OutBufLen]=0;
    lstrcpy(name,rtf->OutBuf);
    StrTrim(name);
    len=lstrlen(name);
    if (len>0 && name[len-1]==';') len--;
    if (len>MAX_SS_NAME_WIDTH) len=MAX_SS_NAME_WIDTH;
    name[len]=0;

    // empty the rtf text buffer
    rtf->OutBufLen=0;
    rtf->OutBuf[0]=0;

    // get the style type
    group=&(rtf->group[rtf->GroupLevel]);
    if (group->gflags&GFLAG_CHAR_STYLE) type=SSTYPE_CHAR;

    // enlarge the RtfSID table if necessary
    if   (type==SSTYPE_PARA) RtfId=group->ParaStyId;
    else                     RtfId=group->CharStyId;
    if (RtfId>=pRtfGroup->MaxRtfSID) {
       if (NULL==(pRtfGroup->RtfSID=(LPINT)OurRealloc(pRtfGroup->RtfSID,sizeof(int)*(DWORD)(RtfId+1)))) {
           PrintError(w,MSG_OUT_OF_MEM,"UpdateRtfStyleSheet");
           return RTF_SYNTAX_ERROR;
       }
       for (i=pRtfGroup->MaxRtfSID;i<=RtfId;i++) pRtfGroup->RtfSID[i]=-1;   // initialize

       pRtfGroup->MaxRtfSID=RtfId+1;
    }

    // check if the style already exists
    for (id=0;id<TotalSID;id++) if (StyleId[id].InUse && StyleId[id].type==type && lstrcmpi(name,StyleId[id].name)==0) break;

    // is it an existing style id
    if (id<TotalSID) {
       pRtfGroup->RtfSID[RtfId]=id;

       // use existing style when a non default style or clipboard or buffer paste
       if (id>1 || RtfInput>=RTF_CB) return TRUE;
    }
    else {          // allocate a new style
       id=GetStyleIdSlot(w);
       if (id<0) return FALSE;

       // initialize the style structure
       FarMemSet(&(StyleId[id]),0,sizeof(struct StrStyleId));
    }

    // initialize the style
    StyleId[id].InUse=TRUE;
    StyleId[id].type=type;
    StyleId[id].RtfIndex=RtfId;                        // keep track if a new style
    pRtfGroup->RtfSID[RtfId]=id;


    // assign other style member variables
    lstrcpy(StyleId[id].name,name);                  // style name
    StyleId[id].next=group->NextStyId;               // next style id

    lstrcpy(StyleId[id].TypeFace,group->TypeFace);   // font type face
    StyleId[id].FontFamily=0;                        // font family
    StyleId[id].TwipsSize=group->PointSize2*10;      // convert PointSize2 to twips
    StyleId[id].style=group->style;                  // character style bits
    StyleId[id].TextColor=group->TextColor;          // text color
    StyleId[id].TextBkColor=group->TextBkColor;      // text background color
    StyleId[id].UlineColor=group->UlineColor;          // text color
    StyleId[id].expand=group->expand;                // text character space
    StyleId[id].offset=group->offset;                // text character space
    StyleId[id].LeftIndentTwips=group->LeftIndent;   // left indent in twips
    StyleId[id].RightIndentTwips=group->RightIndent; // Right indent in twips
    StyleId[id].FirstIndentTwips=group->FirstIndent; // First line indent in twips
    StyleId[id].ParaFlags=group->flags;              // includes such flags as double spacing,centering, right justify etc
    StyleId[id].pflags=group->pflags&PFLAG_STYLES;   // includes PFLAG_ styles
    StyleId[id].shading=group->ParShading;           // shading (0 to 10000)
    StyleId[id].SpaceBefore=group->SpaceBefore;      // space before the paragraph in twips
    StyleId[id].SpaceAfter=group->SpaceAfter;        // space after the paragraph in twips
    StyleId[id].SpaceBetween=group->SpaceBetween;    // space between the paragraph lines in twips
    StyleId[id].LineSpacing=group->LineSpacing;      // variable line spacing
    StyleId[id].ParaBkColor=group->ParaBkColor;      // space between the paragraph lines in twips
    StyleId[id].ParaBorderColor=group->ParaBorderColor;      // space between the paragraph lines in twips
    StyleId[id].OutlineLevel=group->OutlineLevel;      // space between the paragraph lines in twips
    if (type==SSTYPE_CHAR)StyleId[id].flags=SSFLAG_ADD;  // SSFLAG_ constants

    // assign bullet id 
    if (group->RtfLs>0/* && !(group->flags&BULLET)*/) {   // set a bullet for ls line
       group->flags|=BULLET;
       FarMemSet(&(group->blt),0,sizeof(struct StrBlt));
       group->blt.NumberType=NBR_DEFAULT;     // not yet assigned
    } 
    group->blt.ls=rtf->XlateLs[group->RtfLs];
    group->blt.lvl=group->ListLvl;
    if (True(group->flags&BULLET)) StyleId[id].BltId=NewBltId(w,0,&(group->blt));

    // assign the tab id
    StyleId[id].TabId=NewTabId(w,0,&(group->tab));

    // update the default font
    if (false && id==0 && RtfInput<RTF_CB) {   // initial read of the file
       // recreate the default font
       if (lstrlen(group->TypeFace)>0) {
          lstrcpy(TerFont[0].TypeFace,group->TypeFace);
          lstrcpy(TerArg.FontTypeFace,group->TypeFace);
       }
       if (group->PointSize2>0) {
          TerFont[0].TwipsSize=group->PointSize2*10;    // convert pointsize2 to twips
          TerArg.PointSize=group->PointSize2/2;
       }
       TerFont[0].style=group->style;
       TerFont[0].TextColor=group->TextColor;
       TerFont[0].TextBkColor=group->TextBkColor;
       TerFont[0].UlineColor=group->UlineColor;
       TerFont[0].expand=group->expand;
       TerFont[0].offset=group->offset;
       CreateOneFont(w,hTerDC,0,TRUE);                // recreate the default font

       // recreate the default para id
       PfmtId[0].LeftIndentTwips=group->LeftIndent;   // left indent in twips
       PfmtId[0].RightIndentTwips=group->RightIndent; // Right indent in twips
       PfmtId[0].FirstIndentTwips=group->FirstIndent; // First line indent in twips
       PfmtId[0].flags=group->flags;                  // includes such flags as double spacing,centering, right justify etc
       PfmtId[0].shading=group->ParShading;           // shading (0 to 10000)
       PfmtId[0].SpaceBefore=group->SpaceBefore;      // space before the paragraph in twips
       PfmtId[0].SpaceAfter=group->SpaceAfter;        // space after the paragraph in twips
       PfmtId[0].SpaceBetween=group->SpaceBetween;    // space between the paragraph lines in twips
       PfmtId[0].LineSpacing=group->LineSpacing;      // space between the paragraph lines in twips
       PfmtId[0].BkColor=group->ParaBkColor;          // space between the paragraph lines in twips
       PfmtId[0].BorderColor=group->ParaBorderColor;          // space between the paragraph lines in twips
       PfmtId[0].TabId=StyleId[0].TabId;
    }


    return TRUE;
}

/*****************************************************************************
    ReadRtfList:
    Read the list table
******************************************************************************/
ReadRtfList(PTERWND w,struct StrRtf far *rtf)
{
    int    i,ControlGroupLevel=0,CurList,CurLevel,MaxLevels;
    BOOL   InLevelText=FALSE,InListLevel=FALSE,UnicodeUsed;
    int    LevelTextGroup=0,TextCharCount=0,ListLevelGroup;
    int    UniCharSet=-1;
    struct StrList lst;
    struct StrList far *pList=NULL;
    struct StrListLevel far *pLevel;

    ControlGroupLevel=rtf->GroupLevel;

    rtf->group[ControlGroupLevel].gflags|=GFLAG_NO_UNICODE;   // do not process unicode within list table

    while (TRUE) {
       // get color number
       if (!GetRtfWord(w,rtf)) return RTF_FILE_INCOMPLETE;

       if (rtf->GroupBegin) continue;
       if (rtf->GroupEnd) {
          if (rtf->GroupLevel<LevelTextGroup) InLevelText=FALSE;  // out of level text

          // getting out of list level group, capture the font information
          if (InListLevel && rtf->GroupLevel<ListLevelGroup) {
              InListLevel=FALSE;

              if (UniCharSet>=0) rtf->group[rtf->GroupLevel+1].CharSet=UniCharSet;
              
              if (IsRtfPlainFont(w,rtf,&(rtf->group[rtf->GroupLevel+1]))) pLevel->FontId=0;  // font parameters did not change, font value 0 results in using paragraph font the list
              else pLevel->FontId=GetRtfFontId(w,rtf,&(rtf->group[rtf->GroupLevel+1]));  // get font id from the group being exited
              pLevel->FontStylesOff=rtf->group[rtf->GroupLevel+1].StyleOff;
          } 

          if (rtf->GroupLevel<ControlGroupLevel) break;    // color group ended
          else                                   continue; // look for next color
       }
       

       if (rtf->IsControlWord) {
         // list tags
         if (lstrcmpi(rtf->CurWord,"list")==0) {             // start a new list
            FarMemSet(&lst,0,sizeof(lst));                   // initialize a new list structure
            pList=&lst;
            
            pList->InUse=TRUE;
            pList->LevelCount=MaxLevels=9;                   // default levels
            pList->level=MemAlloc(MaxLevels*sizeof(struct StrListLevel));
            FarMemSet(pList->level,0,MaxLevels*sizeof(struct StrListLevel));

            CurLevel=-1;                                     // next level number, not yet enabled
         }
         else if (lstrcmpi(rtf->CurWord,"listtemplateid")==0) {    // list template id
            pList->TmplId=rtf->IntParam;
         }
         else if (lstrcmpi(rtf->CurWord,"listrestarthdn")==0) {    // restart at section
            if (rtf->IntParam) pList->flags|=LISTFLAG_RESTART_SEC;
         }
         else if (lstrcmpi(rtf->CurWord,"listsimple")==0 || lstrcmpi(rtf->CurWord,"listhybrid")==0) {  
            if (lstrcmpi(rtf->CurWord,"listhybrid")==0) MaxLevels=9;
            else if (lstrcmpi(rtf->param,"0")==0) MaxLevels=9;  // listsimple with '0' num argument
            else                                  MaxLevels=1;  // listsimple with '1' argument or no argument
         
            if (pList->LevelCount!=MaxLevels) {
               if (pList->level) MemFree(pList->level);
               pList->level=MemAlloc(MaxLevels*sizeof(struct StrListLevel));
               FarMemSet(pList->level,0,MaxLevels*sizeof(struct StrListLevel));
            }
            pList->LevelCount=MaxLevels;
         }
         else if (lstrcmpi(rtf->CurWord,"listname")==0) {
            BYTE name[MAX_WIDTH+1];                             // read date format
            int len;

            ReadRtfGroupText(w,rtf,name);
            len=lstrlen(name);
            if (len>0 && name[len-1]==';') name[len-1]=0;  // discard ';'

            pList->name=MemAlloc(lstrlen(name)+1);
            lstrcpy(pList->name,name);
         } 
         else if (lstrcmpi(rtf->CurWord,"listid")==0) {
            pList->id=rtf->IntParam;   // long value
            
            for (i=1;i<TotalLists;i++) if (list[i].InUse && list[i].id==rtf->IntParam) break;
            if (i<TotalLists) {
               CurList=i;
               FreeList(w,CurList);        // free any existing list data
            }
            else {                      // get the new list slot
               if ((CurList=GetListSlot(w))<0) return RTF_SYNTAX_ERROR;
            }

            FarMove(pList,&list[CurList],sizeof(struct StrList));  // copy the data so far read
            pList=&list[CurList];                                  // set the current pointer

         }   
         
         // list leve tags
         else if (lstrcmpi(rtf->CurWord,"listlevel")==0) {
            CurLevel++;
            if (CurLevel>=0 && CurLevel<MaxLevels) {  // initialize the new level
               pLevel=&(pList->level[CurLevel]);
               pLevel->start=1;       // starting number
               InLevelText=FALSE;
               InListLevel=TRUE;
               ListLevelGroup=rtf->GroupLevel;
               UniCharSet=-1;
               
               SetRtfFontDefault(w,rtf,rtf->group);
               if (TotalSID>0 && StyleId[0].TwipsSize!=0)     // 20050916
                   rtf->group[rtf->GroupLevel].PointSize2=StyleId[0].TwipsSize/10;   // default bullet point size
            } 
         }
         else if (lstrcmpi(rtf->CurWord,"levelstartat")==0) {
            if (CurLevel>=0 && CurLevel<MaxLevels) pLevel->start=(int)rtf->IntParam;
         } 
         else if (lstrcmpi(rtf->CurWord,"levelnfc")==0) {
            if (CurLevel>=0 && CurLevel<MaxLevels) pLevel->NumType=(int)rtf->IntParam;
         } 
         else if (lstrcmpi(rtf->CurWord,"levelfollow")==0) {
            if (CurLevel>=0 && CurLevel<MaxLevels) pLevel->CharAft=(int)rtf->IntParam;
         } 
         else if (lstrcmpi(rtf->CurWord,"levelindent")==0) {
            if (CurLevel>=0 && CurLevel<MaxLevels) pLevel->MinIndent=(int)rtf->IntParam;
         } 
         else if (lstrcmpi(rtf->CurWord,"levelold")==0) {
            if (CurLevel>=0 && CurLevel<MaxLevels && rtf->IntParam>0) pLevel->flags|=LISTLEVEL_OLD;
         } 
         else if (lstrcmpi(rtf->CurWord,"levellegal")==0) {
            if (CurLevel>=0 && CurLevel<MaxLevels && rtf->IntParam>0) pLevel->flags|=LISTLEVEL_LEGAL;
         } 
         else if (lstrcmpi(rtf->CurWord,"levelnorestart")==0) { // don't reset numbering when a lower level (parent) is encountered
            if (CurLevel>=0 && CurLevel<MaxLevels) pLevel->flags|=LISTLEVEL_NO_RESET;
         } 
         else if (lstrcmpi(rtf->CurWord,"leveltext")==0) {
            rtf->group[rtf->GroupLevel].gflags|=GFLAG_IN_LEVELTEXT;
            if (CurLevel>=0 && CurLevel<MaxLevels) {
               InLevelText=TRUE;
               LevelTextGroup=rtf->GroupLevel;
               TextCharCount=0;
               UnicodeUsed=false;
            }
         }
         else if (lstrcmpi(rtf->CurWord,"levelnumbers")==0) {
            rtf->group[rtf->GroupLevel].gflags|=GFLAG_IN_LEVELNUM;
         }
         else if (InLevelText && lstrcmpi(rtf->CurWord,"u")==0) {  // unicode list text
            int count=UnicodeToRegBullet(w,rtf,&(pLevel->text[TextCharCount]),ListLevelGroup,&UniCharSet);
            if (count>0) UnicodeUsed=true;
            if (UnicodeUsed) {
               TextCharCount++;
               if (count==2) {
                  TextCharCount++;  // unitcode translated to multi-byte character
                  pLevel->text[0]++;  // increase list length
               }
               pLevel->text[TextCharCount]=0;
            } 
         } 
         else {                                  // capture font and paragraph information
            int ReturnCode=ProcessRtfControl(w,rtf);
            if (ReturnCode!=0) return ReturnCode;
         }   
          
       }
       else {                    // text
         if (InLevelText) {
            int begin=UnicodeUsed?1:0;     // discard the '?' character for the unicode character
            if (begin>rtf->WordLen) begin=rtf->WordLen;
            for (i=begin;i<rtf->WordLen;i++,TextCharCount++) {
               if (TextCharCount>=49) break;
               pLevel->text[TextCharCount]=rtf->CurWord[i];
            }
            pLevel->text[TextCharCount]=0;   // end of the template text
            UnicodeUsed=false;
         }
       }
    }

    return 0;                 // successful
}

/*****************************************************************************
    ReadRtfListOr:
    Read the list override table
******************************************************************************/
ReadRtfListOr(PTERWND w,struct StrRtf far *rtf)
{
    int    i,ControlGroupLevel=0,CurLevel,MaxLevels,ListOrGroup,ListLevelGroup;
    BOOL   InLevelText=FALSE,InListLevel=FALSE,InListOr=FALSE,UnicodeUsed;
    int    LevelTextGroup=0,TextCharCount=0;
    struct StrListOr listor;
    struct StrListOr far *pListOr=NULL;
    struct StrListLevel far *pLevel;
    BOOL   OverrideStartAt=FALSE;
    int    UniCharSet=-1;

    ControlGroupLevel=rtf->GroupLevel;

    rtf->group[ControlGroupLevel].gflags|=GFLAG_NO_UNICODE;   // do not process unicode within list override table

    // reset the RtfLs field in the existing override table
    for (i=0;i<TotalListOr;i++) ListOr[i].RtfLs=0;

    while (TRUE) {
       // get color number
       if (!GetRtfWord(w,rtf)) return RTF_FILE_INCOMPLETE;

       if (rtf->GroupBegin) continue;
       if (rtf->GroupEnd) {
          if (rtf->GroupLevel<LevelTextGroup) InLevelText=FALSE;  // out of level text

          // getting out of list level group, capture the font information
          if (InListLevel && rtf->GroupLevel<ListLevelGroup) {
              int FontId;
              InListLevel=FALSE;

              if (UniCharSet>=0) rtf->group[rtf->GroupLevel+1].CharSet=UniCharSet;

              FontId=GetRtfFontId(w,rtf,&(rtf->group[rtf->GroupLevel+1]));  // get font id from the group being exited
              if (FontId>0) pLevel->FontId=FontId; 
              pLevel->FontStylesOff=rtf->group[rtf->GroupLevel+1].StyleOff;  // styles turned off explicity
          } 

          // check if a listoverride group is ending- need to find the proper slot for this override
          if (InListOr && rtf->GroupLevel<ListOrGroup) {
             InListOr=FALSE;

             if (RtfInput>=RTF_CB) {
               for (i=0;i<TotalListOr;i++) {
                  if (!ListOr[i].InUse) continue;
                  if (ListOr[i].RtfLs>0) continue;  // match only with the existing entries - not the newly read from the rtf file
             
                  if (IsSameListOr(w,&(ListOr[i]),pListOr)) {
                     FreeListOr(w,i);
                     break;
                  } 
               }
             }
             else i=TotalListOr;
             
             if (i==TotalListOr) {    // use a new slot
               if ((i=GetListOrSlot(w))<0) return RTF_SYNTAX_ERROR;
             }  

             // copy the listoverride to the slot
             FarMove(pListOr,&(ListOr[i]),sizeof(struct StrListOr));
             rtf->XlateLs[pListOr->RtfLs]=i;    // to translate rtfls to index into ListOr table
          } 
          
          
          if (rtf->GroupLevel<ControlGroupLevel) break;    // color group ended
          else                                   continue; // look for next color
       }
       

       if (rtf->IsControlWord) {
         // list tags
         if (lstrcmpi(rtf->CurWord,"listoverride")==0) {             // start a new list
            FarMemSet(&listor,0,sizeof(listor));                   // initialize a new list structure
            pListOr=&listor;
            
            pListOr->InUse=TRUE;
            pListOr->LevelCount=MaxLevels=0;                   // default levels

            CurLevel=-1;                                     // next level number, not yet enabled
            InListLevel=FALSE;

            InListOr=TRUE;
            ListOrGroup=rtf->GroupLevel;
         }
         else if (lstrcmpi(rtf->CurWord,"listoverridecount")==0) {
            MaxLevels=(int)rtf->IntParam;
         
            if (pListOr->LevelCount!=MaxLevels) {
               if (pListOr->level) MemFree(pListOr->level);
               pListOr->level=NULL;
               if (MaxLevels>0) {
                  pListOr->level=MemAlloc(MaxLevels*sizeof(struct StrListLevel));
                  FarMemSet(pListOr->level,0,MaxLevels*sizeof(struct StrListLevel));
               }
            }
            pListOr->LevelCount=MaxLevels;
         }
         else if (lstrcmpi(rtf->CurWord,"listid")==0) {
            // validate that the list exists in the list table
            for (i=1;i<TotalLists;i++) if (list[i].InUse && list[i].id==rtf->IntParam) break;
            if (i==TotalLists) i=TotalLists-1;  // ingore the file error

            pListOr->ListIdx=i;
         }   
         else if (lstrcmpi(rtf->CurWord,"ls")==0) {  // list number during rtf read
            pListOr->RtfLs=(int)rtf->IntParam;
         } 
         
         // list leve tags
         else if (lstrcmpi(rtf->CurWord,"lfolevel")==0) {
            CurLevel++;
            if (CurLevel>=0 && CurLevel<MaxLevels) {  // initialize the new level
               struct StrList far *pList;

               pLevel=&(pListOr->level[CurLevel]);
               InLevelText=FALSE;
               UnicodeUsed=false;

               // initialize this level from the list table level
               pList=&(list[pListOr->ListIdx]);
               if (CurLevel<pList->LevelCount) {
                  FarMove(&(pList->level[CurLevel]),pLevel,sizeof(struct StrListLevel));
               } 
               
               SetRtfFontDefault(w,rtf,rtf->group);
               OverrideStartAt=FALSE;                // override start at value
            } 
         }
         else if (lstrcmpi(rtf->CurWord,"listlevel")==0) {
            if (CurLevel>=0 && CurLevel<MaxLevels) {  // initialize the new level
               InListLevel=TRUE;
               ListLevelGroup=rtf->GroupLevel;
               UniCharSet=-1;
               if (TotalSID>0 && StyleId[0].TwipsSize!=0)   // 20050916
                    rtf->group[rtf->GroupLevel].PointSize2=StyleId[0].TwipsSize/10;   // default bullet point size
            } 
         }
         else if (lstrcmpi(rtf->CurWord,"listoverridestartat")==0) {
            OverrideStartAt=TRUE;
            if (CurLevel>=0 && CurLevel<MaxLevels) pLevel->flags|=LISTLEVEL_RESTART;
         } 
         else if (lstrcmpi(rtf->CurWord,"levelnorestart")==0) { // don't reset numbering when a lower level (parent) is encountered
            if (CurLevel>=0 && CurLevel<MaxLevels) pLevel->flags|=LISTLEVEL_NO_RESET;
         } 
         else if (lstrcmpi(rtf->CurWord,"listoverrideformat")==0) {
            if (CurLevel>=0 && CurLevel<MaxLevels) pLevel->flags|=LISTLEVEL_REFORMAT;
         } 
         else if (lstrcmpi(rtf->CurWord,"levelstartat")==0) {
            if (CurLevel>=0 && CurLevel<MaxLevels && OverrideStartAt) pLevel->start=(int)rtf->IntParam;
         } 
         else if (lstrcmpi(rtf->CurWord,"levelnfc")==0) {
            if (CurLevel>=0 && CurLevel<MaxLevels) pLevel->NumType=(int)rtf->IntParam;
         } 
         else if (lstrcmpi(rtf->CurWord,"levelfollow")==0) {
            if (CurLevel>=0 && CurLevel<MaxLevels) pLevel->CharAft=(int)rtf->IntParam;
         } 
         else if (lstrcmpi(rtf->CurWord,"levelindent")==0) {
            if (CurLevel>=0 && CurLevel<MaxLevels) pLevel->MinIndent=(int)rtf->IntParam;
         } 
         else if (lstrcmpi(rtf->CurWord,"levellegal")==0) {
            if (CurLevel>=0 && CurLevel<MaxLevels && rtf->IntParam>0) pLevel->flags|=LISTLEVEL_LEGAL;
         } 
         else if (lstrcmpi(rtf->CurWord,"leveltext")==0) {
            rtf->group[rtf->GroupLevel].gflags|=GFLAG_IN_LEVELTEXT;
            if (CurLevel>=0 && CurLevel<MaxLevels) {
               InLevelText=TRUE;
               LevelTextGroup=rtf->GroupLevel;
               TextCharCount=0;
            }
         } 
         else if (lstrcmpi(rtf->CurWord,"levelnumbers")==0) {
            rtf->group[rtf->GroupLevel].gflags|=GFLAG_IN_LEVELNUM;
         }
         else if (InLevelText && lstrcmpi(rtf->CurWord,"u")==0) {  // unicode list text
            int count=UnicodeToRegBullet(w,rtf,&(pLevel->text[TextCharCount]),ListLevelGroup,&UniCharSet);
            if (count>0) UnicodeUsed=true;
            if (UnicodeUsed) {
               TextCharCount++;
               if (count==2) {
                  TextCharCount++;  // unitcode translated to multi-byte character
                  pLevel->text[0]++;  // increase list length
               }
               pLevel->text[TextCharCount]=0;
               UnicodeUsed=false;
            } 
         } 
         else {                                  // capture font and paragraph information
            int ReturnCode=ProcessRtfControl(w,rtf);
            if (ReturnCode!=0) return ReturnCode;
         }   
          
       }
       else {                    // text
         if (InLevelText) {
            int begin=UnicodeUsed?1:0;
            if (begin>rtf->WordLen) begin=rtf->WordLen;
            for (i=begin;i<rtf->WordLen;i++,TextCharCount++) {
               if (TextCharCount>=49) break;
               pLevel->text[TextCharCount]=rtf->CurWord[i];
            }
            pLevel->text[TextCharCount]=0;   // end of the template text
         }
       }
    }

    return 0;                 // successful
}

/*****************************************************************************
    UnicodeToRegBullet:
    Convert unicode to regular bullet.
******************************************************************************/
int UnicodeToRegBullet(PTERWND w,struct StrRtf far *rtf, LPBYTE pBlt,int ListLevelGroup, LPINT UniCharSet)
{
     BYTE  mb[10];      // multi-byte string
     int   count,CodePage;
     USHORT uc=(USHORT)rtf->IntParam;

     IsMbcsCharSet(w,rtf->group[ListLevelGroup].CharSet,&CodePage);  // get the code page
     count=WideCharToMultiByte(CodePage,0x400/*WC_NO_BEST_FIT_CHARS*/,&uc,1,mb,10,NULL,NULL);
     if (count==1 && mb[0]!='?') {
        (*pBlt)=mb[0];
        return 1;
     }
     
     IsMbcsCharSet(w,2,&CodePage);  // try symbol character set
     count=WideCharToMultiByte(CodePage,0x400/*WC_NO_BEST_FIT_CHARS*/,&uc,1,mb,10,NULL,NULL);
     if (count==1 && mb[0]!='?') {
        (*pBlt)=mb[0];
        return 1;
     }
     else if (uc==61607 || uc==61656 || uc==61553 || uc==61558 || uc==61623 || uc==61692) { // select bullet characters 
        if      (uc==61607) (*pBlt)=0xa7;  // square
        else if (uc==61656) (*pBlt)=0xd8;  // arrow
        else if (uc==61558) (*pBlt)='v';  // 4-diamonds
        else if (uc==61623) (*pBlt)=0xb7;  // round bullet
        else if (uc==61692) (*pBlt)=0xfc;  // check-mark
        else if (uc==61553) (*pBlt)='q';   // hollow square
        return 1;
     }
     else if (uc>0xF000 && uc<0xF0FF) {
        (*pBlt)=(BYTE)(uc-0xF000);
        return 1;
     }
     else {
        int CharSet=rtf->group[ListLevelGroup].CharSet;

        // apply unicode range specific character set
        if (uc>=0x4300 && uc<=0x9fbf) CharSet=128;   // japanese 
     
        IsMbcsCharSet(w,CharSet,&CodePage);  // get the code page
        count=WideCharToMultiByte(CodePage,0x400/*WC_NO_BEST_FIT_CHARS*/,&uc,1,mb,10,NULL,NULL);
        if ((count==1 || count==2) && mb[0]!='?') {
           (*pBlt)=mb[0];
           if (count==2) {
             pBlt[1]=mb[1];
           }
           (*UniCharSet)=rtf->group[ListLevelGroup].CharSet=CharSet;
           return count;
        }
     }   

     return 0;
}
 
/*****************************************************************************
    ReadRtfGroupText:
    Read text until the end of the group
******************************************************************************/
ReadRtfGroupText(PTERWND w,struct StrRtf far *rtf, LPBYTE out)
{
    int  CurGroupLevel=rtf->GroupLevel;
                
    out[0]=0;
    while (GetRtfWord(w,rtf)) {           // extract the date format
        if (rtf->GroupBegin) continue;
        if (rtf->GroupEnd) {
            if (rtf->GroupLevel<CurGroupLevel) break;
            else  continue; // look for the next conponent
        }
        if (!rtf->IsControlWord && (lstrlen(out)+lstrlen(rtf->CurWord))<=MAX_WIDTH) lstrcat(out,rtf->CurWord);
    }
    StrTrim(out);
 
    return TRUE;
}

/*****************************************************************************
    SetRtfFootnote:
    Set the FNOTE style for the last character of the given line.
******************************************************************************/
SetRtfFootnote(PTERWND w, struct StrRtfGroup far *pGroup, long line)
{
    LPWORD fmt;
    int i,len,CurFont;

    if (LineLen(line)==0) return TRUE;
    
    len=LineLen(line);

    fmt=OpenCfmt(w,line);
    CurFont=fmt[len-1];
    if (!(TerFont[CurFont].style&(FNOTEALL))) {
       InputFontId=CurFont;
       HilightType=HILIGHT_OFF;    // needed for the following function
       SetTerCharStyle(hTerWnd,FNOTE,TRUE,FALSE);

       for (i=0;i<len;i++) fmt[i]=InputFontId;

       FootnoteRestFont=InputFontId;   // also store this font for FootnoteRest
       InputFontId=-1;
       if (pGroup->EndnoteMarker && len==1) { // change from footnote to endnote marker
          LPBYTE ptr=pText(line);
          ptr[0]=pGroup->EndnoteMarker;
       } 
    }
    CloseCfmt(w,line);

    pGroup->EndnoteMarker=0;                   // reset the endnote marker

    return TRUE;
}

/*****************************************************************************
    FmtRtfFootnoteNbr:
    Format rtf footnote number
******************************************************************************/
FmtRtfFootnoteNbr(PTERWND w,struct StrRtf far *rtf, LPBYTE string, int nbr, int NumFmt)
{
   if      (NumFmt==NBR_UPR_ALPHA) AlphaFormat(string,nbr,TRUE);
   else if (NumFmt==NBR_LWR_ALPHA) AlphaFormat(string,nbr,FALSE);
   else if (NumFmt==NBR_UPR_ROMAN) romanize(string,nbr,TRUE);
   else if (NumFmt==NBR_LWR_ROMAN) romanize(string,nbr,FALSE);
   else                                    wsprintf(string,"%d",nbr);

   return TRUE;
} 

/*****************************************************************************
    ApplyPictureBrightnessContrast:
    Apply picture brightness (-65536 to + 65536), and contrast (0 to 65536)
******************************************************************************/
ApplyPictureBrightnessContrast(PTERWND w,int pict, int bright, int contrast)
{
     int i,j,ColorsUsed,ColorsInInfo;
     LPBITMAPINFOHEADER pInfo;
     BYTE huge *pImage;
     RGBQUAD *rgb;
     int BytesPerLine,ScanLineSize,ColorsPerLine;

     if (!(TerFont[pict].style&PICT)) return TRUE;   // not a picutre element
     if (TerFont[pict].PictType!=PICT_DIBITMAP) return true; // metafile can not be modified by this function

     //*********** lock the DIB handles ***********
     pImage=(LPVOID)TerFont[pict].pImage;
     pInfo=(LPVOID)TerFont[pict].pInfo;

    // calculate the number of significant colors used
    ColorsUsed=pInfo->biClrUsed;      // determines the color table size

    if (ColorsUsed==0) {
       if      (pInfo->biBitCount==1) ColorsUsed=2;
       else if (pInfo->biBitCount==4) ColorsUsed=16;
       else if (pInfo->biBitCount==8) ColorsUsed=256;
    }
    if (pInfo->biClrImportant>0) ColorsUsed=pInfo->biClrImportant;
    if (TerFont[pict].ColorsUsed>256) ColorsUsed=0; //color table not used

    ColorsInInfo=(TerFont[pict].InfoSize-sizeof(BITMAPINFOHEADER))/sizeof(RGBQUAD);
    if (ColorsUsed>ColorsInInfo) ColorsUsed=ColorsInInfo;

    // modify the color table
    if (ColorsUsed) {
      rgb=&(((LPBITMAPINFO)pInfo)->bmiColors[0]);
      for (i=0;i<ColorsUsed;i++) {
         rgb[i].rgbBlue=ApplyBrightnessContrast(w,rgb[i].rgbBlue,bright,contrast);
         rgb[i].rgbGreen=ApplyBrightnessContrast(w,rgb[i].rgbGreen,bright,contrast);
         rgb[i].rgbRed=ApplyBrightnessContrast(w,rgb[i].rgbRed,bright,contrast);
      }
      return true; 
    } 
    
    // adjust the image data
    BytesPerLine=(pInfo->biBitCount*pInfo->biWidth+7)/8;   // round to byte boundary
    ScanLineSize=sizeof(DWORD)*((BytesPerLine+sizeof(DWORD)-1)/sizeof(DWORD)); // number of dwords, round to DWORD boundary
    ColorsPerLine=pInfo->biWidth*3;  // rgb colors

    for (i=0;i<pInfo->biHeight;i++) {    // scan each line
       BYTE *pLine=&(pImage[ScanLineSize*i]);  // pointer to the pixel line data
       for (j=0;j<ColorsPerLine;j++) {
           pLine[j]=ApplyBrightnessContrast(w,pLine[j],bright,contrast);
       }
    }

    return true; 
}  

/*****************************************************************************
    ApplyBrightnessContrast:
    Apply brightness (-32768 to + 32768), and contrast (0 to 65536) to a color value
******************************************************************************/
BYTE ApplyBrightnessContrast(PTERWND w,BYTE clr, int bright, int contrast)
{
    int NewClr;

    // apply contrast
 
    // apply brightness
    NewClr=clr+bright/128;    // actual formula c = c +  b*256/32768,  0<<c>>255
    if (NewClr<0) NewClr=0;
    if (NewClr>255) NewClr=255;

    clr=(BYTE)NewClr;

    return clr;
} 
