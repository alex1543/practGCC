/* by Alexey Subbotin */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

char* GetTR() {
	
    char server[16] = "localhost";
    char username[16] = "root";
    char password[16] = "";
    char database[16] = "test";

    MYSQL* conn = mysql_init(NULL);
    MYSQL_ROW record;

    if (conn == NULL) {
        printf("MySQL initialization failed");
        exit(EXIT_FAILURE);
    }

    if (mysql_real_connect(conn, server, username, password, database, 0, NULL, 0) == NULL) {
        printf("Unable to connect with MySQL server\n");
        mysql_close(conn);
        exit(EXIT_FAILURE);
    }

	// заголовок.
	mysql_query(conn, "SHOW COLUMNS FROM myarttable");
	MYSQL_RES* rsH = mysql_store_result(conn);

	char* tr_head_out;
	tr_head_out = "";
	
	char* tp = (char*)malloc(strlen(tr_head_out)+strlen("<tr>")+1);
	strcpy(tp, tr_head_out);
	strcat(tp, "<tr>");
	tr_head_out = tp;

    while (record = mysql_fetch_row(rsH)) {

		char* tp = (char*)malloc(strlen(tr_head_out)+strlen("<td>")+1);
		strcpy(tp, tr_head_out);
		strcat(tp, "<td>");
		tr_head_out = tp;

		tp = (char*)malloc(strlen(tr_head_out)+strlen(record[0])+1);
		strcpy(tp, tr_head_out);
		strcat(tp, record[0]);
		tr_head_out = tp;		

		tp = (char*)malloc(strlen(tr_head_out)+strlen("</td>")+1);
		strcpy(tp, tr_head_out);
		strcat(tp, "</td>");
		tr_head_out = tp;

    }
	
	tp = (char*)malloc(strlen(tr_head_out)+strlen("</tr>")+1);
	strcpy(tp, tr_head_out);
	strcat(tp, "</tr>");
	tr_head_out = tp;

	printf("head is %s", tr_head_out);	
	
	// строки.
	mysql_query(conn, "SELECT * FROM myarttable WHERE id>14 ORDER BY id DESC");
    MYSQL_RES* rs = mysql_store_result(conn);


	char* tr_out;
	tr_out = "";
    while (record = mysql_fetch_row(rs)) {
		char* tp = (char*)malloc(strlen(tr_out)+strlen("<tr>")+1);
		strcpy(tp, tr_out);
		strcat(tp, "<tr>");
		tr_out = tp;


		
		int i = 0;
		while (i < sizeof(record)/2) {
			char* tp = (char*)malloc(strlen(tr_out)+strlen("<td>")+1);
			strcpy(tp, tr_out);
			strcat(tp, "<td>");
			tr_out = tp;

			tp = (char*)malloc(strlen(tr_out)+strlen(record[i])+1);
			strcpy(tp, tr_out);
			strcat(tp, record[i]);
			tr_out = tp;		
		//	printf(record[i]);
			
			tp = (char*)malloc(strlen(tr_out)+strlen("</td>")+1);
			strcpy(tp, tr_out);
			strcat(tp, "</td>");
			tr_out = tp;			
			
			i++;
		}

		tp = (char*)malloc(strlen(tr_out)+strlen("<tr>")+1);
		strcpy(tp, tr_out);
		strcat(tp, "</tr>");
		tr_out = tp;
    }
    mysql_close(conn);

	// заголовок + все строки.
	tp = (char*)malloc(strlen(tr_head_out)+strlen(tr_out)+1);
	strcpy(tp, tr_head_out);
	strcat(tp, tr_out);
	tr_out = tp;
	
	return tr_out;
}
char* GetHTML() {
	
	char* html;
	html = "";
	
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen("select.html", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
		
		// обработка макроса @tr
		char *ret;
		ret = strstr(line, "@tr");
		if (ret) {
			char* tr;
			tr = GetTR();
			char* tp = (char*)malloc(strlen(html)+strlen(tr)+1);
			strcpy(tp, html);
			strcat(tp, tr);
			html = tp;
			
			
		//	printf(line);
			
		}
		
		if (!ret) {
			char* tp = (char*)malloc(strlen(html)+strlen(line)+1);
			strcpy(tp, html);
			strcat(tp, line);
			html = tp;
		}
    }
	//printf("ok");

    fclose(fp);
    if (line)
        free(line);
	return html;
}
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
	char* shtml;
	shtml = GetHTML();
	char clen[33];
	itoa(strlen(shtml), clen, 10);
	//printf(clen);
	char* shead1;
	shead1 = "HTTP/1.1 200 OK\nContent-length: ";
	char* shead2;
	shead2 = "\nContent-Type: text/html\n\n";
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
		server(atoi(argv[1]));
		clean();
	}
	
    exit(EXIT_SUCCESS);
}