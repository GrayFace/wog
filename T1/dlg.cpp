#include <string.h>
#include <stdlib.h>
#include "structs.h"
#include "common.h"
#include "txtfile.h"
#include "service.h"
#include "string.h"
#include "erm.h"
#include "dlg.h"

#define __FILENUM__ 20
////////////////////////
HDlg *HDlg::Last=NULL;

HDlg::_Body_ *HDlg::Initialize(int x,int y,int dx,int dy,int numofitems){
	STARTNA(__LINE__, 0)
	Byte *(__fastcall *fn)(_Body_ *,int,int x,int y,int dx,int dy,int numofitems); *(Dword *)&fn=0x41AFA0;
	fn(Body,0,x,y,dx,dy,18/*numofitems*/); //???
	// put VTable pointer to Body[0]
//*(Dword *)Body=0x643918;
//  VideoInd=-1;
//  *(int *)&Body[0x70]=-1; // no video
	*(Dword *)Body->Obj=(Dword)VTable;
//  Dword (__fastcall static HDlg::*fn10)(Byte *dlg,int,int par1,int par2); fn10=iActivate;
//  VTable[1]=*(Dword *)&fn10;
	VTable[1]=(Dword)iActivate;
//  Dword (static __fastcall HDlg::*fn11)(Byte *dlg,int,int par1); fn11=iDisactivate;
//  VTable[2]=*(Dword *)&fn11;
	VTable[2]=(Dword)iDisactivate;
	VTable[9]=(Dword)iFunction2;
	Body->VInd=-1; // no video
//  *(Dword *)&Body[0x60]=0;
//  *(Dword *)&Body[0x64]=0;
	for(int i=0;i<10;i++){ Body->ADef[i].Item=0; Body->ADef[i].Cadre=0; } 
	DlgItem **ip=(DlgItem **)h3new(numofitems*sizeof(DlgItem *)); if(ip==0) RETURN(0)
	*(DlgItem ***)&Body->Obj[0x34]=ip;
	*(DlgItem ***)&Body->Obj[0x38]=ip;
	*(DlgItem ***)&Body->Obj[0x3C]=&ip[numofitems];
	Body->Hints=new HDlg::_Body_::_Hint_[numofitems];
	for(int i=0;i<numofitems;i++){ 
		Body->Hints[i].ItemId=0;
		Body->Hints[i].Text=NULL;
	}
	Body->NumOfItems=numofitems;
	RETURN(Body)
}
int HDlg::SetADef(DlgItem *item){
	STARTNA(__LINE__, 0)
	int i;
	for(i=0;i<10;i++) if(Body->ADef[i].Item==0) break;
	if(i>9) RETURN(-1)
	Body->ADef[i].Item=item;
	Body->ADef[i].Cadre=0;
	RETURN(i)
}

void HDlg::SetVideo(int ind,int x,int y){
	STARTNA(__LINE__, 0)
/*  
	*(int *)&Body[0x70]=ind;
	*(int *)&Body[0x70+4]=x;
	*(int *)&Body[0x70+8]=y;
*/
	Body->VInd=ind;
	Body->VX=x;
	Body->VY=y;
	RETURNV
//  if(ind>=0){
//    void (__fastcall *fn)(int vind,int x,int y,int p1,int p2,int p3,int p4,int p5); *(Dword *)&fn=0x597870;
//    fn(ind,x,y,0,0,1,1,1);
//  }
}
void HDlg::AdjustPlayerColor(DlgItem *itp,int pl){
	void (__fastcall *fn2)(DlgItem *,int,int plind); *(Dword *)&fn2=0x4501D0; 
	fn2(itp,0,pl);
}
/*
Byte *HDlg::Demolish(int FreeMem){
	STARTNA(__LINE__, 0)
	Byte *(__fastcall *fn)(Byte *,int,int freemem); *(Dword *)&fn=0x5F3EC0;
	RETURN(fn(Body,0,FreeMem))
}
*/
DlgItem *HDlg::AddItem(DlgItem *itempo){
	STARTNA(__LINE__, 0)
//__asm int 3
	DlgItem **List=(DlgItem **)&Body->Obj[0x30];
	DlgItem *ip=itempo;
	DlgItem *(__fastcall *fn)(DlgItem **List,int,DlgItem *LastItemPo,int Num2Add,DlgItem **List2Add); *(Dword *)&fn=0x5FE2D0;
	RETURN(fn(List,0,List[2],1,&ip));
}

DlgItem *HDlg::BuildPcx(int x,int y,int dx,int dy,int itemind,char *picfile,int flags){
	STARTNA(__LINE__, 0)
	DlgItem *ip=(DlgItem *)h3new(0x34); if(ip==0) RETURN(0)
	DlgItem *(__fastcall *fn)(DlgItem *,int,int x,int y,int dx,int dy,int itemind,char *picfile,int flags); *(Dword *)&fn=0x44FFA0;
	RETURN(fn(ip,0,x,y,dx,dy,itemind,picfile,flags))
}
int HDlg::BuildDefaultBackPcx(int dx,int dy,int withhint){
	STARTNA(__LINE__, 0)
//  void (__fastcall *fn)(_Body_ *,int,int x,int y,int dx,int dy); *(Dword *)&fn=0x48FA80;
//  Dword prevvtable=*(Dword *)Body->Obj;
//  *(Dword *)Body->Obj=0x63DB40;
//  fn(Body,0,x,y,dx,dy);
//  *(Dword *)Body->Obj=prevvtable;
	int i,j,xpics,ypics,px,py,pdx,pdy;
	DlgItem *it=0;
	xpics=(dx+255)/256;
	ypics=(dy+255)/256;
	for(j=0;j<ypics;j++){
		for(i=0;i<xpics;i++){
			px=i*256; pdx=dx-px; if(pdx>256) pdx=256;
			py=j*256; pdy=dy-py; if(pdy>256) pdy=256;
			it=BuildPcx(px,py,pdx,pdy,-1,"diboxbck.pcx",0x800); AddItem(it);
		}
	}
//  int bkitcolor=CurrentUser();
	int ind=100000;
	it=BuildDef(0,0,0x40,0x40,ind++,"dialgbox.def",0,0,0,0,0x10); 
//  void (__fastcall *fn3)(_Body_ *,int,int,int,int,Dword); *(Dword *)&fn3=0x5FF400;
//  fn3(Body,0,0x200,13,100000,bkitcolor);
	AddItem(it);
	it=BuildDef(dx-0x40,0,0x40,0x40,-1/*ind++*/,"dialgbox.def",1,0,0,0,0x10); AddItem(it);
	if(withhint){
		it=BuildDef(0,dy-0x40,0x40,0x40,ind++,"dialgbox.def",8,0,0,0,0x10); AddItem(it);
		it=BuildDef(dx-0x40,dy-0x40,0x40,0x40,ind++,"dialgbox.def",9,0,0,0,0x10); AddItem(it);
	}else{
		it=BuildDef(0,dy-0x40,0x40,0x40,ind++,"dialgbox.def",2,0,0,0,0x10); AddItem(it);
		it=BuildDef(dx-0x40,dy-0x40,0x40,0x40,ind++,"dialgbox.def",3,0,0,0,0x10); AddItem(it);
	}
	xpics=(dx-0x80+0x3F)/0x40;
	ypics=(dy-0x80+0x3F)/0x40;
	for(i=0;i<xpics;i++){
		px=(i+1)*0x40; pdx=dx-0x40-px; if(pdx>0x40) pdx=0x40; if(pdx<0x40){ pdx=0x40; px=dx-0x80; }
		it=BuildDef(px,0,pdx,0x40,ind++,"dialgbox.def",6,0,0,0,0x10); AddItem(it);
		if(withhint){
			it=BuildDef(px,dy-0x40,pdx,0x40,ind++,"dialgbox.def",10,0,0,0,0x10); AddItem(it);
		}else{
			it=BuildDef(px,dy-0x40,pdx,0x40,ind++,"dialgbox.def",7,0,0,0,0x10); AddItem(it);
		}
	}
	for(j=0;j<ypics;j++){
		py=(j+1)*0x40; pdy=dy-0x40-py; if(pdy>0x40) pdy=0x40; if(pdy<0x40){ pdy=0x40; py=dy-0x80; }
		it=BuildDef(0,py,0x40,pdy,ind++,"dialgbox.def",4,0,0,0,0x10); AddItem(it);
		it=BuildDef(dx-0x40,py,0x40,pdy,ind++,"dialgbox.def",5,0,0,0,0x10); AddItem(it);
	}
/*
	for(i=100000;i<ind;i++){ 
		SendCmd2Item(13,i,bkitcolor);
	}
*/
	SendCmd2AllItems(0x0D,100000,6);
	RETURN(ind)
}

DlgItem *HDlg::BuildDef(int x,int y,int dx,int dy,int itemind,char *picfile,int defpicind,int p2,int p3,int p4,int flags){
	STARTNA(__LINE__, 0)
	DlgItem *ip=(DlgItem *)h3new(0x48); if(ip==0) RETURN(0)
	DlgItem *(__fastcall *fn)(DlgItem *,int,int x,int y,int dx,int dy,int itemind,char *picfile,int p1,int p2,int p3,int p4,int flags); *(Dword *)&fn=0x4EA800;
	RETURN(fn(ip,0,x,y,dx,dy,itemind,picfile,defpicind,p2,p3,p4,flags))
}

DlgItem *HDlg::BuildDef2(int x,int y,int dx,int dy,int itemind,char *picfile,int p1,int p2,int p3,int p4,int flags){
	STARTNA(__LINE__, 0)
	DlgItem *ip=(DlgItem *)h3new(0x68); if(ip==0) RETURN(0)
	DlgItem *(__fastcall *fn)(DlgItem *,int,int x,int y,int dx,int dy,int itemind,char *picfile,int p1,int p2,int p3,int p4,int flags); *(Dword *)&fn=0x455BD0;
	RETURN(fn(ip,0,x,y,dx,dy,itemind,picfile,p1,p2,p3,p4,flags))
}

DlgItem *HDlg::BuildScrollBar(int x,int y,int dx,int dy,int itemind,int /*flags*/){
	STARTNA(__LINE__, 0)
	DlgItem *ip=(DlgItem *)h3new(0x68); if(ip==0) RETURN(0)
	DlgItem *(__fastcall *fn)(DlgItem *,int,int x,int y,int dx,int dy,int itemind,int flags,void *callbackfun,int p1,int p2,int p3); *(Dword *)&fn=0x5963C0;
	RETURN(fn(ip,0,x,y,dx,dy,itemind,0x10,NULL/*????*/,0,0,0))
}
DlgItem *HDlg::BuildTxt(int x,int y,int dx,int dy,char *text,char *font,int color,int itemid,int align,int bkcolor,int flags){
	STARTNA(__LINE__, 0)
	DlgItem *ip=(DlgItem *)h3new(0x50); if(ip==0) RETURN(0)
	DlgItem *(__fastcall *fn)(DlgItem *,int,int x,int y,int dx,int dy,char *text,char *font,int color,int itemid,int align,int bkcolor,int flags); *(Dword *)&fn=0x5BC6A0;
	RETURN(fn(ip,0,x,y,dx,dy,text,font,color,itemid,align,bkcolor,flags))
}

DlgItem *HDlg::BuildTxtSB(int x,int y,int dx,int dy,char *text,char *font,int txtcolor,int sbcolor){
	STARTNA(__LINE__, 0)
	DlgItem *ip=(DlgItem *)h3new(0x5C); if(ip==0) RETURN(0)
	DlgItem *(__fastcall *fn)(DlgItem *,int,char *text,int x,int y,int p1,int p2,char *font,int p3,int p4); *(Dword *)&fn=0x5BA360;
	RETURN(fn(ip,0,text,x,y,dx,dy,font,txtcolor,sbcolor))
}

DlgItem *HDlg::BuildTxtED(int x,int y,int dx,int dy,int p1,char *text,char *font,int p2,int p3,int p4,int p5,int itemind,int p6,int p7,int p8,int p9){
	STARTNA(__LINE__, 0)
	DlgItem *ip=(DlgItem *)h3new(0x70); if(ip==0) RETURN(0)
	DlgItem *(__fastcall *fn)(DlgItem *,int,int x,int y,int dx,int dy,int p1,char *text,char *font,int p2,int p3,int p4,int p5,int itemind,int p6,int p7,int p8,int p9); *(Dword *)&fn=0x5BACD0;
	RETURN(fn(ip,0,x,y,dx,dy,p1,text,font,p2,p3,p4,p5,itemind,p6,p7,p8,p9))
}

void HDlg::LoadAllItems(void){
	STARTNA(__LINE__, 0)
	DlgItem *(__fastcall *fn)(_Body_ *,int,DlgItem *,int par); *(Dword *)&fn=0x5FF270;
	DlgItem **ip =*(DlgItem ***)&Body->Obj[0x34];
	DlgItem **ipl=*(DlgItem ***)&Body->Obj[0x38];
	while(ip!=ipl){
		fn(Body,0,*ip,-1);
		ip++;
	}
	RETURNV
}

DlgItem *HDlg::FindItemByID(int id){
	STARTNA(__LINE__, 0)
	DlgItem *(__fastcall *fn)(_Body_ *,int,int id); 
	*(Dword *)&fn=0x5FF5B0;
	DlgItem *dlgit=fn(Body,0,id);
	RETURN(dlgit)
}

void HDlg::EnableItem(DlgItem *itempo,int ENdis){
	STARTNA(__LINE__, 0)
	void (__fastcall *fn)(DlgItem *,int,int ENdis); 
	*(Dword *)&fn=(*(Dword **)itempo)[9];
	fn(itempo,0,ENdis);
	RETURNV
}

void HDlg::Show(int par/*0*/){
	STARTNA(__LINE__, 0)
	*(Dword *)0x6989E8=0;//getTime()+100;
	int curtype,curstype;
	__asm{ // turn ON mouse and set cursor
		mov  ecx, 0x6992B0
		mov  ecx,[ecx]
		mov  eax, [ecx+4Ch] // esi
		mov  curtype,eax
		mov  eax, [ecx+50h] // edi
		mov  curstype,eax
		push 1
		mov  eax,0x50D7B0
		call eax
		mov  ecx, 0x6992B0
		mov  ecx,[ecx]
		xor  eax, eax
		push eax
		push eax
		mov  eax,0x50CEA0
		call eax
	}
//  void (__fastcall *fn0)(_Body_ *,int); *(Dword *)&fn0=0x5FFAC0;
//  fn0(Body,0);
	void (__fastcall *fn)(_Body_ *,int,int par); 
	*(Dword *)&fn=(*(Dword **)Body->Obj)[6];

//    void (__fastcall *fn3)(_Body_ *,int,int,int,int,Dword); *(Dword *)&fn3=0x5FF400;
//    fn3(Body,0,0x200,0,0,0);

	fn(Body,0,par);
	__asm{ // set previous mouse cursor
		mov  ecx, 0x6992B0
		mov  ecx,[ecx]
		mov  eax,curtype
		push eax
		mov  eax,curstype
		push eax
		mov  eax,0x50CEA0
		call eax
	}
	RETURNV
}

void HDlg::Kill(int freemem/*1*/){
	STARTNA(__LINE__, 0)
	void (__fastcall *fn)(_Body_ *,int,int par); 
	*(Dword *)&fn=(*(Dword **)Body->Obj)[0];
	fn(Body,0,freemem);
	RETURNV
}

Dword __fastcall HDlg::iActivate(_Body_ *body,int,int par1,int par2){
	STARTNA(__LINE__, 0)
/*
	int vind=*(Dword *)&dlg[0x70];
	if(vind>=0){
		void (__fastcall *fn)(int vind,int x,int y,int p1,int p2,int p3,int p4,int p5); *(Dword *)&fn=0x597870;
		int vx=*(Dword *)&dlg[0x70+4];
		int vy=*(Dword *)&dlg[0x70+8];
		fn(vind,vx,vy,0,0,1,1,1);
	}
	Dword (__fastcall *fn2)(Byte *dlg,int,int par1,int par2); *(Dword *)&fn2=0x5FF0A0;
	Dword ret=fn2(dlg,0,par1,par2);
	if(ret==0){
		void (__fastcall *fn3)(void); *(Dword *)&fn3=0x5978F0;
		fn3();
	}
*/
	int vind=body->VInd;
	if(vind>=0){
		void (__fastcall *fn)(int vind,int x,int y,int p1,int p2,int p3,int p4,int p5); *(Dword *)&fn=0x597870;
		int vx=body->VX;
		int vy=body->VY;
		fn(vind,vx,vy,0,0,1,1,1);
	}
	Dword (__fastcall *fn2)(_Body_ *dlg,int,int par1,int par2); *(Dword *)&fn2=0x5FF0A0;
	Dword ret=fn2(body,0,par1,par2);
	if(ret==0){
		void (__fastcall *fn3)(void); *(Dword *)&fn3=0x5978F0;
		fn3();
	}
	RETURN(ret)
}

Dword __fastcall HDlg::iDisactivate(_Body_ *body,int,int par1){
	STARTNA(__LINE__, 0)
//  int vind=*(Dword *)&dlg[0x70];
	int vind=body->VInd;
	if(vind>=0){
		void (__fastcall *fn3)(void); *(Dword *)&fn3=0x5978F0;
		fn3();
	}
	Dword (__fastcall *fn2)(_Body_ *dlg,int,int par1); *(Dword *)&fn2=0x5FF220;
	Dword ret=fn2(body,0,par1);
	RETURN(ret)
}

Dword __fastcall HDlg::iFunction1(_Body_ *body,int,int par1){
	STARTNA(__LINE__, 0)
	Dword (__fastcall *fn2)(_Body_ *dlg,int,int par1); *(Dword *)&fn2=0x5D29A0;
//    Dword (__fastcall *fn2)(Byte *dlg,int,int par1); *(Dword *)&fn2=0x41B120;
	Dword ret=fn2(body,0,par1);
/*
	int vind=*(Dword *)&dlg[0x70];
	if(vind>=0){
		int vx=*(Dword *)&dlg[0x70+4];
		int vy=*(Dword *)&dlg[0x70+8];
		__asm{  
			mov   eax,0x597C90
			call  eax
			test  al, al  
			jnz   loc_m1 
			push  1
			push  1
			push  1
			push  0
			push  0
			push  vy
			mov   edx, vx
			mov   ecx, vind
			mov   eax,0x597870
			call  eax
//    call DlgItem_OpenVideo                  ; Call Procedure
		}
loc_m1:;
*/
	int vind=body->VInd;
	if(vind>=0){
		int vx=body->VX;
		int vy=body->VY;
		__asm{  
			mov   eax,0x597C90
			call  eax
			test  al, al  
			jnz   loc_m1 
			push  1
			push  1
			push  1
			push  0
			push  0
			push  vy
			mov   edx, vx
			mov   ecx, vind
			mov   eax,0x597870
			call  eax
//    call DlgItem_OpenVideo                  ; Call Procedure
			}
loc_m1:;
	}
/*
		__asm{
			mov   edx,dlg
			mov   edx, [edx]
			push  0FFFFh
			push  0FFFF0001h
			push  1
			mov   ecx, dlg
			call  dword ptr [edx+14h] 
		}
*/
	RETURN(ret)
}

_MouseStr_ *Dlg_Cmd=NULL;
int Dlg_User_Command=0;
Dword __fastcall HDlg::iFunction2(HDlg::_Body_ *body,int,_MouseStr_ *cmd){
	STARTNA(__LINE__, 0)
	int (__fastcall *dlg_StdProcCmd)(_Body_ *dlg,int,_MouseStr_ *cmd); *(Dword *)&dlg_StdProcCmd=0x41B120;
	int ret=dlg_StdProcCmd(body,0,cmd);
	if(ret) RETURN(ret);
	Byte *mman=*(Byte **)0x6992D0;
	int  ct=getTime();
	int *nt=(int *)0x6989E8;
	if(*nt==0){ *nt=ct+100; RETURN(1) }
	int  dif=ct- *nt;
	if(dif>0){
		if(dif<100) dif=100;
		*nt+=dif;
		for(int i=0;i<10;i++){
			HDlg::_Body_::_ADef_ *adef=&body->ADef[i];
			if(adef->Item==0) continue;
			++adef->Cadre;
//__asm int 3
			DlgItem *it=adef->Item;
			int     cdr=adef->Cadre;
//          Byte *it2=*(Byte *)&it[0x30];
			__asm{
				mov   ecx, it
				mov   eax, [ecx+0x30]
				cmp   dword ptr [eax+0x28], 7 
				jle   Lab1
				mov   esi, [eax+0x2C]
				mov   edi, [esi+0x1C]
				test  edi, edi
				jz    Lab1
				mov   eax, [eax+0x1C]
				mov   eax, [eax+0x1C]
				mov   eax, [eax]
				jmp   Lab2
			}
Lab1: __asm{
				xor   eax, eax 
			}
Lab2: __asm{
				mov   edx, cdr
				cmp   edx, eax 
				jl    Lab3
//            mov   dword ptr [ebx+0x60], 0
			}
			adef->Cadre=0;
Lab3:
			cdr=adef->Cadre;
			__asm{
				mov   ecx, it
				mov   edx, cdr
				push  edx
				mov   eax,0x4EB0D0
				call  eax
			}
		}
		int vind=body->VInd;
		if(vind>=0){
			int vx=body->VX;
			int vy=body->VY;
			__asm{  
				mov   eax,0x597C90
				call  eax
				test  al, al  
				jnz   loc_m1 
				push  1
				push  1
				push  1
				push  0
				push  0
				push  vy
				mov   edx, vx
				mov   ecx, vind
				mov   eax,0x597870
				call  eax
//    call DlgItem_OpenVideo                  ; Call Procedure
			}
loc_m1:;
		}
	// animation
		__asm{
//int 3
			mov   ebx, body
			mov   eax, [ebx]
			push  0xFFFF
			push  0xFFFF0001
			push  1
			mov   ecx, ebx
			call  dword ptr [eax+0x14]
		}
	}
	switch(cmd->Type){
		// 4 is called periodically and may be used to set a cursor
		case 4:{ // show hint
			if(body->HintItemId<1) RETURN(1)
			void (__fastcall *fn2)(Byte *,int,_MouseStr_ *cmd); *(Dword *)&fn2=0x602920;
			fn2(mman,0,cmd);
			int previt=*(int *)&mman[0x3C];
			if(previt==cmd->Item) RETURN(1)
			*(int *)&mman[0x3C]=cmd->Item;
			// process item hint here
//        char *hint="This is a Hint";
			char *hint=HDlg::GetHint(body,cmd->Item);
			char *buf=(char *)0x697428;
			if(hint==NULL) buf[0]=0;
			else strncpy(buf,hint,0x1FF);
			void (__fastcall *fn3)(_Body_ *,int,int,int,int,char *buf); *(Dword *)&fn3=0x5FF400;
			fn3(body,0,0x200,3,body->HintItemId/*hint item index*/,buf);
			void (__fastcall *fn4)(_Body_ *,int,int,int,int); *(Dword *)&fn4=(*(Dword **)body->Obj)[5];
			fn4(body,0,0,7,8);
			void (__fastcall *fn5)(Byte *,int,int,int,int,int); *(Dword *)&fn5=0x603190;
			int p1=*(int *)&body->Obj[0x1C] + 0x16B;
			int p2=*(int *)&body->Obj[0x18] + 8;
//__asm int 3
			fn5(mman,0,p2,p1,0x138,0x11);
			RETURN(1)}
		case 0x200: // mouse push/pop
//__asm int 3
			Dlg_Cmd=cmd;
			Dlg_User_Command=0;
//__asm int 3
			DlgCallBack(body->Id,cmd->Item,cmd->SType);
//__asm int 3
			if(Dlg_User_Command){
				switch(Dlg_User_Command){
					case 1:
						cmd->Type=0x200;
						*(int *)&mman[0x38]=cmd->Item;
						cmd->Item=10;
						cmd->SType=10;
						RETURN(2)
				}
			}
			Dlg_Cmd=NULL;
/*
			switch(cmd->SType){
				case 12: // left mouse push
					DlgCallBack(body->Id,cmd->Item,12);
					break;
				case 13: // left mouse pop
					if(cmd->Item==0x7802){
						cmd->Type=0x200;
						*(int *)&mman[0x38]=cmd->Item;
						cmd->Item=10;
						cmd->SType=10;
						RETURN(2)
					}
					DlgCallBack(body->Id,cmd->Item,13);
					break;
				case 14: // right mouse push
					DlgCallBack(body->Id,cmd->Item,14);
					break;
			}
*/
			RETURN(1)
		default:
			break;
	}

	RETURN(1)
/*
	if(cmd->Flags&0x200){ // right mouse click
		if((cmd->SType!=12) && (cmd->SType!=14)) RETURN(1)
		if(cmd->Item<0) RETURN(1)
		// action or pop up help for right mouse click
		// ...
	}
	ret=1;
	if(cmd->Type==1) RETURN(ret)
	if(cmd->Type!=0x200) RETURN(ret)
	if(cmd->SType==12){ // left mouse push
	
// exit with item:
//.text:005B397B                     mov edx, [edi+_MouseStr_.Item]
//.text:005B397E                     mov [edi+_MouseStr_.Type], 200h
//.text:005B3984                     mov eax, MouseManagerPo // 6992D0
//.text:005B3989                     mov [eax+38h], edx
//.text:005B398C                     mov eax, 0Ah
//.text:005B3991                     mov [edi+_MouseStr_.Item], eax
//.text:005B3994                     mov [edi+_MouseStr_.SubType], eax
//  return 2
	}
	if(cmd->SType==13){ // left mouse release
	}
	RETURN(ret)
	*/
}

//int HDlg::VideoX;
//int HDlg::VideoY;
//int HDlg::VideoInd;

Dword HDlg::VTable[15]={
	 0x5D2900, // Dctor
	 0x5FF0A0, // Activate
	 0x5FF220, // Disactivate
	0x405610,  
	0x49A230,
		0x5FF5E0,
	0x5FFA20,
	0x5FFB30,
	0x5FFBB0,
	 /*0x05D7F30,*/ 0x5D29A0, // action ???
	0x5FFCA0,
	0x5FFD50,
	0x5FFE90,
	0x4842C0,
	0x41B0F0
};

int HDlg::AddHint(int id, char *text){
	STARTNA(__LINE__, 0)
	int i;
	for(i=0;i<Body->NumOfItems;i++){ 
		if(Body->Hints[i].ItemId==id) goto _Found;
	}
	for(i=0;i<Body->NumOfItems;i++){ 
		if(Body->Hints[i].ItemId==0) goto _Found;
	}
	RETURN(-1)
_Found:
	Body->Hints[i].ItemId=id;
	DeleteText(Body->Hints[i].Text);
	if (IsDynamicText(text)) text = NewString(text);
	Body->Hints[i].Text=text;
	RETURN(0)
}

char *HDlg::GetHint(_Body_ *body,int id){
	STARTNA(__LINE__, 0)
	int num=body->NumOfItems;
	for(int i=0;i<num;i++){ 
		if(body->Hints[i].ItemId==id){
			char *txt=body->Hints[i].Text;
			if(!IsDynamicText(txt)) txt = ERM2String(txt,1,0);
			RETURN(txt)
//RETURN(body->Hints[i].Text)
		}
	}
	RETURN(NULL)
}

//  3,->text
//  8,??? [text item]
//  4,# установить кадр в DEF
//  5(уст)/6(сброс),??? flag bits (поле +x16)
//  9,->Def name         \
// 13,индекс акт. игрока | последовательно для DEFа
//  2,0 ???  уст. кадр   /
// 11,->PCX имя
// 53,???
void HDlg::SendCmd2Item(int cmd,int item,Dword par){
	STARTNA(__LINE__, 0)
//  void (__fastcall *fn)(_Body_ *,int,int type,int cmd,int item,Dword par); *(Dword *)&fn=0x5FF400;
//  fn(Body,0,0x200,cmd,item,par);
//  void (__fastcall *fn2)(_Body_ *,int,int p1,int item,int item2); *(Dword *)&fn2=VTable[5];
//  fn2(Body,0,0,item,item);
		void (__fastcall *fn3)(_Body_ *,int,int,int,int,Dword); *(Dword *)&fn3=0x5FF400;
		fn3(Body,0,0x200,cmd,item,par);
//    void (__fastcall *fn4)(_Body_ *,int,int,int,int); *(Dword *)&fn4=(*(Dword **)Body->Obj)[5];
//    fn4(Body,0,0,item,item);
	RETURNV
}
void HDlg::SendCmd2AllItems(int cmd,int item,Dword par){
	STARTNA(__LINE__, 0)
	_MouseStr_ cmdstr; SetMem(&cmdstr,sizeof(cmdstr),0);
	cmdstr.Type=0x200;
	cmdstr.SType=cmd;
	cmdstr.Param=par;
	cmdstr.Item=item;
	void (__fastcall *fn3)(_Body_ *,int,_MouseStr_ *); *(Dword *)&fn3=0x5FF3A0;
	fn3(Body,0,&cmdstr);
	RETURNV
}
void HDlg::Refresh(int item){
	STARTNA(__LINE__, 0)
	Byte *mman=*(Byte **)0x6992D0;
	void (__fastcall *fn5)(Byte *,int,int,int,int,int); *(Dword *)&fn5=0x603190;
	int x =*(int *)&Body->Obj[0x18];
	int y =*(int *)&Body->Obj[0x1C];
	int dx,dy;
	if(item==-1){ // redraw all dialog
		dx=*(int *)&Body->Obj[0x20];
		dy=*(int *)&Body->Obj[0x24];
	}else{
		DlgItem *ip=FindItemByID(item);
		if(ip==NULL) RETURNV
		x+=*(short *)&ip[0x18]; // x
		y+=*(short *)&ip[0x1A]; // y
		dx=*(short *)&ip[0x1C]; // dx
		dy=*(short *)&ip[0x1E]; // dy
	}
	fn5(mman,0,x,y,dx,dy);
	RETURNV
}

HDlg *HDlg::Find(int ID){
	STARTNA(__LINE__, 0)
	HDlg *cur=Last;
	while(cur!=NULL){
		if(cur->Body->Id==ID) break; // found
		cur=cur->Next;
	}
	RETURN(cur);
}

int HDlg::LoadFromText(int id,char *textfilename){
	STARTNA(__LINE__, 0)
	if(!Flags) RETURN(-1)
	TxtFile tf;
	LoadTXT(textfilename,&tf);
//  if(TF==NULL){
//    TF=new TxtFile;
//    LoadTXT(textfilename,TF);
//  }
	// skip 2 first lines
	// first column is for comments
	int i,sn=tf.sn;
	for(i=2;i<sn;i++){
		if(ITxt(i,1,&tf)[0]=='#'){
			if(atoi(ITxt(i,2,&tf))==id) goto _Found;
		}
	}
	RETURN(-2)
_Found:  
	// line : comment,#,Id,x,y,dx,dy,NumOfItems,BorderColor,bcgrPCX
	int x0 =atoi(ITxt(i,3,&tf));
	int y0 =atoi(ITxt(i,4,&tf));
	int dx=atoi(ITxt(i,5,&tf));
	int dy=atoi(ITxt(i,6,&tf));
	if(x0==-1) x0=(800-dx)>>1;
	if(y0==-1) y0=(600-dy)>>1;
	int ni=atoi(ITxt(i,7,&tf));
	int bc=atoi(ITxt(i,8,&tf));
	char *PCXbcgr=ITxt(i,9,&tf);
	int hinttxtitem=atoi(ITxt(i,10,&tf));
	Initialize(x0,y0,dx,dy,ni);
	DlgItem *it=0;
	int bkitems=100000;
	if(PCXbcgr[0]==0){ // default auto size background
		bkitems=BuildDefaultBackPcx(dx,dy,bc);
	}else{ // specific background
		it=BuildPcx(0,0,dx,dy,0,PCXbcgr,0x800); 
		if(bc==-1) bc=CurrentUser(); if(bc<0) bc=0;
		AdjustPlayerColor(it,bc);
		AddItem(it);
	}
	Body->HintItemId=hinttxtitem;
	// parse other items
	char *IFileName=0,*IFontName=0,*text=0,*hint=0;
	int flags=0,cadre,color,color2,align,vidnum;
	int find,x,y;
	for(++i;i<sn;i++){
		char *str=ITxt(i,1,&tf);
		if(str[0]=='#') break; // # means next dialog or end of this dialog
		if(str[0]==';') continue; // ; means a commented item
		int iid=atoi(ITxt(i,2,&tf));
		find=3;
		switch(str[0]){
			case 'P': // PCX
				x =atoi(ITxt(i,find++,&tf));
				y =atoi(ITxt(i,find++,&tf));
				dx=atoi(ITxt(i,find++,&tf));
				dy=atoi(ITxt(i,find++,&tf));
				IFileName=ITxt(i,find++,&tf);
				flags=atoi(ITxt(i,find++,&tf));
				it=BuildPcx(x,y,dx,dy,iid,IFileName,flags); 
				AddItem(it);
				break;
			case 'D': // DEF cadre
				x =atoi(ITxt(i,find++,&tf));
				y =atoi(ITxt(i,find++,&tf));
				dx=atoi(ITxt(i,find++,&tf));
				dy=atoi(ITxt(i,find++,&tf));
				IFileName=ITxt(i,find++,&tf);
				cadre=atoi(ITxt(i,find++,&tf));
				flags=atoi(ITxt(i,find++,&tf));
				it=BuildDef(x,y,dx,dy,iid,IFileName,cadre,0,0,0,flags);
				AddItem(it);
				break;
			case 'A': // Animated Def
/*
				x =atoi(ITxt(i,3,&tf));
				y =atoi(ITxt(i,4,&tf));
				dx=atoi(ITxt(i,5,&tf));
				dy=atoi(ITxt(i,6,&tf));
				IFileName=ITxt(i,7,&tf);
				int phase=atoi(ITxt(i,8,&tf));
				flags=atoi(ITxt(i,9,&tf));
				it=BuildPcx(x,y,dx,dy,iid,IFileName,flags); 
				it=BuildDef(x,y,dx,dy,iid,IFileName,cadre,0,0,0,flags);
				AddItem(it);
*/
				break;
			case 'B': // Button (DEF2)
				x =atoi(ITxt(i,find++,&tf));
				y =atoi(ITxt(i,find++,&tf));
				dx=atoi(ITxt(i,find++,&tf));
				dy=atoi(ITxt(i,find++,&tf));
				IFileName=ITxt(i,find++,&tf);
				flags=atoi(ITxt(i,find++,&tf));
				it=BuildDef2(x,y,dx,dy,iid,IFileName,0,1,1,/*0x1C*/1,flags);
				AddItem(it);
				break;
			case 'T': // Text
				x =atoi(ITxt(i,find++,&tf));
				y =atoi(ITxt(i,find++,&tf));
				dx=atoi(ITxt(i,find++,&tf));
				dy=atoi(ITxt(i,find++,&tf));
				text=ITxt(i,find++,&tf); if(text[0]==0) text=0;
				IFontName=ITxt(i,find++,&tf);
				color =atoi(ITxt(i,find++,&tf));
				color2=atoi(ITxt(i,find++,&tf));
				align=atoi(ITxt(i,find++,&tf));
				flags=atoi(ITxt(i,find++,&tf));
				it=BuildTxt(x,y,dx,dy,text,IFontName,color,iid,align,color2,flags);
				AddItem(it);
				break;
			case 'S': // Scrolled Text
				x =atoi(ITxt(i,find++,&tf));
				y =atoi(ITxt(i,find++,&tf));
				dx=atoi(ITxt(i,find++,&tf));
				dy=atoi(ITxt(i,find++,&tf));
				text=ITxt(i,find++,&tf); if(text[0]==0) text=0;
				IFontName=ITxt(i,find++,&tf);
				color =atoi(ITxt(i,find++,&tf));
				color2=atoi(ITxt(i,find++,&tf));
				it=BuildTxtSB(x,y,dx,dy,text,IFontName,color,color2);
				AddItem(it);
				break;
			case 'E': // Edited Text
				x =atoi(ITxt(i,find++,&tf));
				y =atoi(ITxt(i,find++,&tf));
				dx=atoi(ITxt(i,find++,&tf));
				dy=atoi(ITxt(i,find++,&tf));
				text=ITxt(i,find++,&tf); if(text[0]==0) text=0;
				IFontName=ITxt(i,find++,&tf);
				it=BuildTxtED(x,y,dx,dy,10,text,IFontName,4,5,0,0,iid,0,4,0,0);
				AddItem(it);
				break;
			case 'V': // Video
				x =atoi(ITxt(i,find++,&tf))+x0;
				y =atoi(ITxt(i,find++,&tf))+y0;
				vidnum =atoi(ITxt(i,find++,&tf));
				SetVideo(vidnum,x,y);
				break;
			default:
				continue;
		}
		hint=ITxt(i,find++,&tf);
		if(hint[0]!=0) AddHint(iid, hint);
	}
	Body->Id=id;
	LoadAllItems();
//  int bkitcolor=CurrentUser();
//  void (__fastcall *fn3)(_Body_ *,int,int,int,int,Dword); *(Dword *)&fn3=0x5FF400;
//  for(i=100000;i<bkitems;i++){ 
//    fn3(Body,0,0x200,13,i,bkitcolor);
//  }

//  UnloadTXT(&tf);
	RETURN(0)
}

///////////////////////// 
// Dialog ERM service
int ERM_Dlg(char Cmd,int Num,_ToDo_*sp,Mes *Mp)
{
//  STARTNA(__LINE__,&Mp->m.s[Mp->i])
	STARTNA(__LINE__, 0)
	int ind,id,vv;
	switch(Cmd){
		case 'C': // set callback action
			// C#
			Apply(&Dlg_User_Command,4,Mp,0);
			RETURN(1)
		case 'P': // show cast spell at adventure map
			// P#
			Apply(&ind,4,Mp,0);
			ShowMapSpellDlg(ind);
			RETURN(1)
	}
	HDlg *dlg=NULL;
	ind=GetVarVal(&sp->Par[0]);
	dlg=HDlg::Find(ind);
	ERMFlags[0]=0;

	switch(Cmd){
		case 'N': // create new dialog from template
			//NdlgTXTfilename
			if(dlg==NULL){ // not found - try to load
				dlg=new HDlg(0);
				if(dlg==NULL){ MError("\"DL:N\"- no memory."); RETURN(0) }
				char *FileName; //="ZSTDDLGS.TXT";
				if(Mp->VarI[0].Num!=0){ // переменная
					Apply(&vv,4,Mp,0);
					FileName = GetErmString(vv);
					if (FileName == 0) RETURN(0);
				}else{           // копируем сам текст
//          MError("\"!!DL:H\"-cannot use direct ERM text (z vars only)"); RETURN(0)
					FileName=ERM2String(&Mp->m.s[Mp->i],0,&vv);
					Mp->i+=vv;
				}
				if(dlg->LoadFromText(ind,FileName)==0) ERMFlags[0]=1;
			}else ERMFlags[0]=1;
			break;
		case 'S':{ // show
			// S$
			if(dlg==NULL) break; // not found
//      dlg->LoadAllItems();
			dlg->Show(0);
			Byte *mman=*(Byte **)0x6992D0;
			vv=*(int *)&mman[0x38];
			Apply(&vv,4,Mp,0);
			delete dlg;
			break;}
//    case 'H': // hide
//      break;
		case 'E':{ // enable/disable item
			// Eid/ENdis
			CHECK_ParamsMin(2);
			if(dlg==NULL) break; // not found
			id=0; Apply(&id,4,Mp,0);
			DlgItem *dlgit=dlg->FindItemByID(id); if(dlgit==NULL) break;
			vv=1; Apply(&vv,4,Mp,1);
			dlg->EnableItem(dlgit,vv);
			ERMFlags[0]=1;
			break;}
		case 'H':{ // set item hint
			// Hid/hint
			if(dlg==NULL) break; // not found
			id=0; Apply(&id,4,Mp,0);
			char *hint=0;
			if(Mp->VarI[1].Num!=0){ // переменная
				Apply(&vv,4,Mp,1);
				hint = GetInternalErmString(vv);
			}
			else
			{ 
				if (Mp->m.s[Mp->i] != STRCH) { EWrongSyntax(); RETURN(0)}
				int L;
				hint = ERM2String(&Mp->m.s[Mp->i], 0, &L);
				Mp->i += L;
			}
			dlg->AddHint(id, hint);
			break;}
		case 'A':{ // send command to control
			if(dlg==NULL) break; // not found
			id=0; Apply(&id,4,Mp,0); // item
			int cmd=0; Apply(&cmd,4,Mp,1); // command
			Dword par=0; Apply(&vv,4,Mp,2); // parameter
			int refresh=0; Apply(&refresh,4,Mp,3); // need refresh
			if(Mp->VarI[2].Type==7){ // z var
				if ((*(char**)&par = GetErmString(vv)) == 0) RETURN(0);
			}else{
				par=vv;
			}
//__asm int 3
//      dlg->SendCmd2Item(cmd,id,par);
			dlg->SendCmd2AllItems(cmd,id,par);
			if(refresh) dlg->Refresh(id);
			ERMFlags[0]=1;
			break;}
		default: EWrongCommand(); RETURN(0)
	}
	RETURN(1)
}

/*
//          __asm int 3
					HDlg *dlg=new HDlg(0);
					dlg->Initialize(10,20,329,388,50);
					DlgItem *it=0;
					it=dlg->BuildPcx(0,0,329,388,0,"TPShip.pcx",0x800); 
					dlg->AdjustPlayerColor(it,0);
					dlg->AddItem(it);
					it=dlg->BuildTxt(0,15,329,30,"Hi!","bigfont.fnt",7,1,1,0,8); dlg->AddItem(it);
					it=dlg->BuildDef(250,30,32,32,-1,"resource.def",6,0,0,0,0x10); dlg->AddItem(it);
//dlg->SetADef(it);
					it=dlg->BuildPcx(100,69,128,96,5,"tpshipbk.pcx",0x800); dlg->AddItem(it);
					it=dlg->BuildDef(116,85,96,64,66,"AB02_.def",0,7,0,0,0x10); dlg->AddItem(it);
//*(DlgItem **)&dlg->Body[0x64]=it;
					dlg->SetADef(it);
					it=dlg->BuildDef(10,85,96,64,6,"AB02_.def",0,7,0,0,0x10); dlg->AddItem(it);
					dlg->SetADef(it);
					it=dlg->BuildDef(210,85,96,64,666,"AB02_.def",0,7,0,0,0x10); dlg->AddItem(it);
					dlg->SetADef(it);
					it=dlg->BuildTxtSB(100,69,128,96,"111111111\n222222222\n333333333333333\n4444444444444\n555555555555555555555555555555555\n6\n7\n8\n9\n10\n11\n12","medfont.fnt",0,1); dlg->AddItem(it);
					it=dlg->BuildTxtED(20,20,101,37,10,"99999","bigfont.fnt",4,5,0,0,99,0,4,0,0); dlg->AddItem(it);
					
					it=dlg->BuildDef2(42,312,64,30,0x7802,"iBUY30.def",0,1,1,0x1C,2); dlg->AddItem(it);
					it=dlg->BuildDef2(224,312,64,30,0x7801,"iCancel.def",0,1,1,1,2); dlg->AddItem(it);
					// status bar
//          it=dlg->BuildPcx(8,363,312,17,77,"StatBar.pcx",0x800); dlg->AddItem(it);
					it=dlg->BuildTxt(8,363,312,17,0,"smalfont.fnt",1,88,1,0,8); dlg->AddItem(it);

					dlg->LoadAllItems();
					dlg->SetVideo(5,50,200);
//          dlg->SetVideo(5,50,0);

					dlg->Show(0);
//          dlg->Kill(1);
					delete dlg;
*/


// transparent dialog over a map:
static int ShowMapSpellDlg_DlgNum=0;
static int CallCallBackERMFunction(int act,int x,int y,int l){
	STARTNA(__LINE__, 0)
	int   i;
	int   OldX[16];
	for(i=0;i<16;i++) { OldX[i]=ERMVarX[i]; }
	ERMVarX[0]=act;
	ERMVarX[1]=x;
	ERMVarX[2]=y;
	ERMVarX[3]=l;
	FUCall(ShowMapSpellDlg_DlgNum);
	for(i=0;i<16;i++) { ERMVarX[i]=OldX[i]; }
	RETURN(0)
}

Dword __fastcall TranspDlgOverMap(void *body,int,_MouseStr_ *cmd){
	STARTNA(__LINE__, 0)
	int (__fastcall *dlg_StdProcCmd)(void *dlg,int,_MouseStr_ *cmd); *(Dword *)&dlg_StdProcCmd=0x41B120;
	int ret=dlg_StdProcCmd(body,0,cmd);
	if(ret) RETURN(ret);
	Byte *mman=*(Byte **)0x6992D0;
	Byte *ustr=*(Byte **)0x6992B8;
	int  ct=getTime();
	int *nt=(int *)0x6989E8;
//  if(*nt==0){ *nt=ct+100; RETURN(1) }
	int  dif=ct- *nt;
	if(dif>0){
		__asm{
			mov  ecx,ustr
			push 0
			mov  eax,0x40F7D0
			call eax
			mov  ecx,ustr
			push 0
			push 0
			mov  eax,0x40F1D0
			call eax
		}
	}
	switch(cmd->Type){
		case 1:
			if(cmd->SType==1) RETURN(1)
			*(int *)&mman[0x38]=0;
			cmd->Type=0x200;
			cmd->SType=10;
			cmd->Item=10;
			RETURN(2)
		case 4:{ // show hint
			// 4 is called periodically and may be used to set a cursor
			int (__fastcall *fn1)(Byte *,int,int x,int y); *(Dword *)&fn1=0x407A20;
			if(fn1(ustr,0,cmd->Xabs,cmd->Yabs)==0){
				// out of map
				*(int *)&mman[0x38]=0;
				SetMouseCursor(0,1);
				RETURN(1)
			}
			int xm=cmd->Xabs>>5,ym=cmd->Yabs>>5,*oxm=(int *)&ustr[0xEC],*oym=(int *)&ustr[0xF0];
			// the same square ?
			if((*oxm==xm) && (*oym==ym)) RETURN(1)
			*oxm=xm; *oym=ym;  
			void (__fastcall *fn2)(Byte *,int,int *packedmapitem); *(Dword *)&fn2=0x407A70;
			int mapitem;
			fn2(ustr,0,&mapitem);
//      _MapItem_ *(__fastcall *fn3)(Byte *,int,int packedmapitem); *(Dword *)&fn3=0x412B30;
//      _MapItem_ *mip=fn3(ustr,0,mapitem);
			// set cursor if needed
			// if cursor is not standard arrow, so *(int *)&mman[0x38]=1; - не обязательно
			// if a standard *(int *)&mman[0x38]=0;
			int x,y,l;
			MixedPos(&x,&y,&l,mapitem);
			Dlg_Cmd=cmd;
			Dlg_User_Command=0;
//      DlgCallBack(ShowMapSpellDlg_DlgNum,x,y);
//      FUCall(ShowMapSpellDlg_DlgNum);
			CallCallBackERMFunction(0,x,y,l);
			if(Dlg_User_Command){
				if(Dlg_User_Command>=1000){
					SetMouseCursor(Dlg_User_Command-1000,1);
				}
			}
			Dlg_Cmd=NULL;
			RETURN(1)}
		case 0x200:{ // mouse push/pop
//__asm int 3
//      int *lxm=(int *)&ustr[0xEC],*lym=(int *)&ustr[0xF0];
			void (__fastcall *fn2)(Byte *,int,int *packedmapitem); *(Dword *)&fn2=0x407A70;
			int mapitem;
			fn2(ustr,0,&mapitem);
//      _MapItem_ *(__fastcall *fn3)(Byte *,int,int packedmapitem); *(Dword *)&fn3=0x412B30;
//      _MapItem_ *mip=fn3(ustr,0,mapitem);
			int x,y,l;
			MixedPos(&x,&y,&l,mapitem);
			Dlg_Cmd=cmd;
			Dlg_User_Command=0;
//__asm int 3
//      DlgCallBack(ShowMapSpellDlg_DlgNum,cmd->Item,cmd->SType);
			CallCallBackERMFunction(cmd->SType,x,y,l);
//__asm int 3
			if(Dlg_User_Command){
				if(Dlg_User_Command>=1000){
					SetMouseCursor(Dlg_User_Command-1000,1);
					RETURN(1)
				}
				switch(Dlg_User_Command){
					case 1:
						cmd->Type=0x200;
						*(int *)&mman[0x38]=0;
						cmd->Item=10;
						cmd->SType=10;
						RETURN(2)
				}
			}
			Dlg_Cmd=NULL;
/*
			switch(cmd->SType){
				case 12: // left mouse push
					DlgCallBack(body->Id,cmd->Item,12);
					break;
				case 13: // left mouse pop
					if(cmd->Item==0x7802){
						cmd->Type=0x200;
						*(int *)&mman[0x38]=cmd->Item;
						cmd->Item=10;
						cmd->SType=10;
						RETURN(2)
					}
					DlgCallBack(body->Id,cmd->Item,13);
					break;
				case 14: // right mouse push
					DlgCallBack(body->Id,cmd->Item,14);
					break;
			}
*/
			RETURN(1)}
		default:
			break;
	}

	RETURN(1)
}

Dword ShowMapSpellDlg_VTable[15]={
	0x4917E0, // Dctor
	0x5FF0A0, // Activate
	0x5FF220, // Disactivate
	0x405610,  
	0x49A230, // null proc
	0x5FF5E0,
	0x5FFA20,
	0x5FFB30,
	0x5FFBB0,
	(Dword)TranspDlgOverMap/*0x491880*/, // action
	0x5FFCA0,
	0x5FFD50,
	0x5FFE90,
	0x4842C0,
	 0x491A30
};

void ShowMapSpellDlg(int dlgnum){
	Byte Dlg[160]; // 108 byte but for any case
	// build dlg
	__asm{
		lea  ecx,Dlg
		mov  eax,0x491670
		call eax
	}
	*(Dword *)&Dlg[0]=(Dword)ShowMapSpellDlg_VTable;
	ShowMapSpellDlg_DlgNum=dlgnum;
	// show dlg
	__asm{
		lea  ecx,Dlg
		xor  eax,eax
		push eax
		mov  eax,0x5FFA20
		call eax
	}
	ShowMapSpellDlg_DlgNum=0;
	// destroy dlg
	*(Dword *)&Dlg[0]=0x63DBD8;
	__asm{
		lea  ecx,Dlg
		mov  eax,0x491810
		call eax
	}

}
