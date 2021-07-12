Index = 1

next = true;
dirlist = new TStringList();
dirlist.add('/home/arne/diplom/software/application/diplom_maxdoas/retrievals/qdoas_retrievals_2011_11_09');

molecToTonPerdayNO2 = 1/(6.022e23)*46.01;//now gram per second/cm2
molecToTonPerdayNO2 = molecToTonPerdayNO2*0.0864;//now tons per day
molecToTonPerdayNO2 = molecToTonPerdayNO2*10000
FileList = TDirlistCCARetImg(dirlist,962)
free(dirlist);
//FileList = TDirlist('spektren/spektren20111021/',1)

	VectorSolver.setEmissionFactor(molecToTonPerdayNO2);
	VectorSolver.setImageSmoothValue(5);
    VectorSolver.setSrcSmoothValue(5);
	VectorSolver.setUseAvgAprioriFilter(true);
//	VectorSolver.setConstraintVec(1e8);
	VectorSolver.setAPrioriVec(1,1);

	VectorSolver.setMeanDistance(1000);
    VectorSolver.setConstraintSrcOET(1e-17);
    VectorSolver.setConstraintSrcTikhonov(1e5);
	SADiagSecond = 1e32;
	SADiagFirst = 1e0;
	VecConstrainedSecond = 1e40;
	VecConstrainedFirst = 1e31;
	exportpath="/home/arne/diplom/software/application/diplom_maxdoas/scripts/exports/qdoas2011_11_09/";

	timestep2=60
	timestep1=60
	AKArithmetic = 1;

	stopatseqence = 98
	skippictures = 80

	increment = 1;

	circleindex = 1;

    maxCorridorLengthEnd = 0.0;
    maxCorridorLengthStart = 0.0;

    corridornumberlimit = 1.0;
    corridornumberStart = 0.0;


ok = true;
Filegroup = 5;

for(i=0;i<skippictures;i++){
	ok = FileList.gotoNextCompleteSequence(Filegroup);
}

while(ok){
	for (a = 0;a<increment;a++)
		ok = FileList.gotoNextCompleteSequence(Filegroup);

	if (stopatseqence <= Index+skippictures)
		break;
	if (ok){
		if (circleindex == 1){
			RetImgOld = new TRetrievalImageCCA(FileList.getFileName("NO2_cl"));
			ok = FileList.gotoNextCompleteSequence(Filegroup);
		}else{
			free(RetImgOld);
			RetImgOld = RetImgNew;
		}
		if (ok){
			print(FileList.getFileName("NO2_cl"));
			RetImgNew = new TRetrievalImageCCA(FileList.getFileName("NO2_cl"));
			//ThemalImage = new TRetrievalImageSIGIS(dl.getFileName("temp"),PixelSize,PixelSize);

			plot.setTitle("WindField");
			plot.setXAxisTitle("X")
			plot.setYAxisTitle("Y");
			if (1){
				for(iteration = 0;iteration < 2;iteration++){
					if(iteration == 0){
						VectorSolver.dontUseThisResultForApriori();
						VectorSolver.setAprioriVelocity(0);
						timestep=timestep1;
						VectorSolver.setConstraintVec(VecConstrainedFirst)
						VectorSolver.setSADiagonalvalue(SADiagFirst);
					}
					if(iteration == 1){
						//VectorSolver.loadAprioriByMean(RetImgVectorField);					
						print("loaded with mean vel = "+RetImgVectorField.getMeanVelocity().toString());
						free(RetImgVectorField);
						VectorSolver.setConstraintVec(VecConstrainedSecond);	
						VectorSolver.setSADiagonalvalue(SADiagSecond);	
			
						timestep=timestep2;
					}
					//if(iteration == 0)
						VectorSolver.solve(RetImgOld,RetImgNew);
					if (iteration==1){
						if (AKArithmetic){
							//print("DOF:"+ VectorSolver.getDOF().toString());
							AKX = VectorSolver.getAKDiagX();
							AKY = VectorSolver.getAKDiagY();
							AKSRC = VectorSolver.getAKDiagSRC();
							AKX.save(exportpath+leadingZero(3,circleindex+skippictures)+"AKDiagX.txt",true,false);
							AKY.save(exportpath+leadingZero(3,circleindex+skippictures)+"AKDiagY.txt",true,false);
							AKSRC.save(exportpath+leadingZero(3,circleindex+skippictures)+"AKDiagSRC.txt",true,false);
							free(AKX);
							free(AKY);
							free(AKSRC);
						}
						Residuum = VectorSolver.getResiduum();
						retrieved = VectorSolver.getdcoldt_retrieved();
						observed = VectorSolver.getdcoldt_observed();
						src = VectorSolver.getSrcMatrix();
						sasrc = VectorSolver.getSAinvDiagSRC();
						Residuum.save(exportpath+leadingZero(3,circleindex+skippictures)+"residuum.txt",true,false);
						retrieved.save(exportpath+leadingZero(3,circleindex+skippictures)+"retrieved.txt",true,false);
						observed.save(exportpath+leadingZero(3,circleindex+skippictures)+"observed.txt",true,false);
						src.save(exportpath+leadingZero(3,circleindex+skippictures)+"src.txt",true,false);
						sasrc.save(exportpath+leadingZero(3,circleindex+skippictures)+"sasrc.txt",true,false);
						free(Residuum);
						free(retrieved);
						free(observed);
						free(src);
						free(sasrc);
					}
					previmg = VectorSolver.getRetrievalPrevAvg();
					nextimg = VectorSolver.getRetrievalNextAvg();
					RetImgVectorField = VectorSolver.getRetrieval();

					if(iteration == 1){
						RetImgVectorField.plot(0,100);
						RetImgVectorField.oplotWindField(0,4,true,true);//plotindex,mergingpixels,normalize,excludeZeroValues
						//plot.plotToFile("pdf", "plots/test"+(Index+skippictures).toString()+".pdf",0,80,80, 72)
						print("WindField("+(Index+skippictures).toString()+"): Velocity[m/s]:"+RetImgVectorField.getMaxVelocity().toString());
					}
				
					if (1){
						emissionsrate = RetImgVectorField.emissionrate(timestep);
						emissionsrate.setMaxCorridorLength(maxCorridorLengthStart,maxCorridorLengthEnd);
						emissionsrate.setCorridorNumberLimit(corridornumberStart,corridornumberlimit);

						emissionsrate.calcEmissionPrev(previmg);
						if(iteration == 1){		
							emissionsrate.oplotEmissionRaster(0);		
							emissionsrate.plot(1);
						}
		//				print("old: "+RetImgVectorField.getTime().toString());
						//print("old: "+RetImgOld.getTime().toString()+"  new: "+RetImgNew.getTime().toString());

						emissionsrate.calcEmissionNext(nextimg);
						emissionsrate.calcCorrelation();
						
					}
						//print("SpeedOffset = "+speedoffset.toString()+" Speed[m/s]:"+emissionsrate.getCorrelationSpeed().toString());
						if(iteration == 1){
							previmg.save(exportpath+leadingZero(3,circleindex+skippictures)+"imgpref.txt",true,false);
							nextimg.save(exportpath+leadingZero(3,circleindex+skippictures)+"imgnext.txt",true,false);
							print("save to: "+exportpath+leadingZero(3,circleindex+skippictures)+"imgret.txt");
							RetImgVectorField.save(exportpath+leadingZero(3,circleindex+skippictures)+"imgret.txt",true,true);
						}
					if(1){
						
						if(iteration == 1)
							emissionsrate.save(exportpath+leadingZero(3,circleindex+skippictures)+"emissionsrate1.txt");

						free(emissionsrate);

					}
					free(previmg);
					free(nextimg);
	 			}
				print("");
				//free(ThemalImage);
				free(RetImgVectorField);
			}

		}	
	}	
	Index = Index +increment;
	circleindex = circleindex +1;
	//sleep(500)
}
free(RetImgOld);
free(RetImgNew);
