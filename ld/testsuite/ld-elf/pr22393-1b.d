#source: pr22393-1.s
#ld: -shared -z relro -z textonly
#readelf: -l --wide
#target: *-*-linux-gnu *-*-gnu* *-*-nacl*

#failif
#...
 +[0-9]+  +.*.text.*(.eh_frame|\.rodata).*
#...
