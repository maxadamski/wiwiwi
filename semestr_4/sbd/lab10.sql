-- SBD Laboratorium 10
-- Język definiowania danych DDL, część 2

-- ex 1
alter table projekty
add (
    constraint pk_projekty primary key(id_projektu),
    constraint uk_projekty unique(opis_projektu),
    check(data_zakonczenia > data_rozpoczecia),
    check(fundusz >= 0)
)
modify (
    opis_projektu not null
);

select k.constraint_name, k.constraint_type, k.search_condition, c.column_name
from user_constraints k
inner join user_cons_columns c on k.constraint_name = c.constraint_name
where k.table_name = 'PROJEKTY'
order by k.constraint_name;

-- ex 2
insert into projekty (opis_projektu, data_rozpoczecia, data_zakonczenia, fundusz) values
    ('Indeksy bitmapowe', date '2015-04-12', date '2016-08-30', 40000);

-- ex 3
create table przydzialy (
    id_projektu number(4) constraint fk_przydzialy_01 references projekty(id_projektu) not null,
    nr_pracownika number(6) constraint fk_przydzialy_02 references pracownicy(id_prac) not null,
    od date default current_date,
    do date, constraint chk_przydzialy_daty check(do > od), -- ważny przecinek: bo odwołanie do innej kolumny
    stawka number(7, 2) constraint chk_przydzialy_stawka check(stawka > 0),
    rola varchar(20) constraint chk_przydzialy_rola check(rola in ('KIERUJĄCY', 'ANALITYK', 'PROGRAMISTA'))
);

-- ex 4
insert into przydzialy (id_projektu, nr_pracownika, od, do, stawka, rola) values
    ((select id_projektu from projekty where opis_projektu = 'Indeksy bitmapowe'),
    170, date '1999-04-10', date '1999-05-10', 1000, 'KIERUJĄCY');
insert into przydzialy (id_projektu, nr_pracownika, od, stawka, rola) values
    ((select id_projektu from projekty where opis_projektu = 'Indeksy bitmapowe'),
    140, date '2000-12-01', 1500, 'ANALITYK');
insert into przydzialy (id_projektu, nr_pracownika, od, stawka, rola) values
    ((select id_projektu from projekty where opis_projektu = 'Sieci kręgosłupowe'),
    140, date '2015-09-14', 2500, 'KIERUJĄCY');
select * from przydzialy;

-- ex 5
-- nie można dodać kolumny not null, jeżeli są już jakieś rekordy
alter table przydzialy add godziny number(4) check(godziny <= 9999) not null;

-- ex 6
alter table przydzialy add godziny number(4);
create sequence godzinki start with 10 increment by 10;
update przydzialy set godziny = godzinki.nextval;
drop sequence godzinki;
alter table przydzialy add (check(godziny <= 9999)) modify (godziny not null);
select * from przydzialy;

-- ex 7
alter table projekty disable constraint uk_projekty;
select constraint_name, status
from user_constraints
where constraint_name = 'UK_PROJEKTY';

-- ex 8
insert into projekty (opis_projektu, data_rozpoczecia, data_zakonczenia, fundusz)
    values ('Indeksy bitmapowe', date '2015-04-12', date '2016-09-30', 40000);
select * from projekty;

-- ex 9
alter table projekty enable constraint uk_projekty;

-- ex 10
update projekty set opis_projektu = 'Inne indeksy' where id_projektu = 4;
alter table projekty enable constraint uk_projekty;

-- ex 11
alter table projekty modify opis_projektu varchar(10);

-- ex 12
-- sieci kręgosłupowe są używane w tabeli przydziały
delete from projekty where opis_projektu = 'Sieci kręgosłupowe';

-- ex 13
alter table przydzialy drop constraint fk_przydzialy_01;
alter table przydzialy add constraint fk_przydzialy_01 foreign key(id_projektu) references projekty(id_projektu) on delete cascade;
delete from projekty where opis_projektu = 'Sieci kręgosłupowe';
select * from projekty;
select * from przydzialy;

-- ex 14
drop table projekty cascade constraints;

select constraint_name, constraint_type as C, search_condition
from user_constraints
where table_name = 'PRZYDZIALY'
order by constraint_name;

-- ex 15
drop table przydzialy;
drop table projekty_kopia;
select table_name from user_tables;
