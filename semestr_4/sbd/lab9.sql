-- SBD Laboratorium 9
-- Język definiowania danych DDL, część 1

-- ex 1
create table projekty (
    id_projektu number(4) generated always as identity,
    opis_projektu varchar(20),
    data_rozpoczecia date default current_date,
    data_zakonczenia date,
    fundusz number(7, 2)
);

-- ex 2
insert into projekty (opis_projektu, data_rozpoczecia, data_zakonczenia, fundusz) values
    ('Indeksy bitmapowe', date '1999-04-02', date '2001-08-31', 25000);
insert into projekty (opis_projektu, data_zakonczenia, fundusz) values
    ('Sieci kręgosłupowe', null, 19000);

-- ex 3
select id_projektu, opis_projektu from projekty;

-- ex 4
-- nie można ustawić ręcznie indeksu, jeśli oznaczyliśmy jako 'generate always'
insert into projekty (opis_projektu, data_rozpoczecia, data_zakonczenia, fundusz) values
    ('Indeksy drzewiaste', date '2013-12-24', date '2014-01-01', 1200);

-- ex 5
-- nie można nadpisać ręcznie indeksu, patrz ex4
update projekty p set p.id_projektu = 10
where p.opis_projektu = 'Indeksy drzewiaste';

-- ex 6
create table projekty_kopia as (select * from projekty);
select * from projekty_kopia;

-- ex 7
-- powiedzie się, ponieważ kopia tabeli nie zachowała atrybutu 'generate always'
insert into projekty_kopia (id_projektu, opis_projektu, data_rozpoczecia, data_zakonczenia, fundusz) values
    (10, 'Sieci lokalne', current_date, current_date + interval '1' year , 1200);
select * from projekty_kopia;
describe projekty_kopia;

-- ex 8
-- usunięcie nie modyfikuje kopii, co jest oczywiste...
delete from projekty where opis_projektu = 'Indeksy drzewiaste';
select * from projekty;
select * from projekty_kopia;

-- ex 9
select table_name from tabs order by table_name;

-- rollback
drop table projekty;
drop table projekty_kopia;
