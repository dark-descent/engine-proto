import { Addon, AddonEngine } from "./Addon";
import { Renderer } from "./Renderer";
import { CoreComponents } from "./components";
import { Scene } from "./Scene";

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

		const internalEngine = Addon.module.initialize(conf);

		this._instance = new Engine(internalEngine, conf, await this.initWorkers(conf.workerThreads));

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

	private readonly _activeScene: Scene | null = null;

	public get activeScene(): Scene 
	{
		if (this._activeScene == null)
			throw new Error("No scene is loaded!");
		return this._activeScene;
	}

	/**
	 * @internal
	 */
	public readonly _addonEngine: Readonly<AddonEngine>;

	private constructor(addonEngine: AddonEngine, config: Required<EngineConfig>, workers: ReadonlyArray<Worker>)
	{
		this._addonEngine = addonEngine;

		addonEngine.components.forEach((c) =>
		{
			if (CoreComponents[c.name])
				CoreComponents[c.name]._internalIndex = c.index;
			else
				console.warn(`Could not find CoreComponent "${c.name}"!`);
			// console.log(c.name, CoreComponents[c.name]?._internalIndex);
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