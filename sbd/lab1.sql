select * from zespoly;

select * from prownicy;

select nazwisko, placa_pod * 12 from pracownicy;

select etat, placa_pod + coalesce(placa_dod, 0) as miesieczne_zarobki from pracownicy;

select id_zesp, nazwa, adres from zespoly order by nazwa;

select unique nazwa from etaty;

select * from etaty where nazwa = 'ASYSTENT';

select id_prac, nazwisko, etat, placa_pod, id_zesp from pracownicy where id_zesp in (30, 40) order by placa_pod desc;

select nazwisko, id_zesp, placa_pod from pracownicy where placa_pod between 300 and 800;

select nazwisko, etat, id_zesp from pracownicy where nazwisko like '%SKI';

select id_prac, id_szefa, nazwisko, placa_pod from pracownicy where id_szefa is not null and placa_pod > 1000;

select nazwisko, id_zesp from pracownicy where (nazwisko like 'M%' or nazwisko like '%SKI') and id_zesp = 20;

select nazwisko, etat, placa_pod/20/8 as stawka from pracownicy where etat not in ('ADIUNKT', 'STAZYSTA', 'ASYSTENT') and placa_pod not between 400 and 800 order by stawka;

select nazwisko, etat, placa_pod, placa_dod from pracownicy where placa_pod + coalesce(placa_dod, 0) > 1000 order by etat, nazwisko;

select nazwisko || ' pracuje od ' || zatrudniony || ' i zarabia ' || placa_pod as profesorowie from pracownicy where etat = 'PROFESOR' order by placa_pod desc;

