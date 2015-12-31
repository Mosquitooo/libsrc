
#env opt
DESTDIR = ./build/obj
SRCDIR = database:ioframe:log:mutex:thread
libsdkobj = ioframe.o dbconnector.o log.o mutex.o thread.o
DEPENDFILES = $(addprefix $(DESTDIR)/, $(libsdkobj:.o=.d))

#compile opt
CXX = g++
CXXFLAGS = -g 

#set search path
vpath %.cpp $(SRCDIR)
vpath %.h $(SRCDIR)

SDK: $(libsdkobj)
	@mkdir -p $(DESTDIR)
	$(AR) -r libsdk.a $(addprefix $(DESTDIR)/, $(libsdkobj))

%.o:%.cpp
	@mkdir -p $(DESTDIR)
	@$(CXX) $(CXXFLAGS) -c -o $(DESTDIR)/$@ $<

%.d:%.cpp
	@mkdir -p $(DESTDIR);\
	$(RM) -f $(DESTDIR)/$@;\
	$(CXX) -MM $< > $(DESTDIR)/$@.;\
	sed 's/\($*\)\.o/1.o $@/g' $(DESTDIR)/$@. > $(DESTDIR)/$@;\
	$(RM) -f $(DESTDIR)/$@.;

include $(libsdkobj:.o=.d)

.PHYON:clean
clean:
	@-$(RM) -rf libsdk.a $(addprefix $(DESTDIR)/, $(libsdkobj))
	@-$(RM) -rf $(DEPENDFILES)