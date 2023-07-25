cd C:\xampp\htdocs\practGCC\
if exist winsrv.exe del winsrv.exe
gcc winsrv.c -lwsock32 -o winsrv.exe
winsrv.exe 8080
