#pragma once

typedef struct
{
	unsigned long eventCount;
	WORD    wButtons;
	BYTE    bLeftTrigger;
	BYTE    bRightTrigger;
	SHORT   sThumbLX;
	SHORT   sThumbLY;
	SHORT   sThumbRX;
	SHORT   sThumbRY;
} XINPUT_GAMEPAD_SECRET;

enum EmulationTargetType : int
{
	X360,
	DS4
};

//
// Helper object to keep track of state during every frame
// 
struct EmulationTarget
{
	//
	// Pad index
	// 
	int userIndex;
	
	//
	// Is x360ce reporting a device for the current user index
	// 
	bool isSourceConnected;

	//
	// Set to true if pad has been (dis-)connected
	// 
	bool hasPresenceChanged;

	//
	// Is the target device (virtual controller) currently alive
	// 
	bool isTargetConnected;

	//
	// Device object referring to the emulation target
	// 
	PVIGEM_TARGET target;

	//
	// Type of emulated device (from combo-list)
	// 
	// Currently either an X360 pad or a DS4
	// 
	EmulationTargetType targetType;
};

typedef DWORD(WINAPI* XInputSetState_t)(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration);

EVT_VIGEM_X360_NOTIFICATION notify_x360;
EVT_VIGEM_DS4_NOTIFICATION notify_ds4;

// https://github.com/ocornut/imgui/issues/707#issuecomment-468798935
inline void apply_imgui_style()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	/// 0 = FLAT APPEARENCE
	/// 1 = MORE "3D" LOOK
	int is3D = 0;

	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_Border] = ImVec4(0.12f, 0.12f, 0.12f, 0.71f);
	colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.42f, 0.42f, 0.42f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.42f, 0.42f, 0.42f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.17f, 0.17f, 0.17f, 0.90f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.335f, 0.335f, 0.335f, 1.000f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.24f, 0.24f, 0.24f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.54f, 0.54f, 0.54f, 0.35f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.52f, 0.52f, 0.52f, 0.59f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.76f, 0.76f, 0.76f, 0.77f);
	colors[ImGuiCol_Separator] = ImVec4(0.000f, 0.000f, 0.000f, 0.137f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.700f, 0.671f, 0.600f, 0.290f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.702f, 0.671f, 0.600f, 0.674f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.73f, 0.73f, 0.73f, 0.35f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);

	style.PopupRounding = 3;

	style.WindowPadding = ImVec2(4, 4);
	style.FramePadding = ImVec2(6, 4);
	style.ItemSpacing = ImVec2(6, 2);

	style.ScrollbarSize = 18;

	style.WindowBorderSize = 1;
	style.ChildBorderSize = 1;
	style.PopupBorderSize = 1;
	style.FrameBorderSize = (float)is3D;

	style.WindowRounding = 3;
	style.ChildRounding = 3;
	style.FrameRounding = 3;
	style.ScrollbarRounding = 2;
	style.GrabRounding = 3;

#ifdef IMGUI_HAS_DOCK 
	style.TabBorderSize = is3D;
	style.TabRounding = 3;

	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
	colors[ImGuiCol_DockingPreview] = ImVec4(0.85f, 0.85f, 0.85f, 0.28f);

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}
#endif
}

//
// Sets the current state of the D-PAD on a DualShock 4 report.
// 
VOID FORCEINLINE DS4_EX_SET_DPAD(
	_Out_ PDS4_REPORT_EX Report,
	_In_ DS4_DPAD_DIRECTIONS Dpad
)
{
	Report->Report.wButtons &= ~0xF;
	Report->Report.wButtons |= (USHORT)Dpad;
}

VOID FORCEINLINE DS4_REPORT_EX_INIT(
	_Out_ PDS4_REPORT_EX Report
)
{
	RtlZeroMemory(Report, sizeof(DS4_REPORT_EX));
	Report->Report.bThumbLX = 0x80;
	Report->Report.bThumbLY = 0x80;
	Report->Report.bThumbRX = 0x80;
	Report->Report.bThumbRY = 0x80;
	Report->Report.wTimestamp = 0x63FD;
	Report->Report.bBatteryLvl = 0x06;
	Report->Report.bBatteryLvlSpecial = 0x1B;
	Report->Report.sCurrentTouch.bIsUpTrackingNum1 = 0x80;
	Report->Report.sCurrentTouch.bIsUpTrackingNum2 = 0x80;
	Report->Report.sPreviousTouch[0].bIsUpTrackingNum1 = 0x80;
	Report->Report.sPreviousTouch[0].bIsUpTrackingNum2 = 0x80;
	Report->Report.sPreviousTouch[1].bIsUpTrackingNum1 = 0x80;
	Report->Report.sPreviousTouch[1].bIsUpTrackingNum2 = 0x80;
	Report->ReportBuffer[61] = 0x80;
	DS4_EX_SET_DPAD(Report, DS4_BUTTON_DPAD_NONE);
}

VOID FORCEINLINE XUSB_TO_DS4_REPORT_EX(
	_Out_ PXUSB_REPORT Input,
	_Out_ PDS4_REPORT_EX Output
)
{
	if (Input->wButtons & XUSB_GAMEPAD_BACK) {
		Output->Report.bSpecial |= DS4_SPECIAL_BUTTON_TOUCHPAD;
		Output->Report.bTouchPacketsN = 1;
		Output->Report.sCurrentTouch.bIsUpTrackingNum1 = 0x1;
	}
	if (Input->wButtons & XUSB_GAMEPAD_START) Output->Report.wButtons |= DS4_BUTTON_OPTIONS;
	if (Input->wButtons & XUSB_GAMEPAD_LEFT_THUMB) Output->Report.wButtons |= DS4_BUTTON_THUMB_LEFT;
	if (Input->wButtons & XUSB_GAMEPAD_RIGHT_THUMB) Output->Report.wButtons |= DS4_BUTTON_THUMB_RIGHT;
	if (Input->wButtons & XUSB_GAMEPAD_LEFT_SHOULDER) Output->Report.wButtons |= DS4_BUTTON_SHOULDER_LEFT;
	if (Input->wButtons & XUSB_GAMEPAD_RIGHT_SHOULDER) Output->Report.wButtons |= DS4_BUTTON_SHOULDER_RIGHT;
	if (Input->wButtons & XUSB_GAMEPAD_GUIDE) Output->Report.bSpecial |= DS4_SPECIAL_BUTTON_PS;
	if (Input->wButtons & XUSB_GAMEPAD_A) Output->Report.wButtons |= DS4_BUTTON_CROSS;
	if (Input->wButtons & XUSB_GAMEPAD_B) Output->Report.wButtons |= DS4_BUTTON_CIRCLE;
	if (Input->wButtons & XUSB_GAMEPAD_X) Output->Report.wButtons |= DS4_BUTTON_SQUARE;
	if (Input->wButtons & XUSB_GAMEPAD_Y) Output->Report.wButtons |= DS4_BUTTON_TRIANGLE;

	Output->Report.bTriggerL = Input->bLeftTrigger;
	Output->Report.bTriggerR = Input->bRightTrigger;

	if (Input->bLeftTrigger > 0)Output->Report.wButtons |= DS4_BUTTON_TRIGGER_LEFT;
	if (Input->bRightTrigger > 0)Output->Report.wButtons |= DS4_BUTTON_TRIGGER_RIGHT;

	if (Input->wButtons & XUSB_GAMEPAD_DPAD_UP) DS4_EX_SET_DPAD(Output, DS4_BUTTON_DPAD_NORTH);
	if (Input->wButtons & XUSB_GAMEPAD_DPAD_RIGHT) DS4_EX_SET_DPAD(Output, DS4_BUTTON_DPAD_EAST);
	if (Input->wButtons & XUSB_GAMEPAD_DPAD_DOWN) DS4_EX_SET_DPAD(Output, DS4_BUTTON_DPAD_SOUTH);
	if (Input->wButtons & XUSB_GAMEPAD_DPAD_LEFT) DS4_EX_SET_DPAD(Output, DS4_BUTTON_DPAD_WEST);

	if (Input->wButtons & XUSB_GAMEPAD_DPAD_UP
		&& Input->wButtons & XUSB_GAMEPAD_DPAD_RIGHT) DS4_EX_SET_DPAD(Output, DS4_BUTTON_DPAD_NORTHEAST);
	if (Input->wButtons & XUSB_GAMEPAD_DPAD_RIGHT
		&& Input->wButtons & XUSB_GAMEPAD_DPAD_DOWN) DS4_EX_SET_DPAD(Output, DS4_BUTTON_DPAD_SOUTHEAST);
	if (Input->wButtons & XUSB_GAMEPAD_DPAD_DOWN
		&& Input->wButtons & XUSB_GAMEPAD_DPAD_LEFT) DS4_EX_SET_DPAD(Output, DS4_BUTTON_DPAD_SOUTHWEST);
	if (Input->wButtons & XUSB_GAMEPAD_DPAD_LEFT
		&& Input->wButtons & XUSB_GAMEPAD_DPAD_UP) DS4_EX_SET_DPAD(Output, DS4_BUTTON_DPAD_NORTHWEST);

	Output->Report.bThumbLX = ((Input->sThumbLX + ((USHRT_MAX / 2) + 1)) / 257);
	Output->Report.bThumbLY = (-(Input->sThumbLY + ((USHRT_MAX / 2) - 1)) / 257);
	Output->Report.bThumbLY = (Output->Report.bThumbLY == 0) ? 0xFF : Output->Report.bThumbLY;
	Output->Report.bThumbRX = ((Input->sThumbRX + ((USHRT_MAX / 2) + 1)) / 257);
	Output->Report.bThumbRY = (-(Input->sThumbRY + ((USHRT_MAX / 2) + 1)) / 257);
	Output->Report.bThumbRY = (Output->Report.bThumbRY == 0) ? 0xFF : Output->Report.bThumbRY;
}
