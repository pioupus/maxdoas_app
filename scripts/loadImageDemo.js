Index = 1

next = true;
while(next){
	
	SpecImg = new SpektralImage();
	next = SpecImg.Load('spektren/','Image',Index,0);
		
	plot.setTitle("Spectrum");
	plot.setXAxisTitle("Wavelength")
	plot.setYAxisTitle("Intensity")

	SpecImg.plot(0,30);
	print("spectr "+Index.toString());
	//print('max: '+Spectrum.max().toString());
	sleep(1)
	free(SpecImg);
	
	Index = Index +1;

  
}
