/*
	filename：tool.c tool.h
	完成者：左一铭
*/


#include <windows.h>
#include <stdio.h>
#include <conio.h>
void clearLine() {
	// 获取当前光标位置
	COORD coord;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	coord = csbi.dwCursorPosition;

	// 移动光标到当前行的开头
	SetConsoleCursorPosition(hConsole, coord);

	// 清空当前行
	printf("\r");  // 回车，光标移动到行首
	for (int i = 0; i < 80; i++) {  // 假设控制台宽度为 80 个字符
		printf(" ");  // 打印空格覆盖之前的内容
	}
	printf("\r");  // 再次回到行首
}
// 禁用快速编辑模式的函数
int disableQuickEditMode() {
	// 获取控制台输入句柄
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	if (hInput == INVALID_HANDLE_VALUE) {
		return 0;  // 无法获取控制台句柄
	}

	// 获取当前控制台模式
	DWORD mode;
	if (!GetConsoleMode(hInput, &mode)) {
		return 0;  // 无法获取当前控制台模式
	}

	// 修改控制台模式，禁用快速编辑模式
	mode &= ~ENABLE_QUICK_EDIT_MODE;  // 禁用快速编辑模式
	mode |= ENABLE_MOUSE_INPUT;      // 启用鼠标输入（可选，根据需求）

	if (!SetConsoleMode(hInput, mode)) {
		return 0;  // 无法设置控制台模式
	}

	return 1;  // 成功禁用快速编辑模式
}

void disableResize() {
	HWND hConsole = GetConsoleWindow(); // 获取控制台窗口句柄
	if (hConsole == NULL) {
		fprintf(stderr, "Failed to get console window handle\n");
		return;
	}

// 获取当前窗口样式
	LONG style = GetWindowLong(hConsole, GWL_STYLE);

// 去掉窗口大小调整功能 (WS_SIZEBOX 和 WS_MAXIMIZEBOX)
	style &= ~(WS_SIZEBOX | WS_MAXIMIZEBOX);

// 设置新的样式
	SetWindowLong(hConsole, GWL_STYLE, style);

// 强制刷新窗口，以使样式更改生效
	SetWindowPos(hConsole, NULL, 0, 0, 0, 0,
	             SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}


// 禁用滚动条
void disableScrollbars() {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	// 获取当前控制台信息
	GetConsoleScreenBufferInfo(hConsole, &csbi);

	// 设置屏幕缓冲区大小与窗口大小一致
	COORD bufferSize;
	bufferSize.X = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	bufferSize.Y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

	SetConsoleScreenBufferSize(hConsole, bufferSize);
}

// 设置光标位置
void setCursorPosition(int x, int y) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position = {x, y};
	SetConsoleCursorPosition(hConsole, position);
}

// 获取控制台大小
void getConsoleSize(int *width, int *height) {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	*width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	*height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

// 禁用光标
void disableCursor() {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(hConsole, &cursorInfo);
	cursorInfo.bVisible = FALSE;  // 设置光标不可见
	SetConsoleCursorInfo(hConsole, &cursorInfo);
}

// 启用光标
void enableCursor() {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(hConsole, &cursorInfo);
	cursorInfo.bVisible = TRUE;  // 设置光标可见
	SetConsoleCursorInfo(hConsole, &cursorInfo);
}

// 居中输出文字
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
	setCursorPosition(-1, (consoleHeight / 2) + yOffset + 0); // 要使得输入框显示在提示下方就+1

	fgets(input, maxLength, stdin);
	input[strcspn(input, "\n")] = 0;  // 去除换行符
}
// 获取用户输入的字符串并居中显示
void getInputCentered(const char *prompt, char *input, int maxLength, int yOffset, int inputLimit) {
	printCentered(prompt, yOffset); // 打印居中提示
	int consoleWidth, consoleHeight;
	getConsoleSize(&consoleWidth, &consoleHeight); // 获取控制台大小

	int x = (consoleWidth - maxLength) / 2;
	setCursorPosition(-1, (consoleHeight / 2) + yOffset + 1); // 设置输入框位置
	enableCursor();
// 开始读取输入
	int count = 0;
	char ch;

	while (1) {
		ch = _getch(); // 获取一个字符（不会自动显示在控制台）

		if (ch == '\r') { // 检测到回车，结束输入
			break;
		} else if (ch == '\b') { // 处理退格键
			if (count > 0) {
				printf("\b \b"); // 删除最后一个字符
				count--;
			}
		} else if (ch >= 32 && ch <= 126) { // 可显示字符
			if (count < inputLimit) {
				putchar(ch); // 手动显示字符
				input[count++] = ch;
			}
		}
	}
	disableCursor();//禁用输入光标
	input[count] = '\0'; // 添加字符串结束符
	printf("\n");
}
void getPasswordCentered(const char *prompt, char *input, int maxLength, int yOffset) {
	printCentered(prompt, yOffset); // 居中显示提示
	int consoleWidth, consoleHeight;
	getConsoleSize(&consoleWidth, &consoleHeight);

	int x = (consoleWidth - maxLength) / 2;
	setCursorPosition(-1, (consoleHeight / 2) + yOffset + 0); // 输入框显示在提示下方
	enableCursor();
	int index = 0;
	char ch;

	while (index < maxLength - 1) { // 保留一个空间给字符串终止符
		ch = _getch(); // 获取按键
		if (ch == '\r') { // 回车键结束输入
			break;
		} else if (ch == '\b') { // 退格键处理
			if (index > 0) {
				index--;
				printf("\b \b"); // 删除一个字符
			}
		} else if (ch >= 32 && ch <= 126) { // 可打印字符
			input[index++] = ch;
			printf("*"); // 显示为 *
		}
	}
	disableCursor();//禁用输入光标
	input[index] = '\0'; // 添加字符串终止符
}

void SetConsoleColor(COLORREF textColor, COLORREF backgroundColor) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	// 获取当前控制台屏幕缓冲区信息
	CONSOLE_SCREEN_BUFFER_INFOEX csbi = { sizeof(csbi) };
	GetConsoleScreenBufferInfoEx(hConsole, &csbi);

	// 设置前景和背景颜色
	csbi.ColorTable[0] = backgroundColor;
	csbi.ColorTable[1] = textColor;

	// 应用颜色表
	SetConsoleScreenBufferInfoEx(hConsole, &csbi);

	// 设置文字属性（前景色1，背景色0）
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
}


void SetConsoleFontSize(int fontSizeX, int fontSizeY) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);

	// 获取当前控制台的字体信息
	GetCurrentConsoleFontEx(hConsole, FALSE, &cfi);

	// 设置新的字体大小
	cfi.dwFontSize.X = fontSizeX; // 字体宽度
	cfi.dwFontSize.Y = fontSizeY; // 字体高度

	// 应用新的字体设置
	SetCurrentConsoleFontEx(hConsole, FALSE, &cfi);
}
// 设置字体颜色（保留背景颜色）
void SetTextColor1(int textColor) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	// 获取当前控制台属性
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);

	// 保留背景颜色（高4位），仅修改字体颜色（低4位）
	WORD attributes = consoleInfo.wAttributes;
	attributes &= 0xF0;            // 清除低4位（字体颜色）
	attributes |= textColor;      // 设置新的字体颜色

	SetConsoleTextAttribute(hConsole, attributes);
}
