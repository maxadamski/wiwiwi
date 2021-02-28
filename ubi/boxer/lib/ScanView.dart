import 'package:flutter/material.dart';
import 'package:qr_code_scanner/qr_code_scanner.dart';

import 'BoxView.dart';
import 'ItemView.dart';
import 'Database.dart';
import 'utils.dart';

class ScanView extends StatefulWidget {
  final String navigate;
  final bool capture;

  const ScanView({Key key, this.navigate, this.capture = false}) : super(key: key);

  @override
  State<StatefulWidget> createState() => ScanViewState();
}

class ScanViewState extends State<ScanView> {
  final GlobalKey qrKey = GlobalKey(debugLabel: 'QR');
  QRViewController controller;
  String qrText;
  int index = 0;
  Map map;

  @override
  void initState() {
    super.initState();
  }

  void _onCreated(controller) {
    this.controller = controller;
    controller.scannedDataStream.listen(_onScan);
  }

  void _onScan(data) {
    if (qrText != data) {
      if (widget.navigate != null) {
        if (widget.navigate == data) {
          player.play('success.mp3');
          Navigator.of(context).pop();
        } else {
          player.play('error.mp3');
        }
      } else {
        player.play('scan.mp3');
      }
      if (!widget.capture && widget.navigate == null) {
        DB.getByCode(code: data).then((res) => setState(() => map = res));
      }
    }


    setState(() => qrText = data);

    if (widget.capture) {
      controller.pauseCamera();
      Navigator.of(context).pop(data);
    }
  }

  void _gotoMap() {
    if (map == null) return;
    if (map['code'].startsWith('S-')) {
      pushWidget(context: context, widget: BoxView(box: map));
    } else {
      pushWidget(context: context, widget: ItemView(item: map));
    }
  }

  String _entityPath(entity) {
    return entity['room_name'] + (map.containsKey('box_name') ? ' / '+map['box_name'] : '');
  }

  String _hint() {
    if (widget.navigate != null) return 'Looking for '+widget.navigate;
    return 'Scan QR/EAN Code';
  }

  @override
  Widget build(context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Code Scanner'),
      ),
      body: Column(crossAxisAlignment: CrossAxisAlignment.center, children: [
          Expanded(flex: 5, child:
            QRView(key: qrKey, onQRViewCreated: _onCreated),
          ),
          Expanded(flex: 1, child:
            Center(child: Column(children: [
              InkText(text: qrText ?? _hint(), size: 16, color: Colors.grey),
              if (map != null) ListTile(
                leading: maybeImage(map['photo']) ?? boxPhoto,
                title: Text(map['name']),
                subtitle: Text(_entityPath(map)),
                onTap: _gotoMap,
              ),
            ]))
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