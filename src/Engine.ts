import { Addon } from "./Addon";
import { AssetManager } from "./AssetManager";
import { ECS } from "./ECS";
import { Game } from "./Game";
import { Renderer } from "./Renderer";
import { SceneManager } from "./SceneManager";
import { SubSystem, SubSystemInitProps, SubSystemType } from "./SubSystem";

export class Engine
{
	public static readonly initialize = async (game: Game, config: Config, subSystemOverrides: Partial<SubSystemOverrides> = {}) =>
	{
		console.log(Addon.module);
		const engine = new Engine(game, subSystemOverrides);
		await engine.initialize(config);
		return engine;
	}

	private readonly _subSystemsTypeMap: Map<SubSystemType<any>, SubSystem<any>> = new Map();
	private readonly _subSystemsNameMap: Map<string, SubSystem<any>> = new Map();

	public readonly game: Game;

	private constructor(game: Game, subSystemOverrides: Partial<SubSystemOverrides> = {}) 
	{
		this.game = game;

		const subSystems = Object.values({
			AssetManager,
			SceneManager,
			Renderer,
			ECS,
			...subSystemOverrides
		});

		subSystems.forEach(s => this.loadSystem(s as SubSystemType<any>));
	}

	public readonly loadSystem = <T extends SubSystem<any>>(type: SubSystemType<T>, name: string = type.name): T =>
	{
		if (this._subSystemsTypeMap.has(type))
			return this._subSystemsTypeMap.get(type) as T;

		const subSystem = new type(this, name);

		this._subSystemsNameMap.set(name, subSystem);
		this._subSystemsTypeMap.set(type, subSystem);

		return this._subSystemsTypeMap.get(type) as T;
	}

	private readonly subSystemGetters = {
		string: (name: string | SubSystemType<any>) =>
		{
			const subSystem = this._subSystemsNameMap.get(name as any);
			if (!subSystem)
				throw new Error(`SubSystem ${name} is not loaded!`);
			return subSystem;
		},
		function: (type: string | SubSystemType<any>) =>
		{
			const subSystem = this._subSystemsTypeMap.get(type as any);
			if (!subSystem)
				throw new Error(`SubSystem ${(type as any).name} is not loaded!`);
			return subSystem;
		}
	};

	public readonly getSubSystem = <T extends SubSystem<any>>(type: SubSystemType<T> | string): T =>
	{
		return this.subSystemGetters[this.getSubSystemArgType(type)]!(type) as T;
	}

	private readonly getSubSystemArgType = (type: SubSystemType<any> | string): keyof Engine["subSystemGetters"] => typeof type as keyof Engine["subSystemGetters"];

	private readonly initSubSystem = async (system: SubSystem<any>, type: SubSystemType<any>, config: Config) =>
	{
		if (!system.isInitialized)
		{
			const ctor = type as any as typeof SubSystem;
			const key = system.name as keyof SubSystems;

			if (!config[key])
				throw new Error(``);
			const deps = ctor.getDependencies(type);
			const promises: Promise<any>[] = [];
			if (deps)
				for (let dep of deps)
				{
					if (Array.isArray(dep))
					{
						const foundDep = dep.find(d => 
						{
							let keys = this._subSystemsTypeMap.keys();
							let res = keys.next();

							while (!res.done)
							{
								if (res.value === d)
									return true;
								res = keys.next();
							}

							return false;
						});

						dep = foundDep as any;
					}

					if(dep)
					{
						const s = this.getSubSystem(dep as any);
						if (s && !s.isInitialized && !s.isInitializing)
							promises.push(this.initSubSystem(s, dep as any, config));
					}
				}

			await Promise.all(promises);
			await system["initialize"](config[key]);
		}
	}

	private readonly initialize = async (config: Config) =>
	{
		const systems = this._subSystemsTypeMap.keys();
		const promises: Promise<any>[] = [];
		for (const type of systems)
			promises.push(this.initSubSystem(this.getSubSystem(type), type, config));
		await Promise.all(promises);
	}
}

type SubSystems = {
	SceneManager: SceneManager;
	AssetManager: AssetManager;
	Renderer: Renderer;
	ECS: ECS;
};

export type SubSystemOverrides = {
	[K in keyof SubSystems]: SubSystemType<SubSystem<any>>;
};

type Config = {
	[K in keyof SubSystems]: SubSystemInitProps<SubSystems[K]>;
};

type SubSystemOverride = {
	[key: string]: SubSystem<any>;
};

type SubSystemConfig<T extends SubSystemOverride> = {
	[K in keyof T]: SubSystemInitProps<T[K]>;
}

export type SubSystemsConfig<T extends SubSystemOverride> = Config & SubSystemConfig<T>;