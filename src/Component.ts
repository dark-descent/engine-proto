import { ECS } from "./ECS";

export abstract class Component<D>
{
	private static _idCounter: number = 1;

	private static readonly getNextId = () => 
	{
		const id = this._idCounter;
		this._idCounter += this._idCounter;
		return id;
	}

	public readonly id: number = Component.getNextId();
	public readonly ecs: ECS;

	public constructor(ecs: ECS)
	{
		this.ecs = ecs;
	}

	private readonly data: D[] = [];
	private readonly entityIds: number[] = [];

	public addComponent(entity: number, data: D): any
	{
		const dataIndex = this.data.push(data) - 1;
		this.entityIds[dataIndex] = entity;
	}
};

export type ComponentType<T extends Component<any> = Component<any>> = new (ecs: ECS, ...args: any) => T;
export type ComponentData<T> = T extends Component<infer Data> ? [ComponentType<T>, Data] : never;