import 'package:flutter/material.dart';
import 'package:androidish_ink_well/material_ink_splash.dart';
import 'package:custom_navigator/custom_navigation.dart';

import 'RoomListView.dart';
import 'SearchView.dart';
import 'ScanView.dart';
import 'Database.dart';
import 'ProfileView.dart';
import 'utils.dart';

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await DB.open();
  await Camera.init();
  player.loadAll(['scan.mp3', 'success.mp3', 'error.mp3']);
  runApp(App());
}

class App extends StatefulWidget {
  @override
  State<StatefulWidget> createState() => AppState();
}

class AppState extends State<App> {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Boxer',
      home: Theme(
        data: Theme.of(context).copyWith(
          splashFactory: MaterialInkSplash.splashFactory,
          primaryColor: Colors.black,
        ),
        child: CustomScaffold(
          scaffold: Scaffold(
            bottomNavigationBar: BottomNavigationBar(
              type: BottomNavigationBarType.fixed,
              items: [
                BottomNavigationBarItem(title: Text('My Stuff'), icon: Icon(Icons.category)),
                BottomNavigationBarItem(title: Text('Search'), icon: Icon(Icons.search)),
                BottomNavigationBarItem(title: Text('Scan'), icon: Icon(Icons.center_focus_strong)),
                BottomNavigationBarItem(title: Text('Profile'), icon: Icon(Icons.account_circle)),
              ],
            )
          ),
          children: [
            RoomList(),
            SearchView(),
            ScanView(),
            SyncView(),
          ],
          onItemTap: (index) {
          },
        ),
      )
    );
  }
}
