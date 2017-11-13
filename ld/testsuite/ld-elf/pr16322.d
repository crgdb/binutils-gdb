#ld: -shared -z relro -z notextonly
#readelf: -l --wide
#target: *-*-linux-gnu *-*-gnu* *-*-nacl*

#...
  GNU_RELRO .*
#pass
