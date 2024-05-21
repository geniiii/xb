internal Xtal_OSDateTime GetDateTime(void) {
	// TODO(geni): Error handling
	
	SYSTEMTIME time;
	GetLocalTime(&time);
	return (Xtal_OSDateTime){
		.year    = time.wYear,
		.month   = (u8) time.wMonth,
		.day     = (u8) time.wDay,
		.hour    = (u8) time.wHour,
		.minutes = (u8) time.wMinute,
		.seconds = (u8) time.wSecond,
	};
}
