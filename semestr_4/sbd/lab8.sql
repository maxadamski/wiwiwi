-- ex 1
insert into pracownicy (id_prac, nazwisko, etat, id_szefa, zatrudniony, placa_pod, placa_dod, id_zesp) values
    (231, 'Foo', 'ASYSTENT', 100, date '2018-01-01', 600, null, 10);
insert into pracownicy (id_prac, nazwisko, etat, id_szefa, zatrudniony, placa_pod, placa_dod, id_zesp) values
    (232, 'Bar', 'ASYSTENT', 100, date '2018-01-01', 600, 120, 10);
insert into pracownicy (id_prac, nazwisko, etat, id_szefa, zatrudniony, placa_pod, placa_dod, id_zesp) values
    (233, 'Baz', 'ASYSTENT', 100, date '2018-01-01', 208, 130, 10);
    
-- ex 2
update pracownicy p
set (p.placa_pod, p.placa_dod) = (
    select 1.1 * placa_pod, coalesce(1.2 * placa_dod, 100)
    from pracownicy
    where id_prac = p.id_prac
)
where p.zatrudniony = date '2018-01-01';

-- ex 3
insert into zespoly (id_zesp, nazwa, adres) values (60, 'BAZY DANYCH', 'PIOTROWO 2');

-- ex 4
update pracownicy p
set (p.id_zesp) = (select id_zesp from zespoly where nazwa = 'BAZY DANYCH')
where p.zatrudniony = date '2018-01-01';

-- ex 5
update pracownicy p
set (p.id_szefa) = (select id_prac from pracownicy where nazwisko = 'MORZY')
where p.id_zesp = (select id_zesp from zespoly where nazwa = 'BAZY DANYCH');

-- ex 6
delete from zespoly where nazwa = 'BAZY DANYCH';

-- ex 7
delete from pracownicy where id_zesp = (select id_zesp from zespoly where nazwa = 'BAZY DANYCH');
delete from zespoly where nazwa = 'BAZY DANYCH';

-- ex 8
select id_zesp, 0.1 * avg(placa_pod)
from pracownicy
group by id_zesp;

-- ex 9
update pracownicy p
set (p.placa_pod) = (
    select p.placa_pod + 0.1 * avg(placa_pod)
    from pracownicy
    group by id_zesp
    having id_zesp = p.id_zesp
);

-- ex 10
select *
from pracownicy
order by placa_pod asc
fetch first 1 rows with ties;

-- ex 11
update pracownicy p
set (p.placa_pod) = (
    select avg(placa_pod) from pracownicy
)
where p.id_prac in (
    select id_prac
    from pracownicy
    order by placa_pod asc
    fetch first 1 rows with ties
);

-- ex 12
select round(avg(placa_pod), 2)
from pracownicy
where id_szefa = (select id_szefa from pracownicy where nazwisko = 'MORZY');

select * from pracownicy order by nazwisko asc;
select * from zespoly;
rollback;
