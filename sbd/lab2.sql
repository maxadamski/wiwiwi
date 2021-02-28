select nazwisko, substr(etat, 1, 2) || id_prac as kod from pracownicy;

select nazwisko, translate(nazwisko, 'KLM', 'XXX') as wojna from pracownicy;

select nazwisko from pracownicy where instr(substr(nazwisko, 1, length(nazwisko) / 2), 'L', 1, 1) > 0;

select nazwisko, round(placa_pod * 1.15) as podwyzka from pracownicy;

select nazwisko, to_char(zatrudniony,'YY/DD/MM') as zatrudni, extract(year from (date '2000-01-01' - zatrudniony) year to month) as staz_w_2000 from pracownicy;

select nazwisko, to_char(zatrudniony,'MONTH, DD YYYY') as data_zatrudnienia from pracownicy where id_zesp = 20;

select to_char(current_date, 'DAY') as dzien from dual;

select nazwa, adres,
	case (substr(adres, 1, instr(adres, ' ') - 1))
		when 'PIOTROWO'   then 'NOWE MIASTO'
		when 'WLODKOWICA' then 'GRUNWALD'
		else 'STARE MIASTO'
	end as dzielnica
from zespoly;

select nazwisko, placa_pod,
	case
		when placa_pod < 480 then 'less'
		when placa_pod = 480 then 'equal'
		else 'more'
	end as prog
from pracownicy
order by placa_pod desc;

