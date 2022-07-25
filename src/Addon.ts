import type { EngineConfig } from "./Engine";
import { IRenderer } from "./System";

const isWorker = !("window" in self && "document" in self);

export namespace Addon
{
	export const module: AddonModule = __non_webpack_require__(isWorker ? "../addon.node" : "./addon.node");
}

type AddonModule = {
	initialize: (config: Required<EngineConfig>) => EngineSystems;
	initializeWorker: () => EngineWorkerSystem;
};

export type EngineSystems = {
	Renderer: IRenderer;
};

export type EngineWorkerSystem = {
	
};