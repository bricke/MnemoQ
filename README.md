# MnemoQ

**MnemoQ** is an MQTT client designed for message persistence and easy state retrieval. 
Its standout feature is the `reload()` API, which lets you retrieve the latest known value for any topic at any time.
It's designed to be simple and portable.

## Features

- Subscribe to any MQTT topic
- Persist the most recent message for each topic
- `reload()` API: retrieve the last known value for one or multiple topics
