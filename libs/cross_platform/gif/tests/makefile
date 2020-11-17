# Regression-test suite for the giflib library and tools
# All utilities have tests except gifbg, gifcolor, and gifhisto.

.SUFFIXES: .gif .rgb

# This is what to do by default
test: render-regress \
	gifbuild-regress \
	gifclrmp-regress \
	gifecho-regress \
	giffilter-regress \
	giffix-regress \
	gifsponge-regress \
	giftext-regress \
	giftool-regress \
	gifwedge-regress
	@echo "No output is good news"

rebuild: render-rebuild \
		gif2rgb-rebuild \
		gifclrmp-rebuild \
		gifecho-rebuild \
		giffix-rebuild \
		giftext-rebuild \
		gifwedge-rebuild

UTILS = ..
PICS = ../pic

GIFS := $(shell ls ../pic/*.gif)

# Test decompression and rendering by unpacking images,
# converting them to RGB, and comparing that result to a check file.
render-regress:
	@for test in $(GIFS); \
	do \
	    stem=`basename $${test} | sed -e "s/.gif$$//"`; \
	    if echo "Testing RGB rendering of $${test}" >&2; \
	    $(UTILS)/gif2rgb -1 -o $@.$${stem}.regress $${test} 2>&1; \
	    then cmp $${stem}.rgb $@.$${stem}.regress; \
	    else echo "*** Nonzero return status on $${test}!"; exit 1; fi; \
	done
	@rm -f $@.*.regress
render-rebuild:
	@for test in $(GIFS); do \
		stem=`basename $${test} | sed -e "s/.gif$$//"`; \
		echo "Remaking $${stem}.rgb"; \
		$(UTILS)/gif2rgb -1 -o $${stem}.rgb $${test}; \
	done

gif2rgb-rebuild:
	@echo "Rebuilding gif2rgb checkfile."
	@$(UTILS)/gif2rgb -c 3 -s 100 100 <gifgrid.rgb | $(UTILS)/gifbuild -d >gifgrid.ico
gif2rgb-regress:
	@echo "gif2rgb: Checking idempotency"
	@$(UTILS)/gif2rgb -c 3 -s 100 100 <gifgrid.rgb | $(UTILS)/gifbuild -d | diff -u gifgrid.ico -

gifbuild-regress:
	@echo "gifbuild: basic sanity check"
	@$(UTILS)/gifbuild -d <$(PICS)/treescap.gif | diff -u treescap.ico -
	@echo "gifbuild: Checking idempotency on an icon file."
	@$(UTILS)/gifbuild <$(PICS)/sample.ico | $(UTILS)/gifbuild -d > $@.sample-1.ico; $(UTILS)/gifbuild < $@.sample-1.ico | $(UTILS)/gifbuild -d > $@.sample-2.ico; diff -u $@.sample-1.ico $@.sample-2.ico; rm $@.sample-?.ico
	@echo "gifbuild: Checking idempotency on an animation."
	@$(UTILS)/gifbuild -d <$(PICS)/fire.gif > $@.fire1.ico
	@$(UTILS)/gifbuild < $@.fire1.ico > $@.fire2.gif
	@$(UTILS)/gifbuild -d < $@.fire2.gif > $@.fire2.ico
	@diff -u  $@.fire1.ico  $@.fire2.ico
	@rm -f $@.fire1.ico  $@.fire2.ico $@.fire2.gif

gifclrmp-regress:
	@for test in $(GIFS); \
	do \
	    stem=`basename $${test} | sed -e "s/.gif$$//"`; \
	    if echo "gifclrmap: Checking colormap of $${test}" >&2; \
	    $(UTILS)/gifclrmp <$${test} > $@.$${stem}.regress 2>&1; \
	    then diff -u $${stem}.map $@.$${stem}.regress; \
	    else echo "*** Nonzero return status on $${test}!"; exit 1; fi; \
	done
	@rm -f $@.*.regress
gifclrmp-rebuild:
	@for test in $(GIFS); do \
		stem=`basename $${test} | sed -e "s/.gif$$//"`; \
		echo "Remaking $${stem}.map"; \
		$(UTILS)/gifclrmp <$${test} >$${stem}.map; \
	done

gifecho-rebuild:
	@echo "Rebuilding gifecho test."
	@$(UTILS)/gifecho -t "foobar" | $(UTILS)/gifbuild -d >foobar.ico
gifecho-regress:
	@echo "gifecho: Testing gifecho behavior"
	@$(UTILS)/gifecho -t "foobar" | $(UTILS)/gifbuild -d | diff -u foobar.ico -

giffilter-regress:
	@for test in $(GIFS); \
	do \
	    stem=`basename $${test} | sed -e "s/.gif$$//"`; \
	    if echo "giffiltr: Testing copy of $${test}" >&2; \
	    $(UTILS)/giffilter <$${test} | $(UTILS)/gif2rgb > $@.$${stem}.regress 2>&1; \
	    then cmp $${stem}.rgb $@.$${stem}.regress; \
	    else echo "*** Nonzero return status on $${test}!"; exit 1; fi; \
	done
	@rm -f  $@.*.regress

giffix-rebuild:
	@echo "Rebuilding giffix test."
	@head --bytes=-20 <$(PICS)/treescap.gif | $(UTILS)/giffix 2>/dev/null | $(UTILS)/gifbuild -d >giffixed.ico
giffix-regress:
	@echo "giffix: Testing giffix behavior"
	@head --bytes=-20 <$(PICS)/treescap.gif | $(UTILS)/giffix 2>/dev/null | $(UTILS)/gifbuild -d | diff -u giffixed.ico -

gifinto-regress:
	@echo "gifinto: Checking behavior on short files."
	@rm -f $@.giflib.tmp
	@$(UTILS)/gifinto <$(PICS)/porsche.gif $@.giflib.tmp
	@if test ! -f $@.giflib.tmp; then echo "gifinto failed to create a file when it should have."; fi
	@rm -f $@.giflib.tmp
	@echo "0123456789" | $(UTILS)/gifinto $@.giflib.tmp 2>/dev/null
	@if test -f $@.giflib.tmp; then echo "gifinto created a file when it shouldn't have."; fi
	@rm -f $@.giflib.tmp

gifsponge-regress:
	@for test in $(GIFS); \
	do \
	    stem=`basename $${test} | sed -e "s/.gif$$//"`; \
	    if echo "gifsponge: Testing copy of $${test}" >&2; \
	    $(UTILS)/gifsponge <$${test} | $(UTILS)/gif2rgb > $@.$${stem}.regress 2>&1; \
	    then cmp $${stem}.rgb  $@.$${stem}.regress; \
	    else echo "*** Nonzero return status on $${test}!"; exit 1; fi; \
	done
	@rm -f  $@.*.regress

giftext-regress:
	@for test in $(GIFS); \
	do \
	    stem=`basename $${test} | sed -e "s/.gif$$//"`; \
	    if echo "giftext: Checking text dump of $${test}" >&2; \
	    $(UTILS)/giftext <$${test} > $@.$${stem}.regress 2>&1; \
	    then diff -u $${stem}.dmp  $@.$${stem}.regress; \
	    else echo "*** Nonzero return status on $${test}!"; exit 1; fi; \
	done
	@rm -f  $@.*.regress
giftext-rebuild:
	@for test in $(GIFS); do \
		stem=`basename $${test} | sed -e "s/.gif$$//"`; \
		echo "Remaking $${stem}.dmp"; \
		$(UTILS)/giftext <$${test} >$${stem}.dmp; \
	done

giftool-regress:
	@echo "giftool: Checking that expensive copy via giftool is faithful."
	@$(UTILS)/giftool <$(PICS)/gifgrid.gif | $(UTILS)/gif2rgb | cmp - gifgrid.rgb
	@echo "giftool: Checking that it de-interlaces correctly."
	@$(UTILS)/giftool -i on <$(PICS)/treescap-interlaced.gif | $(UTILS)/gif2rgb | cmp - treescap.rgb
	@echo "giftool: Checking that it interlaces correctly."
	@$(UTILS)/giftool -i off <$(PICS)/treescap.gif | $(UTILS)/gif2rgb | cmp - treescap-interlaced.rgb

gifwedge-rebuild:
	@echo "Remaking the gifwedge test."
	@$(UTILS)/gifwedge >wedge.gif
gifwedge-regress:
	@echo "gifwedge: Checking wedge generation."
	@$(UTILS)/gifwedge | cmp - wedge.gif
