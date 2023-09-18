# practGCC
Как можно работать с таблицами в MySQL на MinGW (GCC)

Пример работает (Web Server) при установленном x86_64-w64-mingw32-gcc-8.3.0 в любой каталог под Windows 11. Потом нужно запустить winsrv.bat и открыть страницу: http://localhost:8080/

Внешний вид Web Server-a и таблицы из MariaDB:

![image](https://github.com/alex1543/practGCC/assets/10297748/a95fcefc-eebd-4f23-a21a-51f8f4fb5b35)



Для работы с MySQL или MariaDB не обязательно устанавливать драйвер mariadb-native-client-2.0.0-win64.msi с сайта https://mariadb.org/download/?t=connector&o=true&p=connector-c&r=2.0.0&os=windows&cpu=x86_64 пример и так скомпиллируется.

Для работы примера на C++ требуется установить MinGW (GCC), либо скопировать библиотеки: libstdc++-6.dll, libgcc_s_seh-1.dll, libwinpthread-1.dll в каталог с программой на C++. После запуска WebServer.exe нужно открыть страницу http://localhost:8080
