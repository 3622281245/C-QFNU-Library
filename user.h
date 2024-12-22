#ifndef USER_H
#define USER_H
#define USER_DAT "users.dat"
#ifndef BOOK_H1
#define BOOK_H1
#include "book.h"
#endif

typedef struct {
	char username[100];//用户名
	char password[100];//密码
	int year;//你是几年级的小朋友
	char phone[100];//书不还了该怎么联系你
	Book borrowed_books[5];
	int borrowNum;//你已经借了几本书了
} User;
int loadUsersFromFile(User users[]);
void save_users(User users[], int count);
//修改个人信息，根据传入的username找到用户数据并将电话与年纪改为传入的phone与year
void modifyPersonalInfo(char *username, int year, char *phone);


//登录逻辑，读取用户文件判断是否存在，账号密码是否匹配
int login(char *username, char *password);

//注册逻辑，需要判断用户名是否已经注册过了
int registerUser(char *username, char *password, int year, char *phone);

//判断用户是否存在，若存在则返回1
int isUserExist(char *username);
Book* get_borrowed_books(const char *username);
int get_borrowed_count(const char *username);
int add_book_to_user(const char *username, Book book);
int removeBorrowedBook(char *username, char* bookname);

#endif
