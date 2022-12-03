hostname ?= auto
ephy3 ?= ephy3

#ifeq (${C},)
#	${error Must enter yaml file using C=}
#endif

all: build_modbus_s39

#VERSION=$(shell echo `hostname` `git describe --all --long  --dirty`)

#esphome = esphome -s PROJECT_NAME $2 -s PROJECT_VERSION "$(VERSION)" $1 $2
esphome = esphome $1 $2

build_%: %.yaml
	$(call esphome, compile, $<)

config_%: %.yaml
	$(call esphome, config, $<)

upload_%: %.yaml
	$(call esphome, upload, $<)

run_%: %.yaml
	$(call esphome, run, $<)

logs_%: %.yaml
	$(call esphome, logs, $<)

rlogs_%: %.yaml
	./scripts/remote ${hostname} ${ephy3} $< logs

uploader: ${merged}
	scp /home/amirv/.platformio/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin /home/amirv/.platformio/packages/framework-arduinoespressif32/tools/sdk/bin/bootloader_dio_40m.bin $(ephy3):uploader/
	ssh $(ephy3) ~/git/rpi/esphome/upload_uploader

logs: ${merged}
	esphome logs ${merged}

run: ${merged}
	esphome run ${merged}

delay:
	sleep 2

remote_run: build remote_upload delay remote_logs

remote_upload:
	./scripts/remote  ${hostname} ${ephy3} ${merged} upload

remote_serial_upload:
	platformio remote run -d ${hostname} -t upload --upload-port /dev/ttyUSB0

remote_upload_8266:
	./scripts/sonoff_uploader ${merged}

clean:
	@rm -fr $(outputs)
	@for y in $(targets:%.bin=%.yaml); do esphome clean $$y; done

binaries/%.bin: %.yaml sonoff_common.yaml
	esphome $< compile
	@cp -v $*/.pioenvs/$*/firmware.bin $@

