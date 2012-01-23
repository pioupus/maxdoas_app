Index = 0
pattern = new Scanpath();
pattern.AddRect(65,-5, 65,5,	75,-5,	75,5	,10,10)
//SetAutoIntegrationTime(40,10,200*1000);
//plot.plotAutoIntegrationtimeParameters(true);
//SetFixedIntegrationTime(50*1000);
Spectrum_avg = 2;
Spectrum_time = 450*1000;
//MotMove(0,-0);
//MotMove(0,45);
plot.setTitle("Spectrum");
plot.setXAxisTitle("Wavelength")
plot.setYAxisTitle("Intensity")
while(1){
	Index = Index+1;

	sleep(500);
	i = GetMinimumIntegrationTime();
	print('MinIntegTime '+  i.toString());
	SetFixedIntegrationTime(GetMinimumIntegrationTime());
	print('Taking darkoffset..');
	darkoffset = MeasureSpektrum(0,100,false);
	plot.setTitle("DarkOffset");
	darkoffset.plot(0);
	darkoffset.SaveSpectrumDefNameDark('spektren/','darkoffset',Index);
	free(darkoffset);
	
	SetFixedIntegrationTime(Spectrum_time);


	print('scan '+  Index.toString());
	SpecImg = new SpektralImage();

	for (a=0;a<pattern.count();a++){
		MotMove(pattern.getPoint(a));
		spectr1 = MeasureSpektrum(0,Spectrum_avg,true);

		SpecImg.add(pattern.getPoint(a),spectr1);
		
		plot.setTitle("Spectrum");

		spectr1.plot(0);

		print('max: '+spectr1.max().toString());

		//free(spectr1);  
  	}
	SpecImg.save('spektren/','Image',Index)

	SpecImg.plot(1)
	plot.setTitle("Intensity",1);
	plot.setXAxisTitle("fixed motor",1)
	plot.setYAxisTitle("mirror motor",1)

	free(SpecImg);

  
}
