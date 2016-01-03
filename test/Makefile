
build_dir  = ./bin
source_dir = .:
header_dir = ../database/ ../ioframe/ ../log/ ../mutex/ ../thread/
libobjects = ../libsdk.a -lpthread

OBJFILES = maintest.o
DEPENDFILE = $(OBJFILES:.o=.d)

CXXFALS += $(addprefix -I, $(header_dir))

#vpath %.h ../ioframe:../thread

#目标变量
test:CXXFALS += -pg
test:$(OBJFILES)
	$(CXX) $(CXXFALS) $(build_dir)/$(OBJFILES) -o $(build_dir)/$@ $(libobjects)

#模式变量
%.o:CXXFALS += -g
%.o:%.cpp
	$(CXX) $(CXXFALS) -c -o $(build_dir)/$@ $<

$(DEPENDFILE):%.d:%.cpp
	@mkdir -p $(build_dir)
	@set -e;$(RM) $(build_dir)/$@;\
	$(CXX) $(CXXFALS) -MM $< > $(build_dir)/$@.;\
	sed 's/\($*\)\.o/\1.o $@/g' $(build_dir)/$@. > $(build_dir)/$@;\
	$(RM) $(build_dir)/$@.;

-include $(DEPENDFILE)
#maintest.o:ioframe.h thread.h

PYHON:clean
clean:
	@$(RM) -rf $(build_dir)/*

