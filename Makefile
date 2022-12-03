targets += sonoff_th16.bin
#targets += sonoff_basic.bin
targets += sonoff_pow.bin

outputs = $(targets:%=binaries/%)
merged=${C:%.yaml=%.merged.yaml}
hostname ?= auto

ephy3 ?= ephy3

ifeq (${C},)
	${error Must enter yaml file using C=}
endif

all: $(outputs)

${C}.d: ${C}
	./scripts/yaml-merge.py $^ --deps > $@

-include ${C}.d

%.merged.yaml: ${C}
	./scripts/yaml-merge.py $^ > ${merged}

#modbus_s%.merged.yaml: ${C} include/modbus_common.yaml include/common.yaml
#	scripts/mkversion
#	./scripts/yaml-merge.py $^ include/version.yaml > ${merged}

#modbus_m%.merged.yaml:  ${C} include/modbus_motor_common.yaml  include/modbus_common.yaml include/common.yaml
#	scripts/mkversion
#	./scripts/yaml-merge.py $^ include/version.yaml > ${merged}

build: ${merged}
	esphome compile  ${merged}

config: ${merged}
	esphome config  ${merged}

remote_logs:
	./scripts/remote  ${hostname} ${ephy3} ${merged} logs

upload: ${merged}
	esphome -v upload ${merged}

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

