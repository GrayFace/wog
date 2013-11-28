#ifndef _DXPROXY_H_
#define _DXPROXY_H_

#include <map>
#include <windows.h>
#include "ddraw.h"


typedef struct px_IDirectDraw				FAR *LPPX_DIRECTDRAW;
typedef struct px_IDirectDrawClipper        FAR *LPPX_DIRECTDRAWCLIPPER;
typedef struct px_IDirectDrawSurface		FAR *LPPX_DIRECTDRAWSURFACE;
typedef struct px_IDirectDrawSurface7       FAR *LPPX_DIRECTDRAWSURFACE7;
typedef struct px_IDirectDrawColorControl   FAR *LPPX_DIRECTDRAWCOLORCONTROL;
typedef struct px_IDirectDrawPalette		FAR *LPPX_DIRECTDRAWPALETTE;


extern LPPX_DIRECTDRAW px_DxObj;
struct px_IDirectDraw;
struct px_IDirectDrawClipper;
struct px_IDirectDrawSurface;
struct px_IDirectDrawSurface7;

struct BMPhead
{
	unsigned short SignBMP;		//signature, must be 4D42 hex
	unsigned long  SizeBMP;		//size of BMP file in bytes (unreliable)
	unsigned short Reserved1;	//reserved, must be zero
	unsigned short Reserved2;	//reserved, must be zero
	unsigned long  OffsetIMG;	//offset to start of image data in bytes
	unsigned long  SizeBIH;		//size of BITMAPINFOHEADER structure, must be 40
	unsigned long  Width;		//image width in pixels
	unsigned long  Height;		//image height in pixels
	unsigned short NumPI;		//number of planes in the image, must be 1
	unsigned short NumBPP;		//number of bits per pixel (1, 4, 8, or 24)
	unsigned long  CompType;	//compression type (0=none, 1=RLE-8, 2=RLE-4)
	unsigned long  SizeID;		//size of image data in bytes (including padding)
	unsigned long  HRPPM;		//horizontal resolution in pixels per meter (unreliable)
	unsigned long  VRPPM;		//vertical resolution in pixels per meter (unreliable)
	unsigned long  NumCI;		//number of colors in image, or zero
	unsigned long  NumIC;		//number of important colors, or zero
	unsigned long  MaskR;		//red	mask must be 0xF800
	unsigned long  MaskG;		//green mask must be 0x07E0
	unsigned long  MaskB;		//blue	mask must be 0x001F
};   

struct px_IDirectDraw:public IDirectDraw
{
	public:
		px_IDirectDraw(LPDIRECTDRAW lpDD);
		virtual ~px_IDirectDraw();
		HRESULT __stdcall QueryInterface(REFIID riid,LPVOID* obp);
		ULONG	__stdcall AddRef();
		ULONG	__stdcall Release();
		HRESULT __stdcall Compact();
		HRESULT __stdcall CreateClipper(DWORD dwFlags,LPDIRECTDRAWCLIPPER FAR *lplpDDClipper,IUnknown FAR *pUnkOuter);
		HRESULT __stdcall CreatePalette(DWORD dwFlags,LPPALETTEENTRY lpColorTable,LPDIRECTDRAWPALETTE FAR* lplpDDPalette,IUnknown FAR *pUnkOuter);
		HRESULT __stdcall CreateSurface(LPDDSURFACEDESC lpDDSurfaceDesc,LPDIRECTDRAWSURFACE FAR *lplpDDSurface,IUnknown FAR *pUnkOuter);
		HRESULT __stdcall DuplicateSurface(LPDIRECTDRAWSURFACE lpDDSurface,LPDIRECTDRAWSURFACE FAR *lplpDupDDSurface);
		HRESULT __stdcall EnumDisplayModes(DWORD dwFlags,LPDDSURFACEDESC lpDDSurfaceDesc,LPVOID lpContext,LPDDENUMMODESCALLBACK lpEnumCallback);
		HRESULT __stdcall EnumSurfaces(DWORD dwFlags,LPDDSURFACEDESC lpDDSD,LPVOID lpContext,LPDDENUMSURFACESCALLBACK lpEnumCallback);
		HRESULT __stdcall FlipToGDISurface();
		HRESULT __stdcall GetCaps(LPDDCAPS lpDDDriverCaps,LPDDCAPS lpDDHELCaps);
		HRESULT __stdcall GetDisplayMode(LPDDSURFACEDESC lpDDSurfaceDesc);
		HRESULT __stdcall GetFourCCCodes(DWORD FAR *lpNumCodes,DWORD FAR *lpCodes);
		HRESULT __stdcall GetGDISurface(LPDIRECTDRAWSURFACE FAR *lplpGDIDDSSurface);
		HRESULT __stdcall GetMonitorFrequency(LPDWORD lpdwFrequency);
		HRESULT __stdcall GetScanLine(LPDWORD lpdwScanLine);
		HRESULT __stdcall GetVerticalBlankStatus(LPBOOL lpbIsInVB);
		HRESULT __stdcall Initialize(GUID FAR *lpGUID);
		HRESULT __stdcall RestoreDisplayMode();
		HRESULT __stdcall SetCooperativeLevel(HWND hWnd,DWORD dwFlags);
		HRESULT __stdcall SetDisplayMode(DWORD dwWidth,DWORD dwHeight,DWORD dwBpp);
		HRESULT __stdcall WaitForVerticalBlank(DWORD dwFlags,HANDLE hEvent);

		void			  MakeScreenShot();
		bool			  IsPrimarySurface(){return isPrimarySurface;};
	private:
		int										iClipperCounter,iBackBufferCounter,iSurfaceCounter;
		bool									isPrimarySurface;
	protected:	
		DWORD									Width,Height,BPP;
		LPDIRECTDRAW							px_DxDraw;
		std::map<int,LPPX_DIRECTDRAWCLIPPER>	lpipx_DxClipper;
		LPPX_DIRECTDRAWSURFACE7					lpipx_DxPrimarySurface,lpipx_DxAddPrimarySurface,lpipx_DxOtherPrimarySurface;//,lpipx_DxDebugSurface,lpipx_DxLoadDebugSurface;
		std::map<int,LPPX_DIRECTDRAWSURFACE7>	lpipx_DxBackBuffer;
		std::map<int,LPPX_DIRECTDRAWSURFACE7>	lpipx_DxOtherSurface;
	//friend:
		friend struct px_IDirectDraw;
		friend struct px_IDirectDrawClipper;
		friend struct px_IDirectDrawSurface;
		friend struct px_IDirectDrawSurface7;
		friend HRESULT WINAPI px_DirectDrawCreate( GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter );
		friend LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam);

};

struct px_IDirectDrawClipper:public IDirectDrawClipper
{
	public:
		//px_IDirectDrawClipper();
		px_IDirectDrawClipper(LPDIRECTDRAWCLIPPER lpDDC);
		virtual ~px_IDirectDrawClipper();
		HRESULT __stdcall QueryInterface(REFIID riid,LPVOID* obp);
		ULONG	__stdcall AddRef();
		ULONG	__stdcall Release();
		HRESULT __stdcall Initialize(LPDIRECTDRAW lpDD,DWORD dwFlags);
		HRESULT __stdcall GetClipList(LPRECT lpRect,LPRGNDATA lpClipList,LPDWORD lpdwSize);
		HRESULT __stdcall IsClipListChanged(BOOL FAR *lpbChanged);
		HRESULT __stdcall SetClipList(LPRGNDATA lpClipList,DWORD dwFlags);
		HRESULT __stdcall SetHWnd(DWORD dwFlags,HWND hWnd);
		HRESULT __stdcall GetHWnd(HWND FAR *lphWnd);
	protected:
		LPDIRECTDRAWCLIPPER px_DxClipper;
		LPPX_DIRECTDRAW lpipx_DxDraw;
	//friend:
		friend struct px_IDirectDraw;
		friend struct px_IDirectDrawClipper;
		friend struct px_IDirectDrawSurface;
		friend struct px_IDirectDrawSurface7;
		friend HRESULT WINAPI px_DirectDrawCreate( GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter );
};

struct px_IDirectDrawSurface7:public IDirectDrawSurface7
{
	public:
		px_IDirectDrawSurface7(LPDIRECTDRAWSURFACE7 lpDDS7);
		virtual ~px_IDirectDrawSurface7();
		HRESULT __stdcall QueryInterface(REFIID riid,LPVOID* obp);
		ULONG	__stdcall AddRef();
		ULONG	__stdcall Release();
		HRESULT __stdcall Initialize(LPDIRECTDRAW lpDD,LPDDSURFACEDESC2 lpDDSurfaceDesc);
		HRESULT __stdcall IsLost();
		HRESULT __stdcall Restore();
		HRESULT __stdcall AddAttachedSurface(LPDIRECTDRAWSURFACE7 lpDDSAttachedSurface);
		HRESULT __stdcall DeleteAttachedSurface(DWORD dwFlags,LPDIRECTDRAWSURFACE7 lpDDSAttachedSurface);
		HRESULT __stdcall EnumAttachedSurfaces(LPVOID lpContext,LPDDENUMSURFACESCALLBACK7 lpEnumSurfacesCallback);
		HRESULT __stdcall GetAttachedSurface(LPDDSCAPS2 lpDDSCaps,LPDIRECTDRAWSURFACE7 FAR *lplpDDAttachedSurface);
		HRESULT __stdcall Blt(LPRECT lpDestRect,LPDIRECTDRAWSURFACE7 lpDDSrcSurface,LPRECT lpSrcRect,DWORD dwFlags,LPDDBLTFX lpDDBltFx);
		HRESULT __stdcall BltBatch(LPDDBLTBATCH lpDDBltBatch,DWORD dwCount,DWORD dwFlags);
		HRESULT __stdcall BltFast(DWORD dwX,DWORD dwY,LPDIRECTDRAWSURFACE7 lpDDSrcSurface,LPRECT lpSrcRect,DWORD dwTrans);
		HRESULT __stdcall GetBltStatus(DWORD dwFlags);
		HRESULT __stdcall GetColorKey(DWORD dwFlags,LPDDCOLORKEY lpDDColorKey);
		HRESULT __stdcall SetColorKey(DWORD dwFlags,LPDDCOLORKEY lpDDColorKey);
		HRESULT __stdcall GetDC(HDC FAR *lphDC);
		HRESULT __stdcall ReleaseDC(HDC hDC);
		HRESULT __stdcall Flip(LPDIRECTDRAWSURFACE7 lpDDSurfaceTargetOverride,DWORD dwFlags);
		HRESULT __stdcall GetFlipStatus(DWORD dwFlags);
		HRESULT __stdcall Lock(LPRECT lpDestRect,LPDDSURFACEDESC2 lpDDSurfaceDesc,DWORD dwFlags,HANDLE hEvent);
		HRESULT __stdcall PageLock(DWORD dwFlags);
		HRESULT __stdcall PageUnlock(DWORD dwFlags);
		HRESULT __stdcall Unlock(LPRECT lpRect);
		HRESULT __stdcall GetDDInterface(LPVOID FAR *lplpDD);
		HRESULT __stdcall AddOverlayDirtyRect(LPRECT lpRect);
		HRESULT __stdcall EnumOverlayZOrders(DWORD dwFlags,LPVOID lpContext,LPDDENUMSURFACESCALLBACK7 lpfnCallback);
		HRESULT __stdcall GetOverlayPosition(LPLONG lplX,LPLONG lplY);
		HRESULT __stdcall SetOverlayPosition(LONG lX,LONG lY);
		HRESULT __stdcall UpdateOverlay(LPRECT lpSrcRect,LPDIRECTDRAWSURFACE7 lpDDDestSurface,LPRECT lpDestRect,DWORD dwFlags,LPDDOVERLAYFX lpDDOverlayFx);
		HRESULT __stdcall UpdateOverlayDisplay(DWORD dwFlags);
		HRESULT __stdcall UpdateOverlayZOrder(DWORD dwFlags,LPDIRECTDRAWSURFACE7 lpDDSReference);
		HRESULT __stdcall FreePrivateData(REFGUID guidTag); 
		HRESULT __stdcall GetPrivateData(REFGUID guidTag,LPVOID lpBuffer,LPDWORD lpcbBufferSize); 
		HRESULT __stdcall SetPrivateData(REFGUID guidTag,LPVOID lpData,DWORD cbSize,DWORD dwFlags); 
		HRESULT __stdcall GetCaps(LPDDSCAPS2 lpDDSCaps);
		HRESULT __stdcall GetClipper(LPDIRECTDRAWCLIPPER FAR *lplpDDClipper);
		HRESULT __stdcall SetClipper(LPDIRECTDRAWCLIPPER lpDDClipper);
		HRESULT __stdcall ChangeUniquenessValue(); 
		HRESULT __stdcall GetPixelFormat(LPDDPIXELFORMAT lpDDPixelFormat);
		HRESULT __stdcall GetSurfaceDesc(LPDDSURFACEDESC2 lpDDSurfaceDesc);
		HRESULT __stdcall GetUniquenessValue(LPDWORD lpValue); 
		HRESULT __stdcall SetSurfaceDesc(LPDDSURFACEDESC2 lpddsd2,DWORD dwFlags);
		HRESULT __stdcall GetPalette(LPDIRECTDRAWPALETTE FAR *lplpDDPalette);
		HRESULT __stdcall SetPalette(LPDIRECTDRAWPALETTE lpDDPalette);
		HRESULT __stdcall GetLOD(LPDWORD lpdwMaxLOD);
		HRESULT __stdcall GetPriority(LPDWORD lpdwPriority); 
		HRESULT __stdcall SetLOD(DWORD dwMaxLOD);
		HRESULT __stdcall SetPriority(DWORD dwPriority);
		HRESULT			  Clear(LPRECT lpSrcRect);

		LPDIRECTDRAWSURFACE7	px_DxSurface;
	protected:	
		LPPX_DIRECTDRAWCLIPPER	lpipx_DxClipper;
		LPPX_DIRECTDRAW			lpipx_DxDraw;
	//friend:
		friend struct px_IDirectDraw;
		friend struct px_IDirectDrawClipper;
		friend struct px_IDirectDrawSurface;
		friend struct px_IDirectDrawSurface7;
		friend HRESULT WINAPI px_DirectDrawCreate( GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter );
};

struct px_IDirectDrawSurface:public IDirectDrawSurface
{
	public:
		px_IDirectDrawSurface(LPDIRECTDRAWSURFACE lpDDS7);
		virtual ~px_IDirectDrawSurface();
		HRESULT __stdcall QueryInterface(REFIID riid,LPVOID* obp);
		ULONG	__stdcall AddRef();
		ULONG	__stdcall Release();
		HRESULT __stdcall Initialize(LPDIRECTDRAW lpDD,LPDDSURFACEDESC2 lpDDSurfaceDesc);
		HRESULT __stdcall IsLost();
		HRESULT __stdcall Restore();
		/*
		HRESULT __stdcall AddAttachedSurface(LPDIRECTDRAWSURFACE7 lpDDSAttachedSurface);
		HRESULT __stdcall DeleteAttachedSurface(DWORD dwFlags,LPDIRECTDRAWSURFACE7 lpDDSAttachedSurface);
		HRESULT __stdcall EnumAttachedSurfaces(LPVOID lpContext,LPDDENUMSURFACESCALLBACK7 lpEnumSurfacesCallback);
		HRESULT __stdcall GetAttachedSurface(LPDDSCAPS2 lpDDSCaps,LPDIRECTDRAWSURFACE7 FAR *lplpDDAttachedSurface);
		HRESULT __stdcall Blt(LPRECT lpDestRect,LPDIRECTDRAWSURFACE7 lpDDSrcSurface,LPRECT lpSrcRect,DWORD dwFlags,LPDDBLTFX lpDDBltFx);
		HRESULT __stdcall BltBatch(LPDDBLTBATCH lpDDBltBatch,DWORD dwCount,DWORD dwFlags);
		HRESULT __stdcall BltFast(DWORD dwX,DWORD dwY,LPDIRECTDRAWSURFACE7 lpDDSrcSurface,LPRECT lpSrcRect,DWORD dwTrans);
		HRESULT __stdcall GetBltStatus(DWORD dwFlags);
		HRESULT __stdcall GetColorKey(DWORD dwFlags,LPDDCOLORKEY lpDDColorKey);
		HRESULT __stdcall SetColorKey(DWORD dwFlags,LPDDCOLORKEY lpDDColorKey);
		HRESULT __stdcall GetDC(HDC FAR *lphDC);
		HRESULT __stdcall ReleaseDC(HDC hDC);
		HRESULT __stdcall Flip(LPDIRECTDRAWSURFACE7 lpDDSurfaceTargetOverride,DWORD dwFlags);
		HRESULT __stdcall GetFlipStatus(DWORD dwFlags);
		HRESULT __stdcall Lock(LPRECT lpDestRect,LPDDSURFACEDESC2 lpDDSurfaceDesc,DWORD dwFlags,HANDLE hEvent);
		HRESULT __stdcall PageLock(DWORD dwFlags);
		HRESULT __stdcall PageUnlock(DWORD dwFlags);
		HRESULT __stdcall Unlock(LPRECT lpRect);
		HRESULT __stdcall GetDDInterface(LPVOID FAR *lplpDD);
		HRESULT __stdcall AddOverlayDirtyRect(LPRECT lpRect);
		HRESULT __stdcall EnumOverlayZOrders(DWORD dwFlags,LPVOID lpContext,LPDDENUMSURFACESCALLBACK7 lpfnCallback);
		HRESULT __stdcall GetOverlayPosition(LPLONG lplX,LPLONG lplY);
		HRESULT __stdcall SetOverlayPosition(LONG lX,LONG lY);
		HRESULT __stdcall UpdateOverlay(LPRECT lpSrcRect,LPDIRECTDRAWSURFACE7 lpDDDestSurface,LPRECT lpDestRect,DWORD dwFlags,LPDDOVERLAYFX lpDDOverlayFx);
		HRESULT __stdcall UpdateOverlayDisplay(DWORD dwFlags);
		HRESULT __stdcall UpdateOverlayZOrder(DWORD dwFlags,LPDIRECTDRAWSURFACE7 lpDDSReference);
		HRESULT __stdcall FreePrivateData(REFGUID guidTag); 
		HRESULT __stdcall GetPrivateData(REFGUID guidTag,LPVOID lpBuffer,LPDWORD lpcbBufferSize); 
		HRESULT __stdcall SetPrivateData(REFGUID guidTag,LPVOID lpData,DWORD cbSize,DWORD dwFlags); 
		HRESULT __stdcall GetCaps(LPDDSCAPS2 lpDDSCaps);
		HRESULT __stdcall GetClipper(LPDIRECTDRAWCLIPPER FAR *lplpDDClipper);
		HRESULT __stdcall SetClipper(LPDIRECTDRAWCLIPPER lpDDClipper);
		HRESULT __stdcall ChangeUniquenessValue(); 
		HRESULT __stdcall GetPixelFormat(LPDDPIXELFORMAT lpDDPixelFormat);
		HRESULT __stdcall GetSurfaceDesc(LPDDSURFACEDESC2 lpDDSurfaceDesc);
		HRESULT __stdcall GetUniquenessValue(LPDWORD lpValue); 
		HRESULT __stdcall SetSurfaceDesc(LPDDSURFACEDESC2 lpddsd2,DWORD dwFlags);
		HRESULT __stdcall GetPalette(LPDIRECTDRAWPALETTE FAR *lplpDDPalette);
		HRESULT __stdcall SetPalette(LPDIRECTDRAWPALETTE lpDDPalette);
		HRESULT __stdcall GetLOD(LPDWORD lpdwMaxLOD);
		HRESULT __stdcall GetPriority(LPDWORD lpdwPriority); 
		HRESULT __stdcall SetLOD(DWORD dwMaxLOD);
		HRESULT __stdcall SetPriority(DWORD dwPriority);*/
	protected:	
		LPDIRECTDRAWSURFACE		px_DxSurface;
		LPPX_DIRECTDRAWCLIPPER	lpipx_DxClipper;
		LPPX_DIRECTDRAW			lpipx_DxDraw;
	//friend:
		friend struct px_IDirectDraw;
		friend struct px_IDirectDrawClipper;
		friend struct px_IDirectDrawSurface;
		friend struct px_IDirectDrawSurface7;
		friend HRESULT WINAPI px_DirectDrawCreate( GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter );
};

#endif
