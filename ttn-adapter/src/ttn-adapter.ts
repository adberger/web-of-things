/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.*
 */

import { Adapter, Device, Property } from 'gateway-addon';

import { connect } from 'mqtt'

// TODO remove
interface Payload {
  app_id: string, // TODO remove
  dev_id: string, // TODO remove
  end_device_ids: {
    device_id: string,
    application_ids: {
      application_id: string;
    }
  },
  hardware_serial: string,
  port: number,
  counter: number,
  confirmed: boolean,
  payload_raw: Buffer,
  uplink_message: {
    decoded_payload: object
  },
  metadata:
  {
    time: string,
    frequency: number,
    modulation: string,
    data_rate: string,
    airtime: number,
    coding_rate: string,
    gateways: Gateway[]
  }
}

interface Gateway {
  gtw_id: string,
  timestamp: number,
  time: string,
  channel: number,
  rssi: number,
  snr: number,
  rf_chain: number
}

class Ttn extends Device {
  constructor(adapter: any, id: string, payload: Payload) {
    super(adapter, id);
    this['@context'] = 'https://iot.mozilla.org/schemas/';
    this['@type'] = [];
    this.name = id;

    for (const [propertyName, value] of Object.entries(payload.uplink_message.decoded_payload)) {
      const type = typeof value;

      if (this.typeIsAllowed(type)) {
        console.log(`Creating property ${propertyName} with type ${type}`);
        const additionalProperties: any = {};

        if (propertyName === 'temperature') {
          additionalProperties['@type'] = 'TemperatureProperty';
          additionalProperties['unit'] = 'degree celsius';
          this['@type'].push('TemperatureSensor');
        }

        if (propertyName === 'humidity') {
          additionalProperties['@type'] = 'HumidityProperty';
          additionalProperties['unit'] = '%';
          additionalProperties['minimum'] = 0;
          additionalProperties['maximum'] = 100;
          this['@type'].push('HumiditySensor');
        }

        if (propertyName === 'pressure') {
          additionalProperties['@type'] = 'BarometricPressureProperty';
          additionalProperties['unit'] = 'hectopascal';
          additionalProperties['minimum'] = '0';
          additionalProperties['maximum'] = '1085';
          this['@type'].push('BarometricPressureSensor');
        }

        const airQualityProperties = ['pm25', 'pm2.5', 'pm10']

        if (airQualityProperties.indexOf(propertyName) > -1) {
          additionalProperties['@type'] = 'DensityProperty';
          additionalProperties['unit'] = 'micrograms per cubic metre';
          additionalProperties['minimum'] = '0';
          this['@type'].push('AirQualitySensor');
        }

        this.addProperty(propertyName, {
          ...additionalProperties,
          type,
          multipleOf: 0.1,
          title: propertyName,
          readOnly: true,
        });
      } else {
        console.log(`Ignoring property ${propertyName} because it's of type ${type}`);
      }
    }
  }

  public update(payload: Payload) {
    for (const [propertyName, value] of Object.entries(payload.uplink_message.decoded_payload)) {
      const type = typeof value;

      if (this.typeIsAllowed(type)) {
        const property = this.properties.get(propertyName);

        if (property) {
          property.setCachedValueAndNotify(value);
        } else {
          console.warn(`Could not find property ${propertyName}`);
        }
      }
    }
  }

  private typeIsAllowed(type: string) {
    switch (type) {
      case 'string':
      case 'number':
      case 'boolean':
        return true;
    }

    return false;
  }

  private addProperty(name: string, description: any): Property {
    const property = new Property(this, name, description);
    this.properties.set(name, property);
    return property;
  }
}

export class TtnAdapter extends Adapter {
  private devicesById: { [id: string]: Ttn } = {};

  constructor(addonManager: any, private manifest: any) {
    super(addonManager, TtnAdapter.name, manifest.name);
    addonManager.addAdapter(this);

    for (const application of this.manifest.moziot.config.applications || []) {
      const {
        appID,
        accessKey,
        region
      } = application;

      this.startListening(appID, accessKey, region || 'eu');
    }
  }

  private async startListening(appID: string, accessKey: string, region: string) {
    const host = `${region}.cloud.thethings.network`;

    console.log(`Connecting to ${host}`);

    const mqtt = connect({
      host,
      port: 1883,
      username: appID,
      password: accessKey,
    });

    mqtt.on("error", (error) => console.error("Could not connect to mqtt server ", error));
    mqtt.on("connect", () => console.log("Successfully connected to mqtt server"));
    mqtt.on("message", (_, message: Buffer) => {
      console.log('message', message.toString())
      const payload: Payload = JSON.parse(message.toString());

      if (payload.uplink_message.decoded_payload) {
        this.onPayload(payload);
      }
    });

    mqtt.subscribe('v3/+/devices/+/up');
  }

  private onPayload(payload: Payload) {
    console.log('onPayload');

    const id = `${payload.end_device_ids.application_ids.application_id}-${payload.end_device_ids.device_id}`;
    let device = this.devicesById[id];

    if (!device) {
      console.log(`Creating new device for ${id}`);
      device = new Ttn(this, id, payload);
      this.devicesById[id] = device;
      this.handleDeviceAdded(device);
    }

    device.update(payload);
  }
}
