#include <my_global.h>
#include <mysql.h>
#include <errmsg.h>
#include <mysqld_error.h>
int main(int argc, char *argv[])
{

    char server[16] = "localhost";
    char username[16] = "root";
    char password[16] = "";
    char database[16] = "test";

    MYSQL* conn = mysql_init(NULL);
    MYSQL_ROW record;

    if (conn == NULL) {
        printf("MySQL initialization failed");
        return 1;
    }

    if (mysql_real_connect(conn, server, username, password, database, 0, NULL, 0) == NULL) {
        printf("Unable to connect with MySQL server\n");
        mysql_close(conn);
        return 1;
    }

	if (mysql_query(conn, "SELECT * FROM myarttable WHERE id>14 ORDER BY id DESC")) {
        printf("Unable to connect with MySQL server\n");
        mysql_close(conn);
        return 1;
    }

    MYSQL_RES* rs = mysql_store_result(conn);

    if (rs == NULL) {
        printf("Unable to compile SQL statement\n");
        mysql_close(conn);
        return 1;
    }

    while (record = mysql_fetch_row(rs)) {
        printf("%s %s %s %s\n", record[0], record[1], record[2], record[3]);
    }
    mysql_close(conn);

    return 0;
}