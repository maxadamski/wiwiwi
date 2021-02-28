select p.nazwisko, p.etat, p.id_zesp, z.nazwa
from pracownicy p inner join zespoly z on p.id_zesp = z.id_zesp
order by p.nazwisko;<Paste>

select p.nazwisko, p.etat, p.id_zesp, z.nazwa
from pracownicy p inner join zespoly z on p.id_zesp = z.id_zesp
where z.adres = 'PIOTROWO 3A'
order by p.nazwisko;

select p.nazwisko, p.etat, p.placa_pod, e.placa_min, e.placa_max
from pracownicy p inner join etaty e on p.etat = e.nazwa
order by p.nazwisko;

select p.nazwisko, p.etat, p.placa_pod, e.placa_min, e.placa_max,
	case when e.placa_min <= p.placa_pod and p.placa_pod <= e.placa_max then 'OK' else 'ERR' end as czy_pensja_ok
from pracownicy p inner join etaty e on p.etat = e.nazwa
order by p.nazwisko;

select p.nazwisko, p.etat, p.placa_pod, e.placa_min, e.placa_max
from pracownicy p inner join etaty e on p.etat = e.nazwa
where p.placa_pod < e.placa_min or p.placa_pod > e.placa_max
order by p.nazwisko;

select p.nazwisko, p.etat, e.nazwa as kat_plac, p.placa_pod, e.placa_min, e.placa_max
from pracownicy p inner join etaty e on e.placa_min <= p.placa_pod and p.placa_pod <= e.placa_max
order by p.nazwisko, kat_plac;

select p.nazwisko, p.etat, e.nazwa as kat_plac, p.placa_pod, e.placa_min, e.placa_max
from pracownicy p inner join etaty e on e.placa_min <= p.placa_pod and p.placa_pod <= e.placa_max
where e.nazwa = 'SEKRETARKA'
order by p.nazwisko, kat_plac;

select p.nazwisko, p.id_prac, s.nazwisko as szef, s.id_prac as id_szefa
from pracownicy p inner join pracownicy s on p.id_szefa = s.id_prac
order by p.nazwisko;

select
	p.nazwisko as pracownik, p.zatrudniony as p_zat, s.nazwisko as szef, s.zatrudniony as s_zat,
	extract(year from p.zatrudniony) - extract(year from s.zatrudniony) as lata
from pracownicy p inner join pracownicy s on p.id_szefa = s.id_prac
where extract(year from p.zatrudniony) - extract(year from s.zatrudniony) < 10
order by lata, p.nazwisko;

select z.nazwa, count(*) as liczba_prac, avg(p.placa_pod) as srednia_placa
from pracownicy p inner join zespoly z on p.id_zesp = z.id_zesp
group by z.nazwa
order by z.nazwa;

select
	z.nazwa, 
	case
		when count(*) >= 7 then 'big'
		when count(*) >= 3 then 'medium'
		else 'small'
	end as etykieta
from pracownicy p inner join zespoly z on p.id_zesp = z.id_zesp
group by z.nazwa
having count(*) > 0
order by z.nazwa;

