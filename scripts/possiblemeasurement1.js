Sum = 0
pattern = new Scanpath();
pattern.AddRect(0.5,0,1,0,0,1,1,1,10,10)
while(1){
	Sum = Sum+1;
	print('hello '+ pattern.count().toString()+" "+  Sum.toString());
	for (a=0;a<pattern.count();a++){
		spectr1 = MeasureSpektrum(pattern.getPoint(a));
		spectr1.plot(0);
		spectr2 = MeasureSpektrum(pattern.getPoint(a));
		spectr2.add(spectr1);
		spectr2.plot(1);
		spectr1.SaveSpectrumDefName('spektren/','Measurement',Sum);		
		print('rms: '+spectr1.rms().toString());
		print('mean: '+spectr1.mean().toString());
		print('stddev: '+spectr1.stddev().toString());
		//sleep(1)		
		free(spectr1);
		free(spectr2);
	}  
}
