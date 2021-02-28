import mysql.connector as mariadb
from flask import Flask, request, jsonify
from flask import request as req


def lst(x):
    return x.split(',')

def head(xs):
    return list(xs.values())[0]

def make_insert(table, fields):
    wild = ','.join('%s' for _ in range(len(fields.split(','))))
    return f'INSERT INTO {table} ({fields}) VALUES ({wild})'

def fetchall():
    columns = [x[0] for x in cursor.description]
    rows = cursor.fetchall()
    return [dict(zip(columns, row)) for row in rows]

def fetchall_one():
    rows = cursor.fetchall()
    return [x for x, in rows]

def fetchone():
    columns = [x[0] for x in cursor.description]
    row = cursor.fetchone()
    if row is None: return None
    return dict(zip(columns, row))

def fetchid():
    cursor.execute('SELECT last_insert_id()')
    return cursor.fetchone()[0]

def get_values(keys):
    return [req.json.get(x, None) for x in lst(keys)]

def error(msg):
    return jsonify(dict(status='error', reason=msg))

def ok(msg='', **kwargs):
    return jsonify(dict(status='ok', message=msg, **kwargs))

def safe_insert(query, args=[], return_id=False):
    new_id = True
    try:
        cursor.execute(query, args)
        if return_id:
            new_id = fetchid()
    except mariadb.IntegrityError as e:
        print(f'[error] {e}')
        return None
    db.commit()
    return new_id

def safe_delete(query, args=[]):
    try:
        cursor.execute(query, args)
    except:
        return None
    db.commit()
    return True

def select_first(query, args=[], on_error='', on_ok='', apply=None):
    cursor.execute(query, args)
    row = fetchone()
    if row is None: return error(on_error)
    if apply is not None: row = apply(row)
    return ok(on_ok, **row)

def select(query, args=[], apply=None, key='id', on_error='', on_ok=''):
    try:
        cursor.execute(query, args)
    except:
        return error(on_error)
    rows = fetchall()
    if apply is not None: rows = [apply(row) for row in rows]
    args = {}
    args[key] = rows
    return ok(on_ok, **args)

def insert(query, args=[], on_error='', on_ok='', id=None):
    return_id = id is not None
    new_id = safe_insert(query, args, return_id=return_id)
    if new_id is None: return error(on_error)
    args = {}
    if return_id: args[id] = new_id
    return ok(on_ok, **args)

def delete(query, args=[], on_error='', on_ok=''):
    status = safe_delete(query, args)
    if status is None: return error(on_error)
    return ok(on_ok)

def encode_decimals(row, cols):
    for col in cols:
        row[col] = f"{row[col]:.2f}"
    return row

def rebate_encode(row):
    return encode_decimals(row, ['znizka'])

def encode_price(row):
    return encode_decimals(row, ['cena'])

def product_encode(row):
    row['cena'] = f"{row['cena']:.2f}"
    return row

app = Flask(__name__)
db = mariadb.connect(host='localhost', user='root', password='root')
cursor = db.cursor()
cursor.execute('use zbd')


###############################################################################
# Użytkownicy
###############################################################################

insert_user = make_insert('uzytkownik', 'imie,nazwisko,email,telefon')

@app.route('/api/users', methods=['GET', 'POST'])
def users():
    if request.method == 'POST':
        return insert(insert_user, get_values('imie,nazwisko,email,telefon'), id='id',
                on_ok='Dodano użytkownika')
    else:
        return select('SELECT id FROM uzytkownik', key='id', apply=head)

@app.route('/api/users/<int:user_id>', methods=['GET', 'DELETE'])
def user(user_id):
    if request.method == 'DELETE':
        delete('DELETE FROM uzytkownik WHERE id = %s', [user_id], on_error='Usunięto użytkownika')
    else:
        cursor.execute('SELECT * FROM uzytkownik WHERE id = %s', [user_id])
        row = fetchone()
        if row is None: return error(f'Użytkownik {user_id} nie istnieje')
        cursor.execute('SELECT * FROM adres WHERE uzytkownik_id = %s', [user_id])
        row['adresy'] = fetchall()
        return ok(**row)


###############################################################################
# Adresy
###############################################################################

insert_addr = make_insert('adres', 'uzytkownik_id,ulica,kod,miejsc,kraj,imie,nazwisko,telefon,firma')

@app.route('/api/users/<int:user_id>/addresses', methods=['GET', 'POST'])
def addresses(user_id):
    if request.method == 'POST':
        values = [user_id] + get_values('ulica,kod,miejscowosc,kraj,imie,nazwisko,telefon,firma')
        return insert(insert_addr, values, id='id', on_error='Ten użytkownik nie istnieje')
    else:
        return select('SELECT id FROM adres WHERE uzytkownik_id = %s', [user_id], key='id', apply=head)

@app.route('/api/users/<int:user_id>/addresses/<int:addr_id>', methods=['GET', 'DELETE'])
def address(user_id, addr_id):
    if req.method == 'DELETE':
        return delete('DELETE FROM adres WHERE id = %s', [addr_id],
                on_error=f'Użytkownik {user_id} nie ma adresu {addr_id}', on_ok='Usunięto adres')
    else:
        return select_first('SELECT * FROM adres WHERE id = %s AND uzytkownik_id = %s', [addr_id, user_id],
                on_error=f'Użytkownik {user_id} nie ma adresu {addr_id}')


###############################################################################
# Kategorie
###############################################################################

insert_category = make_insert('kategoria', 'nazwa,rodzic,opis')

@app.route('/api/categories', methods=['GET', 'POST'])
def categories():
    if request.method == 'POST':
        return insert(insert_category, get_values('nazwa,rodzic,opis'),
                on_error='Rodzic kategorii nie istnieje', on_ok='Dodano kategorię')
    else:
        return select('SELECT * FROM kategoria', key='kategoria')

@app.route('/api/categories/<nazwa>', methods=['GET', 'DELETE'])
def category(nazwa):
    if req.method == 'DELETE':
        return delete('DELETE FROM kategoria WHERE nazwa = %s', [nazwa],
                on_error=f'Kategoria {nazwa} nie istnieje', on_ok='Usunięto kategorię')
    else:
        return select_first('SELECT * FROM kategoria WHERE nazwa = %s', [nazwa],
                on_error=f'Kategoria {nazwa} nie istnieje')


###############################################################################
# Rabaty
###############################################################################

insert_rebate = make_insert('rabat', 'kod,znizka,opis')

@app.route('/api/rebates', methods=['GET', 'POST'])
def rebates():
    if request.method == 'POST':
        return insert(insert_rebate, get_values('kod,znizka,opis'),
                on_error='Rabat o tej nazwie już istnieje', on_ok="Dodano rabat")
    else:
        return select('SELECT * FROM rabat', key='rabat', apply=rebate_encode)

@app.route('/api/rebates/<kod>', methods=['GET', 'DELETE'])
def rebate(kod):
    if req.method == 'DELETE':
        return delete('DELETE FROM rabat WHERE kod = %s', [kod],
                on_error=f'Rabat {kod} nie istnieje', on_ok='Usunięto rabat')
    else:
        return select_first('SELECT * FROM rabat WHERE kod = %s', [kod], apply=rebate_encode,
                on_error=f'Rabat {kod} nie istnieje')


###############################################################################
# Produkt
###############################################################################

insert_product = make_insert('produkt', 'nazwa,cena,kategoria,opis')

@app.route('/api/products', methods=['GET', 'POST'])
def products():
    if request.method == 'POST':
        return insert(insert_product, get_values('nazwa,cena,kategoria,opis'), id='id',
                on_error='Kategoria nie istnieje', on_ok='Dodano produkt')
    else:
        return select('SELECT * FROM produkt', key='produkt', apply=product_encode)

@app.route('/api/products/<int:product_id>', methods=['GET', 'DELETE'])
def product(product_id):
    if req.method == 'DELETE':
        return delete('DELETE FROM produkt WHERE id = %s', [product_id],
                on_error=f'Produkt {product_id} nie istnieje', on_ok='Usunięto produkt')
    else:
        return select_first('SELECT * FROM produkt WHERE id = %s', [product_id], apply=product_encode,
                on_error=f'Produkt {product_id} nie istnieje')


###############################################################################
# Atrybut
###############################################################################

insert_attribute = make_insert('atrybut', 'produkt_id,nazwa,wartosc')

@app.route('/api/products/<int:product_id>/attributes', methods=['GET', 'POST'])
def attributes(product_id):
    if request.method == 'POST':
        return insert(insert_attribute, [product_id] + get_values('nazwa,wartosc'),
                on_error=f'Ten atrybut jest już zdefiniowany dla produktu {product_id}', on_ok='Dodano atrybut')
    else:
        return select('SELECT * FROM atrybut WHERE produkt_id = %s', [product_id], key='atrybut')

@app.route('/api/products/<int:product_id>/attributes/<nazwa>', methods=['GET', 'DELETE'])
def attribute(product_id, nazwa):
    if req.method == 'DELETE':
        return delete('DELETE FROM atrybut WHERE produkt_id = %s AND nazwa = %s', [product_id, nazwa],
                on_error=f'Produkt {product_id} nie ma atrybutu {nazwa}', on_ok='Usunięto atrybut')
    else:
        return select_first('SELECT * FROM atrybut WHERE produkt_id = %s AND nazwa = %s', [product_id, nazwa],
                on_error=f'Produkt {product_id} nie ma atrybutu {nazwa}')


###############################################################################
# Tag
###############################################################################

insert_tag = make_insert('tag', 'nazwa,produkt_id')

@app.route('/api/products/<int:product_id>/tags', methods=['GET', 'POST'])
def tags(product_id):
    if request.method == 'POST':
        try:
            cursor.execute('CALL InsertTag(%s, %s)', [req.json['nazwa'], product_id])
            db.commit()
            return ok('Dodano tag dla produktu')
        except Exception as e:
            print(e)
            return error('Produkt już posiada ten tag')
    else:
        return select('SELECT tag_nazwa FROM tag_produkt WHERE produkt_id = %s', [product_id], key='tag')

@app.route('/api/products/<int:product_id>/tags/<nazwa>', methods=['GET', 'DELETE'])
def tag(product_id, nazwa):
    if req.method == 'DELETE':
        return delete('DELETE FROM tag_produkt WHERE produkt_id = %s AND tag_nazwa = %s', [product_id, nazwa],
                on_error=f'Produkt {product_id} nie ma tagu {nazwa}', on_ok=f'Usunięto tag {nazwa} dla produktu')
    else:
        return select_first('SELECT * FROM tag_produkt WHERE produkt_id = %s AND tag_nazwa = %s', [product_id, nazwa],
                on_error=f'Produkt {product_id} nie ma tagu {nazwa}')


###############################################################################
# Metoda dostawy
###############################################################################

insert_delivery = make_insert('metoda_dostawy', 'nazwa,cena')

@app.route('/api/delivery', methods=['GET', 'POST'])
def deliveries():
    if request.method == 'POST':
        return insert(insert_delivery, get_values('nazwa,cena'),
                on_error='Metoda dostawy o tej nazwie już istnieje', on_ok='Dodano metodę dostawy')
    else:
        return select('SELECT * FROM metoda_dostawy', key='metoda_dostawy', apply=encode_price)

@app.route('/api/delivery/<nazwa>', methods=['GET', 'DELETE'])
def delivery(nazwa):
    if req.method == 'DELETE':
        return delete('DELETE FROM metoda_dostawy WHERE nazwa = %s', [nazwa],
                on_error=f'Metoda dostawy {nazwa} nie istniejje', on_ok=f'Usunięto metodę dostawy {nazwa}')
    else:
        return select_first('SELECT * FROM metoda_dostawy WHERE nazwa = %s', [nazwa], apply=encode_price,
                on_error=f'Metoda dostawy {nazwa} nie istnieje')


###############################################################################
# Punkt dostawy
###############################################################################

insert_delivery_point = make_insert('punkt_dostawy', 'metoda_dostawy_nazwa,ulica,kod,miejsc,kraj')

@app.route('/api/delivery/<nazwa>/point', methods=['GET', 'POST'])
def delivery_points(nazwa):
    if request.method == 'POST':
        return insert(insert_delivery_point, [nazwa] + get_values('ulica,kod,miejsc,kraj'), id='id',
                on_error='Punkt dostawy z takim adresem już istnieje dla tej metody dostawy', on_ok='Dodano punkt dostawy')
    else:
        return select('SELECT * FROM punkt_dostawy WHERE metoda_dostawy_nazwa = %s', [nazwa], key='punkt')

@app.route('/api/delivery/<nazwa>/point/<int:id>', methods=['GET', 'DELETE'])
def delivery_point(nazwa, id):
    if req.method == 'DELETE':
        return delete('DELETE FROM punkt_dostawy WHERE id = %s', [id],
                on_error=f'Metoda dostawy {nazwa} nie ma punktu {id}', on_ok='Usunięto punkt dostawy')
    else:
        return select_first('SELECT * FROM punkt_dostawy WHERE metoda_dostawy_nazwa = %s AND id = %s', [nazwa, id],
                on_error=f'Metoda dostawy {nazwa} nie ma punktu {id}')


###############################################################################
# Zamówienia
###############################################################################

insert_order = make_insert('zamowienie', 'uzytkownik_id,data_zlozenia,data_zaplaty,data_dostarczenia,rabat_kod,metoda_dost,punkt_dost,adres_id')
insert_order_product = make_insert('zamowienie_produkt', 'zamowienie_id,produkt_id,ilosc,cena')
delete_order = 'DELETE FROM zamowienie WHERE id = %s'

@app.route('/api/users/<int:user_id>/orders', methods=['GET', 'POST'])
def orders(user_id):
    if request.method == 'POST':
        fields = 'data_zlozenia,data_zaplaty,data_dostarczenia,rabat_kod,metoda_dost,punkt_dost,adres_id'
        order_id = safe_insert(insert_order, [user_id]+get_values(fields), return_id=True)
        if order_id is None: return error('Wprowadzono niespójne dane zamówienia (zły kod rabatowy, adres, metoda lub punkt dostawy)')

        for p in req.json['produkty']:
            status = safe_insert(insert_order_product, [order_id, p['produkt_id'], p['ilosc'], p['cena']])
            if status is None: return error('Produkt nie istnieje')

        return ok('Dodano zamówienie', id=order_id)

    else:
        return select('SELECT id FROM zamowienie WHERE uzytkownik_id = %s', [user_id], key='zamowienie')

@app.route('/api/users/<int:user_id>/orders/<int:order_id>', methods=['GET', 'DELETE'])
def order(user_id, order_id):
    if req.method == 'DELETE':
        return delete(delete_order, [order_id], on_error='Zamówienie nie istnieje', on_ok='Usunięto zamówienie')
    else:
        cursor.execute('SELECT * FROM zamowienie WHERE id = %s', [order_id])
        order = fetchone()
        if order is None: return error(f'Zamówienie {order_id} użytkownika {user_id} nie istnieje')
        cursor.execute('SELECT * FROM zamowienie_produkt WHERE zamowienie_id = %s', [order_id])
        products = [encode_price(x) for x in fetchall()]
        order['produkty'] = products
        return ok(**order)


###############################################################################
# Koszyk
###############################################################################

insert_cart = 'INSERT INTO koszyk(uzytkownik_id,data_waznosci) VALUES (%s, NOW() + INTERVAL 7 DAY)'
insert_cart_product = make_insert('koszyk_produkt', 'uzytkownik_id,produkt_id,ilosc,cena')

@app.route('/api/users/<int:user_id>/cart', methods=['GET', 'POST', 'DELETE'])
def cart(user_id):
    if req.method == 'DELETE':
        return delete('DELETE FROM koszyk WHERE uzytkownik_id = %s', [user_id],
                on_error='Koszyk lub użytkownik nie istnieje')

    elif req.method == 'POST':
        try:
            cursor.execute(insert_cart, [user_id])
        except mariadb.IntegrityError as e:
            return error(f'Koszyk już istnieje lub użytkownik nie istnieje')

        for p in req.json['produkty']:
            try:
                cursor.execute(insert_cart_product, [user_id, p['produkt_id'], p['ilosc'], p['cena']])
            except mariadb.IntegrityError:
                return error(f'Produkt nie istnieje')

        db.commit()
        return ok()

    else:
        cursor.execute('SELECT * FROM koszyk WHERE uzytkownik_id = %s', [user_id])
        cart = fetchone()
        if cart is None: return error(f'Koszyk użytkownika {user_id} nie istnieje')
        cursor.execute('SELECT * FROM koszyk_produkt WHERE uzytkownik_id = %s', [user_id])
        products = [encode_price(x) for x in fetchall()]
        cart['produkty'] = products
        return jsonify(cart)


###############################################################################
# Punkt wejścia
###############################################################################

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080, debug=True)

