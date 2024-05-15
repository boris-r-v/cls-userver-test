cmake -G Ninja -DUSERVER_FEATURE_GRPC=ON -DUSERVER_FEATURE_REDIS=ON -S . -B cmake-build-debug && cmake --build cmake-build-debug/ --target cls_userver
