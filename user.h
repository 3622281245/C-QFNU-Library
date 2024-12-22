#ifndef USER_H
#define USER_H
#define USER_DAT "users.dat"
#ifndef BOOK_H1
#define BOOK_H1
#include "book.h"
#endif

typedef struct {
	char username[100];//�û���
	char password[100];//����
	int year;//���Ǽ��꼶��С����
	char phone[100];//�鲻���˸���ô��ϵ��
	Book borrowed_books[5];
	int borrowNum;//���Ѿ����˼�������
} User;
int loadUsersFromFile(User users[]);
void save_users(User users[], int count);
//�޸ĸ�����Ϣ�����ݴ����username�ҵ��û����ݲ����绰����͸�Ϊ�����phone��year
void modifyPersonalInfo(char *username, int year, char *phone);


//��¼�߼�����ȡ�û��ļ��ж��Ƿ���ڣ��˺������Ƿ�ƥ��
int login(char *username, char *password);

//ע���߼�����Ҫ�ж��û����Ƿ��Ѿ�ע�����
int registerUser(char *username, char *password, int year, char *phone);

//�ж��û��Ƿ���ڣ��������򷵻�1
int isUserExist(char *username);
Book* get_borrowed_books(const char *username);
int get_borrowed_count(const char *username);
int add_book_to_user(const char *username, Book book);
int removeBorrowedBook(char *username, char* bookname);

#endif
