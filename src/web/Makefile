
all: css html js 

css:
	@ echo "creating style.h from style.css..."

	@ echo "#ifndef STYLE_H_" > ./style.h
	@ echo "#define STYLE_H_" >> ./style.h
	@ echo "const char* style = " >> ./style.h
	
	@ sed ':a;N;$$!ba;s/\n//g' style.css >> style_temp.css
	@ sed -i 's/^/\"<style>/' style_temp.css
	@ sed -i 's/$$/<\/style>\";/' style_temp.css

	@ cat 'style_temp.css' >> ./style.h
	@ echo "#endif" >> ./style.h

	@ rm style_temp.css

	@ echo "done!"
	@ echo ""

html:
	@ echo "creating html.h from index.html..."

	@ sed ':a;N;$$!ba;s/\n//g' index.html > index_temp.html
	@ sed -i -e "s/\\\\'/\\\\\\\'/g" -e 's/"/\\"/g'  index_temp.html 
	@ #-e 's/\//\\\//g'

	@ echo "#ifndef HTML_H_" > ./html.h
	@ echo "#define HTML_H_" >> ./html.h
	@ echo "const char* html = " >> ./html.h

	@ sed -i 's/^/\"/' index_temp.html
	@ sed -i 's/$$/\";/' index_temp.html

	@ cat 'index_temp.html' >> ./html.h
	@ echo "#endif" >> ./html.h

	@ rm index_temp.html

	@ echo "done!"
	@ echo ""

js:
	@ echo "creating script.h from script.js..."

	@ sed ':a;N;$$!ba;s/\n//g' script.js > script_temp.js
	@ sed -i -e 's/"/\\"/g'  -e "s/\\\\xa0/\\\\\\\\\\\\\xa0/g"  script_temp.js 
	@ #-e "s/\\\\'/\\\\\\\\\\\\\\\'/g" -e 's/\s{2,}/\s/g'

	@ echo "#ifndef SCRIPT_H_" > ./script.h
	@ echo "#define SCRIPT_H_" >> ./script.h
	@ echo "const char* script = " >> ./script.h

	@ sed -i 's/^/\"<script>/' script_temp.js
	@ sed -i 's/$$/<\/script> \";/' script_temp.js

	@ cat script_temp.js >> ./script.h
	@ echo "#endif" >> ./script.h

	@ rm script_temp.js

	@ echo "done!"
	@ echo ""

clean:
	rm -f *.h 