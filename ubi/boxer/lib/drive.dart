import 'dart:io';

import 'package:flutter/services.dart';
import 'package:google_sign_in/google_sign_in.dart';
import 'package:googleapis/drive/v3.dart' as ga;
import 'package:http/http.dart';
import 'package:http/io_client.dart';

class GoogleHttpClient extends IOClient {
  Map<String, String> _headers;
  GoogleHttpClient(this._headers) : super();
  @override
  Future<IOStreamedResponse> send(BaseRequest request) =>
    super.send(request..headers.addAll(_headers));
  @override
  Future<Response> head(Object url, {Map<String, String> headers}) =>
    super.head(url, headers: headers..addAll(_headers));
}

class Drive {
  final _auth = GoogleSignIn(scopes: ['https://www.googleapis.com/auth/drive.appdata']);
  ga.DriveApi _api;

  final void Function() onSignIn, onSignOut;

  Drive({this.onSignIn, this.onSignOut});

  void _didSignOut() async {
    onSignOut();
  }

  void _didSignIn(GoogleSignInAccount account) async {
    if (account == null) return;
    this._api = ga.DriveApi(GoogleHttpClient(await account.authHeaders));
    onSignIn();
  }

  Future<void> signIn({silent = false}) async {
    _auth.onCurrentUserChanged.listen(_didSignIn);
    if (silent) {
      try {
        final account = await _auth.signInSilently();
        _didSignIn(account);
      } on PlatformException catch (e) {
        _didSignOut();
      } catch (e) {
        print(e);
        _didSignOut();
      }
    } else {
      final account = await _auth.signIn();
      _didSignIn(account);
    }
  }

  Future<void> signOut() async {
    await _auth.signOut();
    _didSignOut();
  }

  Future<void> upload(File file, String name) async {
    if (_api == null) return;
    final src = ga.Media(file.openRead(), file.lengthSync());
    final dst = ga.File.fromJson({'name': name, 'parents': ['appDataFolder']});
    return await _api.files.create(dst, uploadMedia: src);
  }

  Future<void> download(String id, {onDone, onError}) async {
    if (_api == null) {
      onError('broken connection');
      return;
    }
    final media = await _api.files.get(id, downloadOptions: ga.DownloadOptions.FullMedia);
    List<int> bytes = [];
    media.stream.listen((data) => bytes.insertAll(bytes.length, data),
      onDone: () => onDone(bytes),
      onError: onError ?? (error) {
      },
    );
  }

  Future<void> delete(String id) async {
    if (_api == null) return;
    await _api.files.delete(id);
  }

  Future<String> find(String name) async {
    if (_api == null) return null;
    final list = await _api.files.list(spaces: 'appDataFolder');
    final file = list.files.firstWhere((x) => x.name == name, orElse: () => null);
    return file?.id;
  }
}
