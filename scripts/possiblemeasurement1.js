Sum = 0
pattern = new Scanpath();
pattern.AddRect(0.5,0,1,0,0,1,1,1,10,10)
SetAutoIntegrationTime(50,10,50000);
plot.plotAutoIntegrationtimeParameters(true);
while(1){
	Sum = Sum+1;
	print('hello '+ pattern.count().toString()+" "+  Sum.toString());
	SpecImg = new SpektralImage();
	for (a=0;a<pattern.count();a++){

		spectr1 = MeasureSpektrum(pattern.getPoint(a));
		plot.setTitle("Spectrum 1");
		plot.setXAxisTitle("Wavelength")
		plot.setYAxisTitle("Intensity")


		SpecImg.add(pattern.getPoint(a),spectr1);

		spectr1.plot(0);
		//plot.plotToFile("png",'spektren/','Measurement',Sum,0,300,200,72)//300x200mm 72dpi	

		spectr2 = MeasureSpektrum(pattern.getPoint(a));
		spectr2.add(spectr1);
		plot.setTitle("Spectrum 1 + Spectrum 2");
		plot.setXAxisTitle("Wavelength")
		plot.setYAxisTitle("Intensity")
		plot.setYAxisRange(0,3)
		plot.setCurveColor(6)
		plot.setLegend("Spektrum",0,1);
		plot.plotXYMarker(0,1,"",1)
		spectr2.plot(1);
	//	plot.plotToFile("pdf","test1.pdf",1,300,200,72)//300x200mm 72dpi	
	//	plot.plotToFile("svg","test1.svg",1,300,200,72)//300x200mm 72dpi
		

		spectr1.SaveSpectrumDefName('spektren/','Measurement',Sum);		
		print('rms: '+spectr1.rms().toString());
		print('mean: '+spectr1.mean().toString());
		print('stddev: '+spectr1.stddev().toString());
		//sleep(1)	
		free(spectr2);	
	}  
	SpecImg.save('spektren/','Image',Sum)
	free(SpecImg);
}
