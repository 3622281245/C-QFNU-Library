/*
	filename：user.c user.h
	完成者：刘振 王培洋
*/

#include "user.h"
#include <stdio.h>
#include <string.h>

User users[100]; //最大用户数为100
User nowUser;

// 修改个人信息
void modifyPersonalInfo(char *username, int year, char *phone) {
	int user_count = loadUsersFromFile(users);
	for (int i = 0; i < user_count; i++) {
		if (strcmp(users[i].username, username) == 0) {
			users[i].year = year;
			nowUser.year = year;
			strncpy(nowUser.phone, phone, sizeof(phone));
			strncpy(users[i].phone, phone, sizeof(users[i].phone));
			save_users(users, user_count); // 保存更新后的用户信息
		}
	}
}

// 登录逻辑
int login(char *username, char *password) {
	int user_count = loadUsersFromFile(users);
	for (int i = 0; i < user_count; i++) {
		if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
			nowUser = users[i];
			return 1; // 登录成功
		}
	}
	return 0; // 登录失败
}

// 注册逻辑
int registerUser(char *username, char *password, int year, char *phone) {
	if (isUserExist(username)) {
		return 0; // 用户已存在
	}

	User newUser;
	strncpy(newUser.username, username, sizeof(newUser.username));
	strncpy(newUser.password, password, sizeof(newUser.password));
	newUser.year = year;
	strncpy(newUser.phone, phone, sizeof(newUser.phone));
	newUser.borrowNum = 0;
	saveUsersToFile(&newUser);
	return 1; // 注册成功
}

// 判断用户是否存在
int isUserExist(char *username) {
	int user_count = loadUsersFromFile(users);
	for (int i = 0; i < user_count; i++) {
		if (strcmp(users[i].username, username) == 0) {
			return 1; // 用户存在
		}
	}
	return 0; // 用户不存在
}
// 保存用户信息到文件
void save_users(User users[], int count) {
	FILE *file = fopen(USER_DAT, "wb");
	if (file == NULL) {
		//perror("无法打开文件");
		return;
	}
	fwrite(users, sizeof(User), count, file);
	fclose(file);
}

// 新增用户信息到文件
void saveUsersToFile(User *user) {
	FILE *file = fopen(USER_DAT, "ab");
	if (file == NULL) {
		//perror("无法打开文件");
		return;
	}
	// 将图书结构体写入文件
	fwrite(user, sizeof(User), 1, file);
	fclose(file);

}

// 从文件加载用户信息
int loadUsersFromFile(User users[]) {
	FILE *file = fopen(USER_DAT, "rb");
	if (file == NULL) {
		//perror("无法打开文件");
		return -1;
	}
	int count = 0;
	while (fread(&users[count], sizeof(User), 1, file)) {
		count++;
	}

	fclose(file);
	return count;
}

//获取该用户名下所有图书
Book* get_borrowed_books(const char *username) {
	int user_count = loadUsersFromFile(users);

	for (int i = 0; i < user_count; i++) {
		if (strcmp(users[i].username, username) == 0) {
			return users[i].borrowed_books;
		}
	}
	return NULL;
}

//获取该用户名下所有图书的数量 
int get_borrowed_count(const char *username) {
	int user_count = loadUsersFromFile(users);
	for (int i = 0; i < user_count; i++) {
		if (strcmp(users[i].username, username) == 0) {
			return  users[i].borrowNum;
		}
	}
	return NULL;
}

//将一本书划到该用户名下（借书） 
int add_book_to_user(const char *username, Book book) {
	int user_count = loadUsersFromFile(users);

	// 查找指定用户名的用户
	for (int i = 0; i < user_count; i++) {
		if (strcmp(users[i].username, username) == 0) {
			// 检查借阅数量是否已满
			if (users[i].borrowNum >= 5) {
				printf("用户 %s 的借阅列表已满，无法添加更多图书", username);
				return -1; // 借阅列表已满
			}
			for (int j = 0; j < users[i].borrowNum; j++) {
				if (strcmp(users[i].borrowed_books[j].title, book.title) == 0) {
					printf("该图书正在借阅中", username);
					return -1; // 借阅列表已满
				}
			}
		
			updateAva(book,-1);
			// 添加图书到用户的借阅列表
			users[i].borrowed_books[users[i].borrowNum] = book;
			users[i].borrowNum++;
			save_users(users, user_count); // 保存更新后的用户信息
			printf("图书 '%s' 已成功添加到用户 %s 的借阅列表", book.title, username);
			return 0; // 成功添加图书
		}
	}

	printf("未找到用户名为 %s 的用户\n", username);
	return -1; // 用户不存在
}

// 移除书籍
int removeBorrowedBook(char *username, char* bookname) {
	int user_count = loadUsersFromFile(users);
	for (int i = 0; i < user_count; i++) {
		if (strcmp(users[i].username, username) == 0) {
			for (int j = 0; j < users[i].borrowNum; j++) {
				if (strcmp(users[i].borrowed_books[j].title, bookname ) == 0) {

					// 找到目标书籍 ID，移除
					for (int p = j; p < users[i].borrowNum - 1; p++) {
						users[i].borrowed_books[p] = users[i].borrowed_books[p + 1];
					}
					users[i].borrowNum--; // 更新借阅数量
					save_users(users, user_count);
					return 1; // 成功移除
				}
			}
		}
	}
	return 0;
}
