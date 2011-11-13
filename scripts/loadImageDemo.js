Index = 1

next = true;
while(next){
	
	SpecImg = new SpektralImage();
	next = SpecImg.Load('spektren/','Image',Index,1,1);
		
	plot.setTitle("Spectrum");
	plot.setXAxisTitle("Wavelength")
	plot.setYAxisTitle("Intensity")

	SpecImg.plot(0,30);
	if (next){
  		retrimage2 = SpecImg.getIntensityArray();
	
		retrimage2.save('test2.txt');
		free(retrimage2);	
	}
	print("spectr "+Index.toString());
	//print('max: '+Spectrum.max().toString());
	sleep(1)
	free(SpecImg);
	
	Index = Index +1;

  
}
