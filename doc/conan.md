
## search for precompiled packages

https://docs.conan.io/en/latest/reference/commands/consumer/search.html
```
conan search harfbuzz/2.6.4@bincrafters/stable -r bincrafters -q "os=Windows AND compiler.version=16 AND compiler.runtime=MD"
conan search google-webrtc/m79@
```

## more information about proccesses

https://docs.conan.io/en/latest/reference/conanfile/other.html#running-commands
`CONAN_PRINT_RUN_COMMANDS`

## creating precompiled dependencies

how to create precompiled dependencies for the ci build image
```fish
docker run --rm -it -v (pwd):/source:ro registry.gitlab.com/acof/fubble/ci/build_ubuntu_1804:v1
cd source
./scripts/ci/build_release
conan remove -f google-webrtc/84
conan user -p <token> -r fubble_dependencies freckled
conan upload "*" --all -c
```


