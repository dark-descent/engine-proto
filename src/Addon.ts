import type { EngineConfig } from "./Engine";

const isWorker = !("window" in self && "document" in self);

export namespace Addon
{
	export const module: AddonModule = __non_webpack_require__(isWorker ? "../addon.node" : "./addon.node");
}

type AddonModule = {
	initialize: (config: Required<EngineConfig>) => AddonEngine;
	initializeWorker: () => EngineWorkerSystem;
};

export type AddonEngine = {
	components: ComponentInfo[];
};

type ComponentInfo = {
	name: string;
	index: number;
};

export type EngineWorkerSystem = {
	
};