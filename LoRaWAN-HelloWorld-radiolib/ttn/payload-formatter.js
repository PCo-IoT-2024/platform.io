// TTN / The Things Stack uplink payload formatter for the course/full branch.
//
// Payloads are sent as ASCII CSV strings.
//
// fPort mapping:
//   1   GPS:         latitude,longitude,altitude,hdop
//   2   Temperature: temperatureC
//   3   pH:          ph
//   4   TDS:         tds_ppm,temperatureC
//   5   Turbidity:   ntu
//   220 Downlink request marker
//   221 Info / diagnostics text
//   222 Warning text
//   223 Error text
//
// TTN expects warnings/errors as arrays of strings. If objects are returned,
// they are rendered as "[object Object]" in decoded_payload_warnings/errors.

function bytesToString(bytes) {
  return String.fromCharCode.apply(null, bytes);
}

function toNumber(value) {
  const number = Number(value);
  return Number.isFinite(number) ? number : null;
}

function parseCsvNumbers(decodedString) {
  if (!decodedString || decodedString.length === 0) {
    return [];
  }

  return decodedString.split(",").map(toNumber);
}

function numberOrDefault(value, fallback) {
  return value === null || value === undefined ? fallback : value;
}

function makeInfoResult(input, decodedString) {
  return {
    data: {
      f_port: input.fPort,
      payload_raw: decodedString,
      message: decodedString,
      level: "info"
    }
  };
}

function makeWarningResult(input, decodedString) {
  return {
    data: {
      f_port: input.fPort,
      payload_raw: decodedString,
      message: decodedString,
      level: "warning"
    },
    warnings: ["Warning: " + decodedString]
  };
}

function makeErrorResult(input, decodedString) {
  return {
    data: {
      f_port: input.fPort,
      payload_raw: decodedString,
      message: decodedString,
      level: "error"
    },
    errors: ["Error: " + decodedString]
  };
}

function decodeUplink(input) {
  const decodedString = bytesToString(input.bytes || []);
  const variables = parseCsvNumbers(decodedString);

  switch (input.fPort) {
    case 1: { // GPS location
      const latitude = numberOrDefault(variables[0], 0);
      const longitude = numberOrDefault(variables[1], 0);
      const altitude = numberOrDefault(variables[2], 0);
      const hdop = numberOrDefault(variables[3], 0);

      return {
        // Duplicated at top level for integrations such as TTN Mapper / coverage tooling.
        latitude: latitude,
        longitude: longitude,
        altitude: altitude,
        hdop: hdop,
        data: {
          f_port: input.fPort,
          payload_raw: decodedString,
          latitude: latitude,
          longitude: longitude,
          altitude: altitude,
          hdop: hdop
        }
      };
    }

    case 2: { // DS18B20 temperature
      return {
        data: {
          f_port: input.fPort,
          payload_raw: decodedString,
          temperature_c: numberOrDefault(variables[0], 0)
        }
      };
    }

    case 3: { // PH4502C pH level
      return {
        data: {
          f_port: input.fPort,
          payload_raw: decodedString,
          ph_level: numberOrDefault(variables[0], 0)
        }
      };
    }

    case 4: { // Gravity TDS ppm + compensation temperature
      return {
        data: {
          f_port: input.fPort,
          payload_raw: decodedString,
          tds_ppm: numberOrDefault(variables[0], 0),
          temperature_c: numberOrDefault(variables[1], 0)
        }
      };
    }

    case 5: { // Turbidity NTU
      return {
        data: {
          f_port: input.fPort,
          payload_raw: decodedString,
          turbidity_ntu: numberOrDefault(variables[0], 0)
        }
      };
    }

    case 220:
      return {
        data: {
          f_port: input.fPort,
          payload_raw: decodedString,
          message: "Request further downlinks"
        }
      };

    case 221:
      return makeInfoResult(input, decodedString);

    case 222:
      return makeWarningResult(input, decodedString);

    case 223:
      return makeErrorResult(input, decodedString);

    default:
      return {
        data: {
          f_port: input.fPort,
          payload_raw: decodedString,
          message: decodedString,
          level: "error"
        },
        errors: ["Error (fatal): fPort " + input.fPort + " in uplink not used"]
      };
  }
}
