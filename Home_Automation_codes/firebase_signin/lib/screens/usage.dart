import 'package:flutter/material.dart';
import 'dart:math';

import 'package:fl_chart/fl_chart.dart';


// Define data structure for a bar group
class DataItem {
  int x;
  double y1;
  double y2;
  double y3;
  DataItem(
      {required this.x, required this.y1, required this.y2, required this.y3});
}

class usage extends StatelessWidget {
  usage({Key? key}) : super(key: key);

  // Generate dummy data to feed the chart
  final List<DataItem> _myData = List.generate(
      30,
          (index) => DataItem(
        x: index,
        y1: Random().nextInt(20) + Random().nextDouble(),
        y2: Random().nextInt(20) + Random().nextDouble(),
        y3: Random().nextInt(20) + Random().nextDouble(),
      ));

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Usage'),
      ),
      body: Padding(
        padding: const EdgeInsets.all(10),
        child: SingleChildScrollView(
          scrollDirection: Axis.horizontal,
          child: SizedBox(
            width: 700,
            child: BarChart(

                BarChartData(

                borderData: FlBorderData(
                    border: const Border(
                      top: BorderSide(width: 1),
                      right: BorderSide(width: 1),
                      left: BorderSide(width: 1),
                      bottom: BorderSide(width: 1),
                    )),

                    titlesData: FlTitlesData(
                      show: true,
                      leftTitles: AxisTitles(
                        drawBehindEverything: true,
                        sideTitles: SideTitles(
                          showTitles: true,
                          reservedSize: 30,
                          getTitlesWidget: (value, meta) {
                            return Text(
                              value.toInt().toString(),
                              style: const TextStyle(
                                color: Color(0xFF606060),
                              ),
                              textAlign: TextAlign.left,
                            );
                          },
                        ),
                      ),
                      bottomTitles: AxisTitles(
                        sideTitles: SideTitles(
                          showTitles: true,
                          reservedSize: 36,
                          getTitlesWidget: (value, meta){
                            return Text(
                              value.toInt().toString(),
                              style: const TextStyle(
                                color: Color(0xFF606060),
                              ),
                              textAlign: TextAlign.left,
                            );
                          },
                        ),
                      ),
                      rightTitles: AxisTitles(),
                      topTitles: AxisTitles(),
                    ),

                groupsSpace: 10,
                barGroups: [
                  BarChartGroupData(x: 1, barRods: [
                    BarChartRodData(toY: 10, width: 15),
                  ]),
                  BarChartGroupData(x: 2, barRods: [
                    BarChartRodData(toY: 9, width: 15),
                  ]),
                  BarChartGroupData(x: 3, barRods: [
                    BarChartRodData(toY: 4, width: 15),
                  ]),
                  BarChartGroupData(x: 4, barRods: [
                    BarChartRodData(toY: 2, width: 15),
                  ]),
                  BarChartGroupData(x: 5, barRods: [
                    BarChartRodData(toY: 13, width: 15),
                  ]),
                  BarChartGroupData(x: 6, barRods: [
                    BarChartRodData(toY: 17, width: 15),
                  ]),
                  BarChartGroupData(x: 7, barRods: [
                    BarChartRodData(toY: 19, width: 15),
                  ]),
                  BarChartGroupData(x: 8, barRods: [
                    BarChartRodData(toY: 21, width: 15),
                  ]),
                ])),
          ),
        ),
      ),
    );
  }
}