import 'package:flutter/material.dart';

import 'utils.dart';
import 'RoomView.dart';
import 'Database.dart';


class RoomList extends StatefulWidget {
  @override
  State<StatefulWidget> createState() => RoomListState();
}

class RoomListState extends State<RoomList> {
  List<Map> _rows = [];

  RoomListState() {
    fetchRooms();
  }

  void fetchRooms() {
    DB.getRooms().then((res) => setState(() => _rows = res.map((x) => Map.from(x)).toList()));
  }

  void refresh() {
    fetchRooms();
  }

  Widget _actionMenu(room, context) { 
    return Padding(
      padding: EdgeInsets.all(10),
      child: Column(mainAxisSize: MainAxisSize.min, children: [
        ExpandedRow(child: FlatButton.icon(label: Text('Delete'), icon: Icon(Icons.delete), onPressed: () {
          Navigator.of(context).pop();
          _deleteRoom(room);
        })),
        ExpandedRow(child: RaisedButton.icon(label: Text('Cancel'), icon: Icon(Icons.close), onPressed: () {
          Navigator.of(context).pop();
        })),
      ])
    );
  }

  void _deleteRoom(room) async {
    final accept = await showBinaryDialog(context: context, accept: 'Delete room', title: 'Are you sure?', message: 'This will move all boxes inside to Uncategorized.');
    if (!accept) return;
    await DB.deleteRoom(room);
    refresh();
  }

  void _addRoom() async {
    DB.insertBlankRoom('New Room');
    refresh();
  }

  Widget _buildTile(context, i) {
    if (i.isOdd) return Divider();
    Map room = _rows[i ~/ 2];
    return ListTile(
      leading: maybeImage(room['photo']) ?? roomPhoto,
      title: Text(room['name']),
      subtitle: Text('${room['box_count']} boxes'),
      onTap: () => pushWidget(context: context, widget: RoomView(room: room, refreshParent: refresh)),
      onLongPress: () {
        if (room['id'] == 0) return;
        showModalBottomSheet(context: context, builder: (ctx) => _actionMenu(room, ctx));
      },
    );
  }

  @override
  Widget build(BuildContext context) => Scaffold(
    appBar: AppBar(
      title: Text('Rooms'),
      actions: [
        IconButton(icon: Icon(Icons.add), onPressed: _addRoom),
      ],
    ),
    body: Column(children: [
      Expanded(child: ListView.builder(
        itemBuilder: _buildTile,
        itemCount: _rows.length * 2,
        shrinkWrap: true,
      )),
    ]),
  );
}
