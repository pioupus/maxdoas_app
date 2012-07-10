Index = 1

next = true;

FileList = TDirlist('spektren/spektren20111130/',1)
//FileList = TDirlist('spektren/spektren20111021/',1)
while(next){
	
	Offset = new TSpektrum();
	SpecImg= new TSpektralImage();
	next = FileList.gotoNextCompleteSequence(4);

	ok = Offset.Load(FileList.getFileName('darkspectrum'));
	ok = SpecImg.Load(FileList.getFileName('Image'))&&ok;
	if (ok){	
		print(FileList.getFileName('Image'));
		Offset.interpolatePixel(412)//compensate Pixelerror at 307nm
		Offset.interpolatePixel(559)//compensate Pixelerror at 319nm
		SpecImg.interpolatePixel(412)
		SpecImg.interpolatePixel(559)
		plot.setTitle("Spectrum");
		plot.setXAxisTitle("Wavelength")
		plot.setYAxisTitle("Intensity")

		Offset.plot(0);
		//SpecImg.plot(1,40);
		qc = new TQDoasConfigFile("/home/arne/diplom/software/application/qdoas/projects/Evaluation_SO2_NO2.xml");
		qc.setOffset(Offset);
		qc.saveWorkingCopy("spektren/spektren20111021/qdoas","20111021.xml","spektren/spektren20111021");
		qdoas.retrieve(SpecImg,qc);
		//retImg = qdoas.getRetrievalImage("NO2.SlCol(NO2)");
		plot.setXAxisTitle("mirror")
		plot.setYAxisTitle("fix")
		retImg = qdoas.getRetrievalImage("NO2.SlCol(NO2)");
		print("funktioniert1..");

		//retImg.save("testrt"+Index.toString());
		plot.setColorbarMinMax(1e16,3e78);
		retImg.plot(1,40);
		free(qc);
		free(retImg)
		//sleep(100)
		//print(Index.toString());
		print("funktioniert2..");
	}
	free(Offset);
	free(SpecImg);
	Index = Index +1;
	print("funktioniert3..");
  
}
free(FileList);
