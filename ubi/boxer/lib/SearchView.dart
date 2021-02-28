import 'package:flutter/material.dart';
import 'package:edit_distance/edit_distance.dart';

import 'ItemView.dart';
import 'Database.dart';
import 'utils.dart';

final subsequence = LongestCommonSubsequence();
final levenshtein = Levenshtein();

int dist(x, y) {
  return levenshtein.distance(x.toLowerCase(), y.toLowerCase());
}

class SearchView extends StatefulWidget {
  @override
  State<StatefulWidget> createState() => SearchViewState();
}

class SearchViewState extends State<SearchView> {
  List<Map> allItems = [], items = [];

  @override
  void initState() {
    super.initState();
    refresh();
  }

  void refresh() {
    DB.getAllItems().then((res) => setState(() => allItems = items = res));
  }

  Widget _buildTile(context, i) {
    if (i.isOdd) return Divider();
    final Map item = items[i ~/ 2];
    return ListTile(
      title: Text(item['name']),
      subtitle: Text('${item['room_name']} / ${item['box_name']}'),
      leading: maybeImage(item['photo']) ?? boxPhoto,
      onTap: () => pushWidget(context: context, widget: ItemView(item: item)),
      onLongPress: () {},
    );
  }

  void filterItems(query) {
    items = [...allItems];
    //items = items.where((x) => dist(query, x['name']) <= query.length - x['name'].length);
    items = items.where((x) => dist(query, x['name']) <= x['name'].length - query.length + 1).toList();
    items.sort((x, y) => dist(query, x['name']).compareTo(dist(query, y['name'])));
  }

  @override
  Widget build(context) => Scaffold(
    appBar: AppBar(
      title: Text('Search'),
    ),
    body: Column(children: [
      Expanded(child: ListView.builder(
        itemBuilder: _buildTile,
        itemCount: items.length * 2,
        shrinkWrap: true,
      )),
      Row(children: [
        Flexible(child: TextField(
          decoration: InputDecoration(
            labelText: 'Search for items',
            hintText: 'Type to search',
            prefixIcon: Icon(Icons.search),
            filled: false,
            border: InputBorder.none,
            //border: OutlineInputBorder(borderRadius: BorderRadius.all(Radius.circular(100))),
          ),
          onChanged: (value) {
            setState(() {
              filterItems(value);
            });
          },
        )),
      ])
    ]),
  );
}
