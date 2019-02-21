{
//	system("stty -F /dev/ttyUSB0 -hupcl"); //PREVENT RESET
//	system("stty -F /dev/ttyUSB0 115200"); //SET BAUDRATE
	fstream f;
	f.open("/dev/ttyUSB0");
	cout<<(f.is_open()?"Opened!":"Error :(");
	cout.flush();
	char c = 'p';
	while(1) {
		f.write(&c,1);
		cout<<c<<"_";
		f.read(&c,1);
		cout<<c<<"_";
		cout<<c<<endl;
		cin>>c;
	}
	f.close();
}
