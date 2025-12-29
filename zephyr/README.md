# Zephyr dev container

Docker setup to test Zephyr inside a container.

## Quickstart - Kenning

1. Build docker image
```bash
docker build -t zephyr-base:22.04 -f docker/Dockerfile .
```
2. Run docker container
```bash
./docker/run.sh 
```

3. Install west
```bash
python3 -m venv venv && source venv/bin/activate && pip3 install west
```
3. Init bench workspace
```bash
west init -l bench && west update
```

4. Export Zephyr Cmake package
```bash
west zephyr-export
```
5. Prepare env
```bash
pip3 install -r kenning-zephyr-runtime/requirements.txt -r zephyr/scripts/requirements-base.txt -r zephyr/scripts/requirements-build-test.txt -r zephyr/scripts/requirements-run-test.txt
```
```bash
pushd kenning-zephyr-runtime && ./scripts/prepare_modules.sh && popd
```

6. Install zephyr sdk
```bash
west sdk install
```

7. Build bench app
```bash
west build -p always -b qemu_riscv64 bench/ -- -DEXTRA_CONF_FILE=config/tflite.conf
```

8. Run qemu emulation
```bash
west build -t run
```

[!NOTE]
If the model is substantial and not-optimized the kenning runtime will most likely fail.
