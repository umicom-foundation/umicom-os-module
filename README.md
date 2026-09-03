# Umicom OS Module

`umicom-os-module` is the thin **user-space Control Centre** contribution used by
Umicom Desk and selected Umicom OS profiles.

It is not the operating-system distribution repository.

The accepted production, portability and research strategy is recorded in
[the Umicom OS architecture decision](docs/ARCHITECTURE_DECISION.md). The same
decision is available to tools through a versioned Framework contract so the
documentation and product surfaces can be checked together.

## Ownership boundary

```text
umicom-os
    production distribution using a maintained Linux LTS kernel
    kernel configuration and patches
    boot and initramfs
    root filesystem and packages
    recovery
    image generation
    security profiles

umicom-framework
    portable user-space contracts and adapters

umicom-kernel
    independent original microkernel research
    never selected silently by a production profile

umicom-os-module
    Control Centre identity, views, commands and default layouts
```

The production Linux kernel is not included in Umicom Framework. FreeBSD is a
portability and server/appliance evaluation target. The original Umicom
microkernel remains a separate research track until it satisfies documented
promotion gates. Recovery must continue to operate if Framework cannot start.

## Current capability

The module publishes an inspectable boundary catalogue, the accepted
architecture decision and a console Control Centre. Future work can contribute
Framework-backed system information, package, device, network, storage,
service, security and update views without moving kernel or boot
responsibilities into this repository.

## Ownership

Project lead and author: Sammy Hegab
Organisation: Umicom Foundation
Licence: MIT
