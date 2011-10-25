Index = 1

next = true;
while(next){
	
	Spectrum = new Spektrum();

	next = Spectrum.LoadSpectrDefaultName('spektren/','darkspectrum',Index,0);
		
	plot.setTitle("Spectrum");
	plot.setXAxisTitle("Wavelength")
	plot.setYAxisTitle("Intensity")

	Spectrum.plot(0);

	print('max: '+Spectrum.max().toString());
	sleep(1000)
	free(Spectrum);
	
	Index = Index +1;

  
}
