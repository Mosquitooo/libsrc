
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

SDK: $(libsdkobj)
	$(AR) -r libsdk.a $(addprefix $(DESTDIR)/, $(libsdkobj))

%.d:%.cpp
	@mkdir -p $(DESTDIR)
	@$(RM) -f $(DESTDIR)/$@;\
	@$(CXX) -MM $< > $(DESTDIR)/$@.;\
	@sed 's/\($*\)\.o/1.o $@/g' $@. > $(DESTDIR)/$@;\
	@$(RM) -f $(DESTDIR)/$@.;

include $(addprefix $(DESTDIR)/, $(DEPENDFILES))

.PHYON:clean
clean:
	-$(RM) -rf libsdk.a $(addprefix $(DESTDIR)/, $(libsdkobj))
