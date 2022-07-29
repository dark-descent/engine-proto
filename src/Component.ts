import type { Entity } from "./Entity";

export abstract class Component
{
	public readonly entity: Entity;

	public constructor(entity: Entity)
	{
		this.entity = entity;
	}
}

export type ComponentType<T extends Component> = new (entity: Entity) => T;