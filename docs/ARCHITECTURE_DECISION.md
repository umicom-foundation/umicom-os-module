<!--
Umicom OS Module
File: docs/ARCHITECTURE_DECISION.md

PURPOSE:
Record the accepted Umicom OS product, portability and kernel-research
architecture so future development keeps the same dependency boundaries.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
-->

# Umicom OS Architecture Decision

Status: Accepted  
Decision date: 2 September 2026

## Context

Umicom OS needs a dependable route to a usable desktop operating system while
also leaving room for original operating-system research. Umicom Framework is
already the shared application platform, but it is a user-space framework. A
kernel and its recovery environment have different constraints: they must boot,
manage memory and devices, recover from damaged installations, and remain
useful when ordinary application services cannot start.

The design therefore separates the product that people can use now from the
longer-term research that may produce an original kernel later.

## Decision

1. The production Umicom OS distribution uses a maintained Linux long-term
   support kernel.
2. FreeBSD is a portability and server/appliance evaluation target. Its
   adapters help prove that Framework code does not depend on one kernel.
3. An original Umicom microkernel is an independent research track. It is not
   the production default until it meets explicit compatibility, security,
   reliability, performance and recovery gates.
4. Umicom Framework runs in normal user space. The production kernel and the
   recovery environment do not depend on the full Framework.
5. A very small freestanding support library may be shared with low-level code
   in the future, but only when it has no operating-system runtime dependency
   and receives a separate safety and security review.
6. Distribution engineering, kernel research and reusable application services
   remain in separate repositories with one-way dependency boundaries.

This accepted decision is also represented by the versioned
`UmiCtOsArchitectureDecision` Framework contract. The OS Control Centre reads
that contract, which lets command-line and graphical views explain the same
policy without maintaining a second list.

## Repository responsibilities

### `umicom-os`

This repository owns the installable operating-system product:

- kernel configuration and any carefully maintained patches;
- bootloader integration, early boot and initramfs;
- root filesystem, package selection and system defaults;
- recovery image and repair workflow;
- image generation, signing, installation and updates;
- security profiles and release evidence.

### `umicom-kernel`

This separate repository owns original kernel research:

- boot and architecture bring-up;
- memory, process, interrupt and scheduling experiments;
- kernel IPC and capability experiments;
- device-driver experiments and compatibility evidence;
- the test harness and promotion evidence for research milestones.

Research code is never selected silently by a production profile.

### `umicom-framework`

This repository owns portable normal-user-space capabilities:

- application lifecycle, commands, configuration and settings;
- reusable system-service contracts and platform adapters;
- processes, devices, storage, network, packages, updates and diagnostics
  presented through safe service boundaries;
- desktop workbench, panels, layouts, accessibility and automation contracts;
- cross-target descriptions that keep platform-dependent work replaceable.

### `applications/os`

This thin application module owns Umicom OS product identity and Control Centre
presentation. It composes Framework services into system views and commands. It
does not own boot, kernel, recovery or distribution-image implementation.

## Dependency direction

```text
Umicom applications
        |
        v
Umicom Framework user-space services
        |
        v
Platform adapters and system service clients
        |
        v
System calls, IPC and operating-system services
        |
        v
Production kernel
```

Dependencies point down this diagram. The production kernel and recovery image
must not point back to the full Framework. This rule keeps recovery available
when a desktop or Framework service is damaged.

## Delivery tracks

### Production distribution

Deliver a bootable, installable and recoverable system first. Work proceeds
through reproducible images, hardware support, signed updates, security
hardening, rollback, diagnostics and a Framework-backed desktop experience.

### Portability target

Exercise the platform adapter boundary on FreeBSD where it provides useful
evidence. Portability work should improve public contracts and must not scatter
platform checks through application code.

### Kernel research

Develop the original microkernel through small independently testable
milestones. Early milestones cover boot, memory, interrupts, scheduling, IPC
and a minimal user program. Later milestones cover drivers, storage, networking,
security isolation, recovery and Framework service-host compatibility.

## Promotion gates for the research kernel

The research kernel can become a selectable product profile only after it has:

- a reproducible boot and installation path on supported hardware or a clearly
  defined virtual-machine target;
- memory-safety, privilege-boundary and fuzzing evidence;
- reliable storage, network, process and device support;
- a recovery environment that does not depend on the desktop;
- performance and stability measurements against published targets;
- enough user-space compatibility to host the required Framework adapters;
- documented update, rollback, crash-reporting and security-response processes.

Becoming selectable is not the same as becoming the default. A separate
architecture decision is required to change the production foundation.

## Change control

Any proposal that changes a foundation, repository boundary or dependency rule
must update all of the following together:

1. this decision record;
2. the `UmiCtOsArchitectureDecision` contract and validation rules;
3. the Framework contract test;
4. the OS Control Centre projection and its test;
5. the implementation roadmap and release evidence.

This prevents documentation, code and product behaviour from drifting apart.
