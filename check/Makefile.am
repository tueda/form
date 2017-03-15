TEST_BINS =
if BUILD_FORM
TEST_BINS += form
endif
if BUILD_TFORM
TEST_BINS += tform
endif
if BUILD_PARFORM
TEST_BINS += parform
endif

TEST_OPTS =

TESTS =

if CONFIG_RUBY
TESTS_ENVIRONMENT = \
	PATH="$(top_builddir)/sources:$$PATH" \
	RUBY="$(RUBY)" \
	CHECK_RB="$(srcdir)/check.rb" \
	TEST_BINS="$(TEST_BINS)" \
	TEST_OPTS="$(TEST_OPTS)" \
	$(SHELL)
else
TESTS_ENVIRONMENT = \
	PATH="$(top_builddir)/sources:$$PATH" \
	TEST_BINS="$(TEST_BINS)" \
	$(SHELL)
endif

if CONFIG_RUBY
TESTS += check-help.sh
endif

TESTS += benchmark-fu.sh

EXTRA_DIST = \
	check-help.sh \
	benchmark-fu.sh \
	check.rb \
	examples.frm \
	features.frm \
	fixes.frm \
	aap/fu.frm
