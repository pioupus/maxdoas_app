Sum = 0
pattern = new Scanpath();
pattern.AddRect(0.5,0,1,0,0,1,1,1,10,10)
while(1){
	Sum = Sum+1;
	print('hello '+ pattern.count().toString()+" "+  Sum.toString());
	for (a=0;a<pattern.count();a++){
		spectr = MeasureSpektrum(pattern.getPoint(a));
//		spectr.plot();
		free(spectr);
		//sleep(10)
	}  
}
