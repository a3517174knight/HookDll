// HookDll.cpp: 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "HookDll.h"

using namespace std;

HMODULE hDLL;

HHOOK g_hlowKeyHook = NULL;     //低级键盘钩子句柄  
//HHOOK g_hMouse = NULL;          //鼠标钩子句柄  
//HHOOK g_hMessage = NULL;          //消息钩子句柄

vector<vector<int>> vk_keys;
vector<function<void()>> functions;

void UnsetHook() {
	if (g_hlowKeyHook) {
		UnhookWindowsHookEx(g_hlowKeyHook);    //卸载低级键盘钩子  
		g_hlowKeyHook = NULL;
	}
	//if (g_hMouse) {
	//	UnhookWindowsHookEx(g_hMouse);         //卸载低级鼠标钩子 
	//	g_hMouse = NULL;
	//	//UnhookWindowsHookEx(g_hMessage);    //卸载消息钩子  
	//	//g_hMessage = NULL;
	//}
	
}

bool CallFunction(PKBDLLHOOKSTRUCT LowKey, WPARAM wParam) {
	if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
		return false;
	bool isCallFunction = false;
	for (int i = 0; i < vk_keys.size(); i++) {
		vector<int> vk_key = vk_keys.at(i);
		for (int j = 0; j < vk_key.size(); j++) {
			if (vk_key.at(j) == VK_CONTROL) {
				//isCallFunction = ((GetKeyState(VK_CONTROL) & 0x8000) != 0);
				isCallFunction = ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0);
			}
			//else if (vk_key.at(j) == LLKHF_ALTDOWN) {
			else if (vk_key.at(j) == VK_MENU) {	// 判断alt键是否按下
				//isCallFunction = ((LowKey->flags & LLKHF_ALTDOWN) != 0);
				isCallFunction = ((GetAsyncKeyState(VK_MENU) & 0x8000) != 0);
			}
			else {
				isCallFunction = (LowKey->vkCode == vk_key.at(j));
			}
			if (!isCallFunction)
				break;
		}
		if (isCallFunction) {
			functions.at(i)();
			return true;
		}
	}
	return false;
}

//LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
//{
//	if ((nCode == WM_POWERBROADCAST) && (wParam == PBT_APMQUERYSUSPEND))
//	{
//		if(timerTimeout != NULL)
//			timerTimeout();
//	}
//	return CallNextHookEx(g_hMessage, nCode, wParam, lParam);
//}

//LRESULT CALLBACK ScreenUnlockHook(int nCode, WPARAM wParam, LPARAM lParam)
//{
//	bool isClick = false;
//	PKBDLLHOOKSTRUCT LowKey = NULL;   //该结构体包含底层键盘输入事件的信息  
//
//	if (nCode == HC_ACTION)      //HC_ACTION  表示有消息  
//	{
//		LowKey = (PKBDLLHOOKSTRUCT)lParam;
//		if (LowKey->vkCode == VK_F12 && ((GetKeyState(VK_CONTROL) & 0x8000) != 0)) {
//			if (getFocus != NULL)
//				getFocus();
//		}
//		else {
//			isClick = (LowKey->vkCode == VK_LWIN) || (LowKey->vkCode == VK_RWIN) ||
//				// 屏蔽Alt
//				((LowKey->flags & LLKHF_ALTDOWN) != 0) ||
//				//屏蔽Alt+F4  
//				((LowKey->vkCode == VK_F4) && ((LowKey->flags & LLKHF_ALTDOWN) != 0)) ||
//				// 屏蔽Alt+Tab  
//				((LowKey->vkCode == VK_TAB) && ((LowKey->flags & LLKHF_ALTDOWN) != 0)) ||
//				// 屏蔽Alt+Esc  
//				((LowKey->vkCode == VK_ESCAPE) && ((LowKey->flags & LLKHF_ALTDOWN) != 0)) ||
//				// 屏蔽Ctrl+Esc  
//				((LowKey->vkCode == VK_ESCAPE) && ((GetKeyState(VK_CONTROL) & 0x8000) != 0));
//			switch (wParam)
//			{
//			case WM_KEYDOWN:
//			{
//				//isClick = 1;
//				if (LowKey->vkCode == VK_F8) // 后门  
//					UnSetHook();
//				break;
//			}
//			//case WM_SYSKEYDOWN:
//			//case WM_KEYUP:
//			//case WM_SYSKEYUP:
//			//{              // 屏蔽Win  
//			//	isClick = (LowKey->vkCode == VK_LWIN) || (LowKey->vkCode == VK_RWIN) ||
//			//		//屏蔽Alt+F4  
//			//		((LowKey->vkCode == VK_F4) && ((LowKey->flags & LLKHF_ALTDOWN) != 0)) ||
//			//		// 屏蔽Alt+Tab  
//			//		((LowKey->vkCode == VK_TAB) && ((LowKey->flags & LLKHF_ALTDOWN) != 0)) ||
//			//		// 屏蔽Alt+Esc  
//			//		((LowKey->vkCode == VK_ESCAPE) && ((LowKey->flags & LLKHF_ALTDOWN) != 0)) ||
//			//		// 屏蔽Ctrl+Esc  
//			//		((LowKey->vkCode == VK_ESCAPE) && ((GetKeyState(VK_CONTROL) & 0x8000) != 0));
//			//	break;
//			//}
//			default:
//				break;
//			}
//		}
//	}
//	if (isClick)
//		return 1;
//	return CallNextHookEx(g_hlowKeyHook, nCode, wParam, lParam);  //传给下一个钩子  
//}

LRESULT CALLBACK KeyBlockHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	bool isClick = false;
	PKBDLLHOOKSTRUCT LowKey = NULL;   //该结构体包含底层键盘输入事件的信息  

	if (nCode == HC_ACTION)      //HC_ACTION  表示有消息  
	{
		LowKey = (PKBDLLHOOKSTRUCT)lParam;
		if(!CallFunction(LowKey, wParam)) {
			isClick = (LowKey->vkCode == VK_LWIN) || (LowKey->vkCode == VK_RWIN) ||
				// 屏蔽Alt
				((LowKey->flags & LLKHF_ALTDOWN) != 0) ||
				//屏蔽Alt+F4  
				((LowKey->vkCode == VK_F4) && ((LowKey->flags & LLKHF_ALTDOWN) != 0)) ||
				// 屏蔽Alt+Tab  
				((LowKey->vkCode == VK_TAB) && ((LowKey->flags & LLKHF_ALTDOWN) != 0)) ||
				// 屏蔽Alt+Esc  
				((LowKey->vkCode == VK_ESCAPE) && ((LowKey->flags & LLKHF_ALTDOWN) != 0)) ||
				// 屏蔽Ctrl+Esc  
				((LowKey->vkCode == VK_ESCAPE) && ((GetKeyState(VK_CONTROL) & 0x8000) != 0));
			switch (wParam)
			{
			case WM_SYSKEYDOWN:
			case WM_KEYDOWN:
			{
				//if (LowKey->vkCode == VK_F8 && ((LowKey->flags & LLKHF_ALTDOWN) != 0) && ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0)) // 后门  
				// ctrl+alt+F8, VK_MENU是alt的虚拟键码,即可用LowKey->vkCode == VK_MENU表示alt键按下
				if (LowKey->vkCode == VK_F8 && ((GetAsyncKeyState(VK_MENU) & 0x8000) != 0) && ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0)) // 后门  
					UnsetHook();
				break;
			}
			default:
				break;
			}
		}
	}
	if (isClick)
		return 1;
	return CallNextHookEx(g_hlowKeyHook, nCode, wParam, lParam);  //传给下一个钩子  
}

LRESULT CALLBACK KeyNotBlockHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	PKBDLLHOOKSTRUCT LowKey = NULL;   //该结构体包含底层键盘输入事件的信息  
	if (nCode == HC_ACTION) {      //HC_ACTION  表示有消息  
		LowKey = (PKBDLLHOOKSTRUCT)lParam;
		CallFunction(LowKey, wParam);
	}
	return CallNextHookEx(g_hlowKeyHook, nCode, wParam, lParam);  //传给下一个钩子  
}

//屏蔽所有鼠标消息  
//LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
//{
//	if (nCode == HC_ACTION) {     //HC_ACTION  表示有消息  
//		if (resetTimer != NULL)
//			resetTimer();
//	}
//	return CallNextHookEx(g_hlowKeyHook, nCode, wParam, lParam);  //传给下一个钩子  
//}

//void SetHook(const std::function<void()> &callback1, const std::function<void()> &callback2)
//{
//	//timerTimeout = callback1;
//	resetTimer = callback2;
//	//安装低级键盘钩子  
//	if (resetTimer == NULL)
//		g_hlowKeyHook = SetWindowsHookEx(WH_KEYBOARD_LL, ScreenUnlockHook, hDLL, 0);
//	else {
//		g_hlowKeyHook = SetWindowsHookEx(WH_KEYBOARD_LL, ScreenLockHook, hDLL, 0);
//		//安装鼠标钩子  
//		g_hMouse = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, hDLL, 0);
//		//g_hMessage = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, hDLL, 0);
//	}
//}

void SetHook(bool isLocked)
{
	UnsetHook();
	//安装低级键盘钩子  
	if (isLocked)
		g_hlowKeyHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyBlockHook, hDLL, 0);
	else {
		g_hlowKeyHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyNotBlockHook, hDLL, 0);
		//安装鼠标钩子  
		//g_hMouse = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, hDLL, 0);
		//g_hMessage = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, hDLL, 0);
	}
}

void SetFunction(const std::vector<int> &vk_key, const std::function<void()> &callback) {
	vk_keys.push_back(vk_key);
	functions.push_back(callback);
}

void SetFunctions(const std::vector<std::vector<int>> &vk_key, const std::vector<std::function<void()>> &callback) {
	vk_keys = vk_key;
	functions = callback;
}