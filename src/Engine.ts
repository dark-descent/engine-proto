import { AssetManager } from "./AssetManager";
import { Guards } from "./Guards";
import { SceneManager } from "./SceneManager";
import { SubSystem, SubSystemInitProps, SubSystemType } from "./SubSystem";

export class Engine
{
	private static _isInitialized: boolean = false;

	private static readonly _instance: Engine = new Engine();

	public static readonly initialize = (config: Config) =>
	{
		if (this._isInitialized)
			throw new Error("Engine is already initialized!");
		this._instance.initialize(config);
	}

	private readonly _subSystemsTypeMap: Map<SubSystemType<any>, SubSystem<any>> = new Map();
	private readonly _subSystemsNameMap: Map<string, SubSystem<any>> = new Map();

	private constructor() 
	{
		const subSystems: DefaultSubSystems = [
			AssetManager,
			SceneManager
		];

		subSystems.forEach(s => this.loadSystem(s));
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
			let promises: Promise<any>[] = [];
			if (deps)
				for (const dep of deps)
				{
					const s = this.getSubSystem(dep);
					if (s && !s.isInitialized && !s._isInitializing)
						promises.push(this.initSubSystem(s, dep, config));
				}
			
			await Promise.all(promises);
			await system["initialize"](config[key]);
		}
	}

	private readonly initialize = async (config: Config) =>
	{
		const systems = this._subSystemsTypeMap.keys();
		for(const type of systems)
			await this.initSubSystem(this.getSubSystem(type), type, config);
	}
}

type SubSystems = {
	SceneManager: SceneManager;
	AssetManager: AssetManager;
};

type DefaultSubSystemTypes = {
	[K in keyof SubSystems]: SubSystemType<SubSystems[K]>
};

type DefaultSubSystems = DefaultSubSystemTypes[keyof DefaultSubSystemTypes][];

type Config = {
	[K in keyof SubSystems]: SubSystemInitProps<SubSystems[K]>;
};