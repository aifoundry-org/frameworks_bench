# Zephyr dev container

Docker setup to test Zephyr inside a container.

## Quickstart

1. Build docker image
```bash
docker build -t zephyr-base:latest -f docker/Dockerfile .
```
2. Run docker container
```bash
./docker/run.sh 
```
3. Install west
```bash
python3 -m venv venv && source venv/bin/activate && pip3 install west
```

4. Init zephyr repository with west *(it'll take some time)*
```bash
west init -m https://github.com/zephyrproject-rtos/zephyr
```

5. Update config and pull all the modules

for liteRT
```bash
west config manifest.project-filter -- +tflite-micro
```

for emlearn
```bash
mkdir -p zephyr/submanifests

cat > zephyr/submanifests/emlearn.yaml <<'EOF'
manifest:
  projects:
    - name: emlearn
      url: https://github.com/emlearn/emlearn.git
      revision: master
      path: modules/lib/emlearn
EOF
```


```bash
west update
```

6. Prepare zephyr
```bash
west zephyr-export
west packages pip --install
```
7. Install zephyr sdk
```bash
west sdk install
```

8. Build project for quemu

liteRT
```bash
west build -b qemu_riscv64 liteRT-micro
```

emlearn
```bash
west build -b qemu_riscv64 emlearn
```

9. Launch project
```bash
west build -t run
```
