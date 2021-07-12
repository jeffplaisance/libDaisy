# libDaisy Changelog

## Unreleased

### Breaking Changes

* move MidiEvent and related data to `daisy` namespace.

### Features

### Bug fixes

* midi: SysEx messages that overflow stop reading data until rx sysexstop. Previously overflowed sysex would cause junk messages.
* midi: NoteOns of velocity 0 cause NoteOffs.

### Other

* test: add unit testing for midi parser.  
* docs: Update TODO comment in `uart.h` to reflect most recent uart update.
* ci: add filters to the workflows

### Migrating

## v0.1.0

Initial Release
