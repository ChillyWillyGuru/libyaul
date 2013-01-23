include ../common.mk

PROJECT:= TicTacToe
OBJECTS+= main.o
DEPENDS:= $(OBJECTS:.o=.d)

.PHONY: ip clean

.SUFFIXES: .cpp .c .S .o .bin

example: $(PROJECT).bin

$(PROJECT).bin: $(PROJECT).elf
	$(OB) -O binary $< $@

$(PROJECT).elf: $(OBJECTS)
	$(LD) $(OBJECTS) $(LXXFLAGS) -o $@
	$(NM) $(PROJECT).elf > $(PROJECT).sym
	$(OD) -S $(PROJECT).elf > $(PROJECT).asm

.cpp.o:
	$(CXX) $(CXXFLAGS) -Wp,-MMD,$*.d -c -o $@ $<
.c.o:
	$(CC) $(CFLAGS) -Wp,-MMD,$*.d -c -o $@ $<
.S.o:
	$(AS) $(AFLAGS) -o $@ $<

clean:
	-rm -f $(OBJECTS) ip.o \
$(DEPENDS) \
$(PROJECT).asm \
$(PROJECT).bin \
$(PROJECT).elf \
$(PROJECT).map \
ip.bin.map \
$(PROJECT).sym

-include $(DEPENDS)
