	Sum = 0
	pattern = new Scanpath();
	pattern.AddLine(-10,0,
					10,0,
					20)
	//SetAutoIntegrationTime(50,10,50000);
	SetFixedIntegrationTime(500*1000);
	plot.plotAutoIntegrationtimeParameters(true);
	SpecImg = new SpektralImage(pattern);

  	for (a=0;a<pattern.count();a++){

		spectr1 = MeasureSpektrum(pattern.getPoint(a));
	
		plot.setTitle("Spectrum 1");
		plot.setXAxisTitle("Wavelength")
		plot.setYAxisTitle("Intensity")

		spectr1.plot(0);
		print('rms: '+spectr1.rms().toString());
	  	SpecImg.add(pattern.getPoint(a),spectr1);
		
  	}  
  	SpecImg.save('spektren/','testLine',0)

	retrimage1 = SpecImg.getIntensityArray();
	
	retrimage1.save('test1.txt');

	free(SpecImg);
	free(retrimage1);

	SpecImg2 = 	new SpektralImage();
	if (SpecImg2.Load('spektren/', 'testLine',0,0,0)){

  		retrimage2 = SpecImg2.getIntensityArray();
	
		retrimage2.save('test2.txt');
		free(retrimage2);
	}else{
		print("couldnt load file");	
	}
	free(SpecImg2);

