import { Addon, EngineSystems } from "./Addon";

export class Engine
{
	private static _instance: Engine;

	private static readonly defaultLogHandler: LogCallback = (level, msg) =>
	{
		const target = level === "exception" ? "error" : level;
		console[target](`[INTERNAL] ${msg}`);
	}

	private static get defaultConfig(): Required<EngineOptionalConfig>
	{
		return {
			logHandler: Engine.defaultLogHandler,
			workerThreads: 3
		};
	};

	private static readonly initWorkers = async (count: number) =>
	{
		const workers: Worker[] = [];
		const promises: Promise<void>[] = [];

		for (let i = 0; i < count; i++)
		{
			const worker = new Worker(new URL('./worker.ts', import.meta.url));
			workers.push(worker);
			promises.push(new Promise<void>((res) =>
			{
				const initListener = (e: MessageEvent<any>) => 
				{
					if (e.data.msg === "initialized")
					{
						worker.removeEventListener("message", initListener);
						res();
					}
				}

				worker.addEventListener("message", initListener);
			}));
		}

		await Promise.all(promises);

		return workers;
	}

	public static readonly initialize = async (config: EngineConfig) => 
	{
		if (this._instance)
			throw new Error(`Engine is already initialized!`);

		const conf = { ...Engine.defaultConfig, ...config };

		const internalEngine = Addon.module.initialize(conf);

		return new Engine(internalEngine, conf, await this.initWorkers(conf.workerThreads));
	};

	public static get()
	{
		if (this._instance)
			throw new Error(`Engine is already initialized!`);
	}

	private readonly _config: Readonly<EngineConfig>;

	private readonly _workers: ReadonlyArray<Worker>;

	public readonly systems: Readonly<EngineSystems>;

	private constructor(systems: EngineSystems, config: Required<EngineConfig>, workers: ReadonlyArray<Worker>)
	{
		this.systems = systems;
		this._config = config;
		this._workers = workers;
	}
}

export type EngineConfig = {
	gameName: string;
	logHandler?: LogCallback;
	workerThreads?: number;
};

export type KeysOfType<T, U> = { [K in keyof T]: T[K] extends U ? K : never }[keyof T];
export type RequiredKeys<T> = Exclude<KeysOfType<T, Exclude<T[keyof T], undefined>>, undefined>;

export type EngineOptionalConfig = Omit<EngineConfig, RequiredKeys<EngineConfig>>;

type LogLevel = "error" | "exception" | "warn" | "info";

type LogCallback = (level: LogLevel, msg: string) => void;