import { Addon, InternalEngine } from "./Addon";

export class Engine
{
	private static _instance: Engine;

	private static readonly defaultLogHandler: LogCallback = (level, msg) =>
	{
		const target = level === "exception" ? "error" : level;
		console[target](`[INTERNAL] ${msg}`);
	}

	private static get defaultConfig(): Required<EngineConfig>
	{
		return {
			logHandler: Engine.defaultLogHandler
		};
	};

	public static readonly initialize = (config: EngineConfig) => 
	{
		if (this._instance)
			throw new Error(`Engine is already initialized!`);

		const conf = { ...Engine.defaultConfig, ...config };

		const internalEngine = Addon.module.initialize(conf);

		return new Engine(internalEngine, conf);
	};

	public static get()
	{
		if (this._instance)
			throw new Error(`Engine is already initialized!`);
	}

	private readonly _internalEngine: Readonly<InternalEngine>;
	private readonly _config: Readonly<EngineConfig>;

	private constructor(internalEngine: InternalEngine, config: Required<EngineConfig>)
	{
		this._internalEngine = internalEngine;
		this._config = config;
	}
}

export type EngineConfig = {
	logHandler?: LogCallback;
};

type LogLevel = "error" | "exception" | "warn" | "info";

type LogCallback = (level: LogLevel, msg: string) => void;