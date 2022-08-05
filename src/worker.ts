import { EngineModule } from "./Addon";

self.onmessage = ({ data }) =>
{
	console.log("worker got msg with data:", data);
};

const engine = EngineModule.initializeWorker();

self.postMessage({ msg: "initialized", data: { hihi: ":D", wop: 123 } });