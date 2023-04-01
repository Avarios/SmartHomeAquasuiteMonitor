import { connect } from "mqtt";
import { hostname } from "os";
import dotenv from "dotenv";
import { startFetching } from "./aquasuiteFetcher.js";

dotenv.config();

const mqttUsername = process.env.MQTTUSER;
const mqttPassword = process.env.MQTTPASS;
const mqttServer = process.env.MQTTSERVER;
const mqttTopic = process.env.MQTTTOPIC;
const fetchUrl = process.env.AQUASUITEURL;

const mqttConfig = {
  resubscribe: true,
};

if (mqttUsername) {
  mqttConfig.username = mqttUsername;
}

if (mqttPassword) {
  mqttConfig.password = mqttPassword;
}

console.log(`Starting process connecting ${mqttServer} to topic ${mqttTopic}`);

const client = connect(mqttServer, mqttConfig);

client.on("connect", function () {
  client.publish(
    "$SYS/aquasuite_listener",
    JSON.stringify({
      message: `Hello from ${hostname()}`,
    })
  );
  console.log(`Connected to ${mqttServer}`);
});

client.on("error", (err) => {
  console.error(err);
});

startFetching(fetchUrl, (result, error) => {
  console.log(result);
  if (error) console.error(error);
  client.publish(mqttTopic,JSON.stringify(result));
});
