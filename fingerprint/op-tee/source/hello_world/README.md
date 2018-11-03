
3. Define the toolchains and environment variables:

```
export TEEC_EXPORT=$PWD/../optee_client/out/export
```

If `normal world` user space is 64-bit:<BR>
```
export HOST_CROSS_COMPILE=$PWD/../toolchains/aarch64/bin/aarch64-linux-gnu-
```

If `normal world` user space is 32-bit:<BR>

```
export HOST_CROSS_COMPILE=$PWD/../toolchains/aarch32/bin/arm-linux-gnueabihf-
```

If `secure world` user space is 64-bit:<BR>

```
export TA_CROSS_COMPILE=$PWD/../toolchains/aarch64/bin/aarch64-linux-gnu-
export TA_DEV_KIT_DIR=$PWD/../optee_os/out/arm/export-ta_arm64
```

If `secure world` user space is 32-bit:<BR>

```
export TA_CROSS_COMPILE=$PWD/../toolchains/aarch32/bin/arm-linux-gnueabihf-
export TA_DEV_KIT_DIR=$PWD/../optee_os/out/arm/export-ta_arm32
```

4. Build it!

`make`