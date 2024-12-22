#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
 int ctrlPressed = 0;  // 标志位，表示Shift键的状态，原使用Ctrl会与窗口缩放冲突
#include "book.h"
#include "user.h"
#include "tool.h"


// 监听Shift键的线程函数
DWORD WINAPI ListenCtrlKey(LPVOID lpParam) {
	while (1) {
		// 检查Shift键的状态
		if (GetAsyncKeyState(VK_LSHIFT) & 0x8000) {  // 0x8000表示键被按下
			if (!ctrlPressed) {
				ctrlPressed = 1;
				//printf("Shift键按下\n");
			}
		} else {
			if (ctrlPressed) {
				ctrlPressed = 0;
				//printf("Shift键抬起\n");
			}
		}
		Sleep(10);  // 延时，避免占用过多的CPU资源
	}
	return 0;
}

// 检查当前程序是否以管理员权限运行
BOOL IsRunningAsAdmin() {
	BOOL isAdmin = FALSE;
	PSID adminGroup = NULL;

	// 创建管理员组的 SID
	SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
	if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
	                             0, 0, 0, 0, 0, 0, &adminGroup)) {
		// 检查当前令牌是否属于管理员组
		CheckTokenMembership(NULL, adminGroup, &isAdmin);
		FreeSid(adminGroup);
	}

	return isAdmin;
}

// 以管理员权限重新启动程序
void RelaunchAsAdmin() {
	wchar_t szPath[MAX_PATH];
	if (GetModuleFileNameW(NULL, szPath, MAX_PATH)) {
		// 设置 SHELLEXECUTEINFO 结构体
		SHELLEXECUTEINFOW sei = { sizeof(SHELLEXECUTEINFOW) };
		sei.lpVerb = L"runas"; // 使用 "runas" 以管理员身份运行
		sei.lpFile = szPath;
		sei.hwnd = NULL;
		sei.nShow = SW_NORMAL;
		if (!ShellExecuteExW(&sei)) {
			printf("无法以管理员身份重新启动程序\n");
		}
	}
}

int main() {
	//检查是否有管理员权限
	if (!IsRunningAsAdmin()) {
		printf("当前没有管理员权限，尝试重新以管理员权限启动程序...\n");
		RelaunchAsAdmin();
		return 0; // 退出当前实例
	}
	// 创建线程监听Shift键
	HANDLE hThread = CreateThread(NULL, 0, ListenCtrlKey, NULL, 0, NULL);
	if (hThread == NULL) {
		printf("创建线程失败\n");
		return 1;
	}
	SetConsoleFontSize(24, 24);//设置控制台字体大小
	SetConsoleColor(RGB(255, 87, 51), RGB(104, 44, 8));//设置控制台背景颜色（校徽RGB）
	SetTextColor1(14);
	system("title QFNU Library");//标题
	disableQuickEditMode();//禁用单击选择
	disableResize();//禁用窗口大小调整
	disableCursor();//禁用输入光标
	disableScrollbars();//禁用滚动条

	const char *menuOptions[] = {
		"用户登录",
		"注册账号",
		"管理员登录",
		"退出系统"
	};
	int optionCount = sizeof(menuOptions) / sizeof(menuOptions[0]);

	while (1) {
		int choice = menuControl(menuOptions, optionCount);
		system("cls");
		switch (choice) {
			case 0:
				loginSystem();
				break;
			case 1:
				registerSystem();
				break;
			case 2:
				adminLoginSystem();
				break;
			case 3:
				system("cls");
				printCentered("感谢使用图书管理系统，再见！", 0);
				Sleep(2000);
				enableCursor();  // 恢复光标
				return 0;
		}
	}
	// 清理资源
	CloseHandle(hThread);
}
