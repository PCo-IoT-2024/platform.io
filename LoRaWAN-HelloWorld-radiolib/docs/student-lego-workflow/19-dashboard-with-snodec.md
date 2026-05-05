# Dashboard with SNode.C

This chapter describes the final visible part of the system: a web dashboard served from the Raspberry Pi.

The target path is:

```text
MariaDB -> SNode.C backend -> browser dashboard
```

The dashboard should help students and scientists inspect the buoy data without manually running SQL queries.

---

## What the dashboard should show

A useful first dashboard should show:

- latest value per sensor
- last update time
- device ID
- GPS position if available
- pH history
- TDS history
- turbidity history
- water-temperature history
- PH4502C board-temperature history
- simple status / diagnostics

The first dashboard does not need to be visually perfect. It should make the data path visible and testable.

---

## Frontend, backend, database

For non-technical students, distinguish the three parts clearly.

| Part | Meaning | Example |
|---|---|---|
| frontend | what the browser displays | HTML/CSS/JavaScript dashboard |
| backend | server program answering browser requests | SNode.C HTTP server |
| database | persistent measurement storage | MariaDB |

The browser should not connect directly to MariaDB. The backend reads from MariaDB and serves selected data through HTTP/API endpoints.

---

## Suggested API endpoints

A minimal backend could provide:

| Endpoint | Purpose |
|---|---|
| `/` | dashboard page |
| `/api/devices` | list known devices |
| `/api/latest` | latest measurements |
| `/api/measurements?device=...&field=...` | time series for one field |
| `/api/gps?device=...` | GPS positions |
| `/api/health` | backend/database status |

These endpoint names are suggestions. The important point is that the API should separate the dashboard from direct database access.

---

## Example dashboard query

A backend endpoint for TDS history could run a SQL query like:

```sql
SELECT received_at, numeric_value
FROM measurements
WHERE device_id = 'buoy-01'
  AND field_name = 'tds_ppm'
ORDER BY received_at DESC
LIMIT 100;
```

The backend converts the result to JSON for the frontend.

Conceptual JSON response:

```json
[
  { "received_at": "2026-05-05T12:00:00Z", "tds_ppm": 350.0 },
  { "received_at": "2026-05-05T12:10:00Z", "tds_ppm": 352.0 }
]
```

---

## Running the dashboard server

The exact command depends on the course dashboard implementation.

Conceptual command:

```bash
~/water-buoy/bin/dashboard-server --config ~/water-buoy/config/dashboard.conf
```

or, if the dashboard is part of the MQTTSuite/SNode.C build:

```bash
~/water-buoy/bin/<dashboard-binary> --help
```

The configuration should contain:

- HTTP listen address
- HTTP port
- MariaDB host
- MariaDB database
- MariaDB user
- MariaDB password
- path to static frontend files if needed

Do not commit real database passwords.

---

## Accessing the dashboard

From a laptop in the same network, open:

```text
http://waterbuoy-pi.local:<port>/
```

or:

```text
http://<raspberry-pi-ip>:<port>/
```

The exact port depends on the server configuration.

---

## Minimum viable dashboard

A minimum dashboard is enough if it proves the full pipeline.

Minimum requirements:

```text
[ ] browser can open the dashboard
[ ] dashboard can reach backend
[ ] backend can query MariaDB
[ ] latest measurements are displayed
[ ] at least one historical chart or table is visible
```

After that, students can improve layout, filtering, maps, and charts.

---

## Useful dashboard features

For a water-quality dashboard, useful features include:

- device selector
- time-range selector
- sensor-field selector
- latest values card
- trend charts
- GPS map
- raw messages view for debugging
- calibration notes
- data export as CSV

The dashboard should not hide uncertainty. It should show enough context for interpretation.

Example context fields:

- device ID
- timestamp
- fPort
- sensor type
- calibration date
- water-temperature compensation state

---

## You are done when...

This chapter is complete when:

```text
[ ] The dashboard server starts on the Raspberry Pi.
[ ] The browser can open the dashboard.
[ ] The backend can connect to MariaDB.
[ ] The dashboard shows recent measurements.
[ ] Students can trace one displayed value back to a database row and a TTN uplink.
```
