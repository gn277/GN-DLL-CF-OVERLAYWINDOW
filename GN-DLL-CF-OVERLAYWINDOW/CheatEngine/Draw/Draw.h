#pragma once
#include "../../pch.h"
#include <dwmapi.h>
#include <d3dx9.h>
#include <d3d11.h>

#include "../../lmGui/imgui.h"
#include "../../lmGui/imgui_impl_dx11.h"
#include "../../lmGui/imgui_impl_win32.h"

#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"d3d11.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct _Direct9
{
	IDirect3D9* object = nullptr;							//����һ��D3D����
	IDirect3DDevice9* device = nullptr;						//����һ��D3D�豸
	D3DPRESENT_PARAMETERS params;							//����һ��D3D��̨���������
	IDirect3DStateBlock9* stateBlock = nullptr;
	ID3DXFont* fontSmall = nullptr;							//����һ��D3D�����ı�����
	ID3DXLine* line = nullptr;								//����һ��D3D���߶���
	int fontSize = 17;										//���������С
};

struct _Direct11
{
	ID3D11Device* pd3d11device = nullptr;
	ID3D11DeviceContext* pd3d11deviceContext = nullptr;
	IDXGISwapChain* pswapchain = nullptr;
	ID3D11RenderTargetView* mainrendertargetview = nullptr;
};


class Draw
{
private:
	HWND game_window_handle = 0;							//��Ϸ���ھ��
	HINSTANCE overlay_window_hinstance = 0;
	HWND overlay_window_handle = 0;
	_Direct9 direct9 = { nullptr };
	_Direct11 direct11 = { nullptr };

public:
	HWND GetGameWindowHandle() { return game_window_handle; }
	void SetOverlayWindowHinstance(HINSTANCE hinstance) { overlay_window_hinstance = hinstance; }
	void SetGameWindowHandle(HWND hand) { game_window_handle = hand; }
	HWND GetOverlayWindowHandle() { return overlay_window_handle; }
	IDirect3DDevice9* GetD3D9Device() { return direct9.device; }
	ID3D11Device* GetD3D11Device() { return direct11.pd3d11device; }
	IDXGISwapChain* GetSwapChain() { return direct11.pswapchain; }

	static LRESULT CALLBACK OverlayWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	ImU32 draw_color = D3DCOLOR_RGBA(0, 255, 0, 255);		//������ɫ
	RECT windowrect, clientrect;
	int window_x = 0, window_y = 0, gamecent_x = 0, gamecent_y = 0, gameheight = 0, gamewidth = 0, gametop = 0, gameleft = 0;

	bool is_present_draw = true;
	bool show_menu = true;						//���Ʋ˵�
	bool show_characterindex = false;			//������Ⱦ
	bool show_rectbox = false;					//��ʾ����
	bool show_health = false;					//��ʾѪ��
	bool show_c4 = false;						//��ʾ�װ�
	bool show_name = false;						//��ʾ�ǳ�
	bool show_crosshair = false;				//��ʾ׼��
	bool show_bone = false;						//��ʾ����
	bool show_userQQnumber = false;				//��ʾ�ۿ�
	bool show_teamate = false;					//��ʾ����
	bool show_previewray = false;				//Ԥ������
	bool show_trackrange = false;				//׷�ٷ�Χ

public:
	Draw();
	~Draw();
	bool CreateOverlayWindow(const wchar_t* class_name, const wchar_t* title_name, bool inverse_screenshot);
	void InitImGui(HWND hwnd);
	bool CreateDeviceD3D9(HWND hwnd);
	bool CreateDeviceD3D11(HWND hwnd);
	void CreateRenderTarget();
	void CleanupDeviceD3D11();
	void CleanupRenderTarget();
	void MoveOverlayWindow(HWND target_hwnd, HWND overlay_hwnd);
	void BegineDirect11();
	void EndDirect11();
	void GetGameWindowRect();
	void SetImGuiMouse();

};



