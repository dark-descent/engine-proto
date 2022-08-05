import { AddonEngine, EngineModule } from "./Addon";
import { Renderer } from "./Renderer";

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
			workerThreads: 3,
			renderer: new Renderer(Engine._instance)
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

		const conf = {
			...Engine.defaultConfig,
			...config
		};

		await conf.renderer.initialize();

		const internalEngine = EngineModule.initialize(conf);

		this._instance = new Engine(internalEngine, conf, await this.initWorkers(conf.workerThreads));
		console.log("Engine initialized!", this._instance);
		(window as any).engine = this._instance;
		return this._instance;
	};

	public static get()
	{
		if (!this._instance)
			throw new Error(`Engine is not initialized yet!`);
		return this._instance;
	}

	private readonly _config: Readonly<EngineConfig>;

	private readonly _workers: ReadonlyArray<Worker>;

	/**
	 * @internal
	 */
	public readonly api: Readonly<AddonEngine>;

	private constructor(addonEngine: AddonEngine, config: Required<EngineConfig>, workers: ReadonlyArray<Worker>)
	{
		this.api = addonEngine;

		const expose = global as any;

		Object.keys(addonEngine.objects).forEach(k => 
		{
			expose[k] = (addonEngine.objects as any)[k]!;
		});

		this._config = config;
		this._workers = workers;
	}
}

export type EngineConfig = {
	gameName: string;
	logHandler?: LogCallback;
	workerThreads?: number;
	renderer?: Renderer;
};

export type KeysOfType<T, U> = { [K in keyof T]: T[K] extends U ? K : never }[keyof T];
export type RequiredKeys<T> = Exclude<KeysOfType<T, Exclude<T[keyof T], undefined>>, undefined>;

export type EngineOptionalConfig = Omit<EngineConfig, RequiredKeys<EngineConfig>>;

type LogLevel = "error" | "exception" | "warn" | "info";

type LogCallback = (level: LogLevel, msg: string) => void;