.SUFFIXES: .xml .html .txt .adoc .1

.xml.html:
	xmlto xhtml-nochunks $<

.xml.1:
	xmlto man $<

.xml.txt:
	xmlto txt $<

.adoc.html:
	asciidoc $<

all: allhtml manpages

# The distinction between XMLMAN and XMLINTERNAL is because
# some pages shouldn't be installed as part of a binary package;
# they're just for test-pattern generators.
XMLMAN = \
	gif2rgb.xml \
	gifbuild.xml \
	gifclrmp.xml \
	gifecho.xml \
	giffix.xml \
	gifinto.xml \
	giflib.xml \
	giftext.xml \
	giftool.xml
XMLINTERNAL = \
	gifbg.xml \
	gifcolor.xml \
	gifhisto.xml \
	gifwedge.xml
XMLDOC = intro.xml gif_lib.xml
XMLALL = $(XMLMAN) $(XMLINTERNAL) $(XMLDOC)

# Philosophical choice: the website gets the internal manual pages
allhtml: $(XMLALL:.xml=.html) $(XMLINTERNAL:.xml=.html)

manpages: $(XMLMAN:.xml=.1) $(XMLINTERNAL:.xml=.1)

# Prepare the website directory to deliver an update.
# ImageMagick and asciidoc are required.
website: allhtml
	rm -fr staging; mkdir staging; 
	cp -r $(XMLALL:.xml=.html) gifstandard whatsinagif staging
	cp index.html.in staging/index.html
	convert ../pic/gifgrid.gif -resize 50x50 staging/giflib-logo.gif
	asciidoc - <../history.adoc >staging/history.html
