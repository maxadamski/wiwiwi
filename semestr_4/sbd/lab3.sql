-- etaty, pracownicy, zespoly
select min(placa_pod), max(placa_pod), max(placa_pod) - min(placa_pod) from pracownicy;

select etat, avg(placa_pod) as srednia from pracownicy group by etat order by srednia desc;

select count(*) as profesorowie from pracownicy group by etat having etat = 'PROFESOR';

select id_zesp, sum(placa_pod) + sum(placa_dod) as sumaryczne from pracownicy group by id_zesp order by id_zesp;

select max(sum(placa_pod) + sum(placa_dod)) as max_sum_placa from pracownicy group by id_zesp;

select id_szefa, min(placa_pod) as minimalna from pracownicy group by id_szefa order by id_szefa;

select id_zesp, count(*) as ilu_pracuje from pracownicy group by id_zesp order by ilu_pracuje desc;

select id_zesp, count(*) as ilu_pracuje from pracownicy group by id_zesp having count(*) > 3 order by ilu_pracuje desc;

select id_prac, count(*) as duplikaty from pracownicy group by id_prac having count(*) > 1;

select etat, avg(placa_pod) as srednia, count(*) as liczba from pracownicy where extract(year from zatrudniony) <= 1990 group by etat order by etat;

select id_zesp, etat, avg(placa_pod + placa_dod) as srednia, max(placa_pod + placa_dod)
from pracownicy
where etat in ('PROFESOR', 'ASYSTENT')
group by id_zesp, etat
order by id_zesp, etat;

select extract(year from zatrudniony) as rok, count(*) as ilu_pracownikow
from pracownicy
group by extract(year from zatrudniony)
order by rok;

select length(nazwisko) as ile_liter, count(*) as w_ilu
from pracownicy
group by length(nazwisko)
order by ile_liter;

select
	count(case when instr(nazwisko, 'A', 1, 1) > 0 then 1 else null end) as ile_a,
	count(case when instr(nazwisko, 'E', 1, 1) > 0 then 1 else null end) as ile_b
from pracownicy;

select
	id_zesp, sum(placa_pod) as suma_plac,
	listagg(nazwisko || ':' || placa_pod, ',') 
	within group (order by nazwisko) as pracownicy
from pracownicy
group by id_zesp
order by id_zesp;

