/*
	filename��tool.c tool.h
	����ߣ���һ��
*/


#include <windows.h>
#include <stdio.h>
#include <conio.h>
void clearLine() {
	// ��ȡ��ǰ���λ��
	COORD coord;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	coord = csbi.dwCursorPosition;

	// �ƶ���굽��ǰ�еĿ�ͷ
	SetConsoleCursorPosition(hConsole, coord);

	// ��յ�ǰ��
	printf("\r");  // �س�������ƶ�������
	for (int i = 0; i < 80; i++) {  // �������̨���Ϊ 80 ���ַ�
		printf(" ");  // ��ӡ�ո񸲸�֮ǰ������
	}
	printf("\r");  // �ٴλص�����
}
// ���ÿ��ٱ༭ģʽ�ĺ���
int disableQuickEditMode() {
	// ��ȡ����̨������
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	if (hInput == INVALID_HANDLE_VALUE) {
		return 0;  // �޷���ȡ����̨���
	}

	// ��ȡ��ǰ����̨ģʽ
	DWORD mode;
	if (!GetConsoleMode(hInput, &mode)) {
		return 0;  // �޷���ȡ��ǰ����̨ģʽ
	}

	// �޸Ŀ���̨ģʽ�����ÿ��ٱ༭ģʽ
	mode &= ~ENABLE_QUICK_EDIT_MODE;  // ���ÿ��ٱ༭ģʽ
	mode |= ENABLE_MOUSE_INPUT;      // ����������루��ѡ����������

	if (!SetConsoleMode(hInput, mode)) {
		return 0;  // �޷����ÿ���̨ģʽ
	}

	return 1;  // �ɹ����ÿ��ٱ༭ģʽ
}

void disableResize() {
	HWND hConsole = GetConsoleWindow(); // ��ȡ����̨���ھ��
	if (hConsole == NULL) {
		fprintf(stderr, "Failed to get console window handle\n");
		return;
	}

// ��ȡ��ǰ������ʽ
	LONG style = GetWindowLong(hConsole, GWL_STYLE);

// ȥ�����ڴ�С�������� (WS_SIZEBOX �� WS_MAXIMIZEBOX)
	style &= ~(WS_SIZEBOX | WS_MAXIMIZEBOX);

// �����µ���ʽ
	SetWindowLong(hConsole, GWL_STYLE, style);

// ǿ��ˢ�´��ڣ���ʹ��ʽ������Ч
	SetWindowPos(hConsole, NULL, 0, 0, 0, 0,
	             SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}


// ���ù�����
void disableScrollbars() {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	// ��ȡ��ǰ����̨��Ϣ
	GetConsoleScreenBufferInfo(hConsole, &csbi);

	// ������Ļ��������С�봰�ڴ�Сһ��
	COORD bufferSize;
	bufferSize.X = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	bufferSize.Y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

	SetConsoleScreenBufferSize(hConsole, bufferSize);
}

// ���ù��λ��
void setCursorPosition(int x, int y) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position = {x, y};
	SetConsoleCursorPosition(hConsole, position);
}

// ��ȡ����̨��С
void getConsoleSize(int *width, int *height) {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	*width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	*height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

// ���ù��
void disableCursor() {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(hConsole, &cursorInfo);
	cursorInfo.bVisible = FALSE;  // ���ù�겻�ɼ�
	SetConsoleCursorInfo(hConsole, &cursorInfo);
}

// ���ù��
void enableCursor() {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(hConsole, &cursorInfo);
	cursorInfo.bVisible = TRUE;  // ���ù��ɼ�
	SetConsoleCursorInfo(hConsole, &cursorInfo);
}

// �����������
void printCentered(const char *message, int yOffset) {
	int consoleWidth, consoleHeight;
	getConsoleSize(&consoleWidth, &consoleHeight);
	int x = (consoleWidth - strlen(message)) / 2;
	int y = (consoleHeight / 2) + yOffset;
	setCursorPosition(x, y);
	printf("%s", message);
}
void getInputCentered_s(const char *prompt, char *input, int maxLength, int yOffset) {
	printCentered(prompt, yOffset);
	int consoleWidth, consoleHeight;
	getConsoleSize(&consoleWidth, &consoleHeight);

	int x = (consoleWidth - maxLength) / 2;
	setCursorPosition(-1, (consoleHeight / 2) + yOffset + 0); // Ҫʹ���������ʾ����ʾ�·���+1

	fgets(input, maxLength, stdin);
	input[strcspn(input, "\n")] = 0;  // ȥ�����з�
}
// ��ȡ�û�������ַ�����������ʾ
void getInputCentered(const char *prompt, char *input, int maxLength, int yOffset, int inputLimit) {
	printCentered(prompt, yOffset); // ��ӡ������ʾ
	int consoleWidth, consoleHeight;
	getConsoleSize(&consoleWidth, &consoleHeight); // ��ȡ����̨��С

	int x = (consoleWidth - maxLength) / 2;
	setCursorPosition(-1, (consoleHeight / 2) + yOffset + 1); // ���������λ��
	enableCursor();
// ��ʼ��ȡ����
	int count = 0;
	char ch;

	while (1) {
		ch = _getch(); // ��ȡһ���ַ��������Զ���ʾ�ڿ���̨��

		if (ch == '\r') { // ��⵽�س�����������
			break;
		} else if (ch == '\b') { // �����˸��
			if (count > 0) {
				printf("\b \b"); // ɾ�����һ���ַ�
				count--;
			}
		} else if (ch >= 32 && ch <= 126) { // ����ʾ�ַ�
			if (count < inputLimit) {
				putchar(ch); // �ֶ���ʾ�ַ�
				input[count++] = ch;
			}
		}
	}
	disableCursor();//����������
	input[count] = '\0'; // ����ַ���������
	printf("\n");
}
void getPasswordCentered(const char *prompt, char *input, int maxLength, int yOffset) {
	printCentered(prompt, yOffset); // ������ʾ��ʾ
	int consoleWidth, consoleHeight;
	getConsoleSize(&consoleWidth, &consoleHeight);

	int x = (consoleWidth - maxLength) / 2;
	setCursorPosition(-1, (consoleHeight / 2) + yOffset + 0); // �������ʾ����ʾ�·�
	enableCursor();
	int index = 0;
	char ch;

	while (index < maxLength - 1) { // ����һ���ռ���ַ�����ֹ��
		ch = _getch(); // ��ȡ����
		if (ch == '\r') { // �س�����������
			break;
		} else if (ch == '\b') { // �˸������
			if (index > 0) {
				index--;
				printf("\b \b"); // ɾ��һ���ַ�
			}
		} else if (ch >= 32 && ch <= 126) { // �ɴ�ӡ�ַ�
			input[index++] = ch;
			printf("*"); // ��ʾΪ *
		}
	}
	disableCursor();//����������
	input[index] = '\0'; // ����ַ�����ֹ��
}

void SetConsoleColor(COLORREF textColor, COLORREF backgroundColor) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	// ��ȡ��ǰ����̨��Ļ��������Ϣ
	CONSOLE_SCREEN_BUFFER_INFOEX csbi = { sizeof(csbi) };
	GetConsoleScreenBufferInfoEx(hConsole, &csbi);

	// ����ǰ���ͱ�����ɫ
	csbi.ColorTable[0] = backgroundColor;
	csbi.ColorTable[1] = textColor;

	// Ӧ����ɫ��
	SetConsoleScreenBufferInfoEx(hConsole, &csbi);

	// �����������ԣ�ǰ��ɫ1������ɫ0��
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
}


void SetConsoleFontSize(int fontSizeX, int fontSizeY) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);

	// ��ȡ��ǰ����̨��������Ϣ
	GetCurrentConsoleFontEx(hConsole, FALSE, &cfi);

	// �����µ������С
	cfi.dwFontSize.X = fontSizeX; // ������
	cfi.dwFontSize.Y = fontSizeY; // ����߶�

	// Ӧ���µ���������
	SetCurrentConsoleFontEx(hConsole, FALSE, &cfi);
}
// ����������ɫ������������ɫ��
void SetTextColor1(int textColor) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	// ��ȡ��ǰ����̨����
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);

	// ����������ɫ����4λ�������޸�������ɫ����4λ��
	WORD attributes = consoleInfo.wAttributes;
	attributes &= 0xF0;            // �����4λ��������ɫ��
	attributes |= textColor;      // �����µ�������ɫ

	SetConsoleTextAttribute(hConsole, attributes);
}
