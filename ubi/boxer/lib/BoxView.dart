import 'package:flutter/material.dart';
import 'package:qr_flutter/qr_flutter.dart';
import 'utils.dart';
import 'ItemView.dart';
import 'Database.dart';
import 'ScanView.dart';

final codeRegex = RegExp(r'^S-[0-9]{8}$');

class CodeView extends StatefulWidget {
  CodeView({@required this.box, this.refreshParent});
  final Map box;
  final void Function() refreshParent;

  @override
  State<StatefulWidget> createState() => CodeViewState(box: box);
}

class CodeViewState extends State<CodeView> {
  Map box;

  CodeViewState({this.box});

  void _editCode(context) async {
    final value = await Navigator.of(context).push(MaterialPageRoute(builder: (context) => ScanView(capture: true)));
    if (value == null) return;
    if (!codeRegex.hasMatch(value)) {
      await showError(context: context, title: 'Invalid code', message: 'A box code must have a format of S-XXXXXXXX, but got $value');
    }
    DB.updateCode(boxId: box['id'], code: value).then((res) {
      setState(() => box['code'] = value);
      widget.refreshParent();
    });
  }

  @override
  Widget build(context) => Scaffold(
    appBar: AppBar(title: Text(box['name'])),
    body: Column(children: [
      Expanded(child: Column(children: [
        QrImage(data: box['code'] ?? ''),
        InkText(text: box['code'] ?? 'No QR Code', size: 28, padding: 10, color: Colors.grey),
      ])),
      ExpandedRow(child: RaisedButton.icon(label: Text('Change code'), icon: Icon(Icons.camera_alt), onPressed: () => _editCode(context))),
    ])
  );
}

class BoxView extends StatefulWidget {
  BoxView({@required this.box, this.refreshParent});
  final Map box;
  final void Function() refreshParent;

  @override
  State<StatefulWidget> createState() => BoxViewState(box: box);
}

class BoxViewState extends State<BoxView> {
  BoxViewState({this.box});
  List<Map> items = [];
  final Map box;

  @override
  void initState() {
    super.initState();
    refresh();
  }

  void refresh() {
    DB.getItems(boxId: box['id']).then((res) => setState(() => items = res));
    if (widget.refreshParent != null) widget.refreshParent();
  }

  Future<void> _addItem() async {
    await DB.insertBlankItem('New Item', box['id']);
    refresh();
  }

  Future<void> _deleteItem(context, item) async {
    final accept = await showBinaryDialog(context: context, accept: 'Delete item', title: 'Are you sure?', message: 'This action cannot be reversed.');
    if (!accept) return;
    await DB.deleteItem(item);
    refresh();
  }

  Future<void> _updateBox(function) async {
    setState(function);
    await DB.updateBox(box);
    refresh();
  }

  Future<void> _moveItem(context, item) async {
    final boxes = await DB.getAllBoxes();
    final names = boxes.map((x) => x['room_name'] + ' / ' + x['name'] as String).toList();
    final index = await Navigator.of(context).push(MaterialPageRoute(builder: (ctx) => MoveDialog(rows: names)));
    if (index == null) return;
    await DB.moveItem(item, boxes[index]);
    refresh();
  }

  Future<void> _editTitle() async {
    String value = await showTextDialog(context: context, title: 'Edit name', label: 'Name', confirm: 'Save changes', initialValue: box['name']);
    if (value == null) return;
    await _updateBox(() => box['name'] = value);
  }

  Future<void> _editComment() async {
    String value = await showTextDialog(context: context, title: 'Edit comment', label: 'Comment', confirm: 'Save changes', initialValue: box['comment']);
    if (value == null) return;
    await _updateBox(() => box['comment'] = value);
  }

  Future<void> _editPhoto() async {
    final newPhoto = await Navigator.of(context).push(MaterialPageRoute(builder: (ctx) => TakePictureScreen()));
    await _updateBox(() => box['photo'] = newPhoto);
  }

  void _navigate() async {
    if (box['code'] == null) {
      showError(context: context, message: 'Cannot navigate to box, as it does not have a QR code.');
      return;
    }
    Navigator.of(context).push(MaterialPageRoute(builder: (context) => ScanView(navigate: box['code'])));
  }

  void _gotoCode() async {
    pushWidget(context: context, widget: CodeView(box: box, refreshParent: refresh));
  }

  Widget _actionMenu(context, item) { 
    return Padding(
      padding: EdgeInsets.all(10),
      child: Column(mainAxisSize: MainAxisSize.min, children: [
        Row(children: <Widget>[
          Expanded(child: FlatButton.icon(label: Text('Move'), icon: Icon(Icons.transfer_within_a_station), onPressed: () {
            Navigator.of(context).pop();
            _moveItem(context, item);
          })),
          Expanded(child: FlatButton.icon(label: Text('Delete'), icon: Icon(Icons.delete), onPressed: () {
            Navigator.of(context).pop();
            _deleteItem(context, item);
          })),
        ]),
        ExpandedRow(child: RaisedButton.icon(label: Text('Cancel'), icon: Icon(Icons.close), onPressed: () {
          Navigator.of(context).pop();
        })),
      ])
    );
  }

  Widget _buildTile(context, i) {
    if (i.isOdd) return Divider();
    Map item = items[i ~/ 2];
    return ListTile(
      leading: maybeImage(item['photo']) ?? boxPhoto,
      title: Text(item['name']),
      subtitle: Text(item['comment'] ?? 'Just an item'),
      onLongPress: () => showModalBottomSheet(context: context, builder: (context) => _actionMenu(context, item)),
      onTap: () => Navigator.of(context).push(MaterialPageRoute(builder: (ctx) => ItemView(item: Map.from(item), refreshParent: refresh)))
    );
  }

  @override
  Widget build(BuildContext context) => Scaffold(
    appBar: AppBar(
      title: Text(box['name']),
      actions: [
        IconButton(icon: Icon(Icons.add), onPressed: _addItem),
        if (box['id'] != 0) IconButton(icon: Icon(Icons.center_focus_weak), onPressed: _gotoCode),
      ],
    ),
    body: Column(children: [
      ListHeader(
        title: box['name'],
        comment: box['comment'] ?? 'Just a box',
        image: maybeImage(box['photo']) ?? boxPhoto,
        onEditTitle: _editTitle,
        onEditSubtitle: _editComment,
        onEditImage: _editPhoto,
      ),
      Divider(),
      Expanded(child: ListView.builder(
        itemBuilder: _buildTile,
        itemCount: items.length * 2,
        shrinkWrap: true,
      )),
    ]),
    floatingActionButton: FloatingActionButton(
      child: Icon(Icons.navigation),
      onPressed: _navigate,
    ),
  );
}