import { Addon, EngineSystems } from "./Addon";

self.onmessage = ({ data }) =>
{
	console.log("worker got msg with data:", data);
};

const engine = Addon.module.initializeWorker();

console.log(engine);

self.postMessage({ msg: "initialized", data: { hihi: ":D", wop: 123 } });