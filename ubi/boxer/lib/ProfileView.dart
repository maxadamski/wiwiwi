import 'dart:io';

import 'package:flutter/material.dart';
import 'package:flutter_secure_storage/flutter_secure_storage.dart';

import 'Database.dart';
import 'utils.dart';
import 'drive.dart';

class SyncView extends StatefulWidget {
  @override
  State<StatefulWidget> createState() => SyncViewState();
}

class SyncViewState extends State<SyncView> {
  final storage = new FlutterSecureStorage();

  bool signedIn = false;
  Drive drive;
  String driveFiles, localFiles;
  bool loading = false;

  @override
  void initState() {
    super.initState();
    drive = Drive(onSignIn: _onSignIn, onSignOut: _onSignOut);
    storage.read(key: 'signedIn').then((signedIn) {
      if (signedIn == 'true')
        drive.signIn(silent: true);
      refresh();
    });
  }

  void refresh() async {
    localFiles = Directory('/data/user/0/com.example.boxer/databases').listSync(recursive: true).map((x) => x.toString()).join('\n');
    if (!signedIn) return;
    await drive.find('boxer.db').then((id) {
      final status = id != null ? '/appData/boxer.db\nid = $id' : null;
      setState(() => driveFiles = status);
    });
  }

  void _onSignIn() async {
    await storage.write(key: 'signedIn', value: 'true');
    setState(() => signedIn = true);
  }

  void _onSignOut() async {
    await showError(context: context, title: 'Info', message: 'Disconnected from Google Drive');
    await storage.write(key: 'signedIn', value: 'false');
    setState(() => signedIn = false);
  }

  void _signIn() async {
    await drive.signIn();
  }

  void _signOut() async {
    final accept = await showBinaryDialog(context: context, title: 'Are you sure?', message: 'You will not be able to back-up or restore Boxer data until you sign in again.', accept: 'Disconnect');
    if (!accept) return;
    await drive.signOut();
  }

  void _pushDatabase() async {
    final accept = await showBinaryDialog(context: context, title: 'Are you sure?', message: 'All Boxer data on your Google Drive will be replaced with the data on your device!', accept: 'Back-up my data');
    if (!accept) return;

    setState(() => loading = true);
    var id = await drive.find('boxer.db');
    while (id != null) {
      await drive.delete(id);
      id = await drive.find('boxer.db');
    }

    await DB.close();
    final db = File(DB.path);
    await drive.upload(db, 'boxer.db');
    await DB.open();
    await showError(context: context, title: 'Success', message: 'Uploaded database');
    setState(() => loading = false);
  }

  void _pullDatabase() async {
    final accept = await showBinaryDialog(context: context, title: 'Are you sure?', message: 'All Boxer data on your device will be replaced with the data from your Google Drive!', accept: 'Restore my data');
    if (!accept) return;

    final id = await drive.find('boxer.db');
    if (id == null) {
      await showError(context: context, message: 'No database found on Google Drive');
      return;
    }

    setState(() => loading = true);
    await drive.download(id,
      onDone: (bytes) async {
        await DB.delete();
        final file = File(DB.path);
        file.writeAsBytesSync(bytes);
        await DB.open();
        await showError(context: context, title: 'Success', message: 'Restored database');
        setState(() => loading = false);
      },
      onError: (error) async {
        await showError(context: context, message: 'Error while downloading database: $error');
        setState(() => loading = false);
      }
    );
  }

  void _resetDatabase() async {
    if (await showConfirmationAlert(context: context, body: Text('This will permanently delete all of Boxer data on this device!'), confirm: 'Delete my data'))
      DB.reset();
  }

  @override
  Widget build(context) => Scaffold(
    appBar: AppBar(
      title: Text('Profile'),
      actions: [
        //IconButton(icon: Icon(Icons.sync), onPressed: refresh)
      ],
    ),
    body: Padding(padding: EdgeInsets.all(10), child: Column(crossAxisAlignment: CrossAxisAlignment.stretch, children: [
      RaisedButton.icon(
        icon: Icon(signedIn ? Icons.cloud_off : Icons.cloud_queue),
        label: Text(signedIn ? 'Disconnect from Google Drive' : 'Connect to Google Drive'), 
        onPressed: signedIn ? _signOut : _signIn,
      ),
      if (signedIn) RaisedButton.icon(label: Text('Back-up Database'), icon: Icon(Icons.cloud_upload), onPressed: _pushDatabase),
      if (signedIn) RaisedButton.icon(label: Text('Restore Database'), icon: Icon(Icons.cloud_download), onPressed: _pullDatabase),
      RaisedButton.icon(label: Text('Reset Database'), icon: Icon(Icons.delete), onPressed: _resetDatabase),
      Expanded(child: Column(crossAxisAlignment: CrossAxisAlignment.start, children: [
        SizedBox(height: 10),
        //InkText(text: 'Local files', size: 18, padding: 0, color: Colors.grey),
        //SizedBox(height: 5),
        //InkText(text: localFiles ?? 'No local files', size: 12, padding: 0),
        //SizedBox(height: 10),
        //if (signedIn) InkText(text: 'Drive files', size: 18, padding: 0, color: Colors.grey),
        //if (signedIn) SizedBox(height: 5),
        //if (signedIn) InkText(text: driveFiles ?? 'No drive files', size: 12, padding: 0),
      ])),
      if (loading) LinearProgressIndicator(),
    ])),
  );
}