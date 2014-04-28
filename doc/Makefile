IMAGES = \
	../plugins/defaulttheme/data/arrow.png \
	../plugins/defaulttheme/data/autokeyframe.png \
	../plugins/defaulttheme/data/camera.png \
	../plugins/defaulttheme/data/channel.png \
	../plugins/defaulttheme/data/crop.png \
	../plugins/defaulttheme/data/cut.png \
	../plugins/defaulttheme/data/expandpatch_checked.png \
	../plugins/defaulttheme/data/ibeam.png \
	../plugins/defaulttheme/data/left_justify.png \
	../plugins/defaulttheme/data/magnify.png \
	../plugins/defaulttheme/data/mask.png \
	../plugins/defaulttheme/data/mutepatch_up.png \
	../plugins/defaulttheme/data/paste.png \
	../plugins/defaulttheme/data/projector.png \
	../plugins/defaulttheme/data/record.png \
	../plugins/defaulttheme/data/recordpatch_up.png \
	../plugins/defaulttheme/data/rewind.png \
	../plugins/defaulttheme/data/singleframe.png \
	../plugins/defaulttheme/data/titlesafe.png \
	../plugins/defaulttheme/data/toolwindow.png \
	../plugins/defaulttheme/data/top_justify.png \
	../plugins/defaulttheme/data/wrench.png \
	../plugins/defaulttheme/data/magnify.png

$(OUTPUT):
	cinelerra.html

all:
	cp $(IMAGES) .
	makeinfo --html --no-split cinelerra.texi

install:
	cp $(IMAGES) /amazon/httpd/html/cinelerra
	cp cinelerra.html /amazon/httpd/html/cinelerra
