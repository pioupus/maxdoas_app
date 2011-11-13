Index = 1

next = true;
while(next){
	
	//Spectrum = new Spektrum();

	//next = Spectrum.LoadSpectrDefaultName('spektren/','darkspectrum',Index,0);
		
	//plot.setTitle("Spectrum");
	//plot.setXAxisTitle("Wavelength")
	//plot.setYAxisTitle("Intensity")

	//Spectrum.plot(0);

	//print('max: '+Spectrum.max().toString());
	//sleep(1000)
	//free(Spectrum);
	
	SpecImg2 = 	new SpektralImage();
	if (SpecImg2.Load('spektren/', 'Image',0,1,0)){

  		retrimage2 = SpecImg2.getIntensityArray();
	
		retrimage2.save('test2.txt');
		free(retrimage2);
	}else{
		print("couldnt load file");	
	}
	free(SpecImg2);

	Index = Index +1;

  
}
