import 'package:sqflite/sqflite.dart';

class DB {
  static String path;
  static Database db;

  static Future<void> open() async {
    path = await getDatabasesPath() + '/boxer.db';
    db = await openDatabase(path, version: 1, onCreate: (db, version) async => init(db));
    print('loaded database');
  }

  static Future<void> close() async {
    await db?.close();
  }

  static Future<void> delete() async {
    if (path == null) return;
    await close();
    await deleteDatabase(path);
    print('deleted database');
  }

  static Future<void> reset() async {
    if (path == null || db == null) return;
    await db.close();
    await deleteDatabase(path);
    await open();
  }

  static Future<void> init(db) async {
      var batch = db.batch();
      batch.execute('CREATE TABLE rooms  (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT NOT NULL, comment TEXT, photo BLOB)');
      batch.execute('CREATE TABLE boxes  (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT NOT NULL, comment TEXT, photo BLOB, code TEXT, room_id INTEGER NOT NULL)');
      batch.execute('CREATE TABLE items  (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT NOT NULL, comment TEXT, photo BLOB, code TEXT, box_id INTEGER NOT NULL, meta TEXT)');
      batch.execute('CREATE TABLE photos (id INTEGER PRIMARY KEY AUTOINCREMENT, data BLOB NOT NULL, item_id INTEGER NOT NULL)');
      batch.insert('rooms', {'id': 0, 'name': 'Uncategorized', 'comment': 'Everything and nothing'});
      batch.insert('boxes', {'id': 0, 'name': 'All', 'comment': 'Lost and found', 'room_id': 0});

      /*
      batch.insert('rooms', {'id': 1, 'name': 'Basement'});
      batch.insert('rooms', {'id': 2, 'name': 'Attic'});
      batch.insert('rooms', {'id': 3, 'name': 'Storage'});
      batch.insert('rooms', {'id': 4, 'name': 'Shed'});
      batch.insert('boxes', {'id': 1, 'name': 'Electronics', 'room_id': 1});
      batch.insert('boxes', {'id': 2, 'name': 'Video Games', 'room_id': 1});
      batch.insert('boxes', {'id': 3, 'name': 'Consoles', 'room_id': 2});
      batch.insert('items', {'id': 1, 'name': 'PlayStation 2', 'box_id': 3});
      batch.insert('items', {'id': 2, 'name': 'Sega Saturn', 'box_id': 3});
      batch.insert('items', {'id': 3, 'name': 'Nintendo 64', 'box_id': 3});
      batch.insert('items', {'id': 4, 'name': 'iPod Touch', 'box_id': 1});
      batch.insert('items', {'id': 5, 'name': 'Samsung Galaxy S3', 'box_id': 1});
      batch.insert('items', {'id': 6, 'name': 'AirPods', 'box_id': 1});
      */
      await batch.commit();
      print('initialized database');
  }

  static Future<List<Map>> getRooms() async {
    return await db.rawQuery('''
      SELECT
        id, name, comment, photo,
        (SELECT count(*) FROM boxes WHERE COALESCE(boxes.room_id, 0) = rooms.id) box_count
      FROM rooms''');
  }

  static Future<Map> getRoom(id) async {
    final rooms = await db.rawQuery('''
      SELECT
        id, name, comment, photo,
        (SELECT count(*) FROM boxes WHERE COALESCE(boxes.room_id, 0) = rooms.id) box_count
      FROM rooms
      WHERE id = ?
      ''', [id]);
    return Map.from(rooms.first);
  }

  static Future<Map> getBox(id) async {
    final boxes = await db.rawQuery('''
      SELECT
        id, name, comment, photo,
        (SELECT count(*) from items WHERE COALESCE(items.box_id, 0) = boxes.id) item_count
      FROM boxes
      WHERE id = ?''', [id]);
    return Map.from(boxes.first);
  }

  static Future<List<Map>> getBoxes({roomId}) async {
    return await db.rawQuery('''
      SELECT
        id, name, comment, code, photo,
        (SELECT count(*) from items WHERE COALESCE(items.box_id, 0) = boxes.id) item_count
      FROM boxes
      WHERE boxes.room_id = ?''', [roomId]);
  }

  static Future<List<Map>> getAllBoxes() async {
    return await db.rawQuery('''
      SELECT boxes.id id, boxes.name name, boxes.comment comment, boxes.code code, boxes.photo photo, rooms.name room_name FROM boxes
      JOIN rooms ON rooms.id = boxes.room_id''');
  }

  static Future<List<Map>> getAllItems() async {
    return await db.rawQuery('''
      SELECT items.id id, items.name name, items.comment comment, items.code code, items.photo photo, items.meta meta,
         boxes.id box_id, boxes.name box_name,
         rooms.id room_id, rooms.name room_name
      FROM items
      JOIN boxes ON boxes.id = items.box_id
      JOIN rooms ON rooms.id = boxes.room_id''');
  }

  static Future<List<Map>> getItems({boxId}) async {
    return (await getAllItems()).where((x) => x['box_id'] == boxId).toList();
  }

  static Future<List<Map>> getPhotos({itemId}) async {
    return await db.rawQuery('SELECT id, data, item_id FROM photos WHERE item_id = ?', [itemId]);
  }

  static Future<void> insertPhoto({itemId, data}) async {
    return await db.rawInsert('INSERT INTO photos (item_id, data) VALUES (?, ?)', [itemId, data]);
  }

  static Future<void> deletePhoto({id}) async {
    return await db.rawDelete('DELETE FROM photos WHERE id = ?', [id]);
  }

  static Future<void> deleteRoom(room) async {
    await db.rawUpdate("UPDATE boxes SET room_id = ? WHERE room_id = ?", [0, room['id']]);
    await db.rawDelete("DELETE FROM rooms WHERE id = ?", [room['id']]);
  }

  static Future<void> deleteBox(box) async {
    await db.rawUpdate("UPDATE items SET box_id = ? WHERE box_id = ?", [0, box['id']]);
    await db.rawDelete("DELETE FROM boxes WHERE id = ?", [box['id']]);
  }

  static Future<void> deleteItem(item) async {
    await db.rawDelete("DELETE FROM photos WHERE item_id = ?", [item['id']]);
    await db.rawDelete("DELETE FROM items WHERE id = ?", [item['id']]);
  }

  static Future<void> updateRoom(room) async {
    await db.rawUpdate("UPDATE rooms SET name = ?, comment = ?, photo = ? WHERE id = ?",
      [room['name'], room['comment'], room['photo'], room['id']]);
  }

  static Future<void> updateItem(item) async {
    await db.rawUpdate("UPDATE items SET name = ?, comment = ?, photo = ?, code = ?, meta = ? WHERE id = ?",
      [item['name'], item['comment'], item['photo'], item['code'], item['meta'], item['id']]);
  }

  static Future<void> updateBox(map) async {
    await db.rawUpdate("UPDATE boxes SET name = ?, comment = ?, photo = ? WHERE id = ?",
      [map['name'], map['comment'], map['photo'], map['id']]);
  }

  static Future<void> insertRoom(room) async {
    await db.rawInsert("INSERT INTO rooms (name, comment, photo) VALUES (?, ?, ?)", 
      [room['name'], room['comment'], room['photo']]);
  }

  static Future<void> insertBlankBox(name, roomId) async {
    await db.rawInsert("INSERT INTO boxes (name, room_id) VALUES (?, ?)", [name, roomId]);
  }

  static Future<void> insertBlankRoom(name) async {
    await db.rawInsert("INSERT INTO rooms (name) VALUES (?)", [name]);
  }

  static Future<void> insertBlankItem(name, boxId) async {
    await db.rawInsert("INSERT INTO items (name, box_id) VALUES (?, ?)", [name, boxId]);
  }

  static Future<void> moveBox(box, room) async {
    await db.rawUpdate("UPDATE boxes SET room_id = ? WHERE id = ?", [room['id'], box['id']]);
  }

  static Future<void> moveItem(item, box) async {
    await db.rawUpdate("UPDATE items SET box_id = ? WHERE id = ?", [box['id'], item['id']]);
  }

  static Future<void> updateCode({boxId, code}) async {
    await db.rawUpdate("UPDATE boxes SET code = NULL WHERE code = ?", [code]);
    await db.rawUpdate("UPDATE boxes SET code = ? WHERE id = ?", [code, boxId]);
  }

  static Future<Map> getByCode({String code}) async {
    print('search $code');
    if (code.startsWith('S-')) {
      return (await getAllBoxes()).firstWhere((x) => x['code'] == code, orElse: () => null);
    } else {
      return (await getAllItems()).firstWhere((x) => x['code'] == code, orElse: () => null);
    }
  }
}