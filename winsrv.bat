cd C:\xampp\htdocs\practGCC\
if exist winsrv.exe del winsrv.exe
gcc winsrv.c -lwsock32 -I mysql\include libmariadb.dll -o winsrv.exe
winsrv.exe 8080
