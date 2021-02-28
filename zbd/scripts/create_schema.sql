DROP DATABASE IF EXISTS zbd;
CREATE DATABASE zbd;
USE zbd;

CREATE OR REPLACE TABLE uzytkownik (
    id          INTEGER NOT NULL AUTO_INCREMENT PRIMARY KEY,
    email       VARCHAR(50) NOT NULL,
    imie        VARCHAR(50),
    nazwisko    VARCHAR(50),
    telefon     VARCHAR(25)
);

CREATE OR REPLACE TABLE adres (
    id            INTEGER NOT NULL AUTO_INCREMENT PRIMARY KEY,
    ulica         VARCHAR(50) NOT NULL,
    kod           VARCHAR(10) NOT NULL,
    miejsc        VARCHAR(50) NOT NULL,
    kraj          VARCHAR(25) NOT NULL,
    imie          VARCHAR(50) NOT NULL,
    nazwisko      VARCHAR(50) NOT NULL,
    telefon       VARCHAR(25) NOT NULL,
    firma         VARCHAR(50),
    uzytkownik_id INTEGER NOT NULL
);

CREATE OR REPLACE TABLE produkt (
    id              INTEGER       NOT NULL AUTO_INCREMENT PRIMARY KEY,
    nazwa           VARCHAR(100)  NOT NULL,
    cena            DECIMAL(10,2) NOT NULL,
    kategoria       VARCHAR(25),
    opis            TEXT
);

CREATE OR REPLACE TABLE atrybut (
    nazwa      VARCHAR(20) NOT NULL,
    wartosc    VARCHAR(20) NOT NULL,
    produkt_id INTEGER     NOT NULL
);

ALTER TABLE atrybut ADD CONSTRAINT atrybut_pk PRIMARY KEY ( nazwa, produkt_id );

CREATE OR REPLACE TABLE tag (
    nazwa      VARCHAR(25) NOT NULL PRIMARY KEY
);

CREATE OR REPLACE TABLE tag_produkt (
    tag_nazwa  VARCHAR(25) NOT NULL,
    produkt_id INTEGER     NOT NULL
);

ALTER TABLE tag_produkt ADD CONSTRAINT tag_produkt_pk PRIMARY KEY ( tag_nazwa, produkt_id );

CREATE OR REPLACE TABLE kategoria (
    nazwa           VARCHAR(25) NOT NULL PRIMARY KEY,
    rodzic          VARCHAR(25),
    opis            TEXT
);

CREATE OR REPLACE TABLE metoda_dostawy (
    nazwa VARCHAR(25)   NOT NULL PRIMARY KEY,
    cena  DECIMAL(10,2) NOT NULL
);

CREATE OR REPLACE TABLE punkt_dostawy (
    id                   INTEGER     NOT NULL AUTO_INCREMENT PRIMARY KEY,
    ulica                VARCHAR(50) NOT NULL,
    kod                  VARCHAR(10) NOT NULL,
    miejsc               VARCHAR(50) NOT NULL,
    kraj                 VARCHAR(25) NOT NULL,
    metoda_dostawy_nazwa VARCHAR(25) NOT NULL
);

CREATE OR REPLACE TABLE rabat (
    kod    VARCHAR(10)   NOT NULL PRIMARY KEY,
    znizka DECIMAL(10,2) NOT NULL,
    opis   TEXT
);

CREATE OR REPLACE TABLE koszyk (
    uzytkownik_id INTEGER  NOT NULL PRIMARY KEY,
    data_waznosci DATETIME NOT NULL
);

CREATE OR REPLACE TABLE koszyk_produkt (
    uzytkownik_id    INTEGER NOT NULL,
    produkt_id       INTEGER NOT NULL,
    ilosc            INTEGER NOT NULL,
    cena             DECIMAL(10,2) NOT NULL
);

ALTER TABLE koszyk_produkt ADD CONSTRAINT koszyk_produkt_pk PRIMARY KEY ( uzytkownik_id, produkt_id );

CREATE OR REPLACE TABLE zamowienie (
    id                   INTEGER  NOT NULL AUTO_INCREMENT PRIMARY KEY,
    uzytkownik_id        INTEGER  NOT NULL,
    data_zlozenia        DATETIME NOT NULL,
    data_zaplaty         DATETIME,
    data_dostarczenia    DATETIME,
    rabat_kod            VARCHAR(10),
    metoda_dost          VARCHAR(25),
	punkt_dost           INTEGER,
    adres_id             INTEGER
);

CREATE OR REPLACE TABLE zamowienie_produkt (
    produkt_id               INTEGER        NOT NULL,
	zamowienie_id            INTEGER        NOT NULL,
    ilosc                    INTEGER        NOT NULL,
    cena                     DECIMAL(10,2)  NOT NULL
);

ALTER TABLE zamowienie_produkt ADD CONSTRAINT zamowienie_produkt_pk PRIMARY KEY (produkt_id, zamowienie_id);

ALTER TABLE adres ADD CONSTRAINT adres_uzytkownik_fk FOREIGN KEY ( uzytkownik_id ) REFERENCES uzytkownik ( id ) ON DELETE CASCADE;
ALTER TABLE produkt ADD CONSTRAINT produkt_kategoria_fk FOREIGN KEY ( kategoria ) REFERENCES kategoria ( nazwa ) ON DELETE CASCADE;
ALTER TABLE atrybut ADD CONSTRAINT atrybut_produkt_fk FOREIGN KEY ( produkt_id ) REFERENCES produkt ( id ) ON DELETE CASCADE;

ALTER TABLE tag_produkt ADD CONSTRAINT tp_produkt_fk FOREIGN KEY ( produkt_id ) REFERENCES produkt ( id ) ON DELETE CASCADE;
ALTER TABLE tag_produkt ADD CONSTRAINT tp_tag_fk FOREIGN KEY ( tag_nazwa ) REFERENCES tag ( nazwa ) ON DELETE CASCADE;

ALTER TABLE kategoria ADD CONSTRAINT kategoria_kategoria_fk FOREIGN KEY ( rodzic ) REFERENCES kategoria ( nazwa ) ON DELETE SET NULL;

ALTER TABLE punkt_dostawy ADD CONSTRAINT produkt_dostawy_metoda_dostawy_fk FOREIGN KEY ( metoda_dostawy_nazwa ) REFERENCES metoda_dostawy ( nazwa ) ON DELETE CASCADE;

ALTER TABLE koszyk ADD CONSTRAINT koszyk_uzytkownik_fk FOREIGN KEY ( uzytkownik_id ) REFERENCES uzytkownik ( id ) ON DELETE CASCADE;
ALTER TABLE koszyk_produkt ADD CONSTRAINT koszyk_produkt_uzytkownik_fk FOREIGN KEY ( uzytkownik_id ) REFERENCES uzytkownik ( id ) ON DELETE CASCADE;
ALTER TABLE koszyk_produkt ADD CONSTRAINT koszyk_produkt_produkt_fk FOREIGN KEY ( produkt_id ) REFERENCES produkt ( id ) ON DELETE CASCADE;

ALTER TABLE zamowienie ADD CONSTRAINT zamowienie_uzytkownik_fk FOREIGN KEY ( uzytkownik_id ) REFERENCES uzytkownik ( id ) ON DELETE CASCADE;
ALTER TABLE zamowienie ADD CONSTRAINT zamowienie_metoda_dostawy_fk FOREIGN KEY ( metoda_dost ) REFERENCES metoda_dostawy ( nazwa ) ON DELETE SET NULL;
ALTER TABLE zamowienie ADD CONSTRAINT zamowienie_punkt_dostawy_fk FOREIGN KEY ( punkt_dost ) REFERENCES punkt_dostawy ( id ) ON DELETE SET NULL;
ALTER TABLE zamowienie ADD CONSTRAINT zamowienie_adres_fk FOREIGN KEY ( adres_id ) REFERENCES adres ( id ) ON DELETE SET NULL;
ALTER TABLE zamowienie ADD CONSTRAINT zamowienie_rabat_fk FOREIGN KEY ( rabat_kod ) REFERENCES rabat ( kod ) ON DELETE SET NULL;

ALTER TABLE zamowienie_produkt ADD CONSTRAINT zamowienie_produkt_produkt_fk FOREIGN KEY ( produkt_id ) REFERENCES produkt ( id ) ON DELETE CASCADE;
ALTER TABLE zamowienie_produkt ADD CONSTRAINT zamowienie_produkt_zamowienie_fk FOREIGN KEY ( zamowienie_id ) REFERENCES zamowienie ( id ) ON DELETE CASCADE;

DELIMITER //

CREATE OR REPLACE TRIGGER trig_koszyk
    AFTER UPDATE ON koszyk_produkt FOR EACH ROW
    UPDATE koszyk SET koszyk.data_waznosci = (NOW() + INTERVAL 7 DAY) WHERE koszyk.uzytkownik_id = NEW.uzytkownik_id;
//

CREATE OR REPLACE TRIGGER delete_tag AFTER DELETE ON tag_produkt FOR EACH ROW
BEGIN
	DECLARE vCount INTEGER;
	SELECT count(*) INTO vCount FROM tag_produkt WHERE tag_nazwa = OLD.tag_nazwa;
	IF vCount = 0 THEN
		DELETE FROM tag WHERE nazwa = OLD.tag_nazwa;
	END IF;
END
//

CREATE PROCEDURE InsertTag (vName VARCHAR(25), vProductId INTEGER)
MODIFIES SQL DATA
BEGIN
	INSERT INTO tag (nazwa) VALUES (vName) ON DUPLICATE KEY UPDATE nazwa=vName;
	INSERT INTO tag_produkt (tag_nazwa, produkt_id) VALUES (vName, vProductId);
END
//

