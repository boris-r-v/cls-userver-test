
### install deps packages
`sudo apt install python3-venv libyaml-cpp-dev libjemalloc-dev libnghttp2-dev libev-dev libhiredis-dev`

### build
`cmake -G Ninja -DUSERVER_FEATURE_GRPC=ON -DUSERVER_FEATURE_REDIS=ON -S . -B cmake-build-debug && cmake --build cmake-build-debug/ --target cls_userver`

### run
`cls_userver --config ../configs/static_config.yaml`
`curl -d "test" 127.0.0.1:8092/hello`
