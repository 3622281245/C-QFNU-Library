#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
 int ctrlPressed = 0;  // ��־λ����ʾCtrl����״̬
#include "book.h"
#include "user.h"
#include "tool.h"


// ����Shift�����̺߳���
DWORD WINAPI ListenCtrlKey(LPVOID lpParam) {
	while (1) {
		// ���Shift����״̬
		if (GetAsyncKeyState(VK_LSHIFT) & 0x8000) {  // 0x8000��ʾ��������
			if (!ctrlPressed) {
				ctrlPressed = 1;
				//printf("Shift������\n");
			}
		} else {
			if (ctrlPressed) {
				ctrlPressed = 0;
				//printf("Shift��̧��\n");
			}
		}
		Sleep(10);  // ��ʱ������ռ�ù����CPU��Դ
	}
	return 0;
}

// ��鵱ǰ�����Ƿ��Թ���ԱȨ������
BOOL IsRunningAsAdmin() {
	BOOL isAdmin = FALSE;
	PSID adminGroup = NULL;

	// ��������Ա��� SID
	SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
	if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
	                             0, 0, 0, 0, 0, 0, &adminGroup)) {
		// ��鵱ǰ�����Ƿ����ڹ���Ա��
		CheckTokenMembership(NULL, adminGroup, &isAdmin);
		FreeSid(adminGroup);
	}

	return isAdmin;
}

// �Թ���ԱȨ��������������
void RelaunchAsAdmin() {
	wchar_t szPath[MAX_PATH];
	if (GetModuleFileNameW(NULL, szPath, MAX_PATH)) {
		// ���� SHELLEXECUTEINFO �ṹ��
		SHELLEXECUTEINFOW sei = { sizeof(SHELLEXECUTEINFOW) };
		sei.lpVerb = L"runas"; // ʹ�� "runas" �Թ���Ա�������
		sei.lpFile = szPath;
		sei.hwnd = NULL;
		sei.nShow = SW_NORMAL;
		if (!ShellExecuteExW(&sei)) {
			printf("�޷��Թ���Ա���������������\n");
		}
	}
}

int main() {
	//����Ƿ��й���ԱȨ��
	if (!IsRunningAsAdmin()) {
		printf("��ǰû�й���ԱȨ�ޣ����������Թ���ԱȨ����������...\n");
		RelaunchAsAdmin();
		return 0; // �˳���ǰʵ��
	}
	// �����̼߳���Shift��
	HANDLE hThread = CreateThread(NULL, 0, ListenCtrlKey, NULL, 0, NULL);
	if (hThread == NULL) {
		printf("�����߳�ʧ��\n");
		return 1;
	}
	SetConsoleFontSize(24, 24);//���ÿ���̨�����С
	SetConsoleColor(RGB(255, 87, 51), RGB(104, 44, 8));//���ÿ���̨������ɫ��У��RGB��
	SetTextColor1(14);
	system("title QFNU Library");//����
	disableQuickEditMode();//���õ���ѡ��
	disableResize();//���ô��ڴ�С����
	disableCursor();//����������
	disableScrollbars();//���ù�����

	const char *menuOptions[] = {
		"�û���¼",
		"ע���˺�",
		"����Ա��¼",
		"�˳�ϵͳ"
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
				printCentered("��лʹ��ͼ�����ϵͳ���ټ���", 0);
				Sleep(2000);
				enableCursor();  // �ָ����
				return 0;
		}
	}
	// ������Դ
	CloseHandle(hThread);
}
