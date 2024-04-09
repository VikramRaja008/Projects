import 'package:firebase_database/firebase_database.dart';
import 'package:flutter/material.dart';
import 'package:firebase_signin/screens/control.dart';

class ToggleButtons1 extends StatefulWidget {
  @override
  _ToggleButtons1State createState() => _ToggleButtons1State();
}

class _ToggleButtons1State extends State<ToggleButtons1> {
  List<bool> isSelected = [true, false, false];
  final dbRef = FirebaseDatabase.instance.reference().child("users").child("12").child("devices");
  @override
  Widget build(BuildContext context) => Container(
    color: Colors.green.withOpacity(0.5),
    child: ToggleButtons(
      isSelected: isSelected,
      selectedColor: Colors.white,
      color: Colors.black,
      fillColor: Colors.lightBlue.shade900,
      renderBorder: false,
      //splashColor: Colors.red,
      highlightColor: Colors.orange,
      children: <Widget>[
        Padding(
          padding: const EdgeInsets.symmetric(horizontal: 12),
          child: Text('ON', style: TextStyle(fontSize: 18)),
        ),
        Padding(
          padding: const EdgeInsets.symmetric(horizontal: 12),
          child: Text('OFF', style: TextStyle(fontSize: 18)),
        ),
        Padding(
          padding: const EdgeInsets.symmetric(horizontal: 12),
          child: Text('', style: TextStyle(fontSize: 18)),
        ),
      ],
      onPressed: (int newIndex) {

        setState(() {
          for (int index = 0; index < isSelected.length; index++) {
            if (index == newIndex) {
              isSelected[index] = true;
              dbRef.set({
                "646": {
                  "Relay": "ON"
                }
              });
              print(newIndex);
            } else {
              isSelected[index] = false;
              dbRef.set({
                "646": {
                  "Relay": "OFF"
                }
              });
            }
          }
        });
      },
    ),
  );
}