Sum = 0
pattern = new Scanpath();
pattern.AddRect(0.5,0,1,0,0,1,1,1,2,2)
//SetAutoIntegrationTime(50,10,50000);
SetFixedIntegrationTime(500*1000);
plot.plotAutoIntegrationtimeParameters(true);
while(1){
  Sum = Sum+1;
  print('hello '+ pattern.count().toString()+" "+  Sum.toString());
  
  for (a=0;a<pattern.count();a++){

    spectr1 = MeasureSpektrum(pattern.getPoint(a));
    plot.setTitle("Spectrum 1");
    plot.setXAxisTitle("Wavelength")
    plot.setYAxisTitle("Intensity")

    spectr1.plot(0);
	print('rms: '+spectr1.rms().toString());
  
	free(spectr1);  
  }  
  
}
