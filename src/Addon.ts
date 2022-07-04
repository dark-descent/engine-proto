import type { EngineConfig } from "./Engine";

export namespace Addon
{
	export const module: AddonModule = __non_webpack_require__("./addon.node");
}

type AddonModule = {
	initialize: (config: Required<EngineConfig>) => InternalEngine;
};

export type InternalEngine = {

};