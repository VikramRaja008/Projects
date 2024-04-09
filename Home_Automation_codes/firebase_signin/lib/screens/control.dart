import 'dart:async';
import 'dart:collection';
import 'package:intl/intl.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';
import 'package:firebase_signin/reusable_widgets/toggle_buttons_1.dart';
import 'package:firebase_auth/firebase_auth.dart';

class controlScreen extends StatefulWidget {
  const controlScreen({Key? key, this.text}) : super(key: key);
  final String? text;
  @override
  _controlScreenState createState() => _controlScreenState();

}
late String value4,value1,value2,value3;
late String value41,value11,value21,value31;

final uid =  FirebaseAuth.instance.currentUser?.uid;
late String temp= uid.toString();
late StreamSubscription _sub;

class _controlScreenState extends State<controlScreen> {

  TextEditingController _textFieldController= TextEditingController();
  List<List <bool>> isSwitched = [[false, false, false, false],[false, false, false, false],[false, false, false, false]];
  final FirebaseAuth _auth = FirebaseAuth.instance;
  DatabaseReference dbRef= FirebaseDatabase.instance.reference().child("users").child(FirebaseAuth.instance.currentUser!.uid).child("devices");
  List<Map<dynamic, dynamic>> lists = [];
  List<String> lists1 = [];

  List<Map<dynamic, dynamic>> lists11 = [];
  List<List<String>> names1 = [];

  final Stream _myStream =
  Stream.periodic(const Duration(seconds: 1), (int count) {


  });

  String? codeDialog;
  String? valueText;

  Future<void> _displayTextInputDialog(BuildContext context) async {
    return showDialog(
        context: context,
        builder: (context) {
          return AlertDialog(
            title: Text('TextField in Dialog'),
            content: TextField(
              onChanged: (value) {
                setState(() {
                  valueText = value;
                });
              },
              controller: _textFieldController,
              decoration: InputDecoration(hintText: "Text Field in Dialog"),
            ),
            actions: <Widget>[
              FlatButton(
                color: Colors.red,
                textColor: Colors.white,
                child: Text('CANCEL'),
                onPressed: () {
                  setState(() {
                    Navigator.pop(context);
                  });
                },
              ),
              FlatButton(
                color: Colors.green,
                textColor: Colors.white,
                child: Text('OK'),
                onPressed: () {
                  setState(() {
                    codeDialog = valueText;
                    Navigator.pop(context);
                  });
                },
              ),
            ],
          );
        });
  }


  @override
  void initState() {
    super.initState();
  dbRef.once().then((DataSnapshot snapshot) async{
    lists1.clear();
    lists.clear();
    Map<dynamic, dynamic> values =  snapshot.value;
    values.forEach((key, values) {
      lists1.add(key);
      lists.add(values);
      isSwitched.add([false, false, false, false]);
      names1.add(["", "", "", ""]);
    });
  });


 /*   String temp1;
    _sub=_myStream.listen((event) {
      for (int i = 0; i < lists1.length; i++) {

        dbRef.child(lists1[i]).child("s")
            .once()
            .then((result) {
          value1 = result.value.toString();
          temp1=value1;
          print(value1);
          if(temp1!=value1)
          {
            print("online");
          }
          else if(temp1==value1)
          {
            print("offline");
          }
        });

      }


    });*/


    dbRef.onChildAdded.listen((event) async{

      for(int i=0;i<lists1.length;i++) {

        await dbRef.child(lists1[i]).child("relay Name")
            .once()
            .then((result) {
          value11 = result.value.toString();
      names1[i][0]=value11;
        });

        await dbRef.child(lists1[i]).child("relay1 Name")
            .once()
            .then((result) {
          value21 = result.value.toString();
          names1[i][1]=value21;
        });

        await dbRef.child(lists1[i]).child("relay2 Name")
            .once()
            .then((result) {
          value31 = result.value.toString();
          names1[i][2]=value31;
        });

        await dbRef.child(lists1[i]).child("relay3 Name")
            .once()
            .then((result) {
          value41 = result.value.toString();
          names1[i][3]=value41;
        });

        print("ff"+names1[i][0]+names1[i][1]+names1[i][2]+names1[i][3]);

        dbRef.child(lists1[i]).child("relay")
            .once()
            .then((result) {
          value1 = result.value.toString();
          if(value1=='ON')
          {
            setState(() {
              isSwitched[i][0] = true;
            });
          }
          else if(value1=='OFF')
          {
            setState(() {
              isSwitched[i][0] = false;
            });
          }
        });

        dbRef.child(lists1[i]).child("relay1")
            .once()
            .then((result) {
          value2 = result.value.toString();
          if(value2=='ON')
          {
            setState(() {
              isSwitched[i][1] = true;
            });
          }
          else if(value2=='OFF')
          {
            setState(() {
              isSwitched[i][1] = false;
            });
          }

        });
        dbRef.child(lists1[i]).child("relay2")
            .once()
            .then((result) {
          value3 = result.value.toString();
          if(value3=='ON')
          {
            setState(() {
              isSwitched[i][2] = true;
            });
          }
          else if(value3=='OFF')
          {
            setState(() {
              isSwitched[i][2] = false;
            });
          }

        });
        dbRef.child(lists1[i]).child("relay3")
            .once()
            .then((result) {
          value4 = result.value.toString();
          if(value4=='ON')
          {
            setState(() {
              isSwitched[i][3] = true;
            });
          }
          else if(value4=='OFF')
          {
            setState(() {
              isSwitched[i][3] = false;
            });
          }

        });
      }
    });


dbRef.onChildChanged.listen((event) {

      for(int i=0;i<lists1.length;i++) {

        dbRef.child(lists1[i]).child("relay")
            .once()
            .then((result) {
          value1 = result.value.toString();
          if(value1=='ON')
          {
            setState(() {
              isSwitched[i][0] = true;
            });
          }
          else if(value1=='OFF')
          {
            setState(() {
              isSwitched[i][0] = false;
            });
          }
        });
        dbRef.child(lists1[i]).child("relay1")
            .once()
            .then((result) {
          value2 = result.value.toString();
          if(value2=='ON')
          {
            setState(() {
              isSwitched[i][1] = true;
            });
          }
          else if(value2=='OFF')
          {
            setState(() {
              isSwitched[i][1] = false;
            });
          }

        });
        dbRef.child(lists1[i]).child("relay2")
            .once()
            .then((result) {
          value3 = result.value.toString();
          if(value3=='ON')
          {
            setState(() {
              isSwitched[i][2] = true;
            });
          }
          else if(value3=='OFF')
          {
            setState(() {
              isSwitched[i][2] = false;
            });
          }

        });
        dbRef.child(lists1[i]).child("relay3")
            .once()
            .then((result) {
          value4 = result.value.toString();
          if(value4=='ON')
          {
            setState(() {
              isSwitched[i][3] = true;
            });
          }
          else if(value4=='OFF')
          {
            setState(() {
              isSwitched[i][3] = false;
            });
          }

        });
      }
    });



  }




  Future<void> _loadData() async {
    setState(() {});
  }



  @override
  Widget build(BuildContext context) {
    return Scaffold(
        backgroundColor: const Color(0xFFEEEEEE),
        appBar: AppBar(
          title: const Text('Control'),
        ),

    body: RefreshIndicator(

        onRefresh: _loadData,


        child: FutureBuilder(
            future: dbRef.once(),

            builder: (context, AsyncSnapshot<DataSnapshot> snapshot) {
              if (snapshot.hasData) {
                lists.clear();
                lists1.clear();
                Map<dynamic, dynamic> values = snapshot.data!.value;
                values.forEach((key, values) {
                  lists1.add(key);
                  lists.add(values);
                });
/*
StreamBuilder(
  stream: Stream.periodic(const Duration(seconds: 1)),
  builder: (context, snapshot) {
    return Center(
      child: Text(
        DateFormat('MM/dd/yyyy hh:mm:ss').format(DateTime.now()),
      ),
    );
  },
);*/

                return new ListView.builder(
                    shrinkWrap: true,
                    itemCount: lists1.length,
                    itemBuilder: (BuildContext context, int index) {


                      return Card(
                        elevation: 4,
                          margin: EdgeInsets.only(top:10, left: 20, right:20, bottom: 10),
                        shape:RoundedRectangleBorder(
                            borderRadius: BorderRadius.circular(20)
                        ),

                        child: Column(

                          mainAxisAlignment: MainAxisAlignment.center,
                          crossAxisAlignment: CrossAxisAlignment.start,

                          children: <Widget>[

                            Container(
                              padding: EdgeInsets.only(top:10,left: 20, right:20),
                            child:Text("Node" +index.toString()),
                            ),

                      Row(
                      mainAxisAlignment: MainAxisAlignment.center,
                      children: <Widget>[
                            Container(
                              padding: EdgeInsets.only(left: 10, right:60),
                              child:Text(names1[index][0]),
                            ),
/*
                       Container(
                          padding: EdgeInsets.only(left: 60, right:60),
                          child:Text(""),
                        ),
                      */

                     Container(
                          child: ElevatedButton(
                            onPressed: () async{
                              await _displayTextInputDialog(context);
                              print(valueText);
                              await dbRef.child(lists1[index]).update({"relay Name": valueText,
                              });
      },
                            child: const Text('Edit'),
                          ),
                        ),


    ]),


                            Row(
                                mainAxisAlignment: MainAxisAlignment.center,
                                children: <Widget>[
                       Container(
                         padding: EdgeInsets.only(left: 10, right:60),
                          child:Text("Relay1"),
                        ),


                                  Container(
                                    padding: EdgeInsets.only(left: 60, right:30),
                                    child: Switch(
                                      key: ValueKey("0"),
                                      value: isSwitched[index][0],
                                      onChanged: (value) async{

                                        setState(() {
                                          isSwitched[index][0] = value;
                                        });

                                        if(isSwitched[index][0]==true)
                                        {
                                          await dbRef.child(lists1[index]).update({"relay": "ON",
                                          });
                                        }
                                        else if (isSwitched[index][0]==false)
                                        {
                                          await dbRef.child(lists1[index]).update({"relay": "OFF",
                                          });
                                        }

                                      },
                                      activeTrackColor: Colors.lightGreenAccent,
                                      activeColor: Colors.green,
                                    ),

                                  ),
                                ]),

                            const SizedBox(
                              width: 2,
                              height: 10,
                            ),
                      Row(
                      mainAxisAlignment: MainAxisAlignment.center,
                      children: <Widget>[
                      Container(
                        padding: EdgeInsets.only(left: 10, right:60),
                      child:Text(names1[index][1]),
                      ),


                        Container(
                          child: ElevatedButton(
                            onPressed: () async{
                              await _displayTextInputDialog(context);
                              print(valueText);
                              await dbRef.child(lists1[index]).update({"relay1 Name": valueText,
                              });
                            },
                            child: const Text('Edit'),
                          ),
                        ),


                      ]),

                            Row(
                                mainAxisAlignment: MainAxisAlignment.center,
                                children: <Widget>[

                                  Container(
                                    padding: EdgeInsets.only(left: 10, right:60),
                                    child:Text("Relay2"),
                                  ),


                                  Container(
                                    padding: EdgeInsets.only(left: 60, right:30),


                                    child: Switch(
                                      value: isSwitched[index][1],
                                      onChanged: (value) async{

                                        setState(() {
                                          isSwitched[index][1] = value;
                                        });

                                        if(isSwitched[index][1]==true)
                                        {
                                          await dbRef.child(lists1[index]).update({"relay1": "ON",
                                          });
                                        }
                                        else if (isSwitched[index][1]==false)
                                        {
                                          await dbRef.child(lists1[index]).update({"relay1": "OFF",
                                          });
                                        }

                                      },
                                      activeTrackColor: Colors.lightGreenAccent,
                                      activeColor: Colors.green,
                                    ),

                                  ),
                                ]),


                            const SizedBox(
                              width: 2,
                              height: 10,
                            ),
                            Row(
                                mainAxisAlignment: MainAxisAlignment.center,
                                children: <Widget>[
                                  Container(
                                    padding: EdgeInsets.only(left: 10, right:60),
                                    child:Text(names1[index][2]),
                                  ),

                                  Container(
                                    child: ElevatedButton(
                                      onPressed: () async{
                                        await _displayTextInputDialog(context);
                                        print(valueText);
                                        await dbRef.child(lists1[index]).update({"relay2 Name": valueText,
                                        });
                                      },
                                      child: const Text('Edit'),
                                    ),
                                  ),

    ]),


                            Row(
                                mainAxisAlignment: MainAxisAlignment.center,
                                children: <Widget>[
                                  Container(
                                    padding: EdgeInsets.only(left: 10, right:60),
                                    child:Text("Relay3"),
                                  ),


                                  Container(
                                    padding: EdgeInsets.only(left: 60, right:30),

                                    child: Switch(
                                      value: isSwitched[index][2],
                                      onChanged: (value) async{

                                        setState(() {
                                          isSwitched[index][2] = value;
                                        });

                                        if(isSwitched[index][2]==true)
                                        {
                                          await dbRef.child(lists1[index]).update({"relay2": "ON",
                                          });
                                        }
                                        else if (isSwitched[index][2]==false)
                                        {
                                          await dbRef.child(lists1[index]).update({"relay2": "OFF",
                                          });
                                        }

                                      },
                                      activeTrackColor: Colors.lightGreenAccent,
                                      activeColor: Colors.green,
                                    ),

                                  ),
                                ]),


                            const SizedBox(
                              width: 2,
                              height: 10,
                            ),

                      Row(
                      mainAxisAlignment: MainAxisAlignment.center,
                      children: <Widget>[
                      Container(
                        padding: EdgeInsets.only(left: 10, right:60),
                      child:Text(names1[index][3]),
                      ),

                        Container(
                          child: ElevatedButton(
                            onPressed: () async{
                              await _displayTextInputDialog(context);
                              print(valueText);
                              await dbRef.child(lists1[index]).update({"relay3 Name": valueText,
                              });
                            },
                            child: const Text('Edit'),
                          ),
                        ),


                          ],
                        ),



                            Row(
                              mainAxisAlignment: MainAxisAlignment.center,
                              children: <Widget>[
                                Container(
                                  padding: EdgeInsets.only(left: 10, right:60),
                                  child:Text("Relay4"),
                                ),

                                Container(
                                  padding: EdgeInsets.only(left: 60, right:30),


                                  child: Switch(
                                    value: isSwitched[index][3],
                                    onChanged: (value) async{

                                      setState(() {
                                        isSwitched[index][3] = value;
                                      });

                                      if(isSwitched[index][3]==true)
                                      {
                                        await dbRef.child(lists1[index]).update({"relay3": "ON",
                                        });
                                      }
                                      else if (isSwitched[index][3]==false)
                                      {
                                        await dbRef.child(lists1[index]).update({"relay3": "OFF",
                                        });
                                      }

                                    },
                                    activeTrackColor: Colors.lightGreenAccent,
                                    activeColor: Colors.green,
                                  ),

                                ),

                              ],
                            ),


                      Container(
                      padding: EdgeInsets.only(left: 240, right:0),
                            child: ElevatedButton(
                              onPressed: () async{
                                await dbRef.child(lists1[index]).remove();
                                setState(() {});

                              },
                              child: const Text('Remove'),
                            ),
                      ),
                          ]),

                      );


                    });
              }
              return CircularProgressIndicator();
            })
    )
    ,

    );
  }
  @override
  void dispose() {
   // _sub.cancel();
    super.dispose();
  }

}

