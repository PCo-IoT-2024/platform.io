# Final Presentation and Conclusion

The final presentation is not only a demonstration that the system worked once. It is the moment where the group explains the complete system, its data path, its limitations, and what should be improved in the next version.

For management students, the conclusion is especially important. A working prototype is not yet an operational product. The final discussion should show technical understanding and project judgment.

---

## What the group should present

Each group should present:

- project goal
- end-to-end architecture
- group organization
- sensor selection
- LoRaWAN/TTN setup
- Raspberry Pi backend
- database storage
- dashboard
- field measurement results
- problems and limitations
- proposed improvements

The presentation should make clear how the value moved through the whole system.

---

## Required end-to-end explanation

The group should be able to explain this chain:

```text
sensor
  -> ESP32 firmware
  -> LoRaWAN uplink
  -> TTN payload formatter
  -> TTN MQTT
  -> mqttbridge
  -> local mqttbroker
  -> mqttcli
  -> MariaDB
  -> SNode.C dashboard
  -> browser
```

Each student should explain the part they owned and how it connects to the other parts.

---

## Evidence to include

Good final evidence includes:

```text
[ ] photo of the buoy or lab setup
[ ] screenshot of generator settings
[ ] serial monitor output
[ ] TTN live data with decoded payload
[ ] local MQTT subscriber output
[ ] MariaDB SELECT query result
[ ] dashboard screenshot
[ ] field-test context log
[ ] calibration table
```

The group should not rely only on one screenshot. The point is to prove the complete path.

---

## Discussing measurement quality

The final presentation should include a measurement-quality discussion.

Questions:

- Which sensors were calibrated?
- Which references were used?
- Which values are absolute measurements?
- Which values are mainly trend indicators?
- What could disturb the readings?
- How stable were the readings over time?
- Was the water measurement plausible?

The correct answer is not always “our sensor value is exact”. A mature answer often says:

```text
This value is useful as a trend, but not a laboratory-grade measurement.
```

---

## Discussing system reliability

The group should also discuss reliability.

Questions:

- Did LoRaWAN coverage work reliably?
- Did GPS get a fix?
- Did the Raspberry Pi backend run continuously?
- Did MariaDB store all expected values?
- Did the dashboard update correctly?
- What happens after power loss?
- What happens when Wi-Fi or internet is unavailable?

---

## Suggested final report structure

```text
1. Introduction and project goal
2. Group organization and responsibilities
3. System architecture
4. Hardware and sensor setup
5. Firmware and generator configuration
6. TTN cloud setup
7. Raspberry Pi backend
8. Database and dashboard
9. Calibration
10. Field measurement
11. Results
12. Limitations
13. Improvements
14. Conclusion
```

Each student should contribute to the section that belongs to their workstream.

---

## Improvement ideas

Possible improvements for a next version:

- better waterproof enclosure
- switched sensor power rails
- lower-power ESP32 board
- larger solar panel or better battery management
- more robust dashboard
- better database schema
- calibration storage in database
- multiple buoys
- map visualization
- CSV export
- alerting when values exceed thresholds
- better sensor references for TDS and turbidity

---

## Final conclusion

The final conclusion should answer:

```text
What did we build?
What did we measure?
How reliable is it?
What did we learn?
What would we improve?
```

A good conclusion connects technical work to system understanding. It does not just list commands that were executed.

---

## Course success criterion

The course project is successful when the group can demonstrate and explain one complete data path from the physical lake water to the browser dashboard, including the limitations of the measurement and the responsibilities of each system layer.
