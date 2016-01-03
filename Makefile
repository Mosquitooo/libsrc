
#env opt
DESTDIR = ./build/obj
SRCDIR = database:ioframe:log:mutex:thread
libsdkobj = ioframe.o dbconnector.o log.o mutex.o thread.o
DEPENDFILES = $(libsdkobj:.o=.d)

#compile opt
CXX = g++
CXXFLAGS = -g

#set search path
vpath %.cpp $(SRCDIR)
vpath %.h $(SRCDIR)

SDK:$(libsdkobj)
	$(AR) -r libsdk.a $(addprefix $(DESTDIR)/, $(libsdkobj))

%.o:%.cpp
	@$(CXX) $(CXXFLAGS) -c -o $(DESTDIR)/$@ $<

$(DEPENDFILES):%.d:%.cpp
	@mkdir -p $(DESTDIR)
	@set -e;rm -rf $(DESTDIR)/$@;\
	$(CXX) $(CXXFLAGS) -MM $< > $(DESTDIR)/$@.;\
	sed 's/\($*\)\.o/\1.o $@/g' $(DESTDIR)/$@. > $(DESTDIR)/$@;\
	$(RM) -f $(DESTDIR)/$@.;

-include $(DEPENDFILES)

.PHYON:clean
clean:
	@-rm -rf libsdk.a $(addprefix $(DESTDIR)/, $(libsdkobj))
	@-rm -rf $(DEPENDFILES)
