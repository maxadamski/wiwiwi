-- pracownicy: id_prac, nazwisko, etat, id_szefa, zatrudniony, placa_pod/dod, id_zesp

select nazwisko, etat, id_zesp
from pracownicy
where id_zesp = (
	select id_zesp
	from pracownicy
	where nazwisko = 'BRZEZINSKI'
)
order by nazwisko;

select p.nazwisko, p.etat, z.nazwa
from pracownicy p join zespoly z on p.id_zesp = z.id_zesp
where p.id_zesp = (
	select id_zesp
	from pracownicy
	where nazwisko = 'BRZEZINSKI'
)
order by p.nazwisko;

select nazwisko, etat, zatrudniony
from pracownicy
where 
etat = 'PROFESOR' and
zatrudniony = (
	select min(zatrudniony)
	from pracownicy
  	where etat = 'PROFESOR'
);

select nazwisko, zatrudniony, id_zesp
from pracownicy
where zatrudniony in (
	select max(zatrudniony)
	from pracownicy
	group by id_zesp
)
order by zatrudniony;

select nazwa from zespoly
where id_zesp is not null and id_zesp not in (
	select distinct id_zesp
	from pracownicy
);

select nazwisko
from pracownicy
where etat = 'PROFESOR' and id_prac not in (
	select id_szefa
	from pracownicy
	where etat = 'STAZYSTA'
)

select p.id_zesp, sum(p.placa_pod) as suma_plac
from pracownicy p inner join zespoly z on p.id_zesp = z.id_zesp
group by p.id_zesp
having sum(p.placa_pod) = (
	select max(sum(p.placa_pod))
	from pracownicy p inner join zespoly z on p.id_zesp = z.id_zesp
	group by p.id_zesp
)
order by suma_plac desc;

select z.nazwa, sum(p.placa_pod) as suma_plac
from pracownicy p inner join zespoly z on p.id_zesp = z.id_zesp
group by z.id_zesp, z.nazwa
having sum(p.placa_pod) = (
	select max(sum(p.placa_pod))
	from pracownicy p inner join zespoly z on p.id_zesp = z.id_zesp
	group by p.id_zesp
)
order by suma_plac desc;

select z.nazwa, count(*) as ilu_pracownikow
from pracownicy p inner join zespoly z on p.id_zesp = z.id_zesp
group by z.id_zesp, z.nazwa
having count(*) > (
	select count(*)
	from pracownicy p inner join zespoly z on p.id_zesp = z.id_zesp
	group by z.id_zesp, z.nazwa
	having z.nazwa = 'ADMINISTRACJA'
)
order by z.nazwa;

select etat
from pracownicy
group by etat
having count(*) = (
	select max(count(*))
	from pracownicy
	group by etat
);

select etat, listagg(nazwisko, ',') within group (order by nazwisko) as pracownicy
from pracownicy
group by etat
having count(*) = (
	select max(count(*))
	from pracownicy
	group by etat
);

select p.nazwisko as pracownik, s.nazwisko as szef
from pracownicy p inner join pracownicy s on p.id_szefa = s.id_prac
where s.placa_pod - p.placa_pod = (
	select min(abs(s.placa_pod - p.placa_pod))
	from pracownicy p inner join pracownicy s on p.id_szefa = s.id_prac
);

