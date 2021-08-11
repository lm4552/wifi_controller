
all: 	
	@ echo "creating certificates if not already existing..." 
	@ echo ""

	@ [[ !(-f key.h && -f cert.h) ]] \
	&& openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days 3650 \
	&& openssl x509 -inform PEM -outform DER -in cert.pem -out cert.der\
	&& openssl rsa -inform PEM -outform DER -in key.pem -out key.der\
	&& echo "#ifndef KEY_H_" > ./key.h \
	&& echo "#define KEY_H_" >> ./key.h \
	&& xxd -i key.der >> ./key.h \
	&& echo "#endif" >> ./key.h\
	&& echo "#ifndef CERT_H_" > ./cert.h \
	&& echo "#define CERT_H_" >> ./cert.h \
	&& xxd -i cert.der >> ./cert.h \
	&& echo "#endif" >> ./cert.h	
	
	@ echo ""
	@ echo "done!"

clean:	
		rm -f *.h *.der *.pem 
