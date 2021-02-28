
-- ex 1

create or replace view asystenci (
    id, nazwisko, placa, staz_pracy
) as select
    id_prac,
    nazwisko,
    placa_pod + coalesce(placa_dod, 0),
    'lat: ' || extract(year from zatrudniony) || ' miesięcy: ' || extract(month from zatrudniony)
from pracownicy
where etat = 'ASYSTENT';

select * from asystenci order by nazwisko;

-- ex 2

create or replace view place (
    id_zesp, srednia, minimum, maximum,
    fundusz, l_pensji, l_dodatkow
) as select
    z.id_zesp,
    avg(placa_pod),
    min(placa_pod),
    max(placa_pod),
    sum(placa_pod) + sum(placa_dod),
    count(placa_pod),
    count(placa_dod)
from pracownicy p join zespoly z on p.id_zesp = z.id_zesp
group by z.id_zesp
;

select * from place order by id_zesp;
    
-- ex 3

select nazwisko, placa_pod
from pracownicy p join place x on p.id_zesp = x.id_zesp
where p.placa_pod < x.srednia
order by nazwisko;

-- ex 4

create or replace view place_minimalne
as select id_prac, nazwisko, etat, placa_pod
from pracownicy
where placa_pod < 700
with check option constraint chk_placa_pod;

select * from place_minimalne order by nazwisko;

-- ex 5

update place_minimalne
set placa_pod = 800
where nazwisko = 'HAPKE';

-- ex 6

create or replace view prac_szef (
    id_prac, id_szefa, pracownik, etat, szef
) as select p.id_prac, p.id_szefa, p.nazwisko, p.etat, s.nazwisko
from pracownicy p join pracownicy s on p.id_szefa = s.id_prac
;

insert into prac_szef (id_prac, id_szefa, pracownik, etat) values (280, 150, 'MORZY', 'ASYSTENT');
update prac_szef set id_szefa = 130 where id_prac = 280;
delete from prac_szef where id_prac = 280;

-- ex 7

create or replace view zarobki
as select id_prac, id_szefa, nazwisko, etat, placa_pod
from pracownicy p
where placa_pod <= (select placa_pod from pracownicy where id_prac = p.id_szefa)
with check option constraint chk_zarobki
;

update zarobki set placa_pod = 2000 where nazwisko = 'BIALY';

select * from zarobki order by nazwisko;

-- ex 8

select column_name, updatable, insertable, deletable
from user_updatable_columns
where table_name = 'PRAC_SZEF';
