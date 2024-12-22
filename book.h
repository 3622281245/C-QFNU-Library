#ifndef BOOK_H
#define BOOK_H

#define BOOK_DAT "books.dat"
typedef struct {
	char title[100];//图书名
	char author[100];//作者
	char press[100];//出版社
	int id;//图书ID
	int available;//库存数量
} Book;

//写结构体到文件,为该图书分配id(可以从1000开始)
void addBook(char *title[], char *author[], char*type[], int available);

//根据id删除图书
void deleteBook(char*bookname);


//返回该用户已经借阅的图书，存到结构体数组并返回指针
Book * borrowedBooks(char *username[]);

//返回所有被录入的图书信息，存到结构体数组并返回指针
Book * allBooks();
//图书数量 
int getBooksCount();
//根据书名查找图书，若找到则返回该图书结构体，未找到则返回NULL
Book* search_books(const char *keyword, int *found_count);

//借书，available-1，borrowNum+1。PS：最多只能借五本
int borrowBook(char *username[], int id);

//将图书归还，available+1，borrowNum-1
void returnBook(char *username[], int id);

int loadBooksFromFile(Book books[]);

void save_books(Book books[], int count);

void updateAva(Book book,int num);

void updateAva2(Book book,int num);
#endif
