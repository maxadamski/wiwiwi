# Porównania rozważanych technologii

## Baza danych: Postgresql vs MariaDB vs MySQL

### MariaDB > MySQL

* Struktura bazy danych i indeksy MariaDB są takie same jak w MySQL, jednak MariaDB jest i będzie opensourcow'a w przeciwieństwie do Mysql, bo po przejęciu przez Oracle nie wiadomo czego się spodziewać
* MariaDB jest zdecydowanie szybszy w porównaniu z MySQL. Dzięki silnikom pamięci masowej wykorzystanym w MariaDB, instrukcja INSERT może być wykonana 24% szybciej niż w standardowym MySQL dla indeksu liczb całkowitych i 60% szybciej dla indeksu na kolumnie CHAR (20)
* MariaDB korzysta z najbardziej wydajnych funkcji multipleksowania IO dla każdego systemu operacyjnego: Windows (Native Thread Pools), Linux (epoll), Solaris (event ports), FreeBSD i OSX (kevent). Oracle MySQL używa zoptymalizowanego multipleksowania we/wy tylko w systemie Linux z epoll, a w innych przypadkach korzysta z funkcji poll(). Dzięki temu pula wątków w MariaDB działa zdecydowanie szybciej
* MariaDB has had a significant boost in  terms of security features like internal security and password check,  PAM and LDAP authentication, Kerberos, user roles, and strong encryption over tablespaces, tables and logs
* MariaDB uzyskała znaczący postęp w zakresie funkcji bezpieczeństwa, takich jak bezpieczeństwo wewnętrzne i sprawdzanie hasłeł, uwierzytelnianie PAM i LDAP, Kerberos, role użytkowników i silne szyfrowanie w obszarach tabel, przestrzeń tabel i rejestrach zdarzeń
* Klasyczne (i powolne) silniki baz danych MySQL: MyISAM i InnoDB, zostały zastąpione w MariaDB odpowiednio przez Aria i XtraDB. Aria oferuje lepsze buforowanie, co ma duże znaczenie, jeśli chodzi o operacje w dużym stopniu korzystające z dysku.
* https://mariadb.com/kb/en/mariadb-vs-mysql-features/
* https://mariadb.org/maria-10-1-mysql-5-7-commodity-hardware/

### PostgreSQL > MariaDB

* MariaDB często automatycznie wprowadza korekcję danych wejściowych, gdy ich typ nie pasuje do danego typu w bazie i powiadamia o tym komunikatem ostrzegawczym. Z drugiej strony PostgreSQL nie wprowadza żadnych automatycznych poprawek. Zamiast tego, jeśli istnieje jakakolwiek rozbieżność w typie danych wejściowych, PostgreSQL wyświetla błąd i nie akceptuje danych, dopóki nie będą one zgodne z wymaganym typem danych. PostgreSQL jest zatem bardziej rygorystyczny pod względem integralności danych niż MariaDB, gdzie użytkownik musi być o wiele bardziej ostrożny podczas wprowadzania danych.
* PostgreSQL oferuje lepszą ogólną wydajność, skutkując krótszymi czasami odczytu i zapisu. 
* Ponadto PostgreSQL oferuje pewne dodatkowe funkcje bazy danych, takie jak partial indexes, range types, user-defined types, które nie są oferowane przez MariaDB.
* Dodatkowo dostęp do postgresql za pomocą naszej preferowanej biblioteki: Pugsql jest znacznie lepszy i wygodniejszy, niż w przypadku MariaDB.
* https://www.cybertec-postgresql.com/en/why-favor-postgresql-over-mariadb-mysql/
* https://www.trustradius.com/compare-products/mariadb-vs-postgresql

## Web Framework: FastAPI > Flask

* FastAPI, jak sama nazwa wskazuje, jest jednym z najszybszych frameworków, oferowanych przez język Python, przewyższając Flask o ponad 300%
* Szybki interfejs FastAPI obsługuje asynchroniczność domyślnie, w przeciwieństwie do Flaska
* Domyślnie komunikaty o błędach mają format JSON, a wartości zwracane przez moduły obsługi to słowniki. Natomiast w przypadku Flaska, błędy domyślnie są stronami HTML
* Testowanie za pomocą FastAPI jest naprawdę proste i można je wykonać za pomocą [TestClient](https://fastapi.tiangolo.com/tutorial/testing) dostarczonego przez FastAPI.
* Aby zająć się obsługą wyjątków w FastAPI, wystarczy użyć adnotacji `@ app.exception_handler` lub funkcji `app.add_exception_handler` w celu zarejestrowania odpowiedzi na wyjątek, który zostanie wtedy obsłużony przez FastAPI.
* FastAPI ma bardzo obszerną i bogatą w przykłady [dokumentację](https://fastapi.tiangolo.com/), co znacznie ułatwia pracę z tym frameworkiem. Zwykle nie ma potrzeby szukać daleko poza dokumentacją.
* https://dev.to/meseta/flask-vs-fastapi-first-impressions-1bnm
* https://dev.to/fuadrafid/fastapi-the-good-the-bad-and-the-ugly-20ob

## Połączenie z bazą danych: PugSQL

* Do połączenia z bazą postgresql wykorzystamy prosty pythonowy interface wspierający każdą bazę zawartą w SQLAlchemy toolkit: pugsql
* Głównym ideą pugsql jest pisanie potrzebnych kwerend SQL, a następnie wywoływanie ich w formie czytelnych funkcji.
* Jest to podejście anti-ORM, co oznacza, że nie ma potrzeby tworzenia wielu niepotrzebnych klas na potrzeby odtworzenia bazy w kodzie.