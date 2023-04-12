#include "Draw.h"
#include "../../DllMain/DllMain.h"


Draw::Draw()
{
}

Draw::~Draw()
{
}

bool Draw::CreateOverlayWindow(const wchar_t* class_name, const wchar_t* title_name, bool inverse_screenshot)
{
	RECT MyRect;
	GetWindowRect(GetDesktopWindow(), &MyRect);	//获取桌面窗口分辨率
	WNDCLASSEX wc = { NULL };
	wc.cbSize = sizeof(wc);
	wc.style = CS_CLASSDC;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpfnWndProc = (WNDPROC)Draw::OverlayWindowProc;
	wc.lpszClassName = class_name;
	if (!RegisterClassEx(&wc))
	{
		OutputDebugStringA_1Param("[GN]:%s-> RegisterClassEx() Error", __FUNCTION__);
		return false;
	}
	long width = MyRect.right - MyRect.left, height = MyRect.bottom - MyRect.top;
	//Sleep(500);
	//OutputDebugStringA_2Param("[GN]:window width:%d,height:%d", width, height);
	HWND my_overlay_window_handle = ::CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW, wc.lpszClassName, title_name, WS_POPUP, MyRect.left, MyRect.top, width, height, NULL, NULL, wc.hInstance, NULL);
	this->SetOverlayWindowHandle(my_overlay_window_handle);

	OutputDebugStringA_1Param("[GN]:%s->", __FUNCTION__);
	if (!this->overlay_window_handle)
	{
		OutputDebugStringA_2Param("[GN]:%s-> overlay_window_handle:%p", __FUNCTION__, this->overlay_window_handle);
		return false;
	}
	ShowWindow(this->overlay_window_handle, SW_SHOW);
	UpdateWindow(this->overlay_window_handle);
	SetWindowLong(this->overlay_window_handle, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
	SetLayeredWindowAttributes(this->overlay_window_handle, 0, 255, 2);
	MARGINS n = { MyRect.left, MyRect.top, width, height };
	DwmExtendFrameIntoClientArea(this->overlay_window_handle, &n);
	if (inverse_screenshot)//半透明api 防截图,防直播
		SetWindowDisplayAffinity(this->overlay_window_handle, 17);
	if (!this->Draw::CreateDeviceD3D9(this->overlay_window_handle))
	{
		OutputDebugStringA_1Param("[GN]:%s-> CreateDeviceD3D9() failed", __FUNCTION__);
		return false;
	}
	if (!this->Draw::CreateDeviceD3D11(this->overlay_window_handle))
	{
		OutputDebugStringA_1Param("[GN]:%s-> CreateDeviceD3D11() failed", __FUNCTION__);
		return false;
	}
	this->Draw::InitImGui(this->Draw::overlay_window_handle);
	return true;
}

void Draw::InitImGui(HWND hwnd)
{
	OutputDebugStringA_1Param("[GN]:%s->", __FUNCTION__);
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = nullptr;
	io.LogFilename = nullptr;
	ImGui::StyleColorsChinaRed();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(this->direct11.pd3d11device, this->direct11.pd3d11deviceContext);
	ImFontConfig f_cfg;
	f_cfg.FontDataOwnedByAtlas = false;
	fstream _file;
	_file.open("c:\\Windows\\Fonts\\msyh.ttc", ios::in);
	if (!_file)
		ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\simsun.ttc", 10.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
	else
		ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msyh.ttc", 14.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
}

bool Draw::CreateDeviceD3D9(HWND hwnd)
{
	OutputDebugStringA_1Param("[GN]:%s->", __FUNCTION__);
	this->direct9.object = Direct3DCreate9(D3D_SDK_VERSION);
	if (this->direct9.object == FALSE)
	{
		MessageBox(NULL, L"Direct3D9创建失败", L"温馨提示", 0);
		return false;
	}
	memset(&this->direct9.params, 0, sizeof(this->direct9.params));
	this->direct9.params.Windowed = TRUE;
	this->direct9.params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	this->direct9.params.BackBufferFormat = D3DFMT_UNKNOWN;
	HRESULT result = this->direct9.object->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &this->direct9.params, &this->direct9.device);
	if (result != S_OK)
	{
		MessageBox(NULL, L"Create Device失败！", L"温馨提示", 0);
		return false;
	}
	return true;
}

bool Draw::CreateDeviceD3D11(HWND hwnd)
{
	OutputDebugStringA_1Param("[GN]:%s->", __FUNCTION__);
	HMODULE libD3D11;
	void* D3D11CreateDeviceAndSwapChain;
	if ((libD3D11 = ::GetModuleHandleW(L"d3d11.dll")) == NULL) return false;
	if ((D3D11CreateDeviceAndSwapChain = ::GetProcAddress(libD3D11, "D3D11CreateDeviceAndSwapChain")) == NULL) return false;
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hwnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	//const D3D_FEATURE_LEVEL featureLevelArray[6] = { D3D_FEATURE_LEVEL_9_1,	D3D_FEATURE_LEVEL_9_2,	D3D_FEATURE_LEVEL_9_3,	D3D_FEATURE_LEVEL_10_0,	D3D_FEATURE_LEVEL_10_1,	D3D_FEATURE_LEVEL_11_0, };
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HRESULT result = ((long(__stdcall*)(IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT, const D3D_FEATURE_LEVEL*, UINT, UINT, const DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**, ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext**))(D3D11CreateDeviceAndSwapChain))
		(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &this->direct11.pswapchain, &this->direct11.pd3d11device, &featureLevel, &this->direct11.pd3d11deviceContext);
	if (result != S_OK)
	{
		OutputDebugStringA_2Param("[GN]:%s-> D3D11CreateDeviceAndSwapChain():%p", __FUNCTION__, result);
		return false;
	}
	this->CreateRenderTarget();
	return true;
}

void Draw::CreateRenderTarget()
{
	//OutputDebugStringA_1Param("[GN]:%s->", __FUNCTION__);
	ID3D11Texture2D* pBackBuffer;
	this->direct11.pswapchain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	this->direct11.pd3d11device->CreateRenderTargetView(pBackBuffer, NULL, &this->direct11.mainrendertargetview);
	pBackBuffer->Release();
}

void Draw::CleanupDeviceD3D11()
{
	CleanupRenderTarget();
	if (this->direct11.pswapchain) { this->direct11.pswapchain->Release(); this->direct11.pswapchain = NULL; }
	if (this->direct11.pd3d11deviceContext) { this->direct11.pd3d11deviceContext->Release(); this->direct11.pd3d11deviceContext = NULL; }
	if (this->direct11.pd3d11device) { this->direct11.pd3d11device->Release(); this->direct11.pd3d11device = NULL; }
}

void Draw::CleanupRenderTarget()
{
	if (this->direct11.mainrendertargetview) { this->direct11.mainrendertargetview->Release(); this->direct11.mainrendertargetview = NULL; }
}

void Draw::MoveOverlayWindow(HWND target_hwnd, HWND overlay_hwnd)
{
	WINDOWINFO info = { NULL };
	int height, width;
	//ZeroMemory(&info, sizeof(info));
	info.cbSize = sizeof(info);
	GetWindowInfo(target_hwnd, &info);
	height = ((info.rcClient.bottom) - (info.rcClient.top)) + 3;
	width = ((info.rcClient.right) - (info.rcClient.left)) + 4;
	MoveWindow(overlay_hwnd, info.rcClient.left, info.rcClient.top, width, height, true);
	////SetWindowPos(overlay_hwnd, HWND_TOPMOST, NULL, NULL, NULL, NULL, SWP_NOMOVE | SWP_NOSIZE);
}

void Draw::BegineDirect11()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void Draw::EndDirect11()
{
	ImGui::Render();
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	this->direct11.pd3d11deviceContext->OMSetRenderTargets(1, &this->direct11.mainrendertargetview, NULL);
	this->direct11.pd3d11deviceContext->ClearRenderTargetView(this->direct11.mainrendertargetview, clearColor);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	this->direct11.pswapchain->Present(1, 0); // 垂直同步
}

void Draw::GetGameWindowRect()
{
	GetWindowRect(this->game_window_handle, &this->Draw::windowrect);
	GetClientRect(this->game_window_handle, &this->Draw::clientrect);
	//OutputDebugStringA_4Param("[GN]:window:left:%d,top:%d,right:%d,bottom:%d", game_window_rect.left, game_window_rect.top, game_window_rect.right, game_window_rect.bottom);
	//OutputDebugStringA_3Param("[GN]:client_window:left:%d,right:%d,bottom:%d", this->Draw::clientrect.left, this->Draw::clientrect.right, this->Draw::clientrect.bottom);
	this->Draw::gamewidth = (int)this->Draw::clientrect.right;
	this->Draw::gameheight = (int)this->Draw::clientrect.bottom;
	this->Draw::gamecent_x = (int)this->Draw::clientrect.right / 2;
	this->Draw::gamecent_y = (int)this->Draw::clientrect.bottom / 2;
}

void Draw::SetImGuiMouse()
{
	ImGuiIO& io = ImGui::GetIO();
	POINT mousePosition;
	::GetCursorPos(&mousePosition);
	io.MousePos.x = (float)mousePosition.x - this->Draw::windowrect.left;
	io.MousePos.y = (float)mousePosition.y - this->Draw::windowrect.top - 26;
	if (GetAsyncKeyState(VK_LBUTTON))io.MouseDown[0] = true;
	else io.MouseDown[0] = false;
}

void Draw::FillRectangle(float Left, float Top, float Width, float Height, float R, float G, float B, float A)//FillRectangle
{
	ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(Left, Top), ImVec2(Left + Width, Top + Height), ImGui::ColorConvertFloat4ToU32(ImVec4(R / 255.0, G / 255.0, B / 255.0, A / 255.0)), NULL);
}

void Draw::DrawCrossHair(RECT rect, float width, D3DCOLOR color)
{
	int x = rect.right / 2, y = rect.bottom / 2;
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x - 5, y), ImVec2(x + 6, y), color, width);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y - 5), ImVec2(x, y + 6), color, width);
}

void Draw::DrawAntenna(RECT rect, int objx, int objy, int objw, int objh, D3DCOLOR color, float width)
{
	int topx = rect.right / 2;
	objx = objx + objw / 2;
	if (objy < rect.bottom)
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(topx, 0), ImVec2(objx, objy), color, width);
}

void Draw::DrawPreviewRay(RECT rect, int objx, int objy, int objw, int objh, bool side, D3DCOLOR color, float width)
{
	int x = rect.right / 2, y = rect.bottom / 2, objcenty, objcentx = (objx + objw) / 2;
	if (side == FALSE)
		objcenty = (objy + objh) / 2;
	else
		objcenty = objy + 5;
	//if (track == TRUE || AimBot == TRUE)
	{
		if (objcenty < rect.bottom && objcenty > 0 && objx > 0 && objcentx < rect.right)
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y), ImVec2(objcentx, objcenty), color, width);
	}
}

void Draw::DrawTrackRange(RECT rect, int range, D3DCOLOR color, float width)
{
	int x = rect.right / 2, y = rect.bottom / 2;
	ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(x, y), range, color, 0, width);
}

void Draw::MenuDraw()
{
	if (this->show_menu)
	{
		ImGui::Begin(u8"Red Bull B   Home隐藏显示");
		static systime runtime = { NULL };
		runtime = this->Tools::GetSystemtime();
		ImGui::Text(u8"当前时间：%4d-%02d-%02d %02d:%02d:%02d", runtime.year, runtime.month, runtime.day, runtime.hour, runtime.min, runtime.sec);
		ImGui::SameLine(360.0f);
		ImGui::Text(u8"欢迎使用Red Bull"); ImGui::Separator();
		if (ImGui::CollapsingHeader(u8"显示专区"))
		{
			ImGui::Checkbox(u8"显示方框", &this->show_rectbox);													ImGui::SameLine();
			ImGui::Checkbox(u8"显示骨骼", &this->show_bone);													ImGui::SameLine();
			ImGui::Checkbox(u8"显示昵称", &this->show_name);													ImGui::SameLine();
			ImGui::Checkbox(u8"显示血量", &this->show_health);													ImGui::SameLine();
			ImGui::Checkbox(u8"显示雷包", &this->show_c4);														ImGui::Separator();
			//
			ImGui::Checkbox(u8"显示准心", &this->show_crosshair);												ImGui::SameLine();
			ImGui::Checkbox(u8"显示扣扣", &this->show_userQQnumber);											ImGui::SameLine();
			ImGui::Checkbox(u8"显示队友", &this->show_teamate);													ImGui::SameLine();
			ImGui::Checkbox(u8"预瞄射线", &this->show_previewray);												ImGui::SameLine();
			ImGui::Checkbox(u8"追踪范围", &this->show_trackrange);												ImGui::Separator();
		}
		if (ImGui::CollapsingHeader(u8"打击专区"))
		{
			if (ImGui::Checkbox(u8"鼠标自瞄", &ce->Game::aimbot))
			{
				ce->Game::track = false; ce->Game::redname_track = false; ce->Game::silence_track = false;/*this->Game::range_track = false;*/
			}
			ImGui::SameLine();
			if (ImGui::Checkbox(u8"红名追踪", &ce->Game::redname_track))
			{
				ce->Game::aimbot = false; ce->Game::track = false; ce->Game::silence_track = false;/*this->Game::range_track = false;*/
			}
			ImGui::SameLine();
			if (ImGui::Checkbox(u8"视角追踪", &ce->Game::silence_track))
			{
				ce->Game::aimbot = false; ce->Game::track = false; ce->Game::redname_track = false;/*this->Game::range_track = false;*/
			}
			ImGui::SameLine();
			ImGui::Checkbox(u8"判断障碍", &ce->Game::judgementbarrier);											ImGui::Separator();
			ImGui::SetNextItemWidth(60);
			ImGui::Combo(u8"瞄准热键", &ce->Game::aim_hotkey, u8"左键\0右键\0Alt键\0");							ImGui::SameLine();
			ImGui::SetNextItemWidth(60);
			ImGui::Combo(u8"打击部位", &ce->Game::aim_position, u8"身体\0头部\0");								ImGui::Separator();
			ImGui::DragInt(u8"自瞄速度(越小越好)", &ce->Game::aim_speed, 0.3f, 1, 80, "%d");					ImGui::Separator();
			ImGui::DragInt(u8"打击范围(越大越好)", &ce->Game::track_range, 0.6f, 30, 1200, "%d");				ImGui::Separator();
		}
		//if (ImGui::CollapsingHeader(u8"范围专区"))
		//{
		//	if (ImGui::Checkbox(u8"范围开关", &this->Game::range_track))
		//	{
		//		this->CheatEngine::CheckSoftWareBreakPoint(this->Game::GameBase.Cross + RangeTrackAddressOne);
		//		this->CheatEngine::CheckSoftWareBreakPoint(this->Game::GameBase.Cross + RangeTrackAddressTwo);
		//		this->CheatEngine::CheckSoftWareBreakPoint(this->Game::GameBase.Cross + RangeTrackAddressThree);
		//		//this->Game::aimbot = false; this->Game::track = false; this->Game::redname_track = false; this->Game::silence_track = false;
		//	}	ImGui::SameLine();
		//	static int range_trace_position_select = 3; ImGui::SetNextItemWidth(75);
		//	if (ImGui::Combo(u8"设置部位", &range_trace_position_select, u8"黄金爆头\0银色爆头\0颈部\0锁骨\0手\0脚\0"))
		//	{
		//		switch (range_trace_position_select)
		//		{
		//		case 0: {	this->Game::range_track_position = RANGE_TRACK_POSITION_GOLDEN_HEAD; break;	}
		//		case 1: {	this->Game::range_track_position = RANGE_TRACK_POSITION_SILVER_HEAD; break; }
		//		case 2: {	this->Game::range_track_position = RANGE_TRACK_POSITION_NECK; break; }
		//		case 3: {	this->Game::range_track_position = RANGE_TRACK_POSITION_CLAVICLE; break; }
		//		case 4: {	this->Game::range_track_position = RANGE_TRACK_POSITION_HAND; break; }
		//		case 5: {	this->Game::range_track_position = RANGE_TRACK_POSITION_FOOT; break; }
		//		default:	break;
		//		}
		//	}
		//	ImGui::Separator();
		//	ImGui::DragFloat(u8"设置范围大小", &this->Game::range_track_value, 1.0f, 1, 200, "%.0f");			ImGui::Separator();
		//}
		//if (ImGui::CollapsingHeader(u8"功能专区"))
		//{
		//	if (ImGui::Checkbox(u8"无后座力", &this->Game::no_backseat))
		//		this->CheatEngine::CheckSoftWareBreakPoint(this->CheatEngine::GameBase.Cshell + BulletWithoutBackSeatHookOffset);
		//	ImGui::SameLine();
		//	if (ImGui::Checkbox(u8"零秒换弹", &this->Game::zero_second_reload))
		//	{
		//		this->CheatEngine::CheckSoftWareBreakPoint(this->CheatEngine::GameBase.Cshell + ZeroBulletChange_NewMapHookOffset);
		//		this->CheatEngine::CheckSoftWareBreakPoint(this->CheatEngine::GameBase.Cshell + ZeroBulletChange_OldMapHookOffset);
		//	}	ImGui::SameLine();
		//	if (ImGui::Checkbox(u8"卡不掉血", &this->Game::no_blood_loss))
		//		this->CheatEngine::CheckSoftWareBreakPoint(this->CheatEngine::GameBase.Cshell + KillSelfFuncOffset);
		//	ImGui::SameLine();
		//	if (ImGui::Checkbox(u8"空格飞天", &this->Game::space_fly))
		//		this->CheatEngine::CheckSoftWareBreakPoint(this->CheatEngine::GameBase.Cross + FlyOffset);
		//	ImGui::SameLine();
		//	if (ImGui::Checkbox(u8"武器连发", &this->Game::weapon_fire))
		//		this->CheatEngine::CheckSoftWareBreakPoint(this->CheatEngine::GameBase.Cshell + WeaponFireHookOffset);
		//	ImGui::Separator();
		//	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//	ImGui::Checkbox(u8"B键追敌", &this->Game::teleport_track_enemy);	ImGui::Separator();
		//	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//	if (ImGui::InputFloat(u8"静步加速度", &this->Game::static_acceleration_value, 0.5f, 1.0f, "%0.1f"))
		//		this->CheatEngine::CheckSoftWareBreakPoint(this->CheatEngine::GameBase.Cross + StaticAccelerationHookOffset);
		//	ImGui::Separator();
		//	if (ImGui::InputFloat(u8"下蹲加速度", &this->Game::squat_acceleration_value, 0.5f, 1.0f, "%0.1f"))
		//		this->CheatEngine::CheckSoftWareBreakPoint(this->CheatEngine::GameBase.Cross + SquatAccelerationHookOffset);
		//	ImGui::Separator();
		//}
		if (ImGui::CollapsingHeader(u8"刀距专区"))
		{
			static bool ChangeStatu = FALSE;
			ImGui::Text(u8"请在持刀状态下修改刀距！还原刀距时需要手持武器先还原");			ImGui::Separator();
			ImGui::Text(u8"当前近战武器：");												ImGui::SameLine();
			ImGui::Text(this->Tools::string_to_utf8(ce->Game::GameBase.WeaponNameValue).c_str());		ImGui::Separator();
			static float KnifeWieldingDistance = 540.0;//挥刀距离
			ImGui::InputFloat(u8"挥刀距离", &KnifeWieldingDistance, 1.0f, 1.0f, "%.0f");	ImGui::Separator();
			static float TapSpeed = 1.2;//轻击速度
			ImGui::InputFloat(u8"轻击速度", &TapSpeed, 0.1f, 1.0f, "%.1f");					ImGui::Separator();
			static float HitSpeed = 1.3;//重击速度
			ImGui::InputFloat(u8"重击速度", &HitSpeed, 0.1f, 1.0f, "%.1f");					ImGui::Separator();
			static float TapRange = 2.0;//轻击范围
			ImGui::InputFloat(u8"轻击范围", &TapRange, 1.0f, 1.0f, "%.0f");					ImGui::Separator();
			static float HitRange = 2.0;//重击范围
			ImGui::InputFloat(u8"重击范围", &HitRange, 1.0f, 1.0f, "%.0f");					ImGui::Separator();
			if (!ChangeStatu)
			{
				if (ImGui::Button(u8"修改刀距"))
				{
					////修改刀距
					//if (this->Game::ChangeKnifeDistance(KnifeWieldingDistance, TapSpeed, HitSpeed, TapRange, HitRange))
					//	ChangeStatu = TRUE;
					//else
					//	ChangeStatu = FALSE;
				}
			}
			else
			{
				if (ImGui::Button(u8"还原刀距"))
				{
					////还原刀距
					//if (this->Game::ReductionKnifeDistance())
					//	ChangeStatu = FALSE;
					//else
					//	ChangeStatu = TRUE;
				}
			}
			ImGui::Separator();
		}
		if (ImGui::CollapsingHeader(u8"菜单专区"))
		{
			static int MenuStyleSwitch = 0;
			ImGui::SetNextItemWidth(68);
			//if (ImGui::Combo(u8"菜单风格", &MenuStyleSwitch, u8"亮白色\0经典色\0半透明\0全透明\0"))
			if (ImGui::Combo(u8"菜单风格", &MenuStyleSwitch, u8"基础色\0亮白色\0经典色\0半透明\0"))
			{
				switch (MenuStyleSwitch)
				{
				case 0: {ImGui::StyleColorsChinaRed(); break; }
				case 1: {ImGui::StyleColorsLight(); break; }
				case 2: {ImGui::StyleColorsDark(); break; }
				case 3: {ImGui::StyleColorsClassic(); break; }
					  //case 3: {ImGui::My_Style_FullyTransparent(); break; }
				default:break;
				}
			}
			ImGui::Separator();
		}
		static systime start_time = this->Tools::GetSystemtime();
		ImGui::Text(u8"辅助启动时间：%4d-%02d-%02d %02d:%02d:%02d", start_time.year, start_time.month, start_time.day, start_time.hour, start_time.min, start_time.sec); ImGui::SameLine();
		ImGui::Text(u8"绘制速度: %.1f FPS", ImGui::GetIO().Framerate);
		ImGui::End();
	}
}

