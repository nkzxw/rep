time33 = function (a) {
	a = a.slice(0, 7);
	for (var b = 0, c = a.length, d = 5381; c > b; ++b) 
		d += (d << 5) + a.charAt(b).charCodeAt();
	return 2147483647 & d
};
