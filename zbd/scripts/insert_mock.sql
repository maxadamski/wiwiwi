USE zbd;

START TRANSACTION;

DELETE FROM uzytkownik;
DELETE FROM kategoria;
DELETE FROM produkt;
DELETE FROM tag;
DELETE FROM tag_produkt;
DELETE FROM atrybut;
DELETE FROM rabat;
DELETE FROM metoda_dostawy;
DELETE FROM punkt_dostawy;
DELETE FROM zamowienie;
DELETE FROM zamowienie_produkt;
DELETE FROM koszyk;
DELETE FROM koszyk_produkt;

INSERT INTO uzytkownik(id, imie, nazwisko, email, telefon) VALUES
    (1, 'Max', 'Adamski', 'max@maxadamski.com', '+48 123 456 789'),
    (2, 'Daman', 'Buda', 'damian.buda@gmail.com', '+48 123 456 789'),
    (3, 'Alicja', 'Blue', 'alicjaz@o2.pl', NULL),
    (4, 'Anita', 'Zielinska', 'anitazielinska@gmail.com', '+48 123 456 789'),
    (5, 'Yoshua', 'Bengio', 'jbengio@mit.edu', NULL),
    (6, 'Yann', 'LeCun', 'lecun@bell.com', '+48 123 456 789');

INSERT INTO adres(id, uzytkownik_id, ulica, kod, miejsc, kraj, imie, nazwisko, telefon, firma) VALUES
    (1, 1, 'Bajkowa 4', '60-000', 'Poznan', 'Polska', 'Max', 'Adamski', '+48 123 456 789', NULL),
    (2, 1, 'Kontenerowa 8/14', '40-000', 'Poznan', 'Polska', 'Max', 'Adamski', '+48 666 666 666', 'Windex sp z o.o.'),
    (3, 2, 'Wiktorianska 20', '13-212', 'Warszawa', 'Polska', 'Alicja', 'Blue', '+48 60 51 28 76', NULL),
    (4, 5, 'Holt st. 40', '123456', 'Montreal', 'Canada', 'Yoshua', 'Bengio', '514 555 5555', NULL);

INSERT INTO kategoria(nazwa, rodzic) VALUES
    ('elektronika', NULL),
    ('moda', NULL),
    ('dom', NULL),
    ('uroda', NULL),
    ('zdrowie', NULL),
    ('kultura', NULL),
    ('smartfony', 'elektronika'),
    ('laptopy', 'elektronika'),
    ('buty', 'moda'),
    ('dresy', 'moda'),
    ('lampy', 'dom'),
    ('kremy', 'uroda'),
    ('szampony', 'uroda'),
    ('soczewki', 'zdrowie'),
    ('filmy', 'kultura');

INSERT INTO produkt(id, nazwa, cena, kategoria) VALUES
    (1,  'Thinkpad X220', 2499.99, 'laptopy'),
    (2,  'Xiaomi Air 13', 1999.99, 'laptopy'),
    (3,  'Buty czarne garniturowe', 200.50, 'buty'),
    (4,  'Buty tenisowe Nike', 150.99, 'buty'),
    (5,  'Huawei P3', 1999.99, 'smartfony'),
    (6,  'Samsung Glaxy S10', 2499.99, 'smartfony'),
    (7,  'Dresy czarne', 49.99, 'dresy'),
    (8,  'Dresy czerwone', 49.99, 'dresy'),
    (9,  'Szampon konopny', 19.99, 'szampony'),
    (10, 'Szampon Bambino', 5.99, 'szampony'),
    (11, 'Acuvue', 59.99, 'soczewki'),
    (12, 'Baush+Lomb', 89.99, 'soczewki'),
    (13, 'Zagubiona Autostrada DVD', 19.99, 'filmy'),
    (14, 'Inland Empire Blue-Ray', 59.99, 'filmy');

CALL InsertTag('biznesowe', 1);
CALL InsertTag('chinskie', 2);
CALL InsertTag('tanie', 2);
CALL InsertTag('biznesowe', 3);
CALL InsertTag('eleganckie', 3);
CALL InsertTag('hit', 10);
CALL InsertTag('klasyk', 13);

INSERT INTO atrybut(produkt_id, nazwa, wartosc) VALUES
    (1,  'Kolor', 'czarny'),
    (1,  'Procesor', 'Intel Core 2 Duo'),
    (2,  'Procesor', 'Intel i5'),
    (2,  'Rozdzielczość', '1920x1080'),
    (3,  'Kolor', 'czarny'),
    (4,  'Kolor', 'zielony'),
    (7,  'Kolor', 'czarny'),
    (8,  'Kolor', 'czerwony'),
    (9,  'Zawiera siarczany', 'nie'),
    (10, 'Zawiera siarczany', 'tak'),
    (11, 'Typ', 'Hydrogel'),
    (13, 'Medium', 'DVD'),
    (14, 'Medium', 'Blue-Ray');

INSERT INTO rabat(kod, znizka) VALUES
    ('SUMMER20', 0.2),
    ('GETXIAOMI', 0.05),
    ('DVD2020', 0.4);

INSERT INTO metoda_dostawy(nazwa, cena) VALUES
    ('DHL-Pobranie', 19.99),
    ('DHL-Karta', 9.99),
    ('Paczkomat-InPost', 5.00);

INSERT INTO punkt_dostawy(id, metoda_dostawy_nazwa, kraj, miejsc, kod, ulica) VALUES
    (1, 'Paczkomat-InPost', 'Polska', 'Poznan', '60-000', 'Ogrodowa 20'),
    (2, 'Paczkomat-InPost', 'Polska', 'Poznan', '60-000', 'Piekna 60'),
    (3, 'Paczkomat-InPost', 'Polska', 'Poznan', '60-123', 'Dworcowa 12'),
    (4, 'Paczkomat-InPost', 'Polska', 'Poznan', '60-520', 'Radosna 2'),
    (5, 'Paczkomat-InPost', 'Polska', 'Poznan', '60-520', 'Ratajczaka 5'),
    (6, 'Paczkomat-InPost', 'Polska', 'Poznan', '60-132', 'Uliczna 1');

INSERT INTO zamowienie(id, uzytkownik_id, data_zlozenia, data_zaplaty, data_dostarczenia, rabat_kod, metoda_dost, punkt_dost, adres_id) VALUES
	(1, 1, date '2020-01-14', NULL, NULL, NULL, 'DHL-Pobranie', NULL, 2),
	(2, 1, date '2019-11-28', date '2019-11-28', date '2019-12-02', 'DVD2020', 'DHL-Karta', NULL, 2),
	(3, 1, date '2019-11-29', date '2019-11-29', date '2019-12-02', NULL, 'DHL-Karta', NULL, 2),
	(4, 1, date '2019-11-29', NULL, NULL, NULL, 'Paczkomat-InPost', 1, NULL),
	(5, 1, date '2019-11-29', NULL, NULL, NULL, 'Paczkomat-InPost', 2, NULL);

INSERT INTO zamowienie_produkt(zamowienie_id, produkt_id, ilosc, cena) VALUES
	(1, 1, 1, 1999.99),
	(1, 2, 2, 2499.99),
	(2, 13, 1, 19.99),
	(3, 14, 1, 59.99),
	(4, 7, 1, 49.99),
	(5, 8, 1, 49.99);

INSERT INTO koszyk(uzytkownik_id, data_waznosci) VALUES
	(1, date '2020-01-31'),
	(2, date '2019-12-01'),
	(3, date '2019-01-12'),
	(4, date '2020-02-10');

INSERT INTO koszyk_produkt(uzytkownik_id,produkt_id,ilosc,cena) VALUES
	(1, 11, 2, 59.99),
	(1, 12, 2, 89.99),
	(2, 6, 1, 2499.99),
	(3, 4, 1, 150.99),
	(4, 14, 1, 59.99);

COMMIT;
