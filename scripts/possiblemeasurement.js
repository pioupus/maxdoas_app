Sum = 0
while(1){
	Avg = new TAvg;
	for (a=0;a<4;a++){
		DarkSpectrum = MeasureDark();
		pattern = IniScanPattern();
		scanimage = new TScanImage();
		for (i=0;i<pattern.count;i++){
			spectr = Measure(pattern[i]);
			spectr = spectr - DarkSpectrum;
			scanimage(pattern(i),spectr);
		
		}
		avg.add(scanimage);
	}	
	scanimage.save('Measurement');
}

