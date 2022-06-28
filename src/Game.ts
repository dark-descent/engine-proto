import { Engine, SubSystemOverrides, SubSystemsConfig } from "./Engine";

export abstract class Game
{
	public readonly name: string;

	private _engine: Engine | null = null;

	public get engine(): Engine
	{
		if (!this._engine)
			throw new Error(`Engine is not initialized yet!`);
		return this._engine;
	}

	public constructor(name: string)
	{
		this.name = name;
	}

	protected abstract get subSystemsConfig(): SubSystemsConfig<any>;

	public get subSystemsOverrides(): Partial<SubSystemOverrides> { return {}; }

	public async initialize()
	{
		if(this._engine)
			throw new Error(`The game is already initialized!`);
		
		this._engine = await Engine.initialize(this, this.subSystemsConfig, this.subSystemsOverrides);
	}
}