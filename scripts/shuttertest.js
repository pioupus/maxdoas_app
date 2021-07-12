
MotHome();
while(1){
	print('closeing..');
	SetShutterOpen(false); //lets measure dark offset
	sleep(5000);
	print('opening..');

	SetShutterOpen(true);
	sleep(5000);

}
