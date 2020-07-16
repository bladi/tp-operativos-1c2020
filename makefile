sin_color=\x1b[0m
verde=\x1b[32;01m
amarillo=\x1b[33;01m
negrita := $(shell tput bold)
sin_negrita := $(shell tput sgr0)

all:
	mkdir -p logs
	gcc -g -w GameCard/gameCard.c GameCard/funcionesGameCard.c libreriasCompartidas/sockets.c libreriasCompartidas/serializacion.c -o gamecard -lcommons -lpthread
	@printf '$(negrita)$(amarillo)Game Card$(sin_color) ..... $(verde)ok!$(sin_color)$(sin_negrita)\n'

	gcc -g -w GameBoy/gameBoy.c GameBoy/funcionesGameBoy.c libreriasCompartidas/sockets.c libreriasCompartidas/serializacion.c -o gameboy -lcommons -lpthread
	@printf '$(negrita)$(amarillo)Game Boy$(sin_color) ..... $(verde)ok!$(sin_color)$(sin_negrita)\n'

	gcc -g -w Broker/broker.c Broker/funcionesBroker.c libreriasCompartidas/sockets.c libreriasCompartidas/serializacion.c -lm -o broker -lcommons -lpthread
	@printf '$(negrita)$(amarillo)Broker$(sin_color) ..... $(verde)ok!$(sin_color)$(sin_negrita)\n'

	gcc -g -w Team/team.c Team/funcionesTeam.c libreriasCompartidas/sockets.c libreriasCompartidas/serializacion.c -o team -lcommons -lpthread
	@printf '$(negrita)$(amarillo)Team$(sin_color) ..... $(verde)ok!$(sin_color)$(sin_negrita)\n'


# Clean
clean:
	rm -f gamecard gameboy broker team *.o
	rm GameCard/TALL_GRASS/Blocks/*.bin GameCard/TALL_GRASS/Metadata/Bitmap.bin
	rm logs/gameCardLog.log logs/gameBoyLog.log logs/brokerLog.log logs/teamLog.log
	rm Broker/DumpCache.bin
