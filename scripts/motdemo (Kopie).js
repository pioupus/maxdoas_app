Index = 0
SetMotIdle(false);

pattern = new Scanpath();
pattern.AddRect(-5,-5,	5,5,	-5,5,	5,-5	,10,10)

Spectrum_avg = 2;
Spectrum_time = 350*1000;

while(1){
	MotHome();
	Index = Index+1;
	SetShutterOpen(false);
	print('Taking darkspectrum..');
	SetFixedIntegrationTime(Spectrum_time); 
	darkcurrent = MeasureSpektrum(0,Spectrum_avg,false);
	darkcurrent.SaveSpectrumDefName('spektren/','darkspectrum',Index);
	SetShutterOpen(true);
	SetFixedIntegrationTime(Spectrum_time);

	print('scan '+  Index.toString());
	SpecImg = new SpektralImage();

	for (a=0;a<pattern.count();a++){
		MotMove(pattern.getPoint(a));
		spectrum = MeasureSpektrum(0,Spectrum_avg,true);
		SpecImg.add(pattern.getPoint(a),spectrum);
		plot.setTitle("Spectrum");
		plot.setXAxisTitle("Wavelength")
		plot.setYAxisTitle("Intensity")
		spectrum.plot(0);
		print('max: '+spectrum.max().toString());
  	}
	SpecImg.save('spektren/','Image',Index)

	SpecImg.plot(1)
	plot.setTitle("Intensity",1);
	plot.setXAxisTitle("fixed motor",1)
	plot.setYAxisTitle("mirror motor",1)
	free(SpecImg);
}
