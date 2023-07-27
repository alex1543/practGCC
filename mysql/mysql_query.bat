cd C:\xampp\htdocs\practGCC\mysql\
rem  -I "C:\Program Files\MariaDB\MariaDB C Client Library 64-bit\include"
gcc mysql_query.c -I include libmariadb.dll -o mysql_query.exe
mysql_query.exe
