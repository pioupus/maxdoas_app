Sum = 0
tl = TTimeLine();
tl.scaleTimeAxis(1);
plot.setTitle("NO2 VCD at "+getSiteName());
plot.setXAxisTitle("Time")
plot.setYAxisTitle("molec/cm2")
print("waiting..")
WaitUntilTomorrow(0,10);
print("continueing..")
for (i=1; i<=10; i++){
  print('hello'+ Sum.toString());
  tl.addPoint(msSinceEpoch(),Sum);
  tl.plot(0);
  Sum += 1
  sleep(1000);
}
plot.plotToFile("pdf","test1.pdf",0,300,200,72)

