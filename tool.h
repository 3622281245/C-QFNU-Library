

#include <windows.h>
#include <stdio.h>
extern User nowUser;
int flok = 1; //键监听程序是否轮询；
int refresh = 0; //是否刷新；
int disableQuickEditMode();
void disableResize();
void disableScrollbars();
void setCursorPosition(int x, int y);
void SetConsoleFont(const wchar_t* fontName, SHORT fontSize);
void disableCursor();
void enableCursor();
void printCentered(const char *message, int yOffset);
void getInputCentered(const char *prompt, char *input, int maxLength, int yOffset, int inputLimit);
void getPasswordCentered(const char *prompt, char *input, int maxLength, int yOffset);
void SetConsoleColor(COLORREF textColor, COLORREF backgroundColor);
void SetConsoleFontSize(int fontSizeX, int fontSizeY);
void SetTextColor1(int textColor);
#define BUFFER_SIZE 1024//应该够用

/*
Funname: 图书简介在线获取
Author: Sunight
Description: 通过构建curl命令，-L跟随重定向至指定内容后截取字符串返回
*/
void fetch_and_extract_description(const char *book_name) {
	char command[BUFFER_SIZE];
	char line[BUFFER_SIZE];
	char *start, *end;
	FILE *pipe;

// 构建 curl 命令，获取百度百科数据，-s忽略无关输出
	snprintf(command, sizeof(command),
	         "curl -s -L \"https://baike.baidu.com/search/word?word=%s\"", book_name);

	printf("Https Getting...\n");

// 使用 popen 执行 curl 命令并读取输出
	pipe = popen(command, "r");
	if (!pipe) {
		perror("无法执行命令");
		return;
	}

	while (fgets(line, sizeof(line), pipe)) {
		start = strstr(line, "\"description\":\"");
		if (start) {
// 移动指针到文本内容的开头
			start += strlen("\"description\":\"");
			end = strchr(start, '"');
			if (end) {
				*end = '\0';
				printf("\n%s\n", start);
				break;
			}
		}
	}
	pclose(pipe);
}


// 全局变量
HINSTANCE hInst;

// 窗口过程函数声明
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// 输入框和按钮句柄
HWND hEditTitle, hEditAuthor, hEditPress, hEditStock, hButtonWrite;

// 检查用户输入并打印或提示
void ProcessInput(HWND hwnd) {
	char title[100], author[100], press[100], stockStr[20];
	int  stock;

// 获取用户输入
	GetWindowText(hEditTitle, title, sizeof(title));
	GetWindowText(hEditAuthor, author, sizeof(author));
	GetWindowText(hEditPress, press, sizeof(press));
	GetWindowText(hEditStock, stockStr, sizeof(stockStr));

// 检查是否为空
	if (strlen(title) == 0 || strlen(author) == 0 || strlen(press) == 0 || strlen(stockStr) == 0) {
		MessageBox(hwnd, TEXT("请输入完整的信息！"), TEXT("错误"), MB_OK | MB_ICONERROR);
		return;
	}

// 转换库存为整数
	stock = atoi(stockStr);
	addBook(title, author, press, stock);
// 显示输入内容
	char message[500];
	snprintf(message, sizeof(message),
	         "图书名: %s\n作者: %s\n出版社: %s\n库存: %d",
	         title, author, press,  stock);
	MessageBox(hwnd, message, TEXT("信息已写入"), MB_OK | MB_ICONINFORMATION);
	refresh = 1;
	//flok = 0;
//	drawAdminGrid(book, bookCount,1);
}

// 窗口线程函数
DWORD WINAPI WindowThread(LPVOID lpParam) {
	WNDCLASS wc = {0};
	wc.lpfnWndProc = WndProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = TEXT("BookAddTool");
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	if (!RegisterClass(&wc)) {
		printf("窗口注册失败\n");
		return 1;
	}

	HWND hwnd = CreateWindow(
	                wc.lpszClassName,
	                TEXT("图书添加工具"),
	                WS_OVERLAPPEDWINDOW,
	                CW_USEDEFAULT, CW_USEDEFAULT, 400, 400,
	                NULL, NULL, wc.hInstance, NULL);

	if (!hwnd) {
		printf("窗口创建失败\n");
		return 1;
	}

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

// 消息循环
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

// 窗口过程函数
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_CREATE:
// 创建输入框和标签
			CreateWindow(TEXT("STATIC"), TEXT("图书名:"),
			             WS_CHILD | WS_VISIBLE,
			             50, 30, 80, 25,
			             hwnd, NULL, hInst, NULL);
			hEditTitle = CreateWindow(TEXT("EDIT"), TEXT(""),
			                          WS_CHILD | WS_VISIBLE | WS_BORDER,
			                          140, 30, 200, 25,
			                          hwnd, NULL, hInst, NULL);

			CreateWindow(TEXT("STATIC"), TEXT("作者:"),
			             WS_CHILD | WS_VISIBLE,
			             50, 70, 80, 25,
			             hwnd, NULL, hInst, NULL);
			hEditAuthor = CreateWindow(TEXT("EDIT"), TEXT(""),
			                           WS_CHILD | WS_VISIBLE | WS_BORDER,
			                           140, 70, 200, 25,
			                           hwnd, NULL, hInst, NULL);

			CreateWindow(TEXT("STATIC"), TEXT("出版社:"),
			             WS_CHILD | WS_VISIBLE,
			             50, 110, 80, 25,
			             hwnd, NULL, hInst, NULL);
			hEditPress = CreateWindow(TEXT("EDIT"), TEXT(""),
			                          WS_CHILD | WS_VISIBLE | WS_BORDER,
			                          140, 110, 200, 25,
			                          hwnd, NULL, hInst, NULL);

			CreateWindow(TEXT("STATIC"), TEXT("库存数量:"),
			             WS_CHILD | WS_VISIBLE,
			             50, 150, 80, 25,
			             hwnd, NULL, hInst, NULL);
			hEditStock = CreateWindow(TEXT("EDIT"), TEXT(""),
			                          WS_CHILD | WS_VISIBLE | WS_BORDER,
			                          140, 150, 200, 25,
			                          hwnd, NULL, hInst, NULL);

			// 创建按钮
			hButtonWrite = CreateWindow(TEXT("BUTTON"), TEXT("写入书库"),
			                            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			                            140, 190, 100, 30,
			                            hwnd, (HMENU)1, hInst, NULL);
			break;

		case WM_COMMAND:
			if (LOWORD(wParam) == 1) { // 按钮点击事件
				ProcessInput(hwnd);
			}
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

// 菜单选项显示和控制
int menuControl(const char *options[], int optionCount) {
	int currentSelection = 0;
	while (1) {
		system("cls");
		// 打印标题居中
		printCentered("======== 欢迎使用图书管理系统 ========", -4);
		// 打印菜单选项居中
		for (int i = 0; i < optionCount; i++) {
			int yOffset = i * 2;  // 每个菜单项间隔 2 行
			if (i == currentSelection) {
				char highlightedOption[100];
				sprintf(highlightedOption, "> %s <", options[i]);  // 强调当前选项
				printCentered(highlightedOption, yOffset);
			} else {
				printCentered(options[i], yOffset);
			}
		}
		printCentered("使用上下方向键选择，回车键确认。", optionCount * 2 + 1);
		int ch = _getch();
		if (ch == 224) {  // 检测方向键
			ch = _getch();
			if (ch == 72) {  // 上
				currentSelection = (currentSelection - 1 + optionCount) % optionCount;
			} else if (ch == 80) {  // 下
				currentSelection = (currentSelection + 1) % optionCount;
			}
		} else if (ch == 13) {  // 回车
			return currentSelection;
		}
	}
}

void getColor(int value, char *color, size_t length) {
	//Sunight精挑细选的配色
	int rgbColors[][3] = {
		{205, 170, 125},
		{132, 112, 255},
		{0, 206, 209},
		{205, 92, 92},
		{102, 205, 170},
		{255, 182, 193},
		{147, 112, 219},
		{205, 183, 158},
		{0, 191, 255}
	};

	int r = rgbColors[value % 9][0]; // 红
	int g = rgbColors[value % 9][1]; // 绿
	int b = rgbColors[value % 9][2]; // 蓝

	snprintf(color, length, "\033[38;2;255;255;255;48;2;%d;%d;%dm", r, g, b);
}
int custom_ceil(double value) {
	int intPart = (int)value;  // 获取整数部分
	if (value > intPart) {
		return intPart + 1;  // 如果有小数部分，则向上取整
	}
	return intPart;  // 没有小数部分，直接返回整数部分
}
#define SIZE 4 // 网格大小，4x4
extern int ctrlPressed;
void drawAdminGrid_s(Book books[], int bookCount, int page) {
	int tmp = bookCount;
	bookCount = bookCount - (page - 1) * SIZE * SIZE;
	if (bookCount > SIZE * SIZE)bookCount = SIZE * SIZE;
	printf("\n该页图书数量：%d ", bookCount);
	int exit = 0;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // 控制台句柄
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	// 获取控制台大小
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	int consoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

	int cellWidth = consoleWidth / SIZE; // 每个矩形的宽度
	int cellHeight = consoleHeight / SIZE - 1; // 每个矩形的高度
	int marginX = (consoleWidth - cellWidth * SIZE) / 2; // 横向居中
	char color[40], reset[] = "\033[m";
	// 格子范围记录
	SMALL_RECT cellRanges[SIZE * SIZE];

	printf("当前页数：%d/%d  键菜单：s-搜索书籍 n-下一页 p-上一页 q-退出\n\n", page, custom_ceil((double)tmp / (SIZE * SIZE)));
	for (int y = 0; y < SIZE; y++) {
		for (int x = 0; x < SIZE; x++) {
			int bookIndex =  y * SIZE + x;
			if (bookIndex >= bookCount) break;

			// 计算格子坐标
			int top = y * cellHeight + 2; // 2 是标题占用的行数
			int bottom = top + cellHeight - 1;
			int left = marginX + x * cellWidth;
			int right = left + cellWidth - 1;

			// 保存格子范围
			cellRanges[bookIndex].Top = top;
			cellRanges[bookIndex].Bottom = bottom;
			cellRanges[bookIndex].Left = left;
			cellRanges[bookIndex].Right = right;
		}
	}

	for (int y = 0; y < SIZE; y++) {
		for (int row = 0; row < cellHeight; row++) {
			printf("%*s", marginX, "");  // 左侧留白
			for (int x = 0; x < SIZE; x++) {
				int bookIndex =  (y + (page - 1) * SIZE) * SIZE + x;
				//printf("%d",bookIndex);
				if (bookIndex - (page - 1) * SIZE * SIZE >= bookCount) break;
				getColor(bookIndex, color, 40);  // 获取颜色
				if (row == 0 || row == cellHeight - 1) {
					// 绘制矩形边框
					printf("%s%*s%s", color, cellWidth, "", reset);
				} else {
					// 居中显示图书信息
					int contentRow = row ;  // 跳过边框行
					char content[100] = "";
					if (contentRow == 0) {
						snprintf(content, sizeof(content), "ID: %d", books[bookIndex].id);
					} else if (contentRow == 1) {
						snprintf(content, sizeof(content), "书名: %s", books[bookIndex].title);
					} else if (contentRow == 2) {
						snprintf(content, sizeof(content), "作者: %s", books[bookIndex].author);
					} else if (contentRow == 3) {
						snprintf(content, sizeof(content), "出版社: %s", books[bookIndex].press);
					} else if (contentRow == 4) {
						snprintf(content, sizeof(content), "库存: %d", books[bookIndex].available);
					}

					int padding = (cellWidth - strlen(content)) / 2;
					printf("%s%*s%s%*s%s", color, padding, "", content, cellWidth - padding - strlen(content), "", reset);
				}
			}
			printf("\n");
		}
	}
	printf("\n");
	SetTextColor1(14);
//	printf("Tips：对图书项目使用鼠标滚轮与左右键可进行库存的修改，对光标下的项目使用delete键可删除该图书");
	// 监听鼠标事件
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	SetConsoleMode(hInput, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

	INPUT_RECORD inputRecord;
	DWORD events;
	COORD mousePos = {0, 0}; // 用于记录当前鼠标位置

	while ( flok && !refresh) {
		// 监听鼠标和键盘事件
		if (ReadConsoleInput(hInput, &inputRecord, 1, &events)) {
			if (inputRecord.EventType == MOUSE_EVENT) {
				MOUSE_EVENT_RECORD mouseEvent = inputRecord.Event.MouseEvent;
				mousePos = mouseEvent.dwMousePosition;
				// 如果没有滚轮或鼠标按钮事件，直接忽略（避免多次刷新）
				if (!(mouseEvent.dwEventFlags == MOUSE_WHEELED ||
				        mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED ||
				        mouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED)) {
					continue;
				}

				// 获取鼠标点击位置
				COORD clickPos = mouseEvent.dwMousePosition;

				// 遍历格子，找到鼠标所在的格子
				for (int i = 0; i < bookCount; i++) {
					int index = i + SIZE * SIZE * (page - 1);
					SMALL_RECT range = cellRanges[i];
					if (clickPos.Y >= range.Top && clickPos.Y <= range.Bottom &&
					        clickPos.X >= range.Left && clickPos.X <= range.Right) {

						int changed = 0; // 标志是否发生库存变化

						// 鼠标滚轮事件
						if (mouseEvent.dwEventFlags == MOUSE_WHEELED) {
							int delta = (SHORT)HIWORD(mouseEvent.dwButtonState) > 0 ? 1 : -1; // 滚轮方向
							books[index].available += delta;
							updateAva(books[index], delta);
							if (books[index].available < 0) {
								updateAva2(books[index], 0);
								books[index].available = 0; // 防止库存为负数
							}
							changed = 1; // 标记库存已修改
						}

						// 鼠标左键单击事件
						if (mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
							books[index].available += 50; // 增加库存 50
							updateAva(books[index], 50);
							changed = 1; // 标记库存已修改
						}

						// 鼠标右键单击事件
						if (mouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) {
							books[index].available -= 50; // 减少库存 50
							updateAva(books[index], -50);
							if (books[index].available < 0) {
								updateAva2(books[index], 0);
								books[index].available = 0; // 防止库存为负数
							}
							changed = 1; // 标记库存已修改
						}

						// 如果库存发生变化，更新显示
						if (changed) {
							system("cls"); // 清屏
							drawAdminGrid_s(books, bookCount, page);
						}

						break;
					}
				}
			}
			// 检查键盘事件
			else if (inputRecord.EventType == KEY_EVENT) {
				KEY_EVENT_RECORD keyEvent = inputRecord.Event.KeyEvent;
				if (keyEvent.bKeyDown) {  // 如果按键被按下
					char key = keyEvent.uChar.AsciiChar;
					if (key == 's' || key == 'S') {
						clearLine();
						printf("您按下了 's' 键：搜索书籍");
						Sleep(200);
						clearLine();
						system("cls");
						char keyword[50] = "";
						getInputCentered_s("请输入关键词（为空表示全部）：", keyword, sizeof(keyword), 0);
						if (strlen(keyword) == 0) {
//							printCentered("输入为空");
//							Sleep(1000);
							system("cls");
							if (books != NULL) {
								free(books);
								books = NULL;
							}
							drawAdminGrid(allBooks(), getBooksCount(), 1);
						} else {
							int book_count = 0;
							Book*searched_books = search_books(keyword, &book_count);
							if (book_count == 0) {
								printCentered("未找到相关书籍", 2);
								Sleep(1000);
							} else {
								system("cls");
								drawAdminGrid_s(searched_books, book_count, 1);
							}
						}
						break;
					} else if (key == 'q' || key == 'Q') {
						clearLine();
						printf("您按下了 'q' 键：退出系统");
						Sleep(500);
						exit = 1;
						flok = 0;
						system("cls"); // 清屏
						break;
					} else if (key == 'n' || key == 'N') {
						if (bookCount < SIZE * SIZE) {
							clearLine();
							printf("已经是最后一页了");
							Sleep(700);
							clearLine();
						} else {
							clearLine();
							printf("您按下了 'n' 键：下一页");
							Sleep(200);
							system("cls");
							drawAdminGrid(books, bookCount, page + 1);
						}

					} else if (key == 'p' || key == 'P') {
						if (page == 1) {
							clearLine();
							printf("已经是第一页了");
							Sleep(700);
							clearLine();
						} else {
							clearLine();
							printf("您按下了 'p' 键：上一页");
							Sleep(200);
							system("cls");
							drawAdminGrid(books, bookCount, page - 1);
						}
					} else if (keyEvent.wVirtualKeyCode == VK_DELETE) {
// Delete 键被按下


// 遍历格子，找到鼠标停留的格子
						for (int i = 0; i < bookCount; i++) {
							int index = i + SIZE * SIZE * (page - 1);
							SMALL_RECT range = cellRanges[i];
							if (mousePos.Y >= range.Top && mousePos.Y <= range.Bottom &&
							        mousePos.X >= range.Left && mousePos.X <= range.Right) {
								clearLine();
								printf("删除书籍: %s", books[index].title);

// 删除书籍逻辑
								deleteBook(books[index].title);

// 更新显示
								system("cls");
								if (books != NULL) {
									free(books);
									books = NULL;
								}
								drawAdminGrid(allBooks(), bookCount, page);
								break;
							}
						}
					}
				}
			}
		}
	}
	if (!exit && flok || refresh) {
		refresh = 0;
		flok = 1;
		system("cls"); // 清屏
		drawAdminGrid_s(books, bookCount, page);
	}

}
//绘制管理员网格
void drawAdminGrid(Book books[], int bookCount, int page) {
	bookCount = getBooksCount() - (page - 1) * SIZE * SIZE;
	if (bookCount > SIZE * SIZE)bookCount = SIZE * SIZE;
	printf("\n该页图书数量：%d ", bookCount);
	int exit = 0;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // 控制台句柄
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	// 获取控制台大小
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	int consoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

	int cellWidth = consoleWidth / SIZE; // 每个矩形的宽度
	int cellHeight = consoleHeight / SIZE - 1; // 每个矩形的高度
	int marginX = (consoleWidth - cellWidth * SIZE) / 2; // 横向居中
	char color[40], reset[] = "\033[m";
	// 格子范围记录
	SMALL_RECT cellRanges[SIZE * SIZE];
	SetTextColor1(14);
	printf("当前页数：%d/%d  键菜单：s-搜索书籍 n-下一页 p-上一页 q-退出\n\n", page, custom_ceil((double)getBooksCount() / (SIZE * SIZE)));
	for (int y = 0; y < SIZE; y++) {
		for (int x = 0; x < SIZE; x++) {
			int bookIndex =  y * SIZE + x;
			if (bookIndex >= bookCount) break;

			// 计算格子坐标
			int top = y * cellHeight + 2; // 2 是标题占用的行数
			int bottom = top + cellHeight - 1;
			int left = marginX + x * cellWidth;
			int right = left + cellWidth - 1;

			// 保存格子范围
			cellRanges[bookIndex].Top = top;
			cellRanges[bookIndex].Bottom = bottom;
			cellRanges[bookIndex].Left = left;
			cellRanges[bookIndex].Right = right;
		}
	}

	for (int y = 0; y < SIZE; y++) {
		for (int row = 0; row < cellHeight; row++) {
			printf("%*s", marginX, "");  // 左侧留白
			for (int x = 0; x < SIZE; x++) {
				int bookIndex =  (y + (page - 1) * SIZE) * SIZE + x;
				//printf("%d",bookIndex);
				if (bookIndex - (page - 1) * SIZE * SIZE >= bookCount) break;
				getColor(bookIndex, color, 40);  // 获取颜色
				if (row == 0 || row == cellHeight - 1) {
					// 绘制矩形边框
					printf("%s%*s%s", color, cellWidth, "", reset);
				} else {
					// 居中显示图书信息
					int contentRow = row ;  // 跳过边框行
					char content[100] = "";
					SetTextColor1(14);
					if (contentRow == 0) {
						snprintf(content, sizeof(content), "ID: %d", books[bookIndex].id);
					} else if (contentRow == 1) {
						snprintf(content, sizeof(content), "书名: %s", books[bookIndex].title);
					} else if (contentRow == 2) {
						snprintf(content, sizeof(content), "作者: %s", books[bookIndex].author);
					} else if (contentRow == 3) {
						snprintf(content, sizeof(content), "出版社: %s", books[bookIndex].press);
					} else if (contentRow == 4) {
						snprintf(content, sizeof(content), "库存: %d", books[bookIndex].available);
					}

					int padding = (cellWidth - strlen(content)) / 2;
					printf("%s%*s%s%*s%s", color, padding, "", content, cellWidth - padding - strlen(content), "", reset);
				}
			}
			printf("\n");
		}
	}
	printf("\n");
	SetTextColor1(14);
	printf("Tips：对图书项目使用鼠标滚轮（按住Shift）与左右键可进行库存的修改，对光标下的项目使用delete键可删除该图书");

	// 监听鼠标事件
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	SetConsoleMode(hInput, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

	INPUT_RECORD inputRecord;
	DWORD events;
	COORD mousePos = {0, 0}; // 用于记录当前鼠标位置

	while ( flok && !refresh) {
		// 监听鼠标和键盘事件
		if (ReadConsoleInput(hInput, &inputRecord, 1, &events)) {
			if (inputRecord.EventType == MOUSE_EVENT) {
				MOUSE_EVENT_RECORD mouseEvent = inputRecord.Event.MouseEvent;
				mousePos = mouseEvent.dwMousePosition;

				// 如果没有滚轮或鼠标按钮事件，直接忽略（避免多次刷新）
				if (!(mouseEvent.dwEventFlags == MOUSE_WHEELED ||
				        mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED ||
				        mouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED)) {
					continue;
				}

				// 获取鼠标点击位置
				COORD clickPos = mouseEvent.dwMousePosition;

				// 遍历格子，找到鼠标所在的格子
				for (int i = 0; i < bookCount; i++) {
					int index = i + SIZE * SIZE * (page - 1);
					SMALL_RECT range = cellRanges[i];
					if (clickPos.Y >= range.Top && clickPos.Y <= range.Bottom &&
					        clickPos.X >= range.Left && clickPos.X <= range.Right) {

						int changed = 0; // 标志是否发生库存变化

						// 鼠标滚轮事件
						if (mouseEvent.dwEventFlags == MOUSE_WHEELED) {
							int delta = (SHORT)HIWORD(mouseEvent.dwButtonState) > 0 ? 1 : -1; // 滚轮方向
							if (ctrlPressed) {
								books[index].available += delta;
								updateAva(books[index], delta);
								if (books[index].available < 0) {
									updateAva2(books[index], 0);
									books[index].available = 0; // 防止库存为负数
								}
								changed = 1; // 标记库存已修改
							} else {
								if (page - delta > 0 && page - delta <= custom_ceil((double)getBooksCount() / (SIZE * SIZE))) {
									system("cls");
									if (books != NULL) {
										free(books);
										books = NULL;
									}
									drawAdminGrid(allBooks(), bookCount, page - delta);
								}
							}

						}

						// 鼠标左键单击事件
						if (mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
							books[index].available += 50; // 增加库存 50
							updateAva(books[index], 50);
							changed = 1; // 标记库存已修改
						}

						// 鼠标右键单击事件
						if (mouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) {
							books[index].available -= 50; // 减少库存 50
							updateAva(books[index], -50);
							if (books[index].available < 0) {
								updateAva2(books[index], 0);
								books[index].available = 0; // 防止库存为负数
							}
							changed = 1; // 标记库存已修改
						}

						// 如果库存发生变化，更新显示
						if (changed) {
							system("cls");
							//释放动态分配的内存，防止泄露
							if (books != NULL) {
								free(books);
								books = NULL;
							}
							drawAdminGrid(allBooks(), bookCount, page);
						}

						break;
					}
				}
			}
			// 检查键盘事件
			else if (inputRecord.EventType == KEY_EVENT) {
				KEY_EVENT_RECORD keyEvent = inputRecord.Event.KeyEvent;
				if (keyEvent.bKeyDown) {  // 如果按键被按下
					char key = keyEvent.uChar.AsciiChar;
					if (key == 's' || key == 'S') {
//						printf("您按下了 's' 键：搜索书籍功能\n");
//						Sleep(200);
//						clearLine();
						system("cls");
						char keyword[50] = "";
						getInputCentered_s("请输入关键词（为空表示全部）：", keyword, sizeof(keyword), 0);
						if (strlen(keyword) == 0) {
//							printCentered("输入为空");
//							Sleep(1000);
							system("cls");
							if (books != NULL) {
								free(books);
								books = NULL;
							}
							drawAdminGrid(allBooks(), getBooksCount(), 1);
						} else {
							int book_count = 0;
							Book*searched_books = search_books(keyword, &book_count);
							if (book_count == 0) {
								printCentered("未找到相关书籍", 2);
								Sleep(1000);
							} else {
								system("cls");
								if (books != NULL) {
									free(books);
									books = NULL;
								}
								drawAdminGrid_s(searched_books, book_count, 1);
							}
						}
						break;
					} else if (key == 'q' || key == 'Q') {
//						clearLine();
//						printf("您按下了 'q' 键：退出系统\n");
//						Sleep(500);
						exit = 1;
						flok = 0;
						system("cls"); // 清屏
						break;
					} else if (key == 'n' || key == 'N') {
						if (bookCount < SIZE * SIZE) {
//							printf("已经是最后一页了");
//							Sleep(700);
//							clearLine();
						} else {
//							clearLine();
//							printf("您按下了 'n' 键：下一页\n");
//							Sleep(200);
							system("cls");
							if (books != NULL) {
								free(books);
								books = NULL;
							}
							drawAdminGrid(allBooks(), bookCount, page + 1);
						}

					} else if (key == 'p' || key == 'P') {
						if (page == 1) {
//							clearLine();
//							printf("已经是第一页了");
//							Sleep(700);
//							clearLine();
						} else {
//							clearLine();
//							printf("您按下了 'p' 键：上一页\n");
//							Sleep(200);
							system("cls");
							if (books != NULL) {
								free(books);
								books = NULL;
							}
							drawAdminGrid(allBooks(), bookCount, page - 1);
						}

					} else if (keyEvent.wVirtualKeyCode == VK_DELETE) {
// Delete 键被按下


// 遍历格子，找到鼠标停留的格子
						for (int i = 0; i < bookCount; i++) {
							int index = i + SIZE * SIZE * (page - 1);
							SMALL_RECT range = cellRanges[i];
							if (mousePos.Y >= range.Top && mousePos.Y <= range.Bottom &&
							        mousePos.X >= range.Left && mousePos.X <= range.Right) {
//								clearLine();
//								printf("删除书籍: %s\n", books[index].title);

// 删除书籍逻辑
								deleteBook(books[index].title);

// 更新显示
								system("cls");
								if (books != NULL) {
									free(books);
									books = NULL;
								}
								drawAdminGrid(allBooks(), bookCount, page);
								break;
							}
						}
					}
				}
			}
		}
	}
	if (!exit && flok || refresh) {
		refresh = 0;
		flok = 1;
		system("cls"); // 清屏
		if (books != NULL) {
			free(books);
			books = NULL;
		}
		drawAdminGrid(allBooks(), bookCount, page);
	}
	if (books != NULL) {
		free(books);
		books = NULL;
	}
}

void drawBorrowGrid_s(Book books[], int bookCount, int page) {
	int tmp = bookCount;
	bookCount = bookCount - (page - 1) * SIZE * SIZE;
	if (bookCount > SIZE * SIZE)bookCount = SIZE * SIZE;
	printf("\n该页图书数量：%d ", bookCount);
	int exit = 0;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // 控制台句柄
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	// 获取控制台大小
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	int consoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

	int cellWidth = consoleWidth / SIZE; // 每个矩形的宽度
	int cellHeight = consoleHeight / SIZE - 1; // 每个矩形的高度
	int marginX = (consoleWidth - cellWidth * SIZE) / 2; // 横向居中
	char color[40], reset[] = "\033[m";
	// 格子范围记录
	SMALL_RECT cellRanges[SIZE * SIZE];
	printf("当前页数：%d/%d  单击图书即可借阅，键菜单：n-下一页 p-上一页 q-退出\n\n", page, custom_ceil((double)tmp / (SIZE * SIZE)));
	for (int y = 0; y < SIZE; y++) {
		for (int x = 0; x < SIZE; x++) {
			int bookIndex = y * SIZE + x;
			if (bookIndex >= bookCount) break;

			// 计算格子坐标
			int top = y * cellHeight + 2; // 2 是标题占用的行数
			int bottom = top + cellHeight - 1;
			int left = marginX + x * cellWidth;
			int right = left + cellWidth - 1;

			// 保存格子范围
			cellRanges[bookIndex].Top = top;
			cellRanges[bookIndex].Bottom = bottom;
			cellRanges[bookIndex].Left = left;
			cellRanges[bookIndex].Right = right;
		}
	}

	for (int y = 0; y < SIZE; y++) {
		for (int row = 0; row < cellHeight; row++) {
			printf("%*s", marginX, "");  // 左侧留白
			for (int x = 0; x < SIZE; x++) {
				int bookIndex =  (y + (page - 1) * SIZE) * SIZE + x;
				//printf("%d",bookIndex);
				if (bookIndex - (page - 1) * SIZE * SIZE >= bookCount) break;
				getColor(bookIndex, color, 40);  // 获取颜色
				if (row == 0 || row == cellHeight - 1) {
					// 绘制矩形边框
					printf("%s%*s%s", color, cellWidth, "", reset);
				} else {
					// 居中显示图书信息
					int contentRow = row ;  // 跳过边框行
					char content[100] = "";
					if (contentRow == 0) {
						snprintf(content, sizeof(content), "ID: %d", books[bookIndex].id);
					} else if (contentRow == 1) {
						snprintf(content, sizeof(content), "书名: %s", books[bookIndex].title);
					} else if (contentRow == 2) {
						snprintf(content, sizeof(content), "作者: %s", books[bookIndex].author);
					} else if (contentRow == 3) {
						snprintf(content, sizeof(content), "出版社: %s", books[bookIndex].press);
					} else if (contentRow == 4) {
						snprintf(content, sizeof(content), "库存: %d", books[bookIndex].available);
					}

					int padding = (cellWidth - strlen(content)) / 2;
					printf("%s%*s%s%*s%s", color, padding, "", content, cellWidth - padding - strlen(content), "", reset);
				}
			}
			printf("\n");
		}
	}
	printf("\n");
	SetTextColor1(14);
	// 监听鼠标事件
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	SetConsoleMode(hInput, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

	INPUT_RECORD inputRecord;
	DWORD events;

	while (flok && !refresh) {
		// 监听鼠标和键盘事件
		if (ReadConsoleInput(hInput, &inputRecord, 1, &events)) {
			if (inputRecord.EventType == MOUSE_EVENT) {
				MOUSE_EVENT_RECORD mouseEvent = inputRecord.Event.MouseEvent;

				// 如果没有滚轮或鼠标按钮事件，直接忽略（避免多次刷新）
				if (!(mouseEvent.dwEventFlags == MOUSE_WHEELED ||
				        mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED ||
				        mouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED)) {
					continue;
				}

				// 鼠标滚轮事件
				if (mouseEvent.dwEventFlags == MOUSE_WHEELED) {
					int delta = (SHORT)HIWORD(mouseEvent.dwButtonState) > 0 ? 1 : -1; // 滚轮方向
					if (page - delta > 0 && page - delta <= custom_ceil((double)tmp / (SIZE * SIZE))) {
						system("cls");
						drawBorrowGrid_s(books, bookCount, page - delta);
					}
				}
				// 鼠标左键单击事件
				if (mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
					// 获取鼠标点击位置
					COORD clickPos = mouseEvent.dwMousePosition;

					// 遍历格子，找到点击的格子
					for (int i = 0; i < bookCount; i++) {
						SMALL_RECT range = cellRanges[i];
						if (clickPos.Y >= range.Top && clickPos.Y <= range.Bottom &&
						        clickPos.X >= range.Left && clickPos.X <= range.Right) {
							int index = i + SIZE * SIZE * (page - 1);
							if (books[index].available) {
								Sleep(100);
								char message[] = "是否借阅此书籍？\n\n";
								char *title[128];

								sprintf(title, "%s  剩余数量：%d", books[index].title, books[index].available);
								int result = MessageBox(NULL,
								                        message,
								                        title,
								                        MB_OKCANCEL | MB_TOPMOST);
								if (result == IDOK) {
									add_book_to_user(nowUser.username, books[index]);
									Sleep(1000);
									system("cls");
									if (books != NULL) {
										free(books);
										books = NULL;
									}
									drawBorrowGrid(allBooks(), bookCount, page);
								}
							} else {
								Sleep(100);
								MessageBox(
								    NULL,
								    "库存不足",
								    books[index].title,
								    MB_OK | MB_TOPMOST
								);
							}
							break;
						}
					}
				}

				break;
			}
			// 检查键盘事件
			else if (inputRecord.EventType == KEY_EVENT) {
				KEY_EVENT_RECORD keyEvent = inputRecord.Event.KeyEvent;

				if (keyEvent.bKeyDown) {  // 如果按键被按下
					char key = keyEvent.uChar.AsciiChar;
					if (key == 's' || key == 'S') {
						printf("您按下了 's' 键：搜索书籍");
						Sleep(200);
						clearLine();
						system("cls");
						char keyword[50] = "";
						getInputCentered_s("请输入关键词（为空表示全部）：", keyword, sizeof(keyword), 0);
						if (strlen(keyword) == 0) {
							system("cls");
							if (books != NULL) {
								free(books);
								books = NULL;
							}

							drawBorrowGrid(allBooks(), getBooksCount(), 1);
						} else {
							int book_count = 0;
							Book*searched_books = search_books(keyword, &book_count);
							if (book_count == 0) {
								printCentered("未找到相关书籍", 2);
								Sleep(1000);
								system("cls");
								if (books != NULL) {
									free(books);
									books = NULL;
								}

								drawBorrowGrid(allBooks(), getBooksCount(), 1);
							} else {
								system("cls");
								drawBorrowGrid_s(searched_books, book_count, 1);
							}
						}
						break;
					} else if (key == 'q' || key == 'Q') {
						printf("您按下了 'q' 键：退出系统");
						Sleep(500);
						exit = 1;
						flok = 0;
						system("cls"); // 清屏
						break;
					} else if (key == 'n' || key == 'N') {
						if (bookCount < SIZE * SIZE) {
							printf("已经是最后一页了");
							Sleep(700);
							clearLine();
						} else {
							printf("您按下了 'n' 键：下一页");
							Sleep(200);
							system("cls");
							drawBorrowGrid_s(books, bookCount, page + 1);
						}

					} else if (key == 'p' || key == 'P') {
						if (page == 1) {
							printf("已经是第一页了");
							Sleep(700);
							clearLine();
						} else {
							printf("您按下了 'p' 键：上一页");
							Sleep(200);
							system("cls");
							drawBorrowGrid_s(books, bookCount, page - 1);
						}

					}
				}
			}
		}
	}
	if (!exit && flok || refresh) {
		refresh = 0;
		flok = 1;
		system("cls"); // 清屏
		drawBorrowGrid_s(books, bookCount, page);
	}
	if (books != NULL) {
		free(books);
		books = NULL;
	}
}
void drawBorrowGrid(Book books[], int bookCount, int page) {
	bookCount = getBooksCount() - (page - 1) * SIZE * SIZE;
	if (bookCount > SIZE * SIZE)bookCount = SIZE * SIZE;
	printf("\n该页图书数量：%d ", bookCount);
	int exit = 0;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // 控制台句柄
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	// 获取控制台大小
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	int consoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

	int cellWidth = consoleWidth / SIZE; // 每个矩形的宽度
	int cellHeight = consoleHeight / SIZE - 1; // 每个矩形的高度
	int marginX = (consoleWidth - cellWidth * SIZE) / 2; // 横向居中
	char color[40], reset[] = "\033[m";
	// 格子范围记录
	SMALL_RECT cellRanges[SIZE * SIZE];
	printf("当前页数：%d/%d  单击图书即可借阅，键菜单：s-搜索书籍 n-下一页 p-上一页 q-退出\n\n", page, custom_ceil((double)getBooksCount() / (SIZE * SIZE)));
	for (int y = 0; y < SIZE; y++) {
		for (int x = 0; x < SIZE; x++) {
			int bookIndex = y * SIZE + x;
			if (bookIndex >= bookCount) break;

			// 计算格子坐标
			int top = y * cellHeight + 2; // 2 是标题占用的行数
			int bottom = top + cellHeight - 1;
			int left = marginX + x * cellWidth;
			int right = left + cellWidth - 1;

			// 保存格子范围
			cellRanges[bookIndex].Top = top;
			cellRanges[bookIndex].Bottom = bottom;
			cellRanges[bookIndex].Left = left;
			cellRanges[bookIndex].Right = right;
		}
	}

	for (int y = 0; y < SIZE; y++) {
		for (int row = 0; row < cellHeight; row++) {
			printf("%*s", marginX, "");  // 左侧留白
			for (int x = 0; x < SIZE; x++) {
				int bookIndex =  (y + (page - 1) * SIZE) * SIZE + x;
				//printf("%d",bookIndex);
				if (bookIndex - (page - 1) * SIZE * SIZE >= bookCount) break;
				getColor(bookIndex, color, 40);  // 获取颜色
				if (row == 0 || row == cellHeight - 1) {
					// 绘制矩形边框
					printf("%s%*s%s", color, cellWidth, "", reset);
				} else {
					// 居中显示图书信息
					int contentRow = row ;  // 跳过边框行
					char content[100] = "";
					if (contentRow == 0) {
						snprintf(content, sizeof(content), "ID: %d", books[bookIndex].id);
					} else if (contentRow == 1) {
						snprintf(content, sizeof(content), "书名: %s", books[bookIndex].title);
					} else if (contentRow == 2) {
						snprintf(content, sizeof(content), "作者: %s", books[bookIndex].author);
					} else if (contentRow == 3) {
						snprintf(content, sizeof(content), "出版社: %s", books[bookIndex].press);
					} else if (contentRow == 4) {
						snprintf(content, sizeof(content), "库存: %d", books[bookIndex].available);
					}

					int padding = (cellWidth - strlen(content)) / 2;
					printf("%s%*s%s%*s%s", color, padding, "", content, cellWidth - padding - strlen(content), "", reset);
				}
			}
			printf("\n");
		}
	}
	printf("\n");
	SetTextColor1(14);
	// 监听鼠标事件
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	SetConsoleMode(hInput, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

	INPUT_RECORD inputRecord;
	DWORD events;

	while (flok && !refresh) {
		// 监听鼠标和键盘事件
		if (ReadConsoleInput(hInput, &inputRecord, 1, &events)) {
			if (inputRecord.EventType == MOUSE_EVENT) {
				MOUSE_EVENT_RECORD mouseEvent = inputRecord.Event.MouseEvent;
				// 如果没有滚轮或鼠标按钮事件，直接忽略（避免多次刷新）
				if (!(mouseEvent.dwEventFlags == MOUSE_WHEELED ||
				        mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED ||
				        mouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED)) {
					continue;
				}

				// 鼠标滚轮事件
				if (mouseEvent.dwEventFlags == MOUSE_WHEELED) {
					int delta = (SHORT)HIWORD(mouseEvent.dwButtonState) > 0 ? 1 : -1; // 滚轮方向
					if (page - delta > 0 && page - delta <= custom_ceil((double)getBooksCount() / (SIZE * SIZE))) {
						system("cls");
						if (books != NULL) {
							free(books);
							books = NULL;
						}
						drawBorrowGrid(allBooks(), bookCount, page - delta);
					}
				}
				// 鼠标左键单击事件
				if (mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
					// 获取鼠标点击位置
					COORD clickPos = mouseEvent.dwMousePosition;

					// 遍历格子，找到点击的格子
					for (int i = 0; i < bookCount; i++) {
						SMALL_RECT range = cellRanges[i];
						if (clickPos.Y >= range.Top && clickPos.Y <= range.Bottom &&
						        clickPos.X >= range.Left && clickPos.X <= range.Right) {
							int index = i + SIZE * SIZE * (page - 1);
							if (books[index].available) {
								Sleep(100);
								char message[] = "是否借阅此书籍？\n\n";
								char *title[128];

								sprintf(title, "%s  剩余数量：%d", books[index].title, books[index].available);
								int result = MessageBox(NULL,
								                        message,
								                        title,
								                        MB_OKCANCEL | MB_TOPMOST);
								if (result == IDOK) {
									add_book_to_user(nowUser.username, books[index]);
									Sleep(1000);
									system("cls");
									if (books != NULL) {
										free(books);
										books = NULL;
									}
									drawBorrowGrid(allBooks(), getBooksCount(), 1);
								}
							} else {
								Sleep(100);
								MessageBox(
								    NULL,
								    "库存不足",
								    books[index].title,
								    MB_OK | MB_TOPMOST
								);
							}
							break;
						}
					}
				}
				break;
			}
			// 检查键盘事件
			else if (inputRecord.EventType == KEY_EVENT) {
				KEY_EVENT_RECORD keyEvent = inputRecord.Event.KeyEvent;

				if (keyEvent.bKeyDown) {  // 如果按键被按下
					char key = keyEvent.uChar.AsciiChar;
					if (key == 's' || key == 'S') {
						printf("您按下了 's' 键：搜索书籍");
						Sleep(200);
						clearLine();
						system("cls");
						char keyword[50] = "";
						getInputCentered_s("请输入关键词（为空表示全部）：", keyword, sizeof(keyword), 0);
						if (strlen(keyword) == 0) {
							system("cls");
							if (books != NULL) {
								free(books);
								books = NULL;
							}
							drawBorrowGrid(allBooks(), getBooksCount(), 1);
						} else {
							int book_count = 0;
							Book*searched_books = search_books(keyword, &book_count);
							if (book_count == 0) {
								printCentered("未找到相关书籍", 2);
								Sleep(1000);
							} else {
								system("cls");
								if (books != NULL) {
									free(books);
									books = NULL;
								}
								drawBorrowGrid_s(searched_books, book_count, 1);
							}
						}
						break;
					} else if (key == 'q' || key == 'Q') {
						printf("您按下了 'q' 键：退出系统");
						Sleep(500);
						exit = 1;
						flok = 0;
						system("cls"); // 清屏
						break;
					} else if (key == 'n' || key == 'N') {
						if (bookCount < SIZE * SIZE) {
							printf("已经是最后一页了");
							Sleep(700);
							clearLine();
						} else {
							printf("您按下了 'n' 键：下一页");
							Sleep(200);
							system("cls");
							if (books != NULL) {
								free(books);
								books = NULL;
							}
							drawBorrowGrid(allBooks(), bookCount, page + 1);
						}

					} else if (key == 'p' || key == 'P') {
						if (page == 1) {
							printf("已经是第一页了");
							Sleep(700);
							clearLine();
						} else {
							printf("您按下了 'p' 键：上一页");
							Sleep(200);
							system("cls");
							if (books != NULL) {
								free(books);
								books = NULL;
							}
							drawBorrowGrid(allBooks(), bookCount, page - 1);
						}

					}
				}
			}
		}
	}
	if (!exit && flok || refresh) {
		refresh = 0;
		flok = 1;
		system("cls"); // 清屏
		if (books != NULL) {
			free(books);
			books = NULL;
		}
		drawBorrowGrid(allBooks(), bookCount, page);
	}
	if (books != NULL) {
		free(books);
		books = NULL;
	}
}


void drawBorrowedGrid() {
	system("cls");
	Book *books = get_borrowed_books(nowUser.username);
	int bookCount = get_borrowed_count(nowUser.username);

	int exit = 0;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // 控制台句柄
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	// 获取控制台大小
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	int consoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

	int cellWidth = consoleWidth / SIZE; // 每个矩形的宽度
	int cellHeight = consoleHeight / SIZE - 1; // 每个矩形的高度
	int marginX = (consoleWidth - cellWidth * SIZE) / 2; // 横向居中
	char color[40], reset[] = "\033[m";
	// 格子范围记录
	SMALL_RECT cellRanges[SIZE * SIZE];
	printf("鼠标单击图书即可归还，按任意键退出\n\n");
	for (int y = 0; y < SIZE; y++) {
		for (int x = 0; x < SIZE; x++) {
			int bookIndex = y * SIZE + x;
			if (bookIndex >= bookCount) break;

			// 计算格子坐标
			int top = y * cellHeight + 2; // 2 是标题占用的行数
			int bottom = top + cellHeight - 1;
			int left = marginX + x * cellWidth;
			int right = left + cellWidth - 1;

			// 保存格子范围
			cellRanges[bookIndex].Top = top;
			cellRanges[bookIndex].Bottom = bottom;
			cellRanges[bookIndex].Left = left;
			cellRanges[bookIndex].Right = right;
		}
	}

	for (int y = 0; y < SIZE; y++) {
		for (int row = 0; row < cellHeight; row++) {
			printf("%*s", marginX, "");  // 左侧留白
			for (int x = 0; x < SIZE; x++) {
				int bookIndex = y * SIZE + x;
				//printf("%d",bookIndex);
				if (bookIndex  >= bookCount) break;
				getColor(bookIndex, color, 40);  // 获取颜色
				if (row == 0 || row == cellHeight - 1) {
					// 绘制矩形边框
					printf("%s%*s%s", color, cellWidth, "", reset);
				} else {
					// 居中显示图书信息
					int contentRow = row ;  // 跳过边框行
					char content[100] = "";
					if (contentRow == 0) {
						snprintf(content, sizeof(content), "ID: %d", books[bookIndex].id);
					} else if (contentRow == 1) {
						snprintf(content, sizeof(content), "书名: %s", books[bookIndex].title);
					} else if (contentRow == 2) {
						snprintf(content, sizeof(content), "作者: %s", books[bookIndex].author);
					} else if (contentRow == 3) {
						snprintf(content, sizeof(content), "出版社: %s", books[bookIndex].press);
					}

					int padding = (cellWidth - strlen(content)) / 2;
					printf("%s%*s%s%*s%s", color, padding, "", content, cellWidth - padding - strlen(content), "", reset);
				}
			}
			printf("\n");
		}
	}
	printf("\n");
	SetTextColor1(14);
	// 监听鼠标事件
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	SetConsoleMode(hInput, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

	INPUT_RECORD inputRecord;
	DWORD events;

	while (1) {
		// 监听鼠标和键盘事件
		if (ReadConsoleInput(hInput, &inputRecord, 1, &events)) {
			if (inputRecord.EventType == MOUSE_EVENT) {
				MOUSE_EVENT_RECORD mouseEvent = inputRecord.Event.MouseEvent;

				// 如果没有滚轮或鼠标按钮事件，直接忽略（避免多次刷新）
				if (!(
				            mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED ||
				            mouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED)) {
					continue;
				}
				// 鼠标左键单击事件
				if (mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
					// 获取鼠标点击位置
					COORD clickPos = mouseEvent.dwMousePosition;
					// 遍历格子，找到点击的格子
					for (int i = 0; i < bookCount; i++) {
						SMALL_RECT range = cellRanges[i];
						if (clickPos.Y >= range.Top && clickPos.Y <= range.Bottom &&
						        clickPos.X >= range.Left && clickPos.X <= range.Right) {
							Sleep(100);
							char message[] = "是否归还此书籍？\n\n";
							char *title[128];
							sprintf(title, "%s", books[i].title);
							int result = MessageBox(NULL, // 父窗口句柄（NULL表示无父窗口）
							                        message, // 提示框内容
							                        title, // 提示框标题
							                        MB_OKCANCEL | MB_TOPMOST);
							if (result == IDOK) {
								updateAva(books[i], +1);
								if (removeBorrowedBook(nowUser.username, books[i].title)) {
									MessageBox(
									    NULL, // 父窗口句柄（NULL表示无父窗口）
									    "归还成功", // 提示框内容
									    "提示", // 提示框标题
									    MB_OK | MB_TOPMOST // 按钮类型和图标类型
									);
									system("cls"); // 清屏
									drawBorrowedGrid();
								}
							}
							break;
						}
					}
				}

				break;
			}
			// 检查键盘事件
			else if (inputRecord.EventType == KEY_EVENT) {
				KEY_EVENT_RECORD keyEvent = inputRecord.Event.KeyEvent;
				if (keyEvent.bKeyDown) {  // 如果按键被按下
					char key = keyEvent.uChar.AsciiChar;
					if (key == 'q' || key == 'Q') {
						printf("您按下了 'q' 键：退出系统\n");
						Sleep(500);
						exit = 1;
						flok = 0;
						system("cls"); // 清屏
						break;
					}
				}
			}
		}
	}

}


void showAdminOptions() {
// 示例图书数据
	Book books[] = {
		{"C Programming", "Author A", "Press A", 1, 10},
		{"Algorithms", "Author B", "Press B", 2, 15},
		{"Data Structures", "Author C", "Press C", 3, 20},
		{"AI Basics", "Author D", "Press D", 4, 5},
		{"Machine Learning", "Author E", "Press E", 5, 8},
		{"Networks", "Author F", "Press F", 6, 12},
		{"Web Dev", "Author G", "Press G", 7, 7},
		{"OS Concepts", "Author H", "Press H", 8, 3},
		{"Databases", "Author I", "Press I", 9, 25},
		{"C Programming", "Author A", "Press A", 1, 10},
		{"Algorithms", "Author B", "Press B", 2, 15},
		{"Data Structures", "Author C", "Press C", 3, 20},
		{"AI Basics", "Author D", "Press D", 4, 5},
		{"Machine Learning", "Author E", "Press E", 5, 8},
		{"Networks", "Author F", "Press F", 6, 12},
		{"Web Dev", "Author G", "Press G", 7, 7},
		{"OS Concepts", "Author H", "Press H", 8, 3},
		{"Databases", "Author I", "Press I", 9, 25}
	};
	Book *book = allBooks();

	int bookCount = getBooksCount();

	system("cls");
	flok = 1;
	// 绘制网格
	drawAdminGrid(book, bookCount, 1);


}

void showBorrowOptions() {
// 示例图书数据
	Book books[] = {
		{"C Programming", "Author A", "Press A", 1, 10},
		{"Algorithms", "Author B", "Press B", 2, 15},
		{"Data Structures", "Author C", "Press C", 3, 20},
		{"AI Basics", "Author D", "Press D", 4, 5},
		{"Machine Learning", "Author E", "Press E", 5, 8},
		{"Networks", "Author F", "Press F", 6, 12},
		{"Web Dev", "Author G", "Press G", 7, 7},
		{"OS Concepts", "Author H", "Press H", 8, 3},
		{"Databases", "Author I", "Press I", 9, 25},
		{"C Programming", "Author A", "Press A", 1, 10},
		{"Algorithms", "Author B", "Press B", 2, 15},
		{"Data Structures", "Author C", "Press C", 3, 20},
		{"AI Basics", "Author D", "Press D", 4, 5},
		{"Machine Learning", "Author E", "Press E", 5, 8},
		{"Networks", "Author F", "Press F", 6, 12},
		{"Web Dev", "Author G", "Press G", 7, 7},
		{"OS Concepts", "Author H", "Press H", 8, 3},
		{"Databases", "Author I", "Press I", 9, 25}
	};

	Book *book = allBooks();

	int bookCount = getBooksCount();
	system("cls");
	flok = 1;
	// 绘制网格
	drawBorrowGrid(book, bookCount, 1);


}
// 判断字符串是否全是数字
int isNumber(const char* str) {
	for (int i = 0; i < strlen(str); i++) {
		if (!isdigit(str[i])) {
			return 0;  // 如果有非数字字符，返回 0
		}
	}
	return 1;  // 所有字符都是数字，返回 1
}
void showUserOptions() {
	const char *userOptions[] = {
		"进入图书馆",
		"在借图书归还",
		"个人信息修改",
		"退出登录"
	};
	int userOptionCount = sizeof(userOptions) / sizeof(userOptions[0]);

	while (1) {
		int choice = menuControl(userOptions, userOptionCount); // 使用现有菜单控件功能
		system("cls");
		switch (choice) {
			case 0:  // 图书借阅
				//printCentered("您选择了图书借阅功能。", 0);
				showBorrowOptions();
				break;
			case 1:  // 图书借阅
				drawBorrowedGrid();

//				//printCentered("按下对应图书序号", (i - 2) * 2);
//				Book* blist = get_borrowed_books(nowUser.username);
//				for (int i = 0; i < 5; i++) {
//					char book[100];
//					sprintf(book, "书名：%s，作者：%s", blist[i].title, blist[i].author);
//					printCentered(book, (i - 2) * 2);
//				}
//				getchar();
				break;

			case 2:  // 个人信息修改
				printCentered("----按任意选项外键退出----", -2);
				char *userinfo[50];
				char *year[50];
				char *phone[50];
				sprintf(userinfo, "当前用户：%s（输入相应数字以修改指定项）", nowUser.username);
				sprintf(year, "\b\b【1】年级：%d          ", nowUser.year);
				sprintf(phone, "【2】联系方式：%s\n", nowUser.phone);
				printCentered(userinfo, -4);
				printCentered(year, 0);
				printCentered(phone, 2);
				char choice = _getch();
				if (choice == '1') {
					system("cls");
					while (1) {
						getInputCentered("请输入当前年级:  \b", year, sizeof(year), 0, 1);
						if (strlen(year) == 1 && isNumber(year)) {
							break;
						} else {
							printCentered("错误: 格式不正确！请修正输入   ", 2);
							Sleep(1000);
							clearLine();
						}
					}
					modifyPersonalInfo(userinfo, atoi(year), nowUser.phone);
					system("cls");
					printCentered("修改完成", 0);
					Sleep(2000);
				} else {
					if (choice == '2') {
						system("cls");
						while (1) {
							getInputCentered("请输入当前手机号:            \b\b\b\b\b\b\b\b\b\b\b", phone, sizeof(phone), 0, 11);
							if (strlen(phone) == 11 && isNumber(phone)) {
								break;
							} else {
								printCentered("错误: 手机号格式不正确！请修正输入     ", 2);
								Sleep(1000);
								clearLine();
							}
						}

						modifyPersonalInfo(userinfo, nowUser.year, phone);
						system("cls");
						printCentered("修改完成", 0);
						Sleep(2000);
					}
				}

				break;

			case 3:  // 退出登录
				printCentered("正在退出登录...", 0);
				Sleep(2000);
				return;  // 返回到主菜单

			default:
				printCentered("无效选择，请重试。", 0);
				break;
		}
	}
}





// 登录逻辑
void loginSystem() {
	char username[50];
	char password[50];
	system("cls");
	getInputCentered("学号: ", username, sizeof(username), 0, 10);
	//system("cls");
	getPasswordCentered("密码: ", password, sizeof(password), 2);

	system("cls");

	//等待小组成员完成登录函数
	int state = login(username, password);
	if (state) {
		printCentered(strcat(username, " 登录成功！"), 0);
		Sleep(2000);  // 停留 2 秒后返回主菜单
		showUserOptions();
	} else {
		printCentered("登录失败，请检查用户名或密码", 0);
		Sleep(2000);  // 停留 2 秒后返回主菜单
	}

}

// 注册逻辑
void registerSystem() {
	char username[50];
	char password[50];
	char year[50];
	char phone[50];
	system("cls");


	while (1) {
		getInputCentered("                    请输入学号:           \b\b\b\b\b\b\b\b\b\b", username, sizeof(username), -5, 10);
		if (strlen(username) == 10 && isNumber(username)) {
			break;
		} else {
			printCentered("          错误: 格式不正确！请修正输入   ", -3);
			Sleep(1000);
			clearLine();
		}
	}
//	system("cls");
	getPasswordCentered("请输入密码: ", password, sizeof(password), -2);
		while (1) {
			getInputCentered("                    请输入年级:           \b\b\b\b\b\b\b\b\b\b", year, sizeof(year), 1, 1);
			if (strlen(year) == 1 && isNumber(year)) {
				break;
			} else {
				printCentered("          错误: 格式不正确！请修正输入   ", 3);
				Sleep(1000);
				clearLine();
			}
		}
//	getInputCentered("请输入年级: ", year, sizeof(year), 1, 1);
	while (1) {
		getInputCentered("                    请输入手机:           \b\b\b\b\b\b\b\b\b\b", phone, sizeof(phone), 4, 11);
		if (strlen(phone) == 11 && isNumber(phone)) {
			break;
		} else {
			printCentered("          错误: 格式不正确！请修正输入   ", 6);
			Sleep(1000);
			clearLine();
		}
	}
	//getInputCentered("请输入手机: ", phone, sizeof(phone), 4, 11);

	system("cls");
	//等待小组成员完成注册函数
	int state = registerUser(username, password, atoi(year), phone);
	if (state)
		printCentered("注册成功！", 0);
	else
		printCentered("注册失败，该用户名已经被注册过了", 0);
	Sleep(2000);  // 停留 2 秒后返回主菜单
}

// 管理员登录逻辑
void adminLoginSystem() {
	char adminUsername[50];
	char adminPassword[50];

	system("cls");
	//printCentered("管理员登录", -4);

	getInputCentered("请输入管理员账号: ", adminUsername, sizeof(adminUsername), 0, 20);
	//system("cls");
	getPasswordCentered("请输入管理员密码: ", adminPassword, sizeof(adminPassword), 2);
	system("cls");

	if (strcmp(adminUsername, "admin") == 0 && strcmp(adminPassword, "123456") == 0) {
		printCentered("管理员登录成功！", 0);
		Sleep(2000);  // 停留 2 秒后返回主菜单
		// 创建窗口线程
		HANDLE hThread = CreateThread(NULL, // 默认安全属性
		                              0, // 默认堆栈大小
		                              WindowThread, // 线程函数
		                              NULL, // 无参数传递
		                              0, // 默认创建标志
		                              NULL // 不需要线程 ID
		                             );

		if (hThread == NULL) {
			printf("无法创建窗口线程\n");
			return;
		}
		showAdminOptions();

	} else {
		printCentered("管理员登录失败，请检查用户名或密码", 0);
		Sleep(2000);  // 停留 2 秒后返回主菜单
	}
}
