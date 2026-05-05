# Dashboard and Web View

**Primary workstream:** Student 5 — dashboard, documentation, and integration test

**Interfaces:** Student 4 provides the running Raspberry Pi backend and MariaDB table; Students 1–3 provide the fPort and decoded-field contract.

This chapter defines the intended browser view and the verification criteria. It does not invent a separate dashboard command line. MQTTSuite provides web-facing components through its SNode.C-based applications; the exact command must come from the built MQTTSuite tool or the course-provided wrapper.

For the course, the browser-facing endpoint is:

```text
http://groupN.local:8080/
```

where `groupN` is `group1`, `group2`, `group3`, or `group4`.

## What the dashboard should show

A useful first dashboard should show:

- latest value per sensor
- last update time
- device ID
- fPort
- pH history
- TDS history
- turbidity history
- water-temperature history
- PH4502C board-temperature history
- simple status information

The first dashboard does not need to be visually perfect. It should prove the full data path.

## Dashboard input data

The dashboard reads from the MariaDB `measurements` table:

```text
id
received_at
application_id
device_id
f_port
value
```

The dashboard must interpret the numeric `value` column through the `f_port` column.

| fPort | Dashboard label |
|---:|---|
| 2 | water temperature °C |
| 3 | pH |
| 4 | TDS ppm |
| 5 | turbidity NTU |
| 6 | PH4502C board temperature °C |

## Start the web view

Use the course-provided MQTTSuite/SNode.C dashboard command or wrapper. The endpoint must listen on port 8080.

Placeholder to be replaced by the verified course wrapper:

```bash
~/water-buoy/bin/<course-dashboard-command> --port 8080
```

Do not replace this with guessed `mqttcli dashboard` syntax unless the built tool actually prints that subcommand in `--help`.

## Open the dashboard

From a laptop in the same network:

```text
http://group1.local:8080/
http://group2.local:8080/
http://group3.local:8080/
http://group4.local:8080/
```

If `.local` does not resolve, use the Pi IP address:

```text
http://<raspberry-pi-ip>:8080/
```

## Example queries behind the dashboard

Latest values:

```sql
SELECT device_id, f_port, value, received_at
FROM measurements
ORDER BY received_at DESC
LIMIT 20;
```

TDS history:

```sql
SELECT received_at, value
FROM measurements
WHERE device_id = 'buoy-01'
  AND f_port = 4
ORDER BY received_at DESC
LIMIT 100;
```

## Minimum viable dashboard

A minimum dashboard is enough if it proves the full pipeline.

```text
[ ] browser can open http://groupN.local:8080/
[ ] dashboard process can connect to MariaDB
[ ] latest measurements are displayed
[ ] fPort is visible or correctly translated into sensor labels
[ ] at least one historical table or chart is visible
```

## Evidence for the final report

Student 5 should collect:

```text
[ ] screenshot of the dashboard root page
[ ] screenshot showing at least one real sensor value
[ ] matching MariaDB SELECT result
[ ] matching TTN live-data entry
[ ] short explanation of fPort-to-label mapping
```

## Done when

```text
[ ] A verified course dashboard/web command starts a server on port 8080.
[ ] Browser opens http://groupN.local:8080/.
[ ] Dashboard shows values from MariaDB.
[ ] A displayed value can be traced back to fPort, MariaDB row, local MQTT message, TTN uplink, and ESP32 serial output.
```
