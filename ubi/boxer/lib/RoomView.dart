import 'package:flutter/material.dart';

import 'BoxView.dart';
import 'Database.dart';
import 'utils.dart';

class RoomView extends StatefulWidget {
  RoomView({this.room, this.refreshParent});
  final Map room;
  final void Function() refreshParent;

  @override
  State<StatefulWidget> createState() => RoomViewState(room: room);
}

class RoomViewState extends State<RoomView> {
  RoomViewState({this.room});
  List<Map> boxes = [];
  final Map room;

  @override
  void initState() {
    super.initState();
    refresh();
  }

  void refresh() {
    DB.getBoxes(roomId: room['id']).then((result) => setState(() => boxes = result.map((x) => Map.from(x)).toList()));
    if (widget.refreshParent != null) widget.refreshParent();
  }

  Future<void> _moveBox(context, box) async {
    final rooms = await DB.getRooms();
    final names = rooms.map((x) => x['name'] as String).toList();
    final index = await Navigator.of(context).push(MaterialPageRoute(builder: (ctx) => MoveDialog(rows: names)));
    if (index == null) return;
    await DB.moveBox(box, rooms[index]);
    refresh();
  }

  Future<void> _addBox() async {
    await DB.insertBlankBox('New Box', room['id']);
    refresh();
  }

  Future<void> _deleteBox(context, box) async {
    final accept = await showBinaryDialog(context: context, accept: 'Delete box', title: 'Are you sure?', message: 'This will move all items inside to Uncategorized/All.');
    if (!accept) return;
    await DB.deleteBox(box);
    refresh();
  }

  Future<void> _updateRoom(function) async {
    setState(function);
    await DB.updateRoom(room);
    refresh();
  }

  Future<void> _editTitle() async {
    String value = await showTextDialog(context: context, title: 'Edit name', label: 'Name', confirm: 'Save changes', initialValue: room['name']);
    if (value == null) return;
    await _updateRoom(() => room['name'] = value);
  }

  Future<void> _editComment() async {
    String value = await showTextDialog(context: context, title: 'Edit comment', label: 'Comment', confirm: 'Save changes', initialValue: room['comment']);
    if (value == null) return;
    await _updateRoom(() => room['comment'] = value);
  }

  Future<void> _editPhoto() async {
    final newPhoto = await Navigator.of(context).push(MaterialPageRoute(builder: (ctx) => TakePictureScreen()));
    await _updateRoom(() => room['photo'] = newPhoto);
  }

  Widget _actionMenu(context, box) { 
    return Padding(
      padding: EdgeInsets.all(10),
      child: Column(mainAxisSize: MainAxisSize.min, children: [
        Row(children: <Widget>[
          Expanded(child: FlatButton.icon(label: Text('Move'), icon: Icon(Icons.transfer_within_a_station), onPressed: () async {
            Navigator.of(context).pop();
            await _moveBox(context, box);
          })),
          Expanded(child: FlatButton.icon(label: Text('Delete'), icon: Icon(Icons.delete), onPressed: () async {
            Navigator.pop(context);
            await _deleteBox(context, box);
          })),
        ]),
        ExpandedRow(child: RaisedButton.icon(label: Text('Cancel'), icon: Icon(Icons.close), onPressed: () {
          Navigator.pop(context);
        })),
      ])
    );
  }

  Widget _boxRow(context, i) {
    if (i.isOdd) return Divider();
    Map box = boxes[i ~/ 2];
    return ListTile(
      leading: maybeImage(box['photo']) ?? boxPhoto,
      title: Text(box['name']),
      subtitle: Text('${box['item_count']} items'),
      onTap: () => pushWidget(context: context, widget: BoxView(box: box, refreshParent: refresh)),
      onLongPress: () {
        if (box['id'] == 0) return;
        showModalBottomSheet(context: context, builder: (context) => _actionMenu(context, box));
      },
    );
  }

  @override
  Widget build(BuildContext context) =>
    Scaffold(
      appBar: AppBar(
        title: Text(room['name']),
        actions: [
          IconButton(icon: Icon(Icons.add), onPressed: _addBox),
        ],
      ),
      body: Column(children: [
        ListHeader(
          title: room['name'],
          comment: room['comment'] ?? 'Just a room',
          image: maybeImage(room['photo']) ?? roomPhoto,
          onEditTitle: _editTitle,
          onEditSubtitle: _editComment,
          onEditImage: _editPhoto,
        ),
        Divider(),
        Expanded(child: ListView.builder(
          itemCount: boxes.length * 2,
          shrinkWrap: true,
          itemBuilder: _boxRow,
        )),
      ]),
    )
    ;
}
