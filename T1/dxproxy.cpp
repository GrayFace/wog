#include <stdio.h>
#include <direct.h> 
#include <time.h>
#include "global.h"
#include "dxproxy.h"
#include "structs.h"

//#define __FILENUM__ 26

#define HeroesW 800
#define HeroesH 600
#define HeroesWnd MainWindow

//DirectDraw start
//>	WogDll.exe!00601ad1() 	
px_IDirectDraw::px_IDirectDraw(LPDIRECTDRAW lplpDD)
{
	px_DxDraw=lplpDD;
	iClipperCounter=0;
//	lpipx_DxClipper=NULL;
	lpipx_DxPrimarySurface=NULL;
	lpipx_DxAddPrimarySurface=NULL;
	lpipx_DxOtherPrimarySurface=NULL;
	//hWnd=*((HWND*)0x699650);
	HDC deskhdc=GetDC(GetDesktopWindow());
		Width=GetDeviceCaps(deskhdc,HORZRES);
		Height=GetDeviceCaps(deskhdc,VERTRES);
		BPP=GetDeviceCaps(deskhdc,BITSPIXEL);
//		addlog("w",Width);
//		addlog("h",Height);
//		addlog("b",BPP);
	ReleaseDC(GetDesktopWindow(),deskhdc);
}
px_IDirectDraw::~px_IDirectDraw()
{
	 
//	if(lpipx_DxClipper)
//		delete lpipx_DxClipper;
	if(lpipx_DxAddPrimarySurface)
		lpipx_DxAddPrimarySurface->Release();
	if(lpipx_DxOtherPrimarySurface)
		lpipx_DxOtherPrimarySurface->Release();
}

void px_IDirectDraw::MakeScreenShot()
{
	mkdir("Screenshots");
	char fname[256];
	int i;
	HRESULT hRes;
	unsigned int tm=time(NULL);unsigned int ms=GetTickCount()%1000;
	sprintf(fname,".\\Screenshots\\%010d%04d.bmp",tm,ms);
	FILE *f=fopen(fname,"wb");
	if(f)
	{
		void* buffer = malloc(HeroesW*2);
		BMPhead sBMPH;
		RECT rect;rect.top=0;rect.left=0; 
		rect.bottom=HeroesH;
		rect.right=HeroesW; 
		ZeroMemory(&sBMPH,sizeof(BMPhead));
		sBMPH.SignBMP=0x4D42;
		sBMPH.NumBPP=16;
		sBMPH.OffsetIMG=sizeof(BMPhead);//+256*sizeof(unsigned long)
		sBMPH.SizeID=HeroesW*HeroesH;
		sBMPH.SizeBMP=sBMPH.OffsetIMG+sBMPH.SizeID;
		sBMPH.SizeBIH=40;
		sBMPH.Width=HeroesW;
		sBMPH.Height=HeroesH;
		sBMPH.NumPI=1;
		sBMPH.MaskR=0xF800;
		sBMPH.MaskG=0x07E0;
		sBMPH.MaskB=0x001F;
   	 	sBMPH.CompType=BI_BITFIELDS;
		fwrite(&sBMPH,sizeof(BMPhead),1,f);
		DDSURFACEDESC2 DDSurfaceDesc;
		ZeroMemory(&DDSurfaceDesc,sizeof(DDSURFACEDESC2)); 
		DDSurfaceDesc.dwSize=sizeof(DDSURFACEDESC2);
		if(px_DxObj->lpipx_DxPrimarySurface->px_DxSurface->IsLost())
			px_DxObj->lpipx_DxPrimarySurface->px_DxSurface->Restore(); 
		hRes=px_DxObj->lpipx_DxPrimarySurface->px_DxSurface->Lock(NULL,&DDSurfaceDesc,DDLOCK_WAIT|DDLOCK_DISCARDCONTENTS|DDLOCK_READONLY,NULL);
		if(hRes!=DD_OK)
		{
			fclose(f);
			return;
		}
		unsigned long nulll=0;
		for(i=HeroesH-1;i>=0;i--)
		{
			memcpy(buffer,((char*)DDSurfaceDesc.lpSurface)+DDSurfaceDesc.lPitch*i, HeroesW*2);
			fwrite(buffer, HeroesW*2, 1, f);;
		}
		hRes=px_DxObj->lpipx_DxPrimarySurface->px_DxSurface->Unlock(NULL);
		if(px_DxObj->lpipx_DxPrimarySurface->px_DxSurface->IsLost())
				px_DxObj->lpipx_DxPrimarySurface->px_DxSurface->Restore(); 
		fclose(f);
		free(buffer);
	}
}

HRESULT __stdcall px_IDirectDraw::QueryInterface(REFIID riid,LPVOID* obp){return px_DxDraw->QueryInterface(riid,obp);}
ULONG	__stdcall px_IDirectDraw::AddRef(){return px_DxDraw->AddRef();}
ULONG	__stdcall px_IDirectDraw::Release()
{
	ULONG ref = px_DxDraw->Release();
	//if(ref <= 0)
	//	delete this;
	return ref;
}
HRESULT __stdcall px_IDirectDraw::Compact(){return px_DxDraw->Compact();}
// 	WogDll.exe!00601c10() 	
HRESULT __stdcall px_IDirectDraw::CreateClipper(DWORD dwFlags,LPDIRECTDRAWCLIPPER FAR *lplpDDClipper,IUnknown FAR *pUnkOuter)
{
//	addlog("clip",1);
	if(lplpDDClipper)
	{
		HRESULT hRes=px_DxDraw->CreateClipper(dwFlags,lplpDDClipper,pUnkOuter);
		lpipx_DxClipper[iClipperCounter]=new px_IDirectDrawClipper(*lplpDDClipper);
		*lplpDDClipper=(LPDIRECTDRAWCLIPPER)lpipx_DxClipper[iClipperCounter];
		//*lplpDDClipper=(LPDIRECTDRAWCLIPPER)new px_IDirectDrawClipper(*lplpDDClipper);
		iClipperCounter++;
		return hRes;
	}	
	return px_DxDraw->CreateClipper(dwFlags,lplpDDClipper,pUnkOuter);
}
HRESULT __stdcall px_IDirectDraw::CreatePalette(DWORD dwFlags,LPPALETTEENTRY lpColorTable,LPDIRECTDRAWPALETTE FAR* lplpDDPalette,IUnknown FAR *pUnkOuter){return px_DxDraw->CreatePalette(dwFlags,lpColorTable,lplpDDPalette,pUnkOuter);}
// 	WogDll.exe!00600c0d() 	
HRESULT __stdcall px_IDirectDraw::CreateSurface(LPDDSURFACEDESC lpDDSurfaceDesc,LPDIRECTDRAWSURFACE FAR *lplpDDSurface,IUnknown FAR *pUnkOuter)
{
	HRESULT hRes;
	if(lpDDSurfaceDesc->ddsCaps.dwCaps&DDSCAPS_PRIMARYSURFACE)
	{
		hRes=px_DxDraw->CreateSurface(lpDDSurfaceDesc,lplpDDSurface,pUnkOuter);
		if(hRes!=DD_OK||!(*lplpDDSurface))
			return hRes;
		if(lpipx_DxAddPrimarySurface)
			lpipx_DxAddPrimarySurface->Release();
		lpipx_DxAddPrimarySurface=new px_IDirectDrawSurface7((LPDIRECTDRAWSURFACE7)*lplpDDSurface);
//		addlog("ADDPRIMARY",(int)lpipx_DxAddPrimarySurface);
		isPrimarySurface=true;
	}
	else
		isPrimarySurface=false;
	lpDDSurfaceDesc->dwFlags|=DDSD_PIXELFORMAT;
	ZeroMemory(&lpDDSurfaceDesc->ddpfPixelFormat,sizeof(DDPIXELFORMAT));
	lpDDSurfaceDesc->ddpfPixelFormat.dwSize=sizeof(DDPIXELFORMAT);
	lpDDSurfaceDesc->ddpfPixelFormat.dwFlags=DDPF_RGB;
	lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount=16;
	lpDDSurfaceDesc->ddpfPixelFormat.dwRBitMask=0xf800;
	lpDDSurfaceDesc->ddpfPixelFormat.dwGBitMask=0x7e0;
	lpDDSurfaceDesc->ddpfPixelFormat.dwBBitMask=0x1f;
	if(isPrimarySurface)
	{
		lpDDSurfaceDesc->dwWidth=HeroesW;
		lpDDSurfaceDesc->dwHeight=HeroesH;
		lpDDSurfaceDesc->dwFlags=DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT;//4103;
		lpDDSurfaceDesc->ddsCaps.dwCaps=DDSCAPS_OFFSCREENPLAIN|DDSCAPS_SYSTEMMEMORY;//2112;
	}
	hRes=px_DxDraw->CreateSurface(lpDDSurfaceDesc,lplpDDSurface,NULL);
	if(hRes!=DD_OK||!(*lplpDDSurface))
		return hRes;
	LPPX_DIRECTDRAWSURFACE7 px_Surf=new px_IDirectDrawSurface7((LPDIRECTDRAWSURFACE7)*lplpDDSurface);
	if(pUnkOuter)
	{
		*lplpDDSurface=(LPDIRECTDRAWSURFACE)px_Surf;
		return hRes;
	}
	if(isPrimarySurface)
	{
//		addlog("PRIMARY",(int)px_Surf);
		lpipx_DxPrimarySurface=px_Surf;
		lpDDSurfaceDesc->dwFlags-=DDSD_PIXELFORMAT;
		px_DxDraw->CreateSurface(lpDDSurfaceDesc,lplpDDSurface,NULL);
		if(lpipx_DxOtherPrimarySurface)
			lpipx_DxOtherPrimarySurface->Release();
		lpipx_DxOtherPrimarySurface=new px_IDirectDrawSurface7((LPDIRECTDRAWSURFACE7)*lplpDDSurface);	

		*lplpDDSurface=(LPDIRECTDRAWSURFACE)lpipx_DxPrimarySurface;
//		addlog("BACK BUFFER",lpDDSurfaceDesc->dwBackBufferCount);
	}
	else if(lpDDSurfaceDesc->ddsCaps.dwCaps&DDSCAPS_BACKBUFFER)
	{
//		addlog("BACK BUFFER",(int)px_Surf);
		lpipx_DxBackBuffer[iBackBufferCounter]=px_Surf;
		*lplpDDSurface=(LPDIRECTDRAWSURFACE)lpipx_DxBackBuffer[iBackBufferCounter];
		iBackBufferCounter++;
	}
	else
	{
//		addlog("OTHER",(int)px_Surf);
		lpipx_DxOtherSurface[iSurfaceCounter]=px_Surf;
		*lplpDDSurface=(LPDIRECTDRAWSURFACE)lpipx_DxOtherSurface[iSurfaceCounter];
		iSurfaceCounter++;
	}
	return hRes;
}
HRESULT __stdcall px_IDirectDraw::DuplicateSurface(LPDIRECTDRAWSURFACE lpDDSurface,LPDIRECTDRAWSURFACE FAR *lplpDupDDSurface)
{
	HRESULT hRes=px_DxDraw->DuplicateSurface(LPDIRECTDRAWSURFACE(LPPX_DIRECTDRAWSURFACE7(lpDDSurface)->px_DxSurface),lplpDupDDSurface);
	if(*lplpDupDDSurface&&hRes==DD_OK)
	{
		lpipx_DxOtherSurface[iSurfaceCounter]=new px_IDirectDrawSurface7((LPDIRECTDRAWSURFACE7)*lplpDupDDSurface);
		*lplpDupDDSurface=(LPDIRECTDRAWSURFACE)lpipx_DxOtherSurface[iSurfaceCounter];
		iSurfaceCounter++;
	}
	return hRes;
}
HRESULT __stdcall px_IDirectDraw::EnumDisplayModes(DWORD dwFlags,LPDDSURFACEDESC lpDDSurfaceDesc,LPVOID lpContext,LPDDENUMMODESCALLBACK lpEnumCallback){return px_DxDraw->EnumDisplayModes(dwFlags,lpDDSurfaceDesc,lpContext,lpEnumCallback);}
HRESULT __stdcall px_IDirectDraw::EnumSurfaces(DWORD dwFlags,LPDDSURFACEDESC lpDDSD,LPVOID lpContext,LPDDENUMSURFACESCALLBACK lpEnumCallback){return px_DxDraw->EnumSurfaces(dwFlags,lpDDSD,lpContext,lpEnumCallback);}
HRESULT __stdcall px_IDirectDraw::FlipToGDISurface(){return px_DxDraw->FlipToGDISurface();}
HRESULT __stdcall px_IDirectDraw::GetCaps(LPDDCAPS lpDDDriverCaps,LPDDCAPS lpDDHELCaps){return px_DxDraw->GetCaps(lpDDDriverCaps,lpDDHELCaps);}
HRESULT __stdcall px_IDirectDraw::GetDisplayMode(LPDDSURFACEDESC lpDDSurfaceDesc){return px_DxDraw->GetDisplayMode(lpDDSurfaceDesc);}
HRESULT __stdcall px_IDirectDraw::GetFourCCCodes(DWORD FAR *lpNumCodes,DWORD FAR *lpCodes){return px_DxDraw->GetFourCCCodes(lpNumCodes,lpCodes);}
HRESULT __stdcall px_IDirectDraw::GetGDISurface(LPDIRECTDRAWSURFACE FAR *lplpGDIDDSSurface){return px_DxDraw->GetGDISurface(lplpGDIDDSSurface);}
HRESULT __stdcall px_IDirectDraw::GetMonitorFrequency(LPDWORD lpdwFrequency){return px_DxDraw->GetMonitorFrequency(lpdwFrequency);}
HRESULT __stdcall px_IDirectDraw::GetScanLine(LPDWORD lpdwScanLine){return px_DxDraw->GetScanLine(lpdwScanLine);}
HRESULT __stdcall px_IDirectDraw::GetVerticalBlankStatus(LPBOOL lpbIsInVB){return px_DxDraw->GetVerticalBlankStatus(lpbIsInVB);}
HRESULT __stdcall px_IDirectDraw::Initialize(GUID FAR *lpGUID){return px_DxDraw->Initialize(lpGUID);}
HRESULT __stdcall px_IDirectDraw::RestoreDisplayMode(){return px_DxDraw->RestoreDisplayMode();}
HRESULT __stdcall px_IDirectDraw::SetCooperativeLevel(HWND hWnd,DWORD dwFlags){return px_DxDraw->SetCooperativeLevel(hWnd,dwFlags);}
HRESULT __stdcall px_IDirectDraw::SetDisplayMode(DWORD dwWidth,DWORD dwHeight,DWORD dwBpp){return px_DxDraw->SetDisplayMode(dwWidth,dwHeight,dwBpp);}	
HRESULT __stdcall px_IDirectDraw::WaitForVerticalBlank(DWORD dwFlags,HANDLE hEvent){return px_DxDraw->WaitForVerticalBlank(dwFlags,hEvent);}

//DirectDrawClipper start
px_IDirectDrawClipper::px_IDirectDrawClipper(LPDIRECTDRAWCLIPPER lpDDC){px_DxClipper=lpDDC;}
px_IDirectDrawClipper::~px_IDirectDrawClipper(){;}
HRESULT __stdcall px_IDirectDrawClipper::QueryInterface(REFIID riid,LPVOID* obp){return px_DxClipper->QueryInterface(riid,obp);}
ULONG	__stdcall px_IDirectDrawClipper::AddRef(){return px_DxClipper->AddRef();}
ULONG	__stdcall px_IDirectDrawClipper::Release(){return px_DxClipper->Release();}
HRESULT __stdcall px_IDirectDrawClipper::Initialize(LPDIRECTDRAW lpDD,DWORD dwFlags){lpipx_DxDraw=LPPX_DIRECTDRAW(lpDD);return px_DxClipper->Initialize(lpipx_DxDraw->px_DxDraw,dwFlags);}
HRESULT __stdcall px_IDirectDrawClipper::GetClipList(LPRECT lpRect,LPRGNDATA lpClipList,LPDWORD lpdwSize){return px_DxClipper->GetClipList(lpRect,lpClipList,lpdwSize);}
HRESULT __stdcall px_IDirectDrawClipper::IsClipListChanged(BOOL FAR *lpbChanged){return px_DxClipper->IsClipListChanged(lpbChanged);}
HRESULT __stdcall px_IDirectDrawClipper::SetClipList(LPRGNDATA lpClipList,DWORD dwFlags){return px_DxClipper->SetClipList(lpClipList,dwFlags);}
HRESULT __stdcall px_IDirectDrawClipper::SetHWnd(DWORD dwFlags,HWND hWnd){return px_DxClipper->SetHWnd(dwFlags,hWnd);}
HRESULT __stdcall px_IDirectDrawClipper::GetHWnd(HWND FAR *lphWnd){return px_DxClipper->GetHWnd(lphWnd);}

//DirectDrawSurface start
px_IDirectDrawSurface7::px_IDirectDrawSurface7(LPDIRECTDRAWSURFACE7 lpDDS7)
{
	px_DxSurface=lpDDS7;
}
px_IDirectDrawSurface7::~px_IDirectDrawSurface7(){;}
HRESULT __stdcall px_IDirectDrawSurface7::QueryInterface(REFIID riid,LPVOID* obp){return px_DxSurface->QueryInterface(riid,obp);}
ULONG	__stdcall px_IDirectDrawSurface7::AddRef(){return px_DxSurface->AddRef();}
ULONG	__stdcall px_IDirectDrawSurface7::Release(){return px_DxSurface->Release();}
HRESULT __stdcall px_IDirectDrawSurface7::Initialize(LPDIRECTDRAW lpDD,LPDDSURFACEDESC2 lpDDSurfaceDesc){lpipx_DxDraw=LPPX_DIRECTDRAW(lpDD);return px_DxSurface->Initialize(lpipx_DxDraw->px_DxDraw,lpDDSurfaceDesc);}
HRESULT __stdcall px_IDirectDrawSurface7::IsLost(){return px_DxSurface->IsLost();}
HRESULT __stdcall px_IDirectDrawSurface7::Restore(){return px_DxSurface->Restore();}
HRESULT __stdcall px_IDirectDrawSurface7::AddAttachedSurface(LPDIRECTDRAWSURFACE7 lpDDSAttachedSurface)
{
	if(lpDDSAttachedSurface)
		return px_DxSurface->AddAttachedSurface(LPPX_DIRECTDRAWSURFACE7(lpDDSAttachedSurface)->px_DxSurface);
	return px_DxSurface->AddAttachedSurface(lpDDSAttachedSurface);
}
HRESULT __stdcall px_IDirectDrawSurface7::DeleteAttachedSurface(DWORD dwFlags,LPDIRECTDRAWSURFACE7 lpDDSAttachedSurface)
{
	if(lpDDSAttachedSurface)
		return px_DxSurface->DeleteAttachedSurface(dwFlags,LPPX_DIRECTDRAWSURFACE7(lpDDSAttachedSurface)->px_DxSurface);
	return px_DxSurface->DeleteAttachedSurface(dwFlags,lpDDSAttachedSurface);
}
HRESULT __stdcall px_IDirectDrawSurface7::EnumAttachedSurfaces(LPVOID lpContext,LPDDENUMSURFACESCALLBACK7 lpEnumSurfacesCallback){return px_DxSurface->EnumAttachedSurfaces(lpContext,lpEnumSurfacesCallback);}
HRESULT __stdcall px_IDirectDrawSurface7::GetAttachedSurface(LPDDSCAPS2 lpDDSCaps,LPDIRECTDRAWSURFACE7 FAR *lplpDDAttachedSurface)
{
	HRESULT hRes=px_DxSurface->GetAttachedSurface(lpDDSCaps,lplpDDAttachedSurface);
	if(lplpDDAttachedSurface)
		*lplpDDAttachedSurface=(LPDIRECTDRAWSURFACE7)LPPX_DIRECTDRAWSURFACE7(*lplpDDAttachedSurface);
	return hRes;
}
// 	WogDll.exe!006007f5() 	
// 	WogDll.exe!0050db59() 	
// 	WogDll.exe!006008b1() 	
// 	WogDll.exe!00600945() 	
HRESULT __stdcall px_IDirectDrawSurface7::Blt(LPRECT lpDestRect,LPDIRECTDRAWSURFACE7 lpDDSrcSurface,LPRECT lpSrcRect,DWORD dwFlags,LPDDBLTFX lpDDBltFx)
{
	HRESULT hRes;
	if(this==px_DxObj->lpipx_DxPrimarySurface)
	{
		bool fullScreen = GetWindowLong(HeroesWnd, GWL_STYLE) & WS_BORDER == 0;
		POINT p;p.x=0;p.y=0;
		RECT rect,r;
		if(!fullScreen)
		{
			ClientToScreen(HeroesWnd,&p);
			if(lpDestRect)
			{
				rect = *lpDestRect;
				lpDestRect->left-=p.x;
				lpDestRect->right-=p.x;
				lpDestRect->top-=p.y;
				lpDestRect->bottom-=p.y;
			} else
			{
				rect.left = p.x;
				rect.top = p.y;
				rect.right = p.x + HeroesW;
				rect.bottom = p.y + HeroesH;
			}
		}
		if(lpDDSrcSurface)
			lpDDSrcSurface = LPPX_DIRECTDRAWSURFACE7(lpDDSrcSurface)->px_DxSurface;
		hRes = px_DxSurface->Blt(lpDestRect, lpDDSrcSurface, lpSrcRect, dwFlags|DDBLT_WAIT, lpDDBltFx);

		if(px_DxObj->lpipx_DxAddPrimarySurface->px_DxSurface->IsLost())
			px_DxObj->lpipx_DxAddPrimarySurface->px_DxSurface->Restore(); 
		
		if(fullScreen)
		{
			px_DxObj->lpipx_DxAddPrimarySurface->px_DxSurface->Blt(lpDestRect, 
				px_DxObj->lpipx_DxPrimarySurface->px_DxSurface, lpDestRect, DDBLT_WAIT, NULL);
		}
		else
		{
			HDC dc = GetWindowDC(HeroesWnd);
			int bpp = GetDeviceCaps(dc, BITSPIXEL);
			ReleaseDC(dc);
			if (bpp == 16)
			{
				px_DxObj->lpipx_DxAddPrimarySurface->px_DxSurface->Blt(&rect, 
					px_DxObj->lpipx_DxPrimarySurface->px_DxSurface, lpDestRect, DDBLT_WAIT, NULL);
			}
			else
			{
				HDC hdca,hdc,hdcb;
				px_DxObj->lpipx_DxOtherPrimarySurface->px_DxSurface->GetDC(&hdca);
				px_DxObj->lpipx_DxPrimarySurface->px_DxSurface->GetDC(&hdc);
				if (lpDestRect)
					BitBlt(hdca, lpDestRect->left, lpDestRect->top, rect.right - rect.left, rect.bottom - rect.top,
						hdc, lpDestRect->left, lpDestRect->top, SRCCOPY);
				else
					BitBlt(hdca, 0, 0, HeroesW, HeroesH, hdc, 0, 0, SRCCOPY);
				px_DxObj->lpipx_DxPrimarySurface->px_DxSurface->ReleaseDC(hdc);
				px_DxObj->lpipx_DxOtherPrimarySurface->px_DxSurface->ReleaseDC(hdca);
				px_DxObj->lpipx_DxAddPrimarySurface->px_DxSurface->Blt(&rect, px_DxObj->lpipx_DxOtherPrimarySurface->px_DxSurface, lpDestRect, DDBLT_WAIT, NULL);


				//hdcb=::GetDC(HeroesWnd);
				//px_DxObj->lpipx_DxAddPrimarySurface->px_DxSurface->GetDC(&hdca);
				//px_DxObj->lpipx_DxPrimarySurface->px_DxSurface->GetDC(&hdc);

				//BitBlt(hdca,p.x,p.y,HeroesW,600,hdc,0,0,SRCCOPY);
				//BitBlt(hdcb,0,0,HeroesW,600,hdc,0,0,SRCCOPY);

				//px_DxObj->lpipx_DxPrimarySurface->px_DxSurface->ReleaseDC(hdc);
				//px_DxObj->lpipx_DxAddPrimarySurface->px_DxSurface->ReleaseDC(hdca);
				//::ReleaseDC(HeroesWnd,hdcb);

				//px_DxObj->lpipx_DxAddPrimarySurface->px_DxSurface->Blt(&rect,px_DxSurface,NULL,DDBLT_WAIT,NULL);
			}
		}
	}
	else
	{	
		if(lpDDSrcSurface==px_DxObj->lpipx_DxPrimarySurface && lpSrcRect)
		{
			POINT p; p.x=0; p.y=0;
			ClientToScreen(HeroesWnd,&p);
			lpSrcRect->left -= p.x;
			lpSrcRect->right -= p.x;
			lpSrcRect->top -= p.y;
			lpSrcRect->bottom -= p.y;
		} 		
		if(lpDDSrcSurface)
			lpDDSrcSurface = LPPX_DIRECTDRAWSURFACE7(lpDDSrcSurface)->px_DxSurface;
		hRes=px_DxSurface->Blt(lpDestRect,lpDDSrcSurface,lpSrcRect,dwFlags,lpDDBltFx);
	}
	return hRes;

}
HRESULT __stdcall px_IDirectDrawSurface7::BltBatch(LPDDBLTBATCH lpDDBltBatch,DWORD dwCount,DWORD dwFlags){return px_DxSurface->BltBatch(lpDDBltBatch,dwCount,dwFlags);}
HRESULT __stdcall px_IDirectDrawSurface7::BltFast(DWORD dwX,DWORD dwY,LPDIRECTDRAWSURFACE7 lpDDSrcSurface,LPRECT lpSrcRect,DWORD dwTrans)
{
	if(lpDDSrcSurface)
		return px_DxSurface->BltFast(dwX,dwY,LPPX_DIRECTDRAWSURFACE7(lpDDSrcSurface)->px_DxSurface,lpSrcRect,dwTrans);
	return px_DxSurface->BltFast(dwX,dwY,lpDDSrcSurface,lpSrcRect,dwTrans);
}
HRESULT __stdcall px_IDirectDrawSurface7::GetBltStatus(DWORD dwFlags){return px_DxSurface->GetBltStatus(dwFlags);}
HRESULT __stdcall px_IDirectDrawSurface7::GetColorKey(DWORD dwFlags,LPDDCOLORKEY lpDDColorKey){return px_DxSurface->GetColorKey(dwFlags,lpDDColorKey);}
HRESULT __stdcall px_IDirectDrawSurface7::SetColorKey(DWORD dwFlags,LPDDCOLORKEY lpDDColorKey){return px_DxSurface->SetColorKey(dwFlags,lpDDColorKey);}
HRESULT __stdcall px_IDirectDrawSurface7::GetDC(HDC FAR *lphDC){return px_DxSurface->GetDC(lphDC);}
HRESULT __stdcall px_IDirectDrawSurface7::ReleaseDC(HDC hDC){return px_DxSurface->ReleaseDC(hDC);}
HRESULT __stdcall px_IDirectDrawSurface7::Flip(LPDIRECTDRAWSURFACE7 lpDDSurfaceTargetOverride,DWORD dwFlags)
{
	if(lpDDSurfaceTargetOverride)
		return px_DxSurface->Flip(LPPX_DIRECTDRAWSURFACE7(lpDDSurfaceTargetOverride)->px_DxSurface,dwFlags);
	return px_DxSurface->Flip(lpDDSurfaceTargetOverride,dwFlags);
}
HRESULT __stdcall px_IDirectDrawSurface7::GetFlipStatus(DWORD dwFlags){return px_DxSurface->GetFlipStatus(dwFlags);}
HRESULT __stdcall px_IDirectDrawSurface7::Lock(LPRECT lpDestRect,LPDDSURFACEDESC2 lpDDSurfaceDesc,DWORD dwFlags,HANDLE hEvent){return px_DxSurface->Lock(lpDestRect,lpDDSurfaceDesc,dwFlags,hEvent);}
HRESULT __stdcall px_IDirectDrawSurface7::PageLock(DWORD dwFlags){return px_DxSurface->PageLock(dwFlags);}
HRESULT __stdcall px_IDirectDrawSurface7::PageUnlock(DWORD dwFlags){return px_DxSurface->PageUnlock(dwFlags);}
HRESULT __stdcall px_IDirectDrawSurface7::Unlock(LPRECT lpRect){HRESULT hRes=px_DxSurface->Unlock(lpRect);return hRes;}
HRESULT __stdcall px_IDirectDrawSurface7::GetDDInterface(LPVOID FAR *lplpDD)
{
	HRESULT hRes=px_DxSurface->GetDDInterface(lplpDD);
	if(*lplpDD&&hRes==DD_OK)
		*lplpDD=(LPVOID)lpipx_DxDraw;
	return hRes;
}
HRESULT __stdcall px_IDirectDrawSurface7::AddOverlayDirtyRect(LPRECT lpRect){return px_DxSurface->AddOverlayDirtyRect(lpRect);}
HRESULT __stdcall px_IDirectDrawSurface7::EnumOverlayZOrders(DWORD dwFlags,LPVOID lpContext,LPDDENUMSURFACESCALLBACK7 lpfnCallback){return px_DxSurface->EnumOverlayZOrders(dwFlags,lpContext,lpfnCallback);}
HRESULT __stdcall px_IDirectDrawSurface7::GetOverlayPosition(LPLONG lplX,LPLONG lplY){return px_DxSurface->GetOverlayPosition(lplX,lplY);}
HRESULT __stdcall px_IDirectDrawSurface7::SetOverlayPosition(LONG lX,LONG lY){return px_DxSurface->SetOverlayPosition(lX,lY);}
HRESULT __stdcall px_IDirectDrawSurface7::UpdateOverlay(LPRECT lpSrcRect,LPDIRECTDRAWSURFACE7 lpDDDestSurface,LPRECT lpDestRect,DWORD dwFlags,LPDDOVERLAYFX lpDDOverlayFx){return px_DxSurface->UpdateOverlay(lpSrcRect,lpDDDestSurface,lpDestRect,dwFlags,lpDDOverlayFx);}
HRESULT __stdcall px_IDirectDrawSurface7::UpdateOverlayDisplay(DWORD dwFlags){return px_DxSurface->UpdateOverlayDisplay(dwFlags);}
HRESULT __stdcall px_IDirectDrawSurface7::UpdateOverlayZOrder(DWORD dwFlags,LPDIRECTDRAWSURFACE7 lpDDSReference){return px_DxSurface->UpdateOverlayZOrder(dwFlags,lpDDSReference);}
HRESULT __stdcall px_IDirectDrawSurface7::FreePrivateData(REFGUID guidTag){return px_DxSurface->FreePrivateData(guidTag);}
HRESULT __stdcall px_IDirectDrawSurface7::GetPrivateData(REFGUID guidTag,LPVOID lpBuffer,LPDWORD lpcbBufferSize){return px_DxSurface->GetPrivateData(guidTag,lpBuffer,lpcbBufferSize);} 
HRESULT __stdcall px_IDirectDrawSurface7::SetPrivateData(REFGUID guidTag,LPVOID lpData,DWORD cbSize,DWORD dwFlags){return px_DxSurface->SetPrivateData(guidTag,lpData,cbSize,dwFlags);} 
HRESULT __stdcall px_IDirectDrawSurface7::GetCaps(LPDDSCAPS2 lpDDSCaps){return px_DxSurface->GetCaps(lpDDSCaps);}
HRESULT __stdcall px_IDirectDrawSurface7::GetClipper(LPDIRECTDRAWCLIPPER FAR *lplpDDClipper)
{
	HRESULT hRes;
	if(this==px_DxObj->lpipx_DxPrimarySurface)
		hRes=px_DxObj->lpipx_DxAddPrimarySurface->GetClipper(lplpDDClipper);
	else
		hRes=px_DxSurface->GetClipper(lplpDDClipper);
	if(*lplpDDClipper&&hRes==DD_OK)
		*lplpDDClipper=(LPDIRECTDRAWCLIPPER)lpipx_DxClipper;
	return hRes;
}
// 	WogDll.exe!00601c60() 	
// 	On exit: WogDll.exe!00601e8e() 	
HRESULT __stdcall px_IDirectDrawSurface7::SetClipper(LPDIRECTDRAWCLIPPER lpDDClipper)//
{
	if(lpDDClipper&&this==px_DxObj->lpipx_DxPrimarySurface)
	{
		px_DxObj->lpipx_DxAddPrimarySurface->lpipx_DxClipper=LPPX_DIRECTDRAWCLIPPER(lpDDClipper);
		HRESULT hRes=px_DxObj->lpipx_DxAddPrimarySurface->px_DxSurface->SetClipper(px_DxObj->lpipx_DxAddPrimarySurface->lpipx_DxClipper->px_DxClipper);
		//lpipx_DxClipper=LPPX_DIRECTDRAWCLIPPER(lpDDClipper);
		return hRes;//px_DxSurface->SetClipper(lpipx_DxClipper->px_DxClipper);
	}
	return px_DxSurface->SetClipper(lpDDClipper);
}
HRESULT __stdcall px_IDirectDrawSurface7::ChangeUniquenessValue(){return px_DxSurface->ChangeUniquenessValue();}
// 	WogDll.exe!00601bc4() 	
HRESULT __stdcall px_IDirectDrawSurface7::GetPixelFormat(LPDDPIXELFORMAT lpDDPixelFormat)
{
	HRESULT hRes=px_DxSurface->GetPixelFormat(lpDDPixelFormat);
	lpDDPixelFormat->dwSize=sizeof(DDPIXELFORMAT);
	lpDDPixelFormat->dwFlags=DDPF_RGB;
	lpDDPixelFormat->dwRGBBitCount=16;
	lpDDPixelFormat->dwRBitMask=0xf800;
	lpDDPixelFormat->dwGBitMask=0x7e0;
	lpDDPixelFormat->dwBBitMask=0x1f;
	return hRes;
}
// 	WogDll.exe!0050d4da() 	
// 	WogDll.exe!0050d1e8() 	
HRESULT __stdcall px_IDirectDrawSurface7::GetSurfaceDesc(LPDDSURFACEDESC2 lpDDSurfaceDesc)
{
	HRESULT hRes;
	if(this==px_DxObj->lpipx_DxPrimarySurface)
		hRes=px_DxObj->lpipx_DxAddPrimarySurface->px_DxSurface->GetSurfaceDesc(lpDDSurfaceDesc);
	else
		hRes=px_DxSurface->GetSurfaceDesc(lpDDSurfaceDesc);
	lpDDSurfaceDesc->ddpfPixelFormat.dwSize=sizeof(DDPIXELFORMAT);
	lpDDSurfaceDesc->ddpfPixelFormat.dwFlags=DDPF_RGB;
	lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount=16;
	lpDDSurfaceDesc->ddpfPixelFormat.dwRBitMask=0xf800;
	lpDDSurfaceDesc->ddpfPixelFormat.dwGBitMask=0x7e0;
	lpDDSurfaceDesc->ddpfPixelFormat.dwBBitMask=0x1f;
	return hRes;
}
HRESULT __stdcall px_IDirectDrawSurface7::GetUniquenessValue(LPDWORD lpValue){return px_DxSurface->GetUniquenessValue(lpValue);}
HRESULT __stdcall px_IDirectDrawSurface7::SetSurfaceDesc(LPDDSURFACEDESC2 lpddsd2,DWORD dwFlags){return px_DxSurface->SetSurfaceDesc(lpddsd2,dwFlags);}
HRESULT __stdcall px_IDirectDrawSurface7::GetPalette(LPDIRECTDRAWPALETTE FAR *lplpDDPalette){return px_DxSurface->GetPalette(lplpDDPalette);}
HRESULT __stdcall px_IDirectDrawSurface7::SetPalette(LPDIRECTDRAWPALETTE lpDDPalette){return px_DxSurface->SetPalette(lpDDPalette);}
HRESULT __stdcall px_IDirectDrawSurface7::GetLOD(LPDWORD lpdwMaxLOD){return px_DxSurface->GetLOD(lpdwMaxLOD);}
HRESULT __stdcall px_IDirectDrawSurface7::GetPriority(LPDWORD lpdwPriority){return px_DxSurface->GetPriority(lpdwPriority);} 
HRESULT __stdcall px_IDirectDrawSurface7::SetLOD(DWORD dwMaxLOD){return px_DxSurface->SetLOD(dwMaxLOD);}
HRESULT __stdcall px_IDirectDrawSurface7::SetPriority(DWORD dwPriority){return px_DxSurface->SetPriority(dwPriority);}
HRESULT			  px_IDirectDrawSurface7::Clear(LPRECT lpSrcRect)
{
	HRESULT hRes;
	DDSURFACEDESC2 DDSurfaceDesc;
	ZeroMemory(&DDSurfaceDesc,sizeof(DDSURFACEDESC2)); 
	DDSurfaceDesc.dwSize=sizeof(DDSURFACEDESC2);
	if(px_DxSurface->IsLost())
		px_DxSurface->Restore(); 
	hRes=px_DxSurface->Lock(lpSrcRect,&DDSurfaceDesc,DDLOCK_WAIT|DDLOCK_WRITEONLY,0);
	if(hRes!=DD_OK)
		return hRes;
	int i,h,w,b,s;
	b=DDSurfaceDesc.ddpfPixelFormat.dwBumpBitCount;
	if(lpSrcRect==NULL)
	{
		h=DDSurfaceDesc.dwHeight;
		w=DDSurfaceDesc.dwWidth; 
	}
	else
	{
		h=lpSrcRect->bottom-lpSrcRect->top;
		w=lpSrcRect->right-lpSrcRect->left;
	}
	s=w*b/8;
	if(s>DDSurfaceDesc.lPitch)
		s=DDSurfaceDesc.lPitch;
	for(i=0;i<h;i++)
		ZeroMemory(((char*)DDSurfaceDesc.lpSurface)+DDSurfaceDesc.lPitch*i,s);
	hRes=px_DxSurface->Unlock(lpSrcRect);
	return hRes;
}
