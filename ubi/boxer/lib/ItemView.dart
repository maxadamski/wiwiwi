import 'package:boxer/ScanView.dart';
import 'package:boxer/utils.dart';
import 'package:flutter/material.dart';
import 'package:image/image.dart';

import 'BoxView.dart';
import 'RoomView.dart';
import 'Database.dart';

class ItemView extends StatefulWidget {
  final Map item;
  final void Function() refreshParent;

  ItemView({@required this.item, this.refreshParent});

  @override
  State<StatefulWidget> createState() => ItemViewState(item: item);
}

class ItemViewState extends State<ItemView> {
  final Map item;
  List<Map> photos = [];

  ItemViewState({this.item});

  @override
  void initState() {
    super.initState();
    refresh();
  }

  void refresh() {
    DB.getPhotos(itemId: item['id']).then((res) => setState(() => photos = res));
    if (widget.refreshParent != null) widget.refreshParent();
  }

  Widget _actionMenu(context, photo) { 
    return Padding(
      padding: EdgeInsets.all(10),
      child: Column(mainAxisSize: MainAxisSize.min, children: [
        Row(children: [
          Expanded(child: FlatButton.icon(label: Text('Set thumbnail'), icon: Icon(Icons.image), onPressed: () async {
            Navigator.of(context).pop();
            _setThumbnail(photo);
          })),
          Expanded(child: FlatButton.icon(label: Text('Delete'), icon: Icon(Icons.delete), onPressed: () async {
            Navigator.pop(context);
            _deletePhoto(photo);
          })),
        ]),
        ExpandedRow(child: RaisedButton.icon(label: Text('Cancel'), icon: Icon(Icons.close), onPressed: () {
          Navigator.pop(context);
        })),
      ])
    );
  }

  Future<void> _updateItem(function) async {
    setState(function);
    await DB.updateItem(item);
    refresh();
  }

  Future<void> _editTitle() async {
    String value = await showTextDialog(context: context, title: 'Edit name', label: 'Name', confirm: 'Save changes', initialValue: item['name']);
    if (value == null) return;
    await _updateItem(() => item['name'] = value);
  }

  Future<void> _editComment() async {
    String value = await showTextDialog(context: context, title: 'Edit comment', label: 'Comment', confirm: 'Save changes', initialValue: item['comment']);
    if (value == null) return;
    await _updateItem(() => item['comment'] = value);
  }

  Future<void> _addPhoto() async {
    final newPhoto = await Navigator.of(context).push(MaterialPageRoute(builder: (ctx) => TakePictureScreen(cropSquare: null)));
    if (newPhoto == null) return;
    await DB.insertPhoto(itemId: item['id'], data: newPhoto);
    refresh();
  }

  Future<void> _deletePhoto(photo) async {
    await DB.deletePhoto(id: photo['id']);
    refresh();
  }

  Future<void> _deleteTag(tag) async {
    final newValue = (item['meta'] ?? '').split(';').where((x) => x != tag).join(';');
    await _updateItem(() => item['meta'] = newValue);
  }

  Future<void> _addTag() async {
    String value = await showTextDialog(context: context, title: 'New tag', label: 'Tag', confirm: 'Add');
    if (value == null) return;
    if (value.contains(';')) return;
    final newValue = ((item['meta'] ?? '').split(';') + [value]).join(';');
    await _updateItem(() => item['meta'] = newValue);
  }

  Future<void> _setThumbnail(photo) async {
    final img = decodeImage(photo['data']);
    final newThumb = encodePng(copyResizeCropSquare(img, 120));
    await _updateItem(() => item['photo'] = newThumb);
  }

  void _showPhotoActionMenu(context, photo) {
    showModalBottomSheet(context: context, builder: (context) => _actionMenu(context, photo));
  }

  void _gotoRoom() async {
    final data = await DB.getRoom(item['room_id']);
    pushWidget(context: context, widget: RoomView(room: data, refreshParent: refresh));
  }

  void _gotoBox() async {
    final data = await DB.getBox(item['box_id']);
    pushWidget(context: context, widget: BoxView(box: data, refreshParent: refresh));
  }

  void _editCode() async {
    final value = await Navigator.of(context).push(MaterialPageRoute(builder: (context) => ScanView(capture: true)));
    if (value == null) return;
    await _updateItem(() => item['code'] = value);
  }

  void _navigate() async {
    if (item['code'] == null) {
      showError(context: context, message: 'Cannot navigate to item, as it does not have a serial number.');
      return;
    }
    Navigator.of(context).push(MaterialPageRoute(builder: (context) => ScanView(navigate: item['code'])));
  }

  List<Widget> _getTags() {
    final tags = (item['meta'] ?? '').split(';');
    List<Widget> widgets = [];
    for (final tag in tags) {
      if (tag.isEmpty) continue;
      widgets.add(SizedBox(width: 10));
      widgets.add(Chip(label: Text(tag), onDeleted: () => _deleteTag(tag)));
    }
    return widgets;
  }

  @override
  Widget build(BuildContext context) => Scaffold(
    appBar: AppBar(
      title: Text(item['name']),
    ),
    body: SingleChildScrollView(child:
      Column(crossAxisAlignment: CrossAxisAlignment.start, children: [
        ListHeader(
          title: item['name'],
          comment: item['comment'] ?? 'Just an item',
          image: maybeImage(item['photo']) ?? boxPhoto,
          onEditTitle: _editTitle,
          onEditSubtitle: _editComment,
        ),

        SingleChildScrollView(scrollDirection: Axis.horizontal, child: Row(children: [
          Row(children: _getTags()),
          FlatButton.icon(label: Text('Add tag'), icon: Icon(Icons.add), onPressed: _addTag),
        ])),

        Divider(),
        Row(children: [
          SizedBox(width: 5),
          InkText(text: item['room_name'], size: 20, color: Colors.lightBlue, onTap: _gotoRoom),
          InkText(text: '/', size: 20, padding: 0),
          InkText(text: item['box_name'], size: 20, color: Colors.lightBlue, onTap: _gotoBox),
        ]),
        if (item['code'] != null)
          InkText(text: 'S/N: ${item['code']}', size: 16, color: Colors.grey, padding: 7, onLongPress: _editCode),
        Divider(),
        Column(
          children: photos.map((x) => InkImage(
            image: maybeImage(x['data']),
            onLongPress: () => _showPhotoActionMenu(context, x),
            margin: 5,
          )).toList()
        ),

        if (photos.isNotEmpty) Divider(),

        ExpandedRow(child: RaisedButton.icon(label: Text('Add photo'), icon: Icon(Icons.camera_alt), onPressed: _addPhoto)),
        ExpandedRow(child: RaisedButton.icon(label: Text('Scan serial number'), icon: Icon(Icons.center_focus_strong), onPressed: _editCode)),
      ]),
    ),
    floatingActionButton: FloatingActionButton(
      child: Icon(Icons.navigation),
      onPressed: _navigate,
    ),
  );
}