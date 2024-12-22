
/*
	filename：book.c book.h
	完成者：王瑞崧 陈文硕 李凡玉
*/

#include "book.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
int nextBookId = 1000;
Book books[100];

// 写结构体到文件，为该图书分配id（可以从1000开始）
void addBook(char *title[], char *author[], char *press[], int available) {
	Book newBook;
	// 复制书名、作者、出版社信息
	strcpy(newBook.title, title);
	strcpy(newBook.author, author);
	strcpy(newBook.press, press);
	newBook.id = nextBookId++;
	newBook.available = available;

	// 以追加二进制模式打开文件
	FILE *fp = fopen(BOOK_DAT, "ab");
	if (fp == NULL) {
//		perror("Error opening file");
		return;
	}
	// 将图书结构体写入文件
	fwrite(&newBook, sizeof(Book), 1, fp);
	fclose(fp);
}

// 根据书名删除图书
void deleteBook(char* bookname) {
	int bookCount = loadBooksFromFile(books);

	int found = 0; // 标志是否找到书籍

	for (int i = 0; i < bookCount; i++) {
		if (strcmp(books[i].title, bookname) == 0) {
			// 找到书籍后，覆盖当前书籍
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
	FILE *fpRead = fopen(BOOK_DAT, "rb"); // 以二进制读取模式打开文件
	if (fpRead == NULL) {
//		perror("Error opening file");
		return NULL;
	}
	fseek(fpRead, 0, SEEK_END);
	long fileSize = ftell(fpRead);
	rewind(fpRead);
	long bookCount = fileSize / sizeof(Book);
	Book *books = (Book *)malloc(bookCount * sizeof(Book)); //为books分配动态内存
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
// 检查输入参数
	if (substr == NULL || str == NULL) {
		return 0; // NULL 输入返回 0
	}
	if (*substr == '\0') {
		return 1; // 空的子字符串被认为总是匹配
	}
	if (*str == '\0') {
		return 0; // 空的主字符串无法匹配非空子字符串
	}

// 遍历主字符串
	while (*str) {
		const char *s = str;
		const char *sub = substr;

// 比较主字符串和子字符串的字符，忽略大小写
		while (*s && *sub && tolower(*s) == tolower(*sub)) {
			s++;
			sub++;
		}

// 如果子字符串匹配到末尾，说明找到匹配
		if (*sub == '\0') {
			return 1;
		}

// 主字符串指针后移一位，继续比较
		str++;
	}

	return 0; // 未找到匹配
}
// 搜索图书，根据图书名模糊匹配并返回匹配的图书数组
Book* search_books(const char *keyword, int *found_count) {
	Book *all_books = allBooks();
	// 分配一个动态数组存储匹配结果
	int bookscount = getBooksCount();
	if (bookscount) {
		Book *matched_books = (Book *)malloc(bookscount * sizeof(Book));

		if (matched_books == NULL) {
			perror("内存分配失败");
			free(all_books); // 释放图书库内存
			exit(1);
		}
		*found_count = 0;
		// 遍历所有图书进行匹配
		for (int i = 0; i < bookscount; i++) {
			if (string_contains(all_books[i].title, keyword)) {
				matched_books[*found_count] = all_books[i];
				(*found_count)++;
			}
		}

		free(all_books); // 释放图书库内存

		if (*found_count == 0) {
			free(matched_books); // 如果没有匹配，释放匹配结果内存
			return NULL;
		}
		return matched_books;
	} else {
		return NULL;
	}


}


// 从文件加载图书信息
int loadBooksFromFile(Book books[]) {
	FILE *file = fopen(BOOK_DAT, "rb");
	if (file == NULL) {
		//perror("无法打开文件");
		return -1;
	}
	int count = 0;
	while (fread(&books[count], sizeof(Book), 1, file)) {
		count++;
	}

	fclose(file);
	return count;
}
// 保存图书信息到文件
void save_books(Book books[], int count) {
	FILE *file = fopen(BOOK_DAT, "wb");
	if (file == NULL) {
		//perror("无法打开文件");
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
			save_books(books, book_count); // 保存更新后的用户信息
		}
	}
}
void updateAva2(Book book, int num) {
	int book_count = loadBooksFromFile(books);
	for (int i = 0; i < book_count; i++) {
		if (strcmp(books[i].title, book.title) == 0) {
			books[i].available = num;
			save_books(books, book_count); // 保存更新后的用户信息
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
	
	//造成问题的原因：未使用如下代码关闭文件
	fclose(fpRead);
	
	return bookCount;
}
