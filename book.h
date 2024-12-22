#ifndef BOOK_H
#define BOOK_H

#define BOOK_DAT "books.dat"
typedef struct {
	char title[100];//ͼ����
	char author[100];//����
	char press[100];//������
	int id;//ͼ��ID
	int available;//�������
} Book;

//д�ṹ�嵽�ļ�,Ϊ��ͼ�����id(���Դ�1000��ʼ)
void addBook(char *title[], char *author[], char*type[], int available);

//����idɾ��ͼ��
void deleteBook(char*bookname);


//���ظ��û��Ѿ����ĵ�ͼ�飬�浽�ṹ�����鲢����ָ��
Book * borrowedBooks(char *username[]);

//�������б�¼���ͼ����Ϣ���浽�ṹ�����鲢����ָ��
Book * allBooks();
//ͼ������ 
int getBooksCount();
//������������ͼ�飬���ҵ��򷵻ظ�ͼ��ṹ�壬δ�ҵ��򷵻�NULL
Book* search_books(const char *keyword, int *found_count);

//���飬available-1��borrowNum+1��PS�����ֻ�ܽ��屾
int borrowBook(char *username[], int id);

//��ͼ��黹��available+1��borrowNum-1
void returnBook(char *username[], int id);

int loadBooksFromFile(Book books[]);

void save_books(Book books[], int count);

void updateAva(Book book,int num);

void updateAva2(Book book,int num);
#endif
