include $(top_srcdir)/tests/Makefile.plugin

noinst_DATA = \
	extension-python.gschema.xml	\
	extension-python.plugin		\
	extension-python.py		\
	extension-python3.gschema.xml	\
	extension-python3.plugin	\
	extension-python3.py

extension-py.%:
	test -e $@

extension-python.gschema.xml extension-python3.gschema.xml: extension-py.gschema.xml
	$(AM_V_GEN) cp $< $@ && \
	$(SED) -i -e 's%PY_LOADER%$(@:extension-%.gschema.xml=%)%g' $@

extension-python.py extension-python3.py: extension-py.py
	$(AM_V_GEN) $(LN_S) $< $@

extension-python.plugin extension-python3.plugin: extension-py.plugin
	$(AM_V_GEN) cp $< $@ && \
	$(SED) -i -e 's%PY_LOADER%$(@:extension-%.plugin=%)%g' $@

EXTRA_DIST = \
	extension-py.gschema.xml	\
	extension-py.plugin		\
	extension-py.py

CLEANFILES = $(noinst_DATA)
DISTCLEANFILES = $(noinst_DATA)
