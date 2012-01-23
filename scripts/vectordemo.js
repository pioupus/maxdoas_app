PI = 3.14159265359;
SIGISStepsTODeg = 0.0004886*180/PI;

//is multiplied with integrationline[m]*sum(cl*v[m/s])/count(cl)
ppmmToTonPerdaySO2 = 2.857/1000;//now gram per second
ppmmToTonPerdaySO2 = ppmmToTonPerdaySO2*0.0864;//now tons per day



Index = 1
//vorbildlicher shift:90,91,92,93,94,96,98,99,100,104
next = true;

//CONSTRAINT 1.00000e+07
//CONSTRAINT_SRC_OET 1.00000
//CONSTRAINT_SRC_THIK 1.0E+00
//SMOOTHEVALUE,SMOOTHEVALUEsource 3 1

//dl = new TDirlistSIGIS("/home/arne/diplom/SIGISData/artgradient_44x17");
//dl = new TDirlistSIGIS("/home/arne/diplom/SIGISData/artgradient_slit_44x17");
//dl = new TDirlistSIGIS("/home/arne/diplom/SIGISData/2007_12_01_00_13");

maxCorridorLengthEnd = 0.9;
maxCorridorLengthStart = 0.9;
corridornumberlimit = 0.9;
corridornumberStart = 0.25;

Filegroup = 2;
select = 6;
VectorSolver.setEmissionFactor(ppmmToTonPerdaySO2);
VectorSolver.setUseDirectPixelsize(false);
corrthreshold = 0.97;
AKArithmetic = 1;
if (select == 0){
	Filegroup = 5;
	SIGISSteps = 16;
	PixelSize = SIGISStepsTODeg * SIGISSteps;
	VectorSolver.setImageSmoothValue(3);
    VectorSolver.setSrcSmoothValue(3);
	VectorSolver.setUseMedianAprioriFilter(false);
	VectorSolver.setAPrioriVec(0,1);
	VectorSolver.setConstraintVec(1e5);
    VectorSolver.setConstraintSrcOET(100.7);
    VectorSolver.setConstraintSrcTikhonov(1.3);
	VectorSolver.setSADiagonalvalue(10.3);
	VectorSolver.setThermalTheshold(7);
	timestep1 = 15;
	timestep2 = 15;
	exportpath="/home/arne/diplom/software/application/diplom_maxdoas/scripts/exports/2007_12_02_02_41/";
	dl = new TDirlistSIGIS("/home/arne/diplom/SIGISData/2007_12_02_02_41");
	AKArithmetic = 1;
	skippictures = 59;
	stopatseqence = 62;
}
if(select == 1){
	Filegroup = 2;
	SIGISSteps = 10;
	PixelSize = SIGISStepsTODeg * SIGISSteps;
	VectorSolver.setImageSmoothValue(9);
    VectorSolver.setSrcSmoothValue(9);
	VectorSolver.setUseMedianAprioriFilter(true);
//	VectorSolver.setConstraintVec(1e8);
	VectorSolver.setConstraintVec(1e4);
	VectorSolver.setSADiagonalvalue(1e3);
	timestep1 = 5;
	timestep2 = 5;
	corrthreshold = 0.96;
	exportpath="/home/arne/diplom/software/application/diplom_maxdoas/scripts/exports/170306/";
	dl = new TDirlistSIGIS("/home/arne/diplom/SIGISData/170306");
	AKArithmetic = 1;
}
if(select == 2){
	PixelSize = 1;
	VectorSolver.setUseDirectPixelsize(true);
	VectorSolver.setImageSmoothValue(9);
    VectorSolver.setSrcSmoothValue(9);
	VectorSolver.setConstraintVec(1e8);
	timestep = 60;
}
if (select == 3){
	corrthreshold = 0.85;
	Filegroup = 5;
	SIGISSteps = 16;
	PixelSize = SIGISStepsTODeg * SIGISSteps;
	VectorSolver.setImageSmoothValue(3);
    VectorSolver.setSrcSmoothValue(3);
	VectorSolver.setUseMedianAprioriFilter(false);
	VectorSolver.setAPrioriVec(0,0);
//	VectorSolver.setConstraintVec(1e5);
	VectorSolver.setConstraintVec(1e1);
    VectorSolver.setConstraintSrcOET(100.7);
    VectorSolver.setConstraintSrcTikhonov(1.3);
	VectorSolver.setSADiagonalvalue(1.3);
	VectorSolver.setThermalTheshold(40);
	timestep1 = 15;
	timestep2 = 15;
	exportpath="/home/arne/diplom/software/application/diplom_maxdoas/scripts/exports/sif4_2008_11_17_07_18/";
	dl = new TDirlistSIGIS("/home/arne/diplom/SIGISData/sif4_300k/2008_11_17_07_18");
	AKArithmetic = 1;
	skippictures = 40;
	stopatseqence = 0;

}
if (select == 4){
	corrthreshold = 0.7;
	Filegroup = 5;
	SIGISSteps = 16;
	PixelSize = SIGISStepsTODeg * SIGISSteps;
	VectorSolver.setImageSmoothValue(5);
    VectorSolver.setSrcSmoothValue(5);
	VectorSolver.setUseMedianAprioriFilter(false);
	VectorSolver.setAPrioriVec(-1,2);
//	VectorSolver.setConstraintVec(1e5);
	VectorSolver.setConstraintVec(10);
    VectorSolver.setConstraintSrcOET(1e-17);
    VectorSolver.setConstraintSrcTikhonov(100.1);
	VectorSolver.setSADiagonalvalue(0.00005);
	VectorSolver.setThermalTheshold(20);
	timestep1 = 15;
	timestep2 = 15;
	exportpath="/home/arne/diplom/software/application/diplom_maxdoas/scripts/exports/sif4_2008_11_17_11_24/";
	dl = new TDirlistSIGIS("/home/arne/diplom/SIGISData/sif4_300k/2008_11_17_11_24");
	AKArithmetic = 1;
	skippictures = 0;
	stopatseqence = 0;

}
if (select == 5){
	corrthreshold = 0.85;
	Filegroup = 5;
	SIGISSteps = 16;
	PixelSize = SIGISStepsTODeg * SIGISSteps;
	VectorSolver.setImageSmoothValue(3);
    VectorSolver.setSrcSmoothValue(3);
	VectorSolver.setUseMedianAprioriFilter(false);
	VectorSolver.setAPrioriVec(0,0);
//	VectorSolver.setConstraintVec(1e5);
	VectorSolver.setConstraintVec(1e1);
    VectorSolver.setConstraintSrcOET(1e-17);
    VectorSolver.setConstraintSrcTikhonov(100);
	VectorSolver.setSADiagonalvalue(1.3);
	VectorSolver.setThermalTheshold(20);
	timestep1 = 15;
	timestep2 = 15;
	exportpath="/home/arne/diplom/software/application/diplom_maxdoas/scripts/exports/sif4_2008_11_17_10_12/";
	dl = new TDirlistSIGIS("/home/arne/diplom/SIGISData/sif4_300k/2008_11_17_10_12");
	AKArithmetic = 1;
	skippictures = 0;
	stopatseqence = 0;

}
if (select == 6){
	corrthreshold = 0.97;
	Filegroup = 5;
	SIGISSteps = 16;
	PixelSize = SIGISStepsTODeg * SIGISSteps;
	VectorSolver.setImageSmoothValue(3);
    VectorSolver.setSrcSmoothValue(3);
	VectorSolver.setUseMedianAprioriFilter(false);
	VectorSolver.setAPrioriVec(-1,2);
//	VectorSolver.setConstraintVec(1e5);
	VectorSolver.setConstraintVec(1e6);
    VectorSolver.setConstraintSrcOET(1e-17);
    VectorSolver.setConstraintSrcTikhonov(1.3);
	VectorSolver.setSADiagonalvalue(0.005);
	SADiagSecond = 5e4;
	VecConstrainedSecond = 5e5;
	VectorSolver.setThermalTheshold(245);
	timestep1 = 50;
	timestep2 = 50;
	exportpath="/home/arne/diplom/software/application/diplom_maxdoas/scripts/exports/so2diff_300k_2008_11_17_11_24/";
	dl = new TDirlistSIGIS("/home/arne/diplom/SIGISData/so2diff_300k_2008_11_17_11_24");
	AKArithmetic = 1;
	skippictures = 4;
	stopatseqence = 6;

    maxCorridorLengthEnd = 0.75;
    maxCorridorLengthStart = 0.9;

    corridornumberlimit = 1;
    corridornumberStart = 0.4;

}
ok = true;
for(i=0;i<skippictures;i++){
	ok = dl.gotoNextCompleteSequence(Filegroup);
}
while(ok){
	ok = dl.gotoNextCompleteSequence(Filegroup);
	if (stopatseqence == Index+skippictures)
		break;
	if (ok){
		if (Index == 1){
			RetImgOld = new TRetrievalImageSIGIS(dl.getFileName("ppm"),PixelSize,PixelSize);
			RetImgOld.loadWeightSIGIS(dl.getFileName("corr"),PixelSize,PixelSize);
			ok = dl.gotoNextCompleteSequence(Filegroup);
		}else{
			free(RetImgOld);
			RetImgOld = RetImgNew;
		}
		if (ok){
			RetImgNew = new TRetrievalImageSIGIS(dl.getFileName("ppm"),PixelSize,PixelSize);
			RetImgNew.loadWeightSIGIS(dl.getFileName("corr"),PixelSize,PixelSize);		

			ThemalImage = new TRetrievalImageSIGIS(dl.getFileName("temp"),PixelSize,PixelSize);

			plot.setTitle("WindField");
			plot.setXAxisTitle("X")
			plot.setYAxisTitle("Y");
			for(iteration = 0;iteration < 2;iteration++){
				if(iteration == 0){
					VectorSolver.dontUseThisResultForApriori();
					VectorSolver.setAprioriVelocity(0);
					timestep=timestep1;
				}
				if(iteration == 1){
					VectorSolver.loadAprioriByMean(RetImgVectorField);
					print("loaded with mean vel = "+RetImgVectorField.getMeanVelocity().toString());
					free(RetImgVectorField);
					VectorSolver.setConstraintVec(VecConstrainedSecond);	
					VectorSolver.setSADiagonalvalue(SADiagSecond);	
			
					timestep=timestep2;
				}
				VectorSolver.loadThermalImage(ThemalImage);
				VectorSolver.solve(RetImgOld,RetImgNew);
				if (iteration==1){
					if (AKArithmetic){
						//print("DOF:"+ VectorSolver.getDOF().toString());
						AKX = VectorSolver.getAKDiagX();
						AKY = VectorSolver.getAKDiagY();
						AKSRC = VectorSolver.getAKDiagSRC();
						AKX.save(exportpath+leadingZero(3,Index+skippictures)+"AKDiagX.txt",true,false);
						AKY.save(exportpath+leadingZero(3,Index+skippictures)+"AKDiagY.txt",true,false);
						AKSRC.save(exportpath+leadingZero(3,Index+skippictures)+"AKDiagSRC.txt",true,false);
						free(AKX);
						free(AKY);
						free(AKSRC);
					}
					Residuum = VectorSolver.getResiduum();
					retrieved = VectorSolver.getdcoldt_retrieved();
					observed = VectorSolver.getdcoldt_observed();
					src = VectorSolver.getSrcMatrix();
					sasrc = VectorSolver.getSAinvDiagSRC();
					Residuum.save(exportpath+leadingZero(3,Index+skippictures)+"residuum.txt",true,false);
					retrieved.save(exportpath+leadingZero(3,Index+skippictures)+"retrieved.txt",true,false);
					observed.save(exportpath+leadingZero(3,Index+skippictures)+"observed.txt",true,false);
					src.save(exportpath+leadingZero(3,Index+skippictures)+"src.txt",true,false);
					sasrc.save(exportpath+leadingZero(3,Index+skippictures)+"sasrc.txt",true,false);
					free(Residuum);
					free(retrieved);
					free(observed);
					free(src);
					free(sasrc);
				}
				previmg = VectorSolver.getRetrievalPrevAvg();
				nextimg = VectorSolver.getRetrievalNextAvg();
				RetImgVectorField = VectorSolver.getRetrieval();
				RetImgVectorField.scaleThresholdImageValues(corrthreshold);
				if(iteration == 1){
					RetImgVectorField.plot(0,20);
					RetImgVectorField.oplotWindField(0,4,true,true);//plotindex,mergingpixels,normalize,excludeZeroValues
					//plot.plotToFile("pdf", "plots/test"+(Index+skippictures).toString()+".pdf",0,80,80, 72)
					print("WindField("+(Index+skippictures).toString()+"): Velocity[m/s]:"+RetImgVectorField.getMaxVelocity().toString());
				}
		
				RetImgVectorField.thresholdImageValues(corrthreshold);			
				emissionsrate = RetImgVectorField.emissionrate(timestep);
			
				emissionsrate.setMaxCorridorLength(maxCorridorLengthStart,maxCorridorLengthEnd);
				emissionsrate.setCorridorNumberLimit(corridornumberStart,corridornumberlimit);
				nextimg.thresholdImageValues(corrthreshold);		
				previmg.thresholdImageValues(corrthreshold);		
				emissionsrate.calcEmissionPrev(previmg);
				if(iteration == 1){		
					emissionsrate.oplotEmissionRaster(0);		
					emissionsrate.plot(1);
				}
//				print("old: "+RetImgVectorField.getTime().toString());
				//print("old: "+RetImgOld.getTime().toString()+"  new: "+RetImgNew.getTime().toString());

				emissionsrate.calcEmissionNext(nextimg);
				emissionsrate.calcCorrelation();
				speedcorrection = emissionsrate.getCorrelationCorrectionfactor();
				if(iteration == 0){	
					RetImgVectorField.setSpeedCorrection(speedcorrection);
					nextimg.setSpeedCorrection(speedcorrection);
					previmg.setSpeedCorrection(speedcorrection);
				}
					emissionsrate.calcEmissionNext(nextimg);
					emissionsrate.calcEmissionPrev(previmg);
					emissionsrate.calcEmission(RetImgVectorField);
				
				if(iteration == 1){					
					plot.setCurveColor(5);
					emissionsrate.oplot(1);
					previmg.plot(2,20);
					nextimg.plot(3,20);		

					//emissionsrate.plotCorrelation(2);
				}

				//print("SpeedOffset = "+speedoffset.toString()+" Speed[m/s]:"+emissionsrate.getCorrelationSpeed().toString());
				previmg.save(exportpath+leadingZero(3,Index+skippictures)+"imgpref.txt",true,false);
				nextimg.save(exportpath+leadingZero(3,Index+skippictures)+"imgnext.txt",true,false);
				RetImgVectorField.save(exportpath+leadingZero(3,Index+skippictures)+"imgret.txt",true,true);
				emissionsrate.save(exportpath+leadingZero(3,Index+skippictures)+"emissionsrate.txt");
				free(emissionsrate);

				
				free(previmg);
				free(nextimg);
 			}
			print("");
			free(ThemalImage);
			free(RetImgVectorField);


		}	
	}	
	Index = Index +1;
	//sleep(500)
}
free(RetImgOld);
free(RetImgNew);
