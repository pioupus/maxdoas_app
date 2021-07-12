Index = 1

next = true;
dirlist = new TStringList();
dirlist.add('spektren/spektren20111005/');
dirlist.add('spektren/spektren20111021/');
dirlist.add('spektren/spektren20111104/');
dirlist.add('spektren/spektren20111107/');
dirlist.add('spektren/spektren20111109/');
dirlist.add('spektren/spektren20111110/');

dirlist.add('spektren/spektren20111111/');
dirlist.add('spektren/spektren20111114/');
dirlist.add('spektren/spektren20111115/');
dirlist.add('spektren/spektren20111116/');
dirlist.add('spektren/spektren20111118/');
dirlist.add('spektren/spektren20111121/');
dirlist.add('spektren/spektren20111122/');
dirlist.add('spektren/spektren20111123/');
dirlist.add('spektren/spektren20111124/');
dirlist.add('spektren/spektren20111125/');
dirlist.add('spektren/spektren20111128/');
dirlist.add('spektren/spektren20111130/');

dirlist.add('spektren/spektren20111201/');
dirlist.add('spektren/spektren20111202/');
dirlist.add('spektren/spektren20111205/');
dirlist.add('spektren/spektren20111206/');
dirlist.add('spektren/spektren20111208/');

FileList = TDirlist(dirlist,1)
free(dirlist);
//FileList = TDirlist('spektren/spektren20111021/',1)
while(next){
	
	Offset = new TSpektrum();
	SpecImg= new TSpektralImage();
	next = FileList.gotoNextCompleteSequence(4);

	ok = Offset.Load(FileList.getFileName('darkspectrum'));
	ok = SpecImg.Load(FileList.getFileName('Image'))&&ok;
	//ok = SpecImg.Load("/home/arne/diplom/software/application/diplom_maxdoas/spektren/spektren20111114/Image_2011_11_14__14_03_22_seq00038s.spe")&&ok;

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
		SpecImg.plot(1,40);
		ItenImage = SpecImg.getIntensityArray();
		qc = new TQDoasConfigFile("/home/arne/diplom/software/application/qdoas/projects/Evaluation_SO2_NO2.xml");
		qc.setOffset(Offset);
		qc.saveWorkingCopy("retrievals/qdoas","20111021.xml",FileList.getDir());
		plot.setXAxisTitle("mirror")
		plot.setYAxisTitle("fix")
		if (1){	
			qdoas.retrieve(SpecImg,qc);	
			NO2Img = qdoas.getRetrievalImage("NO2.SlCol(NO2)");
			NO2RMS = qdoas.getRetrievalImage("NO2.RMS");

			SO2Img = qdoas.getRetrievalImage("SO2.SlCol(SO2)");
			SO2RMS = qdoas.getRetrievalImage("SO2.RMS");

			NO2Img.save("retrievals_/"+FileList.getGroupName()+"retr_"+leadingZero(4,Index)+"_NO2_cl.txt",1,0);
			NO2RMS.save("retrievals_/"+FileList.getGroupName()+"retr_"+leadingZero(4,Index)+"_NO2_RMS.txt",1,0);
			SO2Img.save("retrievals_/"+FileList.getGroupName()+"retr_"+leadingZero(4,Index)+"_SO2_cl.txt",1,0);
			SO2RMS.save("retrievals_/"+FileList.getGroupName()+"retr_"+leadingZero(4,Index)+"_SO2_RMS.txt",1,0);
			ItenImage.save("retrievals_/"+FileList.getGroupName()+"retr_"+leadingZero(4,Index)+"_intensity.txt",1,0);
			//plot.setColorbarMinMax(1e16,3e78);
			//NO2Img.plot(1,40);

			free(NO2Img)
			free(NO2RMS)
			free(SO2Img)
			free(SO2RMS)
			free(ItenImage)
		}
		free(qc);
		//sleep(100)
		//print(Index.toString());
		Index = Index +1;
	}else{
		print("couldnt loaded("+leadingZero(4,Index)+"): "+FileList.getGroupName());
	}
	free(Offset);
	free(SpecImg);

	
  
}
free(FileList);
