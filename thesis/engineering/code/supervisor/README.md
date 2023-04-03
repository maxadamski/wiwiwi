# Supervisor

Create container
```
lxc-create -n <id> -t <template> -- --rootfs=containers/ --sharedir=share/
```

Execute build script
```
lxc-execute -n <id> -- sh -c "export PATH && cd root && chmod +x build && ./build"
```

Execute run script
```
lxc-execute -n <id> -- sh -c "export PATH && cd root && chmod +x run && ./run"
```

