/*
	filename��user.c user.h
	����ߣ����� ������
*/

#include "user.h"
#include <stdio.h>
#include <string.h>

User users[100]; //����û���Ϊ100
User nowUser;

// �޸ĸ�����Ϣ
void modifyPersonalInfo(char *username, int year, char *phone) {
	int user_count = loadUsersFromFile(users);
	for (int i = 0; i < user_count; i++) {
		if (strcmp(users[i].username, username) == 0) {
			users[i].year = year;
			nowUser.year = year;
			strncpy(nowUser.phone, phone, sizeof(phone));
			strncpy(users[i].phone, phone, sizeof(users[i].phone));
			save_users(users, user_count); // ������º���û���Ϣ
		}
	}
}

// ��¼�߼�
int login(char *username, char *password) {
	int user_count = loadUsersFromFile(users);
	for (int i = 0; i < user_count; i++) {
		if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
			nowUser = users[i];
			return 1; // ��¼�ɹ�
		}
	}
	return 0; // ��¼ʧ��
}

// ע���߼�
int registerUser(char *username, char *password, int year, char *phone) {
	if (isUserExist(username)) {
		return 0; // �û��Ѵ���
	}

	User newUser;
	strncpy(newUser.username, username, sizeof(newUser.username));
	strncpy(newUser.password, password, sizeof(newUser.password));
	newUser.year = year;
	strncpy(newUser.phone, phone, sizeof(newUser.phone));
	newUser.borrowNum = 0;
	saveUsersToFile(&newUser);
	return 1; // ע��ɹ�
}

// �ж��û��Ƿ����
int isUserExist(char *username) {
	int user_count = loadUsersFromFile(users);
	for (int i = 0; i < user_count; i++) {
		if (strcmp(users[i].username, username) == 0) {
			return 1; // �û�����
		}
	}
	return 0; // �û�������
}
// �����û���Ϣ���ļ�
void save_users(User users[], int count) {
	FILE *file = fopen(USER_DAT, "wb");
	if (file == NULL) {
		//perror("�޷����ļ�");
		return;
	}
	fwrite(users, sizeof(User), count, file);
	fclose(file);
}

// �����û���Ϣ���ļ�
void saveUsersToFile(User *user) {
	FILE *file = fopen(USER_DAT, "ab");
	if (file == NULL) {
		//perror("�޷����ļ�");
		return;
	}
	// ��ͼ��ṹ��д���ļ�
	fwrite(user, sizeof(User), 1, file);
	fclose(file);

}

// ���ļ������û���Ϣ
int loadUsersFromFile(User users[]) {
	FILE *file = fopen(USER_DAT, "rb");
	if (file == NULL) {
		//perror("�޷����ļ�");
		return -1;
	}
	int count = 0;
	while (fread(&users[count], sizeof(User), 1, file)) {
		count++;
	}

	fclose(file);
	return count;
}

//��ȡ���û���������ͼ��
Book* get_borrowed_books(const char *username) {
	int user_count = loadUsersFromFile(users);

	for (int i = 0; i < user_count; i++) {
		if (strcmp(users[i].username, username) == 0) {
			return users[i].borrowed_books;
		}
	}
	return NULL;
}

//��ȡ���û���������ͼ������� 
int get_borrowed_count(const char *username) {
	int user_count = loadUsersFromFile(users);
	for (int i = 0; i < user_count; i++) {
		if (strcmp(users[i].username, username) == 0) {
			return  users[i].borrowNum;
		}
	}
	return NULL;
}

//��һ���黮�����û����£����飩 
int add_book_to_user(const char *username, Book book) {
	int user_count = loadUsersFromFile(users);

	// ����ָ���û������û�
	for (int i = 0; i < user_count; i++) {
		if (strcmp(users[i].username, username) == 0) {
			// �����������Ƿ�����
			if (users[i].borrowNum >= 5) {
				printf("�û� %s �Ľ����б��������޷���Ӹ���ͼ��", username);
				return -1; // �����б�����
			}
			for (int j = 0; j < users[i].borrowNum; j++) {
				if (strcmp(users[i].borrowed_books[j].title, book.title) == 0) {
					printf("��ͼ�����ڽ�����", username);
					return -1; // �����б�����
				}
			}
		
			updateAva(book,-1);
			// ���ͼ�鵽�û��Ľ����б�
			users[i].borrowed_books[users[i].borrowNum] = book;
			users[i].borrowNum++;
			save_users(users, user_count); // ������º���û���Ϣ
			printf("ͼ�� '%s' �ѳɹ���ӵ��û� %s �Ľ����б�", book.title, username);
			return 0; // �ɹ����ͼ��
		}
	}

	printf("δ�ҵ��û���Ϊ %s ���û�\n", username);
	return -1; // �û�������
}

// �Ƴ��鼮
int removeBorrowedBook(char *username, char* bookname) {
	int user_count = loadUsersFromFile(users);
	for (int i = 0; i < user_count; i++) {
		if (strcmp(users[i].username, username) == 0) {
			for (int j = 0; j < users[i].borrowNum; j++) {
				if (strcmp(users[i].borrowed_books[j].title, bookname ) == 0) {

					// �ҵ�Ŀ���鼮 ID���Ƴ�
					for (int p = j; p < users[i].borrowNum - 1; p++) {
						users[i].borrowed_books[p] = users[i].borrowed_books[p + 1];
					}
					users[i].borrowNum--; // ���½�������
					save_users(users, user_count);
					return 1; // �ɹ��Ƴ�
				}
			}
		}
	}
	return 0;
}
