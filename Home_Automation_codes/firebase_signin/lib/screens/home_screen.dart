import 'package:firebase_auth/firebase_auth.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:firebase_signin/screens/bl/MainPage.dart';
import 'package:firebase_signin/screens/control.dart';
import 'package:firebase_signin/screens/signin_screen.dart';
import 'package:firebase_signin/screens/usage.dart';
import 'package:flutter/material.dart';
import 'package:firebase_signin/reusable_widgets/reusable_widget.dart';
import 'package:firebase_signin/screens/bl/MainPage.dart';
import 'package:intl/intl.dart';

class HomeScreen extends StatefulWidget {
  const HomeScreen({Key? key}) : super(key: key);

  @override
  _HomeScreenState createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {
  DatabaseReference ref = FirebaseDatabase.instance.reference().child("users");
  @override
  Widget build(BuildContext context) {
    // to get size
    var size = MediaQuery.of(context).size;

    // style
    var cardTextStyle = TextStyle(
        fontFamily: "Montserrat Regular",
        fontSize: 14,
        color: Color.fromRGBO(63, 63, 63, 1));

    return Scaffold(
      body: Stack(
        children: <Widget>[
          Container(
            height: size.height * .3,
            decoration: BoxDecoration(
              image: DecorationImage(
                  alignment: Alignment.topCenter,
                  image: AssetImage('assets/images/top_header.png')),
            ),
          ),

          SafeArea(
            child: Padding(
              padding: EdgeInsets.all(9.0),
              child: Column(
                children: <Widget>[
                  Container(
                    height: 64,
                    margin: EdgeInsets.only(bottom: 20),
                    child: Row(
                      crossAxisAlignment: CrossAxisAlignment.start,
                      children: <Widget>[
                        CircleAvatar(
                          radius: 32,
                          backgroundImage: AssetImage(
                              'assets/images/logo.png'),
                        ),
                        SizedBox(
                          width: 16,
                        ),
                        Column(
                          mainAxisAlignment: MainAxisAlignment.center,
                          crossAxisAlignment: CrossAxisAlignment.start,
                          children: <Widget>[
                            Text(
                              'DASHBOARD',
                              style: TextStyle(
                                  fontFamily: "Montserrat Medium",
                                  color: Colors.white,
                                  fontSize: 20),
                            ),
                            Text(
                              'Master User',
                              style: TextStyle(
                                  fontSize: 14,
                                  color: Colors.white,
                                  fontFamily: "Montserrat Regular"),
                            )
                          ],
                        )
                      ],
                    ),
                  ),

                  const SizedBox(
                    height: 40,
                  ),


                  Expanded(
                    child: GridView.count(
                      mainAxisSpacing: 50,
                      crossAxisSpacing: 10,
                      primary: false,
                      crossAxisCount: 2,
                      children: <Widget>[

                        GestureDetector(
                        onTap: () {
                  Navigator.push(context,
                  MaterialPageRoute(builder: (context) => MainPage()));
                  },
                    child: Card(
                          shape:RoundedRectangleBorder(
                              borderRadius: BorderRadius.circular(8)
                          ),
                          elevation: 4,
                          child: Column(
                            mainAxisAlignment: MainAxisAlignment.center,
                            children: <Widget>[
                              logoWidget1("assets/images/logo.png"),
                              Text(
                                'Add Device',
                                style: cardTextStyle,
                              )
                            ],
                          ),
                        ),
                  ),

            GestureDetector(
              onTap: () {
                final uid =  FirebaseAuth.instance.currentUser?.uid;
                late String temp= uid.toString();
                Navigator.push(context,
                    MaterialPageRoute(builder: (context) => controlScreen(text: temp,)));
              },
              child: Card(
                          shape:RoundedRectangleBorder(
                              borderRadius: BorderRadius.circular(8)
                          ),
                          elevation: 4,
                          child: Column(
                            mainAxisAlignment: MainAxisAlignment.center,
                            children: <Widget>[
                              logoWidget1("assets/images/logo.png"),
                              Text(
                                'Control',
                                style: cardTextStyle,
                              )
                            ],
                          ),
                        ),
            ),


                        GestureDetector(
                          onTap: () {
                            Navigator.push(context,
                                MaterialPageRoute(builder: (context) => usage()));
                          },
                          child:Card(

                            shape:RoundedRectangleBorder(
                                borderRadius: BorderRadius.circular(8)
                            ),
                            elevation: 4,


                            child: Column(
                              mainAxisAlignment: MainAxisAlignment.center,
                              children: <Widget>[
                                logoWidget1("assets/images/logo.png"),
                                Text(
                                  'Usage',
                                  style: cardTextStyle,
                                )
                              ],
                            ),

                          ),
                        ),


                      ],
                    ),


                  ),


                  ElevatedButton(
                    child: Text("Logout"),
                    onPressed: () async{
                      await FirebaseAuth.instance.signOut().then((value) {
                        print("Signed Out");
                        Navigator.push(context,
                            MaterialPageRoute(builder: (context) => SignInScreen()));
                      });
                    },
                  ),

                ],
              ),

            ),
          ),
        ],
      ),
    );
  }
}