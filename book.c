
/*
	filename��book.c book.h
	����ߣ������� ����˶ ���
*/

#include "book.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
int nextBookId = 1000;
Book books[100];

// д�ṹ�嵽�ļ���Ϊ��ͼ�����id�����Դ�1000��ʼ��
void addBook(char *title[], char *author[], char *press[], int available) {
	Book newBook;
	// �������������ߡ���������Ϣ
	strcpy(newBook.title, title);
	strcpy(newBook.author, author);
	strcpy(newBook.press, press);
	newBook.id = nextBookId++;
	newBook.available = available;

	// ��׷�Ӷ�����ģʽ���ļ�
	FILE *fp = fopen(BOOK_DAT, "ab");
	if (fp == NULL) {
//		perror("Error opening file");
		return;
	}
	// ��ͼ��ṹ��д���ļ�
	fwrite(&newBook, sizeof(Book), 1, fp);
	fclose(fp);
}

// ��������ɾ��ͼ��
void deleteBook(char* bookname) {
	int bookCount = loadBooksFromFile(books);

	int found = 0; // ��־�Ƿ��ҵ��鼮

	for (int i = 0; i < bookCount; i++) {
		if (strcmp(books[i].title, bookname) == 0) {
			// �ҵ��鼮�󣬸��ǵ�ǰ�鼮
			for (int j = i; j < bookCount - 1; j++) {
				books[j] = books[j + 1];
			}
			bookCount--;
			found = 1;
			break;
		}
	}

	if (found) {
		save_books(books, bookCount);
		return 1;
	} else {
		return 0;
	}
}




Book * allBooks() {
	FILE *fpRead = fopen(BOOK_DAT, "rb"); // �Զ����ƶ�ȡģʽ���ļ�
	if (fpRead == NULL) {
//		perror("Error opening file");
		return NULL;
	}
	fseek(fpRead, 0, SEEK_END);
	long fileSize = ftell(fpRead);
	rewind(fpRead);
	long bookCount = fileSize / sizeof(Book);
	Book *books = (Book *)malloc(bookCount * sizeof(Book)); //Ϊbooks���䶯̬�ڴ�
	if (books == NULL) {
//		perror("Memory allocation failed");
		fclose(fpRead);
		return NULL;
	}
	fread(books, sizeof(Book), bookCount, fpRead);
	fclose(fpRead);
	return books;
}

int string_contains(const char *str, const char *substr) {
// ����������
	if (substr == NULL || str == NULL) {
		return 0; // NULL ���뷵�� 0
	}
	if (*substr == '\0') {
		return 1; // �յ����ַ�������Ϊ����ƥ��
	}
	if (*str == '\0') {
		return 0; // �յ����ַ����޷�ƥ��ǿ����ַ���
	}

// �������ַ���
	while (*str) {
		const char *s = str;
		const char *sub = substr;

// �Ƚ����ַ��������ַ������ַ������Դ�Сд
		while (*s && *sub && tolower(*s) == tolower(*sub)) {
			s++;
			sub++;
		}

// ������ַ���ƥ�䵽ĩβ��˵���ҵ�ƥ��
		if (*sub == '\0') {
			return 1;
		}

// ���ַ���ָ�����һλ�������Ƚ�
		str++;
	}

	return 0; // δ�ҵ�ƥ��
}
// ����ͼ�飬����ͼ����ģ��ƥ�䲢����ƥ���ͼ������
Book* search_books(const char *keyword, int *found_count) {
	Book *all_books = allBooks();
	// ����һ����̬����洢ƥ����
	int bookscount = getBooksCount();
	if (bookscount) {
		Book *matched_books = (Book *)malloc(bookscount * sizeof(Book));

		if (matched_books == NULL) {
			perror("�ڴ����ʧ��");
			free(all_books); // �ͷ�ͼ����ڴ�
			exit(1);
		}
		*found_count = 0;
		// ��������ͼ�����ƥ��
		for (int i = 0; i < bookscount; i++) {
			if (string_contains(all_books[i].title, keyword)) {
				matched_books[*found_count] = all_books[i];
				(*found_count)++;
			}
		}

		free(all_books); // �ͷ�ͼ����ڴ�

		if (*found_count == 0) {
			free(matched_books); // ���û��ƥ�䣬�ͷ�ƥ�����ڴ�
			return NULL;
		}
		return matched_books;
	} else {
		return NULL;
	}


}


// ���ļ�����ͼ����Ϣ
int loadBooksFromFile(Book books[]) {
	FILE *file = fopen(BOOK_DAT, "rb");
	if (file == NULL) {
		//perror("�޷����ļ�");
		return -1;
	}
	int count = 0;
	while (fread(&books[count], sizeof(Book), 1, file)) {
		count++;
	}

	fclose(file);
	return count;
}
// ����ͼ����Ϣ���ļ�
void save_books(Book books[], int count) {
	FILE *file = fopen(BOOK_DAT, "wb");
	if (file == NULL) {
		//perror("�޷����ļ�");
		return;
	}
	fwrite(books, sizeof(Book), count, file);
	fclose(file);
}
void updateAva(Book book, int num) {
	int book_count = loadBooksFromFile(books);
	for (int i = 0; i < book_count; i++) {
		if (strcmp(books[i].title, book.title) == 0) {
			books[i].available += num;
			save_books(books, book_count); // ������º���û���Ϣ
		}
	}
}
void updateAva2(Book book, int num) {
	int book_count = loadBooksFromFile(books);
	for (int i = 0; i < book_count; i++) {
		if (strcmp(books[i].title, book.title) == 0) {
			books[i].available = num;
			save_books(books, book_count); // ������º���û���Ϣ
		}
	}
}

int getBooksCount() {
	FILE *fpRead = fopen(BOOK_DAT, "rb");
	if (fpRead == NULL) {
		return NULL;
	}
	fseek(fpRead, 0, SEEK_END);
	long fileSize = ftell(fpRead);
	rewind(fpRead);
	long bookCount = fileSize / sizeof(Book);
	
	//��������ԭ��δʹ�����´���ر��ļ�
	fclose(fpRead);
	
	return bookCount;
}
