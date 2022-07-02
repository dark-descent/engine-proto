import { Addon, InternalEngine } from "./Addon";

export class Engine
{
	private static _instance: Engine;

	public static readonly initialize = (config: EngineConfig) => 
	{
		if (this._instance)
			throw new Error(`Engine is already initialized!`);

		const internalEngine = Addon.module.initialize(config);

		return new Engine(internalEngine, config);
	};

	public static get()
	{
		if (this._instance)
		throw new Error(`Engine is already initialized!`);
	}

	private readonly _internalEngine: Readonly<InternalEngine>;
	private readonly _config: Readonly<EngineConfig>;

	private constructor(internalEngine: InternalEngine, config: EngineConfig)
	{
		this._internalEngine = internalEngine;
		this._config = config;
	}
}

export type EngineConfig = {

};