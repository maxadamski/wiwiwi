import 'dart:io';

import 'package:flutter/material.dart';
import 'package:path_provider/path_provider.dart';
import 'package:camera/camera.dart';
import 'package:image/image.dart' as img;
import 'package:qr_code_scanner/qr_code_scanner.dart';
import 'package:audioplayers/audio_cache.dart';

const double PHI = 1.618;

final boxPhoto = Image.asset('assets/box.png');
final roomPhoto = Image.asset('assets/box-stack.png');
final player = AudioCache();

void pushWidget({context, widget}) {
  Navigator.of(context).push(MaterialPageRoute(builder: (ctx) => widget));
}
 
dynamic maybeImage(dynamic bytes) {
  return bytes != null ? Image.memory(bytes) : null;
}

class Camera {
  static var camera;

  static Future<void> init() async {
    final cameras = await availableCameras();
    Camera.camera = cameras.first;
  }
}

class NavigatorPage extends StatelessWidget {
  NavigatorPage({this.child});
  final Widget child;

  @override
  Widget build(BuildContext context) => Navigator(
    onGenerateRoute: (settings) => MaterialPageRoute(builder: (context) => child),
    onPopPage: (a, b) { print('pop navigator page'); return false; },
  );
}

class ExpandedRow extends StatelessWidget {
  ExpandedRow({this.child});
  final Widget child;

  @override
  Widget build(BuildContext context) => Row(children: <Widget>[Expanded(child: child)]);
}

class MoveDialog extends StatelessWidget {
  MoveDialog({this.rows});
  final List<String> rows;

  Widget _buildTile(context, i) {
    if (i.isOdd) return Divider();
    String item = rows[i ~/ 2];
    return ListTile(
      title: Text(item),
      onTap: () {
        Navigator.of(context).pop(i ~/ 2);
      }
    );
  }

  @override
  Widget build(context) => Scaffold(
    appBar: AppBar(
      title: Text('Move To'),
      leading: IconButton(icon: Icon(Icons.arrow_back), onPressed: () {
        Navigator.of(context).pop(null);
      }),
    ),
    body: Column(children: [
      if (rows.isNotEmpty) Divider(),
      Expanded(child: ListView.builder(
        itemBuilder: _buildTile,
        itemCount: rows.length * 2,
        shrinkWrap: true,
      )),
      Padding(padding: EdgeInsets.all(10), child:
        ExpandedRow(child: RaisedButton.icon(label: Text('Cancel'), icon: Icon(Icons.close), onPressed: () {
          Navigator.of(context).pop(null);
        }))
      )
    ]),
  );
}

class ListHeader extends StatelessWidget {
  final void Function() onEditTitle, onEditSubtitle, onEditImage;
  final String title, comment;
  final Image image;

  ListHeader({this.image, this.title, this.comment, this.onEditTitle, this.onEditSubtitle, this.onEditImage});

  @override
  Widget build(context) =>
    Padding(padding: EdgeInsets.all(10), child:
      Row(children: [
        InkWell(child: SizedBox(width: 90, child: image ?? boxPhoto), onLongPress: onEditImage, borderRadius: BorderRadius.circular(5)),
        SizedBox(width: 10),
        Flexible(child: Column(crossAxisAlignment: CrossAxisAlignment.start, children: [
          InkText(text: title, size: 24, padding: 5, onLongPress: onEditTitle),
          InkText(text: comment, size: 16, padding: 5, onLongPress: onEditSubtitle),
        ])),
      ])
    )
    ;
}

class TakePictureScreen extends StatefulWidget {
  final int cropSquare;

  const TakePictureScreen({Key key, this.cropSquare = 120}) : super(key: key);

  @override
  createState() => TakePictureScreenState();
}

class TakePictureScreenState extends State<TakePictureScreen> {
  CameraController _controller;
  Future<void> _initializeControllerFuture;

  @override
  void initState() {
    super.initState();
    _controller = CameraController(Camera.camera, ResolutionPreset.medium);
    _initializeControllerFuture = _controller.initialize();
  }

  @override
  void dispose() {
    _controller.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: Text('Take a picture')),
      body: FutureBuilder<void>(
        future: _initializeControllerFuture,
        builder: (context, snapshot) {
          if (snapshot.connectionState == ConnectionState.done) {
            return AspectRatio(
              aspectRatio: _controller.value.aspectRatio,
              child: CameraPreview(_controller)
            );
          } else {
            return Center(child: CircularProgressIndicator());
          }
        },
      ),
      floatingActionButton: FloatingActionButton(
        child: Icon(Icons.camera_alt),
        onPressed: () async {
          try {
            await _initializeControllerFuture;
            final path = (await getTemporaryDirectory()).path + '${DateTime.now()}.png';
            await _controller.takePicture(path);
            var image = img.decodeImage(File(path).readAsBytesSync());
            if (widget.cropSquare != null)
              image = img.copyResizeCropSquare(image, widget.cropSquare);
            final data = img.encodePng(image);
            Navigator.of(context).pop(data);
          } catch (e) {
            print(e);
          }
        },
      ),
    );
  }
}

Future<bool> showConfirmationAlert({context, title = 'Are you sure?', body, confirm = 'Confirm', cancel = 'Cancel'}) async {
  bool result = false;
  await showDialog(context: context, builder: (context) => AlertDialog(
    title: Text(title),
    content: body,
    actions: [
      FlatButton(
        child: Text(cancel),
        onPressed: () {
          Navigator.of(context).pop();
        },
      ),
      FlatButton(
        child: Text(confirm),
        onPressed: () {
          result = true;
          Navigator.of(context).pop();
        },
      )
    ],
  ));
  return result;
}

class MyDialog extends StatelessWidget {
  final Function() onConfirm, onCancel;
  final String title, confirm, cancel;
  final Widget body;

  MyDialog({this.title = 'Are you sure?', this.body, this.onConfirm, this.onCancel, this.confirm = 'Are you sure?', this.cancel = 'Cancel'});

  @override
  Widget build(context) => AlertDialog(
    title: Text(title),
    content: body,
    actions: [
      FlatButton(
        child: Text(cancel),
        onPressed: () {
          Navigator.of(context).pop();
          if (onCancel != null) onCancel();
        },
      ),
      FlatButton(
        child: Text(confirm),
        onPressed: () {
          Navigator.of(context).pop();
          if (onConfirm != null) onConfirm();
        },
      )
    ],
  );
}

Future<String> showTextDialog({context, title = 'Edit', label = 'Value', initialValue, confirm = 'Save changes'}) async {
  String newValue;
  String result;
  await showDialog(context: context, builder: (ctx) => MyDialog(
    title: title,
    confirm: confirm,
    body: TextFormField(
      initialValue: initialValue,
      decoration: InputDecoration(labelText: label, border: OutlineInputBorder()),
      onChanged: (value) => newValue = value,
    ),
    onConfirm: () async {
      if (newValue == null || newValue.isEmpty) return;
      result = newValue;
    },
  ));
  return result;
}

class InkText extends StatelessWidget {
  final void Function() onTap, onLongPress;
  final String text;
  final double size, radius, padding;
  final Color color;

  InkText({this.text, this.onTap, this.onLongPress, this.radius = 5, this.size, this.color, this.padding = 5});

  @override
  Widget build(context) => InkWell(
    onTap: onTap, onLongPress: onLongPress,
    borderRadius: BorderRadius.circular(radius),
    child: Container(
      margin: EdgeInsets.symmetric(vertical: padding, horizontal: padding*PHI),
      child: Text(this.text, style: TextStyle(fontSize: size, color: color)),
    )
  );
}

class InkImage extends StatelessWidget {
  final void Function() onTap, onLongPress;
  final Widget image;
  final double margin;

  InkImage({this.image, this.margin, this.onTap, this.onLongPress});

  @override
  Widget build(context) => Container(
    margin: EdgeInsets.all(margin),
    child: Stack(children: [
      image,
      Positioned.fill(
        child: Material(color: Colors.transparent, child: InkWell(
          onLongPress: onLongPress,
          onTap: onTap,
        ))
      )
    ])
  );
}

class QRViewExample extends StatefulWidget {
  const QRViewExample({Key key}) : super(key: key);

  @override
  State<StatefulWidget> createState() => QRViewExampleState();
}

class QRViewExampleState extends State<QRViewExample> {
  final GlobalKey qrKey = GlobalKey(debugLabel: 'QR');
  QRViewController controller;
  String qrText = "";

  void _onCreated(QRViewController controller) {
    this.controller = controller;
    controller.scannedDataStream.listen((scanData) {
      setState(() => qrText = scanData);
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Code Scanner'),
      ),
      body: Column(children: [
          Expanded(flex: 5, child:
            QRView(key: qrKey, onQRViewCreated: _onCreated),
          ),
          Expanded(flex: 1, child:
            Center(
              child: Text('Scan result: $qrText'),
            ),
          ),
      ]),
    );
  }

  @override
  void dispose() {
    controller?.dispose();
    super.dispose();
  }
}

Future<void> showError({@required context, message, title = 'Error', action = 'Ok'}) {
  return showDialog(context: context, builder: (context) => AlertDialog(
    title: Text(title),
    content: Text(message),
    actions: [FlatButton(child: Text(action), onPressed: () => Navigator.of(context).pop())]
  ));
}

Future<bool> showBinaryDialog({@required context, message, title = 'Error', accept = 'Ok', cancel = 'Cancel'}) async {
  var result = await showDialog(context: context, builder: (context) => AlertDialog(
    title: Text(title),
    content: Text(message),
    actions: [
      FlatButton(child: Text(cancel), onPressed: () {
        Navigator.of(context).pop(false);
      }),
      FlatButton(child: Text(accept), onPressed: () {
        Navigator.of(context).pop(true);
      }),
    ]
  ));
  return result ?? false;
}