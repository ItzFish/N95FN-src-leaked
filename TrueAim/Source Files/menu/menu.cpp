/*    YTMcGamer#1337    */
#include "../../Header Files/menu/menu.h"
#include "../../Header Files/includes.h"
#include "../../Header Files/Config/config.h"
#include "../../DiscordHook/Discord.h"
#include "../../Helper/Helper.h"
#include <iostream>

BOOLEAN GetTargetHead(FVector& out) {
	if (!Core::TargetPawn) {
		return FALSE;
	}
}

ID3D11Device* device = nullptr;
ID3D11DeviceContext* immediateContext = nullptr;
ID3D11RenderTargetView* renderTargetView = nullptr;

HRESULT(*PresentOriginal)(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags) = nullptr;
HRESULT(*ResizeOriginal)(IDXGISwapChain* swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags) = nullptr;
WNDPROC oWndProc;

HRESULT __stdcall hk_present(IDXGISwapChain* pthis, UINT sync_interval, UINT flags);

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static bool ShowMenu = true;

const char* items[] = { "  Head", "  Chest", "  Leg", "  Dick" };
const char* current_item = "  Head";

const char* aimmodes[] = { "  None", "  Memory", "  Silent", };
const char* current_aimmode = "  Memory";

const char* aimkeys[] = { "Left Click", "Right Click", "Middle Click", "CAPS", "Left Shift", "Alt", "Q", "Z", "X", "C", "`" };
const char* current_aimkey = "Right Click";

const char* snaplinelocations[] = { "  Top", "  Center", "  Bottom" };
const char* current_snaplinelocation = "  Top";

const char* boxtypes[] = { "  Cornered", "  Cornered Fill", "  Box", "  Box Fill" };
const char* current_boxtype = "  Box Fill";

void ToggleButton(const char* str_id, bool* v)
{
	ImVec2 p = ImGui::GetCursorScreenPos();
	ImDrawList* draw_list = ImGui::GetOverlayDrawList();

	float height = ImGui::GetFrameHeight() - 10;
	float width = height * 1.45f;
	float radius = height * 0.50f;

	ImGui::InvisibleButton(str_id, ImVec2(width, height));
	if (ImGui::IsItemClicked())
		*v = !*v;

	float t = *v ? 1.0f : 0.0f;

	ImGuiContext& g = *GImGui;
	float ANIM_SPEED = 0.05f;
	if (g.LastActiveId == g.CurrentWindow->GetID(str_id))// && g.LastActiveIdTimer < ANIM_SPEED)
	{
		float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
		t = *v ? (t_anim) : (1.0f - t_anim);
	}

	ImU32 col_bg;
	if (ImGui::IsItemHovered())
		col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.61f, 0.04f, 0.05f, 1.0f), ImVec4(0.0f, 0.44f, 0.21f, 1.00f), t));
	else
		col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.61f, 0.04f, 0.05f, 1.0f), ImVec4(0.0f, 0.44f, 0.21f, 1.00f), t));

	draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
	draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius - 1.5f + 1.0f, IM_COL32(0, 0, 0, 255));
	draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
}



float color_red = 1.;
float color_green = 0;
float color_blue = 0;
float color_random = 0.0;
float color_speed = -10.0;


void ImGui::SeparatorRainbow(float red, float green, float blue)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return;
	ImGuiContext& g = *GImGui;

	ImGuiSeparatorFlags flags = (window->DC.LayoutType == ImGuiLayoutType_Horizontal) ? ImGuiSeparatorFlags_Vertical : ImGuiSeparatorFlags_Horizontal;
	IM_ASSERT(ImIsPowerOfTwo((int)(flags & (ImGuiSeperatorFlags_Horizontal | ImGuiSeparatorFlags_Vertical))));
	if (flags & ImGuiSeparatorFlags_Vertical)
	{
		VerticalSeparator();
		return;
	}

	if (window->DC.ColumnsSet)
		PopClipRect();

	float x1 = window->Pos.x;
	float x2 = window->Pos.x + window->Size.x;
	if (!window->DC.GroupStack.empty())
		x1 += window->DC.IndentX;

	const ImRect bb(ImVec2(x1, window->DC.CursorPos.y), ImVec2(x2, window->DC.CursorPos.y));
	ItemSize(ImVec2(0.0f, 0.0f));
	if (!ItemAdd(bb, 0))
	{
		if (window->DC.ColumnsSet)
			PushColumnClipRect();
		return;
	}

	float ColorHSV[3];
	float ColorFloat;

	ImGui::ColorConvertRGBtoHSV(red, green, blue, ColorHSV[0], ColorHSV[1], ColorHSV[2]);
	ColorFloat = ColorHSV[0];
	for (int i = 0; i < 64; i++)
	{
		ColorFloat += 1.0f / 64.0f;
		if (ColorFloat > 1.0f)
			ColorFloat -= 1.0f;
		ImGui::PushStyleColor(ImGuiCol_Separator, (ImVec4)ImColor::HSV(ColorFloat, ColorHSV[1], ColorHSV[2]));
		window->DrawList->AddLine(ImVec2(bb.Min.x + window->Size.x / 64.0f * i, bb.Min.y), ImVec2(bb.Min.x + window->Size.x / 64.0f * (i + 1), bb.Max.y), GetColorU32(ImGuiCol_Separator));
		ImGui::PopStyleColor();
	}

	if (g.LogEnabled)
		//LogRenderedText(NULL, IM_NEWLINE "-------------------------------------------");


		if (window->DC.ColumnsSet)
		{
			PushColumnClipRect();
			window->DC.ColumnsSet->CellMinY = window->DC.CursorPos.y;
		}
}




void ColorChange()
{
	static float Color[3];
	static DWORD Tickcount = 0;
	static DWORD Tickcheck = 0;
	ImGui::ColorConvertRGBtoHSV(color_red, color_green, color_blue, Color[0], Color[1], Color[2]);
	if (GetTickCount() - Tickcount >= 1)
	{
		if (Tickcheck != Tickcount)
		{
			Color[0] += 0.001f * color_speed;
			Tickcheck = Tickcount;
		}
		Tickcount = GetTickCount();
	}
	if (Color[0] < 0.0f) Color[0] += 1.0f;
	ImGui::ColorConvertHSVtoRGB(Color[0], Color[1], Color[2], color_red, color_green, color_blue);
}




VOID AddMarker(ImGuiWindow& window, float width, float height, float* start, PVOID pawn, LPCSTR text, ImU32 color) {
	float minX = FLT_MAX;
	float maxX = -FLT_MAX;
	float minY = FLT_MAX;
	float maxY = -FLT_MAX;
	if (minX < width && maxX > 0 && minY < height && maxY > 0) {
		auto topLeft = ImVec2(minX - 3.0f, minY - 3.0f);
		auto bottomRight = ImVec2(maxX + 3.0f, maxY + 3.0f);
		auto centerTop = ImVec2((topLeft.x + bottomRight.x) / 2.0f, topLeft.y);
		auto root = Util::GetPawnRootLocation(pawn);
		if (root) {
			auto pos = *root;
			float dx = start[0] - pos.X;
			float dy = start[1] - pos.Y;
			float dz = start[2] - pos.Z;

			if (Util::WorldToScreen(width, height, &pos.X)) {
				float dist = Util::SpoofCall(sqrtf, dx * dx + dy * dy + dz * dz) / 1000.0f;

				CHAR modified[0xFF] = { 0 };
				snprintf(modified, sizeof(modified), ("%s [%dm]"), text, static_cast<INT>(dist));

				auto size = ImGui::GetFont()->CalcTextSizeA(window.DrawList->_Data->FontSize, FLT_MAX, 0, modified);
				window.DrawList->AddText(ImVec2(pos.X - size.x / 2.0f, pos.Y - size.y / 2.0f), color, modified);
				window.DrawList->AddText(ImVec2(pos.X - size.x / 2.0f, pos.Y - size.y / 2.0f + 1), ImGui::GetColorU32({ 0.00f, 0.00f, 0.00f, 1.00f }), modified);
				window.DrawList->AddText(ImVec2(pos.X - size.x / 2.0f, pos.Y - size.y / 2.0f - 1), ImGui::GetColorU32({ 0.00f, 0.00f, 0.00f, 1.00f }), modified);
				window.DrawList->AddText(ImVec2(pos.X - size.x / 2.0f - 1, pos.Y - size.y / 2.0f), ImGui::GetColorU32({ 0.00f, 0.00f, 0.00f, 1.00f }), modified);
				window.DrawList->AddText(ImVec2(pos.X - size.x / 2.0f + 1, pos.Y - size.y / 2.0f), ImGui::GetColorU32({ 0.00f, 0.00f, 0.00f, 1.00f }), modified);
			}
		}
	}
}

FLOAT GetDistance(ImGuiWindow& window, float width, float height, float* start, PVOID pawn) {
	auto root = Util::GetPawnRootLocation(pawn);
	float dist;
	if (root) {
		auto pos = *root;
		float dx = start[0] - pos.X;
		float dy = start[1] - pos.Y;
		float dz = start[2] - pos.Z;

		if (Util::WorldToScreen(width, height, &pos.X)) {
			dist = Util::SpoofCall(sqrtf, dx * dx + dy * dy + dz * dz) / 1000.0f;
			return dist;
		}
	}

}

__declspec(dllexport) LRESULT CALLBACK WndProcHook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_KEYUP && (wParam == config_system.keybind.Menu || (ShowMenu && wParam == VK_ESCAPE))) {
		ShowMenu = !ShowMenu;
		ImGui::GetIO().MouseDrawCursor = ShowMenu;

	}
	else if (msg == WM_QUIT && ShowMenu) {
		ExitProcess(0);
	}

	if (ShowMenu) {
		ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
		return TRUE;
	}

	return CallWindowProc(oWndProc, hWnd, msg, wParam, lParam);
}

extern uint64_t base_address = 0;
DWORD processID;
const ImVec4 color = { 255.0,255.0,255.0,1 };
const ImVec4 red = { 0.65,0,0,1 };
const ImVec4 white = { 255.0,255.0,255.0,1 };
const ImVec4 green = { 0.03,0.81,0.14,1 };
const ImVec4 blue = { 0.21960784313,0.56470588235,0.90980392156,1.0 };

ImGuiWindow& BeginScene() {
	ImGui_ImplDX11_NewFrame();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	ImGui::Begin(("##scene"), nullptr, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar);

	auto& io = ImGui::GetIO();
	ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y), ImGuiCond_Always);

	return *ImGui::GetCurrentWindow();
}

char streamsnipena[256] = "Username";

VOID EndScene(ImGuiWindow& window) {
	ColorChange();
	window.DrawList->PushClipRectFullScreen();
	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.17f, 0.18f, 0.2f, 1.0f));

	if (ShowMenu) {





		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.70f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.11f, 0.14f, 0.92f);
		colors[ImGuiCol_Border] = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.43f, 0.43f, 0.43f, 0.39f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.47f, 0.47f, 0.69f, 0.40f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.42f, 0.41f, 0.64f, 0.69f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.27f, 0.27f, 0.54f, 0.83f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.32f, 0.32f, 0.63f, 0.87f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.40f, 0.40f, 0.55f, 0.80f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.40f, 0.40f, 0.80f, 0.30f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.80f, 0.40f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.41f, 0.39f, 0.80f, 0.60f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 0.50f);
		colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.41f, 0.39f, 0.80f, 0.60f);
		colors[ImGuiCol_Button] = ImVec4(0.49f, 0.65f, 0.85f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.92f, 0.0f, 0.54f, 1.0f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.38f, 0.01f, 0.37f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.40f, 0.40f, 0.90f, 0.45f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.53f, 0.53f, 0.87f, 0.80f);
		colors[ImGuiCol_Separator] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.60f, 0.60f, 0.70f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.70f, 0.70f, 0.90f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.16f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.78f, 0.82f, 1.00f, 0.60f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.78f, 0.82f, 1.00f, 0.90f);
		colors[ImGuiCol_CloseButton] = ImVec4(0.50f, 0.50f, 0.90f, 0.50f);
		colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.70f, 0.70f, 0.90f, 0.60f);
		colors[ImGuiCol_CloseButtonActive] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
		colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);

		ImGui::GetStyle().WindowTitleAlign = ImVec2(0.5, 0.5);
		ImGui::GetStyle().FramePadding = ImVec2(4.0, 2.0);
		ImGui::GetStyle().WindowRounding = 0;
		ImGui::GetStyle().GrabMinSize = 12;
		ImGui::GetStyle().ScrollbarSize = 14;
		ImGui::GetStyle().ScrollbarRounding = 0;
		ImGui::GetStyle().FrameRounding = 4.0f;



		ImGuiWindowFlags window_flags = 1;
		static bool no_titlebar = false;
		if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.Fonts->AddFontFromFileTTF(("C:\\Windows\\Fonts\\Fixedsys.ttf"), 12);
		io.Fonts->AddFontFromFileTTF(("C:\\Windows\\Fonts\\Fixedsys.ttf"), 10);
		io.Fonts->AddFontFromFileTTF(("C:\\Windows\\Fonts\\Fixedsys.ttf"), 14);
		io.Fonts->AddFontFromFileTTF(("C:\\Windows\\Fonts\\Fixedsys.ttf"), 18);

		ImGui::SetNextWindowSize({ 650, 400 }, ImGuiCond_Always);
		ImGui::SetNextWindowCollapsed(false, ImGuiCond_Always);

		std::string builddate = __DATE__;
		std::string buildtime = __TIME__;

		std::string kek = " N95 FN - A Project By Cha0t1k & Dreams " + builddate + "   " + __TIME__ + std::to_string(NULL);
		ImGui::Begin((kek.c_str()), reinterpret_cast<bool*>(true), ImGuiWindowFlags_NoCollapse); // start open
		float LineWitdh = 860;
		ImVec2 Loaction = ImGui::GetCursorScreenPos();
		float DynamicRainbow = 0.01;
		static float staticHue = 0;
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		staticHue -= DynamicRainbow;
		if (staticHue < -1.f) staticHue += 1.f;
		for (int i = 0; i < LineWitdh; i++)
		{
			float hue = staticHue + (1.f / (float)LineWitdh) * i;
			if (hue < 0.f) hue += 1.f;
			ImColor cRainbow = ImColor::HSV(hue, 1.f, 1.f);
			draw_list->AddRectFilled(ImVec2(Loaction.x + i, Loaction.y), ImVec2(Loaction.x + i + 1, Loaction.y + 4), cRainbow);
		}

		static int tabb = 0;

		ImGui::Text(" ");
		ImGui::Text(" ");
		{ImGui::SameLine();
		if (ImGui::Button(("Usual Stuff"), ImVec2(150.0f, 0.0f)))
		{
			tabb = 0;
		}
		ImGui::SameLine();

		if (ImGui::Button(("ESP"), ImVec2(150.0f, 0.0f)))
		{
			tabb = 1;
		}
		ImGui::SameLine();
		if (ImGui::Button(("Misc"), ImVec2(150.0f, 0.0f)))
		{
			tabb = 2;
		}
		{ImGui::SameLine();
		if (ImGui::Button(("Colors"), ImVec2(150.0f, 0.0f)))
		{
			tabb = 3;
		}
		}
		}

		if (tabb == 0) {
			ImGui::Text("");
			ImGui::Text("Aim Type:");
			if (ImGui::BeginCombo(" ", current_aimmode, 1))
			{
				for (int n = 0; n < IM_ARRAYSIZE(aimmodes); n++)
				{
					bool is_selected = (current_aimmode == aimmodes[n]);
					if (ImGui::Selectable(aimmodes[n], is_selected))
						current_aimmode = aimmodes[n];

					if (is_selected)
						ImGui::SetItemDefaultFocus();

					if (current_aimmode == "None") {
						config_system.item.Aimbot = false;
						config_system.item.nothing = false;
					}
					if (current_aimmode == "Memory") {
						config_system.item.Aimbot = true;
						config_system.item.SilentAimbot = false;
					}
					if (current_aimmode == "Silent") {
						config_system.item.Aimbot = true;
						config_system.item.SilentAimbot = true;
					}


				}
				ImGui::EndCombo();
			}
			ImGui::Checkbox(("Check Visible"), &config_system.item.CheckVisible);
			ImGui::Checkbox(("Draw Crosshair"), &config_system.item.crosshair);
			if (config_system.item.crosshair)
			{
				ImGui::SliderFloat(("Crosshair Size"), &config_system.item.CrosshairSize, 0.0f, 10.0f, ("%.2f"));
				ImGui::SliderFloat(("Crosshair Thickness"), &config_system.item.CrosshairThickness, 0.0f, 10.0f, ("%.2f"));
			}
			ImGui::Text("Aim Key");
			if (ImGui::BeginCombo("      ", current_aimkey, 2))
			{
				for (int n = 0; n < IM_ARRAYSIZE(aimkeys); n++)
				{
					bool is_selected = (current_aimkey == aimkeys[n]);
					if (ImGui::Selectable(aimkeys[n], is_selected))
						current_aimkey = aimkeys[n];

					if (is_selected)
						ImGui::SetItemDefaultFocus();

					if (current_aimkey == " Left Click") {
						config_system.item.AimKey = " VK_LBUTTON";
					}
					if (current_aimkey == " Right Click") {
						config_system.item.AimKey = " VK_RBUTTON";
					}
					if (current_aimkey == " Middle Click") {
						config_system.item.AimKey = " VK_MBUTTON";
					}
					if (current_aimkey == " CAPS") {
						config_system.item.AimKey = " VK_CAPITAL";
					}
					if (current_aimkey == " Left Shift") {
						config_system.item.AimKey = " VK_LSHIFT";
					}
					if (current_aimkey == " Alt") {
						config_system.item.AimKey = " VK_MENU";
					}
					if (current_aimkey == " Q") {
						config_system.item.AimKey = " 0x51";
					}
					if (current_aimkey == " Z") {
						config_system.item.AimKey = " 0x5A";
					}
					if (current_aimkey == " X") {
						config_system.item.AimKey = " 0x58";
					}
					if (current_aimkey == " C") {
						config_system.item.AimKey = " 0x43";
					}
					if (current_aimkey == " `") {
						config_system.item.AimKey = " VK_OEM_3";

					}
				}
				ImGui::EndCombo();
			}
			ImGui::Text("Aim Part");
			if (ImGui::BeginCombo("   ", current_item, 3))
			{
				for (int n = 0; n < IM_ARRAYSIZE(items); n++)
				{
					bool is_selected = (current_item == items[n]);
					if (ImGui::Selectable(items[n], is_selected))
						current_item = items[n];

					if (is_selected)
						ImGui::SetItemDefaultFocus();

					if (current_item == "  Head") {
						config_system.item.aimpart = "Head";
					}
					if (current_item == "  Chest") {
						config_system.item.aimpart = "Chest";
					}
					if (current_item == "  Leg") {
						config_system.item.aimpart = "Leg";
					}
					if (current_item == "  Dick") {
						config_system.item.aimpart = "Dick";
					}
				}
				ImGui::EndCombo();
			}



			ImGui::Text("FOV");
			ImGui::Checkbox("FOV", &config_system.item.DrawAimbotFOV);
			if (config_system.item.DrawAimbotFOV)
			{
				ImGui::Text("Filled FOV");
				ImGui::Checkbox("Filled", &config_system.item.DrawFilledAimbotFOV);

				ImGui::Text("FOV Circle");
				ImGui::SliderFloat(("Aim FOV"), &config_system.item.AimbotFOV, 0.0f, 1000.0f, ("%.2f"));

				ImGui::SliderFloat(("Aim Smoothing"), &config_system.item.AimbotSlow, 0.0f, 10.0f, ("%.2f"));
			}

		}

		else if (tabb == 1) {
			ImGui::Text("");
			ImGui::Checkbox(("Player ESP"), &config_system.item.Players);

			/*ImGui::Text("WeakPoint");
			ImGui::Switch(("Weakpoint"), &config_system.item.weakpoint);*/

			if (config_system.item.Players) {
				ImGui::Checkbox(("Box ESP"), &config_system.item.PlayerBox);
				if (config_system.item.PlayerBox) {

					ImGui::Text("Box Type");
					if (ImGui::BeginCombo("    ", current_boxtype, 4))
					{
						for (int n = 0; n < IM_ARRAYSIZE(boxtypes); n++)
						{
							bool is_selected = (current_boxtype == boxtypes[n]);
							if (ImGui::Selectable(boxtypes[n], is_selected))
								current_boxtype = boxtypes[n];

							if (is_selected)
								ImGui::SetItemDefaultFocus();

							if (current_boxtype == "  Cornered") {
								config_system.item.BoxType = "Cornered";
							}
							if (current_boxtype == "  Cornered Fill") {
								config_system.item.BoxType = "Cornered Fill";
							}
							if (current_boxtype == "  Box") {
								config_system.item.BoxType = "Box";
							}
							if (current_boxtype == "  Box Fill") {
								config_system.item.BoxType = "Box Fill";
							}

						}
						ImGui::EndCombo();
					}


				}
				ImGui::Checkbox(("Snaplines"), &config_system.item.PlayerLines);
				if (config_system.item.PlayerLines) {

					ImGui::Text("Snapline Position");
					if (ImGui::BeginCombo("     ", current_snaplinelocation, 5))
					{
						for (int n = 0; n < IM_ARRAYSIZE(snaplinelocations); n++)
						{
							bool is_selected = (current_snaplinelocation == snaplinelocations[n]);
							if (ImGui::Selectable(snaplinelocations[n], is_selected))
								current_snaplinelocation = snaplinelocations[n];

							if (is_selected)
								ImGui::SetItemDefaultFocus();

							if (current_snaplinelocation == "  Top") {
								config_system.item.SnaplineLocation = "Top";
							}

							if (current_snaplinelocation == "  Center") {
								config_system.item.SnaplineLocation = "Center";
							}
							if (current_snaplinelocation == "  Bottom") {
								config_system.item.SnaplineLocation = "Bottom";
							}
						}
						ImGui::EndCombo();
					}

					ImGui::Checkbox(("PlayerNames"), &config_system.item.PlayerNames);
					ImGui::Spacing();

				}
				ImGui::Spacing();

				ImGui::PushItemWidth(150.0f);
			}
			ImGui::Checkbox(("Ammo ESP"), &config_system.item.Ammo);
			ImGui::Checkbox(("Chest ESP"), &config_system.item.Chest);
			ImGui::Checkbox(("Weapon ESP"), &config_system.item.Weapons);
			ImGui::Checkbox(("Boat ESP"), &config_system.item.boat);
			ImGui::Checkbox(("Helicopter ESP"), &config_system.item.chopper);
			ImGui::Text(" ");
			if (config_system.item.Weapons) {
				ImGui::SliderInt(("Minimum Weapon Rarity"), &config_system.item.MinWeaponTier, 0, 6);
			}

		}



		else if (tabb == 2) {
			ImGui::Text(" ");
			ImGui::Checkbox(("FOV Slider"), &config_system.item.FOVSlider);
			ImGui::SliderFloat(("FOV Slider:"), &config_system.item.FOV, 60.0f, 160.0f, ("%.2f"));
			ImGui::Checkbox(("Freecam"), &config_system.item.FreeCam);
			ImGui::SliderFloat(("Freecam Speed"), &config_system.item.FreeCamSpeed, 0.0f, 35.0f, ("%.2f"));
			ImGui::Checkbox(("Bullet TP"), &config_system.item.BulletTP);
			if (ImGui::Button("Kill All (RISKY AF!)")) {
				system("start https://www.youtube.com/watch?v=dQw4w9WgXcQ");

			}
			ImGui::Text("                        ");

			if (ImGui::Button("Mobile Graphics")) {
				config_system.item.FOV = 18180.0f;
			}

			if (ImGui::Button("Dreams's YT")) {
				system("start https://www.youtube.com/channel/UCviF2_osis0VJj2WZ-EB0ew");

			}

			if (ImGui::Button("Cha0t1k's YT")) {
				system("start https://www.youtube.com/channel/UCDr5PIxWbRefu1_yQ3NWMUA");

			}
		}

		else if (tabb == 3) {
		ImGui::PushItemWidth(100.0f);
		ImGui::Text("ESP");
		ImGui::SameLine();
		ImGui::ColorPicker3(("Box"), config_system.item.BoxESP, ImGuiColorEditFlags_NoInputs);
		ImGui::SameLine();
		ImGui::ColorPicker3(("Lines"), config_system.item.LineESP, ImGuiColorEditFlags_NoInputs);
		ImGui::SameLine();
		ImGui::ColorPicker3(("Aim\nFOV"), config_system.item.FOVCircleColor, ImGuiColorEditFlags_NoInputs);
		ImGui::Text("Skeleton ESP");
		ImGui::ColorPicker3(("Is\nVisible"), config_system.item.PlayerVisibleColor, ImGuiColorEditFlags_NoInputs);
		ImGui::SameLine();
		ImGui::ColorPicker3(("Not\nVisible"), config_system.item.PlayerNotVisibleColor, ImGuiColorEditFlags_NoInputs);
		ImGui::SameLine();
		ImGui::ColorPicker3(("Teammates"), config_system.item.PlayerTeammate, ImGuiColorEditFlags_NoInputs);
		ImGui::PopItemWidth();
		ImGui::SliderFloat(("Box Opacity"), &config_system.item.BoxESPOpacity, 0.0f, 1.0f, ("%.2f"));
		ImGui::SliderFloat(("FOV Circle Opacity"), &config_system.item.FOVCircleOpacity, 0.0f, 1.0f, ("%.2f"));
		ImGui::SliderFloat(("FOV Circle Filled Opacity"), &config_system.item.FOVCircleFilledOpacity, 0.0f, 1.0f, ("%.2f"));


		}

		/*ImGui::Checkbox(("No Spread"), &Settings.NoSpreadAimbot);
		ImGui::Checkbox(("No Reload"), &Settings.InstantReload);*/



		ImGui::End();
	}

	ImGui::PopStyleColor();
	ImGui::Render();
	

}



auto success = FALSE;


VOID AddLine(ImGuiWindow& window, float width, float height, float a[3], float b[3], ImU32 color, float& minX, float& maxX, float& minY, float& maxY) {
	float ac[3] = { a[0], a[1], a[2] };
	float bc[3] = { b[0], b[1], b[2] };
	if (Util::WorldToScreen(width, height, ac) && Util::WorldToScreen(width, height, bc)) {
		window.DrawList->AddLine(ImVec2(ac[0], ac[1]), ImVec2(bc[0], bc[1]), color, 2.0f);

		minX = min(ac[0], minX);
		minX = min(bc[0], minX);

		maxX = max(ac[0], maxX);
		maxX = max(bc[0], maxX);

		minY = min(ac[1], minY);
		minY = min(bc[1], minY);

		maxY = max(ac[1], maxY);
		maxY = max(bc[1], maxY);
	}
}


__declspec(dllexport) HRESULT PresentHook(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags) {
	static float width = 0;
	static float height = 0;
	static HWND hWnd = 0;
	if (!device) {
		swapChain->GetDevice(__uuidof(device), reinterpret_cast<PVOID*>(&device));
		device->GetImmediateContext(&immediateContext);

		ID3D11Texture2D* renderTarget = nullptr;
		swapChain->GetBuffer(0, __uuidof(renderTarget), reinterpret_cast<PVOID*>(&renderTarget));
		device->CreateRenderTargetView(renderTarget, nullptr, &renderTargetView);
		renderTarget->Release();

		ID3D11Texture2D* backBuffer = 0;
		swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (PVOID*)&backBuffer);
		D3D11_TEXTURE2D_DESC backBufferDesc = { 0 };
		backBuffer->GetDesc(&backBufferDesc);

		hWnd = FindWindow((L"UnrealWindow"), (L"Fortnite  "));
		if (!width) {
			oWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProcHook)));
		}

		width = (float)backBufferDesc.Width;
		height = (float)backBufferDesc.Height;
		backBuffer->Release();

		ImGui::GetIO().Fonts->AddFontFromFileTTF(("C:\\Windows\\Fonts\\Fixedsys.ttf"), 13.0f);

		ImGui_ImplDX11_Init(hWnd, device, immediateContext);
		ImGui_ImplDX11_CreateDeviceObjects();
	}
	immediateContext->OMSetRenderTargets(1, &renderTargetView, nullptr);
	////// reading
	auto& window = BeginScene();
	////// readin
	auto success = FALSE;
	do {
		float closestDistance = FLT_MAX;
		PVOID closestPawn = NULL;

		auto world = *Offsets::uWorld;
		if (!world) break;

		auto gameInstance = ReadPointer(world, Offsets::Engine::World::OwningGameInstance);
		if (!gameInstance) break;

		auto localPlayers = ReadPointer(gameInstance, Offsets::Engine::GameInstance::LocalPlayers);
		if (!localPlayers) break;

		auto localPlayer = ReadPointer(localPlayers, 0);
		if (!localPlayer) break;

		auto localPlayerController = ReadPointer(localPlayer, Offsets::Engine::Player::PlayerController);
		if (!localPlayerController) break;

		auto localPlayerPawn = reinterpret_cast<UObject*>(ReadPointer(localPlayerController, Offsets::Engine::PlayerController::AcknowledgedPawn));
		if (!localPlayerPawn) break;

		auto localPlayerWeapon = ReadPointer(localPlayerPawn, Offsets::FortniteGame::FortPawn::CurrentWeapon);
		if (!localPlayerWeapon) break;

		auto localPlayerRoot = ReadPointer(localPlayerPawn, Offsets::Engine::Actor::RootComponent);
		if (!localPlayerRoot) break;

		auto localPlayerState = ReadPointer(localPlayerPawn, Offsets::Engine::Pawn::PlayerState);
		if (!localPlayerState) break;

		auto localPlayerLocation = reinterpret_cast<float*>(reinterpret_cast<PBYTE>(localPlayerRoot) + Offsets::Engine::SceneComponent::RelativeLocation);
		auto localPlayerTeamIndex = ReadDWORD(localPlayerState, Offsets::FortniteGame::FortPlayerStateAthena::TeamIndex);

		auto weaponName = Util::GetObjectFirstName((UObject*)localPlayerWeapon);
		auto isProjectileWeapon = wcsstr(weaponName.c_str(), L"Rifle_Sniper");

		Core::LocalPlayerPawn = localPlayerPawn;
		Core::LocalPlayerController = localPlayerController;


		std::vector<PVOID> playerPawns;
		for (auto li = 0UL; li < ReadDWORD(world, Offsets::Engine::World::Levels + sizeof(PVOID)); ++li) {
			auto levels = ReadPointer(world, Offsets::Engine::World::Levels);//Levels
			if (!levels) break;

			auto level = ReadPointer(levels, li * sizeof(PVOID));
			if (!level) continue;

			for (auto ai = 0UL; ai < ReadDWORD(level, Offsets::Engine::Level::AActors + sizeof(PVOID)); ++ai) {
				auto actors = ReadPointer(level, Offsets::Engine::Level::AActors);
				if (!actors) break;

				auto pawn = reinterpret_cast<UObject*>(ReadPointer(actors, ai * sizeof(PVOID)));
				if (!pawn || pawn == localPlayerPawn) continue;

				auto name = Util::GetObjectFirstName(pawn);
				if (wcsstr(name.c_str(), L"PlayerPawn_Athena_C") || wcsstr(name.c_str(), L"PlayerPawn_Athena_Phoebe_C") || wcsstr(name.c_str(), L"BP_MangPlayerPawn")) {
					playerPawns.push_back(pawn);
				}
				else if (wcsstr(name.c_str(), L"FortPickupAthena")) {
					auto item = ReadPointer(pawn, Offsets::FortniteGame::FortPickup::PrimaryPickupItemEntry + Offsets::FortniteGame::FortItemEntry::ItemDefinition);
					if (!item) continue;
				

				auto itemName = reinterpret_cast<FText*>(ReadPointer(item, Offsets::FortniteGame::FortItemDefinition::DisplayName));
				if (!itemName || !itemName->c_str()) continue;

				auto isAmmo = wcsstr(itemName->c_str(), L"Ammo: ");
				auto kek ReadBYTE(item, Offsets::FortniteGame::FortItemDefinition::Tier);
				if (kek <= config_system.item.MinWeaponTier) continue;
				std::wcout << L"\nBYTE:\\" << kek << L"\\";
				CHAR text[0xFF] = { 0 };
				wcstombs(text, itemName->c_str() + (isAmmo ? 6 : 0), sizeof(text));
				ImU32 common = ImGui::GetColorU32({ 123.0f, 123.0f, 123.0f, 1.0f }); //grey
				ImU32 noncommon = ImGui::GetColorU32({ 0.f, 1.f, 0.1f, 1.f }); // green
				ImU32 rare = ImGui::GetColorU32({ 0.2f, 0.3f, 1.f, 01.f }); // blue
				ImU32 epic = ImGui::GetColorU32({ 1.0f, 0.0f, 1.0f, 1.0f }); //purple
				ImU32 legendary = ImGui::GetColorU32({ 1.f, 0.5f, 0.05f, 1.0f }); //
				ImU32 mythic = ImGui::GetColorU32({ 1.f, 0.8f, 0.02f, 1.0f });

				if (kek == 0 && config_system.item.Ammo == true) {
					AddMarker(window, width, height, localPlayerLocation, pawn, text, ImGui::GetColorU32({ 0.75f, 0.75f, 0.75f, 1.0f }));
					std::cout << "\n 0";
				}
				else if (kek == 1) {
					AddMarker(window, width, height, localPlayerLocation, pawn, text, common);
					std::cout << "\n 1";
				}
				else if (kek == 2) {
					AddMarker(window, width, height, localPlayerLocation, pawn, text, noncommon);
					std::cout << "\n 2";
				}
				else if (kek == 3) {
					AddMarker(window, width, height, localPlayerLocation, pawn, text, rare);
					std::cout << "\n 3";
				}
				else if (kek == 4) {
					AddMarker(window, width, height, localPlayerLocation, pawn, text, epic);
					std::cout << "\n 4";
				}
				else if (kek == 5) {
					AddMarker(window, width, height, localPlayerLocation, pawn, text, legendary);
					std::cout << "\n 5";
				}
				else if (kek == 6) {
					AddMarker(window, width, height, localPlayerLocation, pawn, text, mythic);
					std::cout << "\n 6";
				}


				}

				else if (config_system.item.Containers && wcsstr(name.c_str(), L"AthenaSupplyDrop_Llama")) {
					AddMarker(window, width, height, localPlayerLocation, pawn, "Llama", ImGui::GetColorU32({ 1.0f, 0.0f, 0.0f, 1.0f }));
				}
				else if (config_system.item.Containers && wcsstr(name.c_str(), L"Tiered_Chest") && !((ReadBYTE(pawn, Offsets::FortniteGame::BuildingContainer::bAlreadySearched) >> 7) & 1)) {
					AddMarker(window, width, height, localPlayerLocation, pawn, "Chest", ImGui::GetColorU32({ 1.0f, 0.84f, 0.0f, 1.0f }));
				}
				else if (config_system.item.Ammo && wcsstr(name.c_str(), L"Tiered_Ammo") && !((ReadBYTE(pawn, Offsets::FortniteGame::BuildingContainer::bAlreadySearched) >> 7) & 1)) {
					AddMarker(window, width, height, localPlayerLocation, pawn, "Ammo Box", ImGui::GetColorU32({ 0.75f, 0.75f, 0.75f, 1.0f }));
				}
				else if (config_system.item.Vehicle && wcsstr(name.c_str(), L"HoagieVehicle_C") && !((ReadBYTE(pawn, Offsets::FortniteGame::BuildingContainer::bAlreadySearched) >> 7) & 1)) {
					AddMarker(window, width, height, localPlayerLocation, pawn, "Helicopter", ImGui::GetColorU32({ 1.0f, 0.84f, 0.0f, 1.0f }));
				}
				else if (config_system.item.Vehicle && wcsstr(name.c_str(), L"MeatballVehicle") && !((ReadBYTE(pawn, Offsets::FortniteGame::BuildingContainer::bAlreadySearched) >> 7) & 1)) {
					AddMarker(window, width, height, localPlayerLocation, pawn, "Boat", ImGui::GetColorU32({ 1.f, 0.f, 0.f, 1.0f }));

				}
				

				

				if (Util::SpoofCall(GetAsyncKeyState, config_system.keybind.Airstuck1)) { *reinterpret_cast<float*>(reinterpret_cast<PBYTE>(Core::LocalPlayerPawn) + 0x98) = 0; }
				if (Util::SpoofCall(GetAsyncKeyState, config_system.keybind.Airstuck2)) { *reinterpret_cast<float*>(reinterpret_cast<PBYTE>(Core::LocalPlayerPawn) + 0x98) = 1; }
				else if (config_system.item.boat && wcsstr(name.c_str(), L"MeatballVehicle_L")) {

					AddMarker(window, width, height, localPlayerLocation, pawn, "Boat", ImGui::GetColorU32({ 1.0f, 0.0f, 0.0f, 1.0f }));
				}


			}
		}

		float CurrentAimPointer[3] = { 0 };
		float AimPointer;
		if (config_system.item.AimPoint == 0) {
			AimPointer = BONE_HEAD_ID;
		}
		else if (config_system.item.AimPoint == 1) {
			AimPointer = BONE_NECK_ID;
		}
		else if (config_system.item.AimPoint == 2) {
			AimPointer = BONE_CHEST_ID;
		}
		else if (config_system.item.AimPoint == 3) {
			AimPointer = BONE_PELVIS_ID;
		}
		else if (config_system.item.AimPoint == 4) {
			AimPointer = BONE_RIGHTELBOW_ID;
		}
		else if (config_system.item.AimPoint == 5) {
			AimPointer = BONE_LEFTELBOW_ID;
		}
		else if (config_system.item.AimPoint == 6) {
			AimPointer = BONE_RIGHTTHIGH_ID;
		}
		else if (config_system.item.AimPoint == 7) {
			AimPointer = BONE_LEFTTHIGH_ID;
		}
		else if (config_system.item.AimPoint == 8) { // automatic

		}
		bool wkekj;

			 

		for (auto pawn : playerPawns)
		{
			auto state = ReadPointer(pawn, Offsets::Engine::Pawn::PlayerState);
			if (!state) continue;

			auto mesh = ReadPointer(pawn, Offsets::Engine::Character::Mesh);
			if (!mesh) continue;

			auto bones = ReadPointer(mesh, Offsets::Engine::StaticMeshComponent::StaticMesh);
			if (!bones) bones = ReadPointer(mesh, Offsets::Engine::StaticMeshComponent::StaticMesh + 0x10);
			if (!bones) continue;

			float compMatrix[4][4] = { 0 };
			Util::ToMatrixWithScale(reinterpret_cast<float*>(reinterpret_cast<PBYTE>(mesh) + 0x1C0), compMatrix);

			// Top
			float head[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 66, head);

			float neck[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 65, neck);

			float chest[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 36, chest);

			float pelvis[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 2, pelvis);

			// Arms
			float leftShoulder[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 9, leftShoulder);

			float rightShoulder[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 62, rightShoulder);

			float leftElbow[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 10, leftElbow);

			float rightElbow[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 38, rightElbow);

			float leftHand[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 11, leftHand);

			float rightHand[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 39, rightHand);

			// Legs
			float leftLeg[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 67, leftLeg);

			float rightLeg[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 74, rightLeg);

			float leftThigh[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 73, leftThigh);

			float rightThigh[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 80, rightThigh);

			float leftFoot[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 68, leftFoot);

			float rightFoot[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 75, rightFoot);

			float leftFeet[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 71, leftFeet);

			float rightFeet[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 78, rightFeet);

			float leftFeetFinger[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 72, leftFeetFinger);

			float rightFeetFinger[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 79, rightFeetFinger);

			auto color = ImGui::GetColorU32({ red });
			auto dickhead = ImGui::GetColorU32({ white });
			FVector viewPoint = { 0 };
			bool lineofsightk2 = false;
			if (ReadDWORD(state, 0xE68) == localPlayerTeamIndex) {
				color = ImGui::GetColorU32({ 0.0f, 1.0f, 0.0f, 1.0f });
			}
			else if ((ReadBYTE(pawn, Offsets::FortniteGame::FortPawn::bIsDBNO) & 1) && (isProjectileWeapon || Util::LineOfSightTo(localPlayerController, pawn, &viewPoint))) {
				lineofsightk2 = true;
				color = ImGui::GetColorU32({ green });
				if (config_system.item.AutoAimbot) {
					auto dx = head[0] - localPlayerLocation[0];
					auto dy = head[1] - localPlayerLocation[1];
					auto dz = head[2] - localPlayerLocation[2];
					auto dist = dx * dx + dy * dy + dz * dz;
					if (dist < closestDistance) {
						closestDistance = dist;
						closestPawn = pawn;


					}
				}
				else
				{
					auto w2s = *reinterpret_cast<FVector*>(head);
					if (Util::WorldToScreen(width, height, &w2s.X)) {
						auto dx = w2s.X - (width / 2);
						auto dy = w2s.Y - (height / 2);

						auto dist = Util::SpoofCall(sqrtf, dx * dx + dy * dy);
						if (dist < config_system.item.AimbotFOV && dist < closestDistance) {
							closestDistance = dist;
							closestPawn = pawn;


						}
					}
				}
			}


				if (Util::SpoofCall(GetAsyncKeyState, config_system.keybind.Airstuck1)) {
					*reinterpret_cast<float*>(reinterpret_cast<PBYTE>(Core::LocalPlayerPawn) + 0x98) = 0;
						config_system.item.AirStuck != config_system.item.AirStuck;
				}
				if (Util::SpoofCall(GetAsyncKeyState, config_system.keybind.Airstuck2)) {
					*reinterpret_cast<float*>(reinterpret_cast<PBYTE>(Core::LocalPlayerPawn) + 0x98) = 1;
					config_system.item.AirStuck != config_system.item.AirStuck;
				}
			
			

			if (!config_system.item.Players) continue;


			if (config_system.item.crosshair) {
				window.DrawList->AddLine(ImVec2(width / 2 - 15, height / 2), ImVec2(width / 2 + 15, height / 2), ImGui::GetColorU32(white), 2);
				window.DrawList->AddLine(ImVec2(width / 2, height / 2 - 15), ImVec2(width / 2, height / 2 + 15), ImGui::GetColorU32(white), 2);
			}

			if (config_system.item.PlayerLines) {

				if (config_system.item.SnaplineLocation == "Top") {

					auto end = *reinterpret_cast<FVector*>(head);
					if (Util::WorldToScreen(width, height, &end.X)) {
						if (lineofsightk2)
						{
							window.DrawList->AddLine(ImVec2(width / 2, height - 1080), ImVec2(end.X, end.Y), ImGui::GetColorU32({ green }));
						}
						else
						{
							//	window.DrawList->AddLine(ImVec2(width / 2, height - 1080), ImVec2(end.X, end.Y), color);
							window.DrawList->AddLine(ImVec2(width / 2, height - 1080), ImVec2(end.X, end.Y), ImGui::GetColorU32({ red }));
						}
					}

				}
		


				if (config_system.item.SnaplineLocation == "Center") {

					auto end = *reinterpret_cast<FVector*>(head);
					if (Util::WorldToScreen(width, height, &end.X)) {
						if (lineofsightk2)
						{
							window.DrawList->AddLine(ImVec2(width / 2, height - 540), ImVec2(end.X, end.Y), ImGui::GetColorU32({ green }));
						}
						else
						{
							//	window.DrawList->AddLine(ImVec2(width / 2, height - 1080), ImVec2(end.X, end.Y), color);
							window.DrawList->AddLine(ImVec2(width / 2, height - 540), ImVec2(end.X, end.Y), ImGui::GetColorU32({ red }));
						}
					}

				}

				if (config_system.item.SnaplineLocation == "Bottom") {

					auto end = *reinterpret_cast<FVector*>(head);
					if (Util::WorldToScreen(width, height, &end.X)) {
						if (lineofsightk2)
						{
							window.DrawList->AddLine(ImVec2(width / 2, height), ImVec2(end.X, end.Y), ImGui::GetColorU32({ green }));
						}
						else
						{
							//	window.DrawList->AddLine(ImVec2(width / 2, height - 1080), ImVec2(end.X, end.Y), color);
							window.DrawList->AddLine(ImVec2(width / 2, height), ImVec2(end.X, end.Y), ImGui::GetColorU32({ red }));
						}
					}

				}

			}

			float minX = FLT_MAX;
			float maxX = -FLT_MAX;
			float minY = FLT_MAX;
			float maxY = -FLT_MAX;

			if (config_system.item.Players) {
				AddLine(window, width, height, head, neck, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, neck, pelvis, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, chest, leftShoulder, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, chest, rightShoulder, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, leftShoulder, leftElbow, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, rightShoulder, rightElbow, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, leftElbow, leftHand, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, rightElbow, rightHand, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, pelvis, leftLeg, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, pelvis, rightLeg, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, leftLeg, leftThigh, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, rightLeg, rightThigh, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, leftThigh, leftFoot, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, rightThigh, rightFoot, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, leftFoot, leftFeet, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, rightFoot, rightFeet, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, leftFeet, leftFeetFinger, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, rightFeet, rightFeetFinger, color, minX, maxX, minY, maxY);

			}

			/*float dist;
			if (dist >= 100)
				dist = 75;*/

			auto root = Util::GetPawnRootLocation(pawn);
			float dx;
			float dy;
			float dz;
			float dist;
			if (root) {
				auto pos = *root;
				dx = localPlayerLocation[0] - pos.X;
				dy = localPlayerLocation[1] - pos.Y;
				dz = localPlayerLocation[2] - pos.Z;

				if (Util::WorldToScreen(width, height, &pos.X)) {
					dist = Util::SpoofCall(sqrtf, dx * dx + dy * dy + dz * dz) / 1500.0f;
				}
			}


			if (dist >= 100)
				dist = 75;

			if (minX < width && maxX > 0 && minY < height && maxY > 0) {
				auto topLeft = ImVec2(minX - 3.0f, minY - 3.0f);
				auto bottomRight = ImVec2(maxX + 3.0f, maxY + 3.0f);
				float lineW = (width / 5);
				float lineH = (height / 6);
				float lineT = 1;

				auto w2sa = *reinterpret_cast<FVector*>(head);
				Util::WorldToScreen(width, height, &w2sa.X);
				Util::WorldToScreen(width, height, &w2sa.Y);
				auto X = w2sa.X;
				auto Y = w2sa.Y;


				auto bottomRightLEFT = ImVec2(maxX - config_system.item.CornerSize + dist, maxY + 2.5f);
				auto bottomRightUP = ImVec2(maxX + 3.0f, maxY - config_system.item.CornerSize + dist);
				auto topRight = ImVec2(maxX + 3.0f, minY - 3.0f);
				auto topRightLEFT = ImVec2(maxX - config_system.item.CornerSize + dist, minY - 3.0f);
				auto topRightDOWN = ImVec2(maxX + 3.0f, minY + config_system.item.CornerSize - dist);

				auto bottomLeft = ImVec2(minX - 3.0f, maxY + 3.f);
				auto bottomLeftRIGHT = ImVec2(minX + config_system.item.CornerSize - dist, maxY + 3.f);
				auto bottomLeftUP = ImVec2(minX - 3.0f, maxY - config_system.item.CornerSize + dist);
				auto topLeftRIGHT = ImVec2(minX + config_system.item.CornerSize - dist, minY - 3.0f);
				auto topLeftDOWN = ImVec2(minX - 3.0f, minY + config_system.item.CornerSize - dist);


				if (config_system.item.PlayerBox && config_system.item.BoxType == "Cornered") {

					ImU32 kek = ImGui::GetColorU32({ ImGui::GetColorU32({ 1.f, 0.f, 0.f, 1.0f }) });
					window.DrawList->AddLine(topLeft, topLeftRIGHT, ImGui::GetColorU32({ white }), 1.00f);
					window.DrawList->AddLine(topLeft, topLeftDOWN, ImGui::GetColorU32({ white }), 1.00f);

					window.DrawList->AddLine(bottomRight, bottomRightLEFT, ImGui::GetColorU32({ white }), 1.5f);
					window.DrawList->AddLine(bottomRight, bottomRightUP, ImGui::GetColorU32({ white }), 1.5f);

					window.DrawList->AddLine(topRight, topRightLEFT, ImGui::GetColorU32({ white }), 1.5f);
					window.DrawList->AddLine(topRight, topRightDOWN, ImGui::GetColorU32({ white }), 1.5f);

					window.DrawList->AddLine(bottomLeft, bottomLeftRIGHT, ImGui::GetColorU32({ white }), 1.5f);
					window.DrawList->AddLine(bottomLeft, bottomLeftUP, ImGui::GetColorU32({ white }), 1.5f);

				}

				if (config_system.item.PlayerBox && config_system.item.BoxType == "Cornered Fill") {


					window.DrawList->AddRectFilled(topLeft, bottomRight, ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, 0.50f }));
					ImU32 kek = ImGui::GetColorU32({ ImGui::GetColorU32({ 1.f, 0.f, 0.f, 1.0f }) });
					window.DrawList->AddLine(topLeft, topLeftRIGHT, ImGui::GetColorU32({ white }), 1.00f);
					window.DrawList->AddLine(topLeft, topLeftDOWN, ImGui::GetColorU32({ white }), 1.00f);

					window.DrawList->AddLine(bottomRight, bottomRightLEFT, ImGui::GetColorU32({ white }), 1.5f);
					window.DrawList->AddLine(bottomRight, bottomRightUP, ImGui::GetColorU32({ white }), 1.5f);

					window.DrawList->AddLine(topRight, topRightLEFT, ImGui::GetColorU32({ white }), 1.5f);
					window.DrawList->AddLine(topRight, topRightDOWN, ImGui::GetColorU32({ white }), 1.5f);

					window.DrawList->AddLine(bottomLeft, bottomLeftRIGHT, ImGui::GetColorU32({ white }), 1.5f);
					window.DrawList->AddLine(bottomLeft, bottomLeftUP, ImGui::GetColorU32({ white }), 1.5f);

				}

				if (config_system.item.PlayerBox && config_system.item.BoxType == "Box") {

					auto Spikey1 = ImVec2(maxX + 4.0f, maxY + 4.0f);
					auto Spikey2 = ImVec2(minX - 4.0f, minY - 4.0f);;


					window.DrawList->AddRect(Spikey1, Spikey2, ImGui::GetColorU32({ white }), 0.5, 15, 1.5f);

				}

				if (config_system.item.PlayerBox && config_system.item.BoxType == "Box Fill") {

					auto Spikey1 = ImVec2(maxX + 4.0f, maxY + 4.0f);
					auto Spikey2 = ImVec2(minX - 4.0f, minY - 4.0f);;

					window.DrawList->AddRectFilled(topLeft, bottomRight, ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, 0.50f }));
					window.DrawList->AddRect(Spikey1, Spikey2, ImGui::GetColorU32({ white }), 0.5, 15, 1.5f);

				}
				if (config_system.item.PlayerNames) {
					FString playerName;
					Core::ProcessEvent(state, Offsets::Engine::PlayerState::GetPlayerName, &playerName, 0);
					if (playerName.c_str()) {
						CHAR copy[0xFF] = { 0 };
						auto w2s = *reinterpret_cast<FVector*>(head);
						float dist;
						if (Util::WorldToScreen(width, height, &w2s.X)) {
							auto dx = w2s.X;
							auto dy = w2s.Y;
							auto dz = w2s.Z;
							dist = Util::SpoofCall(sqrtf, dx * dx + dy * dy + dz * dz) / 100.0f;
						}
						CHAR lel[0xFF] = { 0 };
						wcstombs(lel, playerName.c_str(), sizeof(lel));
						Util::FreeInternal(playerName.c_str());
						snprintf(copy, sizeof(copy), ("%s [%dm]"), lel, static_cast<INT>(dist));
						auto centerTop = ImVec2((topLeft.x + bottomRight.x) / 2.0f, topLeft.y);
						auto size = ImGui::GetFont()->CalcTextSizeA(window.DrawList->_Data->FontSize, FLT_MAX, 0, copy);
						//	window.DrawList->AddRectFilled(ImVec2(centerTop.x - size.x / 2.0f, centerTop.y - size.y + 3.0f), ImVec2(centerTop.x + size.x / 2.0f, centerTop.y), ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, 0.4f }));
						ImVec2 kek = ImVec2(centerTop.x - size.x / 2.0f + 10, centerTop.y - size.y);
						//	window.DrawList->AddRectFilled(kek, ImVec2(centerTop.y - size.y), ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, 0.20f }));
						std::string jsj = copy;
						if (jsj.find(streamsnipena) != std::string::npos) {
							window.DrawList->AddText(ImVec2(centerTop.x - size.x / 2.0f + 10, centerTop.y - size.y), ImGui::GetColorU32({ 1.0f, 0.0f, 1.0f, 1.0f }), copy);
						}
						else
						{
							window.DrawList->AddText(ImVec2(centerTop.x - size.x / 2.0f + 10, centerTop.y - size.y), color, copy);
						}
					}
				}
			}
		}
		if (config_system.item.AimKey = "VK_RBUTTON") {
			if (config_system.item.Aimbot && closestPawn && Util::SpoofCall(GetAsyncKeyState, VK_RBUTTON) < 0 && Util::SpoofCall(GetForegroundWindow) == hWnd) {
				Core::TargetPawn = closestPawn;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
				//	printf("\nworked?");
			}
			else if (config_system.item.SilentAimbot && closestPawn && Util::SpoofCall(GetAsyncKeyState, VK_RBUTTON) < 0 && Util::SpoofCall(GetForegroundWindow) == hWnd) {
				Core::TargetPawn = closestPawn;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
			}
			else {
				Core::TargetPawn = nullptr;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
			}
		}

		if (config_system.item.AimKey = "VK_LBUTTON") {
			if (config_system.item.Aimbot && closestPawn && Util::SpoofCall(GetAsyncKeyState, VK_RBUTTON) < 0 && Util::SpoofCall(GetForegroundWindow) == hWnd) {
				Core::TargetPawn = closestPawn;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
				//	printf("\nworked?");
			}
			else if (config_system.item.SilentAimbot && closestPawn && Util::SpoofCall(GetAsyncKeyState, VK_RBUTTON) < 0 && Util::SpoofCall(GetForegroundWindow) == hWnd) {
				Core::TargetPawn = closestPawn;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
			}
			else {
				Core::TargetPawn = nullptr;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
			}
		}

		if (config_system.item.AimKey = "VK_MBUTTON") {
			if (config_system.item.Aimbot && closestPawn && Util::SpoofCall(GetAsyncKeyState, VK_RBUTTON) < 0 && Util::SpoofCall(GetForegroundWindow) == hWnd) {
				Core::TargetPawn = closestPawn;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
				//	printf("\nworked?");
			}
			else if (config_system.item.SilentAimbot && closestPawn && Util::SpoofCall(GetAsyncKeyState, VK_RBUTTON) < 0 && Util::SpoofCall(GetForegroundWindow) == hWnd) {
				Core::TargetPawn = closestPawn;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
			}
			else {
				Core::TargetPawn = nullptr;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
			}
		}

		if (config_system.item.AimKey = "VK_CAPTIAL") {
			if (config_system.item.Aimbot && closestPawn && Util::SpoofCall(GetAsyncKeyState, VK_RBUTTON) < 0 && Util::SpoofCall(GetForegroundWindow) == hWnd) {
				Core::TargetPawn = closestPawn;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
				//	printf("\nworked?");
			}
			else if (config_system.item.SilentAimbot && closestPawn && Util::SpoofCall(GetAsyncKeyState, VK_RBUTTON) < 0 && Util::SpoofCall(GetForegroundWindow) == hWnd) {
				Core::TargetPawn = closestPawn;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
			}
			else {
				Core::TargetPawn = nullptr;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
			}
		}

		if (config_system.item.AimKey = "VK_LSHIFT") {
			if (config_system.item.Aimbot && closestPawn && Util::SpoofCall(GetAsyncKeyState, VK_RBUTTON) < 0 && Util::SpoofCall(GetForegroundWindow) == hWnd) {
				Core::TargetPawn = closestPawn;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
				//	printf("\nworked?");
			}
			else if (config_system.item.SilentAimbot && closestPawn && Util::SpoofCall(GetAsyncKeyState, VK_RBUTTON) < 0 && Util::SpoofCall(GetForegroundWindow) == hWnd) {
				Core::TargetPawn = closestPawn;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
			}
			else {
				Core::TargetPawn = nullptr;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
			}
		}

		if (config_system.item.AimKey = "VK_MENU") {
			if (config_system.item.Aimbot && closestPawn && Util::SpoofCall(GetAsyncKeyState, VK_RBUTTON) < 0 && Util::SpoofCall(GetForegroundWindow) == hWnd) {
				Core::TargetPawn = closestPawn;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
				//	printf("\nworked?");
			}
			else if (config_system.item.SilentAimbot && closestPawn && Util::SpoofCall(GetAsyncKeyState, VK_RBUTTON) < 0 && Util::SpoofCall(GetForegroundWindow) == hWnd) {
				Core::TargetPawn = closestPawn;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
			}
			else {
				Core::TargetPawn = nullptr;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
			}
		}

		if (config_system.item.AimKey = "0x51") {
			if (config_system.item.Aimbot && closestPawn && Util::SpoofCall(GetAsyncKeyState, VK_RBUTTON) < 0 && Util::SpoofCall(GetForegroundWindow) == hWnd) {
				Core::TargetPawn = closestPawn;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
				//	printf("\nworked?");
			}
			else if (config_system.item.SilentAimbot && closestPawn && Util::SpoofCall(GetAsyncKeyState, VK_RBUTTON) < 0 && Util::SpoofCall(GetForegroundWindow) == hWnd) {
				Core::TargetPawn = closestPawn;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
			}
			else {
				Core::TargetPawn = nullptr;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
			}
		}

		if (config_system.item.AimKey = "0x5A") {
			if (config_system.item.Aimbot && closestPawn && Util::SpoofCall(GetAsyncKeyState, VK_RBUTTON) < 0 && Util::SpoofCall(GetForegroundWindow) == hWnd) {
				Core::TargetPawn = closestPawn;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
				//	printf("\nworked?");
			}
			else if (config_system.item.SilentAimbot && closestPawn && Util::SpoofCall(GetAsyncKeyState, VK_RBUTTON) < 0 && Util::SpoofCall(GetForegroundWindow) == hWnd) {
				Core::TargetPawn = closestPawn;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
			}
			else {
				Core::TargetPawn = nullptr;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
			}
		}

		if (config_system.item.AimKey = "0x58") {
			if (config_system.item.Aimbot && closestPawn && Util::SpoofCall(GetAsyncKeyState, VK_RBUTTON) < 0 && Util::SpoofCall(GetForegroundWindow) == hWnd) {
				Core::TargetPawn = closestPawn;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
				//	printf("\nworked?");
			}
			else if (config_system.item.SilentAimbot && closestPawn && Util::SpoofCall(GetAsyncKeyState, VK_RBUTTON) < 0 && Util::SpoofCall(GetForegroundWindow) == hWnd) {
				Core::TargetPawn = closestPawn;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
			}
			else {
				Core::TargetPawn = nullptr;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
			}
		}

		if (config_system.item.AimKey = "ox43") {
			if (config_system.item.Aimbot && closestPawn && Util::SpoofCall(GetAsyncKeyState, VK_RBUTTON) < 0 && Util::SpoofCall(GetForegroundWindow) == hWnd) {
				Core::TargetPawn = closestPawn;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
				//	printf("\nworked?");
			}
			else if (config_system.item.SilentAimbot && closestPawn && Util::SpoofCall(GetAsyncKeyState, VK_RBUTTON) < 0 && Util::SpoofCall(GetForegroundWindow) == hWnd) {
				Core::TargetPawn = closestPawn;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
			}
			else {
				Core::TargetPawn = nullptr;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
			}
		}

		if (config_system.item.AimKey = "VK_OEM_3") {
			if (config_system.item.Aimbot && closestPawn && Util::SpoofCall(GetAsyncKeyState, VK_RBUTTON) < 0 && Util::SpoofCall(GetForegroundWindow) == hWnd) {
				Core::TargetPawn = closestPawn;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
				//	printf("\nworked?");
			}
			else if (config_system.item.SilentAimbot && closestPawn && Util::SpoofCall(GetAsyncKeyState, VK_RBUTTON) < 0 && Util::SpoofCall(GetForegroundWindow) == hWnd) {
				Core::TargetPawn = closestPawn;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
			}
			else {
				Core::TargetPawn = nullptr;
				Core::NoSpread = config_system.item.NoSpreadAimbot;
			}
		}



		if (config_system.item.Aimbot && closestPawn && Util::SpoofCall(GetAsyncKeyState, config_system.keybind.AimbotLock) < 0 && Util::SpoofCall(GetForegroundWindow) == hWnd) {
			Core::TargetPawn = closestPawn;
			Core::NoSpread = TRUE;
			if (config_system.item.Aimbot && config_system.item.AntiAim && Util::SpoofCall(GetAsyncKeyState, config_system.keybind.AntiAim)) {
				int rnd = rand();
				FRotator args = { 0 };
				args.Yaw = rnd;
				Core::ProcessEvent(Core::LocalPlayerController, Offsets::Engine::Controller::ClientSetRotation, &args, 0);
				//mouse_event(000001, rnd, NULL, NULL, NULL); old anti aim
			}
		}
		else {
			Core::TargetPawn = nullptr;
			Core::NoSpread = FALSE;
		}

		bool isSilent = config_system.item.SilentAimbot;
		bool isRage = config_system.item.AutoAimbot;
		if (config_system.item.SpinBot && Util::SpoofCall(GetAsyncKeyState, config_system.keybind.Spinbot) && Util::SpoofCall(GetForegroundWindow) == hWnd) {
			int rnd = rand();
			FRotator args = { 0 };
			args.Yaw = rnd;
			if (closestPawn) {
				Core::TargetPawn = closestPawn;
				Core::NoSpread = TRUE;
			}
			else {
				Core::ProcessEvent(Core::LocalPlayerController, Offsets::Engine::Controller::ClientSetRotation, &args, 0);
			}
			config_system.item.AutoAimbot = true;
			config_system.item.SilentAimbot = true;
		}
		else {
			if (!isSilent) {
				config_system.item.SilentAimbot = false;
			}
			if (!isRage) {
				config_system.item.AutoAimbot = false;
			}

			if (config_system.item.SilentAimbot) {
				isSilent = true;
			}
			if (config_system.item.AutoAimbot) {
				isRage = true;
			}
		}

		if (config_system.item.FlickAimbot && closestPawn && Util::SpoofCall(GetAsyncKeyState, config_system.keybind.AimbotShoot) < 0 && Util::SpoofCall(GetForegroundWindow) == hWnd) {
			Core::TargetPawn = closestPawn;
			Core::NoSpread = TRUE;
		}

		if (config_system.item.AutoAim && closestPawn && Util::SpoofCall(GetForegroundWindow) == hWnd) {
			//mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
			Core::TargetPawn = closestPawn;
			Core::NoSpread = TRUE;
		}

		if (config_system.item.SpamAutoAim && closestPawn && Util::SpoofCall(GetForegroundWindow) == hWnd) {
			mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
			Core::TargetPawn = closestPawn;
			Core::NoSpread = TRUE;
			mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
			Core::TargetPawn = nullptr;
			Core::NoSpread = FALSE;
		}

		if (config_system.item.DrawAimbotFOV) {
			window.DrawList->AddCircle(ImVec2(width / 2, height / 2), config_system.item.AimbotFOV, ImGui::GetColorU32({ config_system.item.FOVCircleColor[0], config_system.item.FOVCircleColor[1], config_system.item.FOVCircleColor[2], config_system.item.FOVCircleOpacity }), 128);
			if (config_system.item.DrawFilledAimbotFOV) {
				window.DrawList->AddCircleFilled(ImVec2(width / 2, height / 2), config_system.item.AimbotFOV, ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, config_system.item.FOVCircleFilledOpacity }), 128);
			}
		}

		if (&config_system.item.CrosshairSize) {
			//base crosshair
			window.DrawList->AddLine(ImVec2(width / 2 + config_system.item.CrosshairSize, height / 2), ImVec2(width / 2 - config_system.item.CrosshairSize, height / 2), ImGui::GetColorU32({ config_system.item.FOVCircleColor[0], config_system.item.FOVCircleColor[1], config_system.item.FOVCircleColor[2], 1.0f }), config_system.item.CrosshairThickness);
			window.DrawList->AddLine(ImVec2(width / 2, height / 2 + config_system.item.CrosshairSize), ImVec2(width / 2, height / 2 - config_system.item.CrosshairSize), ImGui::GetColorU32({ config_system.item.FOVCircleColor[0], config_system.item.FOVCircleColor[1], config_system.item.FOVCircleColor[2], 1.0f }), config_system.item.CrosshairThickness);

			//fancy crosshair
			window.DrawList->AddLine(ImVec2(width / 2 + config_system.item.CrosshairSize / 2, height / 2), ImVec2(width / 2 - config_system.item.CrosshairSize / 2, height / 2), ImGui::GetColorU32({ 1.0f, 0.0f, 0.0f, 1.0f }), config_system.item.CrosshairThickness);
			window.DrawList->AddLine(ImVec2(width / 2, height / 2 + config_system.item.CrosshairSize / 2), ImVec2(width / 2, height / 2 - config_system.item.CrosshairSize / 2), ImGui::GetColorU32({ 1.0f, 0.0f, 1.0f, 0.0f }), config_system.item.CrosshairThickness);
		}

		success = TRUE;
	} while (FALSE);

	if (!success) {
		Core::LocalPlayerController = Core::LocalPlayerPawn = Core::TargetPawn = nullptr;
	}
	EndScene(window);
	return PresentOriginal(swapChain, syncInterval, flags);
}



__declspec(dllexport) HRESULT ResizeHook(IDXGISwapChain* swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags) {
	ImGui_ImplDX11_Shutdown();
	renderTargetView->Release();
	immediateContext->Release();
	device->Release();
	device = nullptr;

	return ResizeOriginal(swapChain, bufferCount, width, height, newFormat, swapChainFlags);
}


bool Render::Initialize() {
	IDXGISwapChain* swapChain = nullptr;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	auto featureLevel = D3D_FEATURE_LEVEL_11_0;

	DXGI_SWAP_CHAIN_DESC sd = { 0 };
	sd.BufferCount = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.OutputWindow = FindWindow((L"UnrealWindow"), (L"Fortnite  "));
	sd.SampleDesc.Count = 1;
	sd.Windowed = TRUE;

	if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, 0, &featureLevel, 1, D3D11_SDK_VERSION, &sd, &swapChain, &device, nullptr, &context))) {
		MessageBox(0, L"Critical error have happened\nPlease contact an admin with the error code:\n0x0001b", L"Error", MB_ICONERROR);
		return FALSE;
	}

	auto table = *reinterpret_cast<PVOID**>(swapChain);
	auto present = table[8];
	auto resize = table[13];

	context->Release();
	device->Release();
	swapChain->Release();

	const auto pcall_present_discord = Helper::PatternScan(Discord::GetDiscordModuleBase(), "FF 15 ? ? ? ? 8B D8 E8 ? ? ? ? E8 ? ? ? ? EB 10");
	auto presentSceneAdress = Helper::PatternScan(Discord::GetDiscordModuleBase(),
		"48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B D9 41 8B F8");

	DISCORD.HookFunction(presentSceneAdress, (uintptr_t)PresentHook, (uintptr_t)&PresentOriginal);

	DISCORD.HookFunction(presentSceneAdress, (uintptr_t)ResizeHook, (uintptr_t)&PresentOriginal);
	return TRUE;
}

