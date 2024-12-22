

#include <windows.h>
#include <stdio.h>
extern User nowUser;
int flok = 1; //�����������Ƿ���ѯ��
int refresh = 0; //�Ƿ�ˢ�£�
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
#define BUFFER_SIZE 1024//Ӧ�ù���

/*
Funname: ͼ�������߻�ȡ
Author: Sunight
Description: ͨ������curl���-L�����ض�����ָ�����ݺ��ȡ�ַ�������
*/
void fetch_and_extract_description(const char *book_name) {
	char command[BUFFER_SIZE];
	char line[BUFFER_SIZE];
	char *start, *end;
	FILE *pipe;

// ���� curl �����ȡ�ٶȰٿ����ݣ�-s�����޹����
	snprintf(command, sizeof(command),
	         "curl -s -L \"https://baike.baidu.com/search/word?word=%s\"", book_name);

	printf("Https Getting...\n");

// ʹ�� popen ִ�� curl �����ȡ���
	pipe = popen(command, "r");
	if (!pipe) {
		perror("�޷�ִ������");
		return;
	}

	while (fgets(line, sizeof(line), pipe)) {
		start = strstr(line, "\"description\":\"");
		if (start) {
// �ƶ�ָ�뵽�ı����ݵĿ�ͷ
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


// ȫ�ֱ���
HINSTANCE hInst;

// ���ڹ��̺�������
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// �����Ͱ�ť���
HWND hEditTitle, hEditAuthor, hEditPress, hEditStock, hButtonWrite;

// ����û����벢��ӡ����ʾ
void ProcessInput(HWND hwnd) {
	char title[100], author[100], press[100], stockStr[20];
	int  stock;

// ��ȡ�û�����
	GetWindowText(hEditTitle, title, sizeof(title));
	GetWindowText(hEditAuthor, author, sizeof(author));
	GetWindowText(hEditPress, press, sizeof(press));
	GetWindowText(hEditStock, stockStr, sizeof(stockStr));

// ����Ƿ�Ϊ��
	if (strlen(title) == 0 || strlen(author) == 0 || strlen(press) == 0 || strlen(stockStr) == 0) {
		MessageBox(hwnd, TEXT("��������������Ϣ��"), TEXT("����"), MB_OK | MB_ICONERROR);
		return;
	}

// ת�����Ϊ����
	stock = atoi(stockStr);
	addBook(title, author, press, stock);
// ��ʾ��������
	char message[500];
	snprintf(message, sizeof(message),
	         "ͼ����: %s\n����: %s\n������: %s\n���: %d",
	         title, author, press,  stock);
	MessageBox(hwnd, message, TEXT("��Ϣ��д��"), MB_OK | MB_ICONINFORMATION);
	refresh = 1;
	//flok = 0;
//	drawAdminGrid(book, bookCount,1);
}

// �����̺߳���
DWORD WINAPI WindowThread(LPVOID lpParam) {
	WNDCLASS wc = {0};
	wc.lpfnWndProc = WndProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = TEXT("BookAddTool");
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	if (!RegisterClass(&wc)) {
		printf("����ע��ʧ��\n");
		return 1;
	}

	HWND hwnd = CreateWindow(
	                wc.lpszClassName,
	                TEXT("ͼ����ӹ���"),
	                WS_OVERLAPPEDWINDOW,
	                CW_USEDEFAULT, CW_USEDEFAULT, 400, 400,
	                NULL, NULL, wc.hInstance, NULL);

	if (!hwnd) {
		printf("���ڴ���ʧ��\n");
		return 1;
	}

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

// ��Ϣѭ��
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

// ���ڹ��̺���
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_CREATE:
// ���������ͱ�ǩ
			CreateWindow(TEXT("STATIC"), TEXT("ͼ����:"),
			             WS_CHILD | WS_VISIBLE,
			             50, 30, 80, 25,
			             hwnd, NULL, hInst, NULL);
			hEditTitle = CreateWindow(TEXT("EDIT"), TEXT(""),
			                          WS_CHILD | WS_VISIBLE | WS_BORDER,
			                          140, 30, 200, 25,
			                          hwnd, NULL, hInst, NULL);

			CreateWindow(TEXT("STATIC"), TEXT("����:"),
			             WS_CHILD | WS_VISIBLE,
			             50, 70, 80, 25,
			             hwnd, NULL, hInst, NULL);
			hEditAuthor = CreateWindow(TEXT("EDIT"), TEXT(""),
			                           WS_CHILD | WS_VISIBLE | WS_BORDER,
			                           140, 70, 200, 25,
			                           hwnd, NULL, hInst, NULL);

			CreateWindow(TEXT("STATIC"), TEXT("������:"),
			             WS_CHILD | WS_VISIBLE,
			             50, 110, 80, 25,
			             hwnd, NULL, hInst, NULL);
			hEditPress = CreateWindow(TEXT("EDIT"), TEXT(""),
			                          WS_CHILD | WS_VISIBLE | WS_BORDER,
			                          140, 110, 200, 25,
			                          hwnd, NULL, hInst, NULL);

			CreateWindow(TEXT("STATIC"), TEXT("�������:"),
			             WS_CHILD | WS_VISIBLE,
			             50, 150, 80, 25,
			             hwnd, NULL, hInst, NULL);
			hEditStock = CreateWindow(TEXT("EDIT"), TEXT(""),
			                          WS_CHILD | WS_VISIBLE | WS_BORDER,
			                          140, 150, 200, 25,
			                          hwnd, NULL, hInst, NULL);

			// ������ť
			hButtonWrite = CreateWindow(TEXT("BUTTON"), TEXT("д�����"),
			                            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			                            140, 190, 100, 30,
			                            hwnd, (HMENU)1, hInst, NULL);
			break;

		case WM_COMMAND:
			if (LOWORD(wParam) == 1) { // ��ť����¼�
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

// �˵�ѡ����ʾ�Ϳ���
int menuControl(const char *options[], int optionCount) {
	int currentSelection = 0;
	while (1) {
		system("cls");
		// ��ӡ�������
		printCentered("======== ��ӭʹ��ͼ�����ϵͳ ========", -4);
		// ��ӡ�˵�ѡ�����
		for (int i = 0; i < optionCount; i++) {
			int yOffset = i * 2;  // ÿ���˵����� 2 ��
			if (i == currentSelection) {
				char highlightedOption[100];
				sprintf(highlightedOption, "> %s <", options[i]);  // ǿ����ǰѡ��
				printCentered(highlightedOption, yOffset);
			} else {
				printCentered(options[i], yOffset);
			}
		}
		printCentered("ʹ�����·����ѡ�񣬻س���ȷ�ϡ�", optionCount * 2 + 1);
		int ch = _getch();
		if (ch == 224) {  // ��ⷽ���
			ch = _getch();
			if (ch == 72) {  // ��
				currentSelection = (currentSelection - 1 + optionCount) % optionCount;
			} else if (ch == 80) {  // ��
				currentSelection = (currentSelection + 1) % optionCount;
			}
		} else if (ch == 13) {  // �س�
			return currentSelection;
		}
	}
}

void getColor(int value, char *color, size_t length) {
	//Sunight����ϸѡ����ɫ
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

	int r = rgbColors[value % 9][0]; // ��
	int g = rgbColors[value % 9][1]; // ��
	int b = rgbColors[value % 9][2]; // ��

	snprintf(color, length, "\033[38;2;255;255;255;48;2;%d;%d;%dm", r, g, b);
}
int custom_ceil(double value) {
	int intPart = (int)value;  // ��ȡ��������
	if (value > intPart) {
		return intPart + 1;  // �����С�����֣�������ȡ��
	}
	return intPart;  // û��С�����֣�ֱ�ӷ�����������
}
#define SIZE 4 // �����С��4x4
extern int ctrlPressed;
void drawAdminGrid_s(Book books[], int bookCount, int page) {
	int tmp = bookCount;
	bookCount = bookCount - (page - 1) * SIZE * SIZE;
	if (bookCount > SIZE * SIZE)bookCount = SIZE * SIZE;
	printf("\n��ҳͼ��������%d ", bookCount);
	int exit = 0;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // ����̨���
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	// ��ȡ����̨��С
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	int consoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

	int cellWidth = consoleWidth / SIZE; // ÿ�����εĿ��
	int cellHeight = consoleHeight / SIZE - 1; // ÿ�����εĸ߶�
	int marginX = (consoleWidth - cellWidth * SIZE) / 2; // �������
	char color[40], reset[] = "\033[m";
	// ���ӷ�Χ��¼
	SMALL_RECT cellRanges[SIZE * SIZE];

	printf("��ǰҳ����%d/%d  ���˵���s-�����鼮 n-��һҳ p-��һҳ q-�˳�\n\n", page, custom_ceil((double)tmp / (SIZE * SIZE)));
	for (int y = 0; y < SIZE; y++) {
		for (int x = 0; x < SIZE; x++) {
			int bookIndex =  y * SIZE + x;
			if (bookIndex >= bookCount) break;

			// �����������
			int top = y * cellHeight + 2; // 2 �Ǳ���ռ�õ�����
			int bottom = top + cellHeight - 1;
			int left = marginX + x * cellWidth;
			int right = left + cellWidth - 1;

			// ������ӷ�Χ
			cellRanges[bookIndex].Top = top;
			cellRanges[bookIndex].Bottom = bottom;
			cellRanges[bookIndex].Left = left;
			cellRanges[bookIndex].Right = right;
		}
	}

	for (int y = 0; y < SIZE; y++) {
		for (int row = 0; row < cellHeight; row++) {
			printf("%*s", marginX, "");  // �������
			for (int x = 0; x < SIZE; x++) {
				int bookIndex =  (y + (page - 1) * SIZE) * SIZE + x;
				//printf("%d",bookIndex);
				if (bookIndex - (page - 1) * SIZE * SIZE >= bookCount) break;
				getColor(bookIndex, color, 40);  // ��ȡ��ɫ
				if (row == 0 || row == cellHeight - 1) {
					// ���ƾ��α߿�
					printf("%s%*s%s", color, cellWidth, "", reset);
				} else {
					// ������ʾͼ����Ϣ
					int contentRow = row ;  // �����߿���
					char content[100] = "";
					if (contentRow == 0) {
						snprintf(content, sizeof(content), "ID: %d", books[bookIndex].id);
					} else if (contentRow == 1) {
						snprintf(content, sizeof(content), "����: %s", books[bookIndex].title);
					} else if (contentRow == 2) {
						snprintf(content, sizeof(content), "����: %s", books[bookIndex].author);
					} else if (contentRow == 3) {
						snprintf(content, sizeof(content), "������: %s", books[bookIndex].press);
					} else if (contentRow == 4) {
						snprintf(content, sizeof(content), "���: %d", books[bookIndex].available);
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
//	printf("Tips����ͼ����Ŀʹ�������������Ҽ��ɽ��п����޸ģ��Թ���µ���Ŀʹ��delete����ɾ����ͼ��");
	// ��������¼�
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	SetConsoleMode(hInput, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

	INPUT_RECORD inputRecord;
	DWORD events;
	COORD mousePos = {0, 0}; // ���ڼ�¼��ǰ���λ��

	while ( flok && !refresh) {
		// �������ͼ����¼�
		if (ReadConsoleInput(hInput, &inputRecord, 1, &events)) {
			if (inputRecord.EventType == MOUSE_EVENT) {
				MOUSE_EVENT_RECORD mouseEvent = inputRecord.Event.MouseEvent;
				mousePos = mouseEvent.dwMousePosition;
				// ���û�й��ֻ���갴ť�¼���ֱ�Ӻ��ԣ�������ˢ�£�
				if (!(mouseEvent.dwEventFlags == MOUSE_WHEELED ||
				        mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED ||
				        mouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED)) {
					continue;
				}

				// ��ȡ�����λ��
				COORD clickPos = mouseEvent.dwMousePosition;

				// �������ӣ��ҵ�������ڵĸ���
				for (int i = 0; i < bookCount; i++) {
					int index = i + SIZE * SIZE * (page - 1);
					SMALL_RECT range = cellRanges[i];
					if (clickPos.Y >= range.Top && clickPos.Y <= range.Bottom &&
					        clickPos.X >= range.Left && clickPos.X <= range.Right) {

						int changed = 0; // ��־�Ƿ������仯

						// �������¼�
						if (mouseEvent.dwEventFlags == MOUSE_WHEELED) {
							int delta = (SHORT)HIWORD(mouseEvent.dwButtonState) > 0 ? 1 : -1; // ���ַ���
							books[index].available += delta;
							updateAva(books[index], delta);
							if (books[index].available < 0) {
								updateAva2(books[index], 0);
								books[index].available = 0; // ��ֹ���Ϊ����
							}
							changed = 1; // ��ǿ�����޸�
						}

						// �����������¼�
						if (mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
							books[index].available += 50; // ���ӿ�� 50
							updateAva(books[index], 50);
							changed = 1; // ��ǿ�����޸�
						}

						// ����Ҽ������¼�
						if (mouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) {
							books[index].available -= 50; // ���ٿ�� 50
							updateAva(books[index], -50);
							if (books[index].available < 0) {
								updateAva2(books[index], 0);
								books[index].available = 0; // ��ֹ���Ϊ����
							}
							changed = 1; // ��ǿ�����޸�
						}

						// �����淢���仯��������ʾ
						if (changed) {
							system("cls"); // ����
							drawAdminGrid_s(books, bookCount, page);
						}

						break;
					}
				}
			}
			// �������¼�
			else if (inputRecord.EventType == KEY_EVENT) {
				KEY_EVENT_RECORD keyEvent = inputRecord.Event.KeyEvent;
				if (keyEvent.bKeyDown) {  // �������������
					char key = keyEvent.uChar.AsciiChar;
					if (key == 's' || key == 'S') {
						clearLine();
						printf("�������� 's' ���������鼮");
						Sleep(200);
						clearLine();
						system("cls");
						char keyword[50] = "";
						getInputCentered_s("������ؼ��ʣ�Ϊ�ձ�ʾȫ������", keyword, sizeof(keyword), 0);
						if (strlen(keyword) == 0) {
//							printCentered("����Ϊ��");
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
								printCentered("δ�ҵ�����鼮", 2);
								Sleep(1000);
							} else {
								system("cls");
								drawAdminGrid_s(searched_books, book_count, 1);
							}
						}
						break;
					} else if (key == 'q' || key == 'Q') {
						clearLine();
						printf("�������� 'q' �����˳�ϵͳ");
						Sleep(500);
						exit = 1;
						flok = 0;
						system("cls"); // ����
						break;
					} else if (key == 'n' || key == 'N') {
						if (bookCount < SIZE * SIZE) {
							clearLine();
							printf("�Ѿ������һҳ��");
							Sleep(700);
							clearLine();
						} else {
							clearLine();
							printf("�������� 'n' ������һҳ");
							Sleep(200);
							system("cls");
							drawAdminGrid(books, bookCount, page + 1);
						}

					} else if (key == 'p' || key == 'P') {
						if (page == 1) {
							clearLine();
							printf("�Ѿ��ǵ�һҳ��");
							Sleep(700);
							clearLine();
						} else {
							clearLine();
							printf("�������� 'p' ������һҳ");
							Sleep(200);
							system("cls");
							drawAdminGrid(books, bookCount, page - 1);
						}
					} else if (keyEvent.wVirtualKeyCode == VK_DELETE) {
// Delete ��������


// �������ӣ��ҵ����ͣ���ĸ���
						for (int i = 0; i < bookCount; i++) {
							int index = i + SIZE * SIZE * (page - 1);
							SMALL_RECT range = cellRanges[i];
							if (mousePos.Y >= range.Top && mousePos.Y <= range.Bottom &&
							        mousePos.X >= range.Left && mousePos.X <= range.Right) {
								clearLine();
								printf("ɾ���鼮: %s", books[index].title);

// ɾ���鼮�߼�
								deleteBook(books[index].title);

// ������ʾ
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
		system("cls"); // ����
		drawAdminGrid_s(books, bookCount, page);
	}

}
//���ƹ���Ա����
void drawAdminGrid(Book books[], int bookCount, int page) {
	bookCount = getBooksCount() - (page - 1) * SIZE * SIZE;
	if (bookCount > SIZE * SIZE)bookCount = SIZE * SIZE;
	printf("\n��ҳͼ��������%d ", bookCount);
	int exit = 0;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // ����̨���
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	// ��ȡ����̨��С
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	int consoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

	int cellWidth = consoleWidth / SIZE; // ÿ�����εĿ��
	int cellHeight = consoleHeight / SIZE - 1; // ÿ�����εĸ߶�
	int marginX = (consoleWidth - cellWidth * SIZE) / 2; // �������
	char color[40], reset[] = "\033[m";
	// ���ӷ�Χ��¼
	SMALL_RECT cellRanges[SIZE * SIZE];
	SetTextColor1(14);
	printf("��ǰҳ����%d/%d  ���˵���s-�����鼮 n-��һҳ p-��һҳ q-�˳�\n\n", page, custom_ceil((double)getBooksCount() / (SIZE * SIZE)));
	for (int y = 0; y < SIZE; y++) {
		for (int x = 0; x < SIZE; x++) {
			int bookIndex =  y * SIZE + x;
			if (bookIndex >= bookCount) break;

			// �����������
			int top = y * cellHeight + 2; // 2 �Ǳ���ռ�õ�����
			int bottom = top + cellHeight - 1;
			int left = marginX + x * cellWidth;
			int right = left + cellWidth - 1;

			// ������ӷ�Χ
			cellRanges[bookIndex].Top = top;
			cellRanges[bookIndex].Bottom = bottom;
			cellRanges[bookIndex].Left = left;
			cellRanges[bookIndex].Right = right;
		}
	}

	for (int y = 0; y < SIZE; y++) {
		for (int row = 0; row < cellHeight; row++) {
			printf("%*s", marginX, "");  // �������
			for (int x = 0; x < SIZE; x++) {
				int bookIndex =  (y + (page - 1) * SIZE) * SIZE + x;
				//printf("%d",bookIndex);
				if (bookIndex - (page - 1) * SIZE * SIZE >= bookCount) break;
				getColor(bookIndex, color, 40);  // ��ȡ��ɫ
				if (row == 0 || row == cellHeight - 1) {
					// ���ƾ��α߿�
					printf("%s%*s%s", color, cellWidth, "", reset);
				} else {
					// ������ʾͼ����Ϣ
					int contentRow = row ;  // �����߿���
					char content[100] = "";
					SetTextColor1(14);
					if (contentRow == 0) {
						snprintf(content, sizeof(content), "ID: %d", books[bookIndex].id);
					} else if (contentRow == 1) {
						snprintf(content, sizeof(content), "����: %s", books[bookIndex].title);
					} else if (contentRow == 2) {
						snprintf(content, sizeof(content), "����: %s", books[bookIndex].author);
					} else if (contentRow == 3) {
						snprintf(content, sizeof(content), "������: %s", books[bookIndex].press);
					} else if (contentRow == 4) {
						snprintf(content, sizeof(content), "���: %d", books[bookIndex].available);
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
	printf("Tips����ͼ����Ŀʹ�������֣���סShift�������Ҽ��ɽ��п����޸ģ��Թ���µ���Ŀʹ��delete����ɾ����ͼ��");

	// ��������¼�
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	SetConsoleMode(hInput, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

	INPUT_RECORD inputRecord;
	DWORD events;
	COORD mousePos = {0, 0}; // ���ڼ�¼��ǰ���λ��

	while ( flok && !refresh) {
		// �������ͼ����¼�
		if (ReadConsoleInput(hInput, &inputRecord, 1, &events)) {
			if (inputRecord.EventType == MOUSE_EVENT) {
				MOUSE_EVENT_RECORD mouseEvent = inputRecord.Event.MouseEvent;
				mousePos = mouseEvent.dwMousePosition;

				// ���û�й��ֻ���갴ť�¼���ֱ�Ӻ��ԣ�������ˢ�£�
				if (!(mouseEvent.dwEventFlags == MOUSE_WHEELED ||
				        mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED ||
				        mouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED)) {
					continue;
				}

				// ��ȡ�����λ��
				COORD clickPos = mouseEvent.dwMousePosition;

				// �������ӣ��ҵ�������ڵĸ���
				for (int i = 0; i < bookCount; i++) {
					int index = i + SIZE * SIZE * (page - 1);
					SMALL_RECT range = cellRanges[i];
					if (clickPos.Y >= range.Top && clickPos.Y <= range.Bottom &&
					        clickPos.X >= range.Left && clickPos.X <= range.Right) {

						int changed = 0; // ��־�Ƿ������仯

						// �������¼�
						if (mouseEvent.dwEventFlags == MOUSE_WHEELED) {
							int delta = (SHORT)HIWORD(mouseEvent.dwButtonState) > 0 ? 1 : -1; // ���ַ���
							if (ctrlPressed) {
								books[index].available += delta;
								updateAva(books[index], delta);
								if (books[index].available < 0) {
									updateAva2(books[index], 0);
									books[index].available = 0; // ��ֹ���Ϊ����
								}
								changed = 1; // ��ǿ�����޸�
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

						// �����������¼�
						if (mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
							books[index].available += 50; // ���ӿ�� 50
							updateAva(books[index], 50);
							changed = 1; // ��ǿ�����޸�
						}

						// ����Ҽ������¼�
						if (mouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) {
							books[index].available -= 50; // ���ٿ�� 50
							updateAva(books[index], -50);
							if (books[index].available < 0) {
								updateAva2(books[index], 0);
								books[index].available = 0; // ��ֹ���Ϊ����
							}
							changed = 1; // ��ǿ�����޸�
						}

						// �����淢���仯��������ʾ
						if (changed) {
							system("cls");
							//�ͷŶ�̬������ڴ棬��ֹй¶
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
			// �������¼�
			else if (inputRecord.EventType == KEY_EVENT) {
				KEY_EVENT_RECORD keyEvent = inputRecord.Event.KeyEvent;
				if (keyEvent.bKeyDown) {  // �������������
					char key = keyEvent.uChar.AsciiChar;
					if (key == 's' || key == 'S') {
//						printf("�������� 's' ���������鼮����\n");
//						Sleep(200);
//						clearLine();
						system("cls");
						char keyword[50] = "";
						getInputCentered_s("������ؼ��ʣ�Ϊ�ձ�ʾȫ������", keyword, sizeof(keyword), 0);
						if (strlen(keyword) == 0) {
//							printCentered("����Ϊ��");
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
								printCentered("δ�ҵ�����鼮", 2);
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
//						printf("�������� 'q' �����˳�ϵͳ\n");
//						Sleep(500);
						exit = 1;
						flok = 0;
						system("cls"); // ����
						break;
					} else if (key == 'n' || key == 'N') {
						if (bookCount < SIZE * SIZE) {
//							printf("�Ѿ������һҳ��");
//							Sleep(700);
//							clearLine();
						} else {
//							clearLine();
//							printf("�������� 'n' ������һҳ\n");
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
//							printf("�Ѿ��ǵ�һҳ��");
//							Sleep(700);
//							clearLine();
						} else {
//							clearLine();
//							printf("�������� 'p' ������һҳ\n");
//							Sleep(200);
							system("cls");
							if (books != NULL) {
								free(books);
								books = NULL;
							}
							drawAdminGrid(allBooks(), bookCount, page - 1);
						}

					} else if (keyEvent.wVirtualKeyCode == VK_DELETE) {
// Delete ��������


// �������ӣ��ҵ����ͣ���ĸ���
						for (int i = 0; i < bookCount; i++) {
							int index = i + SIZE * SIZE * (page - 1);
							SMALL_RECT range = cellRanges[i];
							if (mousePos.Y >= range.Top && mousePos.Y <= range.Bottom &&
							        mousePos.X >= range.Left && mousePos.X <= range.Right) {
//								clearLine();
//								printf("ɾ���鼮: %s\n", books[index].title);

// ɾ���鼮�߼�
								deleteBook(books[index].title);

// ������ʾ
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
		system("cls"); // ����
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
	printf("\n��ҳͼ��������%d ", bookCount);
	int exit = 0;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // ����̨���
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	// ��ȡ����̨��С
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	int consoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

	int cellWidth = consoleWidth / SIZE; // ÿ�����εĿ��
	int cellHeight = consoleHeight / SIZE - 1; // ÿ�����εĸ߶�
	int marginX = (consoleWidth - cellWidth * SIZE) / 2; // �������
	char color[40], reset[] = "\033[m";
	// ���ӷ�Χ��¼
	SMALL_RECT cellRanges[SIZE * SIZE];
	printf("��ǰҳ����%d/%d  ����ͼ�鼴�ɽ��ģ����˵���n-��һҳ p-��һҳ q-�˳�\n\n", page, custom_ceil((double)tmp / (SIZE * SIZE)));
	for (int y = 0; y < SIZE; y++) {
		for (int x = 0; x < SIZE; x++) {
			int bookIndex = y * SIZE + x;
			if (bookIndex >= bookCount) break;

			// �����������
			int top = y * cellHeight + 2; // 2 �Ǳ���ռ�õ�����
			int bottom = top + cellHeight - 1;
			int left = marginX + x * cellWidth;
			int right = left + cellWidth - 1;

			// ������ӷ�Χ
			cellRanges[bookIndex].Top = top;
			cellRanges[bookIndex].Bottom = bottom;
			cellRanges[bookIndex].Left = left;
			cellRanges[bookIndex].Right = right;
		}
	}

	for (int y = 0; y < SIZE; y++) {
		for (int row = 0; row < cellHeight; row++) {
			printf("%*s", marginX, "");  // �������
			for (int x = 0; x < SIZE; x++) {
				int bookIndex =  (y + (page - 1) * SIZE) * SIZE + x;
				//printf("%d",bookIndex);
				if (bookIndex - (page - 1) * SIZE * SIZE >= bookCount) break;
				getColor(bookIndex, color, 40);  // ��ȡ��ɫ
				if (row == 0 || row == cellHeight - 1) {
					// ���ƾ��α߿�
					printf("%s%*s%s", color, cellWidth, "", reset);
				} else {
					// ������ʾͼ����Ϣ
					int contentRow = row ;  // �����߿���
					char content[100] = "";
					if (contentRow == 0) {
						snprintf(content, sizeof(content), "ID: %d", books[bookIndex].id);
					} else if (contentRow == 1) {
						snprintf(content, sizeof(content), "����: %s", books[bookIndex].title);
					} else if (contentRow == 2) {
						snprintf(content, sizeof(content), "����: %s", books[bookIndex].author);
					} else if (contentRow == 3) {
						snprintf(content, sizeof(content), "������: %s", books[bookIndex].press);
					} else if (contentRow == 4) {
						snprintf(content, sizeof(content), "���: %d", books[bookIndex].available);
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
	// ��������¼�
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	SetConsoleMode(hInput, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

	INPUT_RECORD inputRecord;
	DWORD events;

	while (flok && !refresh) {
		// �������ͼ����¼�
		if (ReadConsoleInput(hInput, &inputRecord, 1, &events)) {
			if (inputRecord.EventType == MOUSE_EVENT) {
				MOUSE_EVENT_RECORD mouseEvent = inputRecord.Event.MouseEvent;

				// ���û�й��ֻ���갴ť�¼���ֱ�Ӻ��ԣ�������ˢ�£�
				if (!(mouseEvent.dwEventFlags == MOUSE_WHEELED ||
				        mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED ||
				        mouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED)) {
					continue;
				}

				// �������¼�
				if (mouseEvent.dwEventFlags == MOUSE_WHEELED) {
					int delta = (SHORT)HIWORD(mouseEvent.dwButtonState) > 0 ? 1 : -1; // ���ַ���
					if (page - delta > 0 && page - delta <= custom_ceil((double)tmp / (SIZE * SIZE))) {
						system("cls");
						drawBorrowGrid_s(books, bookCount, page - delta);
					}
				}
				// �����������¼�
				if (mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
					// ��ȡ�����λ��
					COORD clickPos = mouseEvent.dwMousePosition;

					// �������ӣ��ҵ�����ĸ���
					for (int i = 0; i < bookCount; i++) {
						SMALL_RECT range = cellRanges[i];
						if (clickPos.Y >= range.Top && clickPos.Y <= range.Bottom &&
						        clickPos.X >= range.Left && clickPos.X <= range.Right) {
							int index = i + SIZE * SIZE * (page - 1);
							if (books[index].available) {
								Sleep(100);
								char message[] = "�Ƿ���Ĵ��鼮��\n\n";
								char *title[128];

								sprintf(title, "%s  ʣ��������%d", books[index].title, books[index].available);
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
								    "��治��",
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
			// �������¼�
			else if (inputRecord.EventType == KEY_EVENT) {
				KEY_EVENT_RECORD keyEvent = inputRecord.Event.KeyEvent;

				if (keyEvent.bKeyDown) {  // �������������
					char key = keyEvent.uChar.AsciiChar;
					if (key == 's' || key == 'S') {
						printf("�������� 's' ���������鼮");
						Sleep(200);
						clearLine();
						system("cls");
						char keyword[50] = "";
						getInputCentered_s("������ؼ��ʣ�Ϊ�ձ�ʾȫ������", keyword, sizeof(keyword), 0);
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
								printCentered("δ�ҵ�����鼮", 2);
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
						printf("�������� 'q' �����˳�ϵͳ");
						Sleep(500);
						exit = 1;
						flok = 0;
						system("cls"); // ����
						break;
					} else if (key == 'n' || key == 'N') {
						if (bookCount < SIZE * SIZE) {
							printf("�Ѿ������һҳ��");
							Sleep(700);
							clearLine();
						} else {
							printf("�������� 'n' ������һҳ");
							Sleep(200);
							system("cls");
							drawBorrowGrid_s(books, bookCount, page + 1);
						}

					} else if (key == 'p' || key == 'P') {
						if (page == 1) {
							printf("�Ѿ��ǵ�һҳ��");
							Sleep(700);
							clearLine();
						} else {
							printf("�������� 'p' ������һҳ");
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
		system("cls"); // ����
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
	printf("\n��ҳͼ��������%d ", bookCount);
	int exit = 0;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // ����̨���
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	// ��ȡ����̨��С
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	int consoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

	int cellWidth = consoleWidth / SIZE; // ÿ�����εĿ��
	int cellHeight = consoleHeight / SIZE - 1; // ÿ�����εĸ߶�
	int marginX = (consoleWidth - cellWidth * SIZE) / 2; // �������
	char color[40], reset[] = "\033[m";
	// ���ӷ�Χ��¼
	SMALL_RECT cellRanges[SIZE * SIZE];
	printf("��ǰҳ����%d/%d  ����ͼ�鼴�ɽ��ģ����˵���s-�����鼮 n-��һҳ p-��һҳ q-�˳�\n\n", page, custom_ceil((double)getBooksCount() / (SIZE * SIZE)));
	for (int y = 0; y < SIZE; y++) {
		for (int x = 0; x < SIZE; x++) {
			int bookIndex = y * SIZE + x;
			if (bookIndex >= bookCount) break;

			// �����������
			int top = y * cellHeight + 2; // 2 �Ǳ���ռ�õ�����
			int bottom = top + cellHeight - 1;
			int left = marginX + x * cellWidth;
			int right = left + cellWidth - 1;

			// ������ӷ�Χ
			cellRanges[bookIndex].Top = top;
			cellRanges[bookIndex].Bottom = bottom;
			cellRanges[bookIndex].Left = left;
			cellRanges[bookIndex].Right = right;
		}
	}

	for (int y = 0; y < SIZE; y++) {
		for (int row = 0; row < cellHeight; row++) {
			printf("%*s", marginX, "");  // �������
			for (int x = 0; x < SIZE; x++) {
				int bookIndex =  (y + (page - 1) * SIZE) * SIZE + x;
				//printf("%d",bookIndex);
				if (bookIndex - (page - 1) * SIZE * SIZE >= bookCount) break;
				getColor(bookIndex, color, 40);  // ��ȡ��ɫ
				if (row == 0 || row == cellHeight - 1) {
					// ���ƾ��α߿�
					printf("%s%*s%s", color, cellWidth, "", reset);
				} else {
					// ������ʾͼ����Ϣ
					int contentRow = row ;  // �����߿���
					char content[100] = "";
					if (contentRow == 0) {
						snprintf(content, sizeof(content), "ID: %d", books[bookIndex].id);
					} else if (contentRow == 1) {
						snprintf(content, sizeof(content), "����: %s", books[bookIndex].title);
					} else if (contentRow == 2) {
						snprintf(content, sizeof(content), "����: %s", books[bookIndex].author);
					} else if (contentRow == 3) {
						snprintf(content, sizeof(content), "������: %s", books[bookIndex].press);
					} else if (contentRow == 4) {
						snprintf(content, sizeof(content), "���: %d", books[bookIndex].available);
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
	// ��������¼�
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	SetConsoleMode(hInput, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

	INPUT_RECORD inputRecord;
	DWORD events;

	while (flok && !refresh) {
		// �������ͼ����¼�
		if (ReadConsoleInput(hInput, &inputRecord, 1, &events)) {
			if (inputRecord.EventType == MOUSE_EVENT) {
				MOUSE_EVENT_RECORD mouseEvent = inputRecord.Event.MouseEvent;
				// ���û�й��ֻ���갴ť�¼���ֱ�Ӻ��ԣ�������ˢ�£�
				if (!(mouseEvent.dwEventFlags == MOUSE_WHEELED ||
				        mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED ||
				        mouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED)) {
					continue;
				}

				// �������¼�
				if (mouseEvent.dwEventFlags == MOUSE_WHEELED) {
					int delta = (SHORT)HIWORD(mouseEvent.dwButtonState) > 0 ? 1 : -1; // ���ַ���
					if (page - delta > 0 && page - delta <= custom_ceil((double)getBooksCount() / (SIZE * SIZE))) {
						system("cls");
						if (books != NULL) {
							free(books);
							books = NULL;
						}
						drawBorrowGrid(allBooks(), bookCount, page - delta);
					}
				}
				// �����������¼�
				if (mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
					// ��ȡ�����λ��
					COORD clickPos = mouseEvent.dwMousePosition;

					// �������ӣ��ҵ�����ĸ���
					for (int i = 0; i < bookCount; i++) {
						SMALL_RECT range = cellRanges[i];
						if (clickPos.Y >= range.Top && clickPos.Y <= range.Bottom &&
						        clickPos.X >= range.Left && clickPos.X <= range.Right) {
							int index = i + SIZE * SIZE * (page - 1);
							if (books[index].available) {
								Sleep(100);
								char message[] = "�Ƿ���Ĵ��鼮��\n\n";
								char *title[128];

								sprintf(title, "%s  ʣ��������%d", books[index].title, books[index].available);
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
								    "��治��",
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
			// �������¼�
			else if (inputRecord.EventType == KEY_EVENT) {
				KEY_EVENT_RECORD keyEvent = inputRecord.Event.KeyEvent;

				if (keyEvent.bKeyDown) {  // �������������
					char key = keyEvent.uChar.AsciiChar;
					if (key == 's' || key == 'S') {
						printf("�������� 's' ���������鼮");
						Sleep(200);
						clearLine();
						system("cls");
						char keyword[50] = "";
						getInputCentered_s("������ؼ��ʣ�Ϊ�ձ�ʾȫ������", keyword, sizeof(keyword), 0);
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
								printCentered("δ�ҵ�����鼮", 2);
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
						printf("�������� 'q' �����˳�ϵͳ");
						Sleep(500);
						exit = 1;
						flok = 0;
						system("cls"); // ����
						break;
					} else if (key == 'n' || key == 'N') {
						if (bookCount < SIZE * SIZE) {
							printf("�Ѿ������һҳ��");
							Sleep(700);
							clearLine();
						} else {
							printf("�������� 'n' ������һҳ");
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
							printf("�Ѿ��ǵ�һҳ��");
							Sleep(700);
							clearLine();
						} else {
							printf("�������� 'p' ������һҳ");
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
		system("cls"); // ����
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
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // ����̨���
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	// ��ȡ����̨��С
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	int consoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

	int cellWidth = consoleWidth / SIZE; // ÿ�����εĿ��
	int cellHeight = consoleHeight / SIZE - 1; // ÿ�����εĸ߶�
	int marginX = (consoleWidth - cellWidth * SIZE) / 2; // �������
	char color[40], reset[] = "\033[m";
	// ���ӷ�Χ��¼
	SMALL_RECT cellRanges[SIZE * SIZE];
	printf("��굥��ͼ�鼴�ɹ黹����������˳�\n\n");
	for (int y = 0; y < SIZE; y++) {
		for (int x = 0; x < SIZE; x++) {
			int bookIndex = y * SIZE + x;
			if (bookIndex >= bookCount) break;

			// �����������
			int top = y * cellHeight + 2; // 2 �Ǳ���ռ�õ�����
			int bottom = top + cellHeight - 1;
			int left = marginX + x * cellWidth;
			int right = left + cellWidth - 1;

			// ������ӷ�Χ
			cellRanges[bookIndex].Top = top;
			cellRanges[bookIndex].Bottom = bottom;
			cellRanges[bookIndex].Left = left;
			cellRanges[bookIndex].Right = right;
		}
	}

	for (int y = 0; y < SIZE; y++) {
		for (int row = 0; row < cellHeight; row++) {
			printf("%*s", marginX, "");  // �������
			for (int x = 0; x < SIZE; x++) {
				int bookIndex = y * SIZE + x;
				//printf("%d",bookIndex);
				if (bookIndex  >= bookCount) break;
				getColor(bookIndex, color, 40);  // ��ȡ��ɫ
				if (row == 0 || row == cellHeight - 1) {
					// ���ƾ��α߿�
					printf("%s%*s%s", color, cellWidth, "", reset);
				} else {
					// ������ʾͼ����Ϣ
					int contentRow = row ;  // �����߿���
					char content[100] = "";
					if (contentRow == 0) {
						snprintf(content, sizeof(content), "ID: %d", books[bookIndex].id);
					} else if (contentRow == 1) {
						snprintf(content, sizeof(content), "����: %s", books[bookIndex].title);
					} else if (contentRow == 2) {
						snprintf(content, sizeof(content), "����: %s", books[bookIndex].author);
					} else if (contentRow == 3) {
						snprintf(content, sizeof(content), "������: %s", books[bookIndex].press);
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
	// ��������¼�
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	SetConsoleMode(hInput, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

	INPUT_RECORD inputRecord;
	DWORD events;

	while (1) {
		// �������ͼ����¼�
		if (ReadConsoleInput(hInput, &inputRecord, 1, &events)) {
			if (inputRecord.EventType == MOUSE_EVENT) {
				MOUSE_EVENT_RECORD mouseEvent = inputRecord.Event.MouseEvent;

				// ���û�й��ֻ���갴ť�¼���ֱ�Ӻ��ԣ�������ˢ�£�
				if (!(
				            mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED ||
				            mouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED)) {
					continue;
				}
				// �����������¼�
				if (mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
					// ��ȡ�����λ��
					COORD clickPos = mouseEvent.dwMousePosition;
					// �������ӣ��ҵ�����ĸ���
					for (int i = 0; i < bookCount; i++) {
						SMALL_RECT range = cellRanges[i];
						if (clickPos.Y >= range.Top && clickPos.Y <= range.Bottom &&
						        clickPos.X >= range.Left && clickPos.X <= range.Right) {
							Sleep(100);
							char message[] = "�Ƿ�黹���鼮��\n\n";
							char *title[128];
							sprintf(title, "%s", books[i].title);
							int result = MessageBox(NULL, // �����ھ����NULL��ʾ�޸����ڣ�
							                        message, // ��ʾ������
							                        title, // ��ʾ�����
							                        MB_OKCANCEL | MB_TOPMOST);
							if (result == IDOK) {
								updateAva(books[i], +1);
								if (removeBorrowedBook(nowUser.username, books[i].title)) {
									MessageBox(
									    NULL, // �����ھ����NULL��ʾ�޸����ڣ�
									    "�黹�ɹ�", // ��ʾ������
									    "��ʾ", // ��ʾ�����
									    MB_OK | MB_TOPMOST // ��ť���ͺ�ͼ������
									);
									system("cls"); // ����
									drawBorrowedGrid();
								}
							}
							break;
						}
					}
				}

				break;
			}
			// �������¼�
			else if (inputRecord.EventType == KEY_EVENT) {
				KEY_EVENT_RECORD keyEvent = inputRecord.Event.KeyEvent;
				if (keyEvent.bKeyDown) {  // �������������
					char key = keyEvent.uChar.AsciiChar;
					if (key == 'q' || key == 'Q') {
						printf("�������� 'q' �����˳�ϵͳ\n");
						Sleep(500);
						exit = 1;
						flok = 0;
						system("cls"); // ����
						break;
					}
				}
			}
		}
	}

}


void showAdminOptions() {
// ʾ��ͼ������
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
	// ��������
	drawAdminGrid(book, bookCount, 1);


}

void showBorrowOptions() {
// ʾ��ͼ������
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
	// ��������
	drawBorrowGrid(book, bookCount, 1);


}
// �ж��ַ����Ƿ�ȫ������
int isNumber(const char* str) {
	for (int i = 0; i < strlen(str); i++) {
		if (!isdigit(str[i])) {
			return 0;  // ����з������ַ������� 0
		}
	}
	return 1;  // �����ַ��������֣����� 1
}
void showUserOptions() {
	const char *userOptions[] = {
		"����ͼ���",
		"�ڽ�ͼ��黹",
		"������Ϣ�޸�",
		"�˳���¼"
	};
	int userOptionCount = sizeof(userOptions) / sizeof(userOptions[0]);

	while (1) {
		int choice = menuControl(userOptions, userOptionCount); // ʹ�����в˵��ؼ�����
		system("cls");
		switch (choice) {
			case 0:  // ͼ�����
				//printCentered("��ѡ����ͼ����Ĺ��ܡ�", 0);
				showBorrowOptions();
				break;
			case 1:  // ͼ�����
				drawBorrowedGrid();

//				//printCentered("���¶�Ӧͼ�����", (i - 2) * 2);
//				Book* blist = get_borrowed_books(nowUser.username);
//				for (int i = 0; i < 5; i++) {
//					char book[100];
//					sprintf(book, "������%s�����ߣ�%s", blist[i].title, blist[i].author);
//					printCentered(book, (i - 2) * 2);
//				}
//				getchar();
				break;

			case 2:  // ������Ϣ�޸�
				printCentered("----������ѡ������˳�----", -2);
				char *userinfo[50];
				char *year[50];
				char *phone[50];
				sprintf(userinfo, "��ǰ�û���%s��������Ӧ�������޸�ָ���", nowUser.username);
				sprintf(year, "\b\b��1���꼶��%d          ", nowUser.year);
				sprintf(phone, "��2����ϵ��ʽ��%s\n", nowUser.phone);
				printCentered(userinfo, -4);
				printCentered(year, 0);
				printCentered(phone, 2);
				char choice = _getch();
				if (choice == '1') {
					system("cls");
					while (1) {
						getInputCentered("�����뵱ǰ�꼶:  \b", year, sizeof(year), 0, 1);
						if (strlen(year) == 1 && isNumber(year)) {
							break;
						} else {
							printCentered("����: ��ʽ����ȷ������������   ", 2);
							Sleep(1000);
							clearLine();
						}
					}
					modifyPersonalInfo(userinfo, atoi(year), nowUser.phone);
					system("cls");
					printCentered("�޸����", 0);
					Sleep(2000);
				} else {
					if (choice == '2') {
						system("cls");
						while (1) {
							getInputCentered("�����뵱ǰ�ֻ���:            \b\b\b\b\b\b\b\b\b\b\b", phone, sizeof(phone), 0, 11);
							if (strlen(phone) == 11 && isNumber(phone)) {
								break;
							} else {
								printCentered("����: �ֻ��Ÿ�ʽ����ȷ������������     ", 2);
								Sleep(1000);
								clearLine();
							}
						}

						modifyPersonalInfo(userinfo, nowUser.year, phone);
						system("cls");
						printCentered("�޸����", 0);
						Sleep(2000);
					}
				}

				break;

			case 3:  // �˳���¼
				printCentered("�����˳���¼...", 0);
				Sleep(2000);
				return;  // ���ص����˵�

			default:
				printCentered("��Чѡ�������ԡ�", 0);
				break;
		}
	}
}





// ��¼�߼�
void loginSystem() {
	char username[50];
	char password[50];
	system("cls");
	getInputCentered("ѧ��: ", username, sizeof(username), 0, 10);
	//system("cls");
	getPasswordCentered("����: ", password, sizeof(password), 2);

	system("cls");

	//�ȴ�С���Ա��ɵ�¼����
	int state = login(username, password);
	if (state) {
		printCentered(strcat(username, " ��¼�ɹ���"), 0);
		Sleep(2000);  // ͣ�� 2 ��󷵻����˵�
		showUserOptions();
	} else {
		printCentered("��¼ʧ�ܣ������û���������", 0);
		Sleep(2000);  // ͣ�� 2 ��󷵻����˵�
	}

}

// ע���߼�
void registerSystem() {
	char username[50];
	char password[50];
	char year[50];
	char phone[50];
	system("cls");


	while (1) {
		getInputCentered("                    ������ѧ��:           \b\b\b\b\b\b\b\b\b\b", username, sizeof(username), -5, 10);
		if (strlen(username) == 10 && isNumber(username)) {
			break;
		} else {
			printCentered("          ����: ��ʽ����ȷ������������   ", -3);
			Sleep(1000);
			clearLine();
		}
	}
//	system("cls");
	getPasswordCentered("����������: ", password, sizeof(password), -2);
		while (1) {
			getInputCentered("                    �������꼶:           \b\b\b\b\b\b\b\b\b\b", year, sizeof(year), 1, 1);
			if (strlen(year) == 1 && isNumber(year)) {
				break;
			} else {
				printCentered("          ����: ��ʽ����ȷ������������   ", 3);
				Sleep(1000);
				clearLine();
			}
		}
//	getInputCentered("�������꼶: ", year, sizeof(year), 1, 1);
	while (1) {
		getInputCentered("                    �������ֻ�:           \b\b\b\b\b\b\b\b\b\b", phone, sizeof(phone), 4, 11);
		if (strlen(phone) == 11 && isNumber(phone)) {
			break;
		} else {
			printCentered("          ����: ��ʽ����ȷ������������   ", 6);
			Sleep(1000);
			clearLine();
		}
	}
	//getInputCentered("�������ֻ�: ", phone, sizeof(phone), 4, 11);

	system("cls");
	//�ȴ�С���Ա���ע�ắ��
	int state = registerUser(username, password, atoi(year), phone);
	if (state)
		printCentered("ע��ɹ���", 0);
	else
		printCentered("ע��ʧ�ܣ����û����Ѿ���ע�����", 0);
	Sleep(2000);  // ͣ�� 2 ��󷵻����˵�
}

// ����Ա��¼�߼�
void adminLoginSystem() {
	char adminUsername[50];
	char adminPassword[50];

	system("cls");
	//printCentered("����Ա��¼", -4);

	getInputCentered("���������Ա�˺�: ", adminUsername, sizeof(adminUsername), 0, 20);
	//system("cls");
	getPasswordCentered("���������Ա����: ", adminPassword, sizeof(adminPassword), 2);
	system("cls");

	if (strcmp(adminUsername, "admin") == 0 && strcmp(adminPassword, "123456") == 0) {
		printCentered("����Ա��¼�ɹ���", 0);
		Sleep(2000);  // ͣ�� 2 ��󷵻����˵�
		// ���������߳�
		HANDLE hThread = CreateThread(NULL, // Ĭ�ϰ�ȫ����
		                              0, // Ĭ�϶�ջ��С
		                              WindowThread, // �̺߳���
		                              NULL, // �޲�������
		                              0, // Ĭ�ϴ�����־
		                              NULL // ����Ҫ�߳� ID
		                             );

		if (hThread == NULL) {
			printf("�޷����������߳�\n");
			return;
		}
		showAdminOptions();

	} else {
		printCentered("����Ա��¼ʧ�ܣ������û���������", 0);
		Sleep(2000);  // ͣ�� 2 ��󷵻����˵�
	}
}
