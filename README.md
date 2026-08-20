# Umicom OS Module

`umicom-os-module` is the thin **user-space Control Centre** contribution used by
Umicom Desk and selected Umicom OS profiles.

It is not the operating-system distribution repository.

## Ownership boundary

```text
umicom-os
    kernel configuration and patches
    boot and initramfs
    root filesystem and packages
    recovery
    image generation
    security profiles

umicom-framework
    portable user-space contracts and adapters

umicom-os-module
    Control Centre identity, views, commands and default layouts
```

The Linux kernel is not included in Umicom Framework. Recovery must continue to
operate if Framework cannot start.

## Current capability

The first module implementation publishes an inspectable boundary catalogue and
a console Control Centre. Future batches can contribute Framework-backed system
information, package, device, network, storage, service, security and update
views without moving kernel or boot responsibilities into this repository.

## Ownership

Project lead and author: Sammy Hegab
Organisation: Umicom Foundation
Licence: MIT
