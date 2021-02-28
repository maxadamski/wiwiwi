
# Projekt ZBD - Serwer

## API

Każda odpowiedź na zapytanie zawiera pole status=ok/error, i reason, gdzie reason, to
wiadomość dla człowieka, gotowa do wyświetlenia w alercie, dla status=ok,
reason to na przykład "Dodano użytkownika".

Wszystkie zapytania DELETE przyjmują parametr force=true/false.
Jeśli występuje kaskadowe usunięcie i force=false, to dostajesz status=error z informacją.
W przeciwnym wypadku, usuwa i zwraca status=ok z informacją.
Przy zapytaniach SELECT gdzie status=ok, reason jest raczej pusty.


```
REQUEST-TYPE URI [BODY JSON KEYS]

GET,POST   /api/users [imie, nazwisko, email, telefon?]
GET,DELETE /api/users/{id} (cascade address)

GET,POST   /api/users/{id}/addresses [ulica, kod, miejscowosc, kraj, imie, nazwisko, telefon, firma?]
GET,DELETE /api/users/{id}/addresses/{id}

GET,POST   /api/categories [kategoria,rodzic?,opis]
GET,DELETE /api/categories/{id} (cascade categories)

GET,POST   /api/rebates [kod,znizka,opis?]
GET,DELETE /api/rebates/{id}

GET,POST   /api/products [nazwa,cena,kategoria]
GET,DELETE /api/products/{id}

GET,POST   /api/products/{id}/attributes [nazwa,wartosc]
GET,DELETE /api/products/{id}/attributes/{nazwa}

GET,POST   /api/products/{id}/tags [nazwa]
GET,DELETE /api/products/{id}/tags/{nazwa}

GET,POST   /api/delivery [nazwa,cena]
GET,DELETE /api/delivery/{nazwa}

GET,POST   /api/delivery/{nazwa}/point [ulica,kod,miejsc,kraj]
GET,DELETE /api/delivery/{nazwa}/point/{id}

GET,POST   /api/users/{id}/orders <-> Zamówienie
GET,DELETE /api/users/{id}/orders/{order_id}

GET,POST,DELETE /api/users/{id}/cart <-> Koszyk

Koszyk: {
	produkty: [{
		produkt_id,
		ilosc,
		cena
	}]
}

Zamowienie: {
	data_zlozenia,
	data_zaplaty?,
	data_dostarczenia?,
	rabat_kod?,
	metoda_dost?,    # /api/delivery/{metoda_dostawy}
	punkt_dost?,     # /api/delivery/{metoda_dostawy}/point/{punkt_dostawy}
    adres_id?,       # /api/users/{uzytkownik_id}/adres/{adres_id}
	produkty: [{
		produkt_id, # /api/products/{produkt_id}
		ilosc,
		cena
	}]
}
```

