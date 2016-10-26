# Makefile for butia
#
# This program is free to be used or modified
# at your ouwn risk
#
# writen by Guillermo Reisch (greisch@fing.edu.uy) and andrew (aaguirre@fing.edu.uy)
# in others words => writen by butia TEAM!


TURTLEARTVERSION=v160

CWD=$(cwd)


all: help


help:
	@echo 
	@echo " This is a Makefile for compile/construct TurtleArtButia.        "
	@echo " The follow objetives are defined:                               "
	@echo "    first: Execute this command to init the submodules only the  "
	@echo "          first time after clone                                 "
	@echo "    xo : Generate the XO distribuible package                    "
	@echo "    fakeinstall: compile all necessary for TurtleArtButia        "
	@echo "                 and make a fake install using a symlink         "
	@echo "                 this allow to work and easy change the code     "
	@echo "    install: install activity                                    "
	@echo "    uninstall: unistall activity                                 "
	@echo "    clean: clean all compiled/generated files                    "
	@echo "    genpot: generate pot of plugings                             "
	@echo "    genpot_notraslated: generate pot the plugings that are       "
	@echo "                        not already traslated in taextras.py     "
	@echo "    updatepo: using notraslated.pot update the es.po of plugin's "
	@echo "              the other traslations all already set in mainline  "
	@echo "              of Walter, so use that traslations.                "
	@echo "                                                                 "
	@echo " The follow parameters are optional:                             "
	@echo "    TURTLEARTVERSION=[version or commit] (default $(TURTLEARTVERSION))"
	@echo "                                                                 "
	@echo "           For more info contact \"butia@fing.edu.uy\"           "
	@echo "   wiki: http://www.fing.edu.uy/inco/proyectos/butia/mediawiki   "
	@echo "                                                                 "    



############# Obtain the mainline of TurtleArt (Walter) at a specific version

first:
	# init and update the submodules (only works if you are in the root of git)
	(cd .. && git submodule init)
	# This don't work if local changes, commit local change before
	(cd .. && git submodule update)

walterGIT:
	# walter GIT
	# make sure we have the last changes of the git
	(cd mainline && git fetch)
	# erase any untraced file
	(cd mainline && git clean -xdf)
	# goto origin/master
	#(cd mainline && git checkout origin/master)
	# a especific version of Turtle
	(cd mainline && git checkout bfba0c8a8b0550e4ed328aff3632043ef5f3619a)

wedoGIT:
	# wedo GIT
	(cd WeDoMore && git fetch)
	(cd WeDoMore && git clean -xdf)
	(cd WeDoMore && git checkout ad0baf7fff7db9488d2995845f4e351048f02c91)

nxtGIT:
	# NXT GIT
	(cd nxt_plugin && git fetch)
	(cd nxt_plugin && git clean -xdf)
	(cd nxt_plugin && git checkout a58f49cdf36ef0f6ee8ad70315824b3e5d0a5935)


########### POT GENERATION AND TRASLATION THINKS ###########

POT_FILES= butia/po/Butia.pot sumtia/po/Sumtia.pot followme/po/FollowMe.pot 
POT_PROP= --language=Python --add-comments=TRANS: --keyword=_ --join-existing

POT_NT_FILES= butia/po/Butia.notraslated.pot sumtia/po/Sumtia.notraslated.pot \
              followme/po/FollowMe.notraslated.pot

PO_FILES= butia/po/es.po sumtia/po/es.po followme/po/es.po


## Generate pot of plugins
genpot: $(POT_FILES)

butia/po/Butia.pot: butia/butia.py
	echo msgid \"\" > $@
	echo msgstr \"\" >> $@
	echo \"Content-Type: text/plain\; charset=UTF-8\" >> $@
	xgettext $(POT_PROP) $< -o $@

sumtia/po/Sumtia.pot: sumtia/sumtia.py
	echo msgid \"\" > $@
	echo msgstr \"\" >> $@
	echo \"Content-Type: text/plain\; charset=UTF-8\" >> $@
	xgettext $(POT_PROP) $< -o $@

followme/po/FollowMe.pot: followme/followme.py
	echo msgid \"\" > $@
	echo msgstr \"\" >> $@
	echo \"Content-Type: text/plain\; charset=UTF-8\" >> $@
	xgettext $(POT_PROP) $< -o $@


## Generate a pot of MSGID that ARE NOT ALREADY translated in mainline (Walter GIT)
genpot_notraslated: $(POT_NT_FILES)

butia/po/Butia.notraslated.pot: butia/po/Butia.pot 
	msgmerge --no-fuzzy-matching mainline/po/es.po $< | msgattrib --no-obsolete --untranslated > $@

sumtia/po/Sumtia.notraslated.pot: sumtia/po/Sumtia.pot
	msgmerge --no-fuzzy-matching mainline/po/es.po $< | msgattrib --no-obsolete --untranslated > $@

followme/po/FollowMe.notraslated.pot: followme/po/FollowMe.pot
	msgmerge --no-fuzzy-matching mainline/po/es.po $< | msgattrib --no-obsolete --untranslated > $@


## update de es.po files of plugins using the pot of not already translated
updatepo: $(PO_FILES)

butia/po/es.po: butia/po/Butia.notraslated.pot
	msgmerge --update --no-fuzzy-matching  $@ $<

sumtia/po/es.po: sumtia/po/Sumtia.notraslated.pot
	msgmerge --update --no-fuzzy-matching  $@ $<

followme/po/es.po: followme/po/FollowMe.notraslated.pot
	msgmerge --update --no-fuzzy-matching  $@ $<


################ Support for lua (bobot-server) #############

butiaXO:
	# I want butiaXO folder be OK! (needed by plugin)
	make -C ../butiaXO




################ Construction...

mktmpfolder:
	#making folders and copying mainline
	rm -rf tmp
	cp -rL mainline tmp
	rm -rf tmp/.git
	rm -f tmp/.gitignore
	# lets patch the activity.info and add some icons (TurtleArt => TurtleArtButia)
	cp -rLf stagin/activity tmp/

apply_localization: mktmpfolder $(PO_FILES)
	# let's add localization (if is not already in taextras.py)
	msgcat butia/po/es.po tmp/po/es.po -o tmp/po/es.po
	msgcat sumtia/po/es.po tmp/po/es.po -o tmp/po/es.po
	msgcat followme/po/es.po tmp/po/es.po -o tmp/po/es.po

apply_hard_plugins: apply_localization butiaXO
	cp -rLf butia tmp/plugins/
	cp -rLf sumtia tmp/plugins/
	cp -rLf followme/ tmp/plugins/
	cp -rLf nxt_plugin/ tmp/plugins/
	cp -rLf wedo_plugin/ tmp/plugins/

apply_sym_plugins: walterGIT nxtGIT wedoGIT apply_localization butiaXO
	# let's make symlinks, it lets you, edit and try, more easily!
	ln -s `pwd`/butia tmp/plugins/butia
	ln -s `pwd`/sumtia tmp/plugins/sumtia
	ln -s `pwd`/followme tmp/plugins/followme
	ln -s `pwd`/nxt_plugin tmp/plugins/nxt_plugin
	ln -s `pwd`/wedo_plugin tmp/plugins/wedo_plugin

xo: apply_hard_plugins
	# remove .git from NXT plugin
	rm -rf tmp/plugins/nxt_plugin/.git
	rm -f tmp/plugins/nxt_plugin/.gitignore
	# ok, let's make this .xo
	(cd tmp && python setup.py build)
	(cd tmp && python setup.py dist_xo)
	cp tmp/dist/*.xo .
	#$(MAKE) clean_tmp


################ Plugins #############

butia_plugin: apply_hard_plugins
	cp stagin/plugins/butia/plugin.info tmp/plugins
	(cd tmp/plugins && tar -czvf butia_plugin.tar.gz butia/* plugin.info)
	rm tmp/plugins/plugin.info
	mv tmp/plugins/butia_plugin.tar.gz .

followme_plugin: apply_hard_plugins
	cp stagin/plugins/followme/plugin.info tmp/plugins
	(cd tmp/plugins && tar -czvf followme_plugin.tar.gz followme/* plugin.info)
	rm tmp/plugins/plugin.info
	mv tmp/plugins/followme_plugin.tar.gz .

nxt_plugin: apply_hard_plugins
	# remove .git from NXT plugin
	rm -rf tmp/plugins/nxt_plugin/.git
	rm -f tmp/plugins/nxt_plugin/.gitignore
	cp stagin/plugins/nxt_plugin/plugin.info tmp/plugins
	cd tmp/plugins && tar -czvf nxt_plugin.tar.gz nxt_plugin/* plugin.info
	rm tmp/plugins/plugin.info
	mv tmp/plugins/nxt_plugin.tar.gz .

wedo_plugin: apply_hard_plugins
	cp stagin/plugins/wedo_plugin/plugin.info tmp/plugins
	cd tmp/plugins && tar -czvf wedo_plugin.tar.gz wedo_plugin/* plugin.info
	rm tmp/plugins/plugin.info
	mv tmp/plugins/wedo_plugin.tar.gz .

plugins: butia_plugin followme_plugin nxt_plugin wedo_plugin

install: xo
	sugar-install-bundle *.xo
	$(MAKE) clean_xo


fakeinstall: apply_sym_plugins
	rm -rf ~/Activities/TurtleBots.activity
	mkdir -p ~/Activities
	# fix localization and index
	(cd tmp && python setup.py build)
	(cd tmp && python setup.py dev)



####### CLEAN STUFF ######


clean:
	$(MAKE) -C ../butiaXO clean
	$(MAKE) clean_pot
	$(MAKE) clean_tmp
	$(MAKE) clean_plugins
	$(MAKE) clean_xo
	# this remove only if it's a symlink ; don't remove if installed
	rm -f ~/Activities/TurtleBots.activity

clean_tmp:
	rm -rf tmp

clean_pot:
	rm -f $(POT_FILES) $(POT_NT_FILES)

clean_xo:
	rm -f *.xo

clean_plugins:
	rm -f *.tar.gz

uninstall:
	rm -rf ~/Activities/TurtleBots.activity
	



# targets that not represent generated files
.PHONY: all clean clean_tmp clean_pot clean_xo clean_plugins walterGIT wedoGIT nxtGIT butiaXO mktmpfolder apply_hard_plugins apply_sym_plugins genpot genpot_notraslated updatepo xo install uninstall help butia_plugin followme_plugin nxt_plugin wedo_plugin plugins
