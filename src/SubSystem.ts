import { Engine } from "./Engine";

export abstract class SubSystem<P extends {}>
{
	public static readonly dependsOn = (...deps: SubSystemType<any>[]) =>
	{
		return (_ctor: Function) => 
		{
			const ctor = _ctor as typeof SubSystem;
			ctor._dependencies.set(ctor as any, deps);
		}
	}

	private static readonly _dependencies: Map<SubSystemType<any>, SubSystemType<any>[]> = new Map();

	public static readonly getDependencies = (type: SubSystemType<any>) => this._dependencies.get(type);
	
	private _isInitialized: boolean = false;

	public get isInitialized() { return this._isInitialized; }

	private _isInitializing: boolean = false;

	public get isInitializing() { return this._isInitializing; }

	public readonly name: string;
	public readonly engine: Engine;

	public constructor(engine: Engine, name?: string)
	{
		this.name = name || this.constructor.name;
		this.engine = engine;
	}

	private readonly initialize = async (props: P) => 
	{
		if (!this._isInitialized && !this._isInitializing)
		{
			if(env.isDev)
				console.group(`Initializing ${this.name}...`);
			this._isInitializing = true;
			await this.init(props);
			this._isInitialized = true;
			if(env.isDev)
			{
				console.log(`${this.name} Initialized!`);
				console.groupEnd();
			}
		}
	}

	public abstract init(props: P): Promise<void> | void;
}

export type SubSystemType<T extends SubSystem<any>> = new (engine: Engine, name?: string) => T;

export type SubSystemInitProps<T> = T extends SubSystem<infer P> ? P : never;