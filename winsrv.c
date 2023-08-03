/* by Alexey Subbotin */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// для работы сайта (Web Server)
#include <winsock2.h> // #include<sock.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#pragma comment(lib, "Ws2_32.lib")

// для работы с MySQL
#include <my_global.h>
#include <mysql.h>
#include <errmsg.h>
#include <mysqld_error.h>

// only Win socket()
int init() {
	WSADATA wsaData;
	int iResult;
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}
}
// out only Win socket()
void clean() {
	WSACleanup();
}

// построчное чтение файла (любого).
ssize_t getline(char **linep, size_t *n, FILE *fp) {
	int ch; size_t i = 0; if (!linep || !n || !fp) { return -1; } 
	if (*linep == NULL) { if (NULL == (*linep = malloc(*n = 128))) { *n = 0; return -1; } }
	while ((ch = fgetc(fp)) != EOF)
	{
		if (ch == '\n')
			break;
		if (i + 1 >= *n)
		{
			char *temp = realloc(*linep, *n + 128);
			if (!temp)
			{
				return -1;
			}
			*n += 128;
			*linep = temp;
		}
		(*linep)[i++] = ch;
	}
	(*linep)[i] = '\0';

	return !i && ch == EOF ? -1 : i;
}

// сложение двух строк типа char*
char* cp(char *st, char *apx) {
	char* tp = (char*)malloc(strlen(st)+strlen(apx)+1);
	strcpy(tp, st);
	strcat(tp, apx);
	return tp;
}

// получение объекта подкл.
MYSQL* GetConn() {
	
    char server[16] = "localhost";
    char username[16] = "root";
    char password[16] = "";
    char database[16] = "test";

    MYSQL* conn = mysql_init(NULL);

    if (conn == NULL) {
        printf("MySQL initialization failed");
        exit(EXIT_FAILURE);
    }

    if (mysql_real_connect(conn, server, username, password, database, 0, NULL, 0) == NULL) {
        printf("Unable to connect with MySQL server\n");
        mysql_close(conn);
        exit(EXIT_FAILURE);
    }

	return conn;
}

// получение заголовка и строк таблицы.
char* GetTR() {
	
	MYSQL* conn = GetConn();
    MYSQL_ROW record;

	// заголовок.
	mysql_query(conn, "SHOW COLUMNS FROM myarttable");
	MYSQL_RES* rsH = mysql_store_result(conn);

	char* tr_head_out = "";
	tr_head_out = cp(tr_head_out, "<tr>");
    while (record = mysql_fetch_row(rsH)) {
		tr_head_out = cp(tr_head_out, "<td>");
		tr_head_out = cp(tr_head_out, record[0]);
		tr_head_out = cp(tr_head_out, "</td>");
    }
	tr_head_out = cp(tr_head_out, "</tr>");
	printf("head is %s", tr_head_out);	
	
	// строки.
	mysql_query(conn, "SELECT * FROM myarttable WHERE id>14 ORDER BY id DESC");
    MYSQL_RES* rs = mysql_store_result(conn);

	char* tr_out = "";
    while (record = mysql_fetch_row(rs)) {
		tr_out = cp(tr_out, "<tr>");
		int i = 0;
		while (i < sizeof(record)/2) {
			tr_out = cp(tr_out, "<td>");
			tr_out = cp(tr_out, record[i]);
			tr_out = cp(tr_out, "</td>");			
			i++;
		}
		tr_out = cp(tr_out, "</tr>");
    }
    mysql_close(conn);

	// заголовок + все строки.
	return cp(tr_head_out, tr_out);
}

// получение версии.
char* GetVER() {
	MYSQL* conn = GetConn();
    MYSQL_ROW record;

	// в одну ячейку.
	mysql_query(conn, "SELECT VERSION() AS ver");
	MYSQL_RES* rs = mysql_store_result(conn);

	record = mysql_fetch_row(rs);
    mysql_close(conn);
	return record[0];
}

// получение всего массива на основе шпблона.
char* GetHTML() {
	char* html = "";
    FILE* fp = fopen("select.html", "r");
    if (fp == NULL) exit(EXIT_FAILURE);
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
	
    while ((read = getline(&line, &len, fp)) != -1) {
		// обработка макросов: @tr и @ver
		if (strstr(line, "@tr")!=0) html = cp(html, GetTR());
		if (strstr(line, "@ver")!=0) html = cp(html, GetVER());
		
		if ((strstr(line, "@tr")==0) && (strstr(line, "@ver")==0)) html = cp(html, line);
    }
    fclose(fp);
    if (line) free(line);
	return html;
}

// запуск Web Server.
void server(int port) {
	int sock, connected, bytes_recieved, true = 1;  
	char send_data [1024], recv_data[1024];       
	struct sockaddr_in server_addr,client_addr;    
	int sin_size;
        
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(EXIT_FAILURE);
	}

    int iOptVal = 0;
	if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char *) &iOptVal,sizeof(int)) == -1) {
		perror("Setsockopt");
		exit(EXIT_FAILURE);
	}
        
	server_addr.sin_family = AF_INET;         
	server_addr.sin_port = htons(port);     
	server_addr.sin_addr.s_addr = INADDR_ANY; 
	memset(&(server_addr.sin_zero),0,8);
	if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))== -1) {
		perror("Unable to bind");
		exit(EXIT_FAILURE);
	}

	if (listen(sock, 5) == -1) {
		perror("Listen");
		exit(EXIT_FAILURE);
	}
  
	printf("\n[HTTP Server waiting on port %i] ...\n Link: http://localhost:%i/ \n", port, port);
	fflush(stdout);
	sin_size = sizeof(struct sockaddr_in);
	connected = accept(sock, (struct sockaddr *)&client_addr,&sin_size);
	char kk[9999];
	recv(connected,kk,sizeof(kk),0);
	printf("\n Received:%s",kk); 
	
	// формирование полного ответа.
	char* shtml = GetHTML();
	char clen[33];
	itoa(strlen(shtml), clen, 10);
	//printf(clen);
	char* shead1 = "HTTP/1.1 200 OK\nContent-length: ";
	char* shead2 = "\nContent-Type: text/html\n\n";
	char xx[9999];
    strcpy(xx, shead1);
	strcat(xx, clen);
    strcat(xx, shead2);
	strcat(xx, shtml);
	
	// отправка.
	int c=send(connected,xx,sizeof(xx),0);
	printf("\nSTATUS:%d",c);
	//printf("\nSent : %s\n",xx);
	close(sock);
	WSACleanup();
}

// point enter.
int main(int argc, char **argv) {
	while(1) {
		init();
		int port = 8081; // порт по умолчанию, но можно поменять (первый параметр).
		if (argc > 1) port = atoi(argv[1]);
		printf("\nargc: %i, port: %i", argc, port);
		server(port);
		clean();
	}
	
    exit(EXIT_SUCCESS);
}